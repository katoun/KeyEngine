/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MainWindow.h"
#include "Utils.h"
#include "IconManager.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

namespace
{
	namespace fs = std::filesystem;

	VkAllocationCallbacks* g_Allocator = nullptr;
	VkInstance g_Instance = VK_NULL_HANDLE;
	VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice g_Device = VK_NULL_HANDLE;
	uint32_t g_QueueFamily = UINT32_MAX;
	VkQueue g_Queue = VK_NULL_HANDLE;
	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
	ImGui_ImplVulkanH_Window g_MainWindowData;
	int g_MinImageCount = 2;
	bool g_SwapChainRebuild = false;

	void CheckVkResult(VkResult err)
	{
		if (err == VK_SUCCESS)
			return;

		std::cerr << "Vulkan error: " << err << std::endl;
		std::abort();
	}


	void SetupVulkan(const char* const* extensions, uint32_t extensions_count)
	{
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "KeyEngine";
		app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		app_info.pEngineName = "KeyEngine";
		app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;

		CheckVkResult(vkCreateInstance(&create_info, g_Allocator, &g_Instance));
	}

	void SetupVulkanDevice(VkSurfaceKHR surface)
	{
		uint32_t gpu_count = 0;
		CheckVkResult(vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr));
		if (gpu_count == 0)
			throw std::runtime_error("No Vulkan physical devices found.");

		std::vector<VkPhysicalDevice> gpus(gpu_count);
		CheckVkResult(vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data()));

		for (VkPhysicalDevice gpu : gpus)
		{
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(gpu, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				g_PhysicalDevice = gpu;
				break;
			}
		}
		if (g_PhysicalDevice == VK_NULL_HANDLE)
			g_PhysicalDevice = gpus[0];

		uint32_t queue_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queue_count, nullptr);
		std::vector<VkQueueFamilyProperties> queues(queue_count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queue_count, queues.data());

		for (uint32_t i = 0; i < queue_count; ++i)
		{
			VkBool32 present_support = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, i, surface, &present_support);
			if ((queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support)
			{
				g_QueueFamily = i;
				break;
			}
		}
		if (g_QueueFamily == UINT32_MAX)
			throw std::runtime_error("No Vulkan graphics queue with present support found.");

		const float queue_priority = 1.0f;
		VkDeviceQueueCreateInfo queue_info{};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = g_QueueFamily;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queue_priority;

		const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkDeviceCreateInfo device_info{};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.enabledExtensionCount = 1;
		device_info.ppEnabledExtensionNames = device_extensions;

		CheckVkResult(vkCreateDevice(g_PhysicalDevice, &device_info, g_Allocator, &g_Device));
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
	}

	void SetupVulkanDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		};

		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;

		CheckVkResult(vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool));
	}

	void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
	{
		wd->Surface = surface;
		VkFormat surface_formats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM };
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
			g_PhysicalDevice,
			wd->Surface,
			surface_formats,
			static_cast<int>(std::size(surface_formats)),
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
			g_PhysicalDevice,
			wd->Surface,
			present_modes,
			static_cast<int>(std::size(present_modes)));

		wd->ClearValue.color.float32[0] = 0.06f;
		wd->ClearValue.color.float32[1] = 0.07f;
		wd->ClearValue.color.float32[2] = 0.08f;
		wd->ClearValue.color.float32[3] = 1.0f;

		ImGui_ImplVulkanH_CreateOrResizeWindow(
			g_Instance,
			g_PhysicalDevice,
			g_Device,
			wd,
			g_QueueFamily,
			g_Allocator,
			width,
			height,
			g_MinImageCount,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	}

	void CleanupVulkanWindow()
	{
		ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
	}

	void CleanupVulkan()
	{
		vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
		vkDestroyDevice(g_Device, g_Allocator);
		vkDestroyInstance(g_Instance, g_Allocator);
	}

	void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
	{
		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

		VkResult err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			g_SwapChainRebuild = true;
			return;
		}
		CheckVkResult(err);

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		CheckVkResult(vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));
		CheckVkResult(vkResetFences(g_Device, 1, &fd->Fence));
		CheckVkResult(vkResetCommandPool(g_Device, fd->CommandPool, 0));

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CheckVkResult(vkBeginCommandBuffer(fd->CommandBuffer, &begin_info));

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = wd->RenderPass;
		render_pass_info.framebuffer = fd->Framebuffer;
		render_pass_info.renderArea.extent.width = wd->Width;
		render_pass_info.renderArea.extent.height = wd->Height;
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		vkCmdEndRenderPass(fd->CommandBuffer);
		CheckVkResult(vkEndCommandBuffer(fd->CommandBuffer));

		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &image_acquired_semaphore;
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &fd->CommandBuffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_complete_semaphore;

		CheckVkResult(vkQueueSubmit(g_Queue, 1, &submit_info, fd->Fence));
	}

	void FramePresent(ImGui_ImplVulkanH_Window* wd)
	{
		if (g_SwapChainRebuild)
			return;

		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &render_complete_semaphore;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &wd->Swapchain;
		present_info.pImageIndices = &wd->FrameIndex;

		VkResult err = vkQueuePresentKHR(g_Queue, &present_info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
			g_SwapChainRebuild = true;
		else
			CheckVkResult(err);

		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
	}


	void ApplyStyle()
	{
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 4.0f;
		style.FrameRounding = 3.0f;
		style.GrabRounding = 3.0f;
		style.TabRounding = 3.0f;
		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.085f, 0.095f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.18f, 0.20f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.24f, 0.27f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.31f, 0.35f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.38f, 0.43f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.13f, 0.14f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.33f, 0.38f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.24f, 0.27f, 1.0f);
	}
}

int main(int argc, char* argv[])
{
	fs::path open_project;
	for (int i = 1; i < argc; ++i)
	{
		std::string_view argument = argv[i];
		if ((argument == "-o" || argument == "--open-project") && i + 1 < argc)
			open_project = argv[++i];
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow(
		"KeyEditor2",
		1600,
		900,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	uint32_t extensions_count = 0;
	const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensions_count);
	if (extensions == nullptr)
	{
		std::cerr << "SDL_Vulkan_GetInstanceExtensions failed: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SetupVulkan(extensions, extensions_count);

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (!SDL_Vulkan_CreateSurface(window, g_Instance, g_Allocator, &surface))
	{
		std::cerr << "SDL_Vulkan_CreateSurface failed: " << SDL_GetError() << std::endl;
		CleanupVulkan();
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SetupVulkanDevice(surface);
	SetupVulkanDescriptorPool();

	int width = 0;
	int height = 0;
	SDL_GetWindowSizeInPixels(window, &width, &height);
	SetupVulkanWindow(&g_MainWindowData, surface, width, height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	fs::path editor_config_path = editor::EditorConfigPath();
	std::error_code config_error;
	fs::create_directories(editor_config_path, config_error);
	fs::path editor_layout_path = editor_config_path / "imgui.ini";
	const bool has_saved_layout = fs::exists(editor_layout_path);
	std::string editor_layout_path_string = editor_layout_path.string();
	io.IniFilename = editor_layout_path_string.c_str();

	ApplyStyle();

	ImGui_ImplSDL3_InitForVulkan(window);
	ImGui_ImplVulkan_InitInfo init_info{};
	init_info.Instance = g_Instance;
	init_info.PhysicalDevice = g_PhysicalDevice;
	init_info.Device = g_Device;
	init_info.QueueFamily = g_QueueFamily;
	init_info.Queue = g_Queue;
	init_info.PipelineCache = g_PipelineCache;
	init_info.DescriptorPool = g_DescriptorPool;
	init_info.PipelineInfoMain.RenderPass = g_MainWindowData.RenderPass;
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.MinImageCount = g_MinImageCount;
	init_info.ImageCount = g_MainWindowData.ImageCount;
	init_info.Allocator = g_Allocator;
	init_info.CheckVkResultFn = CheckVkResult;
	ImGui_ImplVulkan_Init(&init_info);
	editor::IconManager::Instance().Initialize(g_PhysicalDevice, g_Device, g_QueueFamily, g_Queue);

	editor::MainWindow app(window);
	app.seed_default_layout = !has_saved_layout;
	if (!open_project.empty())
		app.OpenProject(open_project);

	bool done = false;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
				done = true;
			app.HandleEvent(event);
		}

		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(10);
			continue;
		}

		SDL_GetWindowSizeInPixels(window, &width, &height);
		if (g_SwapChainRebuild && width > 0 && height > 0)
		{
			ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
			ImGui_ImplVulkanH_CreateOrResizeWindow(
				g_Instance,
				g_PhysicalDevice,
				g_Device,
				&g_MainWindowData,
				g_QueueFamily,
				g_Allocator,
				width,
				height,
				g_MinImageCount,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
			g_MainWindowData.FrameIndex = 0;
			g_SwapChainRebuild = false;
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		app.Draw();

		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool minimized = draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f;
		if (!minimized)
			FrameRender(&g_MainWindowData, draw_data);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		if (!minimized)
			FramePresent(&g_MainWindowData);
	}

	CheckVkResult(vkDeviceWaitIdle(g_Device));
	editor::IconManager::Instance().Shutdown();
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	CleanupVulkanWindow();
	CleanupVulkan();
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

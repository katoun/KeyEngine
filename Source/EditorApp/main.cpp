/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "MainWindow.h"
#include "Utils.h"
#include "IconManager.h"
#include "LayoutConstants.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
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

	SDL_HitTestResult SDLCALL WindowHitTest(SDL_Window* window, const SDL_Point* area, void*)
	{
		constexpr int resize_border = 6;
		constexpr int header_height = static_cast<int>(editor::layout::AppHeaderHeight);
		constexpr int left_interactive_width = 560;
		constexpr int right_controls_width = 146;

		int width = 0;
		int height = 0;
		SDL_GetWindowSize(window, &width, &height);

		const bool left = area->x < resize_border;
		const bool right = area->x >= width - resize_border;
		const bool top = area->y < resize_border;
		const bool bottom = area->y >= height - resize_border;

		if (top && left)
			return SDL_HITTEST_RESIZE_TOPLEFT;
		if (top && right)
			return SDL_HITTEST_RESIZE_TOPRIGHT;
		if (bottom && left)
			return SDL_HITTEST_RESIZE_BOTTOMLEFT;
		if (bottom && right)
			return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
		if (top)
			return SDL_HITTEST_RESIZE_TOP;
		if (bottom)
			return SDL_HITTEST_RESIZE_BOTTOM;
		if (left)
			return SDL_HITTEST_RESIZE_LEFT;
		if (right)
			return SDL_HITTEST_RESIZE_RIGHT;

		if (area->y < header_height
			&& area->x > left_interactive_width
			&& area->x < width - right_controls_width)
		{
			return SDL_HITTEST_DRAGGABLE;
		}

		return SDL_HITTEST_NORMAL;
	}

	void SetWindowIcon(SDL_Window* window)
	{
		const fs::path icon_path = editor::SdkPath() / "Source" / "EditorApp" / "Resources" / "KeyEngine.png";
		SDL_Surface* icon = SDL_LoadPNG(icon_path.string().c_str());
		if (icon == nullptr)
		{
			std::cerr << "Unable to load window icon: " << icon_path << " " << SDL_GetError() << std::endl;
			return;
		}

		SDL_SetWindowIcon(window, icon);
		SDL_DestroySurface(icon);
	}

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
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigDockingTransparentPayload = true;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 2.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 2.0f;
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		style.WindowPadding = ImVec2(6.0f, 6.0f);
		style.FramePadding = ImVec2(8.0f, 3.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.ItemSpacing = ImVec2(4.0f, 3.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 15.0f;
		style.ScrollbarSize = 16.0f;
		style.GrabMinSize = 10.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;

		ImVec4* colors = style.Colors;
		const ImVec4 background_darkest = ImColor(48, 48, 48, 255);
		const ImVec4 background_dark = ImColor(56, 56, 56, 255);
		const ImVec4 background_medium = ImColor(62, 62, 62, 255);
		const ImVec4 background_light = ImColor(72, 72, 72, 255);
		const ImVec4 background_popup = ImColor(52, 52, 52, 255);
		const ImVec4 primary_accent = ImColor(58, 121, 187, 255);
		const ImVec4 primary_accent_hover = ImColor(78, 141, 207, 255);
		const ImVec4 primary_accent_strong = ImColor(58, 121, 187, 180);
		const ImVec4 primary_accent_muted = ImColor(58, 121, 187, 60);
		const ImVec4 text_primary = ImColor(220, 220, 220, 255);
		const ImVec4 text_muted = ImColor(128, 128, 128, 255);
		const ImVec4 border_dark = ImColor(35, 35, 35, 255);
		const ImVec4 border_medium = ImColor(80, 80, 80, 255);

		colors[ImGuiCol_WindowBg] = background_dark;
		colors[ImGuiCol_ChildBg] = background_medium;
		colors[ImGuiCol_PopupBg] = background_popup;
		colors[ImGuiCol_Border] = border_dark;
		colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_MenuBarBg] = background_darkest;
		colors[ImGuiCol_Text] = text_primary;
		colors[ImGuiCol_TextDisabled] = text_muted;
		colors[ImGuiCol_TextSelectedBg] = primary_accent_strong;
		colors[ImGuiCol_Button] = ImVec4(0.28f, 0.28f, 0.28f, 0.45f);
		colors[ImGuiCol_ButtonHovered] = primary_accent_muted;
		colors[ImGuiCol_ButtonActive] = primary_accent_strong;
		colors[ImGuiCol_Header] = background_light;
		colors[ImGuiCol_HeaderHovered] = primary_accent_muted;
		colors[ImGuiCol_HeaderActive] = primary_accent_strong;
		colors[ImGuiCol_FrameBg] = ImColor(35, 35, 35, 255);
		colors[ImGuiCol_FrameBgHovered] = ImColor(45, 45, 45, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(55, 55, 55, 255);
		colors[ImGuiCol_TitleBg] = background_darkest;
		colors[ImGuiCol_TitleBgActive] = background_dark;
		colors[ImGuiCol_TitleBgCollapsed] = background_darkest;
		colors[ImGuiCol_Tab] = background_dark;
		colors[ImGuiCol_TabHovered] = primary_accent_muted;
		colors[ImGuiCol_TabSelected] = background_light;
		colors[ImGuiCol_TabSelectedOverline] = primary_accent;
		colors[ImGuiCol_TabDimmed] = background_darkest;
		colors[ImGuiCol_TabDimmedSelected] = background_light;
		colors[ImGuiCol_ScrollbarBg] = background_dark;
		colors[ImGuiCol_ScrollbarGrab] = background_light;
		colors[ImGuiCol_ScrollbarGrabHovered] = border_medium;
		colors[ImGuiCol_ScrollbarGrabActive] = primary_accent;
		colors[ImGuiCol_CheckMark] = primary_accent;
		colors[ImGuiCol_SliderGrab] = primary_accent;
		colors[ImGuiCol_SliderGrabActive] = primary_accent_hover;
		colors[ImGuiCol_Separator] = border_dark;
		colors[ImGuiCol_SeparatorHovered] = border_medium;
		colors[ImGuiCol_SeparatorActive] = primary_accent;
		colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_ResizeGripHovered] = primary_accent_muted;
		colors[ImGuiCol_ResizeGripActive] = primary_accent;
		colors[ImGuiCol_TableHeaderBg] = background_light;
		colors[ImGuiCol_TableBorderStrong] = border_dark;
		colors[ImGuiCol_TableBorderLight] = border_medium;
		colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.01f);
		colors[ImGuiCol_DockingPreview] = primary_accent_strong;
		colors[ImGuiCol_DockingEmptyBg] = background_darkest;
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.65f);
	}

	void LoadEditorFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig default_config;
		default_config.SizePixels = 13.0f;
		io.Fonts->AddFontDefault(&default_config);

		const fs::path icon_font_path = editor::SdkPath() / "Source" / "EditorApp" / "Resources" / "Fonts" / "fontawesome-webfont.ttf";
		if (!fs::exists(icon_font_path))
			return;

		static constexpr ImWchar icon_ranges[] = { 0xf000, 0xf2ff, 0 };
		ImFontConfig config;
		config.MergeMode = true;
		config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF(
			icon_font_path.string().c_str(),
			14.0f,
			&config,
			icon_ranges);
	}
}

int main(int argc, char* argv[])
{
	fs::path open_project;
	bool debug_open_file_menu = false;
	for (int i = 1; i < argc; ++i)
	{
		std::string_view argument = argv[i];
		if ((argument == "-o" || argument == "--open-project") && i + 1 < argc)
			open_project = argv[++i];
		else if (argument == "--debug" && i + 1 < argc)
		{
			while (i + 1 < argc)
			{
				std::string_view debug_command = argv[i + 1];
				if (!debug_command.empty() && debug_command.front() == '-')
					break;

				++i;
				if (debug_command == "open-file-menu")
					debug_open_file_menu = true;
			}
		}
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow(
		"KeyEditor",
		1600,
		900,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS);
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}
	if (!SDL_SetWindowHitTest(window, WindowHitTest, nullptr))
		std::cerr << "SDL_SetWindowHitTest failed: " << SDL_GetError() << std::endl;
	SetWindowIcon(window);

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
	LoadEditorFonts();

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
	if (debug_open_file_menu)
		app.DebugOpenFileMenu();

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

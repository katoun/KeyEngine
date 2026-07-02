/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IconManager.h"
#include "Utils.h"

#include <imgui_impl_vulkan.h>

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace editor
{
	namespace
	{
		std::string ToLower(std::string value)
		{
			std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
				return static_cast<char>(std::tolower(c));
			});
			return value;
		}
	}

	IconManager& IconManager::Instance()
	{
		static IconManager manager;
		return manager;
	}

	void IconManager::Initialize(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family, VkQueue queue)
	{
		m_PhysicalDevice = physical_device;
		m_Device = device;
		m_QueueFamily = queue_family;
		m_Queue = queue;
	}

	void IconManager::Shutdown()
	{
		for (auto& pair : m_Textures)
		{
			Texture& texture = pair.second;
			if (texture.descriptor_set != VK_NULL_HANDLE)
				ImGui_ImplVulkan_RemoveTexture(texture.descriptor_set);
			if (texture.image_view != VK_NULL_HANDLE)
				vkDestroyImageView(m_Device, texture.image_view, nullptr);
			if (texture.image != VK_NULL_HANDLE)
				vkDestroyImage(m_Device, texture.image, nullptr);
			if (texture.memory != VK_NULL_HANDLE)
				vkFreeMemory(m_Device, texture.memory, nullptr);
		}
		m_Textures.clear();
	}

	Icon IconManager::GetIcon(const std::filesystem::path& path)
	{
		Texture* texture = LoadTexture(path);
		if (texture == nullptr)
			return {};

		return { ImTextureRef(static_cast<ImTextureID>(reinterpret_cast<std::uintptr_t>(texture->descriptor_set))), ImVec2(static_cast<float>(texture->width), static_cast<float>(texture->height)) };
	}

	Icon IconManager::GetEditorIcon(const std::string& name)
	{
		return GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / (name + ".png"));
	}

	Icon IconManager::GetContentIcon(const std::string& name)
	{
		return GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / "content" / (name + ".png"));
	}

	Icon IconManager::GetOutlinerIcon(const std::string& name)
	{
		Icon icon = GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / "outliner" / (name + ".png"));
		if (!icon.IsValid())
			icon = GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / "outliner" / "gameobject.png");
		return icon;
	}

	Icon IconManager::GetComponentIcon(const std::string& name, const std::filesystem::path& project_path)
	{
		const std::string icon_name = ToLower(name);

		if (!project_path.empty())
		{
			Icon project_icon = GetIcon(project_path / "Content" / "Icons" / "Components" / (icon_name + ".png"));
			if (project_icon.IsValid())
				return project_icon;
		}

		Icon editor_icon = GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / "components" / (icon_name + ".png"));
		if (editor_icon.IsValid())
			return editor_icon;

		return GetIcon(SdkPath() / "Source" / "EditorApp" / "Resources" / "Icons" / "components" / "default.png");
	}

	void IconManager::DrawIcon(const Icon& icon, float size) const
	{
		if (!icon.IsValid())
			return;

		ImGui::Image(icon.texture, ImVec2(size, size));
	}

	bool IconManager::ImageButton(const char* id, const Icon& icon, float size) const
	{
		if (!icon.IsValid())
			return ImGui::Button(id, ImVec2(size, size));

		return ImGui::ImageButton(id, icon.texture, ImVec2(size, size));
	}

	IconManager::Texture* IconManager::LoadTexture(const std::filesystem::path& path)
	{
		if (m_Device == VK_NULL_HANDLE || !std::filesystem::exists(path))
			return nullptr;

		const std::string key = std::filesystem::weakly_canonical(path).string();
		auto cached = m_Textures.find(key);
		if (cached != m_Textures.end())
			return &cached->second;

		SDL_Surface* loaded_surface = SDL_LoadPNG(path.string().c_str());
		if (loaded_surface == nullptr)
		{
			std::cerr << "Unable to load icon: " << path << " " << SDL_GetError() << std::endl;
			return nullptr;
		}

		SDL_Surface* surface = SDL_ConvertSurface(loaded_surface, SDL_PIXELFORMAT_RGBA32);
		SDL_DestroySurface(loaded_surface);
		if (surface == nullptr)
			return nullptr;

		const VkDeviceSize image_size = static_cast<VkDeviceSize>(surface->w) * static_cast<VkDeviceSize>(surface->h) * 4;

		VkBuffer staging_buffer = VK_NULL_HANDLE;
		VkDeviceMemory staging_memory = VK_NULL_HANDLE;
		CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_memory);

		void* mapped_memory = nullptr;
		vkMapMemory(m_Device, staging_memory, 0, image_size, 0, &mapped_memory);
		std::memcpy(mapped_memory, surface->pixels, static_cast<size_t>(image_size));
		vkUnmapMemory(m_Device, staging_memory);

		Texture texture;
		texture.width = surface->w;
		texture.height = surface->h;
		SDL_DestroySurface(surface);

		VkImageCreateInfo image_info{};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = static_cast<uint32_t>(texture.width);
		image_info.extent.height = static_cast<uint32_t>(texture.height);
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateImage(m_Device, &image_info, nullptr, &texture.image);

		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(m_Device, texture.image, &memory_requirements);
		VkMemoryAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkAllocateMemory(m_Device, &allocate_info, nullptr, &texture.memory);
		vkBindImageMemory(m_Device, texture.image, texture.memory, 0);

		VkCommandPool command_pool = VK_NULL_HANDLE;
		VkCommandPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		pool_info.queueFamilyIndex = m_QueueFamily;
		vkCreateCommandPool(m_Device, &pool_info, nullptr, &command_pool);

		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo command_buffer_info{};
		command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_info.commandPool = command_pool;
		command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_info.commandBufferCount = 1;
		vkAllocateCommandBuffers(m_Device, &command_buffer_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(command_buffer, &begin_info);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = texture.image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkBufferImageCopy copy_region{};
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.mipLevel = 0;
		copy_region.imageSubresource.baseArrayLayer = 0;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = { static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height), 1 };
		vkCmdCopyBufferToImage(command_buffer, staging_buffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		vkQueueSubmit(m_Queue, 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_Queue);

		vkFreeCommandBuffers(m_Device, command_pool, 1, &command_buffer);
		vkDestroyCommandPool(m_Device, command_pool, nullptr);
		vkDestroyBuffer(m_Device, staging_buffer, nullptr);
		vkFreeMemory(m_Device, staging_memory, nullptr);

		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = texture.image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		vkCreateImageView(m_Device, &view_info, nullptr, &texture.image_view);

		texture.descriptor_set = ImGui_ImplVulkan_AddTexture(texture.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		auto inserted = m_Textures.emplace(key, texture);
		return &inserted.first->second;
	}

	uint32_t IconManager::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memory_properties);
		for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
		{
			if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		return 0;
	}

	void IconManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const
	{
		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(m_Device, &buffer_info, nullptr, &buffer);

		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(m_Device, buffer, &memory_requirements);

		VkMemoryAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, properties);
		vkAllocateMemory(m_Device, &allocate_info, nullptr, &memory);
		vkBindBufferMemory(m_Device, buffer, memory, 0);
	}
}

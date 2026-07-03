/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace editor
{
	struct Icon
	{
		ImTextureRef texture;
		ImVec2 size = ImVec2(0.0f, 0.0f);

		bool IsValid() const { return texture.GetTexID() != ImTextureID_Invalid; }
	};

	class IconManager
	{
	public:
		static IconManager& Instance();

		void Initialize(VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_family, VkQueue queue);
		void Shutdown();

		Icon GetIcon(const std::filesystem::path& path);
		Icon GetAppIcon();
		Icon GetEditorIcon(const std::string& name);
		Icon GetContentIcon(const std::string& name);
		Icon GetOutlinerIcon(const std::string& name);
		Icon GetComponentIcon(const std::string& name, const std::filesystem::path& project_path = {});

		void DrawIcon(const Icon& icon, float size = 16.0f) const;
		bool ImageButton(const char* id, const Icon& icon, float size = 24.0f) const;

	private:
		struct Texture
		{
			VkImage image = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkImageView image_view = VK_NULL_HANDLE;
			VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
			int width = 0;
			int height = 0;
		};

		std::optional<std::reference_wrapper<Texture>> LoadTexture(const std::filesystem::path& path);
		uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		uint32_t m_QueueFamily = UINT32_MAX;
		VkQueue m_Queue = VK_NULL_HANDLE;

		std::unordered_map<std::string, Texture> m_Textures;
	};
}

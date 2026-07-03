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

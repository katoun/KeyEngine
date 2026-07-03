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

#include "ContentBrowserWidget.h"
#include "IconManager.h"

#include <imgui.h>

#include <algorithm>
#include <system_error>
#include <vector>

namespace editor
{
	void ContentBrowserWidget::ProjectChanged(const std::filesystem::path& path)
	{
		m_ProjectPath = path;
	}

	void ContentBrowserWidget::Draw()
	{
		ImGui::Begin("Content Browser");

		if (m_ProjectPath.empty())
		{
			ImGui::TextDisabled("No project loaded.");
			ImGui::End();
			return;
		}

		Icon root_icon = IconManager::Instance().GetContentIcon("folder");
		IconManager::Instance().DrawIcon(root_icon, 16.0f);
		ImGui::SameLine();

		bool root_open = ImGui::TreeNodeEx(
			(m_ProjectPath.filename().string() + "##ProjectRoot").c_str(),
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", m_ProjectPath.string().c_str());
		if (root_open)
		{
			DrawPath(m_ProjectPath);
			ImGui::TreePop();
		}

		ImGui::End();
	}

	void ContentBrowserWidget::DrawPath(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
			return;

		std::vector<std::filesystem::path> entries;
		std::error_code error;
		for (const auto& entry : std::filesystem::directory_iterator(path, error))
			entries.emplace_back(entry.path());

		std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
			const bool lhs_directory = std::filesystem::is_directory(lhs);
			const bool rhs_directory = std::filesystem::is_directory(rhs);
			if (lhs_directory != rhs_directory)
				return lhs_directory > rhs_directory;
			return lhs.filename().string() < rhs.filename().string();
		});

		for (const auto& entry : entries)
		{
			const bool is_directory = std::filesystem::is_directory(entry);
			Icon icon = is_directory
				? IconManager::Instance().GetContentIcon("folder")
				: IconManager::Instance().GetContentIcon(IconNameForFile(entry));

			IconManager::Instance().DrawIcon(icon, 16.0f);
			ImGui::SameLine();

			const std::string label = entry.filename().string() + "##" + entry.string();
			if (is_directory)
			{
				bool opened = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("%s", entry.string().c_str());
				if (opened)
				{
					DrawPath(entry);
					ImGui::TreePop();
				}
			}
			else
			{
				ImGui::Selectable(label.c_str());
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("%s", entry.string().c_str());
			}
		}
	}

	const char* ContentBrowserWidget::IconNameForFile(const std::filesystem::path& path) const
	{
		const std::string extension = path.extension().string();
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga")
			return "file_texture";
		if (extension == ".wav" || extension == ".ogg" || extension == ".mp3")
			return "file_audio";
		if (extension == ".ttf" || extension == ".otf")
			return "file_font";
		if (extension == ".txt" || extension == ".json" || extension == ".h" || extension == ".cpp" || extension == ".cmake")
			return "file_txt";
		return "file";
	}
}

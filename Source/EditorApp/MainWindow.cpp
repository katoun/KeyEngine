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
#include "IconManager.h"
#include "WindowLabels.h"

#include <imgui_internal.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <system_error>
#include <utility>

namespace editor
{
	namespace
	{
		void SDLCALL OpenProjectDialogCallback(void* userdata, const char* const* filelist, int)
		{
			uint32_t event_type = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(userdata));
			if (event_type == 0)
				return;

			if (filelist != nullptr && filelist[0] == nullptr)
				return;

			auto result = std::make_unique<OpenProjectDialogResult>();
			if (filelist == nullptr)
			{
				result->has_error = true;
				result->value = SDL_GetError();
			}
			else
			{
				result->value = filelist[0];
			}

			SDL_Event event{};
			event.type = event_type;
			event.user.data1 = result.release();
			SDL_PushEvent(&event);
		}
	}

	MainWindow::MainWindow(SDL_Window* window)
		: m_Window(window)
	{
		m_OpenProjectDialogEvent = SDL_RegisterEvents(1);
		if (m_OpenProjectDialogEvent == 0)
			Log(std::string("SDL_RegisterEvents failed: ") + SDL_GetError());
		m_OutlinerWidget.SetSelectionChangedCallback(&MainWindow::OnOutlinerSelectionChanged, this);
		RefreshComponentsList();
		RefreshTypesList();
	}

	MainWindow::~MainWindow()
	{
		if (m_CurrentProjectModule != nullptr)
			editor::ModuleManager::Instance().UnloadModule(m_CurrentProjectModule);
	}

	void MainWindow::OpenProject(const fs::path& project_file_path)
	{
		fs::path loaded_project_path;
		std::string loaded_project_name;
		if (!LoadProjectFile(project_file_path, loaded_project_path, loaded_project_name))
		{
			Log("Unable to open project: " + project_file_path.string());
			return;
		}

		m_CurrentProjectPath = loaded_project_path;
		m_CurrentProjectName = loaded_project_name;
		m_ContentBrowserWidget.ProjectChanged(m_CurrentProjectPath);
		Log("Project loaded: " + m_CurrentProjectName);
	}

	void MainWindow::SelectionChanged(std::shared_ptr<core::Object> selection)
	{
		m_InspectorWidget.SelectionChanged(std::move(selection));
	}

	void MainWindow::HandleOpenProjectDialogResult(OpenProjectDialogResult* result)
	{
		if (result == nullptr)
			return;

		if (result->has_error)
			Log("Open project dialog failed: " + result->value);
		else
			OpenProject(result->value);
	}

	bool MainWindow::HandleEvent(const SDL_Event& event)
	{
		if (event.type != m_OpenProjectDialogEvent || m_OpenProjectDialogEvent == 0)
			return false;

		std::unique_ptr<OpenProjectDialogResult> result(static_cast<OpenProjectDialogResult*>(event.user.data1));
		HandleOpenProjectDialogResult(result.get());
		return true;
	}

	void MainWindow::Draw()
	{
		DrawAppHeader();
		DrawDockspace();
		DrawViewport();
		m_OutlinerWidget.Draw();
		m_InspectorWidget.Draw();
		m_ContentBrowserWidget.Draw();
		DrawConsole();
		DrawProjectPopups();

		if (m_ShowImguiDemo)
			ImGui::ShowDemoWindow(&m_ShowImguiDemo);
	}

	void MainWindow::DebugOpenFileMenu()
	{
		m_DebugOpenFileMenu = true;
	}

	void MainWindow::OnOutlinerSelectionChanged(void* user_data, std::shared_ptr<core::Object> selection)
	{
		auto* window = static_cast<MainWindow*>(user_data);
		if (window != nullptr)
			window->SelectionChanged(std::move(selection));
	}

	void MainWindow::DrawMainMenu()
	{
		DrawHeaderMenus();
	}

	void MainWindow::DrawHeaderMenus()
	{
		const float header_menu_margin_y = 5.0f;
		const float header_menu_height = m_HeaderHeight - (header_menu_margin_y * 2.0f);
		const ImVec2 header_button_padding(8.0f, 0.0f);
		const ImVec2 dropdown_frame_padding(8.0f, 6.0f);
		const ImVec2 header_window_pos = ImGui::GetWindowPos();

		auto begin_header_menu = [&](const char* label, bool request_open = false) {
			const ImVec2 label_size = ImGui::CalcTextSize(label);
			const ImVec2 button_size(label_size.x + (header_button_padding.x * 2.0f), header_menu_height);
			const bool popup_open = ImGui::IsPopupOpen(label, ImGuiPopupFlags_None);

			ImGui::SetCursorPosY(header_menu_margin_y);
			ImGui::PushStyleColor(ImGuiCol_Button, popup_open ? ImVec4(0.28f, 0.28f, 0.28f, 1.0f) : ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.32f, 0.32f, 0.32f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.23f, 0.47f, 0.73f, 0.75f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, header_button_padding);
			const bool pressed = ImGui::Button(label, button_size);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			const ImVec2 item_min = ImGui::GetItemRectMin();
			const bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
			const bool any_header_popup_open = GImGui->OpenPopupStack.Size > 0;
			if (pressed || request_open || (hovered && any_header_popup_open && !popup_open))
				ImGui::OpenPopup(label);

			ImGui::SetNextWindowPos(ImVec2(item_min.x, header_window_pos.y + m_HeaderHeight), ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(1.0f);
			return ImGui::BeginPopup(label);
		};

		auto draw_dropdown = [&dropdown_frame_padding](const auto& draw_content) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, dropdown_frame_padding);
			draw_content();
			ImGui::PopStyleVar();
		};

		const bool open_file_menu = m_DebugOpenFileMenu;
		if (begin_header_menu("File", open_file_menu))
		{
			draw_dropdown([this]() { DrawFileMenuItems(); });
			ImGui::EndPopup();
		}

		if (m_DebugOpenFileMenu)
		{
			m_DebugOpenFileMenu = false;
		}

		if (begin_header_menu("Edit"))
		{
			draw_dropdown([this]() {
				MenuItemWithIcon("Undo", "undo", "Ctrl+Z", false);
				MenuItemWithIcon("Redo", "redo", "Ctrl+Y", false);
				ImGui::Separator();
				MenuItemWithIcon("Cut", nullptr, "Ctrl+X", false);
				MenuItemWithIcon("Copy", nullptr, "Ctrl+C", false);
				MenuItemWithIcon("Paste", nullptr, "Ctrl+V", false);
				ImGui::Separator();
				MenuItemWithIcon("Duplicate", nullptr, "Ctrl+D", false);
				MenuItemWithIcon("Delete", nullptr, "Del", false);
				ImGui::Separator();
				MenuItemWithIcon("Find", nullptr, "Ctrl+F", false);
				MenuItemWithIcon("Select All", nullptr, "Ctrl+A", false);
				ImGui::Separator();
				MenuItemWithIcon("Editor Preferences", "editor_preferences", nullptr, false);
				MenuItemWithIcon("Project Settings", "project_settings", nullptr, false);
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("GameObject"))
		{
			draw_dropdown([this]() {
				if (MenuItemWithOutlinerIcon("Create", "gameobject", "Ctrl+Shift+N"))
					m_OutlinerWidget.CreateTopLevelGameObject();
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("Component"))
		{
			draw_dropdown([this]() {
				auto selected_game_object = m_OutlinerWidget.GetSelectedGameObject();
				for (const auto& component : m_ComponentTypes)
				{
					auto display_name_attribute = component.GetAttribute<attribute::DisplayName>();
					const std::string component_name = display_name_attribute != nullptr ? display_name_attribute->Value : component.GetName();

					Icon icon = IconManager::Instance().GetComponentIcon(component_name, m_CurrentProjectPath);
					IconManager::Instance().DrawIcon(icon, 16.0f);
					ImGui::SameLine();

					if (ImGui::MenuItem(component_name.c_str(), nullptr, false, selected_game_object != nullptr))
						m_OutlinerWidget.AttachComponent(component.GetID());
				}
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("Build"))
		{
			draw_dropdown([this]() {
				if (MenuItemWithIcon("Compile", "compile", "Ctrl+Shift+C", !m_CurrentProjectPath.empty()))
					OnCompile();
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("Window"))
		{
			draw_dropdown([this]() {
				MenuItemWithIcon("Inspector", "inspector", "Ctrl+0", false);
				MenuItemWithIcon("Outliner", "scene_outliner", "Ctrl+1", false);
				MenuItemWithIcon("Content", "content", "Ctrl+2", false);
				MenuItemWithIcon("Console", "console", "Ctrl+Shift+C", false);
				ImGui::Separator();
				if (MenuItemWithIcon("Reset Layout", nullptr))
					seed_default_layout = true;
				ImGui::MenuItem("Dear ImGui Demo", nullptr, &m_ShowImguiDemo);
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("Help"))
		{
			draw_dropdown([this]() {
				MenuItemWithIcon("Documentation", nullptr, nullptr, false);
				ImGui::Separator();
				MenuItemWithIcon("About", nullptr, nullptr, false);
			});
			ImGui::EndPopup();
		}

		if (begin_header_menu("Types"))
		{
			draw_dropdown([this]() {
				for (const auto& type : m_Types)
					ImGui::MenuItem(type.GetName().c_str(), nullptr, false, false);
			});
			ImGui::EndPopup();
		}

	}

	void MainWindow::DrawFileMenuItems()
	{
		if (MenuItemWithIcon("New", "new_project", "Ctrl+N"))
			OnNewProject();
		if (MenuItemWithIcon("Open", "open_project", "Ctrl+O"))
			OnOpenProject();
		if (MenuItemWithIcon("Save", "save", "Ctrl+S", false))
			OnSave();
		if (MenuItemWithIcon("Save All", "save_all", "Ctrl+Shift+S", false))
			OnSaveAll();

		ImGui::Separator();

		if (MenuItemWithIcon("Open VSCode", "vscode", nullptr, !m_CurrentProjectPath.empty()))
			OpenVSCode();
		if (MenuItemWithIcon("Build", "build", "Ctrl+B", !m_CurrentProjectPath.empty()))
			OnCompile();

		ImGui::Separator();

		if (MenuItemWithIcon("Exit", "exit", "Ctrl+Q"))
			OnExit();
	}

	void MainWindow::DrawAppHeader()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		m_HeaderHeight = layout::AppHeaderHeight;
		m_ToolbarHeight = 32.0f;

		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, m_HeaderHeight + m_ToolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImDrawList* foreground = ImGui::GetForegroundDrawList(viewport);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_MenuBar;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		const float header_menu_padding_y = (m_HeaderHeight - ImGui::GetFontSize()) * 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, header_menu_padding_y > 0.0f ? header_menu_padding_y : 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.188f, 0.188f, 0.188f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.188f, 0.188f, 0.188f, 1.0f));
		ImGui::Begin("AppHeader", nullptr, flags);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImVec2 window_pos = ImGui::GetWindowPos();
		draw_list->AddLine(
			ImVec2(window_pos.x, window_pos.y + m_HeaderHeight - 1.0f),
			ImVec2(window_pos.x + viewport->WorkSize.x, window_pos.y + m_HeaderHeight - 1.0f),
			ImGui::GetColorU32(ImVec4(0.137f, 0.137f, 0.137f, 1.0f)));
		draw_list->AddLine(
			ImVec2(window_pos.x, window_pos.y + m_HeaderHeight + m_ToolbarHeight - 1.0f),
			ImVec2(window_pos.x + viewport->WorkSize.x, window_pos.y + m_HeaderHeight + m_ToolbarHeight - 1.0f),
			ImGui::GetColorU32(ImVec4(0.137f, 0.137f, 0.137f, 1.0f)));

		if (ImGui::BeginMenuBar())
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 48.0f);

			DrawHeaderMenus();

			ImGui::EndMenuBar();
		}

		DrawHeaderToolbar(window_pos, m_HeaderHeight);

		const float logo_size = 20.0f;
		const ImVec2 logo_pos(window_pos.x + 14.0f, window_pos.y + (m_HeaderHeight - logo_size) * 0.5f);
		Icon logo = IconManager::Instance().GetAppIcon();
		if (logo.IsValid())
			foreground->AddImage(logo.texture, logo_pos, ImVec2(logo_pos.x + logo_size, logo_pos.y + logo_size));
		else
			foreground->AddText(logo_pos, ImGui::GetColorU32(ImVec4(0.20f, 0.62f, 0.92f, 1.0f)), "KE");

		std::string title = "KeyEditor";
		if (!m_CurrentProjectName.empty())
			title = m_CurrentProjectName + " - KeyEditor";

		const ImVec2 title_size = ImGui::CalcTextSize(title.c_str());
		const float title_x = window_pos.x + ((viewport->WorkSize.x - title_size.x) * 0.5f);
		const float title_y = window_pos.y + (m_HeaderHeight - title_size.y) * 0.5f;
		foreground->AddText(ImVec2(title_x, title_y), ImGui::GetColorU32(ImVec4(0.86f, 0.86f, 0.86f, 1.0f)), title.c_str());

		const float window_button_size = m_HeaderHeight - 8.0f;
		const float window_button_margin_y = (m_HeaderHeight - window_button_size) * 0.5f;
		const float window_button_margin_right = 4.0f;
		const float window_controls_x = window_pos.x + viewport->WorkSize.x - (window_button_size * 3.0f) - window_button_margin_right;

		auto icon_color = [](bool hovered) {
			return ImGui::GetColorU32(hovered ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.76f, 0.78f, 0.82f, 1.0f));
		};

		auto window_button = [window_pos, window_controls_x, window_button_size, window_button_margin_y, foreground, icon_color](int index, const char* tooltip, bool close_button, const auto& draw_icon) {
			const ImVec2 min(window_controls_x + (window_button_size * static_cast<float>(index)), window_pos.y + window_button_margin_y);
			const ImVec2 max(min.x + window_button_size, min.y + window_button_size);
			const bool hovered = ImGui::IsMouseHoveringRect(min, max, false);
			const bool active = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
			const bool pressed = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			if (hovered || active)
			{
				const ImU32 bg = close_button
					? ImGui::GetColorU32(active ? ImVec4(0.90f, 0.22f, 0.22f, 1.0f) : ImVec4(0.78f, 0.17f, 0.17f, 1.0f))
					: ImGui::GetColorU32(active ? ImVec4(0.30f, 0.32f, 0.36f, 1.0f) : ImVec4(0.24f, 0.25f, 0.28f, 1.0f));
				foreground->AddRectFilled(min, max, bg);
			}
			draw_icon(foreground, min, max, icon_color(hovered || active));
			if (hovered)
				ImGui::SetTooltip("%s", tooltip);
			return pressed;
		};

		if (window_button(0, "Minimize", false, [](ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, ImU32 color) {
			const float center_x = (min.x + max.x) * 0.5f;
			const float center_y = (min.y + max.y) * 0.5f;
			draw_list->AddLine(ImVec2(center_x - 6.0f, center_y), ImVec2(center_x + 6.0f, center_y), color, 1.5f);
		}))
			SDL_MinimizeWindow(m_Window);

		const bool maximized = (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED) != 0;
		if (window_button(1, maximized ? "Restore" : "Maximize", false, [maximized](ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, ImU32 color) {
			const float center_x = (min.x + max.x) * 0.5f;
			const float center_y = (min.y + max.y) * 0.5f;
			if (maximized)
			{
				draw_list->AddRect(ImVec2(center_x - 5.0f, center_y - 2.0f), ImVec2(center_x + 3.0f, center_y + 6.0f), color, 0.0f, 0, 1.3f);
				draw_list->AddRect(ImVec2(center_x - 2.0f, center_y - 5.0f), ImVec2(center_x + 6.0f, center_y + 3.0f), color, 0.0f, 0, 1.3f);
			}
			else
			{
				draw_list->AddRect(ImVec2(center_x - 5.0f, center_y - 5.0f), ImVec2(center_x + 5.0f, center_y + 5.0f), color, 0.0f, 0, 1.3f);
			}
		}))
		{
			if (maximized)
				SDL_RestoreWindow(m_Window);
			else
				SDL_MaximizeWindow(m_Window);
		}

		if (window_button(2, "Close", true, [](ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, ImU32 color) {
			const float center_x = (min.x + max.x) * 0.5f;
			const float center_y = (min.y + max.y) * 0.5f;
			draw_list->AddLine(ImVec2(center_x - 5.0f, center_y - 5.0f), ImVec2(center_x + 5.0f, center_y + 5.0f), color, 1.5f);
			draw_list->AddLine(ImVec2(center_x + 5.0f, center_y - 5.0f), ImVec2(center_x - 5.0f, center_y + 5.0f), color, 1.5f);
		}))
			OnExit();

		ImGui::End();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(5);

		if (m_DebugShowFileMenu)
		{
			m_DebugFileMenuPopupPos.y = window_pos.y + m_HeaderHeight;
			ImGui::SetNextWindowPos(m_DebugFileMenuPopupPos, ImGuiCond_Always);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 3.0f));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.204f, 0.204f, 0.204f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.137f, 0.137f, 0.137f, 1.0f));
			ImGuiWindowFlags debug_menu_flags = ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoDocking
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::Begin("DebugFileMenu", &m_DebugShowFileMenu, debug_menu_flags))
				DrawFileMenuItems();
			ImGui::End();
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(4);
		}
	}

	void MainWindow::DrawHeaderToolbar(const ImVec2& window_pos, float y_offset)
	{
		constexpr const char* configs[] = { "Debug", "Release" };
		const float button_height = 24.0f;
		const float play_width = 36.0f;
		const float pause_width = 36.0f;
		const float combo_width = 112.0f;
		const float spacing = 6.0f;
		const float toolbar_left_margin = 8.0f;
		const float y = y_offset + ((m_ToolbarHeight - button_height) * 0.5f);

		ImGui::SetCursorScreenPos(ImVec2(window_pos.x + toolbar_left_margin, window_pos.y + y));

		auto toolbar_button = [](const char* id, const Icon& icon, const ImVec2& size, bool enabled, const char* fallback_label) {
			ImGui::BeginDisabled(!enabled);
			const ImVec2 min = ImGui::GetCursorScreenPos();
			const bool pressed = ImGui::InvisibleButton(id, size);
			const bool hovered = enabled && ImGui::IsItemHovered();
			const bool active = enabled && ImGui::IsItemActive();
			const ImVec2 max(min.x + size.x, min.y + size.y);

			const ImU32 bg = ImGui::GetColorU32(active
				? ImVec4(0.23f, 0.47f, 0.73f, 0.75f)
				: hovered ? ImVec4(0.34f, 0.34f, 0.34f, 1.0f) : ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
			const ImU32 border = ImGui::GetColorU32(ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(min, max, bg, 1.0f);
			draw_list->AddRect(min, max, border, 1.0f);

			if (icon.IsValid())
			{
				const float icon_size = 14.0f;
				const ImVec2 icon_min(min.x + ((size.x - icon_size) * 0.5f), min.y + ((size.y - icon_size) * 0.5f));
				const ImVec2 icon_max(icon_min.x + icon_size, icon_min.y + icon_size);
				draw_list->AddImage(icon.texture, icon_min, icon_max, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(enabled ? ImVec4(0.88f, 0.88f, 0.88f, 1.0f) : ImVec4(0.56f, 0.56f, 0.56f, 1.0f)));
			}
			else
			{
				const ImVec2 label_size = ImGui::CalcTextSize(fallback_label);
				draw_list->AddText(ImVec2(min.x + ((size.x - label_size.x) * 0.5f), min.y + ((size.y - label_size.y) * 0.5f)), ImGui::GetColorU32(enabled ? ImVec4(0.88f, 0.88f, 0.88f, 1.0f) : ImVec4(0.56f, 0.56f, 0.56f, 1.0f)), fallback_label);
			}

			ImGui::EndDisabled();
			return pressed && enabled;
		};

		const Icon play_icon = IconManager::Instance().GetEditorIcon(m_IsPlaying ? "exit" : "play");
		if (toolbar_button("##HeaderPlay", play_icon, ImVec2(play_width, button_height), true, m_IsPlaying ? "Stop" : "Play"))
		{
			m_IsPlaying = !m_IsPlaying;
			if (!m_IsPlaying)
				m_IsPaused = false;
			Log(m_IsPlaying ? "Play mode entered." : "Play mode stopped.");
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", m_IsPlaying ? "Stop" : "Play");

		ImGui::SameLine(0.0f, spacing);
		const Icon pause_icon = IconManager::Instance().GetEditorIcon("pause");
		if (toolbar_button("##HeaderPause", pause_icon, ImVec2(pause_width, button_height), m_IsPlaying, "Pause"))
		{
			m_IsPaused = !m_IsPaused;
			Log(m_IsPaused ? "Play mode paused." : "Play mode resumed.");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			ImGui::SetTooltip("%s", m_IsPaused ? "Resume" : "Pause");

		ImGui::SameLine(0.0f, spacing);
		ImGui::SetNextItemWidth(combo_width);
		const float combo_padding_y = (button_height - ImGui::GetFontSize()) * 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, combo_padding_y > 0.0f ? combo_padding_y : 0.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.34f, 0.34f, 0.34f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.23f, 0.47f, 0.73f, 0.75f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.30f, 0.30f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.38f, 0.38f, 0.38f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.23f, 0.47f, 0.73f, 0.75f));
		if (ImGui::BeginCombo("##HeaderBuildConfig", configs[m_BuildConfigIndex]))
		{
			for (int i = 0; i < 2; ++i)
			{
				const bool selected = m_BuildConfigIndex == i;
				if (ImGui::Selectable(configs[i], selected))
					m_BuildConfigIndex = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Build configuration");
		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar();
	}

	bool MainWindow::MenuItemWithIcon(const char* label, const char* icon_name, const char* shortcut, bool enabled)
	{
		if (icon_name != nullptr)
			IconManager::Instance().DrawIcon(IconManager::Instance().GetEditorIcon(icon_name), 16.0f);
		else
			ImGui::Dummy(ImVec2(16.0f, 16.0f));
		ImGui::SameLine();
		return ImGui::MenuItem(label, shortcut, false, enabled);
	}

	bool MainWindow::MenuItemWithOutlinerIcon(const char* label, const char* icon_name, const char* shortcut, bool enabled)
	{
		IconManager::Instance().DrawIcon(IconManager::Instance().GetOutlinerIcon(icon_name), 16.0f);
		ImGui::SameLine();
		return ImGui::MenuItem(label, shortcut, false, enabled);
	}

	void MainWindow::DrawDockspace()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + m_HeaderHeight + m_ToolbarHeight));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - m_HeaderHeight - m_ToolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Dockspace", nullptr, flags);
		ImGui::PopStyleVar(2);

		const ImGuiID dockspace_id = ImGui::GetID("KeyEngineDockspace");
		if (seed_default_layout)
		{
			SeedDefaultDockLayout(dockspace_id, viewport->WorkSize);
			seed_default_layout = false;
		}

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::End();
	}

	void MainWindow::SeedDefaultDockLayout(ImGuiID dockspace_id, const ImVec2& size)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, size);

		ImGuiID center_id = dockspace_id;
		ImGuiID left_id = ImGui::DockBuilderSplitNode(center_id, ImGuiDir_Left, 0.25f, nullptr, &center_id);
		ImGuiID right_id = ImGui::DockBuilderSplitNode(center_id, ImGuiDir_Right, 0.26f, nullptr, &center_id);
		ImGuiID bottom_id = ImGui::DockBuilderSplitNode(center_id, ImGuiDir_Down, 0.30f, nullptr, &center_id);

		ImGui::DockBuilderDockWindow(window_label::Outliner, left_id);
		ImGui::DockBuilderDockWindow(window_label::Inspector, right_id);
		ImGui::DockBuilderDockWindow(window_label::ContentBrowser, bottom_id);
		ImGui::DockBuilderDockWindow(window_label::Console, bottom_id);
		ImGui::DockBuilderDockWindow(window_label::Viewport, center_id);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	void MainWindow::DrawViewport()
	{
		ImGui::Begin(window_label::Viewport);
		ImGui::TextUnformatted("Scene viewport");
		ImGui::Separator();
		ImGui::TextWrapped("The Vulkan-backed editor shell is running. This panel is ready for engine rendering integration.");
		ImGui::End();
	}

	void MainWindow::DrawConsole()
	{
		ImGui::Begin(window_label::Console);
		if (ImGui::Button("Clear"))
			m_Log.clear();
		ImGui::SameLine();
		ImGui::TextDisabled("%zu lines", m_Log.size());
		ImGui::Separator();
		for (const std::string& line : m_Log)
			ImGui::TextWrapped("%s", line.c_str());
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
		ImGui::End();
	}

	void MainWindow::DrawProjectPopups()
	{
		if (m_ShowNewProject)
		{
			ImGui::OpenPopup("New Project");
			m_ShowNewProject = false;
		}
		if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Name", m_NewProjectName, sizeof(m_NewProjectName));
			ImGui::InputText("Parent Path", m_NewProjectParent, sizeof(m_NewProjectParent));
			if (ImGui::Button("Create"))
			{
				CreateProject();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	void MainWindow::OnNewProject()
	{
		m_ShowNewProject = true;
	}

	void MainWindow::OnOpenProject()
	{
		fs::path default_location = m_CurrentProjectPath.empty()
			? m_SdkPath.parent_path() / "KeyEngineProjects"
			: m_CurrentProjectPath;

		static const SDL_DialogFileFilter filters[] =
		{
			{ "KeyEngine Project", "keyproject" },
			{ "All Files", "*" },
		};

		std::string default_location_string = default_location.string();
		SDL_ShowOpenFileDialog(
			OpenProjectDialogCallback,
			reinterpret_cast<void*>(static_cast<std::uintptr_t>(m_OpenProjectDialogEvent)),
			m_Window,
			filters,
			2,
			default_location_string.c_str(),
			false);
	}

	void MainWindow::OnSave()
	{
		Log("Save is not wired in the ImGui editor yet.");
	}

	void MainWindow::OnSaveAll()
	{
		Log("Save All is not wired in the ImGui editor yet.");
	}

	void MainWindow::OnExit()
	{
		SDL_Event event{};
		event.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&event);
	}

	void MainWindow::OpenVSCode()
	{
		if (m_CurrentProjectPath.empty())
			return;

		std::string command = "code " + ShellQuote(m_CurrentProjectPath) + " >/dev/null 2>&1 &";
		std::system(command.c_str());
	}

	void MainWindow::OnCompile()
	{
		if (m_CurrentProjectPath.empty() || m_CurrentProjectName.empty())
		{
			Log("No project loaded.");
			return;
		}

		if (m_CurrentProjectModule != nullptr)
		{
			editor::ModuleManager::Instance().UnloadModule(m_CurrentProjectModule);
			m_CurrentProjectModule = nullptr;
			RefreshComponentsList();
			RefreshTypesList();
		}

		std::string parser_command = "KEY_ENGINE_SDK_PATH=" + ShellQuote(m_SdkPath)
			+ " " + ShellQuote(ToolPath("ReflectionParser"))
			+ " --project-name " + ShellQuote(m_CurrentProjectName)
			+ " --project-path " + ShellQuote(m_CurrentProjectPath);
		if (!RunCommand(parser_command, m_Log))
			return;

		std::error_code error;
		fs::remove_all(m_CurrentProjectPath / "Build", error);
		fs::remove_all(m_CurrentProjectPath / "Binaries", error);

		std::string configure_command = "cmake -S " + ShellQuote(m_CurrentProjectPath)
			+ " -B " + ShellQuote(m_CurrentProjectPath / "Build" / "Linux-Debug")
			+ " -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++-20"
			+ " -DKEY_ENGINE_SDK_PATH=" + ShellQuote(m_SdkPath);
		if (!RunCommand(configure_command, m_Log))
			return;

		std::string build_command = "cmake --build "
			+ ShellQuote(m_CurrentProjectPath / "Build" / "Linux-Debug")
			+ " --parallel";
		if (!RunCommand(build_command, m_Log))
			return;

		m_CurrentProjectModule = editor::ModuleManager::Instance().LoadModule(m_CurrentProjectPath.string(), m_CurrentProjectName);
		RefreshComponentsList();
		RefreshTypesList();
		Log("Compile complete.");
	}

	void MainWindow::CreateProject()
	{
		fs::path parent_path = m_NewProjectParent;
		std::string name = m_NewProjectName;
		std::string command = "KEY_ENGINE_SDK_PATH=" + ShellQuote(m_SdkPath)
			+ " " + ShellQuote(ToolPath("ProjectCreator"))
			+ " --project-name " + ShellQuote(name)
			+ " --project-path " + ShellQuote(parent_path);

		if (!RunCommand(command, m_Log))
			return;

		OpenProject(parent_path / name / (name + ".keyproject"));
	}

	void MainWindow::RefreshComponentsList()
	{
		m_ComponentTypes = reflection::ReflectionManager::Instance().GetTypes<game::Component>();
	}

	void MainWindow::RefreshTypesList()
	{
		m_Types = reflection::ReflectionManager::Instance().GetTypes();
	}

	void MainWindow::Log(const std::string& message)
	{
		m_Log.emplace_back(message);
		std::cout << message << std::endl;
	}
}

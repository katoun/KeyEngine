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

#include "ContentBrowserWidget.h"
#include "InspectorWidget.h"
#include "LayoutConstants.h"
#include "OutlinerWidget.h"
#include "Utils.h"

#include <Editor/ModuleManager.h>
#include <SDL3/SDL.h>
#include <imgui.h>

#include <memory>
#include <string>
#include <vector>

namespace editor
{
	struct OpenProjectDialogResult
	{
		bool has_error = false;
		std::string value;
	};

	class MainWindow
	{
	public:
		explicit MainWindow(SDL_Window* window);
		~MainWindow();

		void OpenProject(const fs::path& project_file_path);
		void SelectionChanged(std::shared_ptr<core::Object> selection);
		void HandleOpenProjectDialogResult(OpenProjectDialogResult* result);
		bool HandleEvent(const SDL_Event& event);

		void Draw();
		void DebugOpenFileMenu();

		bool seed_default_layout = false;

	private:
		static void OnOutlinerSelectionChanged(void* user_data, std::shared_ptr<core::Object> selection);

		void DrawMainMenu();
		void DrawHeaderMenus();
		void DrawFileMenuItems();
		void DrawAppHeader();
		void DrawHeaderToolbar(const ImVec2& window_pos, float y_offset);
		bool MenuItemWithIcon(const char* label, const char* icon_name, const char* shortcut = nullptr, bool enabled = true);
		bool MenuItemWithOutlinerIcon(const char* label, const char* icon_name, const char* shortcut = nullptr, bool enabled = true);
		void DrawDockspace();
		void SeedDefaultDockLayout(ImGuiID dockspace_id, const ImVec2& size);
		void DrawViewport();
		void DrawConsole();
		void DrawProjectPopups();

		void OnNewProject();
		void OnOpenProject();
		void OnSave();
		void OnSaveAll();
		void OnExit();
		void OpenVSCode();
		void OnCompile();

		void CreateProject();
		void RefreshComponentsList();
		void RefreshTypesList();

		void Log(const std::string& message);

		uint32_t m_OpenProjectDialogEvent = 0;
		SDL_Window* m_Window = nullptr;
		fs::path m_SdkPath = SdkPath();
		fs::path m_CurrentProjectPath;
		std::string m_CurrentProjectName;
		editor::ModuleManager::ModulePtr m_CurrentProjectModule;

		OutlinerWidget m_OutlinerWidget;
		ContentBrowserWidget m_ContentBrowserWidget;
		InspectorWidget m_InspectorWidget;

		std::vector<std::string> m_Log;
		std::vector<reflection::Type> m_ComponentTypes;
		std::vector<reflection::Type> m_Types;

		float m_HeaderHeight = layout::AppHeaderHeight;
		float m_ToolbarHeight = 0.0f;
		bool m_DebugOpenFileMenu = false;
		bool m_DebugShowFileMenu = false;
		ImVec2 m_DebugFileMenuPopupPos{};
		bool m_ShowNewProject = false;
		bool m_ShowImguiDemo = false;
		bool m_IsPlaying = false;
		bool m_IsPaused = false;
		int m_BuildConfigIndex = 0;
		char m_NewProjectName[128] = "Test";
		char m_NewProjectParent[PATH_MAX] = "/home/catalin/Work/Programming/KeyEngineProjects";
	};
}

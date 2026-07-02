/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "ContentBrowserWidget.h"
#include "InspectorWidget.h"
#include "OutlinerWidget.h"
#include "Utils.h"

#include <Editor/ModuleManager.h>
#include <SDL3/SDL.h>
#include <imgui.h>

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
		void SelectionChanged(core::Object* selection);
		void HandleOpenProjectDialogResult(OpenProjectDialogResult* result);
		bool HandleEvent(const SDL_Event& event);

		void Draw();

		bool seed_default_layout = false;

	private:
		static void OnOutlinerSelectionChanged(void* user_data, core::Object* selection);

		void DrawMainMenu();
		void DrawHeaderMenus();
		void DrawAppHeader();
		bool MenuItemWithIcon(const char* label, const char* icon_name, const char* shortcut = nullptr, bool enabled = true);
		bool MenuItemWithOutlinerIcon(const char* label, const char* icon_name, const char* shortcut = nullptr, bool enabled = true);
		void DrawToolbar();
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
		editor::Module* m_CurrentProjectModule = nullptr;

		OutlinerWidget m_OutlinerWidget;
		ContentBrowserWidget m_ContentBrowserWidget;
		InspectorWidget m_InspectorWidget;

		std::vector<std::string> m_Log;
		std::vector<reflection::Type> m_ComponentTypes;
		std::vector<reflection::Type> m_Types;

		float m_HeaderHeight = 44.0f;
		float m_ToolbarHeight = 60.0f;
		bool m_ShowNewProject = false;
		bool m_ShowImguiDemo = false;
		char m_NewProjectName[128] = "Test";
		char m_NewProjectParent[PATH_MAX] = "/home/catalin/Work/Programming/KeyEngineProjects";
	};
}

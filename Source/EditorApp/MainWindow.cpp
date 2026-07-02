/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MainWindow.h"
#include "IconManager.h"

#include <imgui_internal.h>

#include <cstdint>
#include <iostream>
#include <system_error>

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

			auto* result = new OpenProjectDialogResult();
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
			event.user.data1 = result;
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

	void MainWindow::SelectionChanged(core::Object* selection)
	{
		m_InspectorWidget.SelectionChanged(selection);
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

		auto* result = static_cast<OpenProjectDialogResult*>(event.user.data1);
		HandleOpenProjectDialogResult(result);
		delete result;
		return true;
	}

	void MainWindow::Draw()
	{
		DrawMainMenu();
		DrawToolbar();
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

	void MainWindow::OnOutlinerSelectionChanged(void* user_data, core::Object* selection)
	{
		auto* window = static_cast<MainWindow*>(user_data);
		if (window != nullptr)
			window->SelectionChanged(selection);
	}

	void MainWindow::DrawMainMenu()
	{
		if (!ImGui::BeginMainMenuBar())
			return;

		if (ImGui::BeginMenu("File"))
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

			if (MenuItemWithIcon("Open VSCode", "visual_studio", nullptr, !m_CurrentProjectPath.empty()))
				OpenVSCode();
			if (MenuItemWithIcon("Build", "build", "Ctrl+B", !m_CurrentProjectPath.empty()))
				OnCompile();

			ImGui::Separator();

			if (MenuItemWithIcon("Exit", "exit", "Ctrl+Q"))
				OnExit();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
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
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (MenuItemWithOutlinerIcon("Create", "gameobject", "Ctrl+Shift+N"))
				m_OutlinerWidget.CreateTopLevelGameObject();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Component"))
		{
			game::GameObject* selected_game_object = m_OutlinerWidget.GetSelectedGameObject();
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
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Build"))
		{
			if (MenuItemWithIcon("Compile", "compile", "Ctrl+Shift+C", !m_CurrentProjectPath.empty()))
				OnCompile();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			MenuItemWithIcon("Inspector", "inspector", "Ctrl+0", false);
			MenuItemWithIcon("Outliner", "scene_outliner", "Ctrl+1", false);
			MenuItemWithIcon("Content", "content", "Ctrl+2", false);
			MenuItemWithIcon("Console", "console", "Ctrl+Shift+C", false);
			ImGui::Separator();
			if (MenuItemWithIcon("Reset Layout", nullptr))
				seed_default_layout = true;
			ImGui::MenuItem("Dear ImGui Demo", nullptr, &m_ShowImguiDemo);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			MenuItemWithIcon("Documentation", nullptr, nullptr, false);
			ImGui::Separator();
			MenuItemWithIcon("About", nullptr, nullptr, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Types"))
		{
			for (const auto& type : m_Types)
				ImGui::MenuItem(type.GetName().c_str(), nullptr, false, false);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
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

	void MainWindow::DrawToolbar()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		const float menu_height = ImGui::GetFrameHeight();
		m_ToolbarHeight = 42.0f;

		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + menu_height));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, m_ToolbarHeight));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
		ImGui::Begin("Toolbar", nullptr, flags);
		ImGui::PopStyleVar();

		auto toolbar_button = [](const char* id, const char* tooltip, const Icon& icon) {
			bool pressed = IconManager::Instance().ImageButton(id, icon, 28.0f);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("%s", tooltip);
			ImGui::SameLine();
			return pressed;
		};

		if (toolbar_button("##translate", "Translate", IconManager::Instance().GetEditorIcon("translate")))
		{
		}
		if (toolbar_button("##rotate", "Rotate", IconManager::Instance().GetEditorIcon("rotate")))
		{
		}
		if (toolbar_button("##scale", "Scale", IconManager::Instance().GetEditorIcon("scale")))
		{
		}

		ImGui::Dummy(ImVec2(8.0f, 1.0f));
		ImGui::SameLine();

		if (toolbar_button("##compile", "Compile", IconManager::Instance().GetEditorIcon("compile")))
			OnCompile();
		if (toolbar_button("##build", "Build", IconManager::Instance().GetEditorIcon("build")))
			OnCompile();
		if (toolbar_button("##open_project", "Open Project", IconManager::Instance().GetEditorIcon("open_project")))
			OnOpenProject();

		ImGui::NewLine();
		ImGui::End();
	}

	void MainWindow::DrawDockspace()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_NoBackground;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		const float menu_height = ImGui::GetFrameHeight();
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + menu_height + m_ToolbarHeight));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - menu_height - m_ToolbarHeight));
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

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
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

		ImGui::DockBuilderDockWindow("Outliner", left_id);
		ImGui::DockBuilderDockWindow("Inspector", right_id);
		ImGui::DockBuilderDockWindow("Content Browser", bottom_id);
		ImGui::DockBuilderDockWindow("Console", bottom_id);
		ImGui::DockBuilderDockWindow("Viewport", center_id);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	void MainWindow::DrawViewport()
	{
		ImGui::Begin("Viewport");
		ImGui::TextUnformatted("Scene viewport");
		ImGui::Separator();
		ImGui::TextWrapped("The Vulkan-backed editor shell is running. This panel is ready for engine rendering integration.");
		ImGui::End();
	}

	void MainWindow::DrawConsole()
	{
		ImGui::Begin("Console");
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

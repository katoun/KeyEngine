// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Editor/ModuleManager.h>
#include <Editor/Module.h>

editor::ModuleManager *editor::ModuleManager::m_Instance = nullptr;

namespace editor
{

	ModuleManager::ModuleManager() {}

	ModuleManager::~ModuleManager()
	{
		UnloadAllModules();
	}

	Module* ModuleManager::LoadModule(const std::string& path, const std::string& name)
	{
		Module* module = new Module(path, name);
		module->Load();
		m_Modules.push_back(module);

		return module;
	}

	void ModuleManager::UnloadModule(Module* module)
	{
		if (module == nullptr)
			return;

		module->Unload();

		for (auto it = m_Modules.begin(); it != m_Modules.end(); ++it)
		{
			if (**it == *module)
			{
				m_Modules.erase(it);
				break;
			}
		}
	}

	bool ModuleManager::ReloadModule(Module* module)
	{
		if (module == nullptr)
			return false;

		return module->Reload();
	}

	void ModuleManager::UnloadAllModules()
	{
		for each (auto module in m_Modules)
		{
			if (module == nullptr)
				continue;

			module->Unload();

			SAFE_DELETE(module);
		}

		m_Modules.clear();
	}

	void ModuleManager::ReloadAllModules()
	{
		for each (auto module in m_Modules)
		{
			if (module == nullptr)
				continue;

			module->Reload();
		}
	}

	ModuleManager* ModuleManager::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = new ModuleManager;

		return m_Instance;
	}
}

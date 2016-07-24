/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Editor/ModuleManager.h>
#include <Editor/Module.h>

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

	ModuleManager &ModuleManager::Instance(void)
	{
		static ModuleManager instance;

		return instance;
	}
}

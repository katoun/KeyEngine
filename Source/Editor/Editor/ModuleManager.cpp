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

#include <Editor/ModuleManager.h>
#include <Editor/Module.h>

namespace editor
{

	ModuleManager::ModuleManager() {}

	ModuleManager::~ModuleManager()
	{
		UnloadAllModules();
	}

	ModuleManager::ModulePtr ModuleManager::LoadModule(const std::string& path, const std::string& name)
	{
		auto module = std::make_shared<Module>(path, name);
		module->Load();
		m_Modules.emplace_back(module);

		return module;
	}

	void ModuleManager::UnloadModule(ModulePtr module)
	{
		if (module == nullptr)
			return;

		module->Unload();

		for (auto it = m_Modules.begin(); it != m_Modules.end(); ++it)
		{
			if (*it == module || **it == *module)
			{
				m_Modules.erase(it);
				break;
			}
		}
	}

	bool ModuleManager::ReloadModule(const ModulePtr& module)
	{
		if (module == nullptr)
			return false;

		return module->Reload();
	}

	void ModuleManager::UnloadAllModules()
	{
		for (auto& module : m_Modules)
		{
			if (module == nullptr)
				continue;

			module->Unload();
		}

		m_Modules.clear();
	}

	void ModuleManager::ReloadAllModules()
	{
		for (auto& module : m_Modules)
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

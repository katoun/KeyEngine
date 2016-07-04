/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <EditorConfig.h>

#include <string>
#include <vector>

namespace editor
{
	class Module;

	class EDITOR_API ModuleManager
	{
	public:

		ModuleManager();
		~ModuleManager();

		Module* LoadModule(const std::string& path, const std::string& name);

		void UnloadModule(Module* module);
		bool ReloadModule(Module* module);

		void UnloadAllModules();
		void ReloadAllModules();

		static ModuleManager* GetInstance();

	protected:

		std::vector<Module*> m_Modules;

		static ModuleManager *m_Instance;
	};
}
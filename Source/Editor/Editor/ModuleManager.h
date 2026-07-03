/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <EditorConfig.h>

#include <memory>
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

		using ModulePtr = std::shared_ptr<Module>;

		ModulePtr LoadModule(const std::string& path, const std::string& name);

		void UnloadModule(ModulePtr module);
		bool ReloadModule(const ModulePtr& module);

		void UnloadAllModules();
		void ReloadAllModules();

		static ModuleManager &Instance(void);

	protected:

		std::vector<ModulePtr> m_Modules;
	};
}

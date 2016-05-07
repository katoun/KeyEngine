// Copyright (c) 2016 Catalin Alexandru Nastase

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
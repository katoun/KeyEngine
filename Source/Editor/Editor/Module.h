/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <EditorConfig.h>

#include <string>

#ifdef PLATFORM_WIN
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif PLATFORM_LINUX
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
#endif

namespace editor
{
	typedef void(*LOAD_MODULE_FUNCTION)();
	typedef void(*UNLOAD_MODULE_FUNCTION)();

	class EDITOR_API Module
	{
	public:

		Module(const std::string& path, const std::string& name);
		~Module();

		bool operator==(const Module &rhs) const;
		bool operator!=(const Module &rhs) const;

		const std::string& GetName() const;

		bool Load();
		void Unload();
		bool Reload();

		bool IsLoaded() const;

		void* GetSymbol(const std::string& name) const throw();

	protected:

		std::string m_Name;

		std::string m_Path;
		std::string m_FileName;

		std::string dynlibError();

		DYNLIB_HANDLE m_hInst;
	};
}
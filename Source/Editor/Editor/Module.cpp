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

#include <Editor/Module.h>

#include <cassert>

#ifdef PLATFORM_WIN
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined(PLATFORM_LINUX)
#	include <dlfcn.h>
#endif

namespace editor
{
	Module::Module(const std::string& path, const std::string& name)
	{
		m_Name = name;

		std::string build_path = "Build";
		std::string extention = "";
		std::string postfix = "-EditorModule";

#ifdef PLATFORM_WIN
#	ifdef ARCHITECTURE_64
		build_path += "/Win64";
#	else
		build_path += "/Win32";
#	endif

#	if defined(_DEBUG)
		extention = "_d.dll";
#	else
		extention = ".dll";
#	endif
#elif PLATFORM_LINUX
		build_path += "/Linux";
#	if defined(_DEBUG)
		extention = "_d.so";
#	else
		extention = ".so";
#	endif
#endif
		m_Path = path + "/" + build_path;
		m_FileName = path + "/" + build_path + "/" + name + postfix + extention;

		m_hInst = nullptr;
	}

	Module::~Module() {}

	bool Module::operator==(const Module &rhs) const
	{
		return m_FileName == rhs.m_FileName;
	}

	bool Module::operator!=(const Module &rhs) const
	{
		return m_FileName != rhs.m_FileName;
	}

	const std::string& Module::GetName() const
	{
		return m_Name;
	}

	bool Module::Load()
	{
#ifdef UNICODE
		std::wstring filename = std::wstring(m_FileName.begin(), m_FileName.end());
		m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(filename.c_str());
#else
		m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(m_FileName.c_str());
#endif
		assert(m_hInst != nullptr);

		// Call startup	
		LOAD_MODULE_FUNCTION pLoadFunc = (LOAD_MODULE_FUNCTION)GetSymbol("LoadModule");

		if (pLoadFunc != nullptr) pLoadFunc();

		return true;
	}

	void Module::Unload()
	{
		// Call shutdown
		UNLOAD_MODULE_FUNCTION pUnloadFunc = (UNLOAD_MODULE_FUNCTION)GetSymbol("UnloadModule");

		if (pUnloadFunc != nullptr) pUnloadFunc();

		auto closed = DYNLIB_UNLOAD(m_hInst);

#ifdef PLATFORM_WIN
		assert(closed == TRUE);
#elif defined(PLATFORM_LINUX)
		assert(closed == 0);
#endif
	}

	bool Module::Reload()
	{
		Unload();
		return Load();
	}

	bool Module::IsLoaded() const
	{
		return false;
	}

	void* Module::GetSymbol(const std::string& strName) const throw()
	{
		return (void*)DYNLIB_GETSYM(m_hInst, strName.c_str());
	}

	std::string Module::dynlibError()
	{
#ifdef PLATFORM_WIN
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		std::string ret = (char*)lpMsgBuf;
		// Free the buffer.
		LocalFree(lpMsgBuf);
		return ret;
#elif PLATFORM_LINUX
		const char* error = dlerror();
		return error != nullptr ? std::string(error) : std::string();
#else
		return std::string("");
#endif
	}
}

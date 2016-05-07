// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Editor/Module.h>

#include <cassert>

#ifdef PLATFORM_WIN
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
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

		BOOL closed = DYNLIB_UNLOAD(m_hInst);

		assert(closed == TRUE);
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
		return std::string(dlerror());
#else
		return std::string("");
#endif
	}
}

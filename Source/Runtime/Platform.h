// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

// Detect platform
#if defined( WIN32 ) || defined( _WINDOWS ) || defined( _WIN32 )
#	if !defined( PLATFORM_WIN )
#		define PLATFORM_WIN
#	endif
#elif defined( __APPLE__ ) || defined( __APPLE_CC__ )
#   if !defined( PLATFORM_MAC )
#      define PLATFORM_MAC
#   endif
#else
#	if !defined( PLATFORM_LINUX )
#		define PLATFORM_LINUX
#	endif
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#	if !defined( ARCHITECTURE_64 )
#		define ARCHITECTURE_64
#	endif
#else
#	if !defined( ARCHITECTURE_32 )
#		define ARCHITECTURE_32
#	endif
#endif

#ifdef PLATFORM_WIN
#	if defined(MINGW) || defined(__MINGW32__)
// Linux compilers don't have symbol import/export directives.
#		define DLL_EXPORT
#		define DLL_IMPORT
#		define DLL_PRIVATE
#	else
#		define DLL_EXPORT			__declspec(dllexport)
#		define DLL_IMPORT			__declspec(dllimport)
#		define DLL_PRIVATE
#	endif
#elif PLATFORM_LINUX
#	if defined( __GNUC__ ) && __GNUC__ >= 4
#		define DLL_EXPORT __attribute__ ((visibility("default")))
#		define DLL_IMPORT __attribute__ ((visibility ("default")))
#		define DLL_PRIVATE __attribute__ ((visibility ("hidden")))
#	else
#		define DLL_EXPORT
#		define DLL_IMPORT
#		define DLL_PRIVATE
#	endif
#else
#	define DLL_EXPORT
#	define DLL_IMPORT
#	define DLL_PRIVATE
#endif
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
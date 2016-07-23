/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Platform.h>

#include <experimental/filesystem>

#ifdef RUNTIME_DLL
#	ifdef RUNTIME_EXPORTS
#		define RUNTIME_API DLL_EXPORT
#		define RUNTIME_TEMPLATE DLL_EXPORT
#	else
#		define RUNTIME_API DLL_IMPORT
#		define RUNTIME_TEMPLATE
#	endif
#	define RUNTIME_PRIVATE DLL_PRIVATE
#else
#	define RUNTIME_API
#	define RUNTIME_TEMPLATE
#	define RUNTIME_PRIVATE
#endif

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#pragma warning(disable : 4251)

// This warns about truncation to 255 characters in debug/browse info
#pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#pragma warning (disable : 4503)

#define RUNTIME_VERSION "0.0.1"

#define ENGINE_UNIT_M 1

#define MAX_PATH_SIZE 255

#define SAFE_DELETE(object_ptr) if(object_ptr != nullptr) { delete object_ptr; object_ptr = nullptr; }
#define SAFE_DELETE_ARRAY(object_ptr) if(object_ptr != nullptr) { delete []object_ptr; object_ptr = nullptr; }

#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define __MESSAGE(text) __pragma( message(__FILE__ "(" STRINGIFY(__LINE__) ")" text) )

// Outputs a warning during compile time
#define ENGINE_WARNING(text) __MESSAGE( " : Warning: " ##text )

// Outputs an error during compile time
#define ENGINE_ERROR(text) __MESSAGE( " : Error: " ##text )

// Outputs a message during compile time
#define ENGINE_MESSAGE(text) __MESSAGE( ": " ##text )

#define ENGINE_TODO(text) __MESSAGE( ": TODO: " ##text )

namespace filesystem = std::experimental::filesystem;

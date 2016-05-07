/*
-----------------------------------------------------------------------------
Key Engine (http://github.com/katoun/KeyEngine) is made available under the MIT License.

Copyright (c) 2016 Catalin Alexandru Nastase
-----------------------------------------------------------------------------
*/

#pragma once

#include <Platform.h>

#ifdef PROJECT_DLL
#	ifdef PROJECT_EXPORTS
#		define PROJECT_API DLL_EXPORT
#	else
#		define PROJECT_API DLL_IMPORT
#	endif
#	define PROJECT_PRIVATE DLL_PRIVATE
#else
#	define PROJECT_API
#	define PROJECT_PRIVATE
#endif

#define PROJECT_VERSION "0.0.1"

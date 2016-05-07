// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>

namespace reflection
{
	class Type;
}

namespace core
{
	class RUNTIME_API Object
	{
	public:

		virtual ~Object(void) { }
		virtual reflection::Type GetType(void) const = 0;
	};
} // end namespace core
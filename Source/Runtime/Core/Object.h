/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>

#include <memory>

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

	template<class T>
	class SharedObject : public Object, public std::enable_shared_from_this<T>
	{
	public:
		using SharedPtr = std::shared_ptr<T>;
		using WeakPtr = std::weak_ptr<T>;
	};
} // end namespace core

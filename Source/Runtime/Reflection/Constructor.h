/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>

#include <functional>

namespace reflection
{
	class Any;

	class RUNTIME_API Constructor
	{
	public:
		typedef std::function<Any()> Invoker;

		Constructor(void);
		Constructor(Type classType, Invoker invoker, bool isDynamic);

		Type GetClassType(void) const;

		bool IsValid(void) const;
		bool IsDynamic(void) const;

		Any Invoke(void) const;

	private:

		Type m_ClassType;
		bool m_IsDynamic;

		Invoker m_Invoker;
	};
}

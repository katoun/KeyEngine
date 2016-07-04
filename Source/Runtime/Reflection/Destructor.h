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

	class RUNTIME_API Destructor
	{
	public:
		typedef std::function<void(Any &)> Invoker;

		Destructor(void);
		Destructor(Type classType, Invoker invoker);

		Type GetClassType(void) const;

		bool IsValid(void) const;

		void Invoke(Any &instance) const;

	private:

		Type m_ClassType;

		Invoker m_Invoker;
	};
}

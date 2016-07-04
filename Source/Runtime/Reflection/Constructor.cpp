/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/Constructor.h>
#include <Reflection/Any.h>

#include <cassert>

namespace reflection
{
	Constructor::Constructor(void)
		: m_ClassType({ Type::Invalid })
		, m_IsDynamic(false)
		, m_Invoker(nullptr)
	{ }

	Constructor::Constructor(Type classType, Invoker invoker, bool isDynamic)
		: m_ClassType(classType)
		, m_IsDynamic(isDynamic)
		, m_Invoker(invoker)
	{ }

	Type Constructor::GetClassType(void) const
	{
		return m_ClassType;
	}

	bool Constructor::IsValid(void) const
	{
		return m_Invoker != nullptr;
	}

	bool Constructor::IsDynamic(void) const
	{
		return m_IsDynamic;
	}

	Any Constructor::Invoke(void) const
	{
		assert(IsValid());

		return m_Invoker();
	}
}

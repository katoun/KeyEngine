/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/Any.h>

#include <utility>

namespace reflection
{
	Any Any::Invalid;

	Any::Any(void)
		: m_IsConst(true)
		, m_Type(Type::Invalid)
		, m_Value()
	{}

	Any::Any(const Any &rhs) = default;

	Any::Any(Any &&rhs) noexcept = default;

	Any::~Any(void) = default;

	Any &Any::operator=(Any &&rhs) noexcept = default;

	Any &Any::operator=(const Any &rhs) = default;

	Any::operator bool(void) const
	{
		return IsValid();
	}

	Type Any::GetType(void) const
	{
		return m_Type;
	}

	bool Any::IsValid(void) const
	{
		return m_Value.has_value();
	}

	bool Any::IsConst(void) const
	{
		return m_IsConst;
	}

	void Any::Swap(Any &other)
	{
		std::swap(m_IsConst, other.m_IsConst);
		std::swap(m_Type, other.m_Type);
		std::swap(m_Value, other.m_Value);
	}
}

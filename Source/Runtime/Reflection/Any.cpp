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

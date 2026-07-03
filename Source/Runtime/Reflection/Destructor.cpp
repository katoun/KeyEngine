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

#include <Reflection/Destructor.h>
#include <Reflection/Any.h>

#include <cassert>

namespace reflection
{
	Destructor::Destructor(void)
		: m_ClassType({ Type::Invalid })
		, m_Invoker(nullptr)
	{ }

	Destructor::Destructor(Type classType, Invoker invoker)
		: m_ClassType(classType)
		, m_Invoker(invoker)
	{ }

	Type Destructor::GetClassType(void) const
	{
		return m_ClassType;
	}

	bool Destructor::IsValid(void) const
	{
		return m_Invoker != nullptr;
	}

	void Destructor::Invoke(Any &instance) const
	{
		assert(IsValid());

		assert(m_ClassType == instance.GetType());

		m_Invoker(instance);

		instance.m_Value.reset();
		instance.m_Type = Type::Invalid;
		instance.m_IsConst = true;
	}
}

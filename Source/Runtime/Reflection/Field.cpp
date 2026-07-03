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

#include <Reflection/Field.h>
#include <Reflection/Any.h>
#include <Reflection/Attributes.h>

namespace reflection
{
	Field::Field(void)
		: m_Name("INVALID")
		, m_Type({ Type::Invalid })
		, m_ClassType({ Type::Invalid })
		, m_Getter(nullptr)
		, m_Setter(nullptr)
	{ }

	Field::Field(const std::string &name, Type type, Type classType, const AttributesContainer::Initializer &attributes)
		: m_Name(name)
		, m_Type(type)
		, m_ClassType(classType)
		, m_Getter(nullptr)
		, m_Setter(nullptr)
	{
		SetAttributes(attributes);

		m_Getter = GetAttribute<attribute::Getter>();
		m_Setter = GetAttribute<attribute::Setter>();
	}

	const std::string &Field::GetName(void) const
	{
		return m_Name;
	}

	Type Field::GetType(void) const
	{
		return m_Type;
	}

	Type Field::GetClassType(void) const
	{
		return m_ClassType;
	}

	bool Field::IsValid(void) const
	{
		return m_Getter != nullptr;
	}

	bool Field::IsReadOnly(void) const
	{
		return m_Setter == nullptr;
	}

	Any Field::GetValue(const Any &instance) const
	{
		assert(m_Getter != nullptr);
		return m_Getter->Value(instance);
	}

	bool Field::SetValue(Any &instance, const Any &value) const
	{
		// read only?
		if (m_Setter != nullptr && !instance.IsConst())
		{
			m_Setter->Value(instance, value);

			return true;
		}

		return false;
	}
}
/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

	Field::Field(const std::string &name, Type type, Type classType, const AttibutesContainer::Initializer &attributes)
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
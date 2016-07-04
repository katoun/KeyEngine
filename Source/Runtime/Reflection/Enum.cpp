/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/Enum.h>

namespace reflection
{
	Enum::Enum(void)
		: m_Container(nullptr)
	{}

	Enum::Enum(const Enum::ContainerBase *base)
		: m_Container(base)
	{ }

	bool Enum::IsValid(void) const
	{
		return m_Container != nullptr;
	}

	bool Enum::operator==(const Enum &rhs) const
	{
		return m_Container == rhs.m_Container;
	}

	bool Enum::operator!=(const Enum &rhs) const
	{
		return m_Container != rhs.m_Container;
	}

	std::string Enum::GetName(void) const
	{
		return m_Container != nullptr ? m_Container->GetName() : std::string();
	}

	Type Enum::GetType(void) const
	{
		return m_Container != nullptr ? m_Container->GetType() : Type::Invalid;
	}

	Type Enum::GetParentType(void) const
	{
		return m_Container != nullptr ? m_Container->GetParentType() : Type::Invalid;
	}

	Type Enum::GetUnderlyingType(void) const
	{
		return m_Container != nullptr ? m_Container->GetUnderlyingType() : Type::Invalid;
	}

	std::vector<std::string> Enum::GetKeys(void) const
	{
		return m_Container != nullptr ? m_Container->GetKeys() : std::vector<std::string>();
	}

	std::vector<Any> Enum::GetValues(void) const
	{
		return m_Container != nullptr ? m_Container->GetValues() : std::vector<Any>();
	}

	std::string Enum::GetKey(const Any &value) const
	{
		return m_Container != nullptr ? m_Container->GetKey(value) : std::string();
	}

	Any Enum::GetValue(const std::string &key) const
	{
		return m_Container != nullptr ? m_Container->GetValue(key) : Any();
	}

	Enum::ContainerBase::ContainerBase(const std::string &name, TypeID owner)
		: m_Name(name)
		, m_ParentType(owner)
	{ }

	const std::string &Enum::ContainerBase::GetName(void) const
	{
		return m_Name;
	}

	Type Enum::ContainerBase::GetParentType(void) const
	{
		return m_ParentType;
	}
}
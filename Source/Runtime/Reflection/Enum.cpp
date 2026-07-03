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

#include <Reflection/Enum.h>

#include <utility>

namespace reflection
{
	Enum::Enum(void)
		: m_Container(nullptr)
	{}

	Enum::Enum(std::shared_ptr<const Enum::ContainerBase> base)
		: m_Container(std::move(base))
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
		return m_Container != nullptr ? m_Container->GetType() : Type(Type::Invalid);
	}

	Type Enum::GetParentType(void) const
	{
		return m_Container != nullptr ? m_Container->GetParentType() : Type(Type::Invalid);
	}

	Type Enum::GetUnderlyingType(void) const
	{
		return m_Container != nullptr ? m_Container->GetUnderlyingType() : Type(Type::Invalid);
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

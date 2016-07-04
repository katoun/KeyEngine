/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/TypeData.h>
#include <Reflection/Type.h>
#include <Reflection/Attributes.h>
#include <Reflection/ReflectionManager.h>

namespace reflection
{
	namespace
	{
		// static database instance
		auto &manager = ReflectionManager::Instance();
	}

	TypeData& TypeData::Undefined = TypeData();

	TypeData::TypeData(void)
		: m_IsEnum(false)
		, m_IsPrimitive(false)
		, m_IsPointer(false)
		, m_IsClass(false)
		, m_Name("UNDEFINED")
		, m_Enum{ nullptr }
		, m_Category(nullptr)
	{ }

	TypeData::TypeData(const std::string& name)
		: m_IsEnum(false)
		, m_IsPrimitive(false)
		, m_IsPointer(false)
		, m_IsClass(false)
		, m_Name(name)
		, m_Enum{ nullptr }
		, m_Category(nullptr)
	{ }

	TypeData::TypeData(const std::string &name, const AttibutesContainer::Initializer &attributes)
		: m_IsEnum(false)
		, m_IsPrimitive(false)
		, m_IsPointer(false)
		, m_IsClass(false)
		, m_Name(name)
		, m_Enum{ nullptr }
		, m_Category(nullptr)
	{
		SetAttributes(attributes);

		m_Category = GetAttribute<attribute::Category>();
	}

	TypeData::~TypeData(void)
	{
		if (m_IsEnum)
		{
			SAFE_DELETE(m_Enum.m_Container);
		}
	}

	const std::string& TypeData::GetName(void) const
	{
		return m_Name;
	}

	const bool TypeData::IsEnum(void) const
	{
		return m_IsEnum;
	}

	const bool TypeData::IsPrimitive(void) const
	{
		return m_IsPrimitive;
	}

	const bool TypeData::IsPointer(void) const
	{
		return m_IsPointer;
	}

	const bool TypeData::IsClass(void) const
	{
		return m_IsClass;
	}

	const Enum& TypeData::GetEnum(void)
	{
		return m_Enum;
	}

	const Type::Set& TypeData::GetBaseTypes(void)
	{
		return m_BaseTypes;
	}

	const Constructor& TypeData::GetConstructor(void)
	{
		return m_Constructor;
	}

	const Constructor& TypeData::GetDynamicConstructor(void)
	{
		return m_DynamicConstructor;
	}

	const Destructor& TypeData::GetDestructor(void)
	{
		return m_Destructor;
	}

	const std::string& TypeData::GetCategory(void) const
	{
		if (m_Category == nullptr)
		{
			return attribute::Category::Default;
		}

		return m_Category->Value;
	}

	/*const*/ std::unordered_map<std::string, Field>& TypeData::GetFields(void)
	{
		return m_Fields;
	}

	void TypeData::SetBaseTypes(TypeID id, const std::initializer_list<Type> &types)
	{
		for (auto &type : types)
		{
			// skip invalid types
			if (!type.IsValid())
				continue;

			m_BaseTypes.insert(type);
		}
	}
}
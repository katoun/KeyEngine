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

#include <Reflection/TypeData.h>
#include <Reflection/Any.h>
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

	namespace
	{
		TypeData undefinedTypeData;
	}

	TypeData& TypeData::Undefined = undefinedTypeData;

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

	TypeData::TypeData(const std::string &name, const AttributesContainer::Initializer &attributes)
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

	std::shared_ptr<core::Object> TypeData::CreateDynamicObject(void) const
	{
		assert(m_DynamicObjectFactory != nullptr);

		return m_DynamicObjectFactory();
	}

	Any TypeData::GetDynamicPointer(core::Object& object) const
	{
		assert(m_DynamicPointerCaster != nullptr);

		return m_DynamicPointerCaster(&object);
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

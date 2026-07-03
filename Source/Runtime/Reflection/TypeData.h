/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/Enum.h>
#include <Reflection/Field.h>
#include <Reflection/Constructor.h>
#include <Reflection/Destructor.h>
#include <Reflection/AttributesContainer.h>
#include <Core/Object.h>
#include <Core/Utils.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <cassert>

namespace attribute
{
	struct Category;
}

namespace reflection
{
	class RUNTIME_API TypeData : public AttributesContainer
	{
	public:

		TypeData(void);
		TypeData(const std::string &name);
		TypeData(const std::string &name, const AttributesContainer::Initializer &attributes);

		~TypeData(void);

		template<typename T>
		void Initialize(void);

		template<typename ClassType, typename FieldType>
		void AddField(const std::string &name, const AttributesContainer::Initializer &attributes);

		template<typename EnumType>
		void SetEnum(const std::string &name, const typename Enum::Container<EnumType>::Table &table);

		void SetBaseTypes(const TypeID id, const std::initializer_list<Type> &types);

		const std::string& GetName(void) const;
		const bool IsEnum(void) const;
		const bool IsPrimitive(void) const;
		const bool IsPointer(void) const;
		const bool IsClass(void) const;

		const Enum& GetEnum(void);

		const Type::Set& GetBaseTypes(void);

		const Constructor& GetConstructor(void);
		const Constructor& GetDynamicConstructor(void);
		const Constructor& GetDynamicObjectConstructor(void);
		Any GetDynamicPointer(core::Object& object) const;

		const Destructor& GetDestructor(void);

		const std::string& GetCategory(void) const;

		/*const*/ std::unordered_map<std::string, Field>& GetFields();

		static TypeData& Undefined;

	private:

		friend class Type;

		template<typename T>
		void SetDefaultConstructor(void);

		template<typename T>
		void SetDefaultDestructor(void);

		std::string m_Name;

		bool m_IsEnum;
		bool m_IsPrimitive;
		bool m_IsPointer;
		bool m_IsClass;

		Enum m_Enum;

		Type::Set m_BaseTypes;

		Constructor m_Constructor;
		Constructor m_DynamicConstructor;
		Constructor m_DynamicObjectConstructor;
		std::function<Any(core::Object*)> m_DynamicPointerCaster;

		Destructor m_Destructor;

		std::shared_ptr<const attribute::Category> m_Category;

		std::unordered_map<std::string, Field> m_Fields;
	};

	struct RegisteredTypeData
	{
		TypeID ID;
		TypeData Data;
	};

	template<typename T>
	RegisteredTypeData RegisterClass(const std::string &name)
	{
		auto id = core::string::Hash(name);
		TypeInfo<T>::Register(id);

		auto data = TypeData(name);
		data.Initialize<T>();

		return { id, data };
	}

	template<typename T>
	RegisteredTypeData RegisterClass(const std::string &name, const AttributesContainer::Initializer &attributes)
	{
		auto id = core::string::Hash(name);
		TypeInfo<T>::Register(id);

		auto data = TypeData(name, attributes);
		data.Initialize<T>();

		return { id, data };
	}

	template<typename T>
	void TypeData::Initialize()
	{
		using Decayed = std::decay_t<T>;

		m_IsClass = std::is_class_v<Decayed>;
		m_IsEnum = std::is_enum_v<Decayed>;
		m_IsPointer = std::is_pointer_v<T>;
		m_IsPrimitive = std::is_arithmetic_v<Decayed>;

		SetDefaultConstructor<T>();
		SetDefaultDestructor<T>();
	}

	template<typename ClassType, typename FieldType>
	void TypeData::AddField(const std::string &name, const AttributesContainer::Initializer &attributes)
	{
		Field field{ name, TypeOf<FieldType>(), TypeOf<ClassType>(), attributes };

		m_Fields.emplace(name, field);
	}

	template<typename EnumType>
	void TypeData::SetEnum(const std::string &name, const typename Enum::Container<EnumType>::Table &table)
	{
		m_Enum = { std::make_shared<Enum::Container<EnumType>>(name, table) };
	}

	template<typename T>
	void TypeData::SetDefaultConstructor(void)
	{
		if constexpr (std::is_default_constructible_v<T>)
		{
			m_DynamicConstructor = { TypeOf<T>(), []() { return Any{ std::make_unique<T>().release() }; }, true };

			if constexpr (std::is_copy_constructible_v<T>)
			{
				m_Constructor = { TypeOf<T>(), []() { return Any{ T() }; }, false };
			}

			if constexpr (std::is_base_of_v<core::Object, T>)
			{
				m_DynamicObjectConstructor = { TypeOf<T>(), []() { return Any{ static_cast<core::Object*>(std::make_unique<T>().release()) }; }, true };
				m_DynamicPointerCaster = [](core::Object* object) { return Any{ dynamic_cast<T*>(object) }; };
			}
		}
	}

	template<typename T>
	void TypeData::SetDefaultDestructor(void)
	{
		if constexpr (std::is_destructible_v<T>)
		{
			m_Destructor = { TypeOf<T>(), [](Any &) {} };
		}
	}
}

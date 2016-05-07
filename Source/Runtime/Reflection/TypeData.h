// Copyright (c) 2016 Catalin Alexandru Nastase

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
	class RUNTIME_API TypeData : public AttibutesContainer
	{
	public:

		TypeData(void);
		TypeData(const std::string &name);
		TypeData(const std::string &name, const AttibutesContainer::Initializer &attributes);

		~TypeData(void);

		template<typename T>
		void Initialize(void);

		template<typename ClassType, typename FieldType>
		void AddField(const std::string &name, const AttibutesContainer::Initializer &attributes);

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

		const Destructor& GetDestructor(void);

		const std::string& GetCategory(void) const;

		/*const*/ std::unordered_map<std::string, Field>& GetFields();

		static TypeData& Undefined;

	private:

		friend class Type;

		template<typename T>
		void SetDefaultConstructor(typename std::enable_if<!std::is_default_constructible<T>::value>::type* = nullptr);

		template<typename T>
		void SetDefaultConstructor(typename std::enable_if<std::is_default_constructible<T>::value>::type* = nullptr);

		template<typename T>
		void SetDefaultDestructor(typename std::enable_if<!std::is_destructible<T>::value>::type* = nullptr);

		template<typename T>
		void SetDefaultDestructor(typename std::enable_if<std::is_destructible<T>::value>::type* = nullptr);

		std::string m_Name;

		bool m_IsEnum;
		bool m_IsPrimitive;
		bool m_IsPointer;
		bool m_IsClass;

		Enum m_Enum;

		Type::Set m_BaseTypes;

		Constructor m_Constructor;
		Constructor m_DynamicConstructor;

		Destructor m_Destructor;

		const attribute::Category* m_Category;

		std::unordered_map<std::string, Field> m_Fields;
	};

	template<typename T>
	void TypeData::Initialize()
	{
		typedef std::decay<T> Decayed;

		m_IsClass = std::is_class< Decayed >::value;
		m_IsEnum = std::is_enum< Decayed >::value;
		m_IsPointer = std::is_pointer< T >::value;
		m_IsPrimitive = std::is_arithmetic< Decayed >::value;

		SetDefaultConstructor<T>();
		SetDefaultDestructor<T>();
	}

	template<typename ClassType, typename FieldType>
	void TypeData::AddField(const std::string &name, const AttibutesContainer::Initializer &attributes)
	{
		Field field{ name, typeof(FieldType), typeof(ClassType), attributes };

		m_Fields.emplace(name, field);
	}

	template<typename EnumType>
	void TypeData::SetEnum(const std::string &name, const typename Enum::Container<EnumType>::Table &table)
	{
		m_Enum = { new Enum::Container<EnumType>(name, table) };
	}

#pragma warning(push)

	// unused template parameters
#pragma warning(disable : 4544)

	template<typename T>
	void TypeData::SetDefaultConstructor(typename std::enable_if<!std::is_default_constructible<T>::value>::type*)
	{
		// do nothing
	}

	template<typename T>
	void TypeData::SetDefaultConstructor(typename std::enable_if<std::is_default_constructible<T>::value>::type*)
	{
		m_Constructor = { typeof(T), []() { return Any{ T() }; }, false };
		m_DynamicConstructor = { typeof(T), []() { return Any{ new T() }; }, true };
	}

	template<typename T>
	void TypeData::SetDefaultDestructor(typename std::enable_if<!std::is_destructible<T>::value>::type*)
	{
		// do nothing
	}

	template<typename T>
	void TypeData::SetDefaultDestructor(typename std::enable_if<std::is_destructible<T>::value>::type*)
	{
		m_Destructor = { typeof(T), [](Any &instance) { instance.GetValue<T>().~T(); } };
	}

#pragma warning(pop)
}
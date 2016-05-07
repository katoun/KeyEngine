// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>

#include <functional>
#include <string>
#include <vector>
#include <set>

namespace reflection
{
	class Any;
	class Field;
	struct Attribute;

	class RUNTIME_API Type
	{
	public:
		typedef std::vector<Type> List;
		typedef std::set<Type> Set;

		static const TypeID Invalid = 0;

		Type(void);
		Type(const Type &rhs);
		Type(TypeID id);

		operator bool(void) const;

		Type &operator=(const Type &rhs);

		bool operator<(const Type &rhs) const;
		bool operator>(const Type &rhs) const;
		bool operator<=(const Type &rhs) const;
		bool operator>=(const Type &rhs) const;
		bool operator==(const Type &rhs) const;
		bool operator!=(const Type &rhs) const;

		TypeID GetID(void) const;

		const std::string &GetName(void) const;

		bool IsValid(void) const;
		bool IsPrimitive(void) const;
		bool IsEnum(void) const;
		bool IsPointer(void) const;
		bool IsClass(void) const;

		Any Create() const;
		Any CreateDynamic() const;
		void Destroy(Any &instance) const;

		template<typename AttributeType>
		const AttributeType* GetAttribute(void) const;

		std::vector<std::pair<Type, const Attribute *>> GetAttributes(void) const;

		const Field &GetField(const std::string &name) const;
		std::vector<Field> GetFields(void) const;

		bool IsDerivedFrom(const Type &other) const;
		template<typename T>
		bool IsDerivedFrom(void) const;

		bool IsTypeOf(const Type &other) const;
		template<typename T>
		bool IsTypeOf(void) const;

		const Set &GetBaseClasses(void) const;

	private:

		friend class std::allocator<Type>;

		friend class TypeData;
		friend class Any;
		friend class Field;

		TypeID m_ID;
	};

	template<typename AttributeType>
	const AttributeType* Type::GetAttribute(void) const
	{
		return ReflectionManager::Instance().GetTypeData(m_ID).GetAttribute<AttributeType>();
	}

	template<typename T>
	bool Type::IsDerivedFrom(void) const
	{
		return IsDerivedFrom(typeof(T));
	}

	template<typename T>
	bool Type::IsTypeOf(void) const
	{
		return IsTypeOf(typeof(T));
	}
}

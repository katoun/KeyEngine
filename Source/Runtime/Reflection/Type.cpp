// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Reflection/Type.h>
#include <Reflection/ReflectionManager.h>

#include <cassert>

namespace reflection
{
	namespace
	{
		// static database instance
		auto &manager = ReflectionManager::Instance();
	}

	Type::Type(void)
		: m_ID(Invalid)
	{ }

	Type::Type(const Type &other)
		: m_ID(other.m_ID)
	{ }

	Type::Type(TypeID id)
		: m_ID(id)
	{ }

	Type::operator bool(void) const
	{
		return m_ID != Invalid;
	}

	Type &Type::operator=(const Type &rhs)
	{
		m_ID = rhs.m_ID;

		return *this;
	}

	bool Type::operator<(const Type &rhs) const
	{
		return m_ID < rhs.m_ID;
	}

	bool Type::operator>(const Type &rhs) const
	{
		return m_ID > rhs.m_ID;
	}

	bool Type::operator<=(const Type &rhs) const
	{
		return m_ID <= rhs.m_ID;
	}

	bool Type::operator>=(const Type &rhs) const
	{
		return m_ID >= rhs.m_ID;
	}

	bool Type::operator==(const Type &rhs) const
	{
		return m_ID == rhs.m_ID;
	}

	bool Type::operator!=(const Type &rhs) const
	{
		return m_ID != rhs.m_ID;
	}

	TypeID Type::GetID(void) const
	{
		return m_ID;
	}

	const std::string &Type::GetName(void) const
	{
		return manager.GetTypeData(m_ID).GetName();
	}

	bool Type::IsValid(void) const
	{
		return m_ID != Invalid;
	}

	bool Type::IsPrimitive(void) const
	{
		return manager.GetTypeData(m_ID).IsPrimitive();
	}

	bool Type::IsEnum(void) const
	{
		return manager.GetTypeData(m_ID).IsEnum();
	}

	bool Type::IsPointer(void) const
	{
		return manager.GetTypeData(m_ID).IsPointer();
	}

	bool Type::IsClass(void) const
	{
		return manager.GetTypeData(m_ID).IsClass();
	}

	Any Type::Create() const
	{
		auto &constructor = manager.GetTypeData(m_ID).GetConstructor();
		assert(constructor.IsValid());

		return constructor.Invoke();
	}

	Any Type::CreateDynamic() const
	{
		auto &constructor = manager.GetTypeData(m_ID).GetDynamicConstructor();
		assert(constructor.IsValid());

		return constructor.Invoke();
	}

	void Type::Destroy(Any &instance) const
	{
		auto &destructor = manager.GetTypeData(m_ID).GetDestructor();
		assert(destructor.IsValid());
		
		destructor.Invoke(instance);
	}

	std::vector<std::pair<Type, const Attribute *>> Type::GetAttributes(void) const
	{
		return manager.GetTypeData(m_ID).GetAttributes();
	}

	const Field &Type::GetField(const std::string &name) const
	{
		auto &fields = manager.GetTypeData(m_ID).GetFields();
		return fields[name];
	}

	std::vector<Field> Type::GetFields(void) const
	{
		std::vector<Field> fields;

		auto &basetypes = manager.GetTypeData(m_ID).GetBaseTypes();

		for (auto& basetype : basetypes)
		{
			if (!basetype.IsValid())
				continue;

			auto& basefields = basetype.GetFields();
			
			for (auto &field : basefields)
			{
				fields.emplace_back(field);
			}
		}

		auto &fieldsmap = manager.GetTypeData(m_ID).GetFields();

		for (auto &pair : fieldsmap)
		{
			fields.emplace_back(pair.second);
		}

		return fields;
	}

	bool Type::IsDerivedFrom(const Type &other) const
	{
		auto &basetypes = manager.GetTypeData(m_ID).GetBaseTypes();

		return basetypes.find(other) != basetypes.end();
	}

	bool Type::IsTypeOf(const Type &other) const
	{
		auto &basetypes = manager.GetTypeData(m_ID).GetBaseTypes();

		if (basetypes.empty())
			return false;

		bool result = false;

		for (auto& basetype : basetypes)
		{
			if (!basetype.IsValid())
				continue;

			if (basetype == other)
				return true;

			result |= basetype.IsTypeOf(other);
		}

		return result;
	}

	const Type::Set &Type::GetBaseClasses(void) const
	{
		return manager.GetTypeData(m_ID).GetBaseTypes();
	}
}
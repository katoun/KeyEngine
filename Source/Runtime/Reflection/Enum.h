// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/Any.h>

#include <vector>
#include <unordered_map>
#include <string>

namespace reflection
{
	class RUNTIME_API Enum
	{
	public:

		class ContainerBase;

		Enum(void);

	private:

		Enum(const ContainerBase * container);

	public:

		bool IsValid(void) const;

		bool operator ==(const Enum &rhs) const;
		bool operator !=(const Enum &rhs) const;

		std::string GetName(void) const;

		Type GetType(void) const;
		Type GetParentType(void) const;
		Type GetUnderlyingType(void) const;

		std::vector<std::string> GetKeys(void) const;
		std::vector<Any> GetValues(void) const;

		std::string GetKey(const Any &value) const;
		Any GetValue(const std::string &key) const;

		class RUNTIME_API ContainerBase
		{
		public:

			ContainerBase(const std::string &name, TypeID owner);

			virtual ~ContainerBase(void) { }

			virtual Type GetType(void) const = 0;
			virtual Type GetUnderlyingType(void) const = 0;

			virtual std::vector<std::string> GetKeys(void) const = 0;
			virtual std::vector<Any> GetValues(void) const = 0;

			virtual std::string GetKey(const Any &value) const = 0;
			virtual Any GetValue(const std::string &key) const = 0;

			const std::string &GetName(void) const;

			Type GetParentType(void) const;

		private:

			std::string m_Name;

			Type m_ParentType;
		};

		template<typename T>
		class RUNTIME_TEMPLATE Container : public ContainerBase
		{
		public:
			typedef std::unordered_map<std::string, T> Table;

			Container(const std::string &name, const Table &table);
			Container(const std::string &name, const Table &table, TypeID owner);

			Type GetType(void) const override;
			Type GetUnderlyingType(void) const override;

			std::vector<std::string> GetKeys(void) const override;
			std::vector<Any> GetValues(void) const override;

			std::string GetKey(const Any &value) const override;
			Any GetValue(const std::string &key) const override;

		private:

			Table m_Table;
		};

	private:
		friend class TypeData;

		const ContainerBase* m_Container;
	};

	template<typename T>
	Enum::Container<T>::Container(const std::string &name, const Table &table)
		: Enum::ContainerBase(name, Type::Invalid)
		, m_Table(table)
	{ }

	template<typename T>
	Enum::Container<T>::Container(const std::string &name, const Table &table, TypeID owner)
		: Enum::ContainerBase(name, owner)
		, m_Table(table)
	{ }

	template<typename T>
	Type Enum::Container<T>::GetType(void) const
	{
		return typeof(T);
	}

	template<typename T>
	Type Enum::Container<T>::GetUnderlyingType(void) const
	{
		return typeof(typename std::underlying_type< T >::type);
	}

	template<typename T>
	std::vector<std::string> Enum::Container<T>::GetKeys(void) const
	{
		std::vector<std::string> keys;

		for (auto &entry : m_table)
			keys.emplace_back(entry.first);

		return keys;
	}

	template<typename T>
	std::vector<Any> Enum::Container<T>::GetValues(void) const
	{
		std::vector<Any> values;

		for (auto &entry : m_table)
			values.emplace_back(entry.second);

		return values;
	}

	template<typename T>
	std::string Enum::Container<T>::GetKey(const Any &value) const
	{
		typedef typename std::underlying_type< T >::type UnderlyingType;

		auto type = value.GetType();

		// invalid type
		if (type != typeof(T) && type != typeof(UnderlyingType))
		{
			return std::string();
		}

		auto &converted = value.GetValue<T>();

		for (auto &entry : m_table)
		{
			if (entry.second == converted)
				return entry.first;
		}

		return std::string();
	}

	template<typename T>
	Any Enum::Container<T>::GetValue(const std::string &key) const
	{
		auto search = m_Table.find(key);

		// not found
		if (search == m_Table.end())
			return Any();

		return Any(search->second);
	}
}

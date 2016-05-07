// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>

#include <type_traits>

namespace reflection
{
	template<typename T>
	class RUNTIME_TEMPLATE TypeInfo
	{
	public:

		static void Register(TypeID id);

		static const TypeID& GetID();

	private:

		static TypeID m_ID;
	};

	template<typename T>
	TypeID TypeInfo<T>::m_ID = Type::Invalid;

	template<typename T>
	void TypeInfo<T>::Register(TypeID id)
	{
		m_ID = id;
	}

	template<typename T>
	const TypeID& TypeInfo<T>::GetID()
	{
		return m_ID;
	}
}

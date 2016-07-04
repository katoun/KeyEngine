/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef _ANY_H_
#define _ANY_H_

#include <RuntimeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/TypeInfo.h>

#include <any>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>

namespace reflection
{
	class RUNTIME_API Any
	{
	public:

		static Any Invalid;

		Any(void);
		Any(const Any &rhs);
		Any(Any &&rhs) noexcept;

		~Any(void);

		template<typename T>
		Any(T &data);

		template<typename T>
			requires (!std::same_as<CleanedType<T>, Any> && !std::is_const_v<std::remove_reference_t<T>>)
		Any(T &&data);

		Any &operator=(Any &&rhs) noexcept;
		Any &operator=(const Any &rhs);

		template<typename T>
			requires (!std::same_as<CleanedType<T>, Any>)
		Any &operator=(T &&rhs);

		operator bool(void) const;

		Type GetType(void) const;

		bool IsValid(void) const;
		bool IsConst(void) const;

		void Swap(Any &other);

		template<typename T>
		T &GetValue(void) const;

	private:

		friend class Destructor;

		bool m_IsConst;
		Type m_Type;
		mutable std::any m_Value;
	};

	template<typename T>
	Any::Any(T &data)
		: m_IsConst(std::is_const_v<std::remove_reference_t<T>>)
		, m_Type(TypeOf<T>())
		, m_Value(CleanedType<T>(data))
	{}

	template<typename T>
		requires (!std::same_as<CleanedType<T>, Any> && !std::is_const_v<std::remove_reference_t<T>>)
	Any::Any(T &&data)
		: m_IsConst(false)
		, m_Type(TypeOf<T>())
		, m_Value(CleanedType<T>(std::forward<T>(data)))
	{}

	template<typename T>
		requires (!std::same_as<CleanedType<T>, Any>)
	Any &Any::operator=(T &&rhs)
	{
		Any(std::forward<T>(rhs)).Swap(*this);

		return *this;
	}

	template<typename T>
	T &Any::GetValue(void) const
	{
		using StoredType = CleanedType<T>;

		auto value = std::any_cast<StoredType>(&m_Value);
		assert(value != nullptr);

		return *value;
	}
}

#endif

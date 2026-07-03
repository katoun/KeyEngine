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

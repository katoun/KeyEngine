/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef _ANY_H_
#define _ANY_H_

#include <RuntimeConfig.h>
#include <Core/Object.h>
#include <Reflection/Type.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/TypeInfo.h>

#include <cassert>
#include <concepts>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#define DECLARE_STANDARD_VARIANTS(type)								\
	template<>														\
	class RUNTIME_API Any::Container<type> : public Any::ContainerBase	\
	{																\
	public:															\
																	\
		Container(const type &value);								\
		Container(type &&value);									\
																	\
		Type GetType(void) const override;							\
																	\
		void *GetPtr(void) const override;							\
																	\
		std::unique_ptr<Any::ContainerBase> Clone(void) const override; \
																	\
	private:														\
																	\
		friend class Any;											\
																	\
		Container &operator=(const Container &rhs) = delete;		\
																	\
		type m_Value;												\
	};																\

namespace reflection
{
	class Type;

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

		class RUNTIME_API ContainerBase
		{
		public:

			virtual ~ContainerBase(void) { }

			virtual Type GetType(void) const = 0;

			virtual void *GetPtr(void) const = 0;

			virtual std::unique_ptr<ContainerBase> Clone(void) const = 0;
		};

		template<typename T>
		class RUNTIME_TEMPLATE Container : public ContainerBase
		{
		public:

			Container(const T &value);
			Container(T &&value);

			Type GetType(void) const override;

			void *GetPtr(void) const override;

			std::unique_ptr<ContainerBase> Clone(void) const override;

		private:

			friend class Any;

			Container &operator=(const Container &rhs) = delete;

			T m_Value;
		};

	private:

		friend class Destructor;

		bool m_IsConst;

		std::unique_ptr<ContainerBase> m_Container;
	};

	template<>
	class RUNTIME_API Any::Container<void> : public Any::ContainerBase
	{
	public:

		Container(void);

		Type GetType(void) const override;

		void *GetPtr(void) const override;

		std::unique_ptr<Any::ContainerBase> Clone(void) const override;

	private:

		friend class Any;

		Container &operator=(const Container &rhs) = delete;
	};

	DECLARE_STANDARD_VARIANTS(int)
	DECLARE_STANDARD_VARIANTS(bool)
	DECLARE_STANDARD_VARIANTS(float)
	DECLARE_STANDARD_VARIANTS(double)
	DECLARE_STANDARD_VARIANTS(std::string)

	template<typename T>
	Any::Any(T &data)
		: m_IsConst(std::is_const_v<T>)
		, m_Container(std::make_unique<Container<CleanedType<T>>>(data))
	{}

	template<typename T>
		requires (!std::same_as<CleanedType<T>, Any> && !std::is_const_v<std::remove_reference_t<T>>)
	Any::Any(T &&data)
		: m_IsConst(false)
		, m_Container(std::make_unique<Container<CleanedType<T>>>(std::forward<T>(data)))
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
		assert(m_Container != nullptr);

		return *static_cast<T*>(m_Container->GetPtr());
	}

	//Any::Container
	template<typename T>
	Any::Container<T>::Container(const T &value) : m_Value(value)
	{}

	template<typename T>
	Any::Container<T>::Container(T &&value) : m_Value(std::move(value))
	{}

	template<typename T>
	Type Any::Container<T>::GetType(void) const
	{
		return TypeOf<T>();
	}

	template<typename T>
	void *Any::Container<T>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	template<typename T>
	std::unique_ptr<Any::ContainerBase> Any::Container<T>::Clone(void) const
	{
		return std::make_unique<Any::Container<T>>(m_Value);
	}

}

#endif

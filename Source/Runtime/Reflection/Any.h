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

#include <string>
#include <cassert>

#define DECLARE_STANDARD_VARIANTS(type)								\
	template<>														\
	class RUNTIME_API Container<type> : public ContainerBase		\
	{																\
	public:															\
																	\
		Container(const type &value);								\
		Container(const type &&value);								\
																	\
		Type GetType(void) const override;							\
																	\
		void *GetPtr(void) const override;							\
																	\
		ContainerBase *Clone(void) const override;					\
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
		Any(Any &&rhs);

		~Any(void);

		/*template<typename T>
		Any(T *data, typename std::enable_if<std::is_base_of<core::Object, T>::value>::type* = nullptr);*/

		template<typename T>
		Any(T &data);

		// non-const r-value references, excluding other variants
		template<typename T>
		Any(T &&data, typename std::enable_if<!std::is_same<Any&, T>::value>::type* = nullptr, typename std::enable_if<!std::is_const<T>::value>::type* = nullptr);

		Any &operator=(Any &&rhs);
		Any &operator=(const Any &rhs);

		template<typename T>
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

			virtual ContainerBase *Clone(void) const = 0;
		};

		template<typename T>
		class RUNTIME_TEMPLATE Container : public ContainerBase
		{
		public:

			Container(const T &value);
			Container(const T &&value);

			Type GetType(void) const override;

			void *GetPtr(void) const override;

			ContainerBase *Clone(void) const override;

		private:

			friend class Any;

			Container &operator=(const Container &rhs) = delete;

			T m_Value;
		};

		template<>
		class RUNTIME_API Container<void> : public ContainerBase
		{
		public:

			Container(void);

			Type GetType(void) const override;

			void *GetPtr(void) const override;

			ContainerBase *Clone(void) const override;

		private:

			friend class Any;

			Container &operator=(const Container &rhs) = delete;
		};

		/*class RUNTIME_API ObjectContainer : public ContainerBase
		{
		public:

			ObjectContainer(core::Object* instance);

			Type GetType(void) const override;

			void *GetPtr(void) const override;

			ContainerBase *Clone(void) const override;

		private:

			core::Object* m_Instance;
		};*/

		DECLARE_STANDARD_VARIANTS(int)
		DECLARE_STANDARD_VARIANTS(bool)
		DECLARE_STANDARD_VARIANTS(float)
		DECLARE_STANDARD_VARIANTS(double)
		DECLARE_STANDARD_VARIANTS(std::string)

	private:

		friend class Destructor;

		bool m_IsConst;

		ContainerBase *m_Container;
	};

	/*template <typename T>
	Any::Any(T *data, typename std::enable_if<std::is_base_of<core::Object, T>::value>::type*)
		: m_IsConst(std::is_const<T>::value)
		, m_Container(new ObjectContainer(static_cast<core::Object *>(const_cast<typename std::remove_const<T>::type*>(data))))
	{}*/

	template<typename T>
	Any::Any(T &data)
		: m_IsConst(std::is_pointer<T>::value && std::is_const<T>::value)
		, m_Container(new Container< CleanedType<T> >(data))
	{}

	template<typename T>
	Any::Any(T &&data, typename std::enable_if< !std::is_same<Any&, T>::value >::type*, typename std::enable_if<!std::is_const<T>::value>::type*)
		: m_IsConst(false)
		, m_Container(new Container< CleanedType<T> >(static_cast<T&&>(data)))
	{}

	template<typename T>
	Any &Any::operator=(T &&rhs)
	{
		Any(static_cast<T&&>(rhs)).Swap(*this);

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
	Any::Container<T>::Container(const T &&value) : m_Value(std::move(value))
	{}

	template<typename T>
	Type Any::Container<T>::GetType(void) const
	{
		return typeof(T);
	}

	template<typename T>
	void *Any::Container<T>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	template<typename T>
	Any::ContainerBase *Any::Container<T>::Clone(void) const
	{
		return new Any::Container<T>(m_Value);
	}

	//Any::ObjectContainer
	/*Any::ObjectContainer::ObjectContainer(core::Object* instance) : m_Instance(instance)
	{}

	Type Any::ObjectContainer::GetType(void) const
	{
		return m_Instance->GetType();
	}

	void *Any::ObjectContainer::GetPtr(void) const
	{
		return m_Instance;
	}

	Any::ContainerBase *Any::ObjectContainer::Clone(void) const
	{
		return new ObjectContainer(m_Instance);
	}*/
}

#endif

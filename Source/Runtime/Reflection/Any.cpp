/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/Any.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>

#include <memory>
#include <utility>

namespace reflection
{
	Any Any::Invalid;

	Any::Any(void)
		: m_IsConst(true)
		, m_Container(nullptr)
	{}

	Any::Any(const Any &rhs)
		: m_IsConst(rhs.m_IsConst)
		, m_Container(rhs.m_Container != nullptr ? rhs.m_Container->Clone() : nullptr)
	{}

	Any::Any(Any &&rhs) noexcept
		: m_IsConst(rhs.m_IsConst)
		, m_Container(std::move(rhs.m_Container))
	{
		rhs.m_IsConst = true;
	}

	Any::~Any(void) = default;

	Any &Any::operator=(Any &&rhs) noexcept
	{
		Any(std::move(rhs)).Swap(*this);

		return *this;
	}

	Any &Any::operator=(const Any &rhs)
	{
		Any(rhs).Swap(*this);

		return *this;
	}

	Any::operator bool(void) const
	{
		return m_Container != nullptr;
	}

	Type Any::GetType(void) const
	{
		return (m_Container != nullptr ? m_Container->GetType() : Type(Type::Invalid));
	}

	bool Any::IsValid(void) const
	{
		return m_Container != nullptr;
	}

	bool Any::IsConst(void) const
	{
		return m_IsConst;
	}

	void Any::Swap(Any &other)
	{
		std::swap(m_IsConst, other.m_IsConst);
		std::swap(m_Container, other.m_Container);
	}

	//Any::Container<void>
	Any::Container<void>::Container(void)
	{}

	Type Any::Container<void>::GetType(void) const
	{
		return typeof(void);
	}

	void *Any::Container<void>::GetPtr(void) const
	{
		return nullptr;
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<void>::Clone(void) const
	{
		return std::make_unique<Any::Container<void>>();
	}

	//Any::Container<int>
	Any::Container<int>::Container(const int &value) : m_Value(value)
	{}

	Any::Container<int>::Container(int &&value) : m_Value(std::move(value))
	{}

	Type Any::Container<int>::GetType(void) const
	{
		return typeof(int);
	}

	void *Any::Container<int>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<int>::Clone(void) const
	{
		return std::make_unique<Any::Container<int>>(m_Value);
	}

	//Any::Container<bool>
	Any::Container<bool>::Container(const bool &value) : m_Value(value)
	{}

	Any::Container<bool>::Container(bool &&value) : m_Value(std::move(value))
	{}

	Type Any::Container<bool>::GetType(void) const
	{
		return typeof(bool);
	}

	void *Any::Container<bool>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<bool>::Clone(void) const
	{
		return std::make_unique<Any::Container<bool>>(m_Value);
	}

	//Any::Container<float>
	Any::Container<float>::Container(const float &value) : m_Value(value)
	{}

	Any::Container<float>::Container(float &&value) : m_Value(std::move(value))
	{}

	Type Any::Container<float>::GetType(void) const
	{
		return typeof(float);
	}

	void *Any::Container<float>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<float>::Clone(void) const
	{
		return std::make_unique<Any::Container<float>>(m_Value);
	}

	//Any::Container<float>
	Any::Container<double>::Container(const double &value) : m_Value(value)
	{}

	Any::Container<double>::Container(double &&value) : m_Value(std::move(value))
	{}

	Type Any::Container<double>::GetType(void) const
	{
		return typeof(double);
	}

	void *Any::Container<double>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<double>::Clone(void) const
	{
		return std::make_unique<Any::Container<double>>(m_Value);
	}

	//Any::Container<std::string>
	Any::Container<std::string>::Container(const std::string &value) : m_Value(value)
	{}

	Any::Container<std::string>::Container(std::string &&value) : m_Value(std::move(value))
	{}

	Type Any::Container<std::string>::GetType(void) const
	{
		return typeof(std::string);
	}

	void *Any::Container<std::string>::GetPtr(void) const
	{
		return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(m_Value)));
	}

	std::unique_ptr<Any::ContainerBase> Any::Container<std::string>::Clone(void) const
	{
		return std::make_unique<Any::Container<std::string>>(m_Value);
	}
}

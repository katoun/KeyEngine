// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Reflection/Destructor.h>
#include <Reflection/Any.h>

#include <cassert>

namespace reflection
{
	Destructor::Destructor(void)
		: m_ClassType({ Type::Invalid })
		, m_Invoker(nullptr)
	{ }

	Destructor::Destructor(Type classType, Invoker invoker)
		: m_ClassType(classType)
		, m_Invoker(invoker)
	{ }

	Type Destructor::GetClassType(void) const
	{
		return m_ClassType;
	}

	bool Destructor::IsValid(void) const
	{
		return m_Invoker != nullptr;
	}

	void Destructor::Invoke(Any &instance) const
	{
		assert(IsValid());

		assert(m_ClassType == instance.GetType());

		m_Invoker(instance);

		delete instance.m_Container;

		instance.m_Container = nullptr;
	}
}
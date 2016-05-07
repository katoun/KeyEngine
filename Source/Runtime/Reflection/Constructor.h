// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>

#include <functional>

namespace reflection
{
	class Any;

	class RUNTIME_API Constructor
	{
	public:
		typedef std::function<Any()> Invoker;

		Constructor(void);
		Constructor(Type classType, Invoker invoker, bool isDynamic);

		Type GetClassType(void) const;

		bool IsValid(void) const;
		bool IsDynamic(void) const;

		Any Invoke(void) const;

	private:

		Type m_ClassType;
		bool m_IsDynamic;

		Invoker m_Invoker;
	};
}

// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>

#include <functional>

namespace reflection
{
	class Any;

	class RUNTIME_API Destructor
	{
	public:
		typedef std::function<void(Any &)> Invoker;

		Destructor(void);
		Destructor(Type classType, Invoker invoker);

		Type GetClassType(void) const;

		bool IsValid(void) const;

		void Invoke(Any &instance) const;

	private:

		Type m_ClassType;

		Invoker m_Invoker;
	};
}

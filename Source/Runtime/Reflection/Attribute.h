// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>

#include <utility>

namespace reflection
{
	struct RUNTIME_API Attribute
	{};

	template<typename AttributeType, typename ...Args>
	std::pair<Type, const Attribute *> AttributeInitializer(Args&&... args);

	template<typename AttributeType, typename ...Args>
	std::pair<Type, const Attribute *> AttributeInitializer(Args&&... args)
	{
		static_assert(std::is_base_of<Attribute, AttributeType>::value, "Attributes must inherit from Attribute struct");

		return std::make_pair(typeof(AttributeType), new AttributeType(std::forward<Args>(args)...));
	}
}

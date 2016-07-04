/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

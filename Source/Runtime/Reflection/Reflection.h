/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifndef __REFLECTION_PARSER__

#include <Reflection/Any.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/Constructor.h>
#include <Reflection/Destructor.h>
#include <Reflection/Field.h>
#include <Reflection/Enum.h>
#include <Reflection/Attribute.h>
#include <Reflection/Attributes.h>

#define ATTRIBUTES(...)

#define DEFINE_OBJECT								\
	public:											\
	reflection::Type GetType(void) const override	\
	{												\
		return typeof( decltype(*this) );			\
	}												\
	private:										\

#else

#define ATTRIBUTES(...) __attribute__((annotate(#__VA_ARGS__)))

#define DEFINE_OBJECT

#endif

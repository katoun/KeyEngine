// Copyright (c) 2016 Catalin Alexandru Nastase

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

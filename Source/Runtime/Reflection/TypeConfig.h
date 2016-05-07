// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <cstdint>
#include <type_traits>

// converts the type name into a reflection::Type instance
#define typeof(type) reflection::Type(reflection::TypeInfo<reflection::CleanedType< type >>::GetID())

namespace reflection
{
	template<typename T>
	using CleanedType = typename std::remove_cv<typename std::remove_reference< T >::type>::type;

	typedef uint32_t TypeID;
}
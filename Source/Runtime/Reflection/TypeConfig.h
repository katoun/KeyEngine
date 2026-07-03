/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <cstdint>
#include <type_traits>

// converts the type name into a reflection::Type instance
#define typeof(type) reflection::Type(reflection::TypeInfo<reflection::CleanedType< type >>::GetID())

namespace reflection
{
	template<typename T>
	using CleanedType = std::remove_cvref_t<T>;

	using TypeID = uint32_t;
}

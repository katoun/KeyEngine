/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Attribute.h>

#include <functional>
#include <string>

namespace attribute
{
	struct RUNTIME_API Getter : public reflection::Attribute
	{
		typedef std::function<reflection::Any(const reflection::Any &)> FuctionType;

		FuctionType Value;

		Getter(FuctionType value) : Value(value) {};
	};

	struct RUNTIME_API Setter : public reflection::Attribute
	{
		typedef std::function<void(reflection::Any &, const reflection::Any &)> FuctionType;

		FuctionType Value;

		Setter(FuctionType value) : Value(value) {};
	};

	struct RUNTIME_API DisplayName : public reflection::Attribute
	{
		static std::string Default;

		std::string Value;

		DisplayName(const std::string name) : Value(name) {};
	};

	struct RUNTIME_API Category : public reflection::Attribute
	{
		static std::string Default;

		std::string Value;

		Category(const std::string category) : Value(category) {};
	};

	struct RUNTIME_API Range : public reflection::Attribute
	{
		float A;
		float B;

		Range(const float a, const float b) : A(a), B(b) {};
	};
}

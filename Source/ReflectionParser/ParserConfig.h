/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>

#include <clang-c/Index.h>

#include <tclap/CmdLine.h>

#include <mustache.hpp>

class mustache : public kainjow::mustache::mustache
{
public:
	using kainjow::mustache::mustache::mustache;

	class Data : public kainjow::mustache::data
	{
	public:
		enum class Type
		{
			Object,
			String,
			List,
			True,
			False,
			Partial,
			Invalid
		};

		using PartialType = kainjow::mustache::partial;
		using kainjow::mustache::data::data;

		Data(Type type)
			: kainjow::mustache::data(ToMustacheType(type))
		{
		}

	private:
		static kainjow::mustache::data::type ToMustacheType(Type type)
		{
			switch (type)
			{
			case Type::Object:
				return kainjow::mustache::data::type::object;
			case Type::String:
				return kainjow::mustache::data::type::string;
			case Type::List:
				return kainjow::mustache::data::type::list;
			case Type::True:
				return kainjow::mustache::data::type::bool_true;
			case Type::False:
				return kainjow::mustache::data::type::bool_false;
			case Type::Partial:
				return kainjow::mustache::data::type::partial;
			case Type::Invalid:
				return kainjow::mustache::data::type::invalid;
			}

			return kainjow::mustache::data::type::invalid;
		}
	};

	bool isValid() const
	{
		return is_valid();
	}

	const std::string& errorMessage() const
	{
		return error_message();
	}
};

namespace cmd_parser = TCLAP;

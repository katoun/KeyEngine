/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

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

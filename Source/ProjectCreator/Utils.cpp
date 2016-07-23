/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Utils.h>

#include <iostream>
#include <fstream>

namespace creator
{
	mustache::Data::Type ToMustache(bool value)
	{
		return value ? mustache::Data::Type::True : mustache::Data::Type::False;
	}

	void LoadText(const std::string &filename, std::string &output)
	{
		std::ifstream input;

		input.open(filename);

		if (!input)
		{
			std::stringstream error;

			error << "Unable to open file \"" << filename << "\" for reading." << std::endl;

			throw std::exception(error.str().c_str());
		}

		input.seekg(0, std::ios::end);

		output.reserve(static_cast<std::string::size_type>(input.tellg()));

		input.seekg(0, std::ios::beg);

		// disambiguate function declaration
		output.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

		input.close();
	}

	void WriteText(const std::string &filename, const std::string &text)
	{
		std::ofstream output;
		output.open(filename);

		if (!output)
		{
			std::stringstream error;

			error << "Unable to open file \"" << filename << "\" for writing." << std::endl;

			throw std::exception(error.str().c_str());
		}

		output << text;

		output.close();
	}
}
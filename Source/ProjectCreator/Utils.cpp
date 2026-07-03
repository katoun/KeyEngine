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

#include <Utils.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

namespace creator
{
	mustache::Data ToMustache(bool value)
	{
		return value;
	}

	void LoadText(const std::string &filename, std::string &output)
	{
		std::ifstream input;

		input.open(filename);

		if (!input)
		{
			std::stringstream error;

			error << "Unable to open file \"" << filename << "\" for reading." << std::endl;

			throw std::runtime_error(error.str());
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

			throw std::runtime_error(error.str());
		}

		output << text;

		output.close();
	}
}

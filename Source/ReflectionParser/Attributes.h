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

#include <Cursor.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace parser
{
	const auto ExplicitGetter = "Getter";
	const auto ExplicitSetter = "Setter";
	const auto DisplayName = "DisplayName";

	class Attributes
	{
	public:

		Attributes(const Cursor& cursor);

		bool HasProperties(void) const;
		bool HasProperty(const std::string &key) const;
		std::string GetProperty(const std::string &key) const;
		const size_t GetCount(void) const;
		const size_t GetExportCount(void) const;

		mustache::Data CompileData() const;

	private:

		friend class Class;
		friend class Field;

		typedef std::pair<std::string, std::string> Property;

		std::vector<Property> ExtractProperties(const Cursor &cursor) const;

		std::unordered_map<std::string, std::string> m_Properties;
	};
}

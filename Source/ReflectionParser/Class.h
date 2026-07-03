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

#include <LanguageType.h>
#include <Cursor.h>
#include <Namespace.h>
#include <Field.h>

#include <vector>
#include <string>

namespace parser
{
	struct BaseClass
	{
		BaseClass(const Cursor &cursor);

		std::string Name;
	};

	class Class : public LanguageType
	{
	public:

		Class(const Cursor &cursor, const Namespace &currentNamespace);
		virtual ~Class(void);

		mustache::Data CompileDefinition(void) const;

	private:

		friend class Field;

		std::string m_Name;
		std::string m_DisplayName;

		std::vector<BaseClass> m_BaseClasses;

		std::vector<Field> m_Fields;
	};
}

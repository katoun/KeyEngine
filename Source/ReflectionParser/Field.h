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

#include <string>

namespace parser
{
	class Class;

	class Field : public LanguageType
	{
	public:

		Field(const Cursor &cursor, const Namespace &currentNamespace, Class& parent);
		virtual ~Field(void) {}

		mustache::Data CompileDefinition(void) const;

	private:

		std::string GetNiceName(const std::string& name);
		bool HasGetter(void) const;
		bool HasSetter(void) const;

		std::string m_Name;
		std::string m_DisplayName;

		std::string m_Type;

		bool m_IsConst;

		Class& m_Parent;

		bool m_HasExplicitGetter;
		bool m_HasExplicitSetter;
		std::string m_ExplicitGetter;
		std::string m_ExplicitSetter;

	};
}

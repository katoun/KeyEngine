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

#include <ParserConfig.h>
#include <CursorType.h>

#include <string>
#include <vector>

namespace parser
{
	class Cursor
	{
	public:

		typedef std::vector<Cursor> List;
		typedef CXCursorVisitor Visitor;

		Cursor(const CXCursor &handle);

		CXCursorKind GetKind(void) const;

		Cursor GetLexicalParent(void) const;

		std::string GetSpelling(void) const;
		std::string GetDisplayName(void) const;
		std::string GetMangledName(void) const;

		bool IsDefinition(void) const;
		bool IsConst(void) const;
		bool IsStatic(void) const;

		CX_CXXAccessSpecifier GetAccessModifier(void) const;
		CX_StorageClass GetStorageClass(void) const;

		CursorType GetType(void) const;
		CursorType GetReturnType(void) const;
		CursorType GetTypedefType(void) const;

		List GetChildren(void) const;
		void VisitChildren(Visitor visitor, void *data = nullptr);

	private:

		CXCursor m_Handle;
	};
}
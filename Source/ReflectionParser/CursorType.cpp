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

#include <CursorType.h>
#include <Cursor.h>
#include <Utils.h>

namespace parser
{
	CursorType::CursorType(const CXType &handle) : m_Handle(handle)
	{}

	std::string CursorType::GetDisplayName(void) const
	{
		std::string displayName;

		ToString(clang_getTypeSpelling(m_Handle), displayName);

		return displayName;
	}

	int CursorType::GetArgumentCount(void) const
	{
		return clang_getNumArgTypes(m_Handle);
	}

	CursorType CursorType::GetArgument(unsigned index) const
	{
		return clang_getArgType(m_Handle, index);
	}

	CursorType CursorType::GetCanonicalType(void) const
	{
		return clang_getCanonicalType(m_Handle);
	}

	Cursor CursorType::GetDeclaration(void) const
	{
		return clang_getTypeDeclaration(m_Handle);
	}

	CXTypeKind CursorType::GetKind(void) const
	{
		return m_Handle.kind;
	}

	bool CursorType::IsConst(void) const
	{
		return clang_isConstQualifiedType(m_Handle) ? true : false;
	}
}
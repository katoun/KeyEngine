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

#include <Cursor.h>
#include <Utils.h>

namespace parser
{
	Cursor::Cursor(const CXCursor &handle) : m_Handle(handle)
	{ }

	CXCursorKind Cursor::GetKind(void) const
	{
		return m_Handle.kind;
	}

	Cursor Cursor::GetLexicalParent(void) const
	{
		return clang_getCursorLexicalParent(m_Handle);
	}

	std::string Cursor::GetSpelling(void) const
	{
		std::string spelling;

		ToString(clang_getCursorSpelling(m_Handle), spelling);

		return spelling;
	}

	std::string Cursor::GetDisplayName(void) const
	{
		std::string displayName;

		ToString(clang_getCursorDisplayName(m_Handle), displayName);

		return displayName;
	}

	std::string Cursor::GetMangledName(void) const
	{
		std::string mangled;

		ToString(clang_Cursor_getMangling(m_Handle), mangled);

		return mangled;
	}

	bool Cursor::IsDefinition(void) const
	{
		return clang_isCursorDefinition(m_Handle) ? true : false;
	}

	bool Cursor::IsConst(void) const
	{
		return clang_CXXMethod_isConst(m_Handle) ? true : false;
	}

	bool Cursor::IsStatic(void) const
	{
		return clang_CXXMethod_isStatic(m_Handle) ? true : false;
	}

	CX_CXXAccessSpecifier Cursor::GetAccessModifier(void) const
	{
		return clang_getCXXAccessSpecifier(m_Handle);
	}

	CX_StorageClass Cursor::GetStorageClass(void) const
	{
		return clang_Cursor_getStorageClass(m_Handle);
	}

	CursorType Cursor::GetType(void) const
	{
		return clang_getCursorType(m_Handle);
	}

	CursorType Cursor::GetReturnType(void) const
	{
		return clang_getCursorResultType(m_Handle);
	}

	CursorType Cursor::GetTypedefType(void) const
	{
		return clang_getTypedefDeclUnderlyingType(m_Handle);
	}

	Cursor::List Cursor::GetChildren(void) const
	{
		List children;

		auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
		{
			auto container = static_cast<List *>(data);

			container->emplace_back(cursor);

			if (cursor.kind == CXCursor_LastPreprocessing)
				return CXChildVisit_Break;

			return CXChildVisit_Continue;
		};

		clang_visitChildren(m_Handle, visitor, &children);

		return children;
	}

	void Cursor::VisitChildren(Visitor visitor, void *data)
	{
		clang_visitChildren(m_Handle, visitor, data);
	}
}
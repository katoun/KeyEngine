/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
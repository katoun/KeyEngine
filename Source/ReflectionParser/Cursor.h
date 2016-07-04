/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
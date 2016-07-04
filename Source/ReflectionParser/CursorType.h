/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <ParserConfig.h>

#include <string>

namespace parser
{
	class Cursor;

	class CursorType
	{
	public:
		CursorType(const CXType &handle);

		std::string GetDisplayName(void) const;

		int GetArgumentCount(void) const;
		CursorType GetArgument(unsigned index) const;

		CursorType GetCanonicalType(void) const;

		Cursor GetDeclaration(void) const;

		CXTypeKind GetKind(void) const;

		bool IsConst(void) const;

	private:

		CXType m_Handle;
	};
}
// Copyright (c) 2016 Catalin Alexandru Nastase

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
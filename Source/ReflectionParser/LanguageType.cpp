/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <LanguageType.h>

namespace parser
{
	LanguageType::LanguageType(const Cursor &cursor, const Namespace &currentNamespace)
		: m_Attributes(cursor)
		, m_AccessModifier(cursor.GetAccessModifier())
		, m_IsValid(m_AccessModifier == CX_CXXPublic)
	{}

	bool LanguageType::IsValid(void) const
	{
		return m_IsValid;
	}

	const Attributes& LanguageType::GetAttributes(void) const
	{
		return m_Attributes;
	}
}
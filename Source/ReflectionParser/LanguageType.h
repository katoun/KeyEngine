/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <ParserConfig.h>
#include <Cursor.h>
#include <Namespace.h>
#include <Attributes.h>

namespace parser
{
	class Parser;

	class LanguageType
	{
	public:

		LanguageType(const Cursor &cursor, const Namespace &currentNamespace);
		virtual ~LanguageType(void) { }

		bool IsValid() const;

		const Attributes& GetAttributes(void) const;

	protected:

		CX_CXXAccessSpecifier m_AccessModifier;

		bool m_IsValid;

		Attributes m_Attributes;
	};
}

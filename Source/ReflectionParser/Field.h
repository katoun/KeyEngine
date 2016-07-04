/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <LanguageType.h>
#include <Cursor.h>
#include <Namespace.h>

#include <string>

namespace parser
{
	class Class;

	class Field : public LanguageType
	{
	public:

		Field(const Cursor &cursor, const Namespace &currentNamespace, Class *parent = nullptr);
		virtual ~Field(void) {}

		mustache::Data CompileDefinition(void) const;

	private:

		std::string GetNiceName(const std::string& name);
		bool HasGetter(void) const;
		bool HasSetter(void) const;

		std::string m_Name;
		std::string m_DisplayName;

		std::string m_Type;

		bool m_IsConst;

		Class *m_Parent;

		bool m_HasExplicitGetter;
		bool m_HasExplicitSetter;
		std::string m_ExplicitGetter;
		std::string m_ExplicitSetter;

	};
}

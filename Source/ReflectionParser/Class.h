// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <LanguageType.h>
#include <Cursor.h>
#include <Namespace.h>

#include <vector>
#include <string>

namespace parser
{
	struct BaseClass
	{
		BaseClass(const Cursor &cursor);

		std::string Name;
	};

	class Class : public LanguageType
	{
	public:

		Class(const Cursor &cursor, const Namespace &currentNamespace);
		virtual ~Class(void);

		mustache::Data CompileDeclaration(void) const;
		mustache::Data CompileDefinition(void) const;

	private:

		friend class Field;

		std::string m_Name;
		std::string m_DisplayName;

		std::vector<BaseClass*> m_BaseClasses;

		std::vector<Field*> m_Fields;
	};
}
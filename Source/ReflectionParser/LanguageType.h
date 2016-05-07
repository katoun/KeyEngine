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

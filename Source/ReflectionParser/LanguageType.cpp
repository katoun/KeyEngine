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
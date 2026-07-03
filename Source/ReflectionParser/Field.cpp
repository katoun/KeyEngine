/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include <Field.h>
#include <Class.h>
#include <Parser.h>
#include <Utils.h>
#include <Templates.h>
#include <Core/Utils.h>

namespace parser
{
	Field::Field(const Cursor &cursor, const Namespace &currentNamespace, Class& parent)
		: LanguageType(cursor, currentNamespace)
		, m_Name{ cursor.GetSpelling() }
		, m_Parent{ parent }
		, m_IsConst(cursor.GetType().IsConst())
		, m_Type(cursor.GetType().GetDisplayName())
	{
		m_ExplicitGetter = m_Attributes.GetProperty(ExplicitGetter);
		m_ExplicitSetter = m_Attributes.GetProperty(ExplicitSetter);

		m_HasExplicitGetter = !m_ExplicitGetter.empty();
		m_HasExplicitSetter = !m_ExplicitSetter.empty();

		m_IsValid = m_IsValid || m_HasExplicitGetter;

		m_DisplayName = m_Attributes.GetProperty(DisplayName);
		if (m_DisplayName.empty())
		{
			m_DisplayName = GetNiceName(m_Name);
			m_Attributes.m_Properties[DisplayName] = '\"' + m_DisplayName + '\"';
		}
	}

	mustache::Data Field::CompileDefinition(void) const
	{
		mustache::Data data{ mustache::Data::Type::Object };

		data["FieldName"] = m_Name;
		data["FieldType"] = m_Type;

		data["ClassName"] = m_Parent.m_Name;

		data.set("AttributeInitializerList", Parser::Instance().LoadTemplatePartial(templates::AttributeInitializer));

		data["FieldName"] = m_Name;

		size_t attributes_count = m_Attributes.GetExportCount();

		data["HasGetter"] = ToMustache(HasGetter());
		data["HasSetter"] = ToMustache(HasSetter());
		data["IsGetterLast"] = ToMustache(HasSetter() || attributes_count != 0);
		data["IsSetterLast"] = ToMustache(attributes_count != 0);

		data["HasExplicitGetter"] = ToMustache(m_HasExplicitGetter);
		data["ExplicitGetter"] = m_ExplicitGetter;

		data["HasExplicitSetter"] = ToMustache(m_HasExplicitSetter);
		data["ExplicitSetter"] = m_ExplicitSetter;

		data.set("AttributeInitializer", m_Attributes.CompileData());

		return data;
	}

	std::string Field::GetNiceName(const std::string& name)
	{
		std::string ret = name;

		if (ret.length() > 2 && ret[0] == 'm' && ret[1] == '_')
		{
			ret = ret.substr(2);
		}

		for (auto it = ret.begin(); it != ret.end(); ++it)
		{
			if (isupper(*it) && it != ret.begin())
			{
				it = ret.insert(it, 1, ' ') + 1;
			}
		}

		return ret;
	}

	bool Field::HasGetter(void) const
	{
		return m_HasExplicitGetter || m_AccessModifier == CX_CXXPublic;
	}

	bool Field::HasSetter(void) const
	{
		return m_HasExplicitSetter || (!m_IsConst && m_AccessModifier == CX_CXXPublic);
	}
}

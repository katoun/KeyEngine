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

#include <Class.h>
#include <Field.h>
#include <Utils.h>
#include <Parser.h>
#include <Templates.h>
#include <Reserved.h>

namespace parser
{
	static std::vector<std::string> ValidClasses;

	bool IsReservedType(const std::string& qualifiedName)
	{
		return std::find(ReservedTypes.begin(), ReservedTypes.end(), qualifiedName) != ReservedTypes.end();
	}
	bool IsBaseType(const std::string& qualifiedName)
	{
		return std::find(BaseTypes.begin(), BaseTypes.end(), qualifiedName) != BaseTypes.end();
	}
	bool IsValidType(const std::string& qualifiedName)
	{
		return std::find(ValidClasses.begin(), ValidClasses.end(), qualifiedName) != ValidClasses.end();
	}

	BaseClass::BaseClass(const Cursor &cursor) : Name(cursor.GetType().GetCanonicalType().GetDisplayName())
	{}

	Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
		: LanguageType(cursor, currentNamespace)
		, m_Name(cursor.GetType().GetDisplayName())
	{
		m_IsValid = false;

		m_DisplayName = m_Attributes.GetProperty(DisplayName);

		if (m_DisplayName.empty())
		{
			m_DisplayName = m_Name;
			m_Attributes.m_Properties[DisplayName] = '\"' + m_DisplayName + '\"';
		}

		for (auto &child : cursor.GetChildren())
		{
			switch (child.GetKind())
			{
			case CXCursor_CXXBaseSpecifier:
			{
				auto& baseClass = m_BaseClasses.emplace_back(child);

				if (!IsReservedType(baseClass.Name) && (IsBaseType(baseClass.Name) || IsValidType(baseClass.Name)))
				{
					m_IsValid = true;
				}
			}
			break;
			case CXCursor_FieldDecl:
				m_Fields.emplace_back(child, currentNamespace, *this);
				break;
			case CXCursor_VarDecl:
				break;
			case CXCursor_CXXMethod:
				if (child.IsStatic())
				{
				}
				else
				{
				}
				break;
			}
		}

		if (IsReservedType(m_Name) || IsBaseType(m_Name))
		{
			m_IsValid = false;
		}

		if (m_IsValid)
		{
			ValidClasses.emplace_back(m_Name);
		}
		else
		{
			m_Fields.clear();
		}
	}

	Class::~Class(void) = default;

	mustache::Data Class::CompileDefinition(void) const
	{
		mustache::Data data{ mustache::Data::Type::Object };

		data["ClassName"] = m_Name;
		// base classes
		{
			mustache::Data baseClasses{ mustache::Data::Type::List };

			for (size_t i = 0; i < m_BaseClasses.size(); ++i)
			{
				auto& baseClass = m_BaseClasses[i];

				mustache::Data item{ mustache::Data::Type::Object };

				item["ClassName"] = baseClass.Name;

				item["IsLast"] = ToMustache(i == m_BaseClasses.size() - 1);

				baseClasses << item;
			}

			data["HasBaseTypes"] = ToMustache(m_BaseClasses.size() != 0);
			data.set("BaseClass", baseClasses);

			data["HasClassAttributes"] = ToMustache(m_Attributes.HasProperties());
			data.set("AttributeInitializerList", Parser::Instance().LoadTemplatePartial(templates::AttributeInitializer));
			data["HasGetter"] = ToMustache(false);
			data["HasSetter"] = ToMustache(false);
			data.set("AttributeInitializer", m_Attributes.CompileData());
		}

		// fields
		{
			mustache::Data fields{ mustache::Data::Type::List };

			for (const auto& field : m_Fields)
			{
				if (field.IsValid())
				{
					fields << field.CompileDefinition();
				}
			}

			data.set("Field", fields);
		}

		return data;
	}
}

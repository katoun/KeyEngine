/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
		if (IsReservedType(m_Name) || IsBaseType(m_Name))
		{
			m_IsValid = false;
			return;
		}

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
				auto baseClass = new BaseClass(child);

				m_BaseClasses.emplace_back(baseClass);

				m_IsValid = false;

				if (!IsReservedType(baseClass->Name) && (IsBaseType(baseClass->Name) || IsValidType(baseClass->Name)))
				{
					m_IsValid = true;
				}

				if (m_IsValid)
				{
					ValidClasses.emplace_back(m_Name);
				}
				else
				{
					return;
				}
			}
			break;
			case CXCursor_FieldDecl:
				m_Fields.emplace_back(new Field(child, currentNamespace, this));
				break;
			case CXCursor_VarDecl:
				//m_staticFields.emplace_back(new Global(child, Namespace(), this));
				break;
			case CXCursor_CXXMethod:
				if (child.IsStatic())
				{
					//m_staticMethods.emplace_back(new Function(child, Namespace(), this));
				}
				else
				{
					//m_methods.emplace_back(new Method(child, currentNamespace, this));
				}
				break;
			}
		}
	}

	Class::~Class(void)
	{
		for (auto *baseClass : m_BaseClasses)
		{
			SAFE_DELETE(baseClass);
		}

		for (auto *field : m_Fields)
		{
			SAFE_DELETE(field);
		}

		//TODO!!!
	}

	mustache::Data Class::CompileDeclaration(void) const
	{
		mustache::Data data{ mustache::Data::Type::Object };

		data["ClassName"] = m_Name;

		return data;
	}

	mustache::Data Class::CompileDefinition(void) const
	{
		mustache::Data data{ mustache::Data::Type::Object };

		data["ClassName"] = m_Name;
		// base classes
		{
			mustache::Data baseClasses{ mustache::Data::Type::List };

			for (size_t i = 0; i < m_BaseClasses.size(); ++i)
			{
				auto baseClass = m_BaseClasses[i];

				mustache::Data item{ mustache::Data::Type::Object };

				item["ClassName"] = baseClass->Name;

				item["IsLast"] = ToMustache(i == m_BaseClasses.size() - 1);

				baseClasses << item;
			}

			data["HasBaseTypes"] = ToMustache(m_BaseClasses.size() != 0);
			data["BaseClass"] = baseClasses;

			data["HasClassAttributes"] = ToMustache(m_Attributes.HasProperties());
			data["AttributeInitializerList"] = Parser::Instance().LoadTemplatePartial(templates::AttributeInitializer);
			data["HasGetter"] = ToMustache(false);
			data["HasSetter"] = ToMustache(false);
			data["AttributeInitializer"] = m_Attributes.CompileData();
		}

		// fields
		{
			mustache::Data fields{ mustache::Data::Type::List };

			for (auto field : m_Fields)
			{
				if (field->IsValid())
				{
					fields << field->CompileDefinition();
				}
			}

			data["Field"] = fields;
		}

		return data;
	}
}

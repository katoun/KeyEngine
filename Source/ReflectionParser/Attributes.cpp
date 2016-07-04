/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Attributes.h>
#include <Reserved.h>
#include <Parser.h>
#include <Utils.h>
#include <Core/Utils.h>

#include <regex>

namespace parser
{
	Attributes::Attributes(const Cursor& cursor)
	{
		for (auto &child : cursor.GetChildren())
		{
			if (child.GetKind() != CXCursor_AnnotateAttr)
				continue;

			for (auto &prop : ExtractProperties(child))
			{
				m_Properties[prop.first] = prop.second;
			}
		}
	}

	bool Attributes::HasProperties(void) const
	{
		return (m_Properties.size() != 0);
	}

	bool Attributes::HasProperty(const std::string &key) const
	{
		if (m_Properties.find(key) != m_Properties.end())
			return true;

		if (m_Properties.find("attribute::" + key) != m_Properties.end())
			return true;

		return false;
	}

	std::string Attributes::GetProperty(const std::string &key) const
	{
		auto search = m_Properties.find(key);
		if (search != m_Properties.end())
			return search->second;
			
		search = m_Properties.find("attribute::" + key);
		if (search != m_Properties.end())
			return search->second;
		
		return std::string();
	}

	const size_t Attributes::GetCount(void) const
	{
		return m_Properties.size();
	}

	const size_t Attributes::GetExportCount(void) const
	{
		if (HasProperty(ExplicitGetter) && HasProperty(ExplicitSetter))
			return GetCount() - 2;

		if (HasProperty(ExplicitGetter) || HasProperty(ExplicitSetter))
			return GetCount() - 1;

		return GetCount();
	}

	mustache::Data Attributes::CompileData() const
	{
		mustache::Data data{ mustache::Data::Type::List };

		size_t i = 0;
		size_t count = m_Properties.size();
		for (auto& prop : m_Properties)
		{
			mustache::Data item{ mustache::Data::Type::Object };

			// Explicit Getter and Setter are handled by the Field class
			if (prop.first == ExplicitGetter || prop.first == (std::string("attribute::") + ExplicitGetter) || 
				prop.first == ExplicitSetter || prop.first == (std::string("attribute::") + ExplicitSetter))
			{
				--count;
				continue;
			}

			item["AttributeType"] = prop.first;
			item["Arguments"] = prop.second;
			item["IsLast"] = ToMustache(i == count - 1);

			data << item;

			++i;
		}

		return data;
	}

	std::vector<Attributes::Property> Attributes::ExtractProperties(const Cursor &cursor) const
	{
		auto meta = cursor.GetDisplayName();

		std::vector<Property> properties;

		static const std::regex propertyList(
			// property name
			"([a-z\\:]+)"
			// optional whitespace before
			"(?:\\s*)"
			// constructor
			"("
			// opening paren
			"\\("
			// arguments
			"([^\\)]*)"
			// closing paren
			"\\)"
			// end constructor
			")?"
			// optional comma/whitespace
			"(?:(\\s|,)*)",
			std::regex::icase);

		auto start = meta.cbegin();

		auto flags = std::regex_constants::match_default | std::regex_constants::format_default | std::regex_constants::format_sed | std::regex_constants::format_no_copy | std::regex_constants::format_first_only;
		std::match_results<std::string::const_iterator> match;

		while (std::regex_search(start, meta.cend(), match, propertyList, flags))
		{
			auto name = match[1].str();
			auto arguments = match[3].str();

			core::string::ReplaceFirst(name, "attribute::", "");

			properties.emplace_back(name, arguments);

			// advance the first capture group
			start += match[0].length();
		}

		return properties;
	}
}
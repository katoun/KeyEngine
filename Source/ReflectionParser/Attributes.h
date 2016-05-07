#pragma once

#include <Cursor.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace parser
{
	const auto ExplicitGetter = "Getter";
	const auto ExplicitSetter = "Setter";
	const auto DisplayName = "DisplayName";

	class Attributes
	{
	public:

		Attributes(const Cursor& cursor);

		bool HasProperties(void) const;
		bool HasProperty(const std::string &key) const;
		std::string GetProperty(const std::string &key) const;
		const size_t GetCount(void) const;
		const size_t GetExportCount(void) const;

		mustache::Data CompileData() const;

	private:

		friend class Class;
		friend class Field;

		typedef std::pair<std::string, std::string> Property;

		std::vector<Property> ExtractProperties(const Cursor &cursor) const;

		std::unordered_map<std::string, std::string> m_Properties;
	};
}

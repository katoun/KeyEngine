#pragma once

#include <ParserConfig.h>

#include <string>

namespace parser
{
	void ToString(const CXString &str, std::string &output);
	mustache::Data::Type ToMustache(bool value);
	filesystem::path GetCanonicalPath(const filesystem::path& p);

	void LoadText(const std::string& filename, std::string& output);
	void WriteText(const std::string& filename, const std::string& text);
}

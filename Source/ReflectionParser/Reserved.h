// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <vector>
#include <string>

namespace parser
{
	const std::vector<std::string> ReservedTypes =
	{
		"attribute::Getter",
		"attribute::Setter",
		"attribute::Range",
		"attribute::Category",
	};

	const std::vector<std::string> BaseTypes =
	{
		"reflection::Attribute",
		"core::Object",
		"game::GameObject",
		"game::Component"
	};
}

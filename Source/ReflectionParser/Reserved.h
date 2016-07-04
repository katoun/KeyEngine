/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

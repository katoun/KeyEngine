/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <CreatorConfig.h>

#include <string>

namespace creator
{
	mustache::Data::Type ToMustache(bool value);

	void LoadText(const std::string& filename, std::string& output);
	void WriteText(const std::string& filename, const std::string& text);
}

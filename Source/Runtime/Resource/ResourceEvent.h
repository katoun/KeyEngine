/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>

#include <functional>
#include <optional>

namespace resource
{
	class Resource;

	struct RUNTIME_API ResourceEvent
	{
		std::optional<std::reference_wrapper<Resource>> source;

		ResourceEvent() = default;
	};
}

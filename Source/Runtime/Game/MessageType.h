/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

namespace game
{
	enum MessageType
	{
		UNDEFINED,
		PARENT_CHANGED,
		COMPONENT_ATTACHED,
		COMPONENT_DETACHED,
		NEEDS_UPDATE,
		COUNT
	};
} // end namespace game
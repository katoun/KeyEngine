// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

namespace game
{
	enum Message
	{
		UNDEFINED,
		PARENT_CHANGED,
		COMPONENT_ATTACHED,
		COMPONENT_DETACHED,
		NEEDS_UPDATE,
		COUNT
	};
} // end namespace game
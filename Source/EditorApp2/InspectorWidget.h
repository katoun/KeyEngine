/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Runtime.h>

#include <array>

namespace editor
{
	class InspectorWidget
	{
	public:
		void SelectionChanged(core::Object* selection);
		void Draw();

	private:
		void DrawGameObject(game::GameObject& game_object);
		void DrawField(reflection::Any component_any, const reflection::Field& field);

		core::Object* m_Selection = nullptr;
		std::array<char, 256> m_NameBuffer{};
	};
}

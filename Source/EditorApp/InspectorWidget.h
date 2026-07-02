/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Runtime.h>

#include <array>
#include <string>
#include <unordered_map>

namespace editor
{
	class InspectorWidget
	{
	public:
		void SelectionChanged(core::Object* selection);
		void Draw();

	private:
		void DrawGameObject(game::GameObject& game_object);
		void DrawNameField(game::GameObject& game_object);
		void DrawField(reflection::Any component_any, const reflection::Type& component_type, const reflection::Field& field);
		void DrawPropertyLabel(const std::string& label);
		bool DrawVector3Field(const std::string& id, glm::vec3& value);

		core::Object* m_Selection = nullptr;
		std::array<char, 256> m_NameBuffer{};
		std::unordered_map<std::string, std::array<char, 256>> m_StringBuffers;
	};
}

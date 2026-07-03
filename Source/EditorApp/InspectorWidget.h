/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <Runtime.h>

#include <array>
#include <memory>
#include <string>
#include <unordered_map>

namespace editor
{
	class InspectorWidget
	{
	public:
		void SelectionChanged(std::shared_ptr<core::Object> selection);
		void Draw();

	private:
		void DrawGameObject(game::GameObject& game_object);
		void DrawNameField(game::GameObject& game_object);
		void DrawField(reflection::Any component_any, const reflection::Type& component_type, const reflection::Field& field);
		void DrawPropertyLabel(const std::string& label);
		bool DrawVector3Field(const std::string& id, glm::vec3& value);

		std::shared_ptr<core::Object> m_Selection;
		std::array<char, 256> m_NameBuffer{};
		std::unordered_map<std::string, std::array<char, 256>> m_StringBuffers;
	};
}

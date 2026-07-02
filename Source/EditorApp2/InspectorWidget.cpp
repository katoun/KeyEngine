/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "InspectorWidget.h"

#include <imgui.h>

#include <cstring>

namespace editor
{
	void InspectorWidget::SelectionChanged(core::Object* selection)
	{
		m_Selection = selection;
		m_NameBuffer.fill('\0');

		auto* game_object = dynamic_cast<game::GameObject*>(selection);
		if (game_object != nullptr)
			std::strncpy(m_NameBuffer.data(), game_object->GetName().c_str(), m_NameBuffer.size() - 1);
	}

	void InspectorWidget::Draw()
	{
		ImGui::Begin("Inspector");

		if (m_Selection == nullptr)
		{
			ImGui::TextDisabled("Nothing selected.");
			ImGui::End();
			return;
		}

		auto* game_object = dynamic_cast<game::GameObject*>(m_Selection);
		if (game_object != nullptr)
			DrawGameObject(*game_object);
		else
			ImGui::TextDisabled("Unsupported selection type.");

		ImGui::End();
	}

	void InspectorWidget::DrawGameObject(game::GameObject& game_object)
	{
		if (ImGui::InputText("Name", m_NameBuffer.data(), m_NameBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
			game_object.SetName(m_NameBuffer.data());

		if (ImGui::IsItemDeactivatedAfterEdit())
			game_object.SetName(m_NameBuffer.data());

		auto components_any = game_object.GetComponentsAny();
		for (auto& pair : components_any)
		{
			reflection::Type component_type(pair.first);
			auto display_name_attribute = component_type.GetAttribute<attribute::DisplayName>();
			const std::string component_name = display_name_attribute != nullptr ? display_name_attribute->Value : component_type.GetName();

			if (!ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				continue;

			ImGui::Indent();
			for (const auto& field : component_type.GetFields())
				DrawField(pair.second, field);
			ImGui::Unindent();
		}
	}

	void InspectorWidget::DrawField(reflection::Any component_any, const reflection::Field& field)
	{
		auto display_name_attribute = field.GetAttribute<attribute::DisplayName>();
		const std::string field_name = display_name_attribute != nullptr ? display_name_attribute->Value : field.GetName();
		reflection::Any value_any = field.GetValue(component_any);

		if (field.GetType() == typeof(int))
		{
			int value = value_any.GetValue<int>();
			if (ImGui::InputInt(field_name.c_str(), &value))
				field.SetValue(component_any, reflection::Any(value));
		}
		else if (field.GetType() == typeof(float))
		{
			float value = value_any.GetValue<float>();
			if (ImGui::InputFloat(field_name.c_str(), &value))
				field.SetValue(component_any, reflection::Any(value));
		}
		else if (field.GetType() == typeof(glm::vec3))
		{
			glm::vec3 value = value_any.GetValue<glm::vec3>();
			if (ImGui::InputFloat3(field_name.c_str(), &value.x))
				field.SetValue(component_any, reflection::Any(value));
		}
		else if (field.GetType() == typeof(std::string))
		{
			ImGui::Text("%s: %s", field_name.c_str(), value_any.GetValue<std::string>().c_str());
		}
		else
		{
			ImGui::TextDisabled("%s: %s", field_name.c_str(), field.GetType().GetName().c_str());
		}
	}
}

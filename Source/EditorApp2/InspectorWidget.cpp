/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "InspectorWidget.h"

#include <imgui.h>

#include <cfloat>
#include <cstring>

namespace editor
{
	void InspectorWidget::SelectionChanged(core::Object* selection)
	{
		m_Selection = selection;
		m_NameBuffer.fill('\0');
		m_StringBuffers.clear();

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
		const ImGuiTableFlags table_flags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV;

		if (ImGui::BeginTable("GameObjectProperties", 2, table_flags))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 92.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
			DrawNameField(game_object);
			ImGui::EndTable();
		}

		auto components_any = game_object.GetComponentsAny();
		for (auto& pair : components_any)
		{
			reflection::Type component_type(pair.first);
			auto display_name_attribute = component_type.GetAttribute<attribute::DisplayName>();
			const std::string component_name = display_name_attribute != nullptr ? display_name_attribute->Value : component_type.GetName();

			if (!ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				continue;

			const std::string table_id = "ComponentProperties##" + component_type.GetName();
			if (ImGui::BeginTable(table_id.c_str(), 2, table_flags))
			{
				ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 92.0f);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

				for (const auto& field : component_type.GetFields())
					DrawField(pair.second, component_type, field);

				ImGui::EndTable();
			}
		}
	}

	void InspectorWidget::DrawNameField(game::GameObject& game_object)
	{
		ImGui::TableNextRow();
		DrawPropertyLabel("Name");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);
		if (ImGui::InputText("##GameObjectName", m_NameBuffer.data(), m_NameBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
			game_object.SetName(m_NameBuffer.data());

		if (ImGui::IsItemDeactivatedAfterEdit())
			game_object.SetName(m_NameBuffer.data());
		ImGui::PopItemWidth();
	}

	void InspectorWidget::DrawField(reflection::Any component_any, const reflection::Type& component_type, const reflection::Field& field)
	{
		auto display_name_attribute = field.GetAttribute<attribute::DisplayName>();
		const std::string field_name = display_name_attribute != nullptr ? display_name_attribute->Value : field.GetName();
		const std::string control_id = "##" + component_type.GetName() + "." + field.GetName();
		reflection::Any value_any = field.GetValue(component_any);

		ImGui::TableNextRow();
		DrawPropertyLabel(field_name);
		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);

		if (field.GetType() == typeof(int))
		{
			int value = value_any.GetValue<int>();
			if (ImGui::InputInt(control_id.c_str(), &value))
				field.SetValue(component_any, reflection::Any(value));
		}
		else if (field.GetType() == typeof(float))
		{
			float value = value_any.GetValue<float>();
			if (ImGui::InputFloat(control_id.c_str(), &value))
				field.SetValue(component_any, reflection::Any(value));
		}
		else if (field.GetType() == typeof(glm::vec3))
		{
			glm::vec3 value = value_any.GetValue<glm::vec3>();
			ImGui::PopItemWidth();
			if (DrawVector3Field(control_id, value))
				field.SetValue(component_any, reflection::Any(value));
			return;
		}
		else if (field.GetType() == typeof(std::string))
		{
			auto& buffer = m_StringBuffers[control_id];
			const std::string value = value_any.GetValue<std::string>();
			if (buffer[0] == '\0' && !value.empty())
				std::strncpy(buffer.data(), value.c_str(), buffer.size() - 1);

			if (ImGui::InputText(control_id.c_str(), buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
				field.SetValue(component_any, reflection::Any(std::string(buffer.data())));

			if (ImGui::IsItemDeactivatedAfterEdit())
				field.SetValue(component_any, reflection::Any(std::string(buffer.data())));
		}
		else
		{
			ImGui::TextDisabled("%s", field.GetType().GetName().c_str());
		}

		ImGui::PopItemWidth();
	}

	void InspectorWidget::DrawPropertyLabel(const std::string& label)
	{
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.c_str());
	}

	bool InspectorWidget::DrawVector3Field(const std::string& id, glm::vec3& value)
	{
		bool changed = false;
		const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		const float label_width = ImGui::CalcTextSize("Z").x + spacing;
		const float available_width = ImGui::GetContentRegionAvail().x;
		const float input_width = (available_width - (label_width * 3.0f) - (spacing * 4.0f)) / 3.0f;
		const float clamped_width = input_width > 42.0f ? input_width : 42.0f;

		ImGui::PushID(id.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("X");
		ImGui::SameLine(0.0f, spacing);
		ImGui::PushItemWidth(clamped_width);
		changed |= ImGui::InputFloat("##X", &value.x);
		ImGui::PopItemWidth();

		ImGui::SameLine(0.0f, spacing);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine(0.0f, spacing);
		ImGui::PushItemWidth(clamped_width);
		changed |= ImGui::InputFloat("##Y", &value.y);
		ImGui::PopItemWidth();

		ImGui::SameLine(0.0f, spacing);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Z");
		ImGui::SameLine(0.0f, spacing);
		ImGui::PushItemWidth(clamped_width);
		changed |= ImGui::InputFloat("##Z", &value.z);
		ImGui::PopItemWidth();

		ImGui::PopID();
		return changed;
	}
}

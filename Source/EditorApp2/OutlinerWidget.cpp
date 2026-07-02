/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "OutlinerWidget.h"
#include "IconManager.h"

#include <imgui.h>

#include <algorithm>

namespace editor
{
	OutlinerWidget::OutlinerWidget() = default;

	OutlinerWidget::~OutlinerWidget()
	{
		for (auto& item : m_Items)
		{
			if (item != nullptr && item->type == ItemType::GameObject)
				SAFE_DELETE(item->game_object);
		}
	}

	void OutlinerWidget::SetSelectionChangedCallback(SelectionChangedCallback callback, void* user_data)
	{
		m_SelectionChangedCallback = callback;
		m_SelectionChangedUserData = user_data;
	}

	void OutlinerWidget::Draw()
	{
		ImGui::Begin("Outliner");

		if (ImGui::BeginPopupContextWindow("OutlinerContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Game Object"))
				CreateTopLevelGameObject();
			if (ImGui::MenuItem("Create Folder"))
				CreateFolderItem(nullptr);
			ImGui::EndPopup();
		}

		Icon scene_icon = IconManager::Instance().GetOutlinerIcon("scene");
		IconManager::Instance().DrawIcon(scene_icon, 16.0f);
		ImGui::SameLine();

		ImGuiTreeNodeFlags scene_flags = ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_Selection == nullptr)
			scene_flags |= ImGuiTreeNodeFlags_Selected;

		bool scene_open = ImGui::TreeNodeEx("Scene", scene_flags);
		if (ImGui::IsItemClicked())
			SelectItem(nullptr);

		if (ImGui::BeginPopupContextItem("SceneContext"))
		{
			if (ImGui::MenuItem("Create Game Object"))
				CreateTopLevelGameObject();
			if (ImGui::MenuItem("Create Folder"))
				CreateFolderItem(nullptr);
			ImGui::EndPopup();
		}

		if (scene_open)
		{
			for (auto& item : m_Items)
				DrawItem(*item);
			ImGui::TreePop();
		}

		ImGui::End();
	}

	void OutlinerWidget::CreateTopLevelGameObject()
	{
		CreateGameObjectItem(nullptr);
	}

	void OutlinerWidget::AttachComponent(reflection::TypeID type_id)
	{
		game::GameObject* game_object = GetSelectedGameObject();
		if (game_object == nullptr)
			return;

		game_object->AddComponent(reflection::Type(type_id));
		NotifySelectionChanged();
	}

	game::GameObject* OutlinerWidget::GetSelectedGameObject() const
	{
		if (m_Selection == nullptr || m_Selection->type != ItemType::GameObject)
			return nullptr;

		return m_Selection->game_object;
	}

	void OutlinerWidget::DrawItem(Item& item)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth;
		if (item.children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (&item == m_Selection)
			flags |= ImGuiTreeNodeFlags_Selected;

		Icon icon = IconManager::Instance().GetOutlinerIcon(item.type == ItemType::Folder ? "folder" : "gameobject");
		IconManager::Instance().DrawIcon(icon, 16.0f);
		ImGui::SameLine();

		const std::string label = item.name + "##" + std::to_string(reinterpret_cast<std::uintptr_t>(&item));
		bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
		if (ImGui::IsItemClicked())
			SelectItem(&item);

		DrawContextMenu(&item);

		if (opened)
		{
			for (auto& child : item.children)
				DrawItem(*child);
			ImGui::TreePop();
		}
	}

	void OutlinerWidget::CreateGameObjectItem(Item* parent)
	{
		game::GameObject* game_object = new game::GameObject();

		auto item = std::make_unique<Item>();
		item->type = ItemType::GameObject;
		item->name = game_object->GetName();
		item->game_object = game_object;
		item->parent = parent;
		Item* item_ptr = item.get();

		if (parent != nullptr)
		{
			if (parent->type == ItemType::GameObject && parent->game_object != nullptr)
				parent->game_object->AddChild(game_object);
			parent->children.emplace_back(std::move(item));
		}
		else
		{
			m_Items.emplace_back(std::move(item));
		}

		SelectItem(item_ptr);
	}

	void OutlinerWidget::CreateFolderItem(Item* parent)
	{
		auto item = std::make_unique<Item>();
		item->type = ItemType::Folder;
		item->name = "New Folder";
		item->parent = parent;
		Item* item_ptr = item.get();

		if (parent != nullptr)
			parent->children.emplace_back(std::move(item));
		else
			m_Items.emplace_back(std::move(item));

		SelectItem(item_ptr);
	}

	void OutlinerWidget::DeleteItem(Item* item)
	{
		if (item == nullptr)
			return;

		auto erase_item = [item](std::vector<std::unique_ptr<Item>>& items) {
			auto found = std::find_if(items.begin(), items.end(), [item](const std::unique_ptr<Item>& candidate) {
				return candidate.get() == item;
			});
			if (found != items.end())
				items.erase(found);
		};

		if (item->type == ItemType::GameObject && item->game_object != nullptr)
		{
			game::GameObject* object = item->game_object;
			item->game_object = nullptr;
			SAFE_DELETE(object);
		}

		if (item->parent != nullptr)
			erase_item(item->parent->children);
		else
			erase_item(m_Items);

		m_Selection = nullptr;
		NotifySelectionChanged();
	}

	void OutlinerWidget::SelectItem(Item* item)
	{
		if (m_Selection == item)
			return;

		m_Selection = item;
		NotifySelectionChanged();
	}

	void OutlinerWidget::NotifySelectionChanged()
	{
		if (m_SelectionChangedCallback == nullptr)
			return;

		core::Object* object = nullptr;
		if (m_Selection != nullptr && m_Selection->type == ItemType::GameObject)
			object = static_cast<core::Object*>(m_Selection->game_object);

		m_SelectionChangedCallback(m_SelectionChangedUserData, object);
	}

	void OutlinerWidget::DrawContextMenu(Item* item)
	{
		if (!ImGui::BeginPopupContextItem())
			return;

		if (ImGui::MenuItem("Create Game Object"))
			CreateGameObjectItem(item);
		if (ImGui::MenuItem("Create Folder"))
			CreateFolderItem(item);
		if (ImGui::MenuItem("Delete", "Delete"))
			DeleteItem(item);

		ImGui::EndPopup();
	}
}

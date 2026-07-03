/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Runtime.h>

#include <memory>
#include <string>
#include <vector>

namespace editor
{
	class OutlinerWidget
	{
	public:
		using SelectionChangedCallback = void (*)(void* user_data, std::shared_ptr<core::Object> selection);

		OutlinerWidget();
		~OutlinerWidget();

		void SetSelectionChangedCallback(SelectionChangedCallback callback, void* user_data);
		void Draw();

		void CreateTopLevelGameObject();
		void AttachComponent(reflection::TypeID type_id);
		game::GameObject::SharedPtr GetSelectedGameObject() const;

	private:
		enum class ItemType
		{
			Folder,
			GameObject
		};

		struct Item
		{
			ItemType type = ItemType::Folder;
			std::string name;
			game::GameObject::SharedPtr game_object;
			std::weak_ptr<Item> parent;
			std::vector<std::shared_ptr<Item>> children;
		};

		using ItemPtr = std::shared_ptr<Item>;

		void DrawItem(const ItemPtr& item);
		void CreateGameObjectItem(const ItemPtr& parent);
		void CreateFolderItem(const ItemPtr& parent);
		void DeleteItem(const ItemPtr& item);
		void SelectItem(const ItemPtr& item);
		void NotifySelectionChanged();
		void DrawContextMenu(const ItemPtr& item);

		std::vector<ItemPtr> m_Items;
		ItemPtr m_Selection = nullptr;
		SelectionChangedCallback m_SelectionChangedCallback = nullptr;
		void* m_SelectionChangedUserData = nullptr;
	};
}

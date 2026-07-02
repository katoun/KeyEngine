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
		using SelectionChangedCallback = void (*)(void* user_data, core::Object* selection);

		OutlinerWidget();
		~OutlinerWidget();

		void SetSelectionChangedCallback(SelectionChangedCallback callback, void* user_data);
		void Draw();

		void CreateTopLevelGameObject();
		void AttachComponent(reflection::TypeID type_id);
		game::GameObject* GetSelectedGameObject() const;

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
			game::GameObject* game_object = nullptr;
			Item* parent = nullptr;
			std::vector<std::unique_ptr<Item>> children;
		};

		void DrawItem(Item& item);
		void CreateGameObjectItem(Item* parent);
		void CreateFolderItem(Item* parent);
		void DeleteItem(Item* item);
		void SelectItem(Item* item);
		void NotifySelectionChanged();
		void DrawContextMenu(Item* item);

		std::vector<std::unique_ptr<Item>> m_Items;
		Item* m_Selection = nullptr;
		SelectionChangedCallback m_SelectionChangedCallback = nullptr;
		void* m_SelectionChangedUserData = nullptr;
	};
}

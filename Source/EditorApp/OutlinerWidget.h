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

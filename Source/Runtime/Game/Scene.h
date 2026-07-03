/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Resource/Resource.h>

#include <memory>
#include <string>
#include <vector>

namespace game
{
	class GameObject;

	class RUNTIME_API Scene : public resource::Resource
	{
		DEFINE_OBJECT
	public:

		typedef std::shared_ptr<GameObject> GameObjectPtr;
		typedef std::vector<GameObjectPtr> GameObjects;

		Scene(const filesystem::path& path);
		~Scene();

		GameObjectPtr AddGameObject(GameObjectPtr game_object);
		void RemoveGameObject(GameObjectPtr game_object);

	private:

		GameObjects	m_GameObjects;
	};
}

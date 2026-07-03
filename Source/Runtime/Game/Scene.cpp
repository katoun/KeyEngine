/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Game/Scene.h>
#include <Game/GameObject.h>
#include <Resource/Resource.h>

namespace game
{
	Scene::Scene(const filesystem::path& path)
		: resource::Resource(path)
	{}

	Scene::~Scene()
	{
		m_GameObjects.clear();
	}

	Scene::GameObjectPtr Scene::AddGameObject(GameObjectPtr game_object)
	{
		if (game_object == nullptr)
			return nullptr;

		m_GameObjects.emplace_back(game_object);

		return game_object;
	}

	void Scene::RemoveGameObject(GameObjectPtr game_object)
	{
		if (game_object == nullptr)
			return;

		for (auto i = m_GameObjects.begin(); i != m_GameObjects.end(); ++i)
		{
			if (*i == game_object)
			{
				m_GameObjects.erase(i);
				return;
			}
		}
	}
}

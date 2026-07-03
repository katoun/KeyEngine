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

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Resource/ResourceManager.h>

namespace resource
{
	ResourceManager::ResourceManager(void){}

	void ResourceManager::LoadResources()
	{

	}

	void ResourceManager::UnloadResources()
	{

	}

	const filesystem::path& ResourceManager::GetBasePath() const
	{
		return m_BasePath;
	}

	ResourceManager &ResourceManager::Instance(void)
	{
		static ResourceManager instance;

		return instance;
	}
}
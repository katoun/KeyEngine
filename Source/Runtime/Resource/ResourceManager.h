/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Resource/Resource.h>

#include <memory>
#include <unordered_map>

namespace resource
{
	//class Resource;

	class RUNTIME_API ResourceManager
	{
	public:

		ResourceManager(void);

		template<typename T>
		std::shared_ptr<T> CreateResource(const filesystem::path& path);

		void LoadResources();
		void UnloadResources();

		const filesystem::path& GetBasePath() const;

		static ResourceManager &Instance(void);

	private:

		filesystem::path m_BasePath;

		std::unordered_map<uint32_t, std::shared_ptr<Resource>> m_Resources;
	};

	template<typename T>
	std::shared_ptr<T> ResourceManager::CreateResource(const filesystem::path& path)
	{
		//if (!std::is_base_of<Resource, T>::value)
		//	return nullptr;

		auto value = std::make_shared<T>(path);

		m_Resources.emplace(value->GetID(), value);

		return value;
	}
}

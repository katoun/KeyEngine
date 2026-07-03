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

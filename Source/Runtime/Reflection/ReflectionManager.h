/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/Package.h>
#include <Reflection/ReflectionData.h>

#include <vector>
#include <map>
#include <unordered_map>

namespace reflection
{
	class RUNTIME_API ReflectionManager
	{
	public:

		ReflectionManager(void);

		void RegisterPackage(Package& package);
		void RemovePackage(Package& package);
		void RemovePackage(const std::string& name);

		std::vector<Type> GetTypes(void) const;
		template<typename T>
		std::vector<Type> GetTypes(void) const;

		TypeData& GetTypeData(const TypeID id);

		static ReflectionManager &Instance(void);

	private:

		std::map<std::string, Package&> m_Packages;

		std::unordered_map<TypeID, Package&> m_IDPackageMap;
	};

	template<typename T>
	std::vector<Type> ReflectionManager::GetTypes(void) const
	{
		auto& of_type = typeof(T);

		std::vector<Type> ret;

		for (auto& it : m_Packages)
		{
			auto& ids = it.second.GetTypeIDs();
			for (auto& id : ids)
			{
				auto type = Type(id);

				if (type.IsTypeOf(of_type))
				{
					ret.emplace_back(type);
				}
			}
		}

		return ret;
	}
}

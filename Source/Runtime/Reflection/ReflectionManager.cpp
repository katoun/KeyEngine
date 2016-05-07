// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Reflection/ReflectionManager.h>

namespace reflection
{
	ReflectionManager::ReflectionManager(void)
	{
		// register all of the native type variants explicitly, before
		// anything else
		RegisterReflectionData();

		RegisterPackage(Package::Default);
	}

	void ReflectionManager::RegisterPackage(Package& package)
	{
		//std::cout << package.GetName() << ", " << package.GetID() << std::endl;

		m_Packages.emplace(package.GetName(), package);

		auto& ids = package.GetTypeIDs();
		for (auto& id : ids)
		{
			m_IDPackageMap.emplace(id, package);
		}

		//TODO: see if this package has the same name with a registered one!!!
	}

	void ReflectionManager::RemovePackage(Package& package)
	{
		RemovePackage(package.GetName());
	}

	void ReflectionManager::RemovePackage(const std::string& name)
	{
		auto& search = m_Packages.find(name);
		if (search == m_Packages.end())
			return;

		auto& package = (*search).second;

		auto& ids = package.GetTypeIDs();
		for (auto& id : ids)
		{
			auto& search = m_IDPackageMap.find(id);
			if (search != m_IDPackageMap.end())
			{
				m_IDPackageMap.erase(id);
			}
		}

		package.ClearData();
		m_Packages.erase(name);
	}

	std::vector<Type> ReflectionManager::GetTypes(void) const
	{
		std::vector<Type> ret;

		for (auto& it : m_Packages)
		{
			auto& ids = it.second.GetTypeIDs();
			for (auto& id : ids)
			{
				ret.emplace_back(Type(id));
			}
		}

		return ret;
	}

	TypeData& ReflectionManager::GetTypeData(const TypeID id)
	{
		auto search = m_IDPackageMap.find(id);
		if (search == m_IDPackageMap.end())
			return TypeData::Undefined;

		auto& package = (*search).second;

		return package.GetTypeData(id);
	}

	ReflectionManager &ReflectionManager::Instance(void)
	{
		static ReflectionManager instance;

		return instance;
	}
}
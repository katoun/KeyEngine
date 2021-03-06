/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/Package.h>

namespace reflection
{
	uint32_t Package::m_NextID = 0;

	Package& Package::Default = Package("Engine");

	Package::Package(const std::string& name)
	{
		m_Name = name;
		m_ID = m_NextID++;
	}

	const std::string& Package::GetName()
	{
		return m_Name;
	}

	const uint32_t Package::GetID()
	{
		return m_ID;
	}

	void Package::AddType(const TypeID id, TypeData data)
	{
		m_IDs.push_back(id);
		m_TypeDatas.emplace(id, data);
	}

	std::vector<TypeID>& Package::GetTypeIDs(void)
	{
		return m_IDs;
	}

	TypeData& Package::GetTypeData(const TypeID id)
	{
		auto search = m_TypeDatas.find(id);
		if (search == m_TypeDatas.end())
			return TypeData::Undefined;

		return search->second;
	}

	void Package::ClearData()
	{
		m_TypeDatas.clear();
	}
}
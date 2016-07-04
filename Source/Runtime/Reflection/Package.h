/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/TypeData.h>

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

namespace reflection
{
	class RUNTIME_API Package
	{
	public:

		Package(const std::string& name);

		const std::string& GetName();
		const uint32_t GetID();

		void AddType(const TypeID id, TypeData data);

		std::vector<TypeID>& GetTypeIDs(void);
		TypeData& GetTypeData(const TypeID id);

		void ClearData();

		static Package& Default;

	private:

		static uint32_t m_NextID;

		std::string m_Name;
		uint32_t m_ID;

		std::vector<TypeID> m_IDs;

		std::unordered_map<TypeID, TypeData> m_TypeDatas;
	};
}

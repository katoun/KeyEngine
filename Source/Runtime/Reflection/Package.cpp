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

#include <Reflection/Package.h>

namespace reflection
{
	uint32_t Package::m_NextID = 0;

	namespace
	{
		Package defaultPackage("Engine");
	}

	Package& Package::Default = defaultPackage;

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

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/Type.h>

#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>

#include <functional>
#include <vector>
#include <map>

namespace reflection
{
	struct Attribute;

	class RUNTIME_API AttibutesContainer
	{
	public:

		typedef std::initializer_list<std::pair<Type, const Attribute *>> Initializer;

		template<typename AttributeType>
		const AttributeType* GetAttribute(void) const;

		std::vector<std::pair<Type, const Attribute *>> GetAttributes(void) const;

	protected:

		virtual void SetAttributes(const Initializer &attributes);

	private:

		AttibutesContainer &operator=(const AttibutesContainer &rhs) = delete;

		friend class TypeData;

		std::map<Type, const Attribute*> m_Attributes;
	};

	template<typename AttributeType>
	const AttributeType* AttibutesContainer::GetAttribute(void) const
	{
		static_assert(std::is_base_of<Attribute, AttributeType>::value, "Type must be an Attribute.");

		auto& type = typeof(AttributeType);

		auto& search = m_Attributes.find(type);

		if (search == m_Attributes.end())
			return nullptr;

		return static_cast<const AttributeType*>(search->second);
	}
}

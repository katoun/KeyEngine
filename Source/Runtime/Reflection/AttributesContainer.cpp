/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/AttributesContainer.h>
#include <Reflection/Attribute.h>

#include <cassert>

namespace reflection
{
	std::vector<std::pair<Type, std::shared_ptr<const Attribute>>> AttributesContainer::GetAttributes(void) const
	{
		std::vector<std::pair<Type, std::shared_ptr<const Attribute>>> attributes;

		for (auto &it : m_Attributes)
		{
			attributes.emplace_back(std::make_pair(it.first, it.second));
		}

		return attributes;
	}

	void AttributesContainer::SetAttributes(const Initializer &attributes)
	{
		for (auto &pair : attributes)
		{
			assert(pair.first.IsValid());

			m_Attributes[pair.first] = pair.second;
		}
	}
}

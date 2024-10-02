/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/Any.h>
#include <Reflection/AttributesContainer.h>

#include <functional>
#include <string>
#include <map>

namespace attribute
{
	struct Getter;
	struct Setter;
}

namespace reflection
{
	struct Attribute;

	class RUNTIME_API Field : public AttributesContainer
	{
	public:

		Field(void);
		Field(const std::string &name, Type type, Type classType, const AttributesContainer::Initializer &attributes);

		const std::string &GetName(void) const;

		Type GetType(void) const;
		Type GetClassType(void) const;

		bool IsValid(void) const;
		bool IsReadOnly(void) const;

		Any GetValue(const Any &instance) const;
		bool SetValue(Any &instance, const Any &value) const;

	private:
		friend class TypeData;

		std::string m_Name;

		Type m_Type;
		Type m_ClassType;

		const attribute::Getter* m_Getter;
		const attribute::Setter* m_Setter;
	};
}

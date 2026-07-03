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
#include <Reflection/TypeConfig.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/Any.h>
#include <Reflection/AttributesContainer.h>

#include <functional>
#include <memory>
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

		std::shared_ptr<const attribute::Getter> m_Getter;
		std::shared_ptr<const attribute::Setter> m_Setter;
	};
}

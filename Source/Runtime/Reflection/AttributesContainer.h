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
#include <Reflection/Type.h>

#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>

#include <memory>
#include <vector>
#include <map>

namespace reflection
{
	struct Attribute;

	class RUNTIME_API AttributesContainer
	{
	public:

		typedef std::initializer_list<std::pair<Type, std::shared_ptr<const Attribute>>> Initializer;

		template<typename AttributeType>
		std::shared_ptr<const AttributeType> GetAttribute(void) const;

		std::vector<std::pair<Type, std::shared_ptr<const Attribute>>> GetAttributes(void) const;

	protected:

		virtual void SetAttributes(const Initializer &attributes);

	private:

		AttributesContainer &operator=(const AttributesContainer &rhs) = delete;

		friend class TypeData;

		std::map<Type, std::shared_ptr<const Attribute>> m_Attributes;
	};

	template<typename AttributeType>
	std::shared_ptr<const AttributeType> AttributesContainer::GetAttribute(void) const
	{
		static_assert(std::is_base_of<Attribute, AttributeType>::value, "Type must be an Attribute.");

		auto type = TypeOf<AttributeType>();

		auto search = m_Attributes.find(type);

		if (search == m_Attributes.end())
			return {};

		return std::static_pointer_cast<const AttributeType>(search->second);
	}
}

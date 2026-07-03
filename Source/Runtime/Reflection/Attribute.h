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

#include <memory>
#include <utility>

namespace reflection
{
	struct RUNTIME_API Attribute
	{};

	template<typename AttributeType, typename ...Args>
	std::pair<Type, std::shared_ptr<const Attribute>> AttributeInitializer(Args&&... args);

	template<typename AttributeType, typename ...Args>
	std::pair<Type, std::shared_ptr<const Attribute>> AttributeInitializer(Args&&... args)
	{
		static_assert(std::is_base_of<Attribute, AttributeType>::value, "Attributes must inherit from Attribute struct");

		return std::make_pair(TypeOf<AttributeType>(), std::make_shared<AttributeType>(std::forward<Args>(args)...));
	}
}

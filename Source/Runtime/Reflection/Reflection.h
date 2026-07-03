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

#ifndef __REFLECTION_PARSER__

#include <Reflection/Any.h>
#include <Reflection/Type.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>
#include <Reflection/Constructor.h>
#include <Reflection/Destructor.h>
#include <Reflection/Field.h>
#include <Reflection/Enum.h>
#include <Reflection/Attribute.h>
#include <Reflection/Attributes.h>

#define ATTRIBUTES(...)

#define DEFINE_OBJECT								\
	public:											\
	reflection::Type GetType(void) const override	\
	{												\
		return reflection::TypeOf<decltype(*this)>(); \
	}												\
	private:										\

#else

#define ATTRIBUTES(...) __attribute__((annotate(#__VA_ARGS__)))

#define DEFINE_OBJECT

#endif

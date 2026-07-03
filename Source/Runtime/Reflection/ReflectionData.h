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
#include <Reflection/TypeData.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/TypeConfig.h>

#include <Game/GameObject.h>
#include <Game/Component.h>
#include <Game/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

#define DECLARE_TYPE(type)									\
	template class RUNTIME_API reflection::TypeInfo<type>;	\

DECLARE_TYPE(void)
DECLARE_TYPE(int)
DECLARE_TYPE(bool)
DECLARE_TYPE(float)
DECLARE_TYPE(double)
DECLARE_TYPE(std::string)

DECLARE_TYPE(glm::vec2)
DECLARE_TYPE(glm::vec3)
DECLARE_TYPE(glm::quat)

DECLARE_TYPE(reflection::Attribute)
DECLARE_TYPE(attribute::Getter)
DECLARE_TYPE(attribute::Setter)
DECLARE_TYPE(attribute::Range)
DECLARE_TYPE(attribute::DisplayName)
DECLARE_TYPE(attribute::Category)

DECLARE_TYPE(core::Object)

DECLARE_TYPE(game::GameObject)
DECLARE_TYPE(game::Component)
DECLARE_TYPE(game::Transform)

namespace reflection
{
	RUNTIME_API void RegisterReflectionData();
}

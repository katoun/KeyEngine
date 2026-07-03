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

#include <Core/Utils.h>
#include <Core/Object.h>

#include <Game/MessageType.h>
#include <Game/GameObject.h>
#include <Game/Component.h>
#include <Game/Scene.h>

#include <Resource/SerializeTypes.h>
#include <Resource/Archive.h>
#include <Resource/ResourceState.h>
#include <Resource/ResourceEvent.h>
#include <Resource/ResourceEventReceiver.h>
#include <Resource/Resource.h>
#include <Resource/ResourceManager.h>

#include <Reflection/TypeConfig.h>
#include <Reflection/TypeInfo.h>
#include <Reflection/Type.h>
#include <Reflection/TypeData.h>
#include <Reflection/Enum.h>
#include <Reflection/Field.h>
#include <Reflection/Any.h>
#include <Reflection/Attribute.h>
#include <Reflection/Attributes.h>
#include <Reflection/Constructor.h>
#include <Reflection/Destructor.h>
#include <Reflection/ReflectionData.h>
#include <Reflection/ReflectionManager.h>

//TODO!!!
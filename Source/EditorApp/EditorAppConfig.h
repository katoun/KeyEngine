/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Platform.h>

#include <qmetatype.h>

#include <Reflection/Any.h>
#include <Reflection/Field.h>
#include <Game/GameObject.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

Q_DECLARE_METATYPE(core::Object*)
Q_DECLARE_METATYPE(game::GameObject*)
Q_DECLARE_METATYPE(reflection::Any)
Q_DECLARE_METATYPE(reflection::Field)
Q_DECLARE_METATYPE(glm::vec2)
Q_DECLARE_METATYPE(glm::vec3)
Q_DECLARE_METATYPE(glm::quat)

#define EDITOR_APP_VERSION "0.0.1"

#define SAFE_DELETE(object_ptr) if(object_ptr != nullptr) { delete object_ptr; object_ptr = nullptr; }
#define SAFE_DELETE_ARRAY(object_ptr) if(object_ptr != nullptr) { delete []object_ptr; object_ptr = nullptr; }

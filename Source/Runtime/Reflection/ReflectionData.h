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
	inline RUNTIME_API void RegisterReflectionData();
}

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Reflection/ReflectionData.h>
#include <Core/Utils.h>
#include <Reflection/Any.h>
#include <Reflection/Package.h>

#define REGISTER_BASE_TYPE(type)					\
		{											\
			auto id = core::string::Hash( #type );	\
			TypeInfo<type>::Register(id);			\
			auto& data = TypeData( #type );			\
			data.Initialize<type>();				\
			Package::Default.AddType(id, data);		\
		}											\

#define REGISTER_TYPE_BEGIN(type, name)																							\
		{																														\
			auto id = core::string::Hash( #type );																				\
			TypeInfo<type>::Register(id);																						\
			auto& data = TypeData( #type,																						\
			{																													\
				reflection::AttributeInitializer<attribute::DisplayName>(name),													\
			});																													\
			data.Initialize<type>();																							\

#define REGISTER_TYPE_BASE(base)																								\
			data.SetBaseTypes(id, { typeof(base) });																			\

#define REGISTER_TYPE_FIELD(class_type, type, name, display_name, getter, setter)												\
			data.AddField<class_type,type>(#name,																				\
			{																													\
				reflection::AttributeInitializer<attribute::Getter>([](const reflection::Any &object)							\
				{																												\
					if (object.GetType() == typeof(class_type))																	\
					{																											\
						auto &instance = object.GetValue<class_type>();															\
						return reflection::Any{ instance.getter() };															\
					}																											\
					else																										\
					{																											\
						auto &instance = object.GetValue<class_type*>();														\
						return reflection::Any{ instance->getter() };															\
					}																											\
				}),																												\
				reflection::AttributeInitializer<attribute::Setter>([](reflection::Any &object, const reflection::Any &value)	\
				{																												\
					if (object.GetType() == typeof(class_type))																	\
					{																											\
						auto &instance = object.GetValue<class_type>();															\
						instance.setter(value.GetValue<type>());																\
					}																											\
					else																										\
					{																											\
						auto &instance = object.GetValue<class_type*>();														\
						instance->setter(value.GetValue<type>());																\
					}																											\
				}),																												\
				reflection::AttributeInitializer<attribute::DisplayName>(display_name)											\
			});																													\

#define REGISTER_TYPE_END()																										\
			Package::Default.AddType(id, data);																					\
		}																														\

namespace reflection
{
	void RegisterReflectionData()
	{
		REGISTER_BASE_TYPE(void);
		REGISTER_BASE_TYPE(int);
		REGISTER_BASE_TYPE(bool);
		REGISTER_BASE_TYPE(float);
		REGISTER_BASE_TYPE(double);
		REGISTER_BASE_TYPE(std::string);

		REGISTER_BASE_TYPE(glm::vec2);
		REGISTER_BASE_TYPE(glm::vec3);
		REGISTER_BASE_TYPE(glm::quat);

		REGISTER_BASE_TYPE(Attribute);
		REGISTER_BASE_TYPE(attribute::Getter);
		REGISTER_BASE_TYPE(attribute::Setter);
		REGISTER_BASE_TYPE(attribute::Range);
		REGISTER_BASE_TYPE(attribute::DisplayName)
		REGISTER_BASE_TYPE(attribute::Category);

		REGISTER_BASE_TYPE(core::Object);

		REGISTER_BASE_TYPE(game::GameObject);
		REGISTER_BASE_TYPE(game::Component);

		REGISTER_TYPE_BEGIN(game::Transform, "Transform")
			REGISTER_TYPE_BASE(game::Component)
			REGISTER_TYPE_FIELD(game::Transform, glm::vec3, "m_Position", "Position", GetPosition, SetPosition)
			REGISTER_TYPE_FIELD(game::Transform, glm::vec3, "m_EulerAngles", "Rotation", GetEulerAngles, SetEulerAngles)
			REGISTER_TYPE_FIELD(game::Transform, glm::vec3, "m_Scale", "Scale", GetScale, SetScale)
		REGISTER_TYPE_END()
	}
}
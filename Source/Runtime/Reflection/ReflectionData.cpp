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

#include <Reflection/ReflectionData.h>
#include <Reflection/Any.h>
#include <Reflection/Package.h>

#include <glm/glm.hpp>

#define REGISTER_BASE_TYPE(type)					\
		{											\
			auto [id, data] = RegisterClass<type>(#type); \
			Package::Default.AddType(id, data);		\
		}											\

#define REGISTER_TYPE_BEGIN(type, name)																							\
		{																														\
			auto [id, data] = RegisterClass<type>(#type,																		\
			{																													\
				reflection::AttributeInitializer<attribute::DisplayName>(name),													\
			});																													\

#define REGISTER_TYPE_BASE(base)																								\
			data.SetBaseTypes(id, { reflection::TypeOf<base>() });																\

#define REGISTER_TYPE_FIELD(class_type, type, name, display_name, getter, setter)												\
			data.AddField<class_type,type>(#name,																				\
			{																													\
				reflection::AttributeInitializer<attribute::Getter>([](const reflection::Any &object)							\
				{																												\
					if (object.GetType() == reflection::TypeOf<class_type>())													\
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
					if (object.GetType() == reflection::TypeOf<class_type>())													\
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

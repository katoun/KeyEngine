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

#define REGISTER_COMPONENT(type, name)											\
		{																		\
			auto id = core::string::Hash( #type );								\
			TypeInfo<type>::Register(id);										\
			auto& data = TypeData( #type,										\
			{																	\
				reflection::AttributeInitializer<attribute::DisplayName>(name),	\
			});																	\
			data.Initialize<type>();											\
			data.SetBaseTypes(id, { typeof(game::Component) });					\
			Package::Default.AddType(id, data);									\
		}																		\

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

		//Transform
		{
			auto id = core::string::Hash("Transform");
			reflection::TypeInfo<game::Transform>::Register(id);
			auto& data = reflection::TypeData("Transform",
			{
				reflection::AttributeInitializer<attribute::DisplayName>("Transform")
			});
			data.Initialize<game::Transform>();
			data.SetBaseTypes(id, { typeof(game::Component) });
			data.AddField<game::Transform, glm::vec3>("m_Position",
			{
				reflection::AttributeInitializer<attribute::Getter>([](const reflection::Any &object)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						return reflection::Any{ instance.GetPosition() };
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						return reflection::Any{ instance->GetPosition() };
					}
				}),
				reflection::AttributeInitializer<attribute::Setter>([](reflection::Any &object, const reflection::Any &value)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						instance.SetPosition(value.GetValue<glm::vec3>());
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						instance->SetPosition(value.GetValue<glm::vec3>());
					}
				}),
				reflection::AttributeInitializer<attribute::DisplayName>("Position")
			});
			data.AddField<game::Transform, glm::vec3>("m_EulerAngles",
			{
				reflection::AttributeInitializer<attribute::Getter>([](const reflection::Any &object)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						return reflection::Any{ instance.GetEulerAngles() };
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						return reflection::Any{ instance->GetEulerAngles() };
					}
				}),
				reflection::AttributeInitializer<attribute::Setter>([](reflection::Any &object, const reflection::Any &value)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						instance.SetEulerAngles(value.GetValue<glm::vec3>());
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						instance->SetEulerAngles(value.GetValue<glm::vec3>());
					}
				}),
				reflection::AttributeInitializer<attribute::DisplayName>("Rotation")
			});
			data.AddField<game::Transform, glm::vec3>("m_Scale",
			{
				reflection::AttributeInitializer<attribute::Getter>([](const reflection::Any &object)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						return reflection::Any{ instance.GetScale() };
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						return reflection::Any{ instance->GetScale() };
					}
				}),
				reflection::AttributeInitializer<attribute::Setter>([](reflection::Any &object, const reflection::Any &value)
				{
					if (object.GetType() == typeof(game::Transform))
					{
						auto &instance = object.GetValue<game::Transform>();
						instance.SetScale(value.GetValue<glm::vec3>());
					}
					else
					{
						auto &instance = object.GetValue<game::Transform*>();
						instance->SetScale(value.GetValue<glm::vec3>());
					}
				}),
				reflection::AttributeInitializer<attribute::DisplayName>("Scale")
			});
			Package::Default.AddType(id, data);
		}
	}
}
#pragma once

#include <Reflection/ReflectionManager.h>
#include <Reflection/TypeConfig.h>
#include <Core/Utils.h>

#include <Project.h>

using namespace attribute;

template class PROJECT_API reflection::TypeInfo<MyComponent>;

reflection::Package ProjectPackage("Project");

void RegisterReflectionData()
{	
	{
		auto id = core::string::Hash("MyComponent");
		reflection::TypeInfo<MyComponent>::Register(id);
		auto& data = reflection::TypeData("MyComponent", 
		{
						
			reflection::AttributeInitializer<Category>("MyCategory"), 
			reflection::AttributeInitializer<DisplayName>("MyComponent")
			
		});
		data.Initialize<MyComponent>();
		
		data.SetBaseTypes(id, { typeof(game::Component) });
		
		data.AddField<MyComponent, float>("m_Speed",
		{
						
			reflection::AttributeInitializer<Getter>([](const reflection::Any &object)
			{
				if (object.GetType() == typeof(MyComponent))
				{
					auto &instance = object.GetValue<MyComponent>();
					return reflection::Any{ instance.m_Speed };
				}
				else
				{
					auto &instance = object.GetValue<MyComponent*>();
					return reflection::Any{ instance->m_Speed };
				}
			}),
			
			reflection::AttributeInitializer<Setter>([](reflection::Any &object, const reflection::Any &value)
			{
				if (object.GetType() == typeof(MyComponent))
				{
					auto &instance = object.GetValue<MyComponent>();
					instance.m_Speed = value.GetValue<float>();
				}
				else
				{
					auto &instance = object.GetValue<MyComponent*>();
					instance->m_Speed = value.GetValue<float>();
				}
			}),
			reflection::AttributeInitializer<Range>(0.0f, 10.0f), 
			reflection::AttributeInitializer<DisplayName>("Speed")
			
		});
		data.AddField<MyComponent, float>("m_Test",
		{
						
			reflection::AttributeInitializer<Getter>([](const reflection::Any &object)
			{
				if (object.GetType() == typeof(MyComponent))
				{
					auto &instance = object.GetValue<MyComponent>();
					return reflection::Any{ instance.m_Test };
				}
				else
				{
					auto &instance = object.GetValue<MyComponent*>();
					return reflection::Any{ instance->m_Test };
				}
			}),
			
			reflection::AttributeInitializer<Setter>([](reflection::Any &object, const reflection::Any &value)
			{
				if (object.GetType() == typeof(MyComponent))
				{
					auto &instance = object.GetValue<MyComponent>();
					instance.m_Test = value.GetValue<float>();
				}
				else
				{
					auto &instance = object.GetValue<MyComponent*>();
					instance->m_Test = value.GetValue<float>();
				}
			}),
			reflection::AttributeInitializer<DisplayName>("Test")
			
		});
		ProjectPackage.AddType(id, data);
	}
	reflection::ReflectionManager::Instance().RegisterPackage(ProjectPackage);
}

void UnloadReflectionData()
{
	reflection::ReflectionManager::Instance().RemovePackage(ProjectPackage);
}
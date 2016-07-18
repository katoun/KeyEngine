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

#include <Reflection/ReflectionManager.h>
#include <Reflection/TypeConfig.h>
#include <Core/Utils.h>

#include <Project.h>

using namespace attribute;

template class PROJECT_API reflection::TypeInfo<std::ios_base>;
template class PROJECT_API reflection::TypeInfo<MyComponent>;

reflection::Package ProjectPackage("Project");

void RegisterReflectionData()
{	
	{
		auto id = core::string::Hash("MyComponent");
		reflection::TypeInfo<MyComponent>::Register(id);
		auto& data = reflection::TypeData("MyComponent", 
		{
						
			reflection::AttributeInitializer<DisplayName>("MyComponent")
			
		});
		data.Initialize<MyComponent>();
		
		data.SetBaseTypes(id, { typeof(game::Component) });
		
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
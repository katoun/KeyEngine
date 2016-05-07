/*
-----------------------------------------------------------------------------
Key Engine (http://github.com/katoun/KeyEngine) is made available under the MIT License.

Copyright (c) 2016 Catalin Alexandru Nastase
-----------------------------------------------------------------------------
*/

#include <ProjectConfig.h>
#include <Runtime.h>
#include <Project.h>
#include <ReflectionData.Generated.h>

extern "C" void PROJECT_API LoadModule()
{
	RegisterReflectionData();
}

extern "C" void PROJECT_API UnloadModule()
{
	UnloadReflectionData();
}

#ifndef PROJECT_DLL

int main(int argc, char **argv)
{
	RegisterReflectionData();

	//TEST!!!
	reflection::Type my_component_type = typeof(MyComponent);
	reflection::Field test_field = my_component_type.GetField("m_Test");

	reflection::Any my_component_any = my_component_type.Create();

	//test_field.SetValue(my_component_any, 5.0f);

	float value = test_field.GetValue(my_component_any).GetValue<float>();

	game::GameObject* go = new game::GameObject();

	reflection::Any my_component_any2 = go->AddComponent(my_component_type);

	float value2 = test_field.GetValue(my_component_any2).GetValue<float>();

	int x = 3;
	//TEST!!!

	//TODO: run the game!!!

	return 0;
}

#endif
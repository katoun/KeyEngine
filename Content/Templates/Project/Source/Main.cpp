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
	reflection::Type my_component_type = reflection::TypeOf<MyComponent>();
	reflection::Field test_field = my_component_type.GetField("m_Test");

	reflection::Any my_component_any = my_component_type.Create();

	//test_field.SetValue(my_component_any, 5.0f);

	float value = test_field.GetValue(my_component_any).GetValue<float>();

	auto go = game::GameObject::Create();

	auto my_component = go->AddComponent(my_component_type);
	reflection::Any my_component_any2 = my_component.get();

	float value2 = test_field.GetValue(my_component_any2).GetValue<float>();

	int x = 3;
	//TEST!!!

	//TODO: run the game!!!

	return 0;
}

#endif

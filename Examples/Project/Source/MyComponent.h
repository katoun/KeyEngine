#pragma once

#include <ProjectConfig.h>
#include <Game/Component.h>
#include <Reflection/Reflection.h>

using namespace attribute;

class ATTRIBUTES(Category("MyCategory"))
MyComponent : public game::Component
{
	DEFINE_OBJECT
public:

	MyComponent();

	ATTRIBUTES(Range(0.0f, 10.0f))
	float m_Speed;

	float m_Test;
};

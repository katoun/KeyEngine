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

#include <Game/Component.h>
#include <Game/GameObject.h>
#include <Core/Utils.h>

namespace game
{
	std::uint32_t Component::m_IndexCounter = 0;

	Component::Component()
		: core::Object()
		, m_Name("Component_" + core::string::FromInt(m_IndexCounter++))
		, m_Enabled{ true }
	{}

	Component::~Component() {}

	const std::string& Component::getName() const
	{
		return m_Name;
	}

	const bool Component::IsEnabled() const
	{
		return m_Enabled;
	}

	void Component::SetEnabled(const bool enabled)
	{
		if (m_Enabled == enabled)
			return;

		m_Enabled = enabled;

		if (m_Enabled)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}
	}

	std::shared_ptr<GameObject> Component::GetGameObject() const
	{
		return m_GameObject.lock();
	}

	void Component::OnStart(void) {}

	void Component::OnUpdate(float dt) {}

	void Component::OnStop(void) {}

	void Component::OnMessage(const MessageType message) {}

	void Component::OnEnable(void) {}
	void Component::OnDisable(void) {}

} // end namespace game

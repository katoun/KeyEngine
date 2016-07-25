/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
		, m_GameObject{ nullptr }
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

	GameObject* Component::GetGameObject()
	{
		return m_GameObject;
	}

	void Component::OnStart(void) {}

	void Component::OnUpdate(float dt) {}

	void Component::OnStop(void) {}

	void Component::OnMessage(const MessageType message) {}

	void Component::OnEnable(void) {}
	void Component::OnDisable(void) {}

} // end namespace game

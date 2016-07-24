/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Resource/Resource.h>
#include <Core/Utils.h>

namespace resource
{
	Resource::Resource(const filesystem::path& path)
		: core::Object()
		, m_Path(path)
		, m_ID(core::string::Hash(path.string()))
		, m_State(ResourceState::UNLOADED)
	{
		m_Event.source = this;
	}

	Resource::~Resource(void)
	{}

	const uint32_t& Resource::GetID() const
	{
		return m_ID;
	}

	const filesystem::path& Resource::GetPath(void) const
	{
		return m_Path;
	}

	const ResourceState& Resource::GetState(void) const
	{
		return m_State;
	}

	void Resource::RegisterEventReceiver(ResourceEventReceiver* receiver)
	{
		if (receiver == nullptr)
			return;

		m_EventReceivers.emplace_back(receiver);

		if (m_State == ResourceState::LOADED)
		{
			SendLoadedEvent(receiver);
		}
	}

	void Resource::RemoveEventReceiver(ResourceEventReceiver* receiver)
	{
		if (receiver == nullptr)
			return;
		for (auto i = m_EventReceivers.begin(); i != m_EventReceivers.end(); ++i)
		{
			if ((*i) == receiver)
			{
				m_EventReceivers.erase(i);
				return;
			}
		}
	}

	void Resource::SendLoadedEvent(ResourceEventReceiver* receiver)
	{
		if (receiver == nullptr)
			return;

		receiver->ResourceLoaded(m_Event);
	}

	void Resource::SendLoadedEvent(void)
	{
		for (auto i = m_EventReceivers.begin(); i != m_EventReceivers.end(); ++i)
		{
			SendLoadedEvent(*i);
		}
	}

	void Resource::SendUnloadedEvent(ResourceEventReceiver* receiver)
	{
		if (receiver == nullptr)
			return;

		receiver->ResourceUnloaded(m_Event);
	}

	void Resource::SendUnloadedEvent(void)
	{
		for (auto i = m_EventReceivers.begin(); i != m_EventReceivers.end(); ++i)
		{
			SendUnloadedEvent(*i);
		}
	}
}
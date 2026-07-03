/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Resource/Resource.h>
#include <Core/Utils.h>

#include <algorithm>

namespace resource
{
	Resource::Resource(const filesystem::path& path)
		: core::Object()
		, m_Path(path)
		, m_ID(core::string::Hash(path.string()))
		, m_State(ResourceState::UNLOADED)
	{
		m_Event.source = *this;
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

	void Resource::RegisterEventReceiver(const std::weak_ptr<ResourceEventReceiver>& receiver)
	{
		auto shared_receiver = receiver.lock();
		if (shared_receiver == nullptr)
			return;

		m_EventReceivers.emplace_back(receiver);

		if (m_State == ResourceState::LOADED)
		{
			SendLoadedEvent(shared_receiver);
		}
	}

	void Resource::RemoveEventReceiver(const std::weak_ptr<ResourceEventReceiver>& receiver)
	{
		auto shared_receiver = receiver.lock();
		if (shared_receiver == nullptr)
			return;

		m_EventReceivers.erase(std::remove_if(m_EventReceivers.begin(), m_EventReceivers.end(),
			[&shared_receiver](const std::weak_ptr<ResourceEventReceiver>& current)
			{
				auto locked = current.lock();
				return locked == nullptr || locked == shared_receiver;
			}), m_EventReceivers.end());
	}

	void Resource::SendLoadedEvent(const std::shared_ptr<ResourceEventReceiver>& receiver)
	{
		if (receiver == nullptr)
			return;

		receiver->ResourceLoaded(m_Event);
	}

	void Resource::SendLoadedEvent(void)
	{
		for (auto i = m_EventReceivers.begin(); i != m_EventReceivers.end();)
		{
			auto receiver = i->lock();
			if (receiver == nullptr)
			{
				i = m_EventReceivers.erase(i);
				continue;
			}

			SendLoadedEvent(receiver);
			++i;
		}
	}

	void Resource::SendUnloadedEvent(const std::shared_ptr<ResourceEventReceiver>& receiver)
	{
		if (receiver == nullptr)
			return;

		receiver->ResourceUnloaded(m_Event);
	}

	void Resource::SendUnloadedEvent(void)
	{
		for (auto i = m_EventReceivers.begin(); i != m_EventReceivers.end();)
		{
			auto receiver = i->lock();
			if (receiver == nullptr)
			{
				i = m_EventReceivers.erase(i);
				continue;
			}

			SendUnloadedEvent(receiver);
			++i;
		}
	}
}

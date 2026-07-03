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

	void Resource::RegisterEventReceiver(const ResourceEventReceiver::WeakPtr& receiver)
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

	void Resource::RemoveEventReceiver(const ResourceEventReceiver::WeakPtr& receiver)
	{
		auto shared_receiver = receiver.lock();
		if (shared_receiver == nullptr)
			return;

		m_EventReceivers.erase(std::remove_if(m_EventReceivers.begin(), m_EventReceivers.end(),
			[&shared_receiver](const ResourceEventReceiver::WeakPtr& current)
			{
				auto locked = current.lock();
				return locked == nullptr || locked == shared_receiver;
			}), m_EventReceivers.end());
	}

	void Resource::SendLoadedEvent(const ResourceEventReceiver::SharedPtr& receiver)
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

	void Resource::SendUnloadedEvent(const ResourceEventReceiver::SharedPtr& receiver)
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

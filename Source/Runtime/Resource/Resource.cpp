/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Resource/Resource.h>

namespace resource
{
	Resource::Resource(const filesystem::path& path)
		: core::Object()
		, m_Path(path)
		, m_State(ResourceState::UNLOADED)
	{}

	Resource::~Resource()
	{}

	const filesystem::path& Resource::GetPath() const
	{
		return m_Path;
	}

	const ResourceState& Resource::GetState() const
	{
		return m_State;
	}

	void Resource::RegisterEventReceiver(ResourceEventReceiver& reveicer)
	{

	}

	void Resource::RemoveEventReceiver(ResourceEventReceiver& reveicer)
	{

	}

	void Resource::SendLoadedEvent()
	{

	}

	void Resource::SendUnloadedEvent()
	{

	}
}
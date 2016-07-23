/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Core/Object.h>
#include <Reflection/Reflection.h>
#include <Resource/ResourceState.h>
#include <Resource/ResourceEvent.h>
#include <Resource/ResourceEventReceiver.h>

#include <vector>

namespace resource
{
	class RUNTIME_API Resource : public core::Object
	{
		DEFINE_OBJECT
	public:

		typedef std::vector<ResourceEventReceiver*> EventReceivers;

		Resource(const filesystem::path& path);

		virtual ~Resource(void);

		const filesystem::path& GetPath(void) const;
		const ResourceState& GetState(void) const;

		void RegisterEventReceiver(ResourceEventReceiver* receiver);
		void RemoveEventReceiver(ResourceEventReceiver* receiver);

	protected:

		void SendLoadedEvent(ResourceEventReceiver* receiver);
		void SendLoadedEvent(void);
		void SendUnloadedEvent(ResourceEventReceiver* receiver);
		void SendUnloadedEvent(void);

		filesystem::path m_Path;
		ResourceState m_State;

		ResourceEvent m_Event;

		EventReceivers m_EventReceivers;
	};
}

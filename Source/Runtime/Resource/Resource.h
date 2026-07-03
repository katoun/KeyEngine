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

#include <memory>
#include <vector>

namespace resource
{
	class RUNTIME_API Resource : public core::Object
	{
		DEFINE_OBJECT
	public:

		typedef std::vector<ResourceEventReceiver::WeakPtr> EventReceivers;

		Resource(const filesystem::path& path);

		virtual ~Resource(void);

		const uint32_t& GetID() const;
		const filesystem::path& GetPath(void) const;
		const ResourceState& GetState(void) const;

		void RegisterEventReceiver(const ResourceEventReceiver::WeakPtr& receiver);
		void RemoveEventReceiver(const ResourceEventReceiver::WeakPtr& receiver);

	protected:

		void SendLoadedEvent(const ResourceEventReceiver::SharedPtr& receiver);
		void SendLoadedEvent(void);
		void SendUnloadedEvent(const ResourceEventReceiver::SharedPtr& receiver);
		void SendUnloadedEvent(void);

		uint32_t m_ID;
		filesystem::path m_Path;
		ResourceState m_State;

		ResourceEvent m_Event;

		EventReceivers m_EventReceivers;
	};
}

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

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Core/Object.h>
#include <Reflection/Reflection.h>

#include <string>

namespace game
{
	enum class MessageType;
	class GameObject;

	class RUNTIME_API Component : core::Object
	{
		DEFINE_OBJECT

	public:

		Component();
		virtual ~Component();

		const std::string& getName() const;

		const bool IsEnabled() const;
		void SetEnabled(const bool enabled);

		GameObject* GetGameObject();

	protected:

		friend class GameObject;

		virtual void OnStart(void);
		virtual void OnUpdate(float dt);
		virtual void OnStop(void);
		virtual void OnMessage(MessageType message);

		void OnEnable(void);
		void OnDisable(void);

		static std::uint32_t m_IndexCounter;

		std::string m_Name;

		bool m_Enabled;

		GameObject* m_GameObject;
	};

} // end namespace game
// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Core/Object.h>
#include <Reflection/Reflection.h>

#include <string>

namespace game
{
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
		virtual void OnMessage(const std::uint32_t message);

		void OnEnable(void);
		void OnDisable(void);

		static std::uint32_t m_IndexCounter;

		std::string m_Name;

		bool m_Enabled;

		GameObject* m_GameObject;
	};

} // end namespace game
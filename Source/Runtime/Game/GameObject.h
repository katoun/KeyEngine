/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Core/Object.h>
#include <Game/Component.h>
#include <Reflection/Reflection.h>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

namespace game
{
	class Component;

	class RUNTIME_API GameObject : public core::Object
	{
		DEFINE_OBJECT

	public:

		typedef std::unordered_map<reflection::TypeID, Component*> Components;
		typedef std::unordered_map<reflection::TypeID, const reflection::Any> ComponentsAny;
		typedef std::vector<GameObject*> GameObjects;

		GameObject(void);
		GameObject(const std::string& name);

		~GameObject(void);

		//! Returns the id of the game object.
		const std::uint32_t& GetIndex(void) const;

		const std::string& GetName(void) const;
		void SetName(const std::string& name);

		const bool IsActive() const;
		void SetActive(const bool active);

		//! Gets this GameObject's parent.
		GameObject* GetParent(void);

		//! Sets this GameObject's parent.
		void SetParent(GameObject* parent);

		//! Adds a child to this GameObject.
		void AddChild(GameObject* child);

		void DetachChild(GameObject* child);
		GameObject* DetachChild(const std::uint32_t& index);

		void RemoveChild(GameObject* child);
		void RemoveChild(const std::uint32_t& index);

		void RemoveChildren(void);

		const GameObjects& GetChildren();

		Component* AddComponent(reflection::Type type);

		template<class ComponentType>
		ComponentType* AddComponent(void);

		Component* GetComponent(reflection::Type type) const;

		template<class ComponentType>
		ComponentType* GetComponent(void) const;

		Components GetComponents();
		ComponentsAny GetComponentsAny();

		template<class ComponentType>
		void RemoveComponent(void);

		void RemoveComponents(void);

		void SendMessage(const std::uint32_t message);

		void OnStart(void);
		void OnUpdate(float dt);
		void OnStop(void);

	protected:

		void OnMessage(const std::uint32_t message);
		void OnEnable(void);
		void OnDisable(void);

		static std::uint32_t m_IndexCounter;

		std::uint32_t m_Index;

		std::string m_Name;

		bool m_Active;

		GameObject* m_Parent;

		GameObjects m_Children;

		Components m_Components;
		ComponentsAny m_ComponentsAny;
	};

	template<class ComponentType>
	ComponentType* GameObject::AddComponent(void)
	{
		ComponentType* component = GetComponent<ComponentType>();
		if (component != nullptr)
		{
			return nullptr;
		}

		component = new ComponentType();
		component->m_GameObject = this;
		component->OnMessage(Message::COMPONENT_ATTACHED);

		m_Components.emplace(type.GetID(), component);
		m_ComponentsAny.emplace(type.GetID(), reflection::Any{ component });

		return component;
	}

	template<class ComponentType>
	ComponentType* GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<Component, ComponentType>::value, "Type must be a Component.");

		auto& type = typeof(ComponentType);
		
		auto& search = m_Components.find(type.GetID());

		if (search == m_Components.end())
		{
			return nullptr;
		}

		return static_cast<ComponentType*>(search->second);
	}

	template<class ComponentType>
	void GameObject::RemoveComponent(void)
	{
		static_assert(std::is_base_of<Component, ComponentType>::value, "Type must be a Component.");

		auto& type = typeof(ComponentType);

		auto& search = m_Components.find(type.GetID());
		if (search == m_Components.end())
			return;

		Component* component = search->second;
		assert(component != nullptr);
		component->OnMessage(Message::COMPONENT_DETACHED);

		m_Components.erase(search);

		SAFE_DELETE(component);

		auto& search_any = m_ComponentsAny.find(type.GetID());
		if (search_any == m_ComponentsAny.end())
			return;

		m_ComponentsAny.erase(search_any);
	}
} // end namespace game
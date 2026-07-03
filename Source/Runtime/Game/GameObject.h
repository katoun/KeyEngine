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
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

namespace game
{
	class Component;

	class RUNTIME_API GameObject : public core::SharedObject<GameObject>
	{
		DEFINE_OBJECT

	public:

		typedef std::shared_ptr<GameObject> SharedPtr;
		typedef std::weak_ptr<GameObject> WeakPtr;
		typedef std::unordered_map<reflection::TypeID, Component::SharedPtr> ComponentStorage;
		typedef ComponentStorage Components;
		typedef std::unordered_map<reflection::TypeID, const reflection::Any> ComponentsAny;
		typedef std::vector<SharedPtr> List;

		static SharedPtr Create(void);
		static SharedPtr Create(const std::string& name);

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(GameObject&&) = delete;

		~GameObject(void);

		//! Returns the id of the game object.
		const std::uint32_t& GetIndex(void) const;

		const std::string& GetName(void) const;
		void SetName(const std::string& name);

		const bool IsActive() const;
		void SetActive(const bool active);

		//! Gets this GameObject's parent.
		SharedPtr GetParent(void) const;

		//! Sets this GameObject's parent.
		void SetParent(SharedPtr parent);

		//! Adds a child to this GameObject.
		SharedPtr AddChild(SharedPtr child);

		SharedPtr DetachChild(SharedPtr child);
		SharedPtr DetachChild(const std::uint32_t& index);

		void RemoveChild(SharedPtr child);
		void RemoveChild(const std::uint32_t& index);

		void RemoveChildren(void);

		const List& GetChildren() const;

		Component::SharedPtr AddComponent(reflection::Type type);

		template<class ComponentType>
		std::shared_ptr<ComponentType> AddComponent(void);

		Component::SharedPtr GetComponent(reflection::Type type) const;

		template<class ComponentType>
		std::shared_ptr<ComponentType> GetComponent(void) const;

		Components GetComponents();
		ComponentsAny GetComponentsAny();

		template<class ComponentType>
		void RemoveComponent(void);

		void RemoveComponents(void);

		void SendMessage(MessageType message);

		void OnStart(void);
		void OnUpdate(float dt);
		void OnStop(void);

	protected:

		GameObject(void);
		GameObject(const std::string& name);

		void OnMessage(MessageType message);
		void OnEnable(void);
		void OnDisable(void);

		static std::uint32_t m_IndexCounter;

		std::uint32_t m_Index;

		std::string m_Name;

		bool m_Active;

		WeakPtr m_Parent;

		List m_Children;

		ComponentStorage m_Components;
		ComponentsAny m_ComponentsAny;
	};

	template<class ComponentType>
	std::shared_ptr<ComponentType> GameObject::AddComponent(void)
	{
		auto component = GetComponent<ComponentType>();
		if (component != nullptr)
		{
			return nullptr;
		}

		component = std::make_shared<ComponentType>();
		component->m_GameObject = shared_from_this();
		component->OnMessage(MessageType::COMPONENT_ATTACHED);

		auto type = reflection::TypeOf<ComponentType>();
		m_Components.emplace(type.GetID(), component);
		m_ComponentsAny.emplace(type.GetID(), type.CreateDynamicPointer(*component));

		return component;
	}

	template<class ComponentType>
	std::shared_ptr<ComponentType> GameObject::GetComponent() const
	{
		static_assert(std::is_base_of<Component, ComponentType>::value, "Type must be a Component.");

		auto type = reflection::TypeOf<ComponentType>();
		
		auto search = m_Components.find(type.GetID());

		if (search == m_Components.end())
		{
			return nullptr;
		}

		return std::static_pointer_cast<ComponentType>(search->second);
	}

	template<class ComponentType>
	void GameObject::RemoveComponent(void)
	{
		static_assert(std::is_base_of<Component, ComponentType>::value, "Type must be a Component.");

		auto type = reflection::TypeOf<ComponentType>();

		auto search = m_Components.find(type.GetID());
		if (search == m_Components.end())
			return;

		auto component = search->second;
		assert(component != nullptr);
		component->OnMessage(MessageType::COMPONENT_DETACHED);

		m_Components.erase(search);

		auto search_any = m_ComponentsAny.find(type.GetID());
		if (search_any == m_ComponentsAny.end())
			return;

		m_ComponentsAny.erase(search_any);
	}
} // end namespace game

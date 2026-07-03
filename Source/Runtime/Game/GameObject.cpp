/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Game/GameObject.h>
#include <Game/MessageType.h>
#include <Reflection/Any.h>
#include <Reflection/TypeInfo.h>
#include <Core/Utils.h>

#include <algorithm>
#include <cassert>

namespace game
{
	std::uint32_t GameObject::m_IndexCounter = 0;

	GameObject::SharedPtr GameObject::Create()
	{
		return SharedPtr(new GameObject());
	}

	GameObject::SharedPtr GameObject::Create(const std::string& name)
	{
		return SharedPtr(new GameObject(name));
	}

	GameObject::GameObject()
		: core::SharedObject<GameObject>()
		, m_Index{ m_IndexCounter++ }
		, m_Name("GameObject_" + core::string::FromInt(m_Index))
		, m_Active{ true }
	{}

	GameObject::GameObject(const std::string& name)
		: core::SharedObject<GameObject>()
		, m_Index{ m_IndexCounter++ }
		, m_Name(name)
		, m_Active{ true }
	{}

	GameObject::~GameObject()
	{
		RemoveComponents();
		RemoveChildren();
	}

	const std::uint32_t& GameObject::GetIndex() const
	{
		return m_Index;
	}

	const std::string& GameObject::GetName() const
	{
		return m_Name;
	}

	void GameObject::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const bool GameObject::IsActive() const
	{
		return m_Active;
	}

	void GameObject::SetActive(const bool active)
	{
		if (m_Active == active)
			return;

		m_Active = active;

		if (m_Active)
		{
			OnEnable();
		}
		else
		{
			OnDisable();
		}

		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->SetActive(active);
		}
	}

	GameObject::SharedPtr GameObject::GetParent() const
	{
		return m_Parent.lock();
	}

	void GameObject::SetParent(SharedPtr parent)
	{
		if (m_Parent.lock() == parent)
			return;

		auto self = shared_from_this();
		if (auto current_parent = m_Parent.lock())
		{
			current_parent->DetachChild(self);
		}

		if (parent != nullptr)
		{
			parent->AddChild(self);
			return;
		}

		m_Parent.reset();
		m_Index = m_IndexCounter++;
		SendMessage(MessageType::PARENT_CHANGED);
	}

	GameObject::SharedPtr GameObject::AddChild(SharedPtr child)
	{
		assert(child != nullptr);
		if (child == nullptr)
			return nullptr;

		auto self = shared_from_this();
		if (child == self)
			return nullptr;

		if (auto current_parent = child->m_Parent.lock())
		{
			if (current_parent == self)
				return child;

			current_parent->DetachChild(child);
		}

		m_Children.emplace_back(child);
		child->m_Parent = self;
		child->m_Index = (uint32_t)m_Children.size() - 1;
		child->SendMessage(MessageType::PARENT_CHANGED);

		return child;
	}

	GameObject::SharedPtr GameObject::DetachChild(SharedPtr child)
	{
		assert(child != nullptr);
		if (child == nullptr)
			return nullptr;

		return DetachChild(child->GetIndex());
	}

	GameObject::SharedPtr GameObject::DetachChild(const std::uint32_t& index)
	{
		assert(index < m_Children.size());
		if (index >= m_Children.size())
			return nullptr;

		auto child = m_Children[index];
		if (child == nullptr)
			return nullptr;

		m_Children.erase(m_Children.begin() + index);
		for (std::uint32_t i = index; i < m_Children.size(); ++i)
		{
			if (m_Children[i] == nullptr)
				continue;

			m_Children[i]->m_Index = i;
		}

		child->m_Parent.reset();
		child->m_Index = m_IndexCounter++;
		child->SendMessage(MessageType::PARENT_CHANGED);

		return child;
	}

	void GameObject::RemoveChild(SharedPtr child)
	{
		assert(child != nullptr);
		if (child == nullptr)
			return;

		RemoveChild(child->GetIndex());
	}

	void GameObject::RemoveChild(const std::uint32_t& index)
	{
		assert(index < m_Children.size());
		if (index >= m_Children.size())
			return;

		auto child = m_Children[index];
		if (child != nullptr)
		{
			child->m_Parent.reset();
		}

		m_Children.erase(m_Children.begin() + index);
		for (std::uint32_t i = index; i < m_Children.size(); ++i)
		{
			if (m_Children[i] == nullptr)
				continue;

			m_Children[i]->m_Index = i;
		}
	}

	void GameObject::RemoveChildren()
	{
		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->m_Parent.reset();
		}
		m_Children.clear();
	}

	const GameObject::List& GameObject::GetChildren() const
	{
		return m_Children;
	}

	GameObject::ComponentPtr GameObject::AddComponent(reflection::Type type)
	{
		auto component = GetComponent(type);
		if (component != nullptr)
		{
			return nullptr;
		}

		reflection::Any object_any = type.CreateDynamicObject();
		core::Object* object = object_any.GetValue<core::Object*>();
		Component* component_ptr = dynamic_cast<Component*>(object);
		if (component_ptr == nullptr)
		{
			delete object;
			return nullptr;
		}

		component = ComponentPtr(component_ptr);
		component->m_GameObject = shared_from_this();
		component->OnMessage(MessageType::COMPONENT_ATTACHED);

		m_Components.emplace(type.GetID(), component);
		m_ComponentsAny.emplace(type.GetID(), type.CreateDynamicPointer(*component));

		return component;
	}

	GameObject::ComponentPtr GameObject::GetComponent(reflection::Type type) const
	{
		auto search = m_Components.find(type.GetID());

		if (search == m_Components.end())
		{
			return nullptr;
		}

		return search->second;
	}

	GameObject::Components GameObject::GetComponents()
	{
		return m_Components;
	}

	GameObject::ComponentsAny GameObject::GetComponentsAny()
	{
		return m_ComponentsAny;
	}

	void GameObject::RemoveComponents(void)
	{
		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnMessage(MessageType::COMPONENT_DETACHED);
		}
		m_Components.clear();
		m_ComponentsAny.clear();
	}

	void GameObject::SendMessage(const MessageType message)
	{
		OnMessage(message);

		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->SendMessage(message);
		}
	}

	void GameObject::OnStart()
	{
		if (!m_Active)
			return;

		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnStart();
		}

		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->OnStart();
		}
	}

	void GameObject::OnUpdate(float dt)
	{
		if (!m_Active)
			return;

		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnUpdate(dt);
		}

		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->OnUpdate(dt);
		}
	}

	void GameObject::OnStop()
	{
		if (!m_Active)
			return;

		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnStop();
		}

		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			child->OnStop();
		}
	}

	void GameObject::OnMessage(MessageType message)
	{
		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnMessage(message);
		}
	}

	void GameObject::OnEnable()
	{
		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnEnable();
		}
	}

	void GameObject::OnDisable()
	{
		for (auto& pair : m_Components)
		{
			auto component = pair.second;
			if (component == nullptr)
				continue;

			component->OnDisable();
		}
	}
} // end namespace game

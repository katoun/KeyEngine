// Copyright (c) 2016 Catalin Alexandru Nastase

#include <Game/GameObject.h>
#include <Game/Messages.h>
#include <Reflection/Any.h>
#include <Reflection/TypeInfo.h>
#include <Core/Utils.h>

#include <algorithm>
#include <cassert>

namespace game
{
	std::uint32_t GameObject::m_IndexCounter = 0;

	GameObject::GameObject()
		: core::Object()
		, m_Index{ m_IndexCounter++ }
		, m_Name("GameObject_" + core::string::FromInt(m_Index))
		, m_Active{ true }
		, m_Parent{ nullptr }
	{}

	GameObject::GameObject(const std::string& name)
		: core::Object()
		, m_Index{ m_IndexCounter++ }
		, m_Name(name)
		, m_Active{ true }
		, m_Parent{ nullptr }
	{}

	GameObject::~GameObject()
	{
		RemoveComponents();
		RemoveChildren();

		if (m_Parent != nullptr)
		{
			m_Parent->RemoveChild(this);
		}
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

	GameObject* GameObject::GetParent(void)
	{
		return m_Parent;
	}

	void GameObject::SetParent(GameObject* parent)
	{
		if (m_Parent != nullptr)
		{
			m_Parent->RemoveChild(this);
		}

		m_Parent = parent;

		if (m_Parent != nullptr)
		{
			m_Parent->m_Children.push_back(this);
			m_Index = (uint32_t)m_Parent->m_Children.size() - 1;
		}
		else
		{
			m_Index = m_IndexCounter++;
		}

		SendMessage(Message::PARENT_CHANGED);
	}

	void GameObject::AddChild(GameObject* child)
	{
		assert(child != nullptr);
		if (child == nullptr)
			return;

		if (child->m_Parent != nullptr)
		{
			child->m_Parent->RemoveChild(child);
		}

		m_Children.push_back(child);
		child->m_Parent = this;
		child->m_Index = (uint32_t)m_Children.size() - 1;
		child->SendMessage(Message::PARENT_CHANGED);
	}

	void GameObject::DetachChild(GameObject* child)
	{
		assert(child != nullptr);

		if (child == nullptr)
			return;

		RemoveChild(child->GetIndex());
	}

	GameObject* GameObject::DetachChild(const std::uint32_t& index)
	{
		assert(index < m_Children.size());

		auto& child = m_Children[index];
		if (child != nullptr)
		{
			m_Children.erase(m_Children.begin() + index);
			for (std::uint32_t i = index; i < m_Children.size(); ++i)
			{
				if (m_Children[i] == nullptr)
					continue;

				m_Children[i]->m_Index = i;
			}

			child->m_Parent = nullptr;
			child->m_Index = m_IndexCounter++;

			return child;
		}

		return nullptr;
	}

	void GameObject::RemoveChild(GameObject* child)
	{
		assert(child != nullptr);

		if (child == nullptr)
			return;

		RemoveChild(child->GetIndex());
	}

	void GameObject::RemoveChild(const std::uint32_t& index)
	{
		assert(index < m_Children.size());

		auto& child = m_Children[index];
		if (child != nullptr)
		{
			m_Children.erase(m_Children.begin() + index);
			for (std::uint32_t i = index; i < m_Children.size(); ++i)
			{
				if (m_Children[i] == nullptr)
					continue;

				m_Children[i]->m_Index = i;
			}

			SAFE_DELETE(child);
		}
	}

	void GameObject::RemoveChildren()
	{
		for (auto& child : m_Children)
		{
			if (child == nullptr)
				continue;

			SAFE_DELETE(child);
		}
		m_Children.clear();
	}

	const GameObject::GameObjects& GameObject::GetChildren()
	{
		return m_Children;
	}

	Component* GameObject::AddComponent(reflection::Type type)
	{
		Component* component = GetComponent(type);
		if (component != nullptr)
		{
			return nullptr;
		}

		reflection::Any component_any = type.CreateDynamic();

		component = component_any.GetValue<Component*>();
		component->m_GameObject = this;
		component->OnMessage(Message::COMPONENT_ATTACHED);

		m_Components.emplace(type.GetID(), component);
		m_ComponentsAny.emplace(type.GetID(), reflection::Any{ component });

		return component;
	}

	Component* GameObject::GetComponent(reflection::Type type) const
	{
		auto& search = m_Components.find(type);

		if (search == m_Components.end())
		{
			return nullptr;
		}

		return (search->second);
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
			Component* component = pair.second;
			if (component == nullptr)
				continue;

			component->OnMessage(Message::COMPONENT_DETACHED);

			SAFE_DELETE(component);
		}
		m_Components.clear();
	}

	void GameObject::SendMessage(const std::uint32_t message)
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
			Component* component = pair.second;
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
			Component* component = pair.second;
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
			Component* component = pair.second;
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

	void GameObject::OnMessage(const std::uint32_t message)
	{
		for (auto& pair : m_Components)
		{
			Component* component = pair.second;
			if (component == nullptr)
				continue;

			component->OnMessage(message);
		}
	}

	void GameObject::OnEnable()
	{
		for (auto& pair : m_Components)
		{
			Component* component = pair.second;
			if (component == nullptr)
				continue;

			component->OnEnable();
		}
	}

	void GameObject::OnDisable()
	{
		for (auto& pair : m_Components)
		{
			Component* component = pair.second;
			if (component == nullptr)
				continue;

			component->OnDisable();
		}
	}
} // end namespace game

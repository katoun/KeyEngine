/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Game/Transform.h>
#include <Game/GameObject.h>
#include <Game/MessageType.h>

namespace game
{
	Transform::Transform() : Component()
	{
		m_Name = "Transform";

		m_Position = m_AbsolutePosition = glm::vec3(0, 0, 0);
		m_Orientation = m_AbsoluteOrientation = glm::quat(0, 0, 0, 1);
		m_Scale = m_AbsoluteScale = glm::vec3(1, 1, 1);
		m_EulerAngles = glm::vec3(0, 0, 0);

		m_IsOrientationInherited = true;
		m_IsScaleInherited = true;
		m_TransformNeedsUpdate = true;
	}

	Transform::~Transform() {}

	const glm::vec3& Transform::GetPosition()
	{
		return m_Position;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
		m_TransformNeedsUpdate = true;
	}

	void Transform::SetPosition(const glm::vec3& pos)
	{
		m_Position = pos;
		m_TransformNeedsUpdate = true;
	}

	const glm::quat& Transform::GetOrientation()
	{
		return m_Orientation;
	}

	void Transform::SetOrientation(float x, float y, float z, float w)
	{
		m_Orientation.x = x;
		m_Orientation.y = y;
		m_Orientation.z = z;
		m_Orientation.w = w;
		m_EulerAngles = glm::eulerAngles(m_Orientation);
		m_TransformNeedsUpdate = true;
	}

	void Transform::SetOrientation(const glm::quat& q)
	{
		m_Orientation = q;
		m_EulerAngles = glm::eulerAngles(m_Orientation);
		m_TransformNeedsUpdate = true;
	}

	const glm::vec3& Transform::GetScale()
	{
		return m_Scale;
	}

	void Transform::SetScale(float x, float y, float z)
	{
		m_Scale.x = x;
		m_Scale.y = y;
		m_Scale.z = z;
		m_TransformNeedsUpdate = true;
	}

	void Transform::SetScale(const glm::vec3& scale)
	{
		m_Scale = scale;
		m_TransformNeedsUpdate = true;
	}

	const glm::vec3& Transform::GetEulerAngles()
	{
		return m_EulerAngles;
	}

	void Transform::SetEulerAngles(const glm::vec3& angles)
	{
		m_EulerAngles = angles;
		m_Orientation = glm::quat(angles);

		m_TransformNeedsUpdate = true;
	}

	const bool Transform::IsOrientationInherited() const
	{
		return m_IsOrientationInherited;
	}

	void Transform::SetOrientationInherited(const bool inherit)
	{
		m_IsOrientationInherited = inherit;
	}

	const bool Transform::IsScaleInherited() const
	{
		return m_IsScaleInherited;
	}

	void Transform::SetScaleInherited(const bool inherit)
	{
		m_IsScaleInherited = inherit;
	}

	const glm::vec3& Transform::GetAbsolutePosition()
	{
		return m_AbsolutePosition;
	}

	const glm::quat& Transform::GetAbsoluteOrientation()
	{
		return m_AbsoluteOrientation;
	}

	const glm::vec3& Transform::GetAbsoluteScale()
	{
		return m_AbsoluteScale;
	}

	void Transform::Scale(const glm::vec3& scale)
	{
		m_Scale = m_Scale * scale;

		m_TransformNeedsUpdate = true;
	}

	void Transform::Translate(const glm::vec3& d, TransformSpace relativeTo)
	{
		switch (relativeTo)
		{
		case TransformSpace::LOCAL:
			// position is relative to parent so transform downwards
			m_Position += m_Orientation * d;
			break;
		case TransformSpace::GLOBAL:
			// position is relative to parent so transform upwards
			if (m_Parent != nullptr)
			{
				glm::vec3 offset = glm::inverse(m_Parent->GetAbsoluteOrientation()) * d;
				offset = offset / m_Parent->GetAbsoluteScale();
				m_Position += offset;
			}
			else
			{
				m_Position += d;
			}
			break;
		}

		m_TransformNeedsUpdate = true;
	}

	void Transform::Rotate(const glm::quat& q, TransformSpace relativeTo)
	{
		// Normalise quaternion to avoid drift
		glm::quat qnorm = glm::normalize(q);

		switch (relativeTo)
		{
		case TransformSpace::LOCAL:
			// Note the order of the mult, i.e. q comes after
			m_Orientation = m_Orientation * qnorm;
			break;
		case TransformSpace::GLOBAL:
			// Rotations are normally relative to local axes, transform up
			m_Orientation = m_Orientation * glm::inverse(GetAbsoluteOrientation()) * qnorm * GetAbsoluteOrientation();
			break;
		}
		m_EulerAngles = glm::eulerAngles(m_Orientation);

		m_TransformNeedsUpdate = true;
	}

	void Transform::Rotate(const float& degrees, const glm::vec3& axis, TransformSpace relativeTo)
	{
		glm::quat q = glm::angleAxis(degrees, axis);
		Rotate(q, relativeTo);
	}

	void Transform::RotateX(float degrees, TransformSpace relativeTo)
	{
		Rotate(degrees, glm::vec3(1, 0, 0), relativeTo);
	}

	void Transform::RotateY(float degrees, TransformSpace relativeTo)
	{
		Rotate(degrees, glm::vec3(0, 1, 0), relativeTo);
	}

	void Transform::RotateZ(float degrees, TransformSpace relativeTo)
	{
		Rotate(degrees, glm::vec3(0, 0, 1), relativeTo);
	}

	void Transform::OnStart(void)
	{
		Component::OnStart();
	}

	void Transform::OnUpdate(float dt)
	{
		Component::OnUpdate(dt);

		if (m_TransformNeedsUpdate)
		{
			m_GameObject->SendMessage(MessageType::NEEDS_UPDATE);

			if (m_Parent != nullptr)
			{
				// Combine orientation with that of parent
				const glm::quat& parentOrientation = m_Parent->GetAbsoluteOrientation();
				if (m_IsOrientationInherited)
				{
					// Combine orientation with that of parent
					m_AbsoluteOrientation = parentOrientation * m_Orientation;
				}
				else
				{
					// No inheritance
					m_AbsoluteOrientation = m_Orientation;
				}

				const glm::vec3& parentScale = m_Parent->GetAbsoluteScale();
				// Update scale
				if (m_IsScaleInherited)
				{
					// Set own scale, NB just combine as equivalent axes, no shearing
					m_AbsoluteScale = m_Scale;

					m_AbsoluteScale = m_AbsoluteScale * parentScale;
				}
				else
				{
					// No inheritance
					m_AbsoluteScale = m_Scale;
				}

				const glm::vec3& parentPosition = m_Parent->GetAbsolutePosition();

				// Change position vector based on parent's orientation & scale
				m_AbsolutePosition = m_Position;

				m_AbsoluteScale = m_AbsoluteScale * parentScale;

				m_AbsolutePosition = parentOrientation * m_AbsolutePosition;
				// Add altered position vector to parents
				m_AbsolutePosition += parentPosition;
			}
			else
			{
				m_AbsoluteOrientation = m_Orientation;
				m_AbsolutePosition = m_Position;
				m_AbsoluteScale = m_Scale;
			}

			m_TransformNeedsUpdate = false;
		}
	}

	void Transform::OnStop(void)
	{
		Component::OnStop();
	}

	void Transform::OnMessage(MessageType message)
	{
		Component::OnMessage(message);

		switch (message)
		{
		case MessageType::PARENT_CHANGED:
		{
			GameObject* parent_object = m_GameObject->GetParent();
			if (parent_object)
			{
				m_Parent = parent_object->GetComponent<Transform>();
			}
			else
			{
				m_Parent = nullptr;
			}

			m_TransformNeedsUpdate = true;
		}
		break;
		case MessageType::NEEDS_UPDATE:
		{
			m_TransformNeedsUpdate = true;
		}
		break;
		}
	}
} // end namespace game
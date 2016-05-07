// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>
#include <Game/Component.h>
#include <Game/TransformSpace.h>
#include <Reflection/Reflection.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace game
{
	class RUNTIME_API Transform : public Component
	{
		DEFINE_OBJECT

	public:

		Transform();
		~Transform();

		const glm::vec3& GetPosition();

		virtual void SetPosition(float x, float y, float z);
		virtual void SetPosition(const glm::vec3& pos);

		const glm::quat& GetOrientation();

		virtual void SetOrientation(float x, float y, float z, float w);
		virtual void SetOrientation(const glm::quat& q);

		const glm::vec3& GetScale();

		void SetScale(float x, float y, float z);
		void SetScale(const glm::vec3& scale);

		const glm::vec3& GetEulerAngles();

		void SetEulerAngles(const glm::vec3& angles);

		const bool IsOrientationInherited() const;

		void SetOrientationInherited(const bool inherit);

		const bool IsScaleInherited() const;

		void SetScaleInherited(const bool inherit);

		const glm::vec3& GetAbsolutePosition();
		const glm::quat& GetAbsoluteOrientation();
		const glm::vec3& GetAbsoluteScale();

		virtual void Scale(const glm::vec3& scale);
		virtual void Translate(const glm::vec3& d, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);
		virtual void Rotate(const glm::quat& q, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);

		virtual void Rotate(const float& degrees, const glm::vec3& axis, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);
		virtual void RotateX(float degrees, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);
		virtual void RotateY(float degrees, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);
		virtual void RotateZ(float degrees, TransformSpace relativeTo = TRANSFORM_LOCAL_SPACE);

	protected:

		void OnStart(void) override;
		void OnUpdate(float dt) override;
		void OnStop(void) override;
		void OnMessage(const std::uint32_t message) override;

		Transform* m_Parent;

		glm::vec3 m_Position;
		glm::quat m_Orientation;
		glm::vec3 m_Scale;
		glm::vec3 m_EulerAngles;

		bool m_IsOrientationInherited;
		bool m_IsScaleInherited;

		glm::vec3 m_AbsolutePosition;//Cached value
		glm::quat m_AbsoluteOrientation;//Cached value
		glm::vec3 m_AbsoluteScale;//Cached value

		bool m_TransformNeedsUpdate;
	};
} // end namespace game
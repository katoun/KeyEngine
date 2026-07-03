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
#include <Game/Component.h>
#include <Game/TransformSpace.h>
#include <Reflection/Reflection.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>

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
		virtual void Translate(const glm::vec3& d, TransformSpace relativeTo = TransformSpace::LOCAL);
		virtual void Rotate(const glm::quat& q, TransformSpace relativeTo = TransformSpace::LOCAL);

		virtual void Rotate(const float& degrees, const glm::vec3& axis, TransformSpace relativeTo = TransformSpace::LOCAL);
		virtual void RotateX(float degrees, TransformSpace relativeTo = TransformSpace::LOCAL);
		virtual void RotateY(float degrees, TransformSpace relativeTo = TransformSpace::LOCAL);
		virtual void RotateZ(float degrees, TransformSpace relativeTo = TransformSpace::LOCAL);

	protected:

		void OnStart(void) override;
		void OnUpdate(float dt) override;
		void OnStop(void) override;
		void OnMessage(MessageType message) override;

		std::weak_ptr<Transform> m_Parent;

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

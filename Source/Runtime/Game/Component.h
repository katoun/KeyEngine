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
#include <Game/MessageType.h>

#include <memory>
#include <string>

namespace game
{
	class GameObject;

	class RUNTIME_API Component : public core::Object
	{
		DEFINE_OBJECT

	public:

		using SharedPtr = std::shared_ptr<Component>;
		using WeakPtr = std::weak_ptr<Component>;

		Component();
		virtual ~Component();

		const std::string& getName() const;

		const bool IsEnabled() const;
		void SetEnabled(const bool enabled);

		std::shared_ptr<GameObject> GetGameObject() const;

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

		std::weak_ptr<GameObject> m_GameObject;
	};

} // end namespace game

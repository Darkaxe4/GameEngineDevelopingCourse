#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <BehaviourComponent.h>

namespace GameEngine
{
	class GameObject final
	{
	public:
		GameObject() = default;

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position)
		{
			m_Position = position;
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

		void Update(float dt, size_t frame)
		{
			if (m_BehaviourComponent)
			{
				m_BehaviourComponent->Update(dt);
			}
			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(m_Position, frame);
			}
		}

		void SetBehaviour(std::unique_ptr<BaseBehaviourComponent> component)
		{
			m_BehaviourComponent = std::move(component);
		}

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();

		std::unique_ptr<BaseBehaviourComponent> m_BehaviourComponent = nullptr;
	};
}
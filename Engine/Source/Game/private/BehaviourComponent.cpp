#include <BehaviourComponent.h>
#include <Constants.h>
#include <GameObject.h>
#include <Input/InputHandler.h>
#include <MathHelper.h>

namespace GameEngine
{
	BaseBehaviourComponent::BaseBehaviourComponent(GameObject* obj)
		:m_ParentObject(obj)
	{
	}

	JumpBehaviourComponent::JumpBehaviourComponent(GameObject* obj)
		:BaseBehaviourComponent(obj),
		m_CurrentAcceleration(),
		m_CurrentVelocity()
	{
	}

	const float JumpBehaviourComponent::c_JumpTimerVal = 2.f;

	void JumpBehaviourComponent::Update(float dt)
	{
		if (m_ParentObject)
		{
			switch (m_State)
			{
			case 0:
				Grounded(dt);
				break;
			case 1:
				Midair(dt);
				break;
			}
		}

	}

	void JumpBehaviourComponent::Midair(float dt)
	{
		float y_delta = Math::Constants::G_FORCE / 2.f * dt * dt + m_CurrentVelocity.y * dt;
		m_CurrentVelocity.y += Math::Constants::G_FORCE * dt;
		Math::Vector3f pos = m_ParentObject->GetPosition();
		if (pos.y + y_delta < -Math::Constants::FLOAT_THRESHOLD)
		{
			m_CurrentVelocity.y = 0.f;
			m_State = int8_t{ 0 };
			m_JumpTimer = JumpBehaviourComponent::c_JumpTimerVal;
		}
		else
		{
			m_ParentObject->SetPosition(Math::Vector3f(pos.x, pos.y + y_delta, pos.z));
		}
	}

	void JumpBehaviourComponent::Grounded(float dt)
	{
		m_JumpTimer -= dt;
		if (m_JumpTimer < Math::Constants::FLOAT_THRESHOLD)
		{
			m_CurrentVelocity.y = 10.f;
			m_State = int8_t{ 1 };
		}
	}

	OscillateBehaviourComponent::OscillateBehaviourComponent(GameObject* obj)
		:BaseBehaviourComponent(obj)
	{
	}

	void OscillateBehaviourComponent::Update(float dt)
	{
		Math::Vector3f pos = m_ParentObject->GetPosition();
		float sin = 0.f;
		float cos = 0.f;
		m_Phase += dt * c_Velocity;
		Math::CalculateSinCos(sin, cos, m_Phase);
		
		m_ParentObject->SetPosition(Math::Vector3f(pos.x, pos.y, 2 * sin));
	}

	ControllableBehaviourComponent::ControllableBehaviourComponent(GameObject* obj)
		:BaseBehaviourComponent(obj)
	{
		Core::g_InputHandler->RegisterCallback("GoLeft", [this]() { this->MoveLeftCallback(); });
		Core::g_InputHandler->RegisterCallback("GoRight", [this]() { this->MoveRightCallback(); });
	}

	void ControllableBehaviourComponent::Update(float dt)
	{
		if (m_ParentObject)
		{
			m_ParentObject->SetPosition(m_ParentObject->GetPosition() + m_Motion * c_Velocity * dt);
		}
		m_Motion = GameEngine::Math::Vector3f::Zero();
	}

	void ControllableBehaviourComponent::MoveLeftCallback()
	{
		m_Motion.x = -10.f;
	}

	void ControllableBehaviourComponent::MoveRightCallback()
	{
		m_Motion.x = 10.f;
	}
}

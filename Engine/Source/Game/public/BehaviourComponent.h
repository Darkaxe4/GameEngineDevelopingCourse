#pragma once
#include<Vector.h>

namespace GameEngine
{
	class GameObject;
	
	class BaseBehaviourComponent
	{
	public:
		BaseBehaviourComponent(GameObject* obj);

	public:
		virtual void Update(float dt) = 0;
	protected:
		GameObject* m_ParentObject;
	};

	class JumpBehaviourComponent final : public BaseBehaviourComponent 
	{
	public:
		JumpBehaviourComponent(GameObject* obj);

	public:
		void Update(float dt) override;

	private:
		void Midair(float dt);
		void Grounded(float dt);

	private:
		int8_t m_State = int8_t{ 0 };
		float m_JumpTimer = c_JumpTimerVal;
		Math::Vector3f m_CurrentVelocity;
		Math::Vector3f m_CurrentAcceleration;

		static const float c_JumpTimerVal;
	};

	class OscillateBehaviourComponent final : public BaseBehaviourComponent
	{
	public:
		OscillateBehaviourComponent(GameObject* obj);

	public:
		void Update(float dt) override;

	private:
		float m_Phase = 0.f;
		const float c_Velocity = 2.f;
	};

	class ControllableBehaviourComponent final : public BaseBehaviourComponent
	{
	public:
		ControllableBehaviourComponent(GameObject* obj);
	public:
		void Update(float dt) override;
		void MoveLeftCallback();
		void MoveRightCallback();
	private:
		Math::Vector3f m_Motion = Math::Vector3f::Zero();
		const float c_Velocity = 1.f;
	};
}


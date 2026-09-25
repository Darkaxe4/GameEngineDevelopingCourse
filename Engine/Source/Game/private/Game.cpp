#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>
#include <random>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, 2);

		// How many objects do we want to create
		for (size_t i = size_t{ 0 }; i < 10; ++i)
			for (size_t j = size_t{ 0 }; j < 10; ++j)
			{
				m_Objects.push_back(new GameObject());
				Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
				m_Objects[i * 10 + j]->SetPosition(Math::Vector3f(-3.f * 5.f + (float)i * 3.f, 0.f, (float)j * 3.f));
				m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);

				switch (distrib(gen))
				{
				case 0:
					m_Objects[i * 10 + j]->SetBehaviour(std::make_unique<ControllableBehaviourComponent>(m_Objects[i * 10 + j]));
					break;
				case 1:
					m_Objects[i * 10 + j]->SetBehaviour(std::make_unique<OscillateBehaviourComponent>(m_Objects[i * 10 + j]));
					
					break;
				case 2:
					m_Objects[i * 10 + j]->SetBehaviour(std::make_unique<JumpBehaviourComponent>(m_Objects[i * 10 + j]));
					break;
				}
			}


		Core::g_InputHandler->RegisterCallback("GoForward", []() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", []() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", []() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", []() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			//Math::Vector3f pos = m_Objects[i]->GetPosition();

			//// Showcase
			//if (i == 0)
			//{
			//	pos.x += 0.5f * dt;
			//}
			//else if (i == 1)
			//{
			//	pos.y -= 0.5f * dt;
			//}
			//else if (i == 2)
			//{
			//	pos.x += 0.5f * dt;
			//	pos.y -= 0.5f * dt;
			//}
			//m_Objects[i]->SetPosition(pos);
			m_Objects[i]->Update(dt, m_renderThread->GetMainFrame());
		}
	}
}
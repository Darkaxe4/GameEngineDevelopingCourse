#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>

namespace GameEngine
{
	constexpr float k_CameraSpeed = 1.f;

	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_inputManager = std::make_unique<Input::InputManager>();
		
		m_inputManager->subscribe("rotate_cam_left", [this]() {Core::g_MainCamera->Rotate(-k_CameraSpeed * this->m_GameTimer.GetDeltaTime(), 0.f);});
		m_inputManager->subscribe("rotate_cam_right", [this]() {Core::g_MainCamera->Rotate(k_CameraSpeed * this->m_GameTimer.GetDeltaTime(), 0.f);});

		m_renderThread = std::make_unique<Render::RenderThread>();

		// How many objects do we want to create
		for (int i = 0; i < 3; ++i)
		{
			m_Objects.push_back(new GameObject());
			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}
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

			Update(dt);
			
			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();

			m_renderThread->OnEndFrame();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			Math::Vector3f pos = m_Objects[i]->GetPosition();

			// Showcase
			if (i == 0)
			{
				pos.x += 0.5f * dt;
			}
			else if (i == 1)
			{
				pos.y -= 0.5f * dt;
			}
			else if (i == 2)
			{
				pos.x += 0.5f * dt;
				pos.y -= 0.5f * dt;
			}
			m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
		}
	}
	void Game::ReadKeybinds(std::string config_file)
	{
		INIReader reader(config_file.data());
		m_inputManager->bind_key(reader.Get("cam_movement", "rotate_cam_left", "a")[0], "rotate_cam_left");
		m_inputManager->bind_key(reader.Get("cam_movement", "rotate_cam_right", "d")[0], "rotate_cam_right");
	}

	void Game::ProcessInput(wchar_t key)
	{
		m_inputManager->process_input(key);
	}
}
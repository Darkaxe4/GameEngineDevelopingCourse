#pragma once

#include <RenderEngine.h>
#include <RenderThread.h>
#include <InputManager.h>
#include <Timer.h>
#include <Window/IWindow.h>
#include <INIReader.h>


namespace GameEngine
{
	class GameObject;

	class Game final
	{
	public:
		Game() = delete;
		Game(
			std::function<bool()> PlatformLoopFunc
		);

	public:
		void Run();
		void Update(float dt);
		void ReadKeybinds(std::string config_file);
		void ProcessInput(wchar_t key);

	private:
		// The main idea behind having this functor is to abstract the common code from the platfrom-specific code
		std::function<bool()> PlatformLoop = nullptr;

	private:
		Core::Timer m_GameTimer;
		std::unique_ptr<Render::RenderThread> m_renderThread;
		std::unique_ptr<Input::InputManager> m_inputManager;
		std::vector<GameObject*> m_Objects;
	};
}
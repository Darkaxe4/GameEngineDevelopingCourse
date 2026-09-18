#include "InputManager.h"

namespace GameEngine::Input
{
	InputManager::InputManager() :
		event_handlers(),
		keybinds()
	{
	}

	void InputManager::bind_key(wchar_t key, std::string_view action)
	{
		keybinds[key] = action.data();
	}

	void InputManager::subscribe(std::string_view action, std::function<void()> handler)
	{
		event_handlers[action.data()] = handler;
	}

	void InputManager::process_input(wchar_t key)
	{
		if (const auto& search = keybinds.find(key); search != keybinds.end())
		{
			if (const auto& event_handler = event_handlers.find(search->second); event_handler != event_handlers.end())
				event_handler->second();
		}
	}
}

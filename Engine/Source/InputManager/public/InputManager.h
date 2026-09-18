#pragma once

#include <InputManager/export.h>
#include <map>


namespace GameEngine::Input
{
	class INPUT_MANAGER_API InputManager final
	{
	private:
		std::map<std::string, std::function<void()>> event_handlers;
		std::map<wchar_t, std::string> keybinds;
	public:
		InputManager();

		void bind_key(wchar_t key, std::string_view action);
		void subscribe(std::string_view action, std::function<void()> handler);
		void process_input(wchar_t key);
	};
}
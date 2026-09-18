#pragma once
#include <Windows.h>

namespace GameEngine
{
	enum class InputCommandName
	{
		NONE = 0,
		ROTATE_LEFT = 1,
		ROTATE_RIGHT = 2,
	};
	struct InputCommand
	{
		const wchar_t key;
		const InputCommandName name;
	};
}
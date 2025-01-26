#pragma once
#include <Events/EventHandler.h>

namespace cp {
	class Input {
	public:
		static void init(EventHandler& evtHandler);
		static void onKeyEvent(int keycode, KeyAction action);

		static bool isKeyPressed(int keycode);

	private:
		static std::array<bool, 1032> sKeysPressed;
	};
}
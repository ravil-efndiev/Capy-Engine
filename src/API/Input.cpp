#include "Input.h"

namespace cp {
	std::array<bool, 1032> Input::sKeysPressed{false};

	void Input::init(EventHandler& evtHandler) {
		evtHandler.subscribeToKeyEvt([](int keycode, KeyAction action) {
			Input::onKeyEvent(keycode, action);
		});
	}

	void Input::onKeyEvent(int keycode, KeyAction action) {
		if (action == KeyAction::press) {
			sKeysPressed[keycode] = true;
		}
		else if (action == KeyAction::release) {
			sKeysPressed[keycode] = false;
		}
	}

	bool Input::isKeyPressed(int keycode) {
		return sKeysPressed[keycode];
	}
}

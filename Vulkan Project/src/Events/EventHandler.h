#pragma once
#include <include.h>
#include "Event.h"

namespace cp {
	enum class KeyAction {
		press, release
	};

	class EventHandler {
	public:
		using ResizeEventListener = std::function<void(int, int)>;
		using KeyEventListener = std::function<void(int, KeyAction)>;

		void handle(const Event* event);

		void subscribeToResizeEvt(const ResizeEventListener& listener);
		void subscribeToKeyEvt(const KeyEventListener& listener);

	private:
		std::vector<ResizeEventListener> mResizeEventListeners;
		std::vector<KeyEventListener> mKeyEventListeners;
	};
}

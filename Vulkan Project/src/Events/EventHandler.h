#pragma once
#include <include.h>
#include "Event.h"

namespace cp {
	class EventHandler {
	public:
		using ResizeEventListener = std::function<void(int, int)>;

		void handle(const Event* event);

		void subscribeToResizeEvt(const ResizeEventListener& listener);

	private:
		std::vector<ResizeEventListener> resizeEventListeners;
	};
}

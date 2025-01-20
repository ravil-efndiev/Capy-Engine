#include "EventHandler.h"

namespace cp {
	void EventHandler::handle(const Event* event) {
		switch (event->getType()) {
		case EventType::framebufferResize:
			const ResizeEvent* castedEvt = dynamic_cast<const ResizeEvent*>(event);

			for (auto& listener : resizeEventListeners) {
				listener(castedEvt->width, castedEvt->height);
			}
			break;
		}
	}

	void EventHandler::subscribeToResizeEvt(const ResizeEventListener& listener) {
		resizeEventListeners.push_back(listener);
	}
}

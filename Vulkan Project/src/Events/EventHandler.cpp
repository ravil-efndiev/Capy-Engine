#include "EventHandler.h"

namespace cp {
	void EventHandler::handle(const Event* event) {
		switch (event->getType()) {
		case EventType::framebufferResize:
			const ResizeEvent* castedEvt = dynamic_cast<const ResizeEvent*>(event);

			for (auto& listener : mResizeEventListeners) {
				listener(castedEvt->width, castedEvt->height);
			}
			break;
		}
	}

	void EventHandler::subscribeToResizeEvt(const ResizeEventListener& listener) {
		mResizeEventListeners.push_back(listener);
	}
}

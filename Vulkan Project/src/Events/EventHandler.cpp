#include "EventHandler.h"

namespace cp {
	void EventHandler::handle(const Event* event) {
		switch (event->getType()) {
			case EventType::framebufferResize: {
				const ResizeEvent* castedEvt = dynamic_cast<const ResizeEvent*>(event);

				for (auto& listener : mResizeEventListeners) {
					listener(castedEvt->width, castedEvt->height);
				}
				break;
			}
			case EventType::keyPress: {
				const KeyPressEvent* castedEvt = dynamic_cast<const KeyPressEvent*>(event);

				for (auto& listener : mKeyEventListeners) {
					listener(castedEvt->keycode, KeyAction::press);
				}
				break;
			}
			case EventType::keyRelease: {
				const KeyReleaseEvent* castedEvt = dynamic_cast<const KeyReleaseEvent*>(event);

				for (auto& listener : mKeyEventListeners) {
					listener(castedEvt->keycode, KeyAction::release);
				}
				break;
			}
		}

	}

	void EventHandler::subscribeToResizeEvt(const ResizeEventListener& listener) {
		mResizeEventListeners.push_back(listener);
	}

	void EventHandler::subscribeToKeyEvt(const KeyEventListener& listener) {
		mKeyEventListeners.push_back(listener);
	}
}

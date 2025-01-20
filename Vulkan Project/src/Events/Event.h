#pragma once

namespace cp {
	enum class EventType {
		framebufferResize,
		windowClose,
		windowFocus,
		windowBlur,
		keyPress,
		keyRelease,
		mouseMove,
		mouseButtonPress,
		mouseButtonRelease,
		mouseScroll,
	};

	struct Event {
		virtual EventType getType() const = 0;
	};

	struct ResizeEvent : public Event {
		ResizeEvent(int width, int height) 
			: width(width), height(height) {}

		EventType getType() const override { return EventType::framebufferResize; }

		int width;
		int height;
	};
}

#pragma once

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
	EventType type;
};

#include "Time.h"

namespace cp {
	float Time::sCurrentTime = 0;
	float Time::sLastTime = 0;
	float Time::sDeltaTime = 0;

	void Time::update()  {
		sCurrentTime = glfwGetTime();
		sDeltaTime = sCurrentTime - sLastTime;
		sLastTime = sCurrentTime;
	}

	float Time::dt() {
		return sDeltaTime;
	}
}

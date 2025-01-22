#pragma once
#include <include.h>

namespace cp {
	class Time {
	public:
		static void update();

		static float dt();

	public:
		static constexpr float FixedTime = 1.f / 60.f;

	private:
		static float sDeltaTime;
		static float sCurrentTime;
		static float sLastTime;
	};
}

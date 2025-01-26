#pragma once
#include <include.h>

namespace cp {
	struct Transform {
		glm::mat4 calcModelMatrix() const;

		glm::vec3 position{0.f};
		glm::vec3 rotation{0.f};
		glm::vec3 scale{1.f};
		bool useImplicitDegrees = false;
	};
}

#pragma once
#include <include.h>

namespace cp {
	struct MatrixUBO {
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 model;
	};
}

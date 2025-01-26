#pragma once
#include <include.h>

namespace cp {
	struct ProjViewUBO {
		glm::mat4 proj{1.f};
		glm::mat4 view{1.f};
	};
}

#include "Transform.h"

namespace cp {
	glm::mat4 Transform::calcModelMatrix() const {
		glm::mat4 model{1.f};
		glm::vec3 invYPos = position;
		invYPos.y = -invYPos.y;
		model = glm::translate(model, invYPos);
		if (useImplicitDegrees) {
			model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
			model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
			model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		}
		else {
			model = glm::rotate(model, rotation.z, glm::vec3(0.f, 0.f, 1.f));
			model = glm::rotate(model, rotation.y, glm::vec3(0.f, 1.f, 0.f));
			model = glm::rotate(model, rotation.x, glm::vec3(1.f, 0.f, 0.f));
		}
		model = glm::scale(model, scale);
		return model;
	}
}

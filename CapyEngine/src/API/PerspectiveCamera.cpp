#include "PerspectiveCamera.h"

namespace cp {
	PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, float fov)
		: mPosition(position), mFov(fov) {
	
		applyRotation();
	}

	glm::mat4 PerspectiveCamera::projectionMatrix(float viewportAspectRatio) const {
		return viewportAspectRatio > 0 
			? glm::perspective(mFov, viewportAspectRatio, 0.1f, 100.f)
			: glm::mat4(0.f);
	}

	glm::mat4 PerspectiveCamera::viewMatrix() const {
		glm::vec3 invYPos = mPosition;
		invYPos.y = -invYPos.y;
		return glm::lookAt(invYPos, invYPos + mForward, mUp);
	}

	void PerspectiveCamera::rotate(float x, float y, float z) {
		mRotation = glm::rotate(mRotation, z, glm::vec3(0.f, 0.f, 1.f));
		mRotation = glm::rotate(mRotation, y, glm::vec3(0.f, 1.f, 0.f));
		mRotation = glm::rotate(mRotation, x, glm::vec3(1.f, 0.f, 0.f));

		mVecRotation.x = glm::atan(mRotation[2][1], mRotation[2][2]);
		mVecRotation.y = glm::atan(-mRotation[2][0], glm::sqrt(mRotation[2][1] * mRotation[2][1] + mRotation[2][2] * mRotation[2][2]));
		mVecRotation.z = glm::atan(mRotation[1][0], mRotation[0][0]);

		applyRotation();
	}

	void PerspectiveCamera::rotate(const glm::vec3& rotation) {
		rotate(rotation.x, rotation.y, rotation.z);
	}

	void PerspectiveCamera::setPosition(const glm::vec3& position) {
		mPosition = position;
	}

	void PerspectiveCamera::move(const glm::vec3& moveVec) {
		mPosition += moveVec;
	}

	void PerspectiveCamera::applyRotation() {
		mForward = glm::vec3(mRotation * glm::vec4(0.f, 0.f, -1.f, 1.f));
		mUp		 = glm::vec3(mRotation * glm::vec4(0.f, 1.f, 0.f, 1.f));
		mRight	 = glm::vec3(mRotation * glm::vec4(-1.f, 0.f, 0.f, 1.f));
	}
}

#pragma once
#include <include.h>

namespace cp {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(const glm::vec3& position, float fov);
		
		glm::mat4 projectionMatrix(float viewportAspectRatio) const;
		glm::mat4 viewMatrix() const;
		glm::vec3 vectorRotation() const { return mVecRotation; }

		void rotate(float x, float y, float z);
		void rotate(const glm::vec3& rotation);
		void setPosition(const glm::vec3& position);

	private:
		void applyRotation();

	private:
		glm::vec3 mForward{};
		glm::vec3 mRight{};
		glm::vec3 mUp{};

		float mFov = 0;
		glm::vec3 mPosition{};
		glm::mat4 mRotation{};
		glm::vec3 mVecRotation{};
	};
}

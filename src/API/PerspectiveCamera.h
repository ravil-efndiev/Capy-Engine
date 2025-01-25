#pragma once
#include <include.h>

namespace cp {
	class PerspectiveCamera {
	public:
		PerspectiveCamera(const glm::vec3& position, float fov);
		
		glm::mat4 projectionMatrix(float viewportAspectRatio) const;
		glm::mat4 viewMatrix() const;
		glm::vec3 vectorRotation() const { return mVecRotation; }
		glm::vec3 position() const { return mPosition; }

		void rotate(float x, float y, float z);
		void rotate(const glm::vec3& rotation);
		void setPosition(const glm::vec3& position);
		void move(const glm::vec3& moveVec);

	private:
		void applyRotation();

	private:
		glm::vec3 mForward{};
		glm::vec3 mRight{};
		glm::vec3 mUp{};

		float mFov = 0;
		glm::vec3 mPosition{};
		glm::mat4 mRotation{1.f};
		glm::vec3 mVecRotation{};
	};
}

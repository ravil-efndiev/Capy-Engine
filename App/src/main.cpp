#include <Application.h>

using namespace cp;

class TestApp : public Application {
private:
    void start() override {
		mRenderer = std::make_unique<Renderer>(device(), swapchain(), eventHandler());

		Shader shader(gConstants.spirvDir / "vert.spv", gConstants.spirvDir / "frag.spv");

		PipelineConfiguration pipelineConfig{};
		pipelineConfig.pShader = &shader;

		PipelineHandle pipeline = mRenderer->addPipelineConfiguration(pipelineConfig);
		mRenderer->usePipeline(pipeline);

		std::vector<PositionColorVertex> vertices = {
			{{-0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f, 1.f}},
			{{0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 0.0f, 1.f}},
			{{0.5f, 0.5f, 0.f}, {0.0f, 1.0f, 1.0f, 1.f}},
			{{-0.5f, 0.5f, 0.f}, {1.0f, 0.0f, 1.0f, 1.f}}
		};

		std::vector<uint16> indices = {
			0, 1, 2, 2, 3, 0
		};

		mMesh = std::make_unique<Mesh<PositionColorVertex>>(vertices, indices);
		mMeshTf.position = { 0.f, 1.f, 0.f };
		mMeshTf.scale = { 3.f, 1.f, 1.f };
		mMeshTf.useImplicitDegrees = true;

		mMeshTf2.position.x = -1.f;
    }

    void update() override {
		mMeshTf.rotation.y += 10.f * Time::dt();

		float camSpeed = 10.f;
		if (Input::isKeyPressed(KeyW)) {
			mCamera.move(glm::vec3(0.f, 0.f, -1.f) * camSpeed * Time::dt());
		}
		if (Input::isKeyPressed(KeyS)) {
			mCamera.move(glm::vec3(0.f, 0.f, 1.f) * camSpeed * Time::dt());
		}
		if (Input::isKeyPressed(KeyA)) {
			mCamera.move(glm::vec3(-1.f, 0.f, 0.f) * camSpeed * Time::dt());
		}
		if (Input::isKeyPressed(KeyD)) {
			mCamera.move(glm::vec3(1.f, 0.f, 0.f) * camSpeed * Time::dt());
		}
		if (Input::isKeyPressed(KeySpace)) {
			mCamera.move(glm::vec3(0.f, 1.f, 0.f) * camSpeed * Time::dt());
		}
		if (Input::isKeyPressed(KeyLeftShift)) {
			mCamera.move(glm::vec3(0.f, -1.f, 0.f) * camSpeed * Time::dt());
		}

		glm::vec2 viewportSize = mRenderer->viewportSize();
		float aspect = viewportSize.x / viewportSize.y;
		mRenderer->setProjView(mCamera.projectionMatrix(aspect), mCamera.viewMatrix());
		mRenderer->begin();
		mRenderer->submitMesh(*mMesh, mMeshTf);
		mRenderer->submitMesh(*mMesh, mMeshTf2);
		mRenderer->end();
    }

private:
	std::unique_ptr<Renderer> mRenderer;
	PerspectiveCamera mCamera{ glm::vec3(0.f, 0.f, 2.f), 70.f };
	std::unique_ptr<Mesh<PositionColorVertex>> mMesh;
	Transform mMeshTf;
	Transform mMeshTf2;
};

int main() {
	Application& app = Application::create<TestApp>();
    app.run();
}

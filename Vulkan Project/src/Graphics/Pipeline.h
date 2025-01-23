#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include "Shader.h"
#include "RenderPass.h"
#include "Vertex.h"

namespace cp {
	struct PipelineConfiguration {
		enum VertexType {
			PositionColorVertex,
			TexCoordVertex,
		};

		VertexType vertexType = PositionColorVertex;
		VkCullModeFlags culling = VK_CULL_MODE_NONE;
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

		Shader* pShader;
	};

	class Pipeline {
	public:
		Pipeline(Device& device, Swapchain& swapchain, const PipelineConfiguration& config = {});
		~Pipeline();

		VkPipeline vkHandle() const { return mPipeline; }
		PipelineConfiguration configuration() const { return mConfig; }

	private:
		void create();
		void setShaderStages(const Shader& shaderModules);

	private:
		PipelineConfiguration mConfig;

		Device& mDevice;
		Swapchain& mSwapchain;
		VkPipeline mPipeline = VK_NULL_HANDLE;
		VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
		RenderPass mRenderPass{ mDevice, mSwapchain };

		VkPipelineShaderStageCreateInfo mVertexShaderStage{};
		VkPipelineShaderStageCreateInfo mFragmentShaderStage{};

		const std::array<VkDynamicState, 2> mDynamicStates = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };
	};
}

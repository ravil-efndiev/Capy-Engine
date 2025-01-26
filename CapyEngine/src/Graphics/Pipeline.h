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
		bool backCullingEnabled = false;
		bool wireframeMode = false;

		Shader* pShader;

		struct DescriptorSetBinding {
			VkDescriptorType descriptorType;
			VkShaderStageFlags shaderStage;
		};
		
		std::vector<DescriptorSetBinding> descriptorSetBindings;
	};

	class Pipeline {
	public:
		Pipeline(Device& device, Swapchain& swapchain, const PipelineConfiguration& config = {});
		~Pipeline();

		VkPipeline vkHandle() const { return mPipeline; }
		PipelineConfiguration configuration() const { return mConfig; }
		VkPipelineLayout layout() const { return mPipelineLayout; }
		VkDescriptorSetLayout descriptorSetLayout() const { return mDescSetLayout; }

	private:
		void create();
		void setShaderStages(const Shader& shaderModules);

	private:
		PipelineConfiguration mConfig;

		Device& mDevice;
		Swapchain& mSwapchain;
		VkPipeline mPipeline = VK_NULL_HANDLE;
		VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout mDescSetLayout = VK_NULL_HANDLE;
		RenderPass mRenderPass{ mDevice, mSwapchain };

		VkPipelineShaderStageCreateInfo mVertexShaderStage{};
		VkPipelineShaderStageCreateInfo mFragmentShaderStage{};

		const std::array<VkDynamicState, 2> mDynamicStates = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };
	};
}

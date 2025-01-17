#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include "Shader.h"
#include "RenderPass.h"

class Pipeline {
public:
	Pipeline(Device& device, Swapchain& swapchain);
	~Pipeline();

	void setShaderStages(const Shader& shaderModules);
	void create();

	VkPipeline vkHandle() const { return pipeline_; }

private:
	void setFixedState();

private:
	Device& device_;
	Swapchain& swapchain_;
	VkPipeline pipeline_ = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
	RenderPass renderPass_{ device_, swapchain_ };

	VkPipelineShaderStageCreateInfo vertexShaderStage_{};
	VkPipelineShaderStageCreateInfo fragmentShaderStage_{};
	VkPipelineDynamicStateCreateInfo dynamicState_{};
	VkPipelineInputAssemblyStateCreateInfo inputAssembly_{};
	VkPipelineVertexInputStateCreateInfo vertexInput_{};
	VkPipelineViewportStateCreateInfo viewportState_{};
	VkPipelineRasterizationStateCreateInfo rasterizer_{};
	VkPipelineMultisampleStateCreateInfo multisampleState_{};
	VkPipelineColorBlendStateCreateInfo colorBlending_{};

	const std::array<VkDynamicState, 2> dynamicStates_ = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };
	VkViewport baseViewport_{};
	VkRect2D scissor_{};
	VkPipelineColorBlendAttachmentState colorBlendAttachment_{};
};

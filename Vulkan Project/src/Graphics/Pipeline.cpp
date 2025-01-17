#include "Pipeline.h"
#include <Application.h>

Pipeline::Pipeline(Device& device, Swapchain& swapchain)
	: device_(device), swapchain_(swapchain) {
	setFixedState();
}

Pipeline::~Pipeline() {
	vkDestroyPipeline(device_.vkDevice(), pipeline_, nullptr);
	vkDestroyPipelineLayout(device_.vkDevice(), pipelineLayout_, nullptr);
	RDEBUG_LOG("pipeline and layout destroyed");
}

void Pipeline::setShaderStages(const Shader& shaderModules) {
	VkPipelineShaderStageCreateInfo vertStageInfo{};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = shaderModules.vertexShaderModule;
	vertStageInfo.pName = "main";
	vertexShaderStage_ = vertStageInfo;

	VkPipelineShaderStageCreateInfo fragStageInfo{};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = shaderModules.fragmentShaderModule;
	fragStageInfo.pName = "main";
	fragmentShaderStage_ = fragStageInfo;
}

void Pipeline::setFixedState() {
	dynamicState_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState_.dynamicStateCount = dynamicStates_.size();
	dynamicState_.pDynamicStates = dynamicStates_.data();

	vertexInput_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput_.vertexBindingDescriptionCount = 0;
	vertexInput_.vertexAttributeDescriptionCount = 0;

	inputAssembly_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly_.primitiveRestartEnable = VK_FALSE;

	baseViewport_.x = 0.0f;
	baseViewport_.y = 0.0f;
	baseViewport_.width = (float)swapchain_.extent().width;
	baseViewport_.height = (float)swapchain_.extent().height;
	baseViewport_.minDepth = 0.0f;
	baseViewport_.maxDepth = 1.0f;

	scissor_.offset = { 0, 0 };
	scissor_.extent = swapchain_.extent();

	viewportState_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState_.viewportCount = 1;
	viewportState_.pViewports = &baseViewport_;
	viewportState_.scissorCount = 1;
	viewportState_.pScissors = &scissor_;

	rasterizer_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer_.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_.depthClampEnable = VK_FALSE;
	rasterizer_.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_.lineWidth = 1.0f;
	rasterizer_.depthBiasEnable = VK_FALSE;

	multisampleState_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState_.sampleShadingEnable = VK_FALSE;
	multisampleState_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	colorBlendAttachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment_.blendEnable = VK_TRUE;
	colorBlendAttachment_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment_.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment_.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment_.alphaBlendOp = VK_BLEND_OP_ADD;

	colorBlending_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending_.logicOpEnable = VK_FALSE;
	colorBlending_.logicOp = VK_LOGIC_OP_COPY;
	colorBlending_.attachmentCount = 1;
	colorBlending_.pAttachments = &colorBlendAttachment_;

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	VkResult result = vkCreatePipelineLayout(device_.vkDevice(), &layoutInfo, nullptr, &pipelineLayout_);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("couldnt create Pipeline Layout");
	}
}

void Pipeline::create() {
	if (vertexShaderStage_.sType != VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO
		|| fragmentShaderStage_.sType != VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO) {
		throw std::runtime_error("cannot create pipeline without shader stages");
	}

	VkPipelineShaderStageCreateInfo stages[] = { vertexShaderStage_, fragmentShaderStage_ };
	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = stages;
	createInfo.pVertexInputState = &vertexInput_;
	createInfo.pInputAssemblyState = &inputAssembly_;
	createInfo.pViewportState = &viewportState_;
	createInfo.pRasterizationState = &rasterizer_;
	createInfo.pMultisampleState = &multisampleState_;
	createInfo.pColorBlendState = &colorBlending_;
	createInfo.pDynamicState = &dynamicState_;
	createInfo.layout = pipelineLayout_;
	createInfo.renderPass = renderPass_.vkHandle();
	createInfo.subpass = 0;
	
	VkResult result = vkCreateGraphicsPipelines(device_.vkDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline_);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("couldn't create a graphics pipeline");
	}
}

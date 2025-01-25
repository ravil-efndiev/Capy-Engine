#include "Pipeline.h"
#include <Application.h>

namespace cp {
	Pipeline::Pipeline(Device& device, Swapchain& swapchain, const PipelineConfiguration& config)
		: mDevice(device), mSwapchain(swapchain), mConfig(config) {

		CP_ASSERT(mConfig.pShader != nullptr, "cannot create pipeline with null shader, set pShader in PipelineConfiguration");
		setShaderStages(*mConfig.pShader);
		create();
	}

	Pipeline::~Pipeline() {
		vkDestroyPipeline(mDevice.vkDevice(), mPipeline, nullptr);
		vkDestroyPipelineLayout(mDevice.vkDevice(), mPipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(mDevice.vkDevice(), mDescSetLayout, nullptr);
		CP_DEBUG_LOG("pipeline and layout destroyed");
	}

	void Pipeline::setShaderStages(const Shader& shaderModules) {
		VkPipelineShaderStageCreateInfo vertStageInfo{};
		vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStageInfo.module = shaderModules.vertexShaderModule;
		vertStageInfo.pName = "main";
		mVertexShaderStage = vertStageInfo;

		VkPipelineShaderStageCreateInfo fragStageInfo{};
		fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStageInfo.module = shaderModules.fragmentShaderModule;
		fragStageInfo.pName = "main";
		mFragmentShaderStage = fragStageInfo;
	}

	void Pipeline::create() {
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = (uint)mDynamicStates.size();
		dynamicStateInfo.pDynamicStates = mDynamicStates.data();

		VkVertexInputBindingDescription bindingDesc{};
		AttributeDescriptions attribDescs;

		switch (mConfig.vertexType) {
		case PipelineConfiguration::PositionColorVertex:
			bindingDesc = PositionColorVertex::bindingDescription();
			attribDescs = PositionColorVertex::attributeDescriptions();
			break;
		case PipelineConfiguration::TexCoordVertex:
			bindingDesc = SpriteVertex::bindingDescription();
			attribDescs = SpriteVertex::attributeDescriptions();
			break;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint)attribDescs.size();
		vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport baseViewport{};
		baseViewport.x = 0.0f;
		baseViewport.y = 0.0f;
		baseViewport.width = (float)mSwapchain.extent().width;
		baseViewport.height = (float)mSwapchain.extent().height;
		baseViewport.minDepth = 0.0f;
		baseViewport.maxDepth = 1.0f;

		VkRect2D baseScissor{};
		baseScissor.offset = { 0, 0 };
		baseScissor.extent = mSwapchain.extent();

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &baseViewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &baseScissor;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.cullMode = mConfig.culling;
		rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerInfo.polygonMode = mConfig.polygonMode;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.sampleShadingEnable = VK_FALSE;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachment.blendEnable = VK_TRUE;
		blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &blendAttachment;

		std::vector<VkDescriptorSetLayoutBinding> descSetBindings(mConfig.descriptorSetBindings.size());
		for (size_t i = 0; i < descSetBindings.size(); i++) {
			descSetBindings[i].binding = (uint)i;
			descSetBindings[i].descriptorCount = 1;
			descSetBindings[i].descriptorType = mConfig.descriptorSetBindings[i].descriptorType;
			descSetBindings[i].stageFlags = mConfig.descriptorSetBindings[i].shaderStage;
		}

		VkPushConstantRange pcRange{};
		pcRange.offset = 0;
		pcRange.size = sizeof(glm::mat4);
		pcRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo descSetLayoutInfo{};
		descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descSetLayoutInfo.bindingCount = (uint)descSetBindings.size();
		descSetLayoutInfo.pBindings = descSetBindings.data();

		VkResult descLayoutResult = vkCreateDescriptorSetLayout(mDevice.vkDevice(), &descSetLayoutInfo, nullptr, &mDescSetLayout);
		checkVkResult(descLayoutResult, "failed to create descriptor set layout");

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &mDescSetLayout;
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pcRange;

		VkResult layoutResult = vkCreatePipelineLayout(mDevice.vkDevice(), &layoutInfo, nullptr, &mPipelineLayout);
		checkVkResult(layoutResult, "couldnt create Pipeline Layout");

		VkPipelineShaderStageCreateInfo stages[] = { mVertexShaderStage, mFragmentShaderStage };
		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = 2;
		createInfo.pStages = stages;
		createInfo.pVertexInputState = &vertexInputInfo;
		createInfo.pInputAssemblyState = &inputAssemblyInfo;
		createInfo.pViewportState = &viewportInfo;
		createInfo.pRasterizationState = &rasterizerInfo;
		createInfo.pMultisampleState = &multisampleInfo;
		createInfo.pColorBlendState = &colorBlendInfo;
		createInfo.pDynamicState = &dynamicStateInfo;
		createInfo.layout = mPipelineLayout;
		createInfo.renderPass = mRenderPass.vkHandle();
		createInfo.subpass = 0;

		VkResult pipelineResult = vkCreateGraphicsPipelines(mDevice.vkDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &mPipeline);
		checkVkResult(pipelineResult, "couldn't create a graphics pipeline");
	}
}

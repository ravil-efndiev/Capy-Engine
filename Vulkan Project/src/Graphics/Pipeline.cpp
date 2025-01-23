#include "Pipeline.h"
#include <Application.h>

namespace cp {
	Pipeline::Pipeline(Device& device, Swapchain& swapchain, const PipelineConfiguration& config)
		: mDevice(device), mSwapchain(swapchain), mConfig(config) {

		CP_ASSERT(mConfig.pShader != nullptr, "cannot create pipeline with null shader, set pShader in PipelineConfiguration");
		setShaderStages(*mConfig.pShader);
		setFixedState();
		create();
	}

	Pipeline::~Pipeline() {
		vkDestroyPipeline(mDevice.vkDevice(), mPipeline, nullptr);
		vkDestroyPipelineLayout(mDevice.vkDevice(), mPipelineLayout, nullptr);
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

	void Pipeline::setFixedState() {
		mDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		mDynamicState.dynamicStateCount = mDynamicStates.size();
		mDynamicState.pDynamicStates = mDynamicStates.data();

		switch (mConfig.vertexType) {
		case PipelineConfiguration::PositionColorVertex:
			mBindingDesc = PositionColorVertex::bindingDescription();
			mAttribDescs = PositionColorVertex::attributeDescriptions();
			break;
		case PipelineConfiguration::TexCoordVertex:
			mBindingDesc = SpriteVertex::bindingDescription();
			mAttribDescs = SpriteVertex::attributeDescriptions();
			break;
		}

		mVertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		mVertexInput.vertexBindingDescriptionCount = 1;
		mVertexInput.pVertexBindingDescriptions = &mBindingDesc;
		mVertexInput.vertexAttributeDescriptionCount = mAttribDescs.size();
		mVertexInput.pVertexAttributeDescriptions = mAttribDescs.data();

		mInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		mInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mInputAssembly.primitiveRestartEnable = VK_FALSE;

		mBaseViewport.x = 0.0f;
		mBaseViewport.y = 0.0f;
		mBaseViewport.width = (float)mSwapchain.extent().width;
		mBaseViewport.height = (float)mSwapchain.extent().height;
		mBaseViewport.minDepth = 0.0f;
		mBaseViewport.maxDepth = 1.0f;

		mScissor.offset = { 0, 0 };
		mScissor.extent = mSwapchain.extent();

		mViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		mViewportState.viewportCount = 1;
		mViewportState.pViewports = &mBaseViewport;
		mViewportState.scissorCount = 1;
		mViewportState.pScissors = &mScissor;

		mRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		mRasterizer.cullMode = mConfig.culling;
		mRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		mRasterizer.polygonMode = mConfig.polygonMode;
		mRasterizer.depthClampEnable = VK_FALSE;
		mRasterizer.rasterizerDiscardEnable = VK_FALSE;
		mRasterizer.lineWidth = 1.0f;
		mRasterizer.depthBiasEnable = VK_FALSE;

		mMultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		mMultisampleState.sampleShadingEnable = VK_FALSE;
		mMultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		mColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		mColorBlendAttachment.blendEnable = VK_TRUE;
		mColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		mColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		mColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		mColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		mColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		mColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		mColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		mColorBlending.logicOpEnable = VK_FALSE;
		mColorBlending.logicOp = VK_LOGIC_OP_COPY;
		mColorBlending.attachmentCount = 1;
		mColorBlending.pAttachments = &mColorBlendAttachment;

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		VkResult result = vkCreatePipelineLayout(mDevice.vkDevice(), &layoutInfo, nullptr, &mPipelineLayout);
		checkVkResult(result, "couldnt create Pipeline Layout");
	}

	void Pipeline::create() {
		if (mVertexShaderStage.sType != VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO
			|| mFragmentShaderStage.sType != VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO) {
			throw std::runtime_error("cannot create pipeline without shader stages");
		}

		VkPipelineShaderStageCreateInfo stages[] = { mVertexShaderStage, mFragmentShaderStage };
		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = 2;
		createInfo.pStages = stages;
		createInfo.pVertexInputState = &mVertexInput;
		createInfo.pInputAssemblyState = &mInputAssembly;
		createInfo.pViewportState = &mViewportState;
		createInfo.pRasterizationState = &mRasterizer;
		createInfo.pMultisampleState = &mMultisampleState;
		createInfo.pColorBlendState = &mColorBlending;
		createInfo.pDynamicState = &mDynamicState;
		createInfo.layout = mPipelineLayout;
		createInfo.renderPass = mRenderPass.vkHandle();
		createInfo.subpass = 0;

		VkResult result = vkCreateGraphicsPipelines(mDevice.vkDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &mPipeline);
		checkVkResult(result, "couldn't create a graphics pipeline");
	}
}

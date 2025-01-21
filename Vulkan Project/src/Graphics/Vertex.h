#pragma once
#include <include.h>

struct PositionColorVertex {
	glm::vec3 position;
	glm::vec4 color;

	using AttributeDescriptions = std::array<VkVertexInputAttributeDescription, 2>;

	static VkVertexInputBindingDescription bindingDescription() {
		VkVertexInputBindingDescription desc{};
		desc.binding = 0;
		desc.stride = sizeof(PositionColorVertex);
		desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return desc;
	}

	 static AttributeDescriptions attributeDescriptions() {
		AttributeDescriptions descs{};
		descs[0].binding = 0;
		descs[0].location = 0;
		descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		descs[0].offset = offsetof(PositionColorVertex, position);

		descs[1].binding = 0;
		descs[1].location = 1;
		descs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		descs[1].offset = offsetof(PositionColorVertex, color);

		return descs;
	}
};

#pragma once
#include <include.h>

namespace cp {
	using AttributeDescriptions = std::vector<VkVertexInputAttributeDescription>;

	struct PositionColorVertex {
		glm::vec3 position;
		glm::vec4 color;

		static VkVertexInputBindingDescription bindingDescription() {
			VkVertexInputBindingDescription desc{};
			desc.binding = 0;
			desc.stride = sizeof(PositionColorVertex);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return desc;
		}

		static AttributeDescriptions attributeDescriptions() {
			AttributeDescriptions descs(2);
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

	struct SpriteVertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription bindingDescription() {
			VkVertexInputBindingDescription desc{};
			desc.binding = 0;
			desc.stride = sizeof(SpriteVertex);
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return desc;
		}

		static AttributeDescriptions attributeDescriptions() {
			AttributeDescriptions descs(3);
			descs[0].binding = 0;
			descs[0].location = 0;
			descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			descs[0].offset = offsetof(SpriteVertex, position);

			descs[1].binding = 0;
			descs[1].location = 1;
			descs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			descs[1].offset = offsetof(SpriteVertex, color);

			descs[2].binding = 0;
			descs[2].location = 1;
			descs[2].format = VK_FORMAT_R32G32_SFLOAT;
			descs[2].offset = offsetof(SpriteVertex, texCoord);

			return descs;
		}
	};
}

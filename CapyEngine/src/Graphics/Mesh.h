#pragma once
#include "Buffers.h"
#include "Vertex.h"

namespace cp {
	template <class VertexT>
	class Mesh {
	public:
		Mesh(const std::vector<VertexT>& vertices, const std::vector<uint16>& indices);

		VertexBuffer& vertexBuffer() const { return mVertexBuffer; }
		IndexBuffer& indexBuffer() const { return mIndexBuffer; }
		
		std::vector<VertexT> vertices() const { return mVertices; }
		std::vector<uint16> indices() const { return mIndices; }

	private:
		Device& mDevice;
		std::vector<VertexT> mVertices;
		std::vector<uint16> mIndices;
		mutable VertexBuffer mVertexBuffer;
		mutable IndexBuffer mIndexBuffer;
	};
}

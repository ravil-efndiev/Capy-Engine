#include "Mesh.h"

namespace cp {
	template<class VertexT>
	Mesh<VertexT>::Mesh(Device& device, const std::vector<VertexT>& vertices, const std::vector<uint16>& indices) : 
		mDevice(device), mVertices(vertices), mIndices(indices),
		mVertexBuffer(mDevice, mVertices), 
		mIndexBuffer(mDevice, mIndices) {}

	template class Mesh<PositionColorVertex>;
	template class Mesh<SpriteVertex>;
}

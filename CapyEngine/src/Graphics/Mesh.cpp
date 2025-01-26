#include "Mesh.h"
#include <Application.h>

namespace cp {
	template<class VertexT>
	Mesh<VertexT>::Mesh(const std::vector<VertexT>& vertices, const std::vector<uint16>& indices) : 
		mDevice(Application::get().device()),
		mVertices(vertices), mIndices(indices),
		mVertexBuffer(mDevice, mVertices), 
		mIndexBuffer(mDevice, mIndices) {}

	template class Mesh<PositionColorVertex>;
	template class Mesh<SpriteVertex>;
}

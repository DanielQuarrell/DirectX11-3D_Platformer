#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const GameObject& parent) : vcbuf(gfx), parent(parent){}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	vcbuf.Update(gfx, 
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * 
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	);

	vcbuf.Bind(gfx);
}

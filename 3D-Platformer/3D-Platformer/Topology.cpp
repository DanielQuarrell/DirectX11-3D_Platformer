#include "Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type) : type(type) {}

void Topology::Bind(Graphics& gfx) noexcept
{
	//Set primitive topology to triangle list
	GetContext(gfx)->IASetPrimitiveTopology(type);
}

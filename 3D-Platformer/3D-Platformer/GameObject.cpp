#include "GameObject.h"
#include "IndexBuffer.h"

void GameObject::Draw(Graphics& gfx) const noexcept
{
	//Loop through the bindables and bind them to the pipeline
	for (auto& bindable : binds)
	{
		bindable->Bind(gfx);
	}
	//Draw object
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void GameObject::AddBind(std::unique_ptr<Bindable> bind) noexcept
{
	//MUST use AddIndexBuffer to bind index buffer
	binds.push_back(std::move(bind));
}

void GameObject::AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept
{
	//Don't add index buffer twice
	pIndexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}

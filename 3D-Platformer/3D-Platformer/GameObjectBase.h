#pragma once
#include "GameObject.h"
#include "IndexBuffer.h"

//Class for sharing unchanging bindables per object
template<class T>
class GameObjectBase : public GameObject
{
protected:
	//Check if the static data has been initialised so it is only done once per object
	static bool IsStaticInitialised()
	{
		return !staticBinds.empty();
	}

	//Add the static binds
	static void AddStaticBind(std::unique_ptr<Bindable> bind)
	{
		staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> _indexBuffer) 
	{
		pIndexBuffer = _indexBuffer.get();
		staticBinds.push_back(std::move(_indexBuffer));
	}

	//Find the index buffer from staticBinds so it can be applied to all objects
	void SetIndexFromStatic()
	{
		for (const auto& b : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		OutputDebugStringW(L"Failed to find index buffer in static binds");
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> GameObjectBase<T>::staticBinds;
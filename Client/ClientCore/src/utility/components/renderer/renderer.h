#pragma once
#include "src/utility/components/component.h"

class GameObject;

class Renderer final
{
public:
	enum class kRenderGroup
	{
		kRenderPriority,
		kRenderNonAlpha,
		kRenderAlpha,
		kRenderUi,
		kRenderMouse,
		kRenderEnd
	};

private:
	explicit Renderer() = default;
	~Renderer() = default;

public:
	static auto GetInstance() -> Renderer&
	{
		static Renderer instance;
		return instance;
	}

public:
	auto AddRenderGroup(kRenderGroup group, std::shared_ptr<GameObject> gameObject)->HRESULT;
	auto RenderGameObject()->HRESULT;

private:
	auto RenderPriority()->HRESULT;
	auto RenderNonAlpha()->HRESULT;
	auto RenderAlpha()->HRESULT;
	auto RenderUi()->HRESULT;
	auto RenderMouse()->HRESULT;

private:
	std::vector<std::shared_ptr<GameObject>> _render_objects[static_cast<int32_t>(kRenderGroup::kRenderEnd)];
};


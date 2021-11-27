#include "pch.h"
#include "renderer.h"
#include "src/utility/game_objects/game_object.h"

auto Renderer::AddRenderGroup(kRenderGroup group, std::shared_ptr<GameObject> gameObject) -> HRESULT
{
	if (nullptr == gameObject)
		return E_FAIL;

	_render_objects[static_cast<int32_t>(group)].push_back(gameObject);
	return S_OK;
}

auto Renderer::RenderGameObject() -> HRESULT
{
	RenderPriority();
	RenderNonAlpha();
	RenderAlpha();
	RenderUi();
	return S_OK;
}

auto Renderer::RenderPriority() -> HRESULT
{
	constexpr auto index = static_cast<int32_t>(kRenderGroup::kRenderPriority);
	for (const auto& gameObject : _render_objects[index])
	{
		if (FAILED(gameObject->Render()))
		{
			return E_FAIL;
		}
	}
	_render_objects[index].clear();
	return S_OK;
}

auto Renderer::RenderNonAlpha() -> HRESULT
{
	constexpr auto index = static_cast<int32_t>(kRenderGroup::kRenderNonAlpha);
	for (const auto& gameObject : _render_objects[index])
	{
		if (FAILED(gameObject->Render()))
		{
			return E_FAIL;
		}
	}
	_render_objects[index].clear();
	return S_OK;
}

auto Renderer::RenderAlpha() -> HRESULT
{
	constexpr auto index = static_cast<int32_t>(kRenderGroup::kRenderAlpha);
	for (const auto& gameObject : _render_objects[index])
	{
		if (FAILED(gameObject->Render()))
		{
			return E_FAIL;
		}
	}
	_render_objects[index].clear();
	return S_OK;
}

auto Renderer::RenderUi() -> HRESULT
{
	constexpr auto index = static_cast<int32_t>(kRenderGroup::kRenderUi);
	for (const auto& gameObject : _render_objects[index])
	{
		if (FAILED(gameObject->Render()))
		{
			return E_FAIL;
		}
	}
	_render_objects[index].clear();
	return S_OK;
}

#include "c_pch.h"
#include "player_info.h"

#include "src/managers/character_stat/character_stat.h"
#include "src/system/input/input_device.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

PlayerInfo::PlayerInfo(const ComPtr<IDirect3DDevice9>& device):
	GameObject(device)
{
}

HRESULT PlayerInfo::NativeConstructPrototype()
{
	return GameObject::NativeConstructPrototype();
}

HRESULT PlayerInfo::NativeConstruct(void* arg)
{
	if (FAILED(AddComponents()))
		return E_FAIL;

	_size_x = 136;//169 136
	_size_y = 153;//191 153
	_x = 640;
	_y = 630;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);
	return GameObject::NativeConstruct(arg);
}

int32_t PlayerInfo::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

int32_t PlayerInfo::LateTick(const double timeDelta)
{
	Renderer::GetInstance().AddRenderGroup(Renderer::kRenderGroup::kRenderUi, shared_from_this());
	return GameObject::LateTick(timeDelta);
}

HRESULT PlayerInfo::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size_x;
	transformMatrix._22 = _size_y;
	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -_y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 0);


	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	HpMpRender();
	
	SubFrameRender();
	result = _shader_com->EndShader();
	return GameObject::Render();
}

auto PlayerInfo::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	auto pInstance = std::make_shared<PlayerInfo>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone PlayerInfo");
		return nullptr;
	}
	return pInstance;
}

auto PlayerInfo::Create(const ComPtr<IDirect3DDevice9>& device) -> std::shared_ptr<PlayerInfo>
{
	auto pInstance = std::make_shared<PlayerInfo>(device);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Create PlayerInfo");
		return nullptr;
	}
	return pInstance;
}

auto PlayerInfo::AddComponents() -> HRESULT
{
	/* Com_VIBuffer */
	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_Frame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_HpGage"), TEXT("Com_Texture_Hp"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_hp))))
		return E_FAIL;
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_MpGage"), TEXT("Com_Texture_Mp"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_mp))))
		return E_FAIL;
	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_ExpBarFrame"), TEXT("Com_ExpBar_Frame"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_exp_frame))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_PlayerInfo_ExpBarProgress"), TEXT("Com_ExpBar_Progress"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_exp_progress))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui",
		L"Com_Shader",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	if (FAILED(GameObject::AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui_Gage",
		L"Com_Shader_Hp",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_gage))))
		return E_FAIL;

	return S_OK;
}

auto PlayerInfo::HpMpRender() -> HRESULT
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = 55;//138
	transformMatrix._22 = 119;//151
	transformMatrix._41 = (_x - 27) - (g_WinCX >> 1);
	transformMatrix._42 = -(_y - 8) + (g_WinCY >> 1);

	const auto& statInstance = CharacterStat::GetInstance();
	const float hpPercent =
		static_cast<float>(statInstance.GetHp()) / static_cast<float>(statInstance.GetMaxHp()) * 100.f;
	float hpHeight = 1.f - hpPercent / 100;
	auto result = _shader_gage->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_gage->SetUpConstantTable("g_percentY", &hpHeight, sizeof(float));
	constexpr float x = 1.f;
	result = _shader_gage->SetUpConstantTable("g_percentX", &x, sizeof(float));
	result = _shader_gage->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_gage->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_gage->SetUpTextureConstantTable("g_DiffuseTexture", _texture_hp);
	result = _shader_gage->BeginShader(0);
	result = _vi_buffer_com->RenderViBuffer();
	MpRender();
	ExpRender();
	result = _shader_gage->EndShader();
	return S_OK;
}

auto PlayerInfo::MpRender() -> HRESULT
{
	_matrix			transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	transformMatrix._11 = 55;//138
	transformMatrix._22 = 119;//151
	transformMatrix._41 = (_x - -27) - (g_WinCX >> 1);
	transformMatrix._42 = -(_y - 8) + (g_WinCY >> 1);

	const auto& statInstance = CharacterStat::GetInstance();
	const float mpPercent =
		static_cast<float>(statInstance.GetMp()) / static_cast<float>(statInstance.GetMaxMp()) * 100.f;
	float hpHeight = 1.f - mpPercent / 100;

	auto result = _shader_gage->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_gage->SetUpTextureConstantTable("g_DiffuseTexture", _texture_mp);
	result = _shader_gage->SetUpConstantTable("g_percentY", &hpHeight, sizeof(float));
	const float x = 1.f;
	result = _shader_gage->SetUpConstantTable("g_percentX", &x, sizeof(float));
	_shader_gage->Commit();
	result = _vi_buffer_com->RenderViBuffer();
	return S_OK;
}

auto PlayerInfo::ExpRender() -> HRESULT
{
	_matrix			transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	transformMatrix._11 = 1280;//138
	transformMatrix._22 = 15;//151
	transformMatrix._41 = (g_WinCX / 2) - (g_WinCX >> 1);
	transformMatrix._42 = -(713) + (g_WinCY >> 1);

	const auto& statInstance = CharacterStat::GetInstance();
	const float expPercent =
		static_cast<float>(statInstance.GetExp()) / static_cast<float>(statInstance.GetMaxMp()) * 100.f;
	float expWidth = 1.f - expPercent / 100;

	auto result = _shader_gage->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_gage->SetUpTextureConstantTable("g_DiffuseTexture", _texture_exp_frame);
	constexpr float x = 1.f;
	result = _shader_gage->SetUpConstantTable("g_percentX", &x, sizeof(float));
	constexpr float y = 0.f;
	result = _shader_gage->SetUpConstantTable("g_percentY", &y, sizeof(float));
	_shader_gage->Commit();
	result = _vi_buffer_com->RenderViBuffer();
	ExpProgressRender();
	return S_OK;
}

auto PlayerInfo::ExpProgressRender() -> HRESULT
{
	static float a = 0;

	if (InputDevice::GetInstance().GetKeyDown(DIK_O))
	{
		a--;
	}
	if (InputDevice::GetInstance().GetKeyDown(DIK_P))
	{
		a++;
	}
	_matrix			transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	transformMatrix._11 = 1273;//138
	transformMatrix._22 = 6;//151
	transformMatrix._41 = (g_WinCX / 2) - (g_WinCX >> 1);
	transformMatrix._42 = -(713) + (g_WinCY >> 1);

	const auto& statInstance = CharacterStat::GetInstance();
	const float expPercent =
		static_cast<float>(statInstance.GetExp() + 13) / static_cast<float>(statInstance.GetMaxMp()) * 100.f;
	float expWidth = expPercent / 100;

	auto result = _shader_gage->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_gage->SetUpTextureConstantTable("g_DiffuseTexture", _texture_exp_progress);
	constexpr float x = 1.f;
	result = _shader_gage->SetUpConstantTable("g_percentX", &expWidth, sizeof(float));
	constexpr float y = 0.f;
	result = _shader_gage->SetUpConstantTable("g_percentY", &y, sizeof(float));
	_shader_gage->Commit();
	result = _vi_buffer_com->RenderViBuffer();
	return S_OK;
}

auto PlayerInfo::SubFrameRender() -> HRESULT
{
	//static int a = 0;
	//static int b = 0;

	//if (InputDevice::GetInstance().GetKeyDown(DIK_O))
	//{
	//	a--;
	//}
	//if (InputDevice::GetInstance().GetKeyDown(DIK_P))
	//{
	//	a++;
	//}
	//if (InputDevice::GetInstance().GetKeyDown(DIK_K))
	//{
	//	b--;
	//}
	//if (InputDevice::GetInstance().GetKeyDown(DIK_L))
	//{
	//	b++;
	//}
	_matrix			transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	transformMatrix._11 = 111;//138
	transformMatrix._22 = 121;//151
	transformMatrix._41 = _x - (g_WinCX >> 1);
	transformMatrix._42 = -(_y - 8) + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_com, 1);
	_shader_com->Commit();
	result = _vi_buffer_com->RenderViBuffer();
	return S_OK;
}

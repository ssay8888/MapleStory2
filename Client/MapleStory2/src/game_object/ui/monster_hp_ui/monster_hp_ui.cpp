#include "c_pch.h"
#include "monster_hp_ui.h"

#include "src/game_object/monster/monster.h"
#include "src/game_object/monster/stats/monster_stats.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"

MonsterHpUi::MonsterHpUi():
	GameObject(GraphicDevice::GetInstance().GetDevice())
{
	NativeConstructPrototype();
}

HRESULT MonsterHpUi::NativeConstructPrototype()
{
	if (FAILED(AddComponents()))
	{
		return E_FAIL;
	}
	_size.x = 356;
	_size.y = 51;
	_pos.x = (g_WinCX >> 1);
	_pos.y = (g_WinCY >> 1) - 300.f;
	D3DXMatrixOrthoLH(&_proj_matrix, g_WinCX, g_WinCY, 0.f, 1.f);

	return GameObject::NativeConstructPrototype();
}

HRESULT MonsterHpUi::NativeConstruct(void* arg)
{
	return GameObject::NativeConstruct(arg);
}

HRESULT MonsterHpUi::Tick(const double timeDelta)
{
	return GameObject::Tick(timeDelta);
}

HRESULT MonsterHpUi::LateTick(const double timeDelta)
{
	return GameObject::LateTick(timeDelta);
}

HRESULT MonsterHpUi::Render()
{
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);

	transformMatrix._11 = 302;
	transformMatrix._22 = 31;
	transformMatrix._41 = _pos.x + 19 - (g_WinCX >> 1);
	transformMatrix._42 = -_pos.y + (g_WinCY >> 1);
	auto info = DataReaderManager::GetInstance().FindMonsterInfo(_monster->GetMonsterInfo().monster_id());

	const float hpPercent =
		static_cast<float>(_monster->GetStat()->GetHp()) / static_cast<float>(info->stat.hp) * 100.f;
	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	float hpHeight = 0.f;
	result = _shader_com->SetUpConstantTable("g_percentY", &hpHeight, sizeof(float));
	const float x = hpPercent / 100.f;
	result = _shader_com->SetUpConstantTable("g_percentX", &x, sizeof(float));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_hp_com, 0);
	_shader_com->Commit();
	//result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	//result = _shader_com->EndShader();
	return GameObject::Render();
}

auto MonsterHpUi::Render(std::shared_ptr<Shader> shader) -> HRESULT
{
	if (_monster == nullptr || _monster->IsDead() || _last_time + 5000 < GetTickCount64())
	{
		return S_OK;
	}
	_matrix			transformMatrix, viewMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	D3DXMatrixIdentity(&viewMatrix);
	transformMatrix._11 = _size.x;
	transformMatrix._22 = _size.y;
	transformMatrix._41 = _pos.x - (g_WinCX >> 1);
	transformMatrix._42 = -_pos.y + (g_WinCY >> 1);

	auto result = _shader_com->SetUpConstantTable("g_WorldMatrix", &transformMatrix, sizeof(_matrix));
	float hpHeight = 0.f;
	result = _shader_com->SetUpConstantTable("g_percentY", &hpHeight, sizeof(float));
	constexpr float x = 1.f;
	result = _shader_com->SetUpConstantTable("g_percentX", &x, sizeof(float));
	result = _shader_com->SetUpConstantTable("g_ViewMatrix", &viewMatrix, sizeof(_matrix));
	result = _shader_com->SetUpConstantTable("g_ProjMatrix", &_proj_matrix, sizeof(_matrix));
	result = _shader_com->SetUpTextureConstantTable("g_DiffuseTexture", _texture_frame_com, 0);
	//_shader_com->Commit();
	result = _shader_com->BeginShader(0);
	_vi_buffer_com->RenderViBuffer();
	Render();
	result = _shader_com->EndShader();

	return S_OK;
}

auto MonsterHpUi::Clone(void* arg) -> std::shared_ptr<GameObject>
{
	return nullptr;
}

auto MonsterHpUi::SetMonster(std::shared_ptr<Monster> monster) -> void
{
	_monster = monster;
	_last_time = GetTickCount64();
}

auto MonsterHpUi::AddComponents() -> HRESULT
{
	 
	if (FAILED(AddComponent(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<std::shared_ptr<Component>*>(&_vi_buffer_com))))
	{
		return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Monster_Hp_Ui_Frame"), TEXT("Com_Texture"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_frame_com))))
	{
	 return E_FAIL;
	}

	if (FAILED(AddComponent(kScene::kSceneGamePlay0, TEXT("Prototype_Texture_Monster_Hp_Ui_Bar"), TEXT("Com_Texture2"), reinterpret_cast<std::shared_ptr<Component>*>(&_texture_hp_com))))
	{
	 return E_FAIL;
	}
	if (FAILED(AddComponent(kScene::kSceneStatic,
		L"Prototype_Shader_Ui_Gage",
		L"Com_Shader_Hp",
		reinterpret_cast<std::shared_ptr<Component>*>(&_shader_com))))
		return E_FAIL;

	return S_OK;
}

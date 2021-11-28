#include "pch.h"
#include "loading.h"
#include <thread>

#include "src/game_object/player/player.h"
#include "src/game_object/sky/sky.h"
#include "src/game_object/terrain/terrain.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/vi_buffer/vi_buffer_cube/vi_buffer_cube.h"
#include "src/utility/components/vi_buffer/vi_buffer_terrain/vi_buffer_terrain.h"
#include "src/utility/game_objects/manager/object_manager.h"

Loading::Loading(const ComPtr<IDirect3DDevice9>& device): 
	_graphic_device(device)
{
}

auto Loading::NativeConstruct(const kScene scene)->HRESULT
{
	_next_level = scene;

	_system_message.append(L"로딩을 시작합니다.");
	std::thread thread([&]()
	{
			ThreadMain();
	});
	thread.join();
	return S_OK;
}

auto Loading::ThreadMain()->HRESULT
{
	WRITE_LOCK;
	HRESULT hr = 0;

	switch (this->_next_level)
	{
	case kScene::kSceneGamePlay0:
		hr = this->ReadyGamePlay0();
		break;
	case kScene::kSceneGamePlay1:
		hr = this->ReadyGamePlay1();
		break;
	default: 
		break;
	}

	if (FAILED(hr))
		return E_FAIL;
	

	return S_OK;
}

auto Loading::Create(const ComPtr<IDirect3DDevice9>& device, const kScene nextLevel) -> std::shared_ptr<Loading>
{
	auto pInstance = std::make_shared<Loading>(device);

	if (FAILED(pInstance->NativeConstruct(nextLevel)))
	{
		MSGBOX("Failed to Creating Loading");
		return nullptr;
	}


	return pInstance;
}

auto Loading::ReadyGamePlay0()->HRESULT
{
	/* 원형 게임오브젝 객체를 생성ㅇ핞다. */
	_system_message.clear();
	_system_message.append(L"게임리소스를 생성합니다.");

	auto& objectManager = ObjectManager::GetInstance();
	/* For.Prototype_Player*/
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Player"), Player::Create(_graphic_device))))
		return E_FAIL;

	/* For.Prototype_Terrain*/
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Terrain"), Terrain::Create(_graphic_device))))
		return E_FAIL;


	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Sky"), Sky::Create(_graphic_device))))
		return E_FAIL;

	/* For.Prototype_UI */
	//if (FAILED(objectManager->AddPrototype(TEXT("Prototype_UI"), UI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_Effect */
	//if (FAILED(objectManager->AddPrototype(TEXT("Prototype_Effect"), Effect::Create(m_pGraphic_Device))))
	//	return E_FAIL;



	auto& componentManager = ComponentManager::GetInstance();
	/* 원형 리소스 객체를 생성한다. */

	/* For.Prototype_Texture_Robby */
	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Robby"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Robby.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Sky"), Texture::Create(_graphic_device, Texture::kType::kTypeCube, TEXT("../../Binary/Resources/Textures/SkyBox/burger%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Texture_Terrain */
	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Terrain"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Terrain/Grass_%d.tga"), 2))))
		return E_FAIL;

	/* For.Prototype_VIBuffer_Terrain*/
	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Terrain"), ViBufferTerrain::Create(_graphic_device, 129, 129))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_Texture_Filter"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Terrain/Filter.bmp")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_VIBuffer_Cube"), ViBufferCube::Create(_graphic_device))))
		return E_FAIL;		   
							   
	if (FAILED(componentManager.AddPrototype(static_cast<int32>(kScene::kSceneGamePlay0), TEXT("Prototype_Shader_Terrain"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/Shader_Terrain.hlsl")))))
		return E_FAIL;

	_system_message.clear();
	_system_message.append(L"로딩이 완료되었습니다.");

	_is_finish = true;
	return S_OK;
}

auto Loading::ReadyGamePlay1()->HRESULT
{
	_is_finish = true;
	return S_OK;
}

auto Loading::GetSystemMessage() const -> std::wstring
{
	return _system_message;
}

auto Loading::IsFinished() const -> bool
{
	return _is_finish;
}

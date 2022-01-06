#include "c_pch.h"
#include "game_logic_queue.h"

#include "src/game_object/user/user.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

auto GameLogicQueue::GetCharacterInfo() const -> Protocol::GameServerLoadCharacter
{
	return _character_info;
}

auto GameLogicQueue::CharacterLoad(PacketSessionRef session, Protocol::GameServerLoadCharacter pkt) -> void
{
	const auto scene = SceneLoading::Create(GraphicDevice::GetInstance().GetDevice(), kSceneGamePlay0);
	if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
	{
		_character_info = pkt;
	}
}

auto GameLogicQueue::AddUser(PacketSessionRef session, Protocol::GameServerRespawnPlayer pkt)->void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer;
	layer.append(L"Layer_User_").append(std::to_wstring(pkt.character_id()));
	objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_User"), layer, &pkt);
}

auto GameLogicQueue::MovePlayer(PacketSessionRef session, Protocol::GameServerMovePlayer pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();
	std::wstring layer;
	layer.append(L"Layer_User_").append(std::to_wstring(pkt.character_id()));
	const auto userObject = std::static_pointer_cast<User>(objectManager.GetGameObjectPtr(kSceneGamePlay0, layer, 0));
	if (userObject)
	{
		auto transform = std::static_pointer_cast<Transform>(userObject->GetComponentPtr(L"Com_Transform"));
		transform->SetState(Transform::kState::kStatePosition, _float3(pkt.position().x(), pkt.position().y(), pkt.position().z()));
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), pkt.radian());
		userObject->ChangeAnimation(static_cast<kAnimationType>(pkt.state()));
	}
}

auto GameLogicQueue::RespawnMonster(PacketSessionRef session, Protocol::GameServerRespawnMonster pkt) -> void
{
	auto& objectManager = ObjectManager::GetInstance();

	wchar_t LayerTag[MAX_PATH];
	swprintf_s(LayerTag, L"Layer_Monster_%lld", pkt.object_id());
	if (FAILED(objectManager.AddGameObject(kScene::kSceneGamePlay0, TEXT("Prototype_Mesh_Monster"), LayerTag, &pkt)))
	{
		GetInstance()->DoAsync(&GameLogicQueue::RespawnMonster, session, pkt);
	}
}


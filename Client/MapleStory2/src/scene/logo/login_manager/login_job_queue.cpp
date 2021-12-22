#include "c_pch.h"
#include "login_job_queue.h"

#include "src/game_object/equipped/equipped.h"
#include "src/game_object/ui/character_select/character_select_ui.h"
#include "src/main/main_app.h"
#include "src/managers/characters_manager/character.h"
#include "src/managers/characters_manager/characters_manager.h"
#include "src/network/send_manager.h"
#include "src/network/login_server_packet_handler.h"
#include "src/scene/loading/scene_loading.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"
#include "string_utils/string_utils.h"

auto LoginJobQueue::LoginAttempt(PacketSessionRef session, Protocol::LoginServerLogin pkt) -> Protocol::kLoginMessage
{
	std::cout << " 로그인 처리 " << std::endl;
	if (pkt.result() == Protocol::kLoginMessage::kOk)
	{
		const auto scene = SceneLoading::Create(GraphicDevice::GetInstance().GetDevice(), kSceneCharacterSelect);
		if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
		{
			GameLogicManager::Clear(static_cast<uint32_t>(kSceneLogo));
		}
		EnableWindow(g_hEdit, false);
		Protocol::LoginClientCharacterList pkt;
		
		SendManager::GetInstance().Push(LoginServerPacketHandler::MakeSendBuffer(pkt));
		return Protocol::kLoginMessage::kOk;
	}
	EnableWindow(g_hEdit, true);
	return pkt.result();
}

auto LoginJobQueue::LoadCharacterList(PacketSessionRef session,
	Protocol::LoginServerCharacterList pkt) -> void
{
	auto& instance = CharactersManager::GetInstance();

	for (auto character : pkt.characters())
	{
		auto player = Character::Create();
		player->SetInfo(character);
		const auto items = character.eqp_items();
		for (const auto item : items)
		{
			player->GetEqpList()->AddItem(GameContents::EquipeType(item), item);
		}
		instance.PushCharacter(player);
	}
}

auto LoginJobQueue::CreateCharacter(PacketSessionRef session,
                                    Protocol::LoginServerCreateCharacter pkt) -> Protocol::kCreateMessage
{
	std::cout << "캐릭터 생성" << std::endl;
	auto& instance = ObjectManager::GetInstance();
	auto gameObject = std::static_pointer_cast<CharacterSelectUi>(instance.GetGameObjectPtr(kSceneCharacterSelect, L"Layer_SelectUi", 0));

	if (gameObject)
	{
		gameObject->ChangeState(CharacterSelectUi::kCharacterSelectState::kSelectInit);
	}
	auto result = pkt.result();
	if (result == Protocol::kCreateSuccess)
	{
		const auto character = Character::Create();
		character->SetInfo(pkt.character());
		auto items = pkt.character().eqp_items();
		for (const auto item : items)
		{
			character->GetEqpList()->AddItem(GameContents::EquipeType(item), item);
		}
		CharactersManager::GetInstance().PushCharacter(character);
	}
	return result;
}

auto LoginJobQueue::CharacterSelect(PacketSessionRef session, Protocol::LoginServerCharacterSelect pkt) -> void
{
	if (pkt.result() == Protocol::kSelectSuccess)
	{
		const auto scene = SceneLoading::Create(GraphicDevice::GetInstance().GetDevice(), kSceneGameInit);
		if (SUCCEEDED(SceneManager::GetInstance().SetUpScene(scene)))
		{
			GameLogicManager::Clear(static_cast<uint32_t>(kSceneCharacterSelect));
		}
		g_mainApp->SetAuthInfo(pkt);
		g_mainApp->ClientNetworkThreadInit(
			StringUtils::ConvertCtoW(pkt.ip().c_str()), 
			pkt.port(), 
			Service::kServerType::kClientGame, 
			1, 
			2);
	}
}

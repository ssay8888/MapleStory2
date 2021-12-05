#include "c_pch.h"
#include "login_job_queue.h"

#include "src/scene/loading/scene_loading.h"
#include "src/system/graphic/graphic_device.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/scene_utility/scene_manager.h"

auto LoginJobQueue::LoginAttempt(PacketSessionRef session, Protocol::ServerLogin pkt) -> Protocol::kLoginMessage
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
		return Protocol::kLoginMessage::kOk;
	}
	EnableWindow(g_hEdit, true);
	return Protocol::kLoginMessage::kOk;
}

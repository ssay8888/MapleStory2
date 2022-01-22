#include "c_pch.h"
#include "main_app.h"

#include <iostream>

#include "data_reader/data_reader_manager.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/camera/camera_free.h"
#include "src/game_object/ui/login/text_box_ui.h"
#include "src/network/send_manager.h"
#include "src/network/login_server_packet_handler.h"
#include "src/network/server_session.h"
#include "src/network/service.h"
#include "src/network/socket_utils.h"
#include "src/scene/logo/scene_logo.h"
#include "src/system/graphic/graphic_device.h"
#include "src/thread/thread_manager.h"
#include "src/utility/components/manager/component_manager.h"
#include "src/utility/components/picking/picking.h"
#include "src/utility/components/renderer/renderer.h"
#include "src/utility/components/shader/shader.h"
#include "src/utility/components/textures/texture.h"
#include "src/utility/components/transform/transform.h"
#include "src/utility/components/vi_buffer/vi_buffer_rect/vi_buffer_rect.h"
#include "src/utility/game_logic_manager/game_logic_manager.h"
#include "src/utility/game_objects/manager/object_manager.h"
#include "src/utility/scene_utility/scene_manager.h"
#include "src/common/xml/map_parser.h"
#include "src/game_object/camera/ingame_camera.h"
#include "src/game_object/ui/loading_ui/loading_ui.h"
#include "src/game_object/user/user.h"
#include "src/managers/sound_manager/sound_manager.h"
#include "src/network/game_server_packet_handler.h"
#include "src/utility/components/collider/collider.h"


MainApp::MainApp()
{
	std::wcout.imbue(std::locale("kor"));
}

auto MainApp::NativeConstruct() -> HRESULT
{
	GameLogicManager::InitDevice(g_hInst, g_Wnd, kSceneEnd);

	SoundManager::GetInstance().Initialize();
	if (FAILED(GraphicDevice::GetInstance().ReadyGraphicDevice(g_Wnd, GraphicDevice::kWindowMode::kModeWin, g_WinCX, g_WinCY, &_graphic_device)))
		return E_FAIL;
	
	if (FAILED(AddPrototypeComponent()))
		return E_FAIL;

	if (FAILED(AddPrototypeGameObject()))
		return E_FAIL;

	g_service = ClientNetworkThreadInit(L"127.0.0.1", 7777, Service::kServerType::kClientLogin, 1, 2);
	DataReaderManager::GetInstance().Init(GraphicDevice::GetInstance().GetDevice());
	SceneManager::GetInstance().SetUpScene(SceneLogo::Create(_graphic_device));
	return S_OK;
}

auto MainApp::Tick(const double timeDelta) -> int32_t
{
	m_TimeAcc += timeDelta;

	constexpr int32_t workerTick = 64;

	GameLogicManager::Tick(timeDelta);

	Picking::GetInstance().ComputeMouseCursorPosInWorld(g_Wnd);

	GameLogicManager::LateTick(timeDelta);
	GameLogicManager::DeadRemove();
	LEndTickCount = GetTickCount64() + workerTick;
	ThreadManager::DoGlobalQueueWork();
	return int32_t();
}

auto MainApp::RenderMainApp() -> HRESULT
{
	auto& device = GraphicDevice::GetInstance();
	auto& renderer = Renderer::GetInstance();
	const auto& sceneManager = SceneManager::GetInstance();
	device.RenderBegin();

	renderer.RenderGameObject();

	if (FAILED(sceneManager.Render()))
		return E_FAIL;
	
	device.RenderEnd();
	++m_dwNumRender;

	if (m_TimeAcc > 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_dwNumRender);
		m_TimeAcc = 0.0;
		m_dwNumRender = 0;
		SetWindowText(g_Wnd, m_szFPS);
	}
	return S_OK;
}

auto MainApp::ClientNetworkThreadInit(const std::wstring& ip, const int16_t port, Service::kServerType type, int32_t sessionCount, const int32_t threadCount) -> ClientServiceRef
{
	NetworkThreadStopForWait();
	_exit = false;
	SocketUtils::Init();
	switch(type)
	{
	case Service::kServerType::kClientLogin:
		LoginServerPacketHandler::Init();
		break;
	case Service::kServerType::kClientGame:
		GameServerPacketHandler::Init();
		break;
	default: 
		break;
	}

	g_service = MakeShared<ClientService>(
		NetAddress(ip, port),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager µî
		type,
		sessionCount);

	ASSERT_CRASH(g_service->Start());

	for (int32_t i = 0; i < threadCount; i++)
	{
		ThreadManager::GetInstance().Launch([&]()
			{
				_iocp_thread_running_count.fetch_add(1);
				while (!_exit)
				{
					g_service->GetIocpCore()->Dispatch(10);

					SendManager::GetInstance().Execute();

				}
				_iocp_thread_running_count.fetch_sub(1);
			});
	}

	WaitConnectServer();
	return g_service;
}

auto MainApp::WaitConnectServer() -> void
{
	constexpr auto maxWaitTime = 5000;

	const auto startTime = GetTickCount64() + maxWaitTime;
	while (!this->IsConnected())
	{
		if (startTime < GetTickCount64())
		{
			_exit = true;
			exit(0);
		}
		std::this_thread::sleep_for(10ms);
	}
}

auto MainApp::NetworkThreadStopForWait() -> void
{
	_exit = true;
	constexpr auto maxWaitTime = 5000;

	const auto startTime = GetTickCount64() + maxWaitTime;
	while (_iocp_thread_running_count)
	{
		if (startTime < GetTickCount64())
		{
			break;
		}
		std::this_thread::sleep_for(10ms);
	}
	if (g_service)
	{
		SocketUtils::Clear();
		g_service->CloseService();
	}
}

auto MainApp::IsConnected() const -> bool
{
	return _is_connected;
}

auto MainApp::SetConnected(const bool connected) -> void
{
	_is_connected.store(connected);
	if (!_is_connected)
	{
		_exit = true;
	}
}

auto MainApp::SetAuthInfo(Protocol::LoginServerCharacterSelect info)->void
{
	_auth_info = info;
}

auto MainApp::GetAuthInfo() const -> Protocol::LoginServerCharacterSelect
{
	return _auth_info;
}

HRESULT MainApp::AddPrototypeGameObject()
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_BackGround"), BackGround::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Camera_Free"), CameraFree::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Ingame_Camera"), IngameCamera::Create(_graphic_device))))
		return E_FAIL;
	
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Login_Textbox"), TextBoxUi::Create())))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_User"), User::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Loading_Ui"), LoadingUi::Create())))
		return E_FAIL;
	return S_OK; 
}

HRESULT MainApp::AddPrototypeComponent()
{
	const auto& componentManager = ComponentManager::GetInstance();

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Transform"), Transform::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_VIBuffer_Rect"), ViBufferRect::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Shader_Rect"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/shader_rect.hlsl")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Shader_Ui"), Shader::Create(_graphic_device, TEXT("../../Binary/ShaderFiles/shader_ui.hlsl")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Login_Logo"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/bg_christmas_login_i3.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Default2"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Default2.jpg")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Collider_AABB"), Collider::Create(_graphic_device, Collider::kTypeAabb))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Collider_OBB"), Collider::Create(_graphic_device, Collider::kTypeObb))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Collider_NoTarget_OBB"), Collider::Create(_graphic_device, Collider::kNoTargetObb))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Collider_NoTarget_AABB"), Collider::Create(_graphic_device, Collider::kNoTargetAabb))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Loading_Ui_Side_Bar"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/loading_ui/ui_loading_sidebar_%d.png"), 2))))
	{
		return E_FAIL;
	}


	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Loading_henesys_02"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/loading_ui/henesys_02.dds")))))
	{
		return E_FAIL;
	}

	return S_OK;
}

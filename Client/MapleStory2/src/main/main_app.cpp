#include "c_pch.h"
#include "main_app.h"

#include <iostream>

#include "data_reader/data_reader_manager.h"
#include "src/game_object/back_ground/back_ground.h"
#include "src/game_object/camera/camera_free.h"
#include "src/game_object/ui/login/text_box_ui.h"
#include "src/network/send_manager.h"
#include "src/network/server_packet_handler.h"
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

std::atomic<bool> MainApp::_exit = false;

MainApp::MainApp()
{
	std::wcout.imbue(std::locale("kor"));
}

auto MainApp::NativeConstruct() -> HRESULT
{
	GameLogicManager::InitDevice(g_hInst, g_Wnd, static_cast<int32_t>(kSceneEnd));


	if (FAILED(GraphicDevice::GetInstance().ReadyGraphicDevice(g_Wnd, GraphicDevice::kWindowMode::kModeWin, g_WinCX, g_WinCY, &_graphic_device)))
		return E_FAIL;
	
	if (FAILED(AddPrototypeComponent()))
		return E_FAIL;

	if (FAILED(AddPrototypeGameObject()))
		return E_FAIL;

	g_service = NetworkThreadInit();
	DataReaderManager::GetInstance().Init(GraphicDevice::GetInstance().GetDevice());
	SceneManager::GetInstance().SetUpScene(SceneLogo::Create(_graphic_device));
	return S_OK;
}

auto MainApp::Tick(const double timeDelta) -> int32_t
{
	constexpr int32_t workerTick = 64;

	GameLogicManager::Tick(timeDelta);

	Picking::GetInstance().ComputeMouseCursorPosInWorld(g_Wnd);

	GameLogicManager::LateTick(timeDelta);

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
	return S_OK;
}

auto MainApp::NetworkThreadInit() -> ClientServiceRef
{
	SocketUtils::Init();
	ServerPacketHandler::Init();
	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager µî
		1);

	ASSERT_CRASH(service->Start());

	for (int32_t i = 0; i < 2; i++)
	{
		ThreadManager::GetInstance().Launch([=]()
			{
				while (!_exit)
				{
					service->GetIocpCore()->Dispatch(10);

					SendManager::GetInstance().Execute();

				}

			});
	}

	WaitConnectServer();
	return service;
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

HRESULT MainApp::AddPrototypeGameObject()
{
	auto& objectManager = ObjectManager::GetInstance();
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_BackGround"), BackGround::Create(_graphic_device))))
		return E_FAIL;

	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Camera_Free"), CameraFree::Create(_graphic_device))))
		return E_FAIL;
	
	if (FAILED(objectManager.AddPrototype(TEXT("Prototype_Login_Textbox"), TextBoxUi::Create())))
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

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Login_Logo"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Ui/bg_christmas_login_i3.png")))))
		return E_FAIL;

	if (FAILED(componentManager.AddPrototype(kScene::kSceneStatic, TEXT("Prototype_Texture_Default2"), Texture::Create(_graphic_device, Texture::kType::kTypeGeneral, TEXT("../../Binary/Resources/Textures/Default2.jpg")))))
		return E_FAIL;

	return S_OK;
}

#pragma once
#include <cstdint>

#ifdef _DEBUG
#pragma comment(lib, "ClientCore\\Debug\\ClientCore.lib")
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
//#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

enum class kScene
{
	kSceneStatic,
	kSceneLogo,
	kSceneLoading,
	kSceneGamePlay0,
	kSceneGamePlay1,
	kSceneEnd
};

constexpr int16_t		g_WinCX = 1280;
constexpr int16_t		g_WinCY = 720;
extern HWND				g_Wnd;
extern HINSTANCE		g_hInst;


#include "engine_pch.h"
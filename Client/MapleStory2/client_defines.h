#pragma once


enum kScene
{
	kSceneStatic,
	kSceneLogo,
	kSceneLoading,
	kSceneCharacterSelect,
	kSceneGameInit,
	kSceneGamePlay0,
	kSceneGamePlay1,
	kSceneEnd
};

enum class kEqpType
{
	kPa,
	kCl,
	kFace,
	kSh
};

constexpr int16_t		g_WinCX = 1280;
constexpr int16_t		g_WinCY = 720;
extern HWND				g_Wnd;
extern HINSTANCE		g_hInst;
extern HWND				g_hEdit;
extern ClientServiceRef g_service;
extern class MainApp*	g_mainApp;
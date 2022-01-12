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

enum class kAnimationType
{
	kIdle,
	kRun,
	kJumpUp,
	kJumpDown,
	kJumpFall,
	kAttackIdle,
	kBreakingSkull1A,
	kBreakingSkull2A,
	kBreakingSkull3A,
};


constexpr int16_t		g_WinCX = 1280;
constexpr int16_t		g_WinCY = 720;
extern HWND				g_Wnd;
extern HINSTANCE		g_hInst;
extern HWND				g_hEdit;
extern ClientServiceRef g_service;
extern class MainApp*	g_mainApp;
extern bool g_isWindowsActive;
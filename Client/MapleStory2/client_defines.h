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
	kLargeSwordAttack1A,
	kLargeSwordAttack2A,
	kLargeSwordAttack3A,
	kLargeSwordAttackIdle,
	kLargeSwordRun,
	kLargeSwordJumpUp,
	kLargeSwordJumpDown,
	kBerserkerRageslash1A,
	kBerserkerRageslash2A,
	kBerserkerRageslash3A,
	kBerserketSweepAttack1A,
	kBerserketSweepAttack2A,
	kBerserkerVolcanicslash1A,
	kBerserkerVolcanicslash2A,
};


constexpr int16_t		g_WinCX = 1280;
constexpr int16_t		g_WinCY = 720;
extern HWND				g_Wnd;
extern HINSTANCE		g_hInst;
extern HWND				g_hEdit;
extern ClientServiceRef g_service;
extern class MainApp*	g_mainApp;
extern bool g_isWindowsActive;
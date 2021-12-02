#pragma once

enum class kScene
{
	kSceneStatic,
	kSceneLogo,
	kSceneLoading,
	kSceneGamePlay0,
	kSceneGamePlay1,
	kSceneEnd
};
constexpr int16_t		g_WinCX = 1024;
constexpr int16_t		g_WinCY = 768;
extern HWND				g_Wnd;
extern HINSTANCE		g_hInst;
extern HWND				g_hEdit;
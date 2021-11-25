#pragma once
class GameLogicManager
{
public:
	static HRESULT InitDevice(HINSTANCE hInst, HWND hWnd, int32_t numLevel);

	static auto Clear(uint32_t levelIndex)->void;

	static auto Tick(double timeDelta)->int32_t;
	static auto LateTick(double timeDelta)->int32_t;
};


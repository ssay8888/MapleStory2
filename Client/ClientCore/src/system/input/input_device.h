#pragma once
class InputDevice
{
public:
	enum class kDirectInMouseButton 
	{
		kLeftButton,
		kRightButton,
		kWheelButton,
		kXButton
	};
	enum class kDirectInMouseState
	{
		kX,
		kY,
		kWheel
	};

	static auto GetInstance()->InputDevice&
	{
		static InputDevice instance;
		return instance;
	}

public:
	auto InitDevice(HINSTANCE hInst, HWND hWnd)->HRESULT;
	auto InvalidateInputDevice()->int32_t;

public:
	auto GetDirectKeyState(const int8_t key)->int8_t;
	auto GetDirectMouseKeyState(const kDirectInMouseButton mouseKey)->uint8_t;
	auto GetDirectMouseMoveState(const kDirectInMouseState mouseMove)->int32_t;

private:
	ComPtr<IDirectInput8W>			_input_sdk = nullptr;
	ComPtr<IDirectInputDevice8W>	_keyboard = nullptr;
	ComPtr<IDirectInputDevice8W>	_mouse = nullptr;

private:
	DIMOUSESTATE			_mouse_state = {};
	int8_t					_key_state[256] = {0, };
};


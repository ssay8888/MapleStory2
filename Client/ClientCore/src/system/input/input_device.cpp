#include "pch.h"
#include "input_device.h"

auto InputDevice::InitDevice(const HINSTANCE hInst, const HWND hWnd) -> HRESULT
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, static_cast<void**>(&_input_sdk), nullptr)))
		return E_FAIL;

	if (FAILED(_input_sdk->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr)))
		return E_FAIL;

	_keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	_keyboard->SetDataFormat(&c_dfDIKeyboard);
	_keyboard->Acquire();

	if (FAILED(_input_sdk->CreateDevice(GUID_SysMouse, &_mouse, nullptr)))
		return E_FAIL;

	_mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	_mouse->SetDataFormat(&c_dfDIMouse);
	_mouse->Acquire();
	
	return S_OK;
}

auto InputDevice::InvalidateInputDevice() -> int32_t
{
	_keyboard->GetDeviceState(256, _key_state);
	_mouse->GetDeviceState(sizeof(_mouse_state), &_mouse_state);

	return int32_t();
}

auto InputDevice::GetDirectKeyState(const int8_t key) -> int8_t
{
	return _key_state[key];
}

auto InputDevice::GetDirectMouseKeyState(const kDirectInMouseButton mouseKey) -> uint8_t
{
	return _mouse_state.rgbButtons[static_cast<int8_t>(mouseKey)];
}

auto InputDevice::GetDirectMouseMoveState(const kDirectInMouseState mouseMove) -> int64_t
{
	return reinterpret_cast<int64_t*>(&_mouse_state)[static_cast<int8_t>(mouseMove)];
}

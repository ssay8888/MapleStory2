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

auto InputDevice::GetKeyUp(const uint8_t key) -> uint8_t
{
	if (_key_state[key])
	{
		_key_state_up[key] |= key;
		return false;
	}
	else if (_key_state_up[key])
	{
		_key_state_up[key] ^= key;
		return true;
	}
	return false;
}

auto InputDevice::GetKeyDown(const uint8_t key) -> uint8_t
{
	if (_key_state[key] && !_key_state_down[key])
	{
		_key_state_down[key] |= key;
		return true;
	}
	else if (!_key_state[key] && _key_state_down[key])
	{
		_key_state_down[key] ^= key;
		return false;
	}
	return false;
}

auto InputDevice::GetKeyPressing(const uint8_t key) -> uint8_t
{
	return _key_state[key];
}

auto InputDevice::GetDirectMouseKeyPressing(const kDirectInMouseButton mouseKey) -> uint8_t
{
	return _mouse_state.rgbButtons[static_cast<int8_t>(mouseKey)];
}

auto InputDevice::GetDirectMouseKeyDown(const kDirectInMouseButton mouseKey) -> uint8_t
{
	auto key = static_cast<int8_t>(mouseKey);
	if (_mouse_state.rgbButtons[key] && !_mouse_state_down.rgbButtons[key])
	{
		_mouse_state_down.rgbButtons[key] |= 1;
		return true;
	}
	else if (!_mouse_state.rgbButtons[key] && _mouse_state_down.rgbButtons[key])
	{
		_mouse_state_down.rgbButtons[key] ^= 1;
		return false;
	}
	return false;
}

auto InputDevice::GetDirectMouseKeyUp(const kDirectInMouseButton mouseKey) -> uint8_t
{
	auto key = static_cast<int8_t>(mouseKey);
	if (_mouse_state.rgbButtons[key])
	{
		_mouse_state_up.rgbButtons[key] |= 1;
		return false;
	}
	else if (_mouse_state_up.rgbButtons[key])
	{
		_mouse_state_up.rgbButtons[key] ^= 1;
		return true;
	}
	return false;
}

auto InputDevice::GetDirectMouseMoveState(const kDirectInMouseState mouseMove) -> int32_t
{
	return reinterpret_cast<int32_t*>(&_mouse_state)[static_cast<int8_t>(mouseMove)];
}

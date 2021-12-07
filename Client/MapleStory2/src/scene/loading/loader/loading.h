#pragma once

class Loading final : public std::enable_shared_from_this<Loading>
{
public:
	explicit Loading(const ComPtr<IDirect3DDevice9>& device);
	virtual ~Loading() = default;

public:
	auto GetSystemMessage() const->std::wstring;
	auto IsFinished() const->bool ;

public:
	auto NativeConstruct(kScene scene)->HRESULT;
	static auto Create(const ComPtr<IDirect3DDevice9>& device, kScene nextLevel)->std::shared_ptr<Loading>;

private:
	auto __stdcall ThreadMain()->HRESULT;
	auto ReadyCharacterSelect()->HRESULT;
	auto LoadCharacterSelectUi()->HRESULT;


	auto ReadyGamePlay0()->HRESULT;
	auto ReadyGamePlay1()->HRESULT;

private:
	bool						_is_finish = false;
	kScene						_next_level = kSceneEnd;
	std::wstring				_system_message;
	ComPtr<IDirect3DDevice9>	_graphic_device;
	//std::thread					_thread;

	USE_LOCK;
};


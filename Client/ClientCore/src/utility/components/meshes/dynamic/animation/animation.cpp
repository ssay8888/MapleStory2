#include "pch.h"
#include "animation.h"

#include <iostream>

Animation::Animation(const ComPtr<ID3DXAnimationController>& animationController) :
	_animation_controller(animationController)
{
}

Animation::Animation(const Animation& rhs)
{
	constexpr uint32_t maxAnimNum = 100;
	rhs._animation_controller->CloneAnimationController(rhs._animation_controller->GetMaxNumAnimationOutputs(),
		maxAnimNum,
		rhs._animation_controller->GetMaxNumTracks(),
		rhs._animation_controller->GetMaxNumEvents(),
		&_animation_controller);
}

Animation::~Animation()
{
}

auto Animation::GetAnimationController() const -> ComPtr<ID3DXAnimationController>
{
	return _animation_controller;
}

auto Animation::ResetTimeAcc()
{
	_time_acc = 0;
}

auto Animation::NativeConstructPrototype() -> HRESULT
{
	return S_OK;
}

auto Animation::NativeConstruct() -> HRESULT
{
	return S_OK;
}

auto Animation::SetAnimationIndex(const uint32_t animIndex) -> HRESULT
{
	//if (_current_animation_index == animIndex)
	//	return S_OK;

	//_animation_controller->GetAnimationSetByName()
	LPD3DXANIMATIONSET		pAS = nullptr;
	if (FAILED(_animation_controller->GetAnimationSet(animIndex, &pAS)))
		return E_FAIL;
	_new_track = _current_track == 0 ? 1 : 0;

	/* Ʈ������ �ִϸ��̼� ���� �÷����´�. */
	_animation_controller->SetTrackAnimationSet(_new_track, pAS);
	//_animation_controller->SetTrackAnimationSet(0, pAS2);
	//_animation_controller->SetTrackSpeed(animIndex, 0.1f);

	/* �ִϸ��̼� �ϳ��� ����ϴ� ���ӽð�. */
	auto a =  pAS->GetPeriod();
	//D3DXTRACK_DESC			trackDesc;
	//_animation_controller->GetTrackDesc(_current_track, &trackDesc);

	pAS->Release();
	//pAS2->Release();
	_animation_controller->UnkeyAllTrackEvents(_current_track);
	_animation_controller->UnkeyAllTrackEvents(_new_track);

	_animation_controller->KeyTrackEnable(_current_track, FALSE, _time_acc + 0.2);
	_animation_controller->KeyTrackWeight(_current_track, 0.1f, _time_acc, 0.2, D3DXTRANSITION_LINEAR);
	_animation_controller->KeyTrackSpeed(_current_track, _ani_speed, _time_acc, 0.2, D3DXTRANSITION_LINEAR);

	_animation_controller->SetTrackEnable(_new_track, TRUE);
	_animation_controller->KeyTrackWeight(_new_track, 0.9f, _time_acc, 0.2, D3DXTRANSITION_LINEAR);
	_animation_controller->KeyTrackSpeed(_new_track, _ani_speed, _time_acc, 0.2, D3DXTRANSITION_LINEAR);

	_animation_controller->SetTrackPosition(_new_track, 0.0);
	_animation_controller->SetTrackSpeed(_new_track, _ani_speed);

	_animation_controller->ResetTime();
	_time_acc = 0.0;

	_current_track = _new_track;

	_current_animation_index = animIndex;
	return S_OK;
}

auto Animation::ResetAnimation() -> HRESULT
{
	LPD3DXANIMATIONSET		pAS = nullptr;
	if (FAILED(_animation_controller->GetAnimationSet(0, &pAS)))
		return E_FAIL;

	_animation_controller->SetTrackEnable(0, TRUE);
	_animation_controller->SetTrackAnimationSet(0, pAS);
	_animation_controller->SetTrackPosition(0, 0.0);
	_animation_controller->SetTrackSpeed(0, _ani_speed);
	pAS->Release();
	_animation_controller->ResetTime();
	_time_acc = 0.0;
	return S_OK;
}

auto Animation::PlayAnimation(double timeDelta) -> HRESULT
{
	if (nullptr == _animation_controller)
		return E_FAIL;


	_time_acc += timeDelta;
	_animation_controller->AdvanceTime(timeDelta, nullptr);
	return S_OK;
}

auto Animation::BindFrames(D3DxFrameDerived* pRootFrame) const -> HRESULT
{
	if (FAILED(D3DXFrameRegisterNamedMatrices(pRootFrame, _animation_controller.Get())))
		return E_FAIL;

	return S_OK;
}

auto Animation::GetTimeAcc() -> double
{
	return _time_acc;
}

std::shared_ptr<Animation> Animation::Create(const ComPtr<ID3DXAnimationController>& animationController)
{
	std::shared_ptr<Animation> pInstance = std::make_shared<Animation>(animationController);

	if (FAILED(pInstance->NativeConstructPrototype()))
	{
		MSGBOX("Failed to Creating Animation");
	}
	return pInstance;
}

std::shared_ptr<Animation> Animation::Clone()
{
	std::shared_ptr<Animation> pInstance = std::make_shared<Animation>(*this);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Clone Animation");
	}
	return pInstance;
}

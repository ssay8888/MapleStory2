#pragma once
class Animation
{
public:
	Animation(const ComPtr<ID3DXAnimationController>& animationController);
	Animation(const Animation& rhs);
	~Animation();

public:
	auto GetAnimationController() const->ComPtr<ID3DXAnimationController>;
	auto ResetTimeAcc();
public:
	auto NativeConstructPrototype()->HRESULT;
	auto NativeConstruct()->HRESULT;
	auto SetAnimationIndex(uint32_t animIndex)->HRESULT;
	auto ResetAnimation()->HRESULT;
	auto PlayAnimation(double timeDelta)->HRESULT;
	auto BindFrames(D3DxFrameDerived* pRootFrame) const ->HRESULT;

	auto GetTimeAcc()->double;

private:
	ComPtr<ID3DXAnimationController>	_animation_controller = nullptr;
	uint32_t							_current_animation_index = -1;
	uint32_t							_current_track = 0;
	uint32_t							_new_track = 1;
	double								_time_acc = 0.0;

	const float							_ani_speed = 1.f;

public:
	static std::shared_ptr<Animation> Create(const ComPtr<ID3DXAnimationController>& animationController);
	std::shared_ptr<Animation> Clone();
};


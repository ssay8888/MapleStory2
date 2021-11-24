#pragma once
class Scene;

class SceneManager
{
private:
	SceneManager() = default;
	~SceneManager() = default;

public:
	static auto GetInstance() -> SceneManager&
	{
		static SceneManager instance;
		return instance;
	}

	auto SetUpScene(const std::shared_ptr<Scene>& level)->HRESULT;
	auto Tick(double timeDelta) const ->int32_t;
	auto Render() const ->HRESULT;

private:
	std::shared_ptr<Scene> _current_scene = nullptr;
};


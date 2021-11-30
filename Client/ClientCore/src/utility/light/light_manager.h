#pragma once
class Light;

class LightManager
{
	LightManager() = default;
	~LightManager() = default;
public:
	static auto GetInstance()->LightManager&
	{
		static LightManager instance;
		return instance;
	}

public:
	auto GetLightDesc(int32_t index = 0)->D3DLIGHT9;
	auto AddLight(D3DLIGHT9 lightDesc)->HRESULT;

private:
	std::vector<std::shared_ptr<Light>> _lights;
	typedef std::list<std::shared_ptr<Light>> Lights;
};


#pragma once
#include "src/common/xml/map_parser.h"
#include "src/utility/game_objects/game_object.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

class MapObject;
class Shader;

class MapInstance final : public GameObject
{
public:
	explicit MapInstance(const ComPtr<IDirect3DDevice9>& device);
	virtual ~MapInstance() = default;


	auto NativeConstructPrototype() -> HRESULT override;
	auto NativeConstruct(void* arg) -> HRESULT override;
	auto Tick(double timeDelta) -> int32_t override;
	auto LateTick(double timeDelta) -> int32_t override;
	auto Render() -> HRESULT override;
	auto Clone(void* arg) -> std::shared_ptr<GameObject> override;

public:
	auto AddMapObject(MapParser::MapEntity modelName)->bool;


	auto SetUpConstantTable(std::shared_ptr<Transform> transformCom) const->HRESULT;

private:
	std::shared_ptr<Shader>			 _shader_com = nullptr;
	std::vector<std::shared_ptr<MapObject>>			 _map_objects;
};


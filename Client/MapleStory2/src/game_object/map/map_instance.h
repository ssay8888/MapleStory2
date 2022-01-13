#pragma once
#include "src/common/xml/map_parser.h"
#include "src/utility/game_objects/game_object.h"
#include "src/utility/light/light_manager.h"
#include "src/utility/pipe_line/pipe_line.h"

class Monster;
class Collider;
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

	auto AddMonster(int64_t objectId, std::shared_ptr<Monster> monster)->void;
	auto RemoveMonster(int64_t objectId)->void;
	auto FindMonster(int64_t objectId)->std::shared_ptr<Monster>;
	auto CollisionMonsters(std::shared_ptr<Collider> collider)->std::vector<std::shared_ptr<Monster>>;

public:
	auto FindRangeCellObject(const std::shared_ptr<Collider>& targetCollider)->std::vector<std::shared_ptr<MapObject>>;

private:
	std::shared_ptr<Shader>			 _shader_com = nullptr;
	std::vector<std::shared_ptr<MapObject>>			 _map_objects;

	std::map<int64_t, std::shared_ptr<Monster>>			_monsters;
};


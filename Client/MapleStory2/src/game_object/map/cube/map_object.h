#pragma once
#include "src/common/xml/map_parser.h"
#include "src/utility/game_objects/game_object.h"

class Shader;
class MeshStatic;
class Transform;
class MapObject : public GameObject
{
public:
	MapObject();
	virtual ~MapObject() = default;

public:
	auto NativeConstructPrototype()->HRESULT override;
	auto NativeConstruct(void* arg)->HRESULT override;
	auto Tick(double timeDelta)->int32_t override;
	auto LateTick(double timeDelta)->int32_t override;
	auto Render()->HRESULT override;
	auto Render(const std::shared_ptr<Shader>& shaderCom)->HRESULT;
	auto Clone(void* arg)->std::shared_ptr<GameObject> override;
	static auto Create(void* arg)->std::shared_ptr<MapObject>;
	auto GetTransform()->std::shared_ptr<Transform>&;

private:
	auto AddComponents(MapParser::MapEntity& entity)->HRESULT;
private:
	std::shared_ptr<Transform>		_transform_com = nullptr;
	std::shared_ptr<MeshStatic>		_mesh_com = nullptr;

};


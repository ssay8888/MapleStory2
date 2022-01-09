 #pragma once
#include <d3d9.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

class ItemModel;

class DataReaderManager
{
private:
	DataReaderManager() = default;
	~DataReaderManager() = default;

public:
	static auto GetInstance()->DataReaderManager&
	{
		static DataReaderManager instance;
		return instance;
	}

	auto Init(Microsoft::WRL::ComPtr<IDirect3DDevice9> device)->void;

public:
#pragma region ItemInfoMethod
	auto LoadItemModel(const std::string& path)->void;
	auto FindItemModel(const int32_t itemId)->std::shared_ptr<ItemModel>;
	auto AllItemModel() const ->const std::map<int32_t, std::shared_ptr<ItemModel>>&;
#pragma endregion

#pragma region AnimationLoader

	struct AnimationInfo
	{
		std::string animation_name;
		std::string target_node;
	};
	auto LoadAnimationInfo()->void;
	auto FindAnimationInfo(const std::string& name)->std::shared_ptr<AnimationInfo>;
	auto AllAnimationName() const ->std::vector<std::shared_ptr<AnimationInfo>>;

#pragma endregion


#pragma region FaceLoader
	struct FaceInfo
	{
		int32_t														item_id;
		std::vector<std::wstring>									files;
		Microsoft::WRL::ComPtr<IDirect3DTexture9>					icon_diffuse_map;
		std::vector<Microsoft::WRL::ComPtr<IDirect3DTexture9>>		diffuse_map;
	};
	auto FaceLoader(Microsoft::WRL::ComPtr<IDirect3DDevice9> device)->void;
	auto FindFace(int32_t faceId)->std::shared_ptr<FaceInfo>;

#pragma endregion


#pragma region CreateCharacterItemInfo
	struct Item
	{
		int32_t type;
		int32_t gender;
		int32_t item_id;
	};
	struct CreateCharacterItemInfo
	{
		std::wstring						title_name;
		std::vector<std::shared_ptr<Item>>	items;
	};

	auto LoadCreateCharacterItemInfo()->void;
	auto GetCreateItemInfo()const -> std::vector<std::shared_ptr<CreateCharacterItemInfo>>;
#pragma endregion


#pragma region FieldDataReader
	struct FieldData
	{
		int32_t id;
		struct Environment
		{
			std::wstring feature;
			std::wstring name;
			struct Bgm
			{
				int32_t id;
				int32_t param;
			};
			Bgm bgm;
		};
		Environment environment;

	};

	auto LoadFieldData()->void;
	auto AllFieldData() const ->std::map<int32_t, std::shared_ptr<FieldData>>;
#pragma endregion


#pragma region MonsterInfo
	struct ModelAtt
	{
		std::wstring kfm;
		float scale;
		int32_t walk_speed;
		int32_t run_speed;
	};
	struct StatAtt
	{
		int32_t hp;
		int32_t asp;
		int32_t msp;
		int32_t atp;
		int32_t evp;
		int32_t cap;
		int32_t cad;
		int32_t car;
		int32_t ndd;
		int32_t pap;
		int32_t map;
		int32_t par;
		int32_t mar;
	};
	struct Distance
	{
		int32_t sight;
		int32_t sight_height_up;
		int32_t sight_height_down;
	};
	struct Collision
	{
		float width;
		float height;
		float depth;
	};
	struct MonsterInfo
	{
		int32_t id;
		ModelAtt model;
		StatAtt stat;
		Distance distance;
		Collision collision;
	};
	auto LoadMonsterInfo()->void;
	auto AllMonsterInfo()->std::vector<std::shared_ptr<MonsterInfo>>;
	auto FindMonsterInfo(int32_t id)->std::shared_ptr<MonsterInfo>;
	
	struct Seq
	{
		int id;
		std::wstring name;
		std::map<std::wstring, float> key;
	};
	struct Kfm
	{
		std::wstring name;
		std::unordered_map<int32_t, std::shared_ptr<Seq>> seqs;
	};

	auto LoadAniKeyText()->void;
	auto FindAnyKey(int32_t npcId)->std::shared_ptr<Kfm>;
#pragma endregion
	
private:
	std::map<int32_t, std::shared_ptr<FaceInfo>>			_face_info;
	std::map<int32_t, std::shared_ptr<ItemModel>>			_item_model;
	std::vector<std::shared_ptr<AnimationInfo>>				_animations_info;
	std::vector<std::shared_ptr<CreateCharacterItemInfo>>	_create_item_info;
	std::map<int32_t, std::shared_ptr<FieldData>>			_field_data;
	std::map<int32_t, std::shared_ptr<MonsterInfo>>			_monster_info;
	std::unordered_map<int32_t, std::shared_ptr<Kfm>>		_ani_key;
};


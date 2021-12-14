#pragma once
#include <d3d9.h>
#include <map>
#include <memory>
#include <string>
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
	auto AllAnimationName()->std::vector<std::shared_ptr<AnimationInfo>>;

#pragma endregion


#pragma region FaceLoader
	struct FaceInfo
	{
		int32_t											item_id;
		std::vector<std::wstring>						files;
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
		std::vector<std::shared_ptr<Item>> items;
	};

	auto LoadCreateCharacterItemInfo()->void;
	auto GetCreateItemInfo()const -> std::vector<std::shared_ptr<CreateCharacterItemInfo>>;
#pragma endregion


private:
	std::map<int32_t, std::shared_ptr<FaceInfo>>			_face_info;
	std::map<int32_t, std::shared_ptr<ItemModel>>			_item_model;
	std::map<std::string, std::shared_ptr<AnimationInfo>>	_animations_info;
	std::vector<std::shared_ptr<CreateCharacterItemInfo>>	_create_item_info;
};


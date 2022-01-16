 #pragma once
#include <d3d9.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

 namespace pugi
 {
	 class xpath_node;
	 class xml_node;
 }

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

	struct ItemInfo
	{
		int32_t id;
		struct Property
		{
			int32_t slot_max;
			int32_t icon_code;
			std::wstring slot_icon;
			std::wstring category;
		} property;

		int32_t skill_id;

		int32_t constant_id;

	};
	auto LoadItemInfo(std::string path)->void;
	auto AllItemInfo() const ->std::map<int32_t, std::shared_ptr<ItemInfo>>;
	auto FindItemInfo(int32_t itemId)->std::shared_ptr<ItemInfo>;

	struct Option
	{
		int32_t code;
		struct Rank
		{
			std::wstring name;
			int32_t value;
			int32_t lua_index;
		};

		std::map<std::wstring, std::shared_ptr<Rank>> ranks;
	};

	auto LoadItemOptionConstant()->void;
	auto FindItemOptionConstant(int32_t optionId)->std::shared_ptr<Option>;
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
		float walk_speed;
		float run_speed;
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
		float sight;
		float sight_height_up;
		float sight_height_down;
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
		std::vector<std::pair<int32_t, int32_t>> skills;
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
	auto LoadAniKeyPlayerText()->void;
	auto FindAniKey(int32_t npcId)->std::shared_ptr<Kfm>;
	auto FindPlayerAniKey(int32_t gender)->std::shared_ptr<Kfm>;
#pragma endregion

#pragma region SkillData
	struct DetectProperty
	{
		float distance;
	};
	struct Motion
	{
		std::wstring sequence_name;
		std::wstring motion_effect;
		std::wstring str_tag_effects;
		float movedistance;
		struct Attack
		{
			int32_t target_count;
			struct Range
			{
				float distance;
				float height;
				float width;
			} range;
			struct Damage
			{
				int32_t count;
				float rate;
			} damage;
		} attack;
	};
	struct Skill
	{
		int32_t id;
		std::string comment;
		DetectProperty detect;
		int32_t cooldown;
		std::vector<std::shared_ptr<Motion>> motions;
	};


	auto LoadSkillData()->void;
	auto FindSkillData(int32_t skillId)->std::shared_ptr<Skill>;

	enum class kEffectType
	{
		kNif,
		kSound,
		kEnd
	};
	struct Effect
	{
		kEffectType type;

		//sound타입
		std::wstring group;
		std::wstring event;

		//nif타입
		std::wstring file_path;


		float start_delay;
		float duration;

	};
	auto LoadMotionEffect()->void;
	auto LoadTagEffect()->void;
	auto CreateEffectNode(pugi::xpath_node effectNode)->std::shared_ptr<Effect>;

	auto FindMotionEffect(std::shared_ptr<Motion> key)->std::shared_ptr<Effect>;
	auto FindStrTagEffect(std::shared_ptr<Motion> key)->std::vector<std::shared_ptr<Effect>>;
#pragma endregion
	
private:
	std::map<int32_t, std::shared_ptr<FaceInfo>>								_face_info;
	std::map<int32_t, std::shared_ptr<ItemModel>>								_item_model;
	std::map<int32_t, std::shared_ptr<ItemInfo>>								_item_info;
	std::map<int32_t, std::shared_ptr<Option>>									_item_options;
	std::vector<std::shared_ptr<AnimationInfo>>									_animations_info;
	std::vector<std::shared_ptr<CreateCharacterItemInfo>>						_create_item_info;
	std::map<int32_t, std::shared_ptr<FieldData>>								_field_data;
	std::map<int32_t, std::shared_ptr<MonsterInfo>>								_monster_info;
	std::unordered_map<int32_t, std::shared_ptr<Kfm>>							_ani_key;
	std::unordered_map<int32_t, std::shared_ptr<Kfm>>							_ani_key_player;
	std::map<int32_t, std::shared_ptr<Skill>>									_skills;
	std::map<std::shared_ptr<Motion>, std::shared_ptr<Effect>>					_motion_effects;
	std::map<std::shared_ptr<Motion>, std::vector<std::shared_ptr<Effect>>>	_str_tag_effects;
};


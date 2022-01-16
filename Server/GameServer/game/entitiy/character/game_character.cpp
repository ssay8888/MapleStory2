#include "game_server_pch.h"
#include "game_character.h"

#include "game/entitiy/item/game_item.h"
#include "game/map/map_instance.h"
#include "game/map/map_manager.h"
#include "information_collection/inventorys/inventorys.h"
#include "information_collection/stats/Statistic.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"
#include "src/utility/components/transform/transform.h"

GameCharacter::GameCharacter(const int64_t characterId):
	_character_id(characterId),
	_account_id(0),
	_gender(0),
	_face_id(0)
{
}

auto GameCharacter::Create(int64_t characterId) -> std::shared_ptr<GameCharacter>
{
	auto instance = MakeShared<GameCharacter>(characterId);
	if (FAILED(instance->NativeConstruct()))
	{
		return nullptr;
	}
	return instance;
}

auto GameCharacter::Tick(double timeDelta) -> void
{
}

auto GameCharacter::LateTick(double timeDelta) -> void
{
}

auto GameCharacter::GetCharacterId() const -> int64_t
{
	return _character_id;
}

auto GameCharacter::GetAccountId() const -> int32_t
{
	return _account_id;
}

auto GameCharacter::GetGender() const -> int32_t
{
	return _gender;
}

auto GameCharacter::GetFaceId() const -> int32_t
{
	return _face_id;
}

auto GameCharacter::GetName() const -> std::wstring
{
	return _name;
}

auto GameCharacter::GetMapId() const -> int32_t
{
	return _map_id;
}

auto GameCharacter::GetSpawnPoint() const -> int32_t
{
	return _spawn_point;
}

auto GameCharacter::GetTransform() const -> std::shared_ptr<Transform>
{
	return _transform;
}

auto GameCharacter::NativeConstruct() -> HRESULT
{
	_transform = Transform::Create(nullptr);
	_transform->NativeConstruct(nullptr);
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 26> bind(*con, L"{CALL dbo.spLoadCharacter(?)}");
		bind.BindParam(0, _character_id);

		WCHAR name[100]{0};
		int colIndex = 0;
		bind.BindCol(colIndex++, name);
		bind.BindCol(colIndex++, _account_id);
		bind.BindCol(colIndex++, _gender);
		bind.BindCol(colIndex++, _face_id);
		bind.BindCol(colIndex++, _map_id);
		bind.BindCol(colIndex++, _spawn_point);

		int32_t hp;
		bind.BindCol(colIndex++, hp);
		int32_t maxhp;
		bind.BindCol(colIndex++, maxhp);
		int32_t mp;
		bind.BindCol(colIndex++, mp);
		int32_t maxmp;
		bind.BindCol(colIndex++, maxmp);
		int32_t level;
		bind.BindCol(colIndex++, level);
		int32_t exp;
		bind.BindCol(colIndex++, exp);
		int32_t money;
		bind.BindCol(colIndex++, money);


		int32_t str;
		bind.BindCol(colIndex++, str);
		int32_t dex;
		bind.BindCol(colIndex++, dex);
		int32_t int_;
		bind.BindCol(colIndex++, int_);
		int32_t luk;
		bind.BindCol(colIndex++, luk);

		int32_t itemid;
		bind.BindCol(colIndex++, itemid);
		int32_t position;
		bind.BindCol(colIndex++, position);
		int32_t type;
		bind.BindCol(colIndex++, type);

		int32_t itemQuantity;
		bind.BindCol(colIndex++, itemQuantity);

		int32_t itemStr;
		bind.BindCol(colIndex++, itemStr);

		int32_t itemDex;
		bind.BindCol(colIndex++, itemDex);

		int32_t itemInt;
		bind.BindCol(colIndex++, itemInt);

		int32_t itemLuk;
		bind.BindCol(colIndex++, itemLuk);

		int32_t itemWap;
		bind.BindCol(colIndex++, itemWap);

		if (bind.Execute())
		{
			const std::shared_ptr<Statistic> stats = Statistic::Create(_character_id);
			const std::shared_ptr<Inventorys> inventory = Inventorys::Create(_character_id);
			do
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (bind.Fetch())
					{
						const auto item = MakeShared<GameItem>(itemid, position, static_cast<Protocol::kInventoryType>(type));
						item->SetQuantity(itemQuantity);
						item->SetStr(itemStr);
						item->SetDex(itemDex);
						item->SetInt(itemInt);
						item->SetLuk(itemLuk);
						item->SetWap(itemWap);
						switch (static_cast<Protocol::kInventoryType>(type))
						{
						case Protocol::kInventoryEquipped:
							inventory->PushItem(Protocol::kInventoryType::kInventoryEquipped, position, item);
							break;
						case Protocol::kInventoryEqp:
							inventory->PushItem(Protocol::kInventoryType::kInventoryEqp, position, item);
							break;
						case Protocol::kInventoryEtc:
							inventory->PushItem(Protocol::kInventoryType::kInventoryEtc, position, item);
							break;
						default: ;
						}
					}

					stats->SetStr(str);
					stats->SetDex(str);
					stats->SetInt(str);
					stats->SetLuk(str);
					stats->SetHp(hp);
					stats->SetMaxHp(maxhp);
					stats->SetMp(mp);
					stats->SetMaxMp(maxmp);
					stats->SetLevel(level);
					stats->SetExp(exp);
					_name = name;
				}
			} while (bind.SqlMoreResults() != SQL_NO_DATA);

			const auto& InfoStorageManager = CharacterInfoStorageManager::GetInstance();
			if (false == InfoStorageManager.PushInfo(CharacterInfoStorage::kInfoTypes::kStats, _character_id, stats))
			{
				return E_FAIL;
			}
			if (false == InfoStorageManager.PushInfo(CharacterInfoStorage::kInfoTypes::kInventory, _character_id, inventory))
			{
				return E_FAIL;
			}
			const auto mapInstance = MapManager::GetInstance().FindMapInstance(this->GetMapId());
			auto position = mapInstance->GetSpawnPoint(this->GetSpawnPoint());
			_transform->SetState(Transform::kState::kStatePosition, *position);
		}

		DBConnectionPool::GetInstance().Push(con);
	}
	return S_OK;
}

auto GameCharacter::SaveToDb() -> HRESULT
{
	const auto& InfoStorageManager = CharacterInfoStorageManager::GetInstance();
	const auto baseStatInfo = InfoStorageManager.FindInfo(CharacterInfoStorage::kInfoTypes::kStats, _character_id);
	const auto statInfo = std::static_pointer_cast<Statistic>(baseStatInfo);
	const auto baseInventorysInfo = InfoStorageManager.FindInfo(CharacterInfoStorage::kInfoTypes::kInventory, _character_id);
	const auto InventorysInfo = std::static_pointer_cast<Inventorys>(baseInventorysInfo);
	if (statInfo == nullptr || InventorysInfo == nullptr)
	{
		return E_FAIL;
	}
	int32_t result = 0;
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<14, 1> bind(*con, L"{CALL dbo.spSaveToPlayer(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)}");
		int32_t idx = 0;
		bind.BindParam(idx++, _character_id);
		int32_t spawnPoint = 0;
		bind.BindParam(idx++, spawnPoint);//spawnPoint

		int32_t hp(statInfo->GetHp());
		bind.BindParam(idx++, hp);
		int32_t maxHp(statInfo->GetMaxHp());
		bind.BindParam(idx++, maxHp);

		int32_t mp(statInfo->GetHp());
		bind.BindParam(idx++, mp);
		int32_t maxMp(statInfo->GetMaxHp());
		bind.BindParam(idx++, maxMp);

		int32_t level(statInfo->GetLevel());
		bind.BindParam(idx++, level);
		int32_t exp(statInfo->GetExp());
		bind.BindParam(idx++, exp);
		int32_t money(0);
		bind.BindParam(idx++, money);

		int32_t str(statInfo->GetStr());
		bind.BindParam(idx++, str);
		int32_t dex(statInfo->GetDex());
		bind.BindParam(idx++, dex);
		int32_t int_(statInfo->GetInt());
		bind.BindParam(idx++, int_);
		int32_t luk(statInfo->GetLuk());
		bind.BindParam(idx++, luk);

		auto itemList = InventorysInfo->ItemListToXml();
		bind.BindParam(idx++, itemList.c_str());

		bind.BindCol(0, result);



		if (bind.Execute())
		{
			do
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (bind.Fetch())
					{
					}

				}
			} while (bind.SqlMoreResults() != SQL_NO_DATA);

		}

		DBConnectionPool::GetInstance().Push(con);
	}
	return result == 1 ? S_OK : E_FAIL;
}

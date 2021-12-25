#include "game_server_pch.h"
#include "game_character.h"

#include "information_collection/inventorys/inventorys.h"
#include "information_collection/stats/stats.h"
#include "managers/character_info_manager/character_info_storage_manager.h"
#include "src/database/db_connection_pool.h"
#include "src/database/db_bind.h"

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

auto GameCharacter::NativeConstruct() -> HRESULT
{
	if (auto con = DBConnectionPool::GetInstance().Pop())
	{
		DBBind<1, 10> bind(*con, L"{CALL dbo.spLoadCharacter(?)}");


		bind.BindParam(0, _character_id);

		WCHAR name[100]{0};
		bind.BindCol(0, name);
		bind.BindCol(1, _account_id);
		bind.BindCol(2, _gender);
		bind.BindCol(3, _face_id);

		int32_t str;
		bind.BindCol(4, str);
		int32_t dex;
		bind.BindCol(5, dex);
		int32_t int_;
		bind.BindCol(6, int_);
		int32_t luk;
		bind.BindCol(7, luk);

		int32_t itemid;
		bind.BindCol(8, itemid);
		int32_t position;
		bind.BindCol(9, position);

		if (bind.Execute())
		{
			const std::shared_ptr<Stats> stats = Stats::Create(_character_id);
			const std::shared_ptr<Inventorys> inventory = Inventorys::Create(_character_id);
			do
			{
				int16_t count = 0;
				bind.SqlNumResultCols(&count);
				if (count > 0)
				{
					while (bind.Fetch())
					{
						if (position < 0)
						{
							inventory->PushItem(Protocol::kInventoryType::kInventoryEquipped, position, itemid);
						}
					}

					stats->SetStr(str);
					stats->SetDex(str);
					stats->SetInt(str);
					stats->SetLuk(str);
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
		}

		DBConnectionPool::GetInstance().Push(con);
	}
	return S_OK;
}

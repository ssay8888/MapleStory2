#include "game_server_pch.h"
#include "monster_walk_state.h"

#include "data_reader/data_reader_manager.h"
#include "game/entitiy/monster/game_monster.h"
#include "game/entitiy/monster/spawn_point/spawn_point.h"
#include "game/map/map_instance.h"
#include "protocol/game_enum.pb.h"
#include "protocol/game_protocol.pb.h"
#include "randomizer/randomizer.h"
#include "src/utility/components/collider/collider.h"
#include "src/utility/components/transform/transform.h"

auto MonsterWalkState::Enter(std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc = 0;
	int64_t degree = Randomizer::Rand(static_cast<int64_t>(0), 360);
	_radian = D3DXToRadian(degree);
	_is_move = true;
	auto block = monster->GetBlockRangeAabb();
	block->UpdateCollider();
	auto range = monster->GetReloadRangeAabb();
	for (const auto& reload : range)
	{
		reload->UpdateCollider();
	}
	monster->GetMonsterColliderAabb()->UpdateCollider();
	ReloadMapObject(monster, true);
}

auto MonsterWalkState::Tick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	_animation_acc += timeDelta;
	auto transform = monster->GetTransform();
	if (_is_move)
	{
		transform->SetUpRotation(_float3(0.f, 1.f, 0.f), _radian);
		transform->BackStraight(timeDelta);

		monster->GetMonsterColliderAabb()->UpdateCollider();
		auto range = monster->GetReloadRangeAabb();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}
		_last_tile_map_object = LoadLastTile(monster);
		if (StraightCheck(monster) || !BlockUpCheck(monster))
		{
			transform->BackStraight(-timeDelta);
		}
		else
		{
			Protocol::GameServerMoveMonster sendPkt;
			sendPkt.set_object_id(monster->GetObjectId());
			sendPkt.set_state(Protocol::kWalkA);

			auto sendRight = sendPkt.mutable_right();
			auto right = monster->GetTransform()->GetState(Transform::kState::kStateRight);
			sendRight->set_x(right.x);
			sendRight->set_y(right.y);
			sendRight->set_z(right.z);

			auto sendUp = sendPkt.mutable_up();
			auto up = monster->GetTransform()->GetState(Transform::kState::kStateUp);
			sendUp->set_x(up.x);
			sendUp->set_y(up.y);
			sendUp->set_z(up.z);


			auto sendLook = sendPkt.mutable_look();
			auto look = monster->GetTransform()->GetState(Transform::kState::kStateLook);
			sendLook->set_x(look.x);
			sendLook->set_y(look.y);
			sendLook->set_z(look.z);


			auto sendPos = sendPkt.mutable_position();
			auto pos = monster->GetTransform()->GetState(Transform::kState::kStatePosition);
			sendPos->set_x(pos.x);
			sendPos->set_y(pos.y);
			sendPos->set_z(pos.z);

			sendRight = sendPkt.mutable_cright();
			right = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateRight);
			sendRight->set_x(right.x);
			sendRight->set_y(right.y);
			sendRight->set_z(right.z);

			sendUp = sendPkt.mutable_cup();
			up = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateUp);
			sendUp->set_x(up.x);
			sendUp->set_y(up.y);
			sendUp->set_z(up.z);


			sendLook = sendPkt.mutable_clook();
			look = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStateLook);
			sendLook->set_x(look.x);
			sendLook->set_y(look.y);
			sendLook->set_z(look.z);


			sendPos = sendPkt.mutable_cposition();
			pos = monster->GetMonsterColliderAabb()->GetTransform()->GetState(Transform::kState::kStatePosition);
			sendPos->set_x(pos.x);
			sendPos->set_y(pos.y);
			sendPos->set_z(pos.z);

			monster->GetMapInstance()->BroadCastMessage(sendPkt, nullptr);
		}
		monster->GetMonsterColliderAabb()->UpdateCollider();
		for (const auto& reload : range)
		{
			reload->UpdateCollider();
		}

	}
}

auto MonsterWalkState::LateTick(const double timeDelta, std::shared_ptr<GameMonster> monster) -> void
{
	const auto kfm = DataReaderManager::GetInstance().FindAnyKey(monster->GetSpawnPoint()->GetSpawnNpcId());

	auto index = monster->GetStateIndex(Protocol::kMonsterState::kWalkA);
	auto seq = kfm->seqs[index];
	auto endTime = seq->key[L"end"];
	if (_animation_acc >= endTime * 2)
	{
		if (Randomizer::IsSuccess(50))
		{
			//monster->ChangeState(Protocol::kMonsterState::kBoreA);
		}
		else
		{
			monster->ChangeState(Protocol::kMonsterState::kIdleA);
		}
	}
}

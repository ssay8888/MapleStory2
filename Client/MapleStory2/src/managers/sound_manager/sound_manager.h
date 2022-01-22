#pragma once

class SoundManager
{
public:
	static auto GetInstance()->SoundManager&
	{
		static SoundManager instance;

		return instance;
	}
public:
	enum CHANNELID { kBgm, kDead, kPlayer, kMonster, kMonsterHit, kSkillVoice, kSkillUse, kSkillHit, kLevelUp, kPortal, kPotion, kEffect, kUi, kEnchant, kMaxChannel };
private:
	SoundManager();
	~SoundManager();

public:
	auto Initialize() -> void;

	auto Release() -> void;
public:
	auto PlaySound(const TCHAR* pSoundKey, CHANNELID eID) -> void;
	auto PlayBGM(const TCHAR* pSoundKey) -> void;
	auto StopSound(CHANNELID eID) -> void;
	auto StopAll() -> void;

private:
	void LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	std::map<TCHAR*, FMOD_SOUND*> _map_sound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* _channel_arr[kMaxChannel];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* _system;
	FMOD_CHANNELGROUP* _groupA;
};


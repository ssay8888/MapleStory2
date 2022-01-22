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
	// 사운드 리소스 정보를 갖는 객체 
	std::map<TCHAR*, FMOD_SOUND*> _map_sound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* _channel_arr[kMaxChannel];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* _system;
	FMOD_CHANNELGROUP* _groupA;
};


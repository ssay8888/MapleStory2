#include "c_pch.h"
#include "sound_manager.h"

#include <corecrt_io.h>

SoundManager::SoundManager() :
	_channel_arr{},
	_groupA(nullptr)
{
	_system = nullptr;
}

SoundManager::~SoundManager()
{
	Release();
}

auto SoundManager::Initialize() -> void
{
	FMOD_System_Create(&_system);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(_system, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateChannelGroup(_system, "group1", &_groupA);
	LoadSoundFile();
}

auto SoundManager::Release() -> void
{
	for (auto& Mypair : _map_sound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	_map_sound.clear();
	FMOD_System_Release(_system);
	FMOD_System_Close(_system);
}

auto SoundManager::PlaySound(const TCHAR* pSoundKey, CHANNELID eID) -> void
{
	std::map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(_map_sound.begin(), _map_sound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == _map_sound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(_channel_arr[eID], &bPlay))
	{
		FMOD_System_PlaySound(_system, FMOD_CHANNEL_FREE, iter->second, FALSE, &_channel_arr[eID]);
	}
	FMOD_System_Update(_system);
}

auto SoundManager::PlayBGM(const TCHAR* pSoundKey) -> void
{
	std::map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(_map_sound.begin(), _map_sound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == _map_sound.end())
		return;
	FMOD_System_PlaySound(_system, FMOD_CHANNEL_FREE, iter->second, FALSE, &_channel_arr[kBgm]);
	FMOD_Channel_SetMode(_channel_arr[kBgm], FMOD_LOOP_NORMAL);
	FMOD_System_Update(_system);
//	FMOD_Channel_SetVolume(_channel_arr[kBgm], 0.5);
}

void SoundManager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(_channel_arr[eID]);
}

auto SoundManager::StopAll() -> void
{
	for (int i = 0; i < kMaxChannel; ++i)
	{
		FMOD_Channel_Stop(_channel_arr[i]);
	}
}

auto SoundManager::LoadSoundFile() -> void
{
	_finddata_t fd;

	intptr_t handle = _findfirst("../../Binary/Resources/Sounds/*.*", &fd);

	if (handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Binary/Resources/Sounds/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(_system, szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, static_cast<int>(iLength), pSoundKey, static_cast<int>(iLength));

			_map_sound.emplace(pSoundKey, pSound);
		}
		iResult = _findnext(handle, &fd);
	}
	FMOD_System_Update(_system);
	_findclose(handle);
}

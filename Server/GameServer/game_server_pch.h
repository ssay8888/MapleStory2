#ifndef PCH_H
#define PCH_H

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "ClientCore\\Debug\\ClientCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#pragma comment(lib, "CommonLib\\Debug\\CommonLib.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif
// 여기에 미리 컴파일하려는 헤더 추가
#include "core_pch.h"
#include "engine_pch.h"
#include "game_defines.h"
#include "game_session/game_session.h"
using GameSessionRef = std::shared_ptr<GameSession>;
#include "game_contents/game_contents.h"
#include "common_defines.h"
#include <tbb/concurrent_hash_map.h>
extern std::atomic<int64_t> default_object_id;
#endif //PCH_H

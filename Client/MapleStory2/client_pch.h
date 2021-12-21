#pragma once
#include <cstdint>

#ifdef _DEBUG
#pragma comment(lib, "ClientCore\\Debug\\ClientCore.lib")
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "CommonLib\\Debug\\CommonLib.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif


#include <iostream>
#include <chrono>
using namespace std::chrono_literals;


#include "engine_pch.h"
#include "core_pch.h"
#include "core_macro.h"
#include "client_defines.h"
#include "protocol/login_protocol.pb.h"
#include "game_contents/game_contents.h"
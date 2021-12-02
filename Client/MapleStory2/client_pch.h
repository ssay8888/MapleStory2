#pragma once
#include <cstdint>

#ifdef _DEBUG
#pragma comment(lib, "ClientCore\\Debug\\ClientCore.lib")
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
//#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif


#include "engine_pch.h"
#include "core_macro.h"
#include "client_defines.h"
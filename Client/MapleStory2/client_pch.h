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

const int16_t		g_WinCX = 1280;
const int16_t		g_WinCY = 720;
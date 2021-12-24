#include "game_server_pch.h"
#include "loader.h"


Loader::Loader()
{
	Init();
}

auto Loader::Init() -> void
{
	ComponentLoad();
}

auto Loader::ComponentLoad()-> HRESULT
{
	return S_OK;
}

#pragma once

namespace NSInitClientLua
{
	const TCHAR*	GetRandomLobbyLoadingImage();
	BOOL			GetLobbyLoadingImageFullSize();
	BOOL			IsBattleIslandsWarLoadingImageOn( DWORD dwMapID );	// �ε��̹����� ���籺�� �¸������� ǥ�� �Ǿ� �ϴ°�~?

	HRESULT LuaLoad();
};

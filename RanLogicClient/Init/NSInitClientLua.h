#pragma once

namespace NSInitClientLua
{
	const TCHAR*	GetRandomLobbyLoadingImage();
	BOOL			GetLobbyLoadingImageFullSize();
	BOOL			IsBattleIslandsWarLoadingImageOn( DWORD dwMapID );	// 로딩이미지에 난사군도 승리국가가 표시 되야 하는가~?

	HRESULT LuaLoad();
};

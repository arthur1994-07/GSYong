#pragma once

namespace NSChangeCPS
{
	void SetPath( const TCHAR* pName );
	const TCHAR* GetPath();

	BOOL LuaLoad();

	const TCHAR* GetNewCPS( const TCHAR* pNameCPS );
};
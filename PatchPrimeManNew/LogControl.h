#pragma once

namespace NS_LOG_CONTROL
{
	extern DWORD nCurPos;
	extern DWORD nCurEnd;
	extern DWORD nAllPos;
	extern DWORD nAllEnd;

	void SetProcessCurPosition ( DWORD CurPos, DWORD CurEnd );
	void GetProcessCurPosition ( DWORD* pCurPos, DWORD* pCurEnd );
	void GetProcessCurPositionAddr ( DWORD** ppCurPos, DWORD** ppCurEnd );

	void SetProcessAllPosition ( DWORD AllPos, DWORD AllEnd );
	void GetProcessAllPosition ( DWORD* pAllPos, DWORD* pAllEnd );
};

#include "stdafx.h"

#include "./LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace NS_LOG_CONTROL
{
	DWORD nCurPos = 0;
	DWORD nCurEnd = 0;
	DWORD nAllPos = 0;
	DWORD nAllEnd = 0;

	void SetProcessCurPosition( DWORD CurPos, DWORD CurEnd )
	{
		nCurPos = CurPos;
		nCurEnd = CurEnd;
	}
	void GetProcessCurPosition( DWORD* pCurPos, DWORD* pCurEnd )
	{
		*pCurPos = nCurPos;
		*pCurEnd = nCurEnd;
	}
	void GetProcessCurPositionAddr( DWORD** ppCurPos, DWORD** ppCurEnd )
	{
		*ppCurPos = &nCurPos;
		*ppCurEnd = &nCurEnd;
	}

	void SetProcessAllPosition( DWORD AllPos, DWORD AllEnd )
	{
		nAllPos = AllPos;
		nAllEnd = AllEnd;
	}
	void GetProcessAllPosition( DWORD* pAllPos, DWORD* pAllEnd )
	{
		*pAllPos = nAllPos;
		*pAllEnd = nAllEnd;
	}
};


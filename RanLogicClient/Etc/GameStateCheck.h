#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "../../InternalCommonLib/dxstdafx.h"
#include "../../EngineLib/DxTools/d3dfont.h"

#define STATE_CHECK(name,func) CGameStateCheck::GetInstance().SetStateCheck(name,TRUE); func; CGameStateCheck::GetInstance().SetStateCheck(name,FALSE)

struct SSTATECHECK
{
	LARGE_INTEGER  beginTime;
	LARGE_INTEGER  endTime;

	SSTATECHECK()
	{
		Init();
	}

	void Init()
	{
		beginTime.QuadPart = 0;
		endTime.QuadPart   = 0;
	}

	const LONGLONG GetInterval() const { return endTime.QuadPart - beginTime.QuadPart; }
};

typedef std::map<std::string, SSTATECHECK> MAP_STATECHECK;
typedef	MAP_STATECHECK::iterator		  MAP_STATECHECK_ITER;

class CGameStateCheck
{
public:
	CGameStateCheck(void);
	virtual ~CGameStateCheck(void);

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT CreateGameStateCheck();
	void	FinalCleanup();
	void	DrawText( float x, float y, DWORD dwColor, const char* msg, ... );
	void	FrameMove( float fElapsedTime );
	void	Render();

	void	SetStateCheck( const std::string strCheckName, const bool bBegin )
	{
		LARGE_INTEGER perfCount;
		QueryPerformanceCounter(&perfCount); 
		if( bBegin )
			m_mapStateCheck[strCheckName].beginTime = perfCount;
		else
			m_mapStateCheck[strCheckName].endTime = perfCount;
	}

private:
	CD3DFont *m_pD3DFont;
	MAP_STATECHECK m_mapStateCheck;

	DWORD	  m_dwOldTime;
	float	  m_fFPS;
	BOOL	  m_bRecordLog;


public:
	static CGameStateCheck& GetInstance();
};

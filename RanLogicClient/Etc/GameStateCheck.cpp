#include "pch.h"
#include <map>
#include <numeric>
#include <algorithm>

#include "../../SigmaCore/gassert.h"

#include "../../SigmaCore/DebugSet.h"
#include "../../SigmaCore/gassert.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "./gamestatecheck.h"

CGameStateCheck& CGameStateCheck::GetInstance()
{
	static CGameStateCheck Instance;
	return Instance;
}

CGameStateCheck::CGameStateCheck(void)
: m_pD3DFont(NULL)
, m_dwOldTime(0)
, m_fFPS(0.0f)
{
}

CGameStateCheck::~CGameStateCheck(void)
{
	FinalCleanup();
}

void CGameStateCheck::FinalCleanup()
{
	SAFE_DELETE(m_pD3DFont);
}

HRESULT CGameStateCheck::CreateGameStateCheck()
{
	m_pD3DFont = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
	return S_OK;
}

HRESULT CGameStateCheck::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	hr = m_pD3DFont->InitDeviceObjects( pd3dDevice );
	if( FAILED(hr) ) return E_FAIL;
	return S_OK;
}

HRESULT CGameStateCheck::RestoreDeviceObjects()
{
	HRESULT hr;
	hr = m_pD3DFont->RestoreDeviceObjects();
	if( FAILED(hr) ) return E_FAIL;
	return S_OK;
}

HRESULT CGameStateCheck::InvalidateDeviceObjects()
{
	HRESULT hr;
	hr = m_pD3DFont->InvalidateDeviceObjects();
	if( FAILED(hr) ) return E_FAIL;
	return S_OK;
}

HRESULT CGameStateCheck::DeleteDeviceObjects()
{
	HRESULT hr;
	hr = m_pD3DFont->DeleteDeviceObjects();
	if( FAILED(hr) ) return E_FAIL;
	return S_OK;
}



void CGameStateCheck::DrawText( float x, float y, DWORD dwColor, const char* msg, ... )
{
	GASSERT ( msg && "CONSOLEMSG_WRITE()" );

	char sbuf[256];

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, 256, msg, ap);
	va_end(ap);	

	m_pD3DFont->DrawText( x, y, dwColor, sbuf );
}

void CGameStateCheck::FrameMove( float fElapsedTime )
{
	m_bRecordLog = FALSE;

	DWORD dwCurrentTime = GetTickCount();
	if( m_dwOldTime == 0 ) m_dwOldTime = GetTickCount();
	else{
		DWORD dwTimeInterval = dwCurrentTime - m_dwOldTime;
		if( dwTimeInterval >= 1000 )
		{
			m_fFPS	    = (dwTimeInterval / fElapsedTime)/dwTimeInterval;
			m_dwOldTime = dwCurrentTime;
			m_bRecordLog = TRUE;
		}
	}


	m_mapStateCheck.clear();
}

void CGameStateCheck::Render()
{
	const D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
	const float x = 10.0f;
	float yInterval = 15.0f;
	float y = 100.0f;

	DrawText( x, y, fontColor, "FPS : %.3f", m_fFPS );
	y += yInterval;

	LONGLONG lAllCheckTime = 0;

	if( m_bRecordLog )
	{
		CDebugSet::ToStateCheckFile( "############### Check ###############" );
	}

	MAP_STATECHECK_ITER map_Iter = m_mapStateCheck.begin();
	for( ; map_Iter != m_mapStateCheck.end(); ++map_Iter )
	{
		lAllCheckTime += map_Iter->second.GetInterval();
	}


	std::string strCheckName;
	SSTATECHECK sStateCheck;
	float fStatePercent = 0.0f;

	map_Iter = m_mapStateCheck.begin();
	for( ; map_Iter != m_mapStateCheck.end(); ++map_Iter )
	{
		strCheckName = map_Iter->first;
		sStateCheck  = map_Iter->second;
		if( sStateCheck.beginTime.QuadPart == 0 || sStateCheck.endTime.QuadPart == 0 ) continue;

		fStatePercent = (float)sStateCheck.GetInterval() / (float)lAllCheckTime * 100.0f;
		DrawText( x, y, fontColor, "%s : %I64d microsec %f %%", strCheckName.c_str(), sStateCheck.GetInterval(), fStatePercent );
		y += yInterval;

		if( m_bRecordLog )
		{
			CDebugSet::ToStateCheckFile( "%s : %I64d microsec %f%%", strCheckName.c_str(), sStateCheck.GetInterval(), fStatePercent );
		}

	}

	
	
}


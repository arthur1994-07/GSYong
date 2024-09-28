#pragma once

#include <string>

#include "../../SigmaCore/Loading/LoadingDirect.h"
#include "../../InternalCommonLib/dxstdafx.h"

class GLGaeaClient;

enum ELoadingType
{
	EM_LT_NONE,		// X
	EM_LT_NORMAL,	// 일반적인 로딩
};

class LoadingDirectClient : public LoadingDirect
{
public:
    static LPDIRECT3DDEVICEQ s_pd3dDevice;
	static HWND s_hWnd;
    static GLGaeaClient* s_pGaeaClient;

private:
    BOOL m_bCheckTime;
	WORD m_wType;
	float m_fTimeLAST;
	DWORD m_nStepCountMax;
	DWORD m_nStepCountCur;

public:
	LoadingDirectClient( BOOL bCheckTime );
	virtual ~LoadingDirectClient();

public:
	virtual void Initialize(BOOL _bCheckTime);
	virtual void SetMode( WORD wType, BOOL bShowWinnerCountryFlag );
	virtual void ResetMode();
	virtual void FrameMoveRender( BOOL bImmediately, BOOL bCheckTime );
};
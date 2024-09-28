#include "../pch.h"
#include "./LoadingDirectClient.h"

#include "../../SigmaCore/Util/EBTime.h"
#include "../Stage/DxGlobalStage.h"	// TEST_INTRO_VIDEO
#include "../GfxInterfaceBase.h"

#include "./LoadingThread.h"

//#ifdef TEST_INTRO_VIDEO
#include "../../DirectShowLib/DShow/NSDShow.h"
//#endif

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LPDIRECT3DDEVICEQ LoadingDirectClient::s_pd3dDevice = NULL;
HWND LoadingDirectClient::s_hWnd = NULL;
GLGaeaClient* LoadingDirectClient::s_pGaeaClient = NULL;

LoadingDirectClient::LoadingDirectClient( BOOL bCheckTime )
    : m_bCheckTime( bCheckTime )
    , m_wType( EM_LT_NONE )
	, m_fTimeLAST( 0.f )
	, m_nStepCountMax( 0 )
	, m_nStepCountCur( 0 )
{
}

LoadingDirectClient::~LoadingDirectClient()
{
}

void LoadingDirectClient::Initialize(BOOL _bCheckTime)
{
	m_bCheckTime = _bCheckTime;
	m_wType = EM_LT_NONE;
	m_fTimeLAST = 0.f;
	m_nStepCountMax = 0;
	m_nStepCountCur = 0;
}

void LoadingDirectClient::SetMode( WORD wType, BOOL bShowWinnerCountryFlag )
{
	m_wType = wType;
	m_nStepCountMax = 0;
	m_nStepCountCur = 0;

	switch( m_wType )
	{
	case EM_LT_NORMAL:
		{
			LoadingThread::Start_DirectRender( bShowWinnerCountryFlag );
		}
		break;
	};

	m_fTimeLAST = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
}

void LoadingDirectClient::ResetMode()
{
	switch( m_wType )
	{
	case EM_LT_NORMAL:
		{
			LoadingThread::End_DirectRender();
		}
		break;
	};

	m_wType = EM_LT_NONE;
}

// 기본 30 fps
float g_fUpdateTime( 0.033333f );

void LoadingDirectClient::FrameMoveRender( BOOL bImmediately, BOOL bCheckTime )
{
	if( m_wType == EM_LT_NONE )
		return;

//#ifdef TEST_INTRO_VIDEO
	if( false == NSDSHOW::IsPlayerPlayEnd() )
		return;
//#endif

	if( m_nStepCountCur < UINT_MAX )
		++m_nStepCountCur;

	// 윈도우가 "응답 없음" 상태에 빠지지 않도록 메시지를 받아줌
	{
		// [shhan][2015.04.15] PeekMessage( &msg, s_hWnd, 0, 0, PM_REMOVE ); 를 계속 하였더니
		//						맵이동중 crash 가 발생하였다. 
		//						PM_QS_PAINT 로 변경하니 괜찮아졌는데 이유를 잘 모르겠다.
		//						일단은 주석처리.
		//MSG msg;
		//PeekMessage( &msg, s_hWnd, 0, 0, PM_REMOVE );
	}

	// 동작시킬지 확인작업
	float fElapsedTime( g_fUpdateTime );
	BOOL bEnableRender( FALSE );
	if( bImmediately )
	{
		m_fTimeLAST = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	}
	else
	{
		// 일정시간이 지났으면 렌더링 작업을 한다.
		float fTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
		fElapsedTime = fTime - m_fTimeLAST;
		if( fElapsedTime < g_fUpdateTime )
			return;

		// 체크
		if( m_bCheckTime && bCheckTime )
		{
			if( fElapsedTime > 0.2f )
			{
				// 테스트 코드 일정시간 이상이 지나면 사이사이 더 추가하자.
				int a = 0;
			}
		}
	}

	float fTimeTemp = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

	// FrameMove와 Render
	switch( m_wType )
	{
	case EM_LT_NORMAL:
		{
			LoadingThread::FrameMove();
			LoadingThread::Render();
		}
		break;
	}

	m_fTimeLAST = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

	// 렌더링 Fps를 구한다.
	fElapsedTime = m_fTimeLAST - fTimeTemp;	

	// 렌더링 시간보다 3배는 더 많이 로딩시간을 가지도록 하자.
	g_fUpdateTime = fElapsedTime * 3.f;

	// 30fps는 넘지 않도록
	if( g_fUpdateTime < 0.033333f )
		g_fUpdateTime = 0.033333f;

	// 10fps는 내려가지 않도록
	if( g_fUpdateTime > 0.1f )
		g_fUpdateTime = 0.1f;
}
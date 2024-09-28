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

// �⺻ 30 fps
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

	// �����찡 "���� ����" ���¿� ������ �ʵ��� �޽����� �޾���
	{
		// [shhan][2015.04.15] PeekMessage( &msg, s_hWnd, 0, 0, PM_REMOVE ); �� ��� �Ͽ�����
		//						���̵��� crash �� �߻��Ͽ���. 
		//						PM_QS_PAINT �� �����ϴ� ���������µ� ������ �� �𸣰ڴ�.
		//						�ϴ��� �ּ�ó��.
		//MSG msg;
		//PeekMessage( &msg, s_hWnd, 0, 0, PM_REMOVE );
	}

	// ���۽�ų�� Ȯ���۾�
	float fElapsedTime( g_fUpdateTime );
	BOOL bEnableRender( FALSE );
	if( bImmediately )
	{
		m_fTimeLAST = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	}
	else
	{
		// �����ð��� �������� ������ �۾��� �Ѵ�.
		float fTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
		fElapsedTime = fTime - m_fTimeLAST;
		if( fElapsedTime < g_fUpdateTime )
			return;

		// üũ
		if( m_bCheckTime && bCheckTime )
		{
			if( fElapsedTime > 0.2f )
			{
				// �׽�Ʈ �ڵ� �����ð� �̻��� ������ ���̻��� �� �߰�����.
				int a = 0;
			}
		}
	}

	float fTimeTemp = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );

	// FrameMove�� Render
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

	// ������ Fps�� ���Ѵ�.
	fElapsedTime = m_fTimeLAST - fTimeTemp;	

	// ������ �ð����� 3��� �� ���� �ε��ð��� �������� ����.
	g_fUpdateTime = fElapsedTime * 3.f;

	// 30fps�� ���� �ʵ���
	if( g_fUpdateTime < 0.033333f )
		g_fUpdateTime = 0.033333f;

	// 10fps�� �������� �ʵ���
	if( g_fUpdateTime > 0.1f )
		g_fUpdateTime = 0.1f;
}
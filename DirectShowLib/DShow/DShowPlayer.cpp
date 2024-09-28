#include "stdafx.h"
#include "DShowPlayer.h"

namespace NSUSERID
{
	DWORD_PTR g_pUserId = 0xACDCACDC;
}

DShowPlayer::DShowPlayer(void)
:	m_pDevice( NULL )
,	m_lPlayEndMsg( 0 )
,	m_llLostTime( 0 )
,	m_bPlayingVideo( false )
,	m_nVolume( 0 )
,	m_pVmrAlloc( NULL )
{
}

DShowPlayer::~DShowPlayer(void)
{
	Release();
}

DShowPlayer& DShowPlayer::GetInstance()
{
	static DShowPlayer Instance;
	return Instance;
}

void DShowPlayer::Release()
{
	if( NULL != m_pMediaCtrl )
		m_pMediaCtrl->Stop();
	if( NULL != m_pMediaEvent )
		m_pMediaEvent->SetNotifyWindow( NULL, 0, 0 );

	m_pAudio.Release();
	m_pMediaPos.Release();
	m_pMediaCtrl.Release();
	m_pMediaEvent.Release();
	m_pConfig.Release();
	m_pAlloc.Release();
	m_pGraph.Release();

	m_pVmrAlloc = NULL;
}

HRESULT DShowPlayer::Init( HWND hWnd, long lMsg, LPDIRECT3DDEVICE9 pDevice )
{
	m_hWnd = hWnd;
	m_lPlayEndMsg = lMsg;

	m_pDevice = pDevice;

	HRESULT hr;

	hr = CoInitialize( NULL );
	if( FAILED( hr ) )
		return hr;
	
	return S_OK;
}

HRESULT DShowPlayer::Load( const std::string& strFile )
{
	HRESULT hr;

	// 필터 그래프 생성
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph );
	if( FAILED( hr ) )
		return hr;

	// VMR9 필터 생성
	CComQIPtr< IBaseFilter > pFilter;
	hr = CoCreateInstance( CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFilter );
	if( FAILED( hr ) )
		return hr;

	// 오디오 컨트롤
	hr = m_pGraph->QueryInterface( IID_IBasicAudio, (void**) &m_pAudio );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pAudio->put_Volume( m_nVolume );

	hr = pFilter->QueryInterface( IID_IVMRFilterConfig9, reinterpret_cast<void**> ( &m_pConfig ) );
	if( FAILED( hr ) )
		return hr;

	hr = m_pConfig->SetRenderingMode( VMR9Mode_Renderless );
	if( FAILED( hr ) )
		return hr;

	hr = m_pConfig->SetNumberOfStreams( 2 );
	if( FAILED( hr ) )
		return hr;

	CComQIPtr< IVMRSurfaceAllocatorNotify9 > lpIVMRSurfAllocNotify;
	hr = pFilter->QueryInterface( IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**> ( &lpIVMRSurfAllocNotify ) );
	if( FAILED( hr ) )
		return hr;


	LPDIRECT3D9 pD3D = NULL;
	m_pDevice->GetDirect3D( &pD3D );

	// create our surface allocator
	m_pVmrAlloc = new DxVmrAllocator( m_hWnd, pD3D, m_pDevice );
	m_pAlloc.Attach( m_pVmrAlloc );
	
	pD3D->Release();


	// let the allocator and the notify know about each other
	FAIL_RET( lpIVMRSurfAllocNotify->AdviseSurfaceAllocator( NSUSERID::g_pUserId, m_pAlloc ) );
	FAIL_RET( m_pAlloc->AdviseNotify( lpIVMRSurfAllocNotify ) );

	hr = m_pGraph->AddFilter( pFilter, L"Video Mixing Renderer 9" );
	if( FAILED( hr ) )
		return hr;

	wchar_t strFileW[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, strFile.c_str(), -1, strFileW, MAX_PATH );
	hr = m_pGraph->RenderFile( strFileW, NULL );
	if( FAILED( hr ) )
		return hr;

	// 미디어 이벤트 생성
	hr = m_pGraph->QueryInterface( IID_IMediaEventEx, (void**)&m_pMediaEvent );
	if( FAILED( hr ) )
		return hr;

	m_pMediaEvent->SetNotifyWindow( (OAHWND) m_hWnd, m_lPlayEndMsg, 0 );

	hr = m_pGraph->QueryInterface( IID_IMediaControl, (void **)&m_pMediaCtrl );
	if( FAILED( hr ) )
		return hr;

	hr = m_pGraph->QueryInterface( IID_IMediaPosition, (void**) &m_pMediaPos );
	if( FAILED( hr ) )
		return hr;

	m_strPathFile = strFile;

	return S_OK;
}

void DShowPlayer::OnResetDevice()
{
	if( false == m_bPlayingVideo )
		return;

	Load( m_strPathFile );

	if( NULL != m_pMediaPos )
		m_pMediaPos->put_CurrentPosition( m_llLostTime );

	Play();
}

void DShowPlayer::OnLostDevice()
{
	if( NULL != m_pMediaPos )
		m_pMediaPos->get_CurrentPosition( &m_llLostTime );
	else
		m_llLostTime = 0;

	Release();
}

void DShowPlayer::Play()
{
	if( NULL == m_pMediaCtrl )
		return;

	m_pMediaCtrl->Run();

	m_bPlayingVideo = true;
}

void DShowPlayer::Stop()
{
	if( NULL == m_pMediaCtrl )
		return;

	m_pMediaCtrl->Stop();

	m_bPlayingVideo = false;
}

bool DShowPlayer::IsComplete()
{
	if( NULL == m_pMediaEvent )
		return true;

	LONG evCode;
	LONG params[ 2 ];

	bool bComplte = false;
	HRESULT hr;
	while( hr = m_pMediaEvent->GetEvent( &evCode, &params[ 0 ], &params[ 1 ], 0 ), SUCCEEDED( hr ) )
	{
		switch( evCode )
		{
		case EC_COMPLETE :
			bComplte = true;
			m_bPlayingVideo = false;
			break;
		}

		hr = m_pMediaEvent->FreeEventParams( evCode, params[ 0 ], params[ 1 ] );
	}

	return bComplte;
}

void DShowPlayer::SetVolume( WORD wVolume, int nControlVolume )
{
	if( NULL == m_pAudio )
		return;

	float fPercent = float( wVolume ) / 100.0f;
	int nVol = MIN_DECIBEL;

	long lVolume = long( nControlVolume * fPercent );
	if( lVolume < 1 )
		nVol = MIN_DECIBEL;
	else
		nVol = lVolume - nControlVolume;

	m_pAudio->put_Volume( nVol );

	m_nVolume = nVol;
}
#include "stdafx.h"
#include "DShowTexture.h"

DShowTextureRenderer::DShowTextureRenderer( LPUNKNOWN pUnk, HRESULT *phr, LPDIRECT3DDEVICE9 pDevice, 
										    LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget )
: CBaseVideoRenderer( __uuidof( CLSID_TextureRenderer ), 
					  NAME( "DShowTextureRenderer" ), pUnk, phr ) 
, m_bUseDynamicTextures( FALSE )
, m_pDevice( pDevice )
, m_ppTexture( ppTexture ) 
, m_ppRenderTarget( ppRenderTarget )
{
	if( NULL != phr )
	{
		if( NULL != m_pDevice )
			*phr = S_OK;
		else
			*phr = E_FAIL;
	}
}

DShowTextureRenderer::~DShowTextureRenderer()
{
}

HRESULT DShowTextureRenderer::ResetTexture()
{
	if( NULL != (*m_ppTexture) )
	{
		(*m_ppTexture)->Release();
		(*m_ppTexture) = NULL;
	}

	UINT nWidth = 2;
	UINT nHeight = 2;

	// here let's check if we can use dynamic textures
	D3DCAPS9 caps;
	ZeroMemory( &caps, sizeof( D3DCAPS9 ) );
	m_pDevice->GetDeviceCaps( &caps );

	if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		while( (LONG) nWidth < m_lVidWidth )
			nWidth = nWidth << 1;

		while( (LONG) nHeight < m_lVidHeight )
			nHeight = nHeight << 1;
	}
	else
	{
		nWidth = m_lVidWidth;
		nHeight = m_lVidHeight;
	}

	DWORD dwUsage = 0;
	D3DPOOL d3dPool = D3DPOOL_MANAGED;

	if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
	{
		dwUsage = D3DUSAGE_DYNAMIC;
		d3dPool = D3DPOOL_DEFAULT;

		m_bUseDynamicTextures = TRUE;
	}

	m_pDevice->CreateTexture( nWidth, nHeight, 1, dwUsage, D3DFMT_X8R8G8B8, d3dPool, m_ppTexture, NULL );

	D3DSURFACE_DESC ddsd;
	ZeroMemory( &ddsd, sizeof( ddsd ) );
	if ( FAILED( (*m_ppTexture)->GetLevelDesc( 0, &ddsd ) ) )
		return E_FAIL;

	if( SUCCEEDED( (*m_ppTexture)->GetSurfaceLevel( 0, m_ppRenderTarget ) ) )
		(*m_ppRenderTarget)->GetDesc( &ddsd );

	return S_OK;
}

HRESULT DShowTextureRenderer::CheckMediaType( const CMediaType* pMediaType )
{
	HRESULT hr = E_FAIL;
	VIDEOINFO* pVideoInfo = NULL;

	CheckPointer( pMediaType, E_POINTER );

	// Reject the connection if this is not a video type
	if( *pMediaType->FormatType() != FORMAT_VideoInfo )
		return E_INVALIDARG;

	// Only accept RGB24 video
	pVideoInfo = (VIDEOINFO*) pMediaType->Format();

	if( IsEqualGUID( *pMediaType->Type(), MEDIATYPE_Video ) &&
		IsEqualGUID( *pMediaType->Subtype(), MEDIASUBTYPE_RGB24 ) )
		hr = S_OK;

	return hr;
}

HRESULT DShowTextureRenderer::SetMediaType( const CMediaType* pMediaType )
{
	// Retrive the size of this media type
	VIDEOINFO* pviBmp;
	pviBmp = (VIDEOINFO*) pMediaType->Format();

	m_lVidWidth  = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs( pviBmp->bmiHeader.biHeight );
	m_lVidPitch  = ( m_lVidWidth * 3 + 3 ) & ~( 3 );

	if( FAILED( ResetTexture() ) )
		return E_FAIL;

	return S_OK;
}

HRESULT DShowTextureRenderer::DoRenderSample( IMediaSample* pMediaSample )
{
	BYTE* pBmpBuffer;
	BYTE* pTxtBuffer;
	LONG  lTxtPitch;

	BYTE* pbS = NULL;
	DWORD* pdwS = NULL;
	DWORD* pdwD = NULL;
	UINT nRow, nCol, nWidth;

	CheckPointer( pMediaSample, E_POINTER );
	CheckPointer( m_ppTexture, E_UNEXPECTED );

	if( NULL == *m_ppTexture )
		return E_FAIL;

	// Get the video bitmap buffer
	pMediaSample->GetPointer( &pBmpBuffer );

	// Lock the Texture
	D3DLOCKED_RECT d3dlr;
	if( TRUE == m_bUseDynamicTextures )
	{
		if( FAILED( (*m_ppTexture)->LockRect( 0, &d3dlr, 0, D3DLOCK_DISCARD ) ) )
			return E_FAIL;
	}
	else
	{
		if( FAILED( (*m_ppTexture)->LockRect( 0, &d3dlr, 0, 0 ) ) )
			return E_FAIL;
	}

	// Get the texture buffer & pitch
	pTxtBuffer = static_cast< byte* > ( d3dlr.pBits );
	lTxtPitch = d3dlr.Pitch;

	nWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS

	pTxtBuffer += ( lTxtPitch * m_lVidHeight );

	for( nRow = 0; nRow< (UINT) m_lVidHeight; ++nRow )
	{
		pdwS = ( DWORD*)pBmpBuffer;
		pdwD = ( DWORD*)pTxtBuffer;

		for( nCol = 0; nCol < nWidth; ++nCol )
		{
			pdwD[ 0 ] =  pdwS[ 0 ] | 0xFF000000;
			pdwD[ 1 ] = ( ( pdwS[ 1 ] << 8 )  | 0xFF000000 ) | ( pdwS[ 0 ] >> 24 );
			pdwD[ 2 ] = ( ( pdwS[ 2 ] << 16 ) | 0xFF000000 ) | ( pdwS[ 1 ] >> 16 );
			pdwD[ 3 ] = 0xFF000000 | ( pdwS[ 2 ] >> 8 );
			pdwD += 4;
			pdwS += 3;
		}

		// we might have remaining (misaligned) bytes here
		pbS = (BYTE*) pdwS;
		for( nCol = 0; nCol < (UINT) m_lVidWidth % 4; ++nCol )
		{
			*pdwD = 0xFF000000 | ( pbS[ 2 ] << 16 ) | ( pbS[ 1 ] << 8 ) | ( pbS[ 0 ] );
			++pdwD;
			pbS += 3;
		}

		pBmpBuffer += m_lVidPitch;
		pTxtBuffer -= lTxtPitch;
	}

	if( FAILED( (*m_ppTexture)->UnlockRect( 0 ) ) )
		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------

DShowTexture::DShowTexture(void)
:	m_pTextureRenderer( NULL )
,	m_pMemReader( NULL )
{
}

DShowTexture::~DShowTexture(void)
{
	if( NULL != m_pTextureRenderer )
		m_pTextureRenderer->Release();

	if( NULL != m_pMemReader )
		m_pMemReader->Release();

	if( NULL != m_pMediaCtrl )
	{
		m_pMediaCtrl->Stop();
		m_pMediaCtrl.Release();
	}

	if( NULL != m_pMediaEvent )
		m_pMediaEvent.Release();

	if( NULL != m_pMediaPos )
		m_pMediaPos.Release();

	if( NULL != m_pAudio )
		m_pAudio.Release();

	if( NULL != m_pFilterGraph )
		m_pFilterGraph.Release();

	if( NULL != m_pGraph )
		m_pGraph.Release();
}

void DShowTexture::OnResetDevice()
{
	if( NULL == m_pTextureRenderer )
		return;

	m_pTextureRenderer->ResetTexture();
}

HRESULT DShowTexture::InitTextureRenderer( LPDIRECT3DDEVICE9 pDevice, const std::string& strFile, 
										   LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget )
{
	HRESULT hr;

	CComQIPtr< IBaseFilter > pSrcFilter;
	CComQIPtr< IPin > pSrcFilterPin;

	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pGraph->QueryInterface( IID_IBasicAudio, (void**) &m_pAudio );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pTextureRenderer = new DShowTextureRenderer( NULL, &hr, pDevice, ppTexture, ppRenderTarget );
	if( FAILED( hr ) )
	{
		delete m_pTextureRenderer;
		m_pTextureRenderer = NULL;
		return E_FAIL;
	}

	m_pRenderer = m_pTextureRenderer;
	hr = m_pGraph->AddFilter( m_pRenderer, L"TEXTURERENDERER" );
	if( FAILED( hr ) )
		return E_FAIL;

	wchar_t strFileW[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, strFile.c_str(), -1, strFileW, MAX_PATH );
	hr = m_pGraph->AddSourceFilter( strFileW, L"SOURCE", &pSrcFilter );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = pSrcFilter->FindPin( L"Output", &pSrcFilterPin );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pGraph->Render( pSrcFilterPin );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pGraph.QueryInterface( &m_pMediaCtrl );
	m_pGraph.QueryInterface( &m_pMediaPos );
	m_pGraph.QueryInterface( &m_pMediaEvent );

	return S_OK;
}

HRESULT DShowTexture::InitTextureRendererFromBuffer( LPDIRECT3DDEVICE9 pDevice, const std::string& strFile, 
									  LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget,
									  LPBYTE pBuffer, LONGLONG llLength )
{
	HRESULT hr = S_OK;

	CMediaType mt;
	mt.majortype = MEDIATYPE_Stream;
	mt.subtype = MEDIASUBTYPE_Avi;

	m_Stream = DShowMemStream( pBuffer, llLength );

	m_pMemReader = new DShowMemReader( &m_Stream, &mt, &hr );
	m_pMemReader->AddRef();


	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph, (void**) &m_pFilterGraph );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pFilterGraph->AddFilter( m_pMemReader, NULL );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pFilterGraph->QueryInterface( IID_IGraphBuilder, (void**) &m_pGraph );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pTextureRenderer = new DShowTextureRenderer( NULL, &hr, pDevice, ppTexture, ppRenderTarget );
	if( FAILED( hr ) )
	{
		delete m_pTextureRenderer;
		m_pTextureRenderer = NULL;
		return E_FAIL;
	}

	hr = m_pFilterGraph->QueryInterface( IID_IGraphBuilder, (void **)&m_pGraph );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pRenderer = m_pTextureRenderer;
	hr = m_pGraph->AddFilter( m_pRenderer, L"TEXTURERENDERER" );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pGraph->QueryInterface( IID_IBasicAudio, (void**) &m_pAudio );
	if( FAILED( hr ) )
		return E_FAIL;

	hr = m_pGraph->Render( m_pMemReader->GetPin( 0 ) );
	if( FAILED( hr ) )
		return E_FAIL;

	m_pGraph.QueryInterface( &m_pMediaCtrl );
	m_pGraph.QueryInterface( &m_pMediaPos );
	m_pGraph.QueryInterface( &m_pMediaEvent );

	return S_OK;
}

void DShowTexture::CheckVideoStatus()
{
	long lEventCode;
	LONG_PTR lParam1, lParam2;
	HRESULT hr;

	if( NULL == m_pMediaEvent )
		return;

	// Check for completion events
	hr = m_pMediaEvent->GetEvent( &lEventCode, &lParam1, &lParam2, 0 );
	if( SUCCEEDED( hr ) )
	{
		// If we have reached the end of the media file, reset to beginning
		if( EC_COMPLETE == lEventCode )
			hr = m_pMediaPos->put_CurrentPosition( 0 );

		// Free any memory associated with this event
		hr = m_pMediaEvent->FreeEventParams( lEventCode, lParam1, lParam2 );
	}
}

void DShowTexture::PlayVideo()
{
	if( NULL == m_pMediaCtrl )
		return;

	m_pMediaCtrl->Run();
}

void DShowTexture::PauseVideo()
{
	if( NULL == m_pMediaCtrl )
		return;

	m_pMediaCtrl->Stop();
}

void DShowTexture::StopVideo()
{
	if( NULL == m_pMediaCtrl || NULL == m_pMediaPos )
		return;

	m_pMediaCtrl->Stop();
	m_pMediaPos->put_CurrentPosition( 0 );
}

void DShowTexture::SetVolume( WORD wVolume, int nControlVolume )
{
	float fPercent = float( wVolume ) / 100.0f;
	int nVol = MIN_DECIBEL;

	long lVolume = long( nControlVolume * fPercent );
	if( lVolume < 1 )
		nVol = MIN_DECIBEL;
	else
		nVol = lVolume - nControlVolume;
	
	m_pAudio->put_Volume( lVolume );
	m_pAudio->put_Balance( 0 );
}

void DShowTexture::Set3DVolume( const D3DXVECTOR3& vPosition, 
				 const D3DXVECTOR3& vViewPos, const D3DXVECTOR3& vViewRight, float fRangeSq, int nControlVolume )
{
	if( NULL == m_pAudio )
		return;

	D3DXVECTOR3 v = vPosition - vViewPos;

	// º¼·ý
	{
		float fLength = D3DXVec3LengthSq( &v );
		long lVolume = static_cast<long>(nControlVolume * ( ( 100.0f - ( fLength / fRangeSq * 100.0f ) ) / 100.0f ));
		if( lVolume < 1 )
			lVolume = MIN_DECIBEL;
		else
			lVolume = lVolume - nControlVolume;

		m_pAudio->put_Volume( lVolume );
	}

	// ¹ë·±½º
	{
		D3DXVECTOR3 vNormal;
		D3DXVec3Normalize( &vNormal, &v );

		long lBalace = static_cast<long>(D3DXVec3Dot( &vViewRight, &vNormal ) * RIGHT_BALANCE);

		m_pAudio->put_Balance( lBalace );
	}
}
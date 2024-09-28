#include "StdAfx.h"
#include "UIRenderer.h"
#include "UIXMLManager.h"

#include "../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../EngineLib/GUInterface/UIDataType.h"

#include "../RanLogic/RANPARAM.h"

namespace HTML5UI
{

static DWORD dwAlphaBlendEnable, dwSrcBlend, dwDestBlend, dwZWriteEnable, dwZEnable, dwZFunc, dwClipPlaneEnable, dwFogEnable;
static DWORD dwColorOP, dwAlphaOP, dwMinFilter, dwMagFilter, dwMipFilter;

//////////////////////////////////////////////////////////////////////////////
//! 세팅 Render State;
void CUIRenderer::SetRenderState()
{
	if( !m_pDevice )
		return;

	m_pDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	m_pDevice->GetRenderState( D3DRS_SRCBLEND,			&dwSrcBlend );
	m_pDevice->GetRenderState( D3DRS_DESTBLEND,			&dwDestBlend );

	m_pDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	m_pDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
	m_pDevice->GetRenderState( D3DRS_ZFUNC,				&dwZFunc);
	m_pDevice->GetRenderState( D3DRS_CLIPPLANEENABLE,	&dwClipPlaneEnable );
	m_pDevice->GetRenderState( D3DRS_FOGENABLE,			&dwFogEnable );

	m_pDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );
	m_pDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

	m_pDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,	&dwMinFilter );
	m_pDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER,	&dwMagFilter );
	m_pDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER,	&dwMipFilter );

	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
	m_pDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );
	m_pDevice->SetRenderState( D3DRS_ZFUNC,				D3DCMP_ALWAYS);
	m_pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	FALSE );
	m_pDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );

	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

	m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );
}

//! 회복 Render State;
void CUIRenderer::RecoverRenderState()
{
	if( !m_pDevice )
		return;

	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND,			dwSrcBlend );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND,			dwDestBlend );

	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	m_pDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
	m_pDevice->SetRenderState( D3DRS_ZFUNC,				dwZFunc);
	m_pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	dwClipPlaneEnable );
	m_pDevice->SetRenderState( D3DRS_FOGENABLE,			dwFogEnable );

	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );
	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );

	m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	dwMinFilter );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	dwMagFilter );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	dwMipFilter );
}

//////////////////////////////////////////////////////////////////////////////
//! Device
bool CUIRenderer::InitDevice()
{
	LoadComponentTexture();

	return true;
}

bool CUIRenderer::RestoreDevice()
{
	InvalidateDevice();

	LoadComponentTexture();

	return true;
}

bool CUIRenderer::InvalidateDevice()
{
	UnloadComponentTexture();

	// Unload TextureMap
	m_uiMap.clear();

	return true;
}

bool CUIRenderer::DeleteDevice()
{
	UnloadComponentTexture();

	m_pDevice = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//! 파일을 이용해 Texture 그리기;
bool CUIRenderer::RenderTextureUsingFile(
	GUIBase::UIID uiID,
	const std::string& textureID,
	GUIBase::COMPONENT_POSITION& compPosition )
{
	WindowRender::COMPONENT_TEXTURE compTexture;
	WindowRender::COMP_TEXTURE_MAP_ITER it = m_componentMap.find( textureID );
	if( it != m_componentMap.end() )
		compTexture = it->second;
	else
		return false;

	// 렌더State 회복;
	RecoverRenderState();

	// TEXEL 세팅;
	WindowRender::TEXEL texel;
	texel.uStart = compTexture.texPosition.left;
	texel.uEnd = compTexture.texPosition.right;
	texel.vStart = compTexture.texPosition.top;
	texel.vEnd = compTexture.texPosition.bottom;

	RenderUsingTexture( compPosition.rcGlobalPos,
		compTexture.pTexture, texel );

	// 렌더State 회복;
	RecoverRenderState();

	return true;
}

//////////////////////////////////////////////////////////////////////////////
/// Html Rendering
//! size 크기의 Texture에 Image Data 갱신시키기;
bool CUIRenderer::RenderTextureUsingData(
	GUIBase::UIID uiID,
	GUIBase::UI_IMAGE_DATA& imgData,
	GUIBase::COMPONENT_POSITION& compPosition )
{
	if( !m_pDevice )
		return false;

	// UIID를 이용해 텍스처를 가져옴;
	std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > uiTexture(
		new WindowRender::HTML_TEXTURE );
	WindowRender::UI_TEXTURE_MAP_ITER it = m_uiMap.find( uiID );
	if( it != m_uiMap.end() )
		uiTexture = it->second;
	else
		m_uiMap.insert( WindowRender::UI_TEXTURE_MAP_VALUE( uiID, uiTexture ) );

	// Texture가 만들어져 있지 않을 경우 생성;
	if( !uiTexture->pTexture )
	{
		if( FAILED( m_pDevice->CreateTexture(
			imgData.viewSize.x, imgData.viewSize.y,
			1, D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			&uiTexture->pTexture, NULL ) ) )
			return false;

		uiTexture->pTexture->GetSurfaceLevel(
			0, &uiTexture->pSurface );
	}

	// 렌더State 세팅;
	SetRenderState();

	// 스크롤링;
	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( m_pDevice );
	if( FAILED( Scrolling( uiTexture
		, imgData ) ) )
		return false;
	sBackupTarget.Restore();

	// 메인 렌더링;
	sBackupTarget.Backup( m_pDevice );
	if( FAILED( CreateTextureUsingUpdateImg(
		uiTexture, imgData ) ) )
		return false;
	sBackupTarget.Restore();

	// 렌더State 회복;
	RecoverRenderState();

	return true;
}

//! 만들어진 UI 그리기;
void CUIRenderer::RenderUI(  GUIBase::UIRECT rcGlobalPos
						   , GUIBase::UIID uiID )
{
	std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture(
		new WindowRender::HTML_TEXTURE );
	WindowRender::UI_TEXTURE_MAP_ITER it = m_uiMap.find( uiID );
	if( it != m_uiMap.end() )
		htmlTexture = it->second;
	else
		return;

	// 렌더State 세팅;
	SetRenderState();

	// 만들어진 HTML UI 렌더링;
	RenderUsingTexture( rcGlobalPos
		, htmlTexture->pTexture );

	// 렌더State 회복;
	RecoverRenderState();
}

//////////////////////////////////////////////////////////////////////////////
bool CUIRenderer::RenderUsingTexture(  GUIBase::UIRECT rcGlobalPos
									 , LPDIRECT3DTEXTUREQ pTexture
									 , WindowRender::TEXEL texPos )
{
	if( !m_pDevice )
		return false;

	// 폴리곤 세팅;
	UIVERTEX vertex[ 4 ];
	vertex[0].SetPos( D3DXVECTOR2( rcGlobalPos.left, rcGlobalPos.top ) );
	vertex[1].SetPos( D3DXVECTOR2( rcGlobalPos.right, rcGlobalPos.top ) );
	vertex[2].SetPos( D3DXVECTOR2( rcGlobalPos.right, rcGlobalPos.bottom ) );
	vertex[3].SetPos( D3DXVECTOR2( rcGlobalPos.left, rcGlobalPos.bottom ) );
	SetTexturePos( vertex, texPos );
	static WORD index[ 6 ] = { 0, 1, 3, 3, 1, 2 };

	// 클리어;
	D3DRECT d3dRt;
	d3dRt.x1 = static_cast<LONG>( rcGlobalPos.left );
	d3dRt.x2 = static_cast<LONG>( rcGlobalPos.right );
	d3dRt.y1 = static_cast<LONG>( rcGlobalPos.top );
	d3dRt.y2 = static_cast<LONG>( rcGlobalPos.bottom );
	HRESULT hr = m_pDevice->Clear( 0L, &d3dRt, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		0xffffffff, 1.0f, 0L );
	if ( FAILED ( hr ) )
		return false;

	// 그리기;
	hr = m_pDevice->SetTexture( 0, pTexture );
	if ( FAILED ( hr ) )
		return false;

	hr = m_pDevice->SetFVF ( UIVERTEX::D3DFVF_UIVERTEX );
	if ( FAILED ( hr ) )
		return false;

	hr = m_pDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST, 0, 6, 2,
		index, D3DFMT_INDEX16,
		vertex, sizeof(UIVERTEX) );
	if ( FAILED ( hr ) )
		return false;

	return true;
}

//! 생성된 텍스처를 이용하여 그리기;
void CUIRenderer::SetTexturePos( UIVERTEX* vertex, const WindowRender::TEXEL& texel )
{
	vertex[0].SetTexturePos( texel.uStart, texel.vStart );
	vertex[1].SetTexturePos( texel.uEnd, texel.vStart );
	vertex[2].SetTexturePos( texel.uEnd, texel.vEnd );
	vertex[3].SetTexturePos( texel.uStart, texel.vEnd );
}

HRESULT CUIRenderer::CreateTextureUsingUpdateImg(  std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
												 , GUIBase::UI_IMAGE_DATA& imgData )
{
	HRESULT hr( S_OK );

	// Target 세팅;
	m_pDevice->SetRenderTarget( 0, htmlTexture->pSurface );

	// 업데이트 된 부분의 텍스처 획득;
	PaintTextureToData( htmlTexture, imgData );

	// 스크롤 텍스처 그리기;
	PaintScrollingTexture( htmlTexture, imgData.scrollRect, imgData.scrollOffset );

	// 업데이트 위치 정보 갱신;
	POINT updateRectLocation;
	updateRectLocation.x = imgData.updateRectBounds.left;
	updateRectLocation.y = imgData.updateRectBounds.top;
	for( UINT i=0; i<imgData.updateRectsCount; ++i )
	{
		// 폴리곤 위치 세팅;
		GUIBase::UIRECT updateRect = imgData.updateRects[ i ];
		GUIBase::UIRECT srcRect = updateRect;
		srcRect.left -= updateRectLocation.x;
		srcRect.right -= updateRectLocation.x;
		srcRect.top -= updateRectLocation.y;
		srcRect.bottom -= updateRectLocation.y;

		// UV 세팅;
		WindowRender::TEXEL texel;
		if( imgData.imgWidth )
		{
			if( srcRect.left )
				texel.uStart = srcRect.left / imgData.imgWidth;
			if( srcRect.right < imgData.imgWidth )
				texel.uEnd = srcRect.right / imgData.imgWidth;
		}
		if( imgData.imgHeight )
		{
			if( srcRect.top )
				texel.vStart = srcRect.top / imgData.imgHeight;
			if( srcRect.bottom < imgData.imgHeight )
				texel.vEnd = srcRect.bottom / imgData.imgHeight;
		}

		// Render
		if( !RenderUsingTexture( updateRect
			, htmlTexture->pUpdateTexture
			, texel ) )
			return S_FALSE;
	}

	return hr;
}

// Low Data로 Texture 만들기 ( Update Texture );
void CUIRenderer::PaintTextureToData(  std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
									 , GUIBase::UI_IMAGE_DATA& imgData )
{
	SAFE_RELEASE( htmlTexture->pUpdateTexture );

	// 업데이트 텍스처 생성;
	HRESULT hr = m_pDevice->CreateTexture(
		imgData.imgWidth, imgData.imgHeight, 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		&htmlTexture->pUpdateTexture, 0 );
	ASSERT( FAILED( hr ) );

	// 락;
	D3DLOCKED_RECT rt;
	ZeroMemory( &rt, sizeof(rt) );
	hr = htmlTexture->pUpdateTexture->LockRect( 0, &rt, 0, 0 );
	ASSERT( FAILED( hr ) );

	// 데이터 맵핑;
	void* pBuf = 0;
	pBuf = rt.pBits;
	int width = imgData.imgWidth;
	int height = imgData.imgHeight;
	void* pImgData = reinterpret_cast<void*>( imgData.imgData );
	memcpy( pBuf, pImgData, width*height*4 );

	// 언락;
	htmlTexture->pUpdateTexture->UnlockRect( 0 );
}

HRESULT CUIRenderer::Scrolling(  std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
							   , GUIBase::UI_IMAGE_DATA& imgData )
{
	HRESULT hr( S_OK );

	// Scroll이 적용되는지 검사;
	const POINT& scrollOffset = imgData.scrollOffset;
	if( scrollOffset.x == 0 && scrollOffset.y == 0 )
		return hr;

	// 스크롤 상자 구하기;
	const RECT& scrollRect = imgData.scrollRect;

	// 스크롤 텍스처 초기화;
	SAFE_RELEASE( htmlTexture->pScrollSurface );
	SAFE_RELEASE( htmlTexture->pScrollTexture );

	// 스크롤 크기;
	int nWidth = scrollRect.right-scrollRect.left;
	int nHeight = scrollRect.bottom-scrollRect.top;

	// 텍스처 생성;
	hr = m_pDevice->CreateTexture(
		nWidth, nHeight, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &htmlTexture->pScrollTexture, 0 );
	ASSERT( FAILED( hr ) );

	htmlTexture->pScrollTexture->GetSurfaceLevel( 0, &htmlTexture->pScrollSurface );
	ASSERT( !htmlTexture->pScrollSurface );

	// 렌더타겟 변경;
	m_pDevice->SetRenderTarget( 0, htmlTexture->pScrollSurface );

	// 스크롤링 된 부분의 폴리곤 구하기;
	GUIBase::UIRECT globalPos( 0.f, 0.f, nWidth, nHeight );

	// UV
	WindowRender::TEXEL texel;
	if( imgData.viewSize.x )
	{
		if( scrollRect.left )
			texel.uStart = static_cast<float>( scrollRect.left ) / imgData.viewSize.x;
		if( scrollRect.right < imgData.viewSize.x )
			texel.uEnd = static_cast<float>( scrollRect.right ) / imgData.viewSize.x;
	}
	if( imgData.viewSize.y )
	{
		if( scrollRect.top )
			texel.vStart = static_cast<float>( scrollRect.top ) / imgData.viewSize.y;
		if( scrollRect.bottom < imgData.viewSize.y )
			texel.vEnd = static_cast<float>( scrollRect.bottom ) / imgData.viewSize.y;
	}

	// Render
	if( !RenderUsingTexture( globalPos, htmlTexture->pTexture, texel ) )
		return S_FALSE;

	return hr;
}

HRESULT CUIRenderer::PaintScrollingTexture(  std::tr1::shared_ptr< WindowRender::HTML_TEXTURE > htmlTexture
										   , const RECT& scrollRect
										   , const POINT& scrollOffset )
{
	HRESULT hr( S_OK );

	// Scroll이 적용되는지 검사;
	if (scrollOffset.x == 0 && scrollOffset.y == 0)
		return hr;

	ASSERT( !m_pDevice );

	// 스크롤 되는 영역의 Offset을 구함;
	int sWidth = scrollRect.right - scrollRect.left;
	int sHeight = scrollRect.bottom - scrollRect.top;
	float offsetX = static_cast<float>( scrollOffset.x )/sWidth;
	float offsetY = static_cast<float>( scrollOffset.y )/sHeight;

	// 스크롤 되어 이동되어진 Rect 구하기;
	float left = scrollRect.left + offsetX*sWidth;
	float top = scrollRect.top + offsetY*sHeight;
	float right = scrollRect.right + offsetX*sWidth;
	float bottom = scrollRect.bottom + offsetY*sHeight;
	if( left < scrollRect.left )
		left = scrollRect.left;
	if( top < scrollRect.top )
		top = scrollRect.top;
	if( right > scrollRect.right )
		right = scrollRect.right;
	if( bottom > scrollRect.bottom )
		bottom = scrollRect.bottom;
	GUIBase::UIRECT globalPos( left, top, right-left, bottom-top );

	/// UV 구하기;
	WindowRender::TEXEL texel;
	if( offsetX < 0 )
		texel.uStart = offsetX*-1;
	else
		texel.uEnd = 1.0f - offsetX;

	if( offsetY < 0 )
		texel.vStart = offsetY*-1;
	else
		texel.vEnd = 1.0f - offsetY;

	// 스크롤 텍스처 그릴 위치;
	if( !RenderUsingTexture(
		globalPos,
		htmlTexture->pScrollTexture,
		texel ) )
		return S_FALSE;

	return hr;
}

//////////////////////////////////////////////////////////////////////////////
//! 텍스처 로딩;
void CUIRenderer::LoadComponentTexture()
{
	if( !m_pDevice )
		return;

	CUIXMLManager::GetInstance().LoadTextureList( RANPARAM::strUITexList.GetString() );
	CUIXMLManager::COMPTEX_VECTOR texVector = CUIXMLManager::GetInstance().GetTextureList();

	for( unsigned i=0; i<texVector.size(); ++i )
	{
		CUIXMLManager::TEXTURE_INFO textureInfo = texVector.at( i );

		WindowRender::COMPONENT_TEXTURE tempCompTexture;
		if( textureInfo.textureName.length() )
		{
			tempCompTexture.textureRes = 
				TextureManager::GetInstance().LoadTexture(
				textureInfo.textureName.c_str(),
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				1,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_UI,
				false,
				TRUE );

			tempCompTexture.pTexture = tempCompTexture.textureRes.GetTexture();
			tempCompTexture.texPosition = textureInfo.rcTexturePos;
			m_componentMap.insert( WindowRender::COMP_TEXTURE_MAP_VALUE(
				textureInfo.textureID, tempCompTexture ) );
		}
	}
}

//! 텍스처 언로딩;
void CUIRenderer::UnloadComponentTexture()
{
	CUIXMLManager::GetInstance().ClearTexture();

	for( WindowRender::COMP_TEXTURE_MAP_ITER it = m_componentMap.begin();
		it != m_componentMap.end() ; ++it )
	{
		WindowRender::COMPONENT_TEXTURE compTexture = it->second;
		if( compTexture.textureRes.IsValid() &&
			compTexture.textureRes.GetTexture() == compTexture.pTexture )
			compTexture.pTexture = NULL;

		compTexture.textureRes.Clear();
	}
	m_componentMap.clear();
}

//! 텍스처 리로딩;
void CUIRenderer::ReloadComponentTexture()
{
	UnloadComponentTexture();
	LoadComponentTexture();
}

//////////////////////////////////////////////////////////////////////////////

CUIRenderer::CUIRenderer()
: m_pDevice( NULL ) { }

CUIRenderer::~CUIRenderer() { }

}
#include "StdAfx.h"
#include "GfxRenderTexture.h"

#include "../Core/GfxCore.h"
#include "../GfxInterface.h"

using namespace Scaleform;
using namespace Render;

class GfxRenderTextureBase
{
public :
	Ptr< Scaleform::Render::Texture >	m_pGfxTexture;
	Scaleform::Render::ImageSize		m_OrigImgSize;

	GfxRenderTextureBase()
		: m_OrigImgSize( 0, 0 )
	{
	}
};

GfxRenderTexture::GfxRenderTexture(void)
:	m_nRenderTextureWidth( 0 )
,	m_nRenderTextureHeight( 0 )
,	m_fAspect( 0.0f )
,	m_nRefCount( 0 )
{
	m_pTextureBase = new GfxRenderTextureBase;
}

GfxRenderTexture::~GfxRenderTexture(void)
{
	SAFE_DELETE( m_pTextureBase );
}

void GfxRenderTexture::OnResetDevice()
{ 
}

void GfxRenderTexture::OnLostDevice()
{
	// 스케일폼 TextureManager::CreateTexture에서 Dx텍스처 AddRef 하고 로스트 디바이스 때 릴리즈를 안하고있다.
	// 여기서 직접해주자.
	D3D9::Texture* pd3dTexture = (D3D9::Texture*) m_pTextureBase->m_pGfxTexture.GetRawRef();
	if( NULL == pd3dTexture )
		return;

	D3D9::Texture::HWTextureDesc& desc = pd3dTexture->pTextures[ 0 ];
	if( NULL == desc.pTexture )
		return;

	LPDIRECT3DTEXTUREQ ptexture = desc.pTexture->GetResource();
	SAFE_RELEASE( ptexture );

	desc.pTexture->Clear();
}

HRESULT GfxRenderTexture::CreateGfxTexture( LPDIRECT3DTEXTUREQ pTexture )
{
	D3D9::HAL* pRenderHAL = GetCore()->GetRenderHAL();
	D3D9::TextureManager* pManager = (D3D9::TextureManager*) pRenderHAL->GetTextureManager();

	// gfx 텍스처 생성
	if( NULL == m_pTextureBase->m_pGfxTexture )
	{
		m_pTextureBase->m_pGfxTexture = pManager->CreateTexture( pTexture,
			ImageSize( m_nRenderTextureWidth, m_nRenderTextureHeight ) );
	}
	// 로스트 디바이스에서 직접 텍스처 릴리즈하면 TextureManager 리셋 디바이스에서 텍스처 초기화 시 뻗는다.
	// 텍스처 다시 넣어주자.
	else
	{
		pTexture->AddRef();

		D3D9::Texture* pd3dTexture = (D3D9::Texture*) m_pTextureBase->m_pGfxTexture.GetRawRef();
		if( NULL == pd3dTexture )
			return E_FAIL;

		D3D9::Texture::HWTextureDesc& desc = pd3dTexture->pTextures[ 0 ];
		desc.pTexture = *SF_NEW D3D9::HALIDirect3DTexture9( pTexture );
	}

	if( NULL == m_pTextureBase->m_pGfxTexture )
		return E_FAIL;

	return S_OK;
}

void GfxRenderTexture::ReplaceGfxTexture( const std::string& strLinkID, const INT nWidth, const INT nHeight )
{
	GFx::Movie* pMovie = GfxInterface::GetInstance()->GetGameScene()->GetMovie();
	if( NULL == pMovie )
		return;

	if( NULL == m_pTextureBase->m_pGfxTexture )
		return;

	m_strLinkID = strLinkID;

	if( nWidth != 0 && nHeight != 0 )
	{
		m_pTextureBase->m_OrigImgSize.Width = nWidth;
		m_pTextureBase->m_OrigImgSize.Height = nHeight;
	}

	// main.swf에서 리소스 가져옴.
	GFx::Resource* pRes = pMovie->GetMovieDef()->GetResource( m_strLinkID.c_str() );
	if( NULL == pRes )
		return;

	// 가져온 리소스가 이미지인지 확인.
	ImageResource* pImageRes = NULL;
	if( Resource::RT_Image == pRes->GetResourceType() )
		pImageRes = (ImageResource*) pRes;

	if( NULL == pImageRes )
		return;

	// 실제 이미지
	ImageBase* pImageOrig = pImageRes->GetImage();
	if( NULL == pImageOrig )
		return;

	Ptr< TextureImage > pTextureImg = *new TextureImage( Image_R8G8B8,
		m_pTextureBase->m_pGfxTexture->GetSize(), 0, m_pTextureBase->m_pGfxTexture );

	//  [7/26/2016 gbgim];
	// 아래 방식 사용시 주의!;
	// Release로 배포시 swf내에서 사용하는 리소스들은 하나의 dds로 팩킹되는데;
	// 사이즈를 하나의 dds에서 읽어온다; 원치않는 사이즈가 읽혀진다;
	// 그리고 렌더 텍스쳐를 ReleaseD에선 따로 사용하지만, Release에서는 하나로 공유되는 듯하다;
	// 실제 이미지 사이즈 가져옴.
	/*
	if( true == m_pTextureBase->m_OrigImgSize.IsEmpty() )
	{
		m_pTextureBase->m_OrigImgSize = pImageOrig->GetSize();
	}
	*/

	// gfx 텍스처 사이즈
	ImageSize newSize = m_pTextureBase->m_pGfxTexture->GetSize();

	// 실제 이미지와 텍스처 사이즈 비율 계산
	Size< float > sfParam( (float) m_pTextureBase->m_OrigImgSize.Width / newSize.Width,
						   (float) m_pTextureBase->m_OrigImgSize.Height / newSize.Height );

	// 종횡비 계산
	m_fAspect = sfParam.Width / sfParam.Height;

	Scaleform::Render::HAL* pRenderHAL = GetCore()->GetRenderHAL();
	Matrix2F& vptMatrix = pRenderHAL->GetMatrices()->GetFullViewportMatrix( m_pTextureBase->m_OrigImgSize );

	// 텍스처 행렬 계산 및 적용
	bool invertTexture = vptMatrix.M[ 1 ][ 1 ] > 0.0f;

	Matrix2F textureMatrix = Matrix2F::Scaling( sfParam.Width, sfParam.Height * (invertTexture ? -1.0f : 1.0f) );
	pTextureImg->SetMatrix( textureMatrix );

	// 매핑될 텍스처 이미지에 설정
	pImageRes->SetImage( pTextureImg );
	// 적용
	pMovie->ForceUpdateImages();
}

void GfxRenderTexture::GetGfxTextureSize( int& nWidth, int& nHeight )
{
	if( NULL == m_pTextureBase->m_pGfxTexture )
		return;

	ImageSize textureSize = m_pTextureBase->m_pGfxTexture->GetSize();

	nWidth = textureSize.Width;
	nHeight = textureSize.Height;
}
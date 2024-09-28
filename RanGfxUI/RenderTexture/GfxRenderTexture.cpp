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
	// �������� TextureManager::CreateTexture���� Dx�ؽ�ó AddRef �ϰ� �ν�Ʈ ����̽� �� ����� ���ϰ��ִ�.
	// ���⼭ ����������.
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

	// gfx �ؽ�ó ����
	if( NULL == m_pTextureBase->m_pGfxTexture )
	{
		m_pTextureBase->m_pGfxTexture = pManager->CreateTexture( pTexture,
			ImageSize( m_nRenderTextureWidth, m_nRenderTextureHeight ) );
	}
	// �ν�Ʈ ����̽����� ���� �ؽ�ó �������ϸ� TextureManager ���� ����̽����� �ؽ�ó �ʱ�ȭ �� ���´�.
	// �ؽ�ó �ٽ� �־�����.
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

	// main.swf���� ���ҽ� ������.
	GFx::Resource* pRes = pMovie->GetMovieDef()->GetResource( m_strLinkID.c_str() );
	if( NULL == pRes )
		return;

	// ������ ���ҽ��� �̹������� Ȯ��.
	ImageResource* pImageRes = NULL;
	if( Resource::RT_Image == pRes->GetResourceType() )
		pImageRes = (ImageResource*) pRes;

	if( NULL == pImageRes )
		return;

	// ���� �̹���
	ImageBase* pImageOrig = pImageRes->GetImage();
	if( NULL == pImageOrig )
		return;

	Ptr< TextureImage > pTextureImg = *new TextureImage( Image_R8G8B8,
		m_pTextureBase->m_pGfxTexture->GetSize(), 0, m_pTextureBase->m_pGfxTexture );

	//  [7/26/2016 gbgim];
	// �Ʒ� ��� ���� ����!;
	// Release�� ������ swf������ ����ϴ� ���ҽ����� �ϳ��� dds�� ��ŷ�Ǵµ�;
	// ����� �ϳ��� dds���� �о�´�; ��ġ�ʴ� ����� ��������;
	// �׸��� ���� �ؽ��ĸ� ReleaseD���� ���� ���������, Release������ �ϳ��� �����Ǵ� ���ϴ�;
	// ���� �̹��� ������ ������.
	/*
	if( true == m_pTextureBase->m_OrigImgSize.IsEmpty() )
	{
		m_pTextureBase->m_OrigImgSize = pImageOrig->GetSize();
	}
	*/

	// gfx �ؽ�ó ������
	ImageSize newSize = m_pTextureBase->m_pGfxTexture->GetSize();

	// ���� �̹����� �ؽ�ó ������ ���� ���
	Size< float > sfParam( (float) m_pTextureBase->m_OrigImgSize.Width / newSize.Width,
						   (float) m_pTextureBase->m_OrigImgSize.Height / newSize.Height );

	// ��Ⱦ�� ���
	m_fAspect = sfParam.Width / sfParam.Height;

	Scaleform::Render::HAL* pRenderHAL = GetCore()->GetRenderHAL();
	Matrix2F& vptMatrix = pRenderHAL->GetMatrices()->GetFullViewportMatrix( m_pTextureBase->m_OrigImgSize );

	// �ؽ�ó ��� ��� �� ����
	bool invertTexture = vptMatrix.M[ 1 ][ 1 ] > 0.0f;

	Matrix2F textureMatrix = Matrix2F::Scaling( sfParam.Width, sfParam.Height * (invertTexture ? -1.0f : 1.0f) );
	pTextureImg->SetMatrix( textureMatrix );

	// ���ε� �ؽ�ó �̹����� ����
	pImageRes->SetImage( pTextureImg );
	// ����
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
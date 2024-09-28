#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxDynamicVB.h"

#include "../Frame/DxEffectMan.h"
#include "../NSEffectThread.h"

#include "./Thread/GroundForThread.h"
#include "./DxEffSingleMan.h"

#include "./DxEffectGround.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------[PARTICLESYS_PROPERTY]
const DWORD	GROUND_PROPERTY::VERSION	= 0x0102;
const char	GROUND_PROPERTY::NAME[]	= "����ȿ��-ĳ��";

DxEffSingle* GROUND_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectGround *pEffect = new DxEffectGround;

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;

	//	Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fAniTime = m_fAniTime;
	pEffect->m_iCol = m_iCol;
	pEffect->m_iRow = m_iRow;

	pEffect->m_fRotateAngel = m_fRotateAngel;

	//	Note : ���� �ʱ�ȭ
	pEffect->m_fHeight = m_fHeightStart;
	pEffect->m_fHeightStart = m_fHeightStart;

	pEffect->m_fHeightTime1 = m_fGBeginTime + m_fGLifeTime*m_fHeightRate1/100.f;
	pEffect->m_fHeightTime2 = m_fGBeginTime + m_fGLifeTime*m_fHeightRate2/100.f;

	if ( m_fHeightRate1 == 0.f )				pEffect->m_fHeightDelta1	= 0.f;
	else										pEffect->m_fHeightDelta1 = ( m_fHeightMid1 - m_fHeightStart ) / (pEffect->m_fHeightTime1-m_fGBeginTime);
	if ( m_fHeightRate2-m_fHeightRate1 == 0.f )	pEffect->m_fHeightDelta2	= 0.f;
	else										pEffect->m_fHeightDelta2 = ( m_fHeightMid2 - m_fHeightMid1 ) / (pEffect->m_fHeightTime2-pEffect->m_fHeightTime1);
	if ( m_fHeightRate2 == 100.f )				pEffect->m_fHeightDelta3	= 0.f;
	else										pEffect->m_fHeightDelta3 = ( m_fHeightEnd - m_fHeightMid2 ) / (pEffect->m_fGBeginTime+m_fGLifeTime-pEffect->m_fHeightTime2);

	//	Note : ����(������) �ʱ�ȭ
	pEffect->m_fSize = m_fSizeStart/2;
	pEffect->m_fSizeStart = m_fSizeStart/2;

	pEffect->m_fSizeTime1 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate1/100.f;
	pEffect->m_fSizeTime2 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate2/100.f;

	if ( m_fSizeRate1 == 0.f )				pEffect->m_fSizeDelta1	= 0.f;
	else									pEffect->m_fSizeDelta1 = ( m_fSizeMid1 - m_fSizeStart ) / (pEffect->m_fSizeTime1-m_fGBeginTime);
	if ( m_fSizeRate2-m_fSizeRate1 == 0.f )	pEffect->m_fSizeDelta2	= 0.f;
	else									pEffect->m_fSizeDelta2 = ( m_fSizeMid2 - m_fSizeMid1 ) / (pEffect->m_fSizeTime2-pEffect->m_fSizeTime1);
	if ( m_fSizeRate2 == 100.f )			pEffect->m_fSizeDelta3	= 0.f;
	else									pEffect->m_fSizeDelta3 = ( m_fSizeEnd - m_fSizeMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fSizeTime2);

	//	Note : ���� �ʱ�ȭ
	pEffect->m_fAlpha = m_fAlphaStart;
	pEffect->m_fAlphaStart = m_fAlphaStart;

	pEffect->m_fAlphaTime1 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate1/100.f;
	pEffect->m_fAlphaTime2 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate2/100.f;

	if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
	else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1-m_fGBeginTime);
	if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
	else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
	else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fAlphaTime2);

	pEffect->m_cColorStart = m_cColorStart;
	pEffect->m_cColorVar = m_cColorVar;
	pEffect->m_cColorEnd = m_cColorEnd;

	//	Note : ��������Ʈ ���� �ʱ�ȭ
	pEffect->m_iAllSprite = m_iCol * m_iRow;

	pEffect->m_nBlend = m_nBlend;

	//	Note : �ؽ��� ����.
	pEffect->m_strTexture = m_szTexture;

	// DxEffSingle �� �ִ� m_textureRes �� �ε��Ǿ��� �������,
	// EFF_PROPERTY �� �ִ� m_textureRes �� �ε��Ǿ� ���� ���� ���.
	if ( !m_textureRes.Update() )
	{
		m_textureRes = TextureManager::GetInstance().LoadTexture(
			m_szTexture,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT,
			true,
			m_bBackUpCache );
	}

	pEffect->m_textureRes = m_textureRes;

	//	Note : ���� ����
	pEffect->m_MovSound = m_MovSound;

	// Thread �� ���� �۾��� �Ѵ�.
	if ( m_dwFlag & USEPICKING )
	{
		pEffect->CreateSingleForThread( NSEffectThread::CreateGround( spThisGroup, pEffect ) );
	}

	return pEffect;
}

HRESULT GROUND_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT GROUND_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_textureRes.Clear();
	}
	else if( dwVer == 0x0101 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_textureRes.Clear();
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		PROPERTY_100 pProp;
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_100) );
		

		m_Property.m_fAniTime		= pProp.m_fAniTime;		
		m_Property.m_iCol			= pProp.m_iCol;
		m_Property.m_iRow			= pProp.m_iRow;
		m_Property.m_fRotateAngel	= pProp.m_fRotateAngel;
		m_Property.m_fHeightRate1	= pProp.m_fHeightRate1;
		m_Property.m_fHeightRate2	= pProp.m_fHeightRate2;
		m_Property.m_fHeightStart	= pProp.m_fHeightStart;
		m_Property.m_fHeightMid1	= pProp.m_fHeightMid1;
		m_Property.m_fHeightMid2	= pProp.m_fHeightMid2;
		m_Property.m_fHeightEnd		= pProp.m_fHeightEnd;
		m_Property.m_fSizeRate1		= pProp.m_fSizeRate1;
		m_Property.m_fSizeRate2		= pProp.m_fSizeRate2;
		m_Property.m_fSizeStart		= pProp.m_fSizeStart;
		m_Property.m_fSizeMid1		= pProp.m_fSizeMid1;
		m_Property.m_fSizeMid2		= pProp.m_fSizeMid2;
		m_Property.m_fSizeEnd		= pProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1	= pProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2	= pProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart	= pProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1		= pProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2		= pProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd		= pProp.m_fAlphaEnd;
		m_Property.m_cColorStart	= pProp.m_cColorStart;
		m_Property.m_cColorVar		= pProp.m_cColorVar;
		m_Property.m_cColorEnd		= pProp.m_cColorEnd;
		m_Property.m_nBlend			= pProp.m_nBlend;

		StringCchCopy( m_Property.m_szTexture, 256, pProp.m_szTexture );

		if ( pProp.m_bAni )			m_Property.m_dwFlag		|= USEANI;
		if ( pProp.m_bRotateUse )	m_Property.m_dwFlag		|= USEROTATE;
		if ( pProp.m_bHeight )		m_Property.m_dwFlag		|= USEHEIGHT;
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

// Decal �� ����� ���� Ground �� ���� �����ش�.
HRESULT GROUND_PROPERTY::LoadFile_DecalProp ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	DECAL_PROPERTY::PROPERTY sDecalProp;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == 0x0101 )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );

		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &sDecalProp, sizeof(DECAL_PROPERTY::PROPERTY) );

		m_textureRes.Clear();
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );

		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &sDecalProp, sizeof(DECAL_PROPERTY::PROPERTY) );

		m_textureRes.Clear();
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	m_Property.m_dwFlag			= sDecalProp.m_dwFlag & 0x00020004;
	m_Property.m_dwFlag			|= USEPICKING;

	if ( sDecalProp.m_dwFlag & 0x00010000 )	// DECAL_PROPERTY _ USESEQUENCE
	{
		m_Property.m_dwFlag			|= USEANI;
	}

	m_Property.m_dwFlag			|= USE_SOURCE_DECAL;

	//sDecalProp.m_vArea;

	m_Property.m_fAniTime		= sDecalProp.m_fAniTime;
	m_Property.m_iCol			= sDecalProp.m_iCol;
	m_Property.m_iRow			= sDecalProp.m_iRow;

	m_Property.m_fRotateAngel	= sDecalProp.m_fRotateAngel * 180.f/D3DX_PI;

	m_Property.m_fHeightRate1	= 25.f;
	m_Property.m_fHeightRate2	= 75.f;

	m_Property.m_fHeightStart	= 0.f;
	m_Property.m_fHeightMid1	= 0.f;
	m_Property.m_fHeightMid2	= 0.f;
	m_Property.m_fHeightEnd		= 0.f;

	m_Property.m_fSizeRate1		= sDecalProp.m_fSizeRate1;
	m_Property.m_fSizeRate2		= sDecalProp.m_fSizeRate2;

	if ( sDecalProp.m_dwFlag & 0x00000200 ) // DECAL_PROPERTY _ USETEXSCALE
	{
		m_Property.m_fSizeStart		= sDecalProp.m_fSizeStart * sDecalProp.m_vArea.x;	// ���� x �� z �� �ٸ��� �ִٸ� ����.
		m_Property.m_fSizeMid1		= sDecalProp.m_fSizeMid1 * sDecalProp.m_vArea.x;
		m_Property.m_fSizeMid2		= sDecalProp.m_fSizeMid2 * sDecalProp.m_vArea.x;
		m_Property.m_fSizeEnd		= sDecalProp.m_fSizeEnd * sDecalProp.m_vArea.x;
	}
	else
	{
		m_Property.m_fSizeStart		= sDecalProp.m_vArea.x;	// ���� x �� z �� �ٸ��� �ִٸ� ����.
		m_Property.m_fSizeMid1		= sDecalProp.m_vArea.x;
		m_Property.m_fSizeMid2		= sDecalProp.m_vArea.x;
		m_Property.m_fSizeEnd		= sDecalProp.m_vArea.x;
	}

	m_Property.m_fAlphaRate1	= sDecalProp.m_fAlphaRate1;
	m_Property.m_fAlphaRate2	= sDecalProp.m_fAlphaRate2;

	m_Property.m_fAlphaStart	= sDecalProp.m_fAlphaStart;
	m_Property.m_fAlphaMid1		= sDecalProp.m_fAlphaMid1;
	m_Property.m_fAlphaMid2		= sDecalProp.m_fAlphaMid2;
	m_Property.m_fAlphaEnd		= sDecalProp.m_fAlphaEnd;

	m_Property.m_cColorStart	= sDecalProp.m_cColorStart;
	m_Property.m_cColorVar		= sDecalProp.m_cColorVar;
	m_Property.m_cColorEnd		= sDecalProp.m_cColorEnd;

	m_Property.m_nBlend			= sDecalProp.m_nBlend;

	StringCchCopy( m_Property.m_szTexture, 256, sDecalProp.m_szTexture );
	

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, FALSE );

	return S_OK;
}

HRESULT GROUND_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	m_bBackUpCache = bBackUpCache;
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		true,
		bBackUpCache );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT GROUND_PROPERTY::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectGround::TYPEID		= EFFSINGLE_GROUND;
const DWORD	DxEffectGround::FLAG		= NULL;
const char	DxEffectGround::NAME[]		= "����ȿ��-ĳ��";

LPDIRECT3DSTATEBLOCK9		DxEffectGround::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectGround::m_pDrawRenderStats = NULL;

BOOL DxEffectGround:: VAILEDDEVICE				= FALSE;

DxEffectGround::DxEffectGround(void) :
	m_vGLocation(0,0,0),
	m_nBlend(1),
	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),

	m_bRotateUse(FALSE),
	m_fRotateAngel(0.f),
	m_fRotateAngelSum(0.f),

	m_fHeightTime1(0.f),
	m_fHeightTime2(0.f),
	m_fHeightDelta1(0.f),
	m_fHeightDelta2(0.f),
	m_fHeightDelta3(0.f),

	m_fSizeTime1(0.f),
	m_fSizeTime2(0.f),
	m_fSizeDelta1(0.f),
	m_fSizeDelta2(0.f),
	m_fSizeDelta3(0.f),

	m_fAlphaDelta1(0.f),
	m_fAlphaDelta2(0.f),
	m_fAlphaDelta3(0.f),
	m_fAlphaTime1(0.f),
	m_fAlphaTime2(0.f),

	m_cColorDelta(0.f,0.f,0.f,0.f),

	m_bSequenceRoop(FALSE),
	m_vPrevPicking(0.f,0.f,0.f)

	, m_pd3dDevice(NULL)
{
	m_fAniTime = 0.f;
	m_iCol = 1;
	m_iRow = 1;

	m_fHeight	= 0.f;

	m_fSize		= 0.f;

	m_fAlpha	= 0.f;

	m_cColorStart.r		= 0.0f;
	m_cColorStart.g		= 0.0f;
	m_cColorStart.b		= 0.0f;
	m_cColorStart.a		= 0.4f;
			
	m_cColorVar.r		= 0.0f;
	m_cColorVar.g		= 0.0f;
	m_cColorVar.b		= 0.0f;
	m_cColorVar.a		= 0.0f;

	m_cColorEnd.r		= 0.0f;
	m_cColorEnd.g		= 0.0f;
	m_cColorEnd.b		= 0.0f;
	m_cColorEnd.a		= 0.0f;

	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectGround::~DxEffectGround(void)
{

}

HRESULT DxEffectGround::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	VAILEDDEVICE = TRUE;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffectGround::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectGround::OnInitDevice_STATIC()
{
}

void DxEffectGround::OnDeleteDevice_STATIC()
{
}

void DxEffectGround::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos;
	for( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos, &m_pGroundVertex[i].vPos, &m_matWorld );

		if( vMax.x < vPos.x )	vMax.x = vPos.x;
		if( vMax.y < vPos.y )	vMax.y = vPos.y;
		if( vMax.z < vPos.z )	vMax.z = vPos.z;

		if( vMin.x > vPos.x )	vMin.x = vPos.x;
		if( vMin.y > vPos.y )	vMin.y = vPos.y;
		if( vMin.z > vPos.z )	vMin.z = vPos.z;
	}

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectGround::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : �ʱ� ��ġ ����
	//
	float TempXZ = m_fSize/2;
	m_pGroundVertex[0].vPos = D3DXVECTOR3 ( -TempXZ, m_fHeight, TempXZ );
	m_pGroundVertex[1].vPos = D3DXVECTOR3 ( TempXZ, m_fHeight, TempXZ );
	m_pGroundVertex[2].vPos = D3DXVECTOR3 ( -TempXZ, m_fHeight, -TempXZ );
	m_pGroundVertex[3].vPos = D3DXVECTOR3 ( TempXZ, m_fHeight, -TempXZ );

	//	Note : �÷��� �ʱ�ȭ
	//
	m_cColorDelta.a = 0.f;
	m_cColorDelta.r = ( m_cColorEnd.r - m_cColorStart.r ) / m_fGLifeTime;
	m_cColorDelta.g = ( m_cColorEnd.g - m_cColorStart.g ) / m_fGLifeTime;
	m_cColorDelta.b = ( m_cColorEnd.b - m_cColorStart.b ) / m_fGLifeTime;

	m_pGroundVertex[0].dwColor = m_pGroundVertex[1].dwColor = m_pGroundVertex[2].dwColor = 
	m_pGroundVertex[3].dwColor = m_cColorStart;
	m_dwColor = m_cColorStart;

	m_pGroundVertex[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	m_pGroundVertex[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
	m_pGroundVertex[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	m_pGroundVertex[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );

	// 
	memset( &m_sMaterial, 0, sizeof(D3DMATERIAL9) );

	//	Note : ����, �ڽ� ȣ��
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectGround::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectGround::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fSize		= m_fSizeStart;
	m_fAlpha	= m_fAlphaStart;
	m_fHeight	= m_fHeightStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectGround::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;
	m_fTimeSum += fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fSize		= m_fSizeStart;
			m_fAlpha	= m_fAlphaStart;
			m_fHeight	= m_fHeightStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	//	Note : ȸ�� ����
	//
	if ( m_dwFlag & USEROTATE )	m_fRotateAngelSum += m_fRotateAngel * fElapsedTime;

	//	Note : ����(������) ����
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
		m_fSize += (m_fSizeDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)	
		m_fSize += (m_fSizeDelta2*fElapsedTime)/2;
	else														
		m_fSize += (m_fSizeDelta3*fElapsedTime)/2;

	if ( m_fSize < 0.f )		m_fSize = 0.f;

	m_pGroundVertex[0].vPos = D3DXVECTOR3 ( -m_fSize, 0.f, m_fSize );
	m_pGroundVertex[1].vPos = D3DXVECTOR3 ( m_fSize, 0.f, m_fSize );
	m_pGroundVertex[2].vPos = D3DXVECTOR3 ( -m_fSize, 0.f, -m_fSize );
	m_pGroundVertex[3].vPos = D3DXVECTOR3 ( m_fSize, 0.f, -m_fSize );

	//	Note : ���� ����
	//
	if ( m_dwFlag & USEHEIGHT )
	{
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fHeightTime1)
			m_fHeight += m_fHeightDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fHeightTime1 && m_fGAge < m_fHeightTime2)	
			m_fHeight += m_fHeightDelta2*fElapsedTime;
		else																
			m_fHeight += m_fHeightDelta3*fElapsedTime;
	}

	//	Note : �÷� ���� 
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
		m_fAlpha += m_fAlphaDelta1*fElapsedTime;
	else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
		m_fAlpha += m_fAlphaDelta2*fElapsedTime;
	else														
		m_fAlpha += m_fAlphaDelta3*fElapsedTime;

	if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
	if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

	m_cColorVar = m_cColorStart;
	m_cColorVar.a = m_fAlpha;	

	//m_pGroundVertex[0].Diffuse = m_pGroundVertex[1].Diffuse = m_pGroundVertex[2].Diffuse = 
	//m_pGroundVertex[3].Diffuse = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);
	m_dwColor = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);

	//	Note : ��������Ʈ
	//
	if ( m_dwFlag & USEANI )
	{
		if ( m_dwFlag & USESEQUENCELOOP )
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				if ( m_bSequenceRoop )	m_iNowSprite += 1;
				else					m_iNowSprite -= 1;
				m_fTimeSum = 0.0f;
			}

			if ( m_iNowSprite >= m_iAllSprite )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite -= 2;
			}
			else if ( m_iNowSprite < 0 )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite += 2;
			}
		}
		else
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_iNowSprite += 1;
				m_fTimeSum = 0.0f;
			}
			if ( m_iNowSprite >= m_iAllSprite )		m_iNowSprite = 0;
		}

		m_pGroundVertex[0].vTex.x = m_iNowSprite / (float)m_iCol;
		m_pGroundVertex[0].vTex.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_pGroundVertex[1].vTex.x = (m_iNowSprite+1) / (float)m_iCol;
		m_pGroundVertex[1].vTex.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_pGroundVertex[2].vTex.x = m_iNowSprite / (float)m_iCol;
		m_pGroundVertex[2].vTex.y = ((m_iNowSprite) / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;

		m_pGroundVertex[3].vTex.x = (m_iNowSprite+1) / (float)m_iCol;
		m_pGroundVertex[3].vTex.y = ((m_iNowSprite) / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;
	}
	else
	{
		m_pGroundVertex[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		m_pGroundVertex[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		m_pGroundVertex[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		m_pGroundVertex[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}

	if ( m_dwFlag & USE_SOURCE_DECAL )
	{
		D3DXVECTOR2 vTemp2 = m_pGroundVertex[0].vTex;
		m_pGroundVertex[0].vTex = m_pGroundVertex[1].vTex;
		m_pGroundVertex[1].vTex = m_pGroundVertex[3].vTex;
		m_pGroundVertex[3].vTex = m_pGroundVertex[2].vTex;
		m_pGroundVertex[2].vTex = vTemp2;
	}

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectGround::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matRotate, matTrans, matChild;

	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	matWorld = m_matLocal;

	//	Note : ���̰� ���Ѵ�
	if ( m_dwFlag & USEPICKING )	// ���鿡 ��ġ�� �����ȴ�.
	{
		m_spSingleForThread->SetRenderCullingTRUE();

		SingleForThread* pSingleForThread = m_spSingleForThread.get();
		GroundForThread* pGroundForThread = MIN_BOOST::polymorphic_downcast<GroundForThread*>( pSingleForThread );

		if ( m_dwFlag & USEROTATE )			//	Note : ȸ�� ����
		{
			D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
			//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
			D3DXMatrixMultiply_RM2( matRotate, matWorld );
		}

		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += 0.3f;			// ���̸� �ǵ��� ���ϵ��� �Ѵ�.

		pGroundForThread->SetCheckPoint( D3DXVECTOR3(matChild._41,matChild._42,	matChild._43) );
		if ( pGroundForThread->IsCollisionOK() )
		{
			matChild._42 = pGroundForThread->GetCllisionY();
		}
		else
		{
			// �浹ó���� �ȵǾ����� ������ ���� �ʵ��� �Ѵ�.
			goto _RETURN;
		}
	}
	else if ( m_dwFlag & USEHEIGHT )
	{
		if ( m_dwFlag & USEROTATE )			//	Note : ȸ�� ����
		{
			D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
			//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
			D3DXMatrixMultiply_RM2( matRotate, matWorld );
		}

		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += (m_fHeight+0.3f);
	}
	else	// ���̰� ������ �ʴ´�.
	{
		if ( m_dwFlag & USEROTATE )			//	Note : ȸ�� ����
		{
			D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
			//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
			D3DXMatrixMultiply_RM2( matRotate, matWorld );
		}

		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += 0.3f;			// ���̸� �ǵ��� ���ϵ��� �Ѵ�.
	}

	m_matWorld = matChild;


	if ( !m_rThisGroup->m_bOut_Particle )	goto _RETURN;
	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	if ( m_fSize <= 0.f || m_fAlpha <= 0.f)	goto _RETURN;

	// Note : Get Texture
	if( !m_textureRes.GetTexture() )
        goto _RETURN;

	// Note : OptimizeSequence�� ����ϱ� ���� �ʱ� �۾�.
	OPTMSingleGround::DATA* pData = OPTMManager::GetInstance().m_sSingleGround.GetData();

	pData->matWorld = matChild;

	if( m_dwFlag & USENORMAL )
	{
		pData->sLIGHTING.SetValue( TRUE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE );
	}

	//	Note : ���� ��� ����
	switch ( m_nBlend )
	{
	case 1 :
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		break;
	case 2:
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE2X );
		break;
	case 3:
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE4X );
		break;
	case 5:
		pData->sZWRITEENABLE.SetValue( TRUE );
		pData->sALPHABLENDENABLE.SetValue( FALSE );
		pData->sALPHAREF.SetValue( 0x80 );

		break;
	}

	if ( m_dwFlag & USENORMAL )
	{
		for ( DWORD i=0; i<4; i++ )
		{
			m_pGroundVertNor[i].vPos	= m_pGroundVertex[i].vPos;
			m_pGroundVertNor[i].vNor	= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
			m_pGroundVertNor[i].vTex	= m_pGroundVertex[i].vTex;
		}

		m_sMaterial.Diffuse.a = (float)((m_dwColor&0xff000000)>>24)*DIV_1_255*fAlpha;
		m_sMaterial.Diffuse.r = (float)((m_dwColor&0x00ff0000)>>16)*DIV_1_255;
		m_sMaterial.Diffuse.g = (float)((m_dwColor&0x0000ff00)>>8)*DIV_1_255;
		m_sMaterial.Diffuse.b = (float)((m_dwColor&0x000000ff))*DIV_1_255;

		DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_PNT.GetFVFSize();
        if( !DxDynamicVB::g_sVB_PNT.IsValidSize( dwVertexSizeFULL ) )
        {
            OPTMManager::GetInstance().m_sSingleGround.ReleaseData( pData );
            goto _RETURN;
        }

        if( !DxDynamicVB::g_sVB_PNT.IsValidCopy( dwVertexSizeFULL ) )
		    OPTMManager::GetInstance().Render();

	    UINT nVertexCount = DxDynamicVB::g_sVB_PNT.CopyToVB(
		    m_pGroundVertNor,
		    dwVertexSizeFULL,
		    4 );

		// Note : �����͸� �ִ´�.
		pData->pTex = m_textureRes.GetTexture();
        pData->nVertexCount = nVertexCount;
		pData->nFaceCount = 2;
		pData->dwFVF = DxDynamicVB::g_sVB_PNT.dwFVF;
		pData->pMaterial = &m_sMaterial;

		// Note : ���߿� �Ѹ����� ���� �� ���´�.
		OPTMManager::GetInstance().m_sSingleGround.InsertData( pData );
	}
	else
	{
        // �ܺ� ���İ� ����
        DWORD dwColor = m_dwColor&0xffffff;
        DWORD dwAlpha = static_cast<DWORD>( ((m_dwColor&0xff000000)>>24)*fAlpha );
        dwColor += dwAlpha<<24;

		// �� ����
		m_pGroundVertex[0].dwColor = m_pGroundVertex[1].dwColor = m_pGroundVertex[2].dwColor = 
		m_pGroundVertex[3].dwColor = dwColor;

		DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_PDT.GetFVFSize();
        if( !DxDynamicVB::g_sVB_PDT.IsValidSize( dwVertexSizeFULL ) )
        {
            OPTMManager::GetInstance().m_sSingleGround.ReleaseData( pData );
            goto _RETURN;
        }

        if( !DxDynamicVB::g_sVB_PDT.IsValidCopy( dwVertexSizeFULL ) )
		    OPTMManager::GetInstance().Render();

        UINT nVertexCount = DxDynamicVB::g_sVB_PDT.CopyToVB(
		    m_pGroundVertex,
		    dwVertexSizeFULL,
		    4 );

		// Note : �����͸� �ִ´�.
		pData->pTex = m_textureRes.GetTexture();
        pData->nVertexCount = nVertexCount;
		pData->nFaceCount = 2;
		pData->dwFVF = DxDynamicVB::g_sVB_PDT.dwFVF;

		// Note : ���߿� �Ѹ����� ���� �� ���´�.
		OPTMManager::GetInstance().m_sSingleGround.InsertData( pData );
	}

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matChild, fAlpha  );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return S_OK;
}

HRESULT DxEffectGround::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

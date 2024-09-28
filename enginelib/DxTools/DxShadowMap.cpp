#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../G-Logic/glperiod.h"

#include "../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../DxMeshs/SkinMesh/DxVehicle.h"
#include "DxViewPort.h"
#include "./Light/NSShadowLight.h"
#include "./Light/DxShadowLight.h"
#include "./Light/DxLightMan.h"
#include "./TextureManager.h"
#include "./RENDERPARAM.h"

#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "dxshadowmap.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxShadowMap& DxShadowMap::GetInstance()
{
	static DxShadowMap Instance;
	return Instance;
}

const DWORD					DxShadowMap::VERTEX::FVF			= D3DFVF_XYZ|D3DFVF_TEX1;
const float					DxShadowMap::m_fRange				= 12.f;
const float					DxShadowMap::EYE_TO_LOOKAT			= 220.f;
LPDIRECT3DVERTEXBUFFERQ		DxShadowMap::m_pShadowVB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedStateBlock		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pEffectStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedCircleSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pEffectCircleSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedOneShadowSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pDrawOneShadowSB		= NULL;

DxShadowMap::DxShadowMap() :
	m_dwUseSwShader(D3DUSAGE_SOFTWAREPROCESSING),
	m_bUseMIXED(TRUE),

	m_emShadowDetail(LAND_QT_HIGH_HIGH),	// Shadow	������						- �ְ� 0, �߻�1, �߰� 2, ���� 3
	m_fTexWidth(512.f),
	m_fTexHeight(512.f),

	m_fDistanceRate(2.0f),

	m_vBaseViewMin(0.f,0.f,0.f),
	m_vBaseViewMax(0.f,0.f,0.f),
	m_vViewMin(0.f,0.f,0.f),
	m_vViewMax(0.f,0.f,0.f),
	m_fDistance(0.f), 

	m_fPointDis1(0.f),
	m_nPointNum1(0),
	m_fPointDis2(0.f),
	m_nPointNum2(0),

	m_bCharUse(TRUE),
	m_bImageBlurUse(TRUE)

	, m_dwSpotLightID_PL1(-1)
	, m_dwSpotLightID_PL2(-1)

	, m_pFX(NULL)
{
	D3DXMatrixIdentity ( &m_matView );
	D3DXMatrixIdentity ( &m_matProj );

	D3DXMatrixIdentity ( &m_matLastWVP_OLD );
	D3DXMatrixIdentity ( &m_matLastWVP_D );
	D3DXMatrixIdentity ( &m_matLastWVP_D_2nd );
	D3DXMatrixIdentity ( &m_matLastWVP_PL1 );
	D3DXMatrixIdentity ( &m_matLastWVP_PL2 );
}

DxShadowMap::~DxShadowMap()
{
}

HRESULT DxShadowMap::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );


	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		m_bUseMIXED = FALSE;
	}

	//	Note : Vertex Shader
	//
	if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 0) )
	{
		m_dwUseSwShader = 0;	// �̷� �� MIXED -> H/W ���� �̴�.
	}

	////	Note : �׷��� ī�� ���� ���� �ؽ��� ���� 
	////
	//m_dwTexDetail = (d3dCaps.MaxTextureWidth < m_dwTexDetail) ? d3dCaps.MaxTextureWidth : m_dwTexDetail;
	//m_dwTexDetail = (d3dCaps.MaxTextureHeight < m_dwTexDetail) ? d3dCaps.MaxTextureHeight : m_dwTexDetail;

	//	Note : Z ���ۿ� Texture�� ��� �� �� �ֳ� ������ �׽�Ʈ �ϰ� 
	//			��밡���ϸ� TRUE �� �Ұ��� �ϸ� FALSE �� ȣ���Ѵ�.
	//
	HRESULT hr;
	BOOL		bUseZTexture = TRUE;
	hr = pD3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
									d3dCaps.DeviceType, d3dsdBackBuffer.Format,
									D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE,
									D3DFMT_D24S8 );
	if ( FAILED(hr) )	bUseZTexture = FALSE;

	//	Note : �׷��� ī���� ������ ���� ����
	//
	if ( !m_dwUseSwShader && d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 0) && bUseZTexture )
	{
		m_emShadowDetail = LAND_QT_HIGH_HIGH;													// �ְ� - 2��
	}
	else if ( d3dCreateParam.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
	{
		m_emShadowDetail = LAND_QT_LOW_LOW;												// ����	- �ȳ���
	}
	else
	{
		m_emShadowDetail = LAND_QT_HIGH;													// ���� - 1��
	}

	//	Note : �� �׸��ڸ� �����Ѵ�.
	//
	m_szCircleTex = "Circle_Shadow.dds";
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szCircleTex,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_ETC,
		false,
		TRUE );

    //TextureManager::GetInstance().BackUpCache( m_textureRes );

	SetFX( pd3dDevice );

	return S_OK;
}

HRESULT DxShadowMap::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	SAFE_RELEASE ( m_pShadowVB );
	hr = pd3dDevice->CreateVertexBuffer ( 4*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &m_pShadowVB, NULL );
	if( FAILED(hr) )
	{
        CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- CreateVertexBuffer() -- Failed"));
		return E_FAIL;
	}

	if ( m_pShadowVB )
	{
		VERTEX*	pVertex;
		hr = m_pShadowVB->Lock ( 0, 0, (VOID**)&pVertex, 0 );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxShadowMap::RestoreDeviceObjects() -- Lock() -- Failed"));
			return E_FAIL;
		}

		pVertex[0].p = D3DXVECTOR3 ( -m_fRange, 0.f, m_fRange );
		pVertex[1].p = D3DXVECTOR3 ( m_fRange, 0.f, m_fRange );
		pVertex[2].p = D3DXVECTOR3 ( -m_fRange, 0.f, -m_fRange );
		pVertex[3].p = D3DXVECTOR3 ( m_fRange, 0.f, -m_fRange );
		pVertex[0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertex[1].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertex[2].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertex[3].t = D3DXVECTOR2 ( 1.f, 1.f );

		m_pShadowVB->Unlock ();
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR  );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );	//D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );	//D3DBLEND_SRCCOLOR );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR  );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCircleSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectCircleSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CW );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedOneShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawOneShadowSB );
	}

	//	Note : ����� ���Ƽ� .. 1�� �� ���ش�.
	//
	DxSurfaceTex::GetInstance().InitViewProjection ( m_vBaseViewMax, m_vBaseViewMin, m_matProj, EYE_TO_LOOKAT, m_fTexWidth, m_fTexHeight );

	if ( m_pFX )
	{
		m_pFX->OnResetDevice();
	}

	return S_OK;
}

HRESULT DxShadowMap::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE ( m_pShadowVB );
	
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	SAFE_RELEASE( m_pSavedCircleSB );
	SAFE_RELEASE( m_pEffectCircleSB );
	SAFE_RELEASE( m_pSavedOneShadowSB );
	SAFE_RELEASE( m_pDrawOneShadowSB );

	if ( m_pFX )
	{
		m_pFX->OnLostDevice();
	}

	return S_OK;
}

HRESULT DxShadowMap::DeleteDeviceObjects()
{
	m_textureRes.Clear();

	SAFE_RELEASE ( m_pFX );

	return S_OK;
}

void DxShadowMap::SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ���۰Ÿ��� �ٿ��ִ� ȿ���� �ִ�.
	// �簢���� ���̸� ����, ���� ���������� �������ش�.
	TSTRING strFX = 
		"//-------------------------------------------------------------------------------------- \r\n"
		"// File: FrameMesh.fx \r\n"
		"//       shhan \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Global variables \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"float g_fShadowOffset=0.5f/1024.f; \r\n"
		"float g_fShadowOffset2=0.707f/1024.f; \r\n"
		"float g_fShadowOffsetONE=0.5f/1024.f; \r\n"
		//"float g_fShadowOffset=0.353f/1024.f; \r\n"
		//"float g_fShadowOffset2=0.5f/1024.f; \r\n"
		" \r\n"
		"texture g_BaseTexture; \r\n"
		"sampler BaseTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_BaseTexture); \r\n"
		" \r\n"
		"	MinFilter = Point; \r\n"
		"	MagFilter = Point; \r\n"
		//"	MipFilter = Point; \r\n"
		"	AddressU = Clamp; \r\n"
		"	AddressV = Clamp; \r\n"
		"}; \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS( float2 texCoord : TEXCOORD0 ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"   float4 vColor = tex2D( BaseTexSampler, texCoord ); \r\n"
		"   float vNewDepth = 1.f; \r\n"
		//"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffset2, 0) ).x; \r\n"
		//"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		//"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffset2, 0) ).x; \r\n"
		//"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		//"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(0, g_fShadowOffset2) ).x; \r\n"
		//"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		//"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(0, -g_fShadowOffset2) ).x; \r\n"
		//"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffset, g_fShadowOffset) ).x; \r\n"
		"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffset, g_fShadowOffset) ).x; \r\n"
		"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffset, -g_fShadowOffset) ).x; \r\n"
		"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		"   vNewDepth = tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffset, -g_fShadowOffset) ).x; \r\n"
		"   vColor.x = (vColor.x>vNewDepth) ? vColor.x : vNewDepth; \r\n"
		" \r\n"
		"   return vColor; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS_VSM_INIT( float2 texCoord : TEXCOORD0 ) : COLOR0  \r\n"
		"{ \r\n"
		"   return float4(10000.f,0.f, 0.f, 1.f); \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS_VSM_BLUR_X( float2 texCoord : TEXCOORD0 ) : COLOR0  \r\n"
		"{ \r\n"
		"   float4 vColor = 0.f; \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffsetONE*1, 0.f) ); \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffsetONE*0, 0.f) ); \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffsetONE*1, 0.f) ); \r\n"

		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffsetONE*2, 0.f) ); \r\n"
		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffsetONE*2, 0.f) ); \r\n"

		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(-g_fShadowOffsetONE*3, 0.f) ); \r\n"
		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(g_fShadowOffsetONE*3, 0.f) ); \r\n"
		
		"   return vColor/3; \r\n"
		//"   return vColor/5; \r\n"
		//"   return vColor/7; \r\n"
		"} \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS_VSM_BLUR_Y( float2 texCoord : TEXCOORD0 ) : COLOR0  \r\n"
		"{ \r\n"
		"   float4 vColor = 0.f; \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, -g_fShadowOffsetONE*1) ); \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, g_fShadowOffsetONE*0) ); \r\n"
		"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, g_fShadowOffsetONE*1) ); \r\n"

		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, -g_fShadowOffsetONE*2) ); \r\n"
		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, g_fShadowOffsetONE*2) ); \r\n"

		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, -g_fShadowOffsetONE*3) ); \r\n"
		//"   vColor += tex2D( BaseTexSampler, texCoord + float2(0.f, g_fShadowOffsetONE*3) ); \r\n"

		"   return vColor/3; \r\n"
		//"   return vColor/5; \r\n"
		//"   return vColor/7; \r\n"
		"} \r\n"
		" \r\n"
		"technique Base \r\n"
		"{ \r\n"
			//-------------------------------------------------------------------------------------
			// �⺻ _ 0 - PS_2_0 �̻� ����
			// ���� ���� ������ ������ �Ѵ�. Z ���� ������ ���°���.
		"   pass p0 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS(); \r\n"
		"   } \r\n"
		"   pass p1 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS_VSM_INIT(); \r\n"
		"   } \r\n"
		"   pass p2 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS_VSM_BLUR_X(); \r\n"
		"   } \r\n"
		"   pass p3 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS_VSM_BLUR_Y(); \r\n"
		"   } \r\n"
		"} \r\n";

	SAFE_RELEASE( m_pFX );
	UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
	D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
}

HRESULT DxShadowMap::FrameMove ( float fTime, float fElapsedTime )
{
	m_bCharUse = TRUE;
	m_bImageBlurUse = TRUE;

	return S_OK;
}

void DxShadowMap::LastImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem )
{
	//if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	//{
	//	LastImageBlurForVSM
	//	( 
	//		pd3dDevice, 
	//		DxSurfaceTex::GetInstance().m_pShadowTexR32F_2048, 
	//		DxSurfaceTex::GetInstance().m_pShadowSufR32F_2048,
	//		DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM, 
	//		DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM,
	//		2048
	//	);
	//}
	//else
	//{
	//	LastImageBlurForVSM
	//	( 
	//		pd3dDevice, 
	//		DxSurfaceTex::GetInstance().m_pShadowTexG32R32F, 
	//		DxSurfaceTex::GetInstance().m_pShadowSufG32R32F,
	//		DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024, 
	//		DxSurfaceTex::GetInstance().m_pShadowSufR32F_1024,
	//		1024
	//	);
	//}

	

	if( !m_bImageBlurUse )
		return;

	if( m_emShadowDetail < LAND_QT_MIDDLE )
		return;

	// �����̰� PS.2.0 �̻��̸� ���� Shadow �� �۵����� �ʴ´�.
	if ( bUsedMaterialSystem && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
		return;

	if ( !DxSurfaceTex::GetInstance().m_pShadowSuf )
		return;

	//	Note : �̹����� ��������.
	{
        DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice );

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );

        D3DXImageBlur( pd3dDevice, DxSurfaceTex::GetInstance().m_pTempTex512, m_fTexWidth, m_fTexHeight );
	}

	m_bImageBlurUse = FALSE;
}

void DxShadowMap::LastImageBlurForVSM( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex, LPDIRECT3DSURFACEQ pSur, LPDIRECT3DTEXTUREQ pTexTemp, LPDIRECT3DSURFACEQ pSurTemp, DWORD dwSize )
{
	//	Note : �̹����� ��������.
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		DWORD dwFVF;
		pd3dDevice->GetFVF( &dwFVF );
		pd3dDevice->SetFVF( VERTEXRHW::FVF );

		DWORD dwFogEnable, dwZEnable, dwZWriteEnable;
		pd3dDevice->GetRenderState ( D3DRS_FOGENABLE, &dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->GetRenderState ( D3DRS_ZENABLE, &dwZEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE, FALSE );
		pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );

		// �̹����� �����Ű��..
		{
			float fMicroOffsetT = 0.5f/(float)dwSize;

			VERTEXRHW	sVertBase[4];
			sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,			1.f, 1.f );
			sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwSize,	0.f,			1.f, 1.f );
			sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwSize,	1.f, 1.f );
			sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwSize,	(float)dwSize,	1.f, 1.f );

			sVertBase[0].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetT, 0.f+fMicroOffsetT );
			sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetT, 0.f+fMicroOffsetT );
			sVertBase[2].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetT, 1.f+fMicroOffsetT );
			sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetT, 1.f+fMicroOffsetT );

			m_pFX->SetFloat( "g_fShadowOffsetONE", 1.f/static_cast<float>(dwSize) );


			//////////////////////////////////////////////////////////////////////////
			// 1
			pd3dDevice->SetRenderTarget( 0, pSurTemp );

			m_pFX->SetTexture( "g_BaseTexture", pTex );

			UINT numPasses;
			m_pFX->Begin( &numPasses, 0 );
			if( 2 < numPasses )
			{
				m_pFX->BeginPass( 2 );
				{
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
				}
				m_pFX->EndPass();
			}
			m_pFX->End();


			//////////////////////////////////////////////////////////////////////////
			// 2
			pd3dDevice->SetRenderTarget( 0, pSur );

			m_pFX->SetTexture( "g_BaseTexture", pTexTemp );

			m_pFX->Begin( &numPasses, 0 );
			if( 3 < numPasses )
			{
				m_pFX->BeginPass( 3 );
				{
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
				}
				m_pFX->EndPass();
			}
			m_pFX->End();
		}

		pd3dDevice->SetFVF( dwFVF );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE, dwZEnable );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, dwZWriteEnable );
	}
}

void DxShadowMap::LastImageBlurForCascadeShadow ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pShadowSufR32F, LPDIRECT3DTEXTUREQ pShadowTexSRC, DWORD dwSize )
{
	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	pd3dDevice->SetRenderTarget( 0, pShadowSufR32F );

	DWORD dwFVF;
	pd3dDevice->GetFVF( &dwFVF );
	pd3dDevice->SetFVF( VERTEXRHW::FVF );

	DWORD dwFogEnable, dwZEnable, dwZWriteEnable;
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->GetRenderState ( D3DRS_ZENABLE, &dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE, FALSE );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );

	// �̹����� �����Ű��..
	{
		float fMicroOffsetT = 0.5f/(float)dwSize;

		VERTEXRHW	sVertBase[4];
		sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,			1.f, 1.f );
		sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwSize,	0.f,			1.f, 1.f );
		sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwSize,	1.f, 1.f );
		sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwSize,	(float)dwSize,	1.f, 1.f );

		sVertBase[0].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetT, 0.f+fMicroOffsetT );
		sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetT, 0.f+fMicroOffsetT );
		sVertBase[2].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetT, 1.f+fMicroOffsetT );
		sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetT, 1.f+fMicroOffsetT );

		m_pFX->SetFloat( "g_fShadowOffset", 0.5f/static_cast<float>(dwSize) );
		m_pFX->SetTexture( "g_BaseTexture", pShadowTexSRC );

		UINT numPasses;
		m_pFX->Begin( &numPasses, 0 );
		if( 0 < numPasses )
		{
			m_pFX->BeginPass( 0 );
			{
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
			}
			m_pFX->EndPass();
		}
		m_pFX->End();
	}

	pd3dDevice->SetFVF( dwFVF );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZENABLE, dwZEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, dwZWriteEnable );
}

// Shadow �� �ʿ��� Matrix �� CV �� ���Ѵ�.
void DxShadowMap::CaculateViewProjForMaterialSystemALL()
{
	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		m_dwSpotLightID_PL1 = -1;
		m_dwSpotLightID_PL2 = -1;

		for ( DWORD i=0; i<NSShadowLight::GetLightNumber(); ++i )
		{
			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight( i );
			if ( i==0 )
			{
				if ( pSpotLight )
				{
					m_dwSpotLightID_PL1 = pSpotLight->m_dwID;
					CaculateViewProjForMaterialSystemSpotShadow( m_matLastWVP_PL1, m_sPerspectiveCV_PL1, pSpotLight->m_sLight );
				}
				else
				{
					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
					m_dwSpotLightID_PL1 = -1;
				}
			}
			else
			{
				if ( pSpotLight )
				{
					m_dwSpotLightID_PL2 = pSpotLight->m_dwID;
					CaculateViewProjForMaterialSystemSpotShadow( m_matLastWVP_PL2, m_sPerspectiveCV_PL2, pSpotLight->m_sLight );
				}
				else
				{
					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
					m_dwSpotLightID_PL2 = -1;
				}

			}
		}

		// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
		if ( NSShadowLight::IsCascadeShadow() )
		{
			float fDistance = GetCameraLengthCascadeShadow();

			//CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, 250.f );
			CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, fDistance );
			CaculateViewProjForMaterialSystemCascadeShadow2( m_matLastWVP_D_2nd, m_sOrthoCV_2nd, fDistance );
		}
		else
		{
			CaculateViewProjForMaterialSystem( 1.f );
			//m_bCharUse = FALSE;
			//return S_OK;
		}
	}
	else if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_NM )
	{
		// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		m_dwSpotLightID_PL1 = -1;
		m_dwSpotLightID_PL2 = -1;

		for ( DWORD i=0; i<NSShadowLight::GetLightNumber(); ++i )
		{
			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight( i );
			if ( i==0 )
			{
				if ( pSpotLight )
				{
					m_dwSpotLightID_PL1 = pSpotLight->m_dwID;
				}
				else
				{
					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
					m_dwSpotLightID_PL1 = -1;
				}
			}
			else
			{
				if ( pSpotLight )
				{
					m_dwSpotLightID_PL2 = pSpotLight->m_dwID;
				}
				else
				{
					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
					m_dwSpotLightID_PL2 = -1;
				}
			}
		}

		CaculateViewProjForMaterialSystem( 1.f );
	}
	else
	{
		CaculateViewProjForMaterialSystem( 1.f );
	}
}

HRESULT DxShadowMap::RenderShadowCharMob ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( !pSkinObj )
		return S_OK;

	if ( bWorldSystemOLD )
	{
		if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 && m_emShadowDetail >= LAND_QT_MIDDLE )	// Material ���� �ƴ� �������� ��� �׸��� ������ ����.
		{
			if ( m_bCharUse )	RenderShadow ( pSkinObj, matRot, pd3dDevice );
			else				RenderShadowMob ( pSkinObj, matRot, pd3dDevice );
		}
	}
	else
	{
		//if ( m_bCharUse )
		//{
		//	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
		//	{
		//		// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//		m_dwSpotLightID_PL1 = -1;
		//		m_dwSpotLightID_PL2 = -1;

		//		for ( DWORD i=0; i<NSShadowLight::GetLightNumber(); ++i )
		//		{
		//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight( i );
		//			if ( i==0 )
		//			{
		//				if ( pSpotLight )
		//				{
		//					m_dwSpotLightID_PL1 = pSpotLight->m_dwID;
		//					CaculateViewProjForMaterialSystemSpotShadow( m_matLastWVP_PL1, m_sPerspectiveCV_PL1, pSpotLight->m_sLight );
		//				}
		//				else
		//				{
		//					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//					m_dwSpotLightID_PL1 = -1;
		//				}
		//			}
		//			else
		//			{
		//				if ( pSpotLight )
		//				{
		//					m_dwSpotLightID_PL2 = pSpotLight->m_dwID;
		//					CaculateViewProjForMaterialSystemSpotShadow( m_matLastWVP_PL2, m_sPerspectiveCV_PL2, pSpotLight->m_sLight );
		//				}
		//				else
		//				{
		//					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//					m_dwSpotLightID_PL2 = -1;
		//				}

		//			}
		//		}

		//		// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
		//		if ( NSShadowLight::IsCascadeShadow() )
		//		{
		//			float fDistance = GetCameraLengthCascadeShadow();

		//			//CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, 250.f );
		//			CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, fDistance );
		//			CaculateViewProjForMaterialSystemCascadeShadow2( m_matLastWVP_D_2nd, m_sOrthoCV_2nd, fDistance );
		//		}
		//		else
		//		{
		//			CaculateViewProjForMaterialSystem( 1.f );
		//			//m_bCharUse = FALSE;
		//			//return S_OK;
		//		}
		//	}
		//	else if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_NM )
		//	{
		//		// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//		m_dwSpotLightID_PL1 = -1;
		//		m_dwSpotLightID_PL2 = -1;

		//		for ( DWORD i=0; i<NSShadowLight::GetLightNumber(); ++i )
		//		{
		//			const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight( i );
		//			if ( i==0 )
		//			{
		//				if ( pSpotLight )
		//				{
		//					m_dwSpotLightID_PL1 = pSpotLight->m_dwID;
		//				}
		//				else
		//				{
		//					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//					m_dwSpotLightID_PL1 = -1;
		//				}
		//			}
		//			else
		//			{
		//				if ( pSpotLight )
		//				{
		//					m_dwSpotLightID_PL2 = pSpotLight->m_dwID;
		//				}
		//				else
		//				{
		//					// ����ȭ�� ���� ����Ʈ���� �ִ´�.
		//					m_dwSpotLightID_PL2 = -1;
		//				}
		//			}
		//		}

		//		CaculateViewProjForMaterialSystem( 1.f );
		//	}
		//	else
		//	{
		//		CaculateViewProjForMaterialSystem( 1.f );
		//	}
		//}

		// Deffered Render �̰ų� ������������ SelfShadow �� ���.
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF || RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
		{
			RenderShadowForMaterialSystem( pSkinObj, matRot, pd3dDevice );
		}
	}

	m_bCharUse = FALSE;

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowCharMob ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( !pSkinObj )
		return S_OK;

	if ( bWorldSystemOLD )
	{
		if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 && m_emShadowDetail >= LAND_QT_MIDDLE )	// Material ���� �ƴ� �������� ��� �׸��� ������ ����.
		{
			if ( m_bCharUse )	RenderShadow ( pSkinObj, matRot, pd3dDevice );
			else				RenderShadowMob ( pSkinObj, matRot, pd3dDevice );
		}
	}
	else
	{
		if ( m_bCharUse )
		{
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
			{
				// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
				if ( !NSShadowLight::IsCascadeShadow() )
				{
					m_bCharUse = FALSE;
					return S_OK;
				}

				float fDistance = GetCameraLengthCascadeShadow();

				//CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, 250.f );
				CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, fDistance );
				CaculateViewProjForMaterialSystemCascadeShadow2( m_matLastWVP_D_2nd, m_sOrthoCV_2nd, fDistance );
			}
			else
			{
				CaculateViewProjForMaterialSystem( 1.f );
			}
		}

		// Deffered Render �̰ų� ������������ SelfShadow �� ���.
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF || RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
		{
			RenderShadowForMaterialSystem( pSkinObj, matRot, pd3dDevice );
		}
	}

	m_bCharUse = FALSE;

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowForLobbyStage( BOOL bWorld_MaterialSystem )
{
	if ( !bWorld_MaterialSystem )
		return S_OK;

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
		if ( !NSShadowLight::IsCascadeShadow() )
		{
			return S_OK;
		}

		float fDistance = GetCameraLengthCascadeShadow();

		//CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, 250.f );
		CaculateViewProjForMaterialSystemCascadeShadow( m_matLastWVP_D, m_sOrthoCV, fDistance );
		CaculateViewProjForMaterialSystemCascadeShadow2( m_matLastWVP_D_2nd, m_sOrthoCV_2nd, fDistance );
	}
	else
	{
		CaculateViewProjForMaterialSystem( 1.f );
	}

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowCharMob_SpotLight( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( !pSkinObj )
		return S_OK;

	if ( !bUsedMaterialSystem )
		return S_OK;

	if( RENDERPARAM::g_emGroundQulity < TnL_PS_3_0_CSM )
		return S_OK;

	if ( dwSlot == 0 )
	{
		// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
		if ( DxShadowMap::GetInstance().GetSpotLightID_PL1() == static_cast<DWORD>(-1) )
		{
			return S_OK;
		}

		RenderShadowForMaterialSystem_SpotLight
		( 
			pSkinObj, 
			matRot, 
			pd3dDevice, 
			m_sPerspectiveCV_PL1, 
			m_matLastWVP_PL1, 
			DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL1,
			DxSurfaceTex::GetInstance().m_pShadowZBuf_PL1
		);
	}
	else
	{
		// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
		if ( DxShadowMap::GetInstance().GetSpotLightID_PL2() == static_cast<DWORD>(-1) )
		{
			return S_OK;
		}

		RenderShadowForMaterialSystem_SpotLight
		( 
			pSkinObj, 
			matRot, 
			pd3dDevice, 
			m_sPerspectiveCV_PL2,
			m_matLastWVP_PL2, 
			DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL2,
			DxSurfaceTex::GetInstance().m_pShadowZBuf_PL2
		);
	}

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowCharMob_SpotLight( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( !pSkinObj )
		return S_OK;

	if ( !bUsedMaterialSystem )
		return S_OK;

	if( RENDERPARAM::g_emGroundQulity < TnL_PS_3_0_CSM )
		return S_OK;

	if ( dwSlot == 0 )
	{
		// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
		if ( DxShadowMap::GetInstance().GetSpotLightID_PL1() == static_cast<DWORD>(-1) )
		{
			return S_OK;
		}

		RenderShadowForMaterialSystem_SpotLight
		( 
			pSkinObj, 
			matRot, 
			pd3dDevice, 
			m_sPerspectiveCV_PL1, 
			m_matLastWVP_PL1, 
			DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL1,
			DxSurfaceTex::GetInstance().m_pShadowZBuf_PL1
		);
	}
	else
	{
		// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
		if ( DxShadowMap::GetInstance().GetSpotLightID_PL2() == static_cast<DWORD>(-1) )
		{
			return S_OK;
		}

		RenderShadowForMaterialSystem_SpotLight
		( 
			pSkinObj, 
			matRot, 
			pd3dDevice, 
			m_sPerspectiveCV_PL2,
			m_matLastWVP_PL2, 
			DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL2,
			DxSurfaceTex::GetInstance().m_pShadowZBuf_PL2
		);
	}

	return S_OK;
}

void DxShadowMap::CaculateViewProjForMaterialSystem( float fLengthMulti )
{
	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;
	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	// �ణ�� ������ �־�� �Ѵ�.
	if ( vLightDirect.x == 0.f && vLightDirect.z == 0.f )
	{
		vLightDirect.z = 0.001f;
	}

	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3 vTempDir = LookatPt - FromPt;
	m_fDistance = D3DXVec3Length( &vTempDir );

	vTempDir.y = 0.f;
	vTempDir *= 0.25f;
	vTempDir += LookatPt;

	// ���̸� �� �����ؾ��Ѵ�.
	// matProj �� Far ���� �Ѱ谡 �ִ�.
	vLightPos.x = vTempDir.x - vLightDirect.x*2500.f;
	vLightPos.y = vTempDir.y - vLightDirect.y*2500.f;
	vLightPos.z = vTempDir.z - vLightDirect.z*2500.f;

	// ������ �Ÿ��� ã�´�.
	m_fDistance = 0.1f + m_fDistance*1.75f;
	m_fDistance *= fLengthMulti;

	// ������ �д�.
	if ( m_fDistance > 250.f )
		m_fDistance = 250.f;

	// ����, �¿� ����
	D3DXVECTOR3 vDown(0.f,-1.f,0.f);
	float fRateXDivY = 1.f / D3DXVec3Dot( &vLightDirect, &vDown );
	
	float fProjW = m_fDistance * fRateXDivY;	// ��￩���� �������� �ϱ� ������ ���κ��� �� ������ �ȴ�.
	float fProjH = m_fDistance;

	D3DXMATRIX lightView;
	D3DXMATRIX matProj;
	D3DXMatrixLookAtLH( &lightView, &vLightPos, &vTempDir, &up );
	D3DXMatrixOrthoLH ( &matProj, fProjW, fProjH, 500.0f, 3500.f );

	D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView, &matProj );

	// Cullsion �� ���� ClipVolume �� ������.
	DxViewPort::GetInstance().ComputeClipVolumeOrtho( m_sOrthoCV, vLightPos, vTempDir, fProjW, fProjH );
}

float DxShadowMap::GetCameraLengthCascadeShadow()
{
	float fDistance(40.f);
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt	= DxViewPort::GetInstance().GetFromPt();
	//fDistance *= 1.5f;
	D3DXVECTOR3 vTempDir = LookatPt - FromPt;
	fDistance = fDistance + (fDistance * D3DXVec3Length( &vTempDir ) * 0.05f);
	if ( fDistance > 250.f )
		fDistance = 250.f;

	return fDistance;
};

void DxShadowMap::CaculateViewProjForMaterialSystemCascadeShadow( D3DXMATRIX &matWVP, CLIPVOLUME &sOrthoCV, float fDistance )
{
	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;
	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	// �ణ�� ������ �־�� �Ѵ�.
	if ( vLightDirect.x == 0.f && vLightDirect.z == 0.f )
	{
		vLightDirect.z = 0.001f;
	}

	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt	= DxViewPort::GetInstance().GetFromPt();

	// ���̸� �� �����ؾ��Ѵ�.
	// matProj �� Far ���� �Ѱ谡 �ִ�.
	vLightPos.x = LookatPt.x - vLightDirect.x*2500.f;
	vLightPos.y = LookatPt.y - vLightDirect.y*2500.f;
	vLightPos.z = LookatPt.z - vLightDirect.z*2500.f;

	// ����, �¿� ����
	D3DXVECTOR3 vDown(0.f,-1.f,0.f);
	float fRateXDivY = 1.f / D3DXVec3Dot( &vLightDirect, &vDown );

	//fDistance *= 1.5f;
	//D3DXVECTOR3 vTempDir = LookatPt - FromPt;
	//fDistance = fDistance + (fDistance * D3DXVec3Length( &vTempDir ) * 0.02f);

	float fProjW = fDistance * fRateXDivY;	// ��￩���� �������� �ϱ� ������ ���κ��� �� ������ �ȴ�.
	float fProjH = fDistance;

	D3DXMATRIX lightView;
	D3DXMATRIX matProj;
	D3DXMatrixLookAtLH( &lightView, &vLightPos, &LookatPt, &up );
	D3DXMatrixOrthoLH ( &matProj, fProjW, fProjH, 500.0f, 3500.f );

	D3DXMatrixMultiply ( &matWVP, &lightView, &matProj );

	// Cullsion �� ���� ClipVolume �� ������.
	DxViewPort::GetInstance().ComputeClipVolumeOrtho( sOrthoCV, vLightPos, LookatPt, fProjW, fProjH );
}

void DxShadowMap::CaculateViewProjForMaterialSystemCascadeShadow2( D3DXMATRIX &matWVP, CLIPVOLUME &sOrthoCV, float fDistance )
{
	float fQuadSize(fDistance);

	// �ּ� ������
	if ( fQuadSize < 100.f )
		fQuadSize = 100.f;

	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;
	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	// �ణ�� ������ �־�� �Ѵ�.
	if ( vLightDirect.x == 0.f && vLightDirect.z == 0.f )
	{
		vLightDirect.z = 0.001f;
	}

	const D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	const D3DXVECTOR3 &FromPt	= DxViewPort::GetInstance().GetFromPt();

	// ���̸� �� �����ؾ��Ѵ�.
	// matProj �� Far ���� �Ѱ谡 �ִ�.
	vLightPos.x = LookatPt.x - vLightDirect.x*2500.f;
	vLightPos.y = LookatPt.y - vLightDirect.y*2500.f;
	vLightPos.z = LookatPt.z - vLightDirect.z*2500.f;

	// ���� ũ�� ������ �Ѵ�.
	//fDistance *= 1.5f;
	fQuadSize *= 3.f;

	// ī�޶� �̵���Ų��.
	D3DXVECTOR3 vCameraDir = LookatPt - FromPt;
	vCameraDir.y = 0.f;
	if ( vCameraDir.x == 0.f && vCameraDir.z == 0.f )
	{
		vCameraDir.x = 1.f;
	}
	D3DXVec3Normalize( &vCameraDir, &vCameraDir );
	vCameraDir *= fDistance * 0.68f * 3.f;					// ��ġ�� ���߸� 2 �ܰ� �׸��ڰ� �������� �´�.
	vLightPos += vCameraDir;
	D3DXVECTOR3 LookatPtNew = LookatPt + vCameraDir;

	// ����, �¿� ����
	D3DXVECTOR3 vDown(0.f,-1.f,0.f);
	float fRateXDivY = 1.f / D3DXVec3Dot( &vLightDirect, &vDown );

	float fProjW = fQuadSize * fRateXDivY;	// ��￩���� �������� �ϱ� ������ ���κ��� �� ������ �ȴ�.
	float fProjH = fQuadSize;

	D3DXMATRIX lightView;
	D3DXMATRIX matProj;
	D3DXMatrixLookAtLH( &lightView, &vLightPos, &LookatPtNew, &up );
	D3DXMatrixOrthoLH ( &matProj, fProjW, fProjH, 500.0f, 3500.f );

	D3DXMatrixMultiply ( &matWVP, &lightView, &matProj );

	// Cullsion �� ���� ClipVolume �� ������.
	DxViewPort::GetInstance().ComputeClipVolumeOrtho( sOrthoCV, vLightPos, LookatPtNew, fProjW, fProjH );
}

void DxShadowMap::CaculateViewProjForMaterialSystemSpotShadow( D3DXMATRIX &matWVP, CLIPVOLUME &sCV, const D3DLIGHTQ& sLight )
{
	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	D3DXVECTOR3 vFromPt( sLight.Position.x, sLight.Position.y, sLight.Position.z );
	D3DXVECTOR3 vLookatPt( sLight.Direction.x, sLight.Direction.y, sLight.Direction.z );
	vLookatPt += vFromPt;

	D3DXVECTOR3 vDir = vFromPt-vLookatPt;
	D3DXVec3Normalize( &vDir, &vDir );
	vFromPt += vDir * sLight.Range * 0.05f;

	D3DXMATRIX lightView;
	D3DXMATRIX matProj;
	D3DXMatrixLookAtLH( &lightView, &vFromPt, &vLookatPt, &up );
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI*sLight.Phi/180.f, 1.f, sLight.Range*0.05f, sLight.Range*1.05f );

	D3DXMatrixMultiply ( &matWVP, &lightView, &matProj );

	sCV = DxViewPort::GetInstance().ComputeClipVolume ( vLookatPt, vFromPt, up, sLight.Range*0.1f, sLight.Range*1.05f, 1.f, 1.f, D3DX_PI*sLight.Phi/180.f );
}

HRESULT DxShadowMap::RenderShadowForMaterialSystem( DxSkinCharBase *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;
	pSkinObj->GetAABBBox( vMax, vMin );

	vMax.x += matRot._41;
	vMax.y += matRot._42;
	vMax.z += matRot._43;

	vMin.x += matRot._41;
	vMin.y += matRot._42;
	vMin.z += matRot._43;

	if ( !COLLISION::IsCollisionVolumeOrtho( m_sOrthoCV, vMax, vMin ) )
		return S_OK;

	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM && NSShadowLight::IsCascadeShadow() )
		{
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_CSM );
		}
		else
		{
			//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 || RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
			//{
			//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSufG32R32F_VSM_CSM2 );
			//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );
			//}
			//else
			{
				// CharSelfShadow �ƴϸ鼭 TnL_PS_3_0_DF �� ���
				pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F );
				pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );
			}
		}

		m_pSavedStateBlock->Capture();
		m_pEffectStateBlock->Apply();

		// Z �� ����ؾ��Ѵ�.
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );

		pSkinObj->RenderShadowForMaterialSystem( pd3dDevice, m_matLastWVP_D );

		m_pSavedStateBlock->Apply();
	}

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowForMaterialSystem_SpotLight( DxSkinCharBase *pSkinObj, 
															 const D3DXMATRIX &matRot, 
															 LPDIRECT3DDEVICEQ pd3dDevice, 
															 const CLIPVOLUME& sCV, 
															 const D3DXMATRIX& matVP, 
															 LPDIRECT3DSURFACEQ pSurface,
															 LPDIRECT3DSURFACEQ pZBuffer )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3 vMax;
	D3DXVECTOR3 vMin;
	pSkinObj->GetAABBBox( vMax, vMin );

	vMax.x += matRot._41;
	vMax.y += matRot._42;
	vMax.z += matRot._43;

	vMin.x += matRot._41;
	vMin.y += matRot._42;
	vMin.z += matRot._43;

	if ( !COLLISION::IsCollisionVolume( sCV, vMax, vMin ) )
		return S_OK;

	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		pd3dDevice->SetRenderTarget ( 0, pSurface );
		pd3dDevice->SetDepthStencilSurface( pZBuffer );

		m_pSavedStateBlock->Capture();
		m_pEffectStateBlock->Apply();

		// Z �� ����ؾ��Ѵ�.
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	TRUE );

		//DWORD dwCullMode(0L);
		//pd3dDevice->GetRenderState ( D3DRS_CULLMODE,		&dwCullMode );
		//pd3dDevice->SetRenderState ( D3DRS_CULLMODE,		D3DCULL_NONE );

		pSkinObj->RenderShadowForMaterialSystem( pd3dDevice, matVP );

		//pd3dDevice->SetRenderState ( D3DRS_CULLMODE,		dwCullMode );

		m_pSavedStateBlock->Apply();
	}

	return S_OK;
}

HRESULT DxShadowMap::RenderShadow ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().m_pTempSuf512 )
		return S_OK;

	if ( !DxSurfaceTex::GetInstance().m_pShadowZBuf )
		return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	m_fDistance = sqrtf(powf(FromPt.x - vCharPos.x,2) + powf(FromPt.y - vCharPos.y,2) + powf(FromPt.z - vCharPos.z,2) ) * m_fDistanceRate;

	// Ÿ�� �Ÿ�12m ������ �׸��ڰ� �� ���ü� �ֵ��� �۾���.
	// �׸��ڸ� �׸��� ī�޶��� �Ÿ��� �ʹ� ������.. �ʼ����� ������ ī�޶� ������ ������ �׷����� �Ǿ�.
	// �� ������ Alpha ���� 0 �� �� �� �ִ�. �׷��� �Ǹ� �׸��ڴ� �ȳ����� �ȴ�.
	// �̷� �������� Ÿ�� �Ÿ� 24m�� �츮���ӿ����� ����Ǳ� �����.
	// 
	if ( m_fDistance < 260.f )
	{
		m_fDistance = 130.f + m_fDistance*0.5f;
	}

	{
        DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice );

		//switch( m_emShadowDetail )
		//{
		//case LAND_QT_LOW:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
		//	break;
		//default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
		//	break;
		//};
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT DxShadowMap::RenderShadow ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().m_pTempSuf512 )
		return S_OK;

	if ( !DxSurfaceTex::GetInstance().m_pShadowZBuf )
		return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	m_fDistance = sqrtf(powf(FromPt.x - vCharPos.x,2) + powf(FromPt.y - vCharPos.y,2) + powf(FromPt.z - vCharPos.z,2) ) * m_fDistanceRate;

	// Ÿ�� �Ÿ�12m ������ �׸��ڰ� �� ���ü� �ֵ��� �۾���.
	// �׸��ڸ� �׸��� ī�޶��� �Ÿ��� �ʹ� ������.. �ʼ����� ������ ī�޶� ������ ������ �׷����� �Ǿ�.
	// �� ������ Alpha ���� 0 �� �� �� �ִ�. �׷��� �Ǹ� �׸��ڴ� �ȳ����� �ȴ�.
	// �̷� �������� Ÿ�� �Ÿ� 24m�� �츮���ӿ����� ����Ǳ� �����.
	// 
	if ( m_fDistance < 260.f )
	{
		m_fDistance = 130.f + m_fDistance*0.5f;
	}

	{
        DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice );

		//switch( m_emShadowDetail )
		//{
		//case LAND_QT_LOW:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
		//	break;
		//default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
		//	break;
		//};
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowMob ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().m_pTempSuf512 )
		return S_OK;

	if ( !DxSurfaceTex::GetInstance().m_pShadowZBuf )
		return S_OK;

	HRESULT hr = S_OK;

	D3DXVECTOR3		vMax, vMin;
	vMax.x = m_vBaseViewMax.x + 50.f;
	vMax.z = m_vBaseViewMax.z + 50.f;
	vMin.x = m_vBaseViewMin.x - 50.f;
	vMin.z = m_vBaseViewMin.z - 50.f;

	//	Note : �� �ָ� �ִ� ���� ��� �Ѹ��� �ʴ´�.
	//
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );
	D3DXVECTOR3	vDis;
	vDis.x	= vCharPos.x - LookatPt.x;
	if ( vDis.x > vMax.x || vDis.x < vMin.x )	return S_OK;
	vDis.z	= vCharPos.z - LookatPt.z;
	if ( vDis.z > vMax.z || vDis.z < vMin.z )	return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matLastWVP_OLD );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matIdentity );

	{
        DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice );

		//switch( m_emShadowDetail )
		//{
		//case LAND_QT_LOW:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
		//	break;
		//case LAND_QT_MIDDLE:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0x80808080 );
		//	break;
		//default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
		//	break;
		//};
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowMob ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().m_pTempSuf512 )
		return S_OK;

	if ( !DxSurfaceTex::GetInstance().m_pShadowZBuf )
		return S_OK;

	HRESULT hr = S_OK;

	D3DXVECTOR3		vMax, vMin;
	vMax.x = m_vBaseViewMax.x + 50.f;
	vMax.z = m_vBaseViewMax.z + 50.f;
	vMin.x = m_vBaseViewMin.x - 50.f;
	vMin.z = m_vBaseViewMin.z - 50.f;

	//	Note : �� �ָ� �ִ� ���� ��� �Ѹ��� �ʴ´�.
	//
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );
	D3DXVECTOR3	vDis;
	vDis.x	= vCharPos.x - LookatPt.x;
	if ( vDis.x > vMax.x || vDis.x < vMin.x )	return S_OK;
	vDis.z	= vCharPos.z - LookatPt.z;
	if ( vDis.z > vMax.z || vDis.z < vMin.z )	return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matLastWVP_OLD );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matIdentity );

	{
        DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice );

		//switch( m_emShadowDetail )
		//{
		//case LAND_QT_LOW:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
		//	break;
		//case LAND_QT_MIDDLE:
		//	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		//	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		//	Circle_Day_Night ( pd3dDevice, matRot, 0x80808080 );
		//	break;
		//default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
		//	break;
		//};
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT	DxShadowMap::Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, const D3DXMATRIX &matRot )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;
	
	D3DXVECTOR3 &vCameraPos = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3	 vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	//GLPeriod::GetInstance().SetHour (1);	//������ �Ѵ�.	���������� ���� ����� ���� �� ����Ѵ�.

	D3DXMATRIX lightView;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	float kk	= 0.15f;
	kk	= 1.f - Light.m_Light.Ambient.r;
	kk	*= 0.25f;
	if ( kk < 0.15f )
		kk = 0.15f;
	if ( GLPeriod::GetInstance().IsOffLightForShadow() )	// ���� ���, ������ ���� ���� (�ð��� ���� ���� ����(����) ���� )
	{
		float		fDirectFact = GLPeriod::GetInstance().GetDirectionFact();
		fDirectFact = 0.2f;
		if ( fDirectFact < 0.f ) fDirectFact = -fDirectFact;
		fDirectFact *= 0.4f;
		fDirectFact += 0.3f;		// ���� ��� ����. ?

		vLightDirect.y = 0.f;
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightDirect.x = vLightDirect.x*fDirectFact;
		vLightDirect.y = fabsf(fDirectFact) - 1.f;
		vLightDirect.z = vLightDirect.z*fDirectFact;

		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		kk *= ( (0.7f-fabsf(fDirectFact)) + 0.5f );		// ���� 1.2 �ּ� 0.5


		D3DXVECTOR3 &FromPt = DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3 vTempDir;

		vTempDir.x  = LookatPt.x - (vLightDirect.x*40.f);
		vTempDir.y  = LookatPt.y;
		vTempDir.z  = LookatPt.z - (vLightDirect.z*40.f);

		vLightPos.x = vTempDir.x - (vLightDirect.x*(50.f+m_fDistance));
		vLightPos.y = vTempDir.y - (vLightDirect.y*(50.f+m_fDistance));
		vLightPos.z = vTempDir.z - (vLightDirect.z*(50.f+m_fDistance));

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vTempDir, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW, &lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );

		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		m_pSavedStateBlock->Capture();
		m_pEffectStateBlock->Apply();

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);;
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, TRUE );

		m_pSavedStateBlock->Apply();
	}
	else if ( DxSurfaceTex::GetInstance().m_pShadowSuf && 
				DxSurfaceTex::GetInstance().m_pShadowZBuf &&
				DxSurfaceTex::GetInstance().m_pTempSuf512 )		// ���� ���, ������ ���� ���� (���� ��ġ�� ���� ���� ����(����) ���� )
	{
		hr = pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );		// �������� ���� ��� �Ѵ�. ^^;
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		if(FAILED(hr))	return E_FAIL;

		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		if(FAILED(hr))	return E_FAIL;

		SetPointLight ( pd3dDevice, pSkinObj, matRot, vCharPos, vLightPos, vLightDirect );

		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		pd3dDevice->SetDepthStencilSurface( NULL );

		//	Note : 	View �� Projection �� ��´�.
		//
		SetViewProjection ();

		//	Note : �ǿ�.. �׸��ڸ� �׷� ����.
		//			������ �ؾ� �� ��, World, View, Projection ����, �ؽ��� ����
		//
		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
		pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );

		//	Note : ���� ī�޶� CV�� ������ �� ���ϱ�.
		//
		D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
		D3DXVECTOR3		vPosition[4];

		float	fNearPlane	= 0.5f;
		float	fFarPlane	= 100000.f;
		float	fFOV		= D3DX_PI/4;

		FLOAT dist, t;
		D3DXVECTOR3 pt[8];
		D3DXVECTOR3 v1, v2, n;
		D3DXVECTOR3 vDir, vUp, vPos;	
		D3DXVECTOR3 vCross;

		vCharPos.y = 5.f;
		vDir = vLightDirect;
		vUp = vUpVec;
		vPos = vCharPos - (vLightDirect*33.f);		// 33.f ��� ����. (�̰��� �׸��ڰ� ��Ÿ���� �����̴�.)
													// �׸��ڸ� ũ�� �ϰ� ������� ���� ���̸� �ȴ�.

		D3DXVec3Normalize ( &vUp, &vUp );
		D3DXVec3Normalize ( &vDir, &vDir );

		D3DXVec3Cross ( &vCross, &vUp, &vDir );
		D3DXVec3Normalize ( &vCross, &vCross );

		D3DXVec3Cross ( &vUp, &vDir, &vCross );
		D3DXVec3Normalize ( &vUp, &vUp );

		for(INT i = 0; i < 8; i++)
		{
			//	Note	:	i�� ���� 4�̻��϶�
			//				�հ��� ���� ��ǥ�̴�.

			//	(1)
			dist = (i & 0x4) ? fFarPlane : fNearPlane;
			pt[i] = dist * vDir;

			//	(2)
			t = dist * tanf(fFOV/2);
			t = (i & 0x2) ? t : -t;
			pt[i] += vUp * t;
			
			t = dist * tanf(fFOV/2) * m_fTexWidth/m_fTexHeight; // take into account screen proportions
			t = (i & 0x1) ? -t : t;
			pt[i] += vCross *t;
			pt[i] = vPos + pt[i];
		}

		//	Note : �� �����
		D3DXPLANE	pPlane;
		vPosition[0] = D3DXVECTOR3 ( 100000.f, 0.f, -100000.f );
		vPosition[1] = D3DXVECTOR3 ( -100000.f, 0.f, 100000.f );
		vPosition[2] = D3DXVECTOR3 ( 100000.f, 0.f, 100000.f );
		D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

		//	Note : �� �����
		vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );	// 3 2
		vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );	// 1 0
		vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
		vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

		VERTEX	sShadow[4];

		sShadow[0].p = vPosition[3];
		sShadow[1].p = vPosition[1];
		sShadow[2].p = vPosition[2];
		sShadow[3].p = vPosition[0];

		m_pSavedOneShadowSB->Capture();
		m_pDrawOneShadowSB->Apply();

		pd3dDevice->SetFVF ( VERTEX::FVF );

		DxSetTextureMatrix ( pd3dDevice, 0, m_matLastWVP_OLD );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sShadow, sizeof(VERTEX) );

		DxResetTextureMatrix ( pd3dDevice, 0 );

		m_pSavedOneShadowSB->Apply();

		//DxSurfaceTex::GetInstance().RenderPointShadow ( pd3dDevice, m_vViewMax, m_vViewMin, m_matLastWVP_OLD, m_dwTexDetail );	// ���� �� ( �ӵ� ���� )
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	return hr;
}

HRESULT	DxShadowMap::Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle *pSkinObj, const D3DXMATRIX &matRot )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;

	D3DXVECTOR3 &vCameraPos = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3	 vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	//GLPeriod::GetInstance().SetHour (1);	//������ �Ѵ�.	���������� ���� ����� ���� �� ����Ѵ�.

	D3DXMATRIX lightView;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	float kk	= 0.15f;
	kk	= 1.f - Light.m_Light.Ambient.r;
	kk	*= 0.25f;
	if ( kk < 0.15f )
		kk = 0.15f;
	if ( GLPeriod::GetInstance().IsOffLightForShadow() )	// ���� ���, ������ ���� ���� (�ð��� ���� ���� ����(����) ���� )
	{
		float		fDirectFact = GLPeriod::GetInstance().GetDirectionFact();
		fDirectFact = 0.2f;
		if ( fDirectFact < 0.f ) fDirectFact = -fDirectFact;
		fDirectFact *= 0.4f;
		fDirectFact += 0.3f;		// ���� ��� ����. ?

		vLightDirect.y = 0.f;
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightDirect.x = vLightDirect.x*fDirectFact;
		vLightDirect.y = fabsf(fDirectFact) - 1.f;
		vLightDirect.z = vLightDirect.z*fDirectFact;

		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		kk *= ( (0.7f-fabsf(fDirectFact)) + 0.5f );		// ���� 1.2 �ּ� 0.5


		D3DXVECTOR3 &FromPt = DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3 vTempDir;

		vTempDir.x  = LookatPt.x - (vLightDirect.x*40.f);
		vTempDir.y  = LookatPt.y;
		vTempDir.z  = LookatPt.z - (vLightDirect.z*40.f);

		vLightPos.x = vTempDir.x - (vLightDirect.x*(50.f+m_fDistance));
		vLightPos.y = vTempDir.y - (vLightDirect.y*(50.f+m_fDistance));
		vLightPos.z = vTempDir.z - (vLightDirect.z*(50.f+m_fDistance));

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vTempDir, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW, &lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );

		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		m_pSavedStateBlock->Capture();
		m_pEffectStateBlock->Apply();

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);;
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, TRUE );

		m_pSavedStateBlock->Apply();
	}
	else if ( DxSurfaceTex::GetInstance().m_pShadowSuf && 
				DxSurfaceTex::GetInstance().m_pShadowZBuf &&
				DxSurfaceTex::GetInstance().m_pTempSuf512 )		// ���� ���, ������ ���� ���� (���� ��ġ�� ���� ���� ����(����) ���� )
	{
		hr = pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );		// �������� ���� ��� �Ѵ�. ^^;
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		if(FAILED(hr))	return E_FAIL;

		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		if(FAILED(hr))	return E_FAIL;

		SetPointLight ( pd3dDevice, pSkinObj, matRot, vCharPos, vLightPos, vLightDirect );

		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		pd3dDevice->SetDepthStencilSurface( NULL );

		//	Note : 	View �� Projection �� ��´�.
		//
		SetViewProjection ();

		//	Note : �ǿ�.. �׸��ڸ� �׷� ����.
		//			������ �ؾ� �� ��, World, View, Projection ����, �ؽ��� ����
		//
		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
		pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );

		//	Note : ���� ī�޶� CV�� ������ �� ���ϱ�.
		//
		D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
		D3DXVECTOR3		vPosition[4];

		float	fNearPlane	= 0.5f;
		float	fFarPlane	= 100000.f;
		float	fFOV		= D3DX_PI/4;

		FLOAT dist, t;
		D3DXVECTOR3 pt[8];
		D3DXVECTOR3 v1, v2, n;
		D3DXVECTOR3 vDir, vUp, vPos;	
		D3DXVECTOR3 vCross;

		vCharPos.y = 5.f;
		vDir = vLightDirect;
		vUp = vUpVec;
		vPos = vCharPos - (vLightDirect*33.f);		// 33.f ��� ����. (�̰��� �׸��ڰ� ��Ÿ���� �����̴�.)
		// �׸��ڸ� ũ�� �ϰ� ������� ���� ���̸� �ȴ�.

		D3DXVec3Normalize ( &vUp, &vUp );
		D3DXVec3Normalize ( &vDir, &vDir );

		D3DXVec3Cross ( &vCross, &vUp, &vDir );
		D3DXVec3Normalize ( &vCross, &vCross );

		D3DXVec3Cross ( &vUp, &vDir, &vCross );
		D3DXVec3Normalize ( &vUp, &vUp );

		for(INT i = 0; i < 8; i++)
		{
			//	Note	:	i�� ���� 4�̻��϶�
			//				�հ��� ���� ��ǥ�̴�.

			//	(1)
			dist = (i & 0x4) ? fFarPlane : fNearPlane;
			pt[i] = dist * vDir;

			//	(2)
			t = dist * tanf(fFOV/2);
			t = (i & 0x2) ? t : -t;
			pt[i] += vUp * t;

			t = dist * tanf(fFOV/2) * m_fTexWidth/m_fTexHeight; // take into account screen proportions
			t = (i & 0x1) ? -t : t;
			pt[i] += vCross *t;
			pt[i] = vPos + pt[i];
		}

		//	Note : �� �����
		D3DXPLANE	pPlane;
		vPosition[0] = D3DXVECTOR3 ( 100000.f, 0.f, -100000.f );
		vPosition[1] = D3DXVECTOR3 ( -100000.f, 0.f, 100000.f );
		vPosition[2] = D3DXVECTOR3 ( 100000.f, 0.f, 100000.f );
		D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

		//	Note : �� �����
		vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );	// 3 2
		vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );	// 1 0
		vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
		vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

		VERTEX	sShadow[4];

		sShadow[0].p = vPosition[3];
		sShadow[1].p = vPosition[1];
		sShadow[2].p = vPosition[2];
		sShadow[3].p = vPosition[0];

		m_pSavedOneShadowSB->Capture();
		m_pDrawOneShadowSB->Apply();

		pd3dDevice->SetFVF ( VERTEX::FVF );

		DxSetTextureMatrix ( pd3dDevice, 0, m_matLastWVP_OLD );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sShadow, sizeof(VERTEX) );

		DxResetTextureMatrix ( pd3dDevice, 0 );

		m_pSavedOneShadowSB->Apply();

		//DxSurfaceTex::GetInstance().RenderPointShadow ( pd3dDevice, m_vViewMax, m_vViewMin, m_matLastWVP_OLD, m_dwTexDetail );	// ���� �� ( �ӵ� ���� )
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	return hr;
}


HRESULT	DxShadowMap::Circle_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, DWORD dwTextureFactor )
{
	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;

	D3DXVECTOR3	 vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	if ( GLPeriod::GetInstance().IsOffLightForShadow() )	// ���� ���, ������ ���� ���� (�ð��� ���� ���� ����(����) ���� )
	{
		D3DXVECTOR3 up;
		up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

		D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

		D3DXMATRIX	matIdentity, lightView;

		D3DXMatrixIdentity ( &matIdentity );

		float		fDirectFact = GLPeriod::GetInstance().GetDirectionFact();
		fDirectFact = 0.2f;
		if ( fDirectFact < 0.f ) fDirectFact = -fDirectFact;
		fDirectFact *= 0.4f;
		fDirectFact += 0.3f;		// ���� ��� ����. ?

		vLightDirect.y = 0.f;
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightDirect.x = vLightDirect.x*fDirectFact;
		vLightDirect.y = fabsf(fDirectFact) - 1.f;
		vLightDirect.z = vLightDirect.z*fDirectFact;

		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );


		D3DXVECTOR3 &FromPt = DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3 vTempDir;

		vTempDir.x  = LookatPt.x - (vLightDirect.x*40.f);
		vTempDir.y  = LookatPt.y;
		vTempDir.z  = LookatPt.z - (vLightDirect.z*40.f);

		vLightPos.x = vTempDir.x - (vLightDirect.x*(50.f+m_fDistance));
		vLightPos.y = vTempDir.y - (vLightDirect.y*(50.f+m_fDistance));
		vLightPos.z = vTempDir.z - (vLightDirect.z*(50.f+m_fDistance));

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vTempDir, &up);


		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matRot );
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		pd3dDevice->SetTexture ( 0, m_textureRes.GetTexture() );

		m_pSavedCircleSB->Capture();
		m_pEffectCircleSB->Apply();

		if( dwTextureFactor!=0xffffffff)
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwTextureFactor );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
		}

		pd3dDevice->SetFVF ( VERTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, sizeof(VERTEX) );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pSavedCircleSB->Apply();
	}
	else
	{
		D3DXMATRIX	matTempRot = matRot;
		matTempRot._42 = 0.f;
		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matTempRot );
		//	Note : 	View �� Projection �� ��´�.
		//
		SetViewProjection ();

		//	Note : �ǿ�.. �׸��ڸ� �׷� ����.
		//			������ �ؾ� �� ��, World, View, Projection ����, �ؽ��� ����
		//
		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
		pd3dDevice->SetTexture ( 0, m_textureRes.GetTexture() );

		m_pSavedCircleSB->Capture();
		m_pEffectCircleSB->Apply();

		if( dwTextureFactor!=0xffffffff)
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwTextureFactor );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
		}

		pd3dDevice->SetFVF ( VERTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, sizeof(VERTEX) );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pSavedCircleSB->Apply();
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	return S_OK;
}

void	DxShadowMap::ReSetClosedPL ( D3DLIGHTQ &sLight, D3DXVECTOR3 vCharPos, int nNum )
{
	float	Dis = powf(sLight.Position.x - vCharPos.x,2) + powf(sLight.Position.z - vCharPos.z,2);

	if ( m_nPointNum1 == 0 )
	{
		m_fPointDis1 = Dis;
		m_nPointNum1 = nNum;
	}
	else if ( m_nPointNum2 == 0 )
	{
		if ( Dis < m_fPointDis1 )		// ������� �� �۴ٸ�.
		{
			m_fPointDis2 = m_fPointDis1;
			m_nPointNum2 = m_nPointNum1;
			m_fPointDis1 = Dis;
			m_nPointNum1 = nNum;
		}
		else
		{
			m_fPointDis2 = Dis;
			m_nPointNum2 = nNum;
		}
	}
	else
	{
		if ( Dis < m_fPointDis2 )		// ������� �� �۴ٸ�.
		{
			m_fPointDis2 = Dis;
			m_nPointNum2 = nNum;

			if ( Dis < m_fPointDis1 )		// ������� �� �۴ٸ�.
			{
				m_fPointDis2 = m_fPointDis1;
				m_nPointNum2 = m_nPointNum1;
				m_fPointDis1 = Dis;
				m_nPointNum1 = nNum;
			}
		}
	}
}

void	DxShadowMap::SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, 
									D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect )
{
	D3DXMATRIX lightView;

	D3DXVECTOR3	vDistance;
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();

	D3DXVECTOR3 up;
	
	up.x = 0.0f; up.y = 0.0f; up.z = 1.0f;

	float fLightLeftOver = 0.4f;

	// ����� ���� ��� �´�.
	DXLIGHT *pLightClosed = NULL;
	DXLIGHT *pLightClosed2 = NULL;

	D3DLIGHTQ sLight01, sLight02;

	//	�⺻������ ���� ��
	float Dis, PosX, PosY, PosZ, Range;
	Dis		= 100.f;
	PosX	= 0.f;
	PosY	= 0.f;
	PosZ	= 0.f;
	Range	= FLT_MIN;

	float Dis2, PosX2, PosZ2, Range2;
	Dis2	= 100.f;
	PosX2	= 0.f;
	PosZ2	= 0.f;
	Range2	= FLT_MIN;

	float fDistance1;
	float fDistance2;

	//	�ʱ�ȭ
	m_fPointDis1 = 0.f;
	m_nPointNum1 = 0;
	m_fPointDis2 = 0.f;
	m_nPointNum2 = 0;
	//	���
	for ( DWORD i=1; i<=7; i++ )
	{
		DXLIGHT* pLightClosed = DxLightMan::GetInstance()->GetClosedLight(i);

		if ( pLightClosed )		ReSetClosedPL ( pLightClosed->m_Light, vCharPos, i );
	}

	if ( m_nPointNum1 )
	{
		pLightClosed = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum1);
		if ( pLightClosed )		sLight01 = pLightClosed->m_Light;
	}
	if ( m_nPointNum2 )
	{
		pLightClosed2 = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum2);
		if ( pLightClosed2 )	sLight02 = pLightClosed2->m_Light;
	}

	// ���� �ִٸ� �� ���� ������ ����
	if ( pLightClosed )
	{
		Dis = m_fPointDis1;
		PosX = sLight01.Position.x;
		PosZ = sLight01.Position.z;

		Range	= sLight01.Range;

		fDistance1 = sqrtf(m_fPointDis1);
	}

	if ( pLightClosed2 )
	{
		Dis2 = m_fPointDis2;
		PosX2 = sLight02.Position.x;
		PosZ2 = sLight02.Position.z;

		Range2	= sLight02.Range;
		fDistance2 = sqrtf(m_fPointDis2);
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	float	kk = 0.15f;
	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	kk	= 1.f - Light.m_Light.Ambient.r;
	kk	*= 0.25f;
	if ( kk < 0.15f )
		kk = 0.15f;
	DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
	pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

	if ( pLightClosed && pLightClosed2 )
	{
		float fRate = (fDistance2-fDistance1)/(fDistance1*0.7f);
		if ( fRate>1.f )	fRate = 1.f;

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );			//	������ �ʹ� �ָ� ������ ������,
		vDistance.z = powf ( vLightDirect.z, 2 );			//	�Ÿ��� �����ϰ� ���� ���Ѽ�
		vDistance.y = sqrtf(vDistance.x+vDistance.z);	//	�׸��ڰ� �ʹ� ũ�ų�
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )		//	������ �ʴ� ���׸� �����Ѵ�.
		{													//	.
			vLightDirect.y = -(vDistance.y*2.f/3.f);		//	.
		}													//	.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f*fRate);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f*fRate);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		vLightDirect = vCharPos - vLightPos;					//	�ۿ��� ��� �Ұ��̴�.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );		//
		
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	//	Note : �ƹ� �͵� �ȵȴ�.
	//
	else if ( (!pLightClosed && !pLightClosed2) || GLPeriod::GetInstance().IsOffLightForShadow() )	// ���� ���ų� ���� ���� ������
	{
		float kk = 1.0f - fLightLeftOver;
		kk *= 0.25f;

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( vCharPos.x, vCharPos.y+40.f, vCharPos.z );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x;
		vLightPos.y = vCharPos.y + 150.f;
		vLightPos.z = vCharPos.z;

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	//	Note : �� �ϳ��� �ִ�. 
	//
	else
	{
		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);
		
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	m_pSavedStateBlock->Apply();
}

void	DxShadowMap::SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle *pSkinObj, const D3DXMATRIX &matRot, 
									D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect )
{
	D3DXMATRIX lightView;

	D3DXVECTOR3	vDistance;
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();

	D3DXVECTOR3 up;

	up.x = 0.0f; up.y = 0.0f; up.z = 1.0f;

	float fLightLeftOver = 0.4f;

	// ����� ���� ��� �´�.
	DXLIGHT *pLightClosed = NULL;
	DXLIGHT *pLightClosed2 = NULL;

	D3DLIGHTQ sLight01, sLight02;

	//	�⺻������ ���� ��
	float Dis, PosX, PosY, PosZ, Range;
	Dis		= 100.f;
	PosX	= 0.f;
	PosY	= 0.f;
	PosZ	= 0.f;
	Range	= FLT_MIN;

	float Dis2, PosX2, PosZ2, Range2;
	Dis2	= 100.f;
	PosX2	= 0.f;
	PosZ2	= 0.f;
	Range2	= FLT_MIN;

	float fDistance1;
	float fDistance2;

	//	�ʱ�ȭ
	m_fPointDis1 = 0.f;
	m_nPointNum1 = 0;
	m_fPointDis2 = 0.f;
	m_nPointNum2 = 0;
	//	���
	for ( DWORD i=1; i<=7; i++ )
	{
		DXLIGHT* pLightClosed = DxLightMan::GetInstance()->GetClosedLight(i);

		if ( pLightClosed )		ReSetClosedPL ( pLightClosed->m_Light, vCharPos, i );
	}

	if ( m_nPointNum1 )
	{
		pLightClosed = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum1);
		if ( pLightClosed )		sLight01 = pLightClosed->m_Light;
	}
	if ( m_nPointNum2 )
	{
		pLightClosed2 = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum2);
		if ( pLightClosed2 )	sLight02 = pLightClosed2->m_Light;
	}

	// ���� �ִٸ� �� ���� ������ ����
	if ( pLightClosed )
	{
		Dis = m_fPointDis1;
		PosX = sLight01.Position.x;
		PosZ = sLight01.Position.z;

		Range	= sLight01.Range;

		fDistance1 = sqrtf(m_fPointDis1);
	}

	if ( pLightClosed2 )
	{
		Dis2 = m_fPointDis2;
		PosX2 = sLight02.Position.x;
		PosZ2 = sLight02.Position.z;

		Range2	= sLight02.Range;
		fDistance2 = sqrtf(m_fPointDis2);
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	float	kk = 0.15f;
	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	kk	= 1.f - Light.m_Light.Ambient.r;
	kk	*= 0.25f;
	if ( kk < 0.15f )
		kk = 0.15f;
	DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
	pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

	if ( pLightClosed && pLightClosed2 )
	{
		float fRate = (fDistance2-fDistance1)/(fDistance1*0.7f);
		if ( fRate>1.f )	fRate = 1.f;

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );			//	������ �ʹ� �ָ� ������ ������,
		vDistance.z = powf ( vLightDirect.z, 2 );			//	�Ÿ��� �����ϰ� ���� ���Ѽ�
		vDistance.y = sqrtf(vDistance.x+vDistance.z);	//	�׸��ڰ� �ʹ� ũ�ų�
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )		//	������ �ʴ� ���׸� �����Ѵ�.
		{													//	.
			vLightDirect.y = -(vDistance.y*2.f/3.f);		//	.
		}													//	.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f*fRate);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f*fRate);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		vLightDirect = vCharPos - vLightPos;					//	�ۿ��� ��� �Ұ��̴�.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );		//

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	//	Note : �ƹ� �͵� �ȵȴ�.
	//
	else if ( (!pLightClosed && !pLightClosed2) || GLPeriod::GetInstance().IsOffLightForShadow() )	// ���� ���ų� ���� ���� ������
	{
		float kk = 1.0f - fLightLeftOver;
		kk *= 0.25f;

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( vCharPos.x, vCharPos.y+40.f, vCharPos.z );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x;
		vLightPos.y = vCharPos.y + 150.f;
		vLightPos.z = vCharPos.z;

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	//	Note : �� �ϳ��� �ִ�. 
	//
	else
	{
		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : ��� �߰�
		//
		D3DXMatrixMultiply ( &m_matLastWVP_OLD, &lightView,	&m_matProj );

		//	Note : ��Ų ������Ʈ ������.
		//
		pSkinObj->RenderShadow( pd3dDevice, m_matLastWVP_OLD, FALSE );
	}

	m_pSavedStateBlock->Apply();
}

void	DxShadowMap::SetViewProjection ()
{
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3	vCenter		= LookatPt;
	vCenter.y				= 0.f;

	D3DXVECTOR3	vLightDirect = LookatPt - FromPt;

	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	vCenter.x  = vCenter.x + (vLightDirect.x*50.f);
	//vCenter.y  = vCenter.y;
	vCenter.z  = vCenter.z + (vLightDirect.z*50.f);

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*EYE_TO_LOOKAT;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	D3DXMatrixLookAtLH ( &m_matView, &vTempLookatPt, &vCenter, &vUpVec );		// �� �����..

	m_vViewMax		= D3DXVECTOR3 ( m_vBaseViewMax.x+vCenter.x, vCenter.y, m_vBaseViewMax.z+vCenter.z );
	m_vViewMin		= D3DXVECTOR3 ( m_vBaseViewMin.x+vCenter.x, vCenter.y, m_vBaseViewMin.z+vCenter.z );
}

HRESULT DxShadowMap::ClearShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem )
{
	// �����̰� PS.2.0 �̻��̸� ���ο� Shadow�� ���۵ǰ� �Ѵ�.
	if ( bUsedMaterialSystem && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		if ( NSShadowLight::IsCascadeShadow() )
		{
			if ( !DxSurfaceTex::GetInstance().m_pShadowSufG32R32F_VSM_CSM2 )
				return E_FAIL;

			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufG32R32F_VSM_CSM2 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );
			HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
			if(FAILED(hr))
				return E_FAIL;

			if ( !DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM )
				return E_FAIL;

			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_CSM );
			hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
			if(FAILED(hr))
				return E_FAIL;
		}
		else
		{
			if ( !DxSurfaceTex::GetInstance().m_pShadowSufR32F )
				return S_FALSE;

			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );

			HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
			if(FAILED(hr))
				return E_FAIL;
		}
		
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_PL1 );
		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;

		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_PL2 );
		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;
	}
	else if ( bUsedMaterialSystem && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		if ( !DxSurfaceTex::GetInstance().m_pShadowSufR32F )
			return E_FAIL;

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );

		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;
	}
	else if ( m_emShadowDetail >= LAND_QT_MIDDLE )
	{
		if ( !DxSurfaceTex::GetInstance().m_pShadowSuf )
			return E_FAIL;

		if ( !DxSurfaceTex::GetInstance().m_pTempSuf512 )
			return E_FAIL;

		if ( !DxSurfaceTex::GetInstance().m_pShadowZBuf )
			return E_FAIL;

		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;
	}


	// TnL_PS_3_0_DF ������ �ȵǴ� ���̸鼭, SelfShadow �� �ؾ� �Ѵٸ�, m_pShadowSufR32F �� ���Ǿ�����.
	if ( RENDERPARAM::g_emGroundQulity <= TnL_PS_1_1 && RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_3_0 )
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		if ( !DxSurfaceTex::GetInstance().m_pShadowSufR32F )
			return E_FAIL;

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );

		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
		if(FAILED(hr))
			return E_FAIL;
	}

	return S_OK;
}

void DxShadowMap::SetShadowDetail ( ENUM_LAND_QUALITY Detail )
{ 
	m_emShadowDetail = Detail; 
	RENDERPARAM::CheckTnLMode();
}

void DxShadowMap::GetMultiply_Clip2Tex( D3DXMATRIX& matOut, const D3DXMATRIX matIn )
{
	D3DXMATRIX mClip2Tex = D3DXMATRIX(
		0.5,    0,    0,   0,
		0,	   -0.5,  0,   0,
		0,     0,     1,   0,
		0.5,   0.5,   0,   1 );
	
	D3DXMatrixMultiply(&matOut, &matIn, &mClip2Tex);
}
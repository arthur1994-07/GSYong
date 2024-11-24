#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/File/FileUtil.h"
#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/char/DxEffCharUserColor.h"
#include "../../DxEffect/char/DxEffCharOverlay.h"
#include "../../DxEffect/char/DxEffChar.h"
#include "../../G-Logic/GLogic.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"
#include "../../DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"
#include "../../DxMeshs/SkinMesh/DxSkinDefine.h"
#include "../../DxMeshs/SkinMesh/DxClothColl.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/Texture/NSTexCombine.h"
#include "../../DxTools/DxBackUpRendTarget.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../Common/SerialMemoryEngine.h"
#include "../../Common/StlFunctions.h"
#include "../../Archive/Archive.h"
#include "./_new/DxSkinMesh9.h"
#include "./DxSkinMeshManager.h"

#include "DxSkinPieceContainer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bCHAR_EDIT_RUN;
extern BOOL g_bCHAR_EDIT_PIECE_RENDER_FIXED;

extern char szTRACE_NAME[MAX_TRACE][STRACE_NSIZE] = 
{
	STRACE_IMAGE01,
	STRACE_IMAGE02,
	STRACE_IMAGE11,
	STRACE_IMAGE12,
	STRACE_IMAGE21,
	STRACE_IMAGE22,

	STRACE_STRIKE01,
	STRACE_STRIKE02,
	STRACE_STRIKE11,
	STRACE_STRIKE12,

	STRACE_ARROW01,

	STRACE_RAIN01,
	STRACE_RAIN02,
	STRACE_RAIN03,

	STRACE_BLOOD01,

	STRACE_ENERGY01,
	STRACE_ENERGY02,
	STRACE_ENERGY03,
};

#define D3DFVF_MESHVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

STRACOR::STRACOR () :
	m_vPos(0,0,0),
	m_vNormal(1,0,0)
{
	memset( m_szName, 0, sizeof(char)*STRACE_NSIZE );
}

STRACOR::STRACOR ( const char* _szName, const SVERTEXINFLU &_sVertexInflu )
	: m_vPos(0,0,0)
	, m_vNormal(1,0,0)
{
	StringCchCopy ( m_szName, STRACE_NSIZE, _szName );

	m_sVertexInflu = _sVertexInflu;
}

STRACOR::STRACOR ( const STRACOR& _sTracor ) :
	m_vNormal(1,0,0)
{
	StringCchCopy ( m_szName, STRACE_NSIZE, _sTracor.m_szName );
	m_vPos = _sTracor.m_vPos;
	m_sVertexInflu = _sTracor.m_sVertexInflu;
}

bool STRACOR::operator== ( const STRACOR& lvalue )
{
	return !strcmp(m_szName,lvalue.m_szName);
}

//TextureResource		m_textureRes_mask;	// R-Fake_SSS,			G-CubeMap_Mask,				B-CubeMap_Fresnel,		A-NM_R
LPD3DXEFFECT SMATERIAL_PIECE::SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TSTRING strFX = 
		"//-------------------------------------------------------------------------------------- \r\n"
		"// File: FrameMesh.fx \r\n"
		"//       shhan \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Global variables \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"

		"texture g_BaseTexture; \r\n"
		"sampler BaseTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_BaseTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"texture g_SpecTexture; \r\n"
		"sampler SpecTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_SpecTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"texture g_CubeTexture; \r\n"
		"sampler CubeTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_CubeTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS( float2 texCoord : TEXCOORD0, uniform bool bLevelTex, uniform bool bCubeTex ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"   float4 vAlbedo = tex2D( BaseTexSampler, texCoord ); \r\n"
		"   float4 vSpec = tex2D( SpecTexSampler, texCoord ); \r\n"
		"   float4 vCube = tex2D( CubeTexSampler, texCoord ); \r\n"
		"   float4 vColor = 0.f; \r\n"
		"	if ( vAlbedo.r > vAlbedo.g ) \r\n"
		"	{ \r\n"
		"		if ( vAlbedo.g > vAlbedo.b ) \r\n"
		"		{ \r\n"
		"			float fIntensity = saturate((vAlbedo.r - 0.5f)*12.f); \r\n"		// ���� �Ǻ� ��ĳ������ �̷��� ��.
		"			float fDiff1 = vAlbedo.r - vAlbedo.g; \r\n"
		"			float fDiff2 = vAlbedo.g - vAlbedo.b; \r\n"
		"			if ( fDiff1 >= fDiff2 ) \r\n"
		"			{ \r\n"
		"				 fIntensity *= fDiff2/fDiff1; \r\n"
		"			} \r\n"
		"			else \r\n"
		"			{ \r\n"
		"				 fIntensity *= fDiff1/fDiff2; \r\n"
		"			} \r\n"

		// �ʹ� 20% ������ ������ �˸´�. 51 / 255
		"			fIntensity *= ( 0.2f - abs( min ( fDiff1 - 0.2f,  0.2f ) ) ) * 5.f; \r\n"
		"			fIntensity *= ( 0.2f - abs( min ( fDiff2 - 0.2f,  0.2f ) ) ) * 5.f; \r\n"

		// ���ʿ��� ���� �ʹ� ���� ��������, ������ �� �Ǵ� �͵��� ��ġ�� �����ش�.
		"			fIntensity = saturate(fIntensity*8.f); \r\n"

		// Specular ���°� ���ֵ��� �Ѵ�.
		"			if ( bLevelTex ) \r\n"
		"				fIntensity *= saturate(1.f - vSpec.x*4.f); \r\n"

		// Fake_SSS
		// ������.
		"			vColor = saturate(fIntensity*8.f); \r\n"
		"		} \r\n"
		"	} \r\n"

		// CubeMap_Mask
		"	if ( bCubeTex ) \r\n"
		"		vColor.y = vCube.w; \r\n"
		"	else \r\n"
		"		vColor.y = 1.f; \r\n"

		// CubeMap_Fresnel
		"	vColor.z = 0.25f; \r\n"		// ���� *4�� �ϰԵ�. (0~1) -> (0~4) ������ ��.

		// ���Ĵ� 1��.
		"	vColor.w = 1.f; \r\n"

		"   return vColor; \r\n"
		"} \r\n"

		//	Level	Cube
		// 0 false, false
		// 1 true, false
		// 2 false, true
		// 3 true, true

		"technique Base \r\n"
		"{ \r\n"
		"   //------------------------------------------------------------------------------------- \r\n"
		"   // �⺻ _ 0 - PS_2_0 �̻� ���� \r\n"
		"   pass p0 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( false, false ); \r\n"
		"   } \r\n"
		"   pass p1 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( true, false ); \r\n"
		"   } \r\n"
		"   pass p2 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( false, true ); \r\n"
		"   } \r\n"
		"   pass p3 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( true, true ); \r\n"
		"   } \r\n"
		"} \r\n";

	LPD3DXEFFECT pFX(NULL);
	UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
	D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );

	return pFX;
}

//TextureResource		m_textureRes_mask2;	// R-Level_Mask,	G-Spec_Intensity,		B-Spec_Power,		A-NM_G
LPD3DXEFFECT SMATERIAL_PIECE::SetFX_mask2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TSTRING strFX = 
		"//-------------------------------------------------------------------------------------- \r\n"
		"// File: FrameMesh.fx \r\n"
		"//       shhan \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		" \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"// Global variables \r\n"
		"//-------------------------------------------------------------------------------------- \r\n"
		"texture g_BaseTexture; \r\n"
		"sampler BaseTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_BaseTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"texture g_SpecTexture; \r\n"
		"sampler SpecTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_SpecTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"texture g_CubeTexture; \r\n"
		"sampler CubeTexSampler = sampler_state  \r\n"
		"{ \r\n"
		"	Texture = (g_CubeTexture); \r\n"
		"}; \r\n"
		" \r\n"
		"//---------------------------------------------------------------------------------------------------------- \r\n"
		"//					P	S \r\n"
		"// ���� \r\n"
		"float4 PS( float2 texCoord : TEXCOORD0, uniform bool bLevelTex, uniform bool bCubeTex ) : COLOR0  \r\n"
		"{ \r\n"
		"   // Texture \r\n"
		"   float4 vSpec = tex2D( SpecTexSampler, texCoord ); \r\n"
		"   float4 vCube = tex2D( CubeTexSampler, texCoord ); \r\n"
		"   float4 vColor = 0.f; \r\n"

		// Level_Mask
		// ����ŧ�������� ���� ������ �־���´�.
		// ������ ���ϰ� �ؽ��Ŀ� �ְ�, ���α׷��� 4�� ������ ���� �����ϴٺ��� 4.f �� ���ϰ� �Ǿ���.
		"	if ( bLevelTex ) \r\n"
		"		vColor.x = vSpec.w * 4.f; \r\n"

		// Spec_Intensity
		// �������� CubeMap ���� �ѰŰ� ������ �ǽð����� ����ŧ���� �Ѵ�. �����ִٺ��� 4.f �� ���ϰ� �Ǿ���.
		"	if ( bLevelTex ) \r\n"
		"		vColor.y = vSpec.w * 4.f; \r\n"

		// Spec_Power
		"	vColor.z = 0.1f; \r\n"	// ���� *100 �� �ϰԵ�. (0~1) -> (0~100) ������ ��.

		// ���Ĵ� 1��.
		"	vColor.w = 1.f; \r\n"

		"   return vColor; \r\n"
		"} \r\n"
		" \r\n"

		//	Level	Cube
		// 0 false, false
		// 1 true, false
		// 2 false, true
		// 3 true, true

		"technique Base \r\n"
		"{ \r\n"
		"   //------------------------------------------------------------------------------------- \r\n"
		"   // �⺻ _ 0 - PS_2_0 �̻� ���� \r\n"
		"   pass p0 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( false, false ); \r\n"
		"   } \r\n"
		"   pass p1 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( true, false ); \r\n"
		"   } \r\n"
		"   pass p2 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( false, true ); \r\n"
		"   } \r\n"
		"   pass p3 \r\n"
		"   { \r\n"
		"		PixelShader = compile ps_2_0 PS( true, true ); \r\n"
		"   } \r\n"
		"} \r\n";

	LPD3DXEFFECT pFX(NULL);
	UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
	D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );

	return pFX;
}

static LPD3DXEFFECT g_pFX_mask1(NULL);
static LPD3DXEFFECT g_pFX_mask2(NULL);

void SMATERIAL_PIECE::OnResetDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE ( g_pFX_mask1 );
	SAFE_RELEASE ( g_pFX_mask2 );
	g_pFX_mask1 = SetFX( pd3dDevice );
	g_pFX_mask2 = SetFX_mask2( pd3dDevice );
}

void SMATERIAL_PIECE::OnLostDevice_STATIC ()
{
	SAFE_RELEASE ( g_pFX_mask1 );
	SAFE_RELEASE ( g_pFX_mask2 );
}

void SMATERIAL_PIECE::CreateBakeMaskDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
										   LPD3DXEFFECT pFX, 
										   LPDIRECT3DTEXTUREQ& pBakeMaskTex, 
										   int nDivision,
										   LPDIRECT3DTEXTUREQ pLevelTex,
										   LPDIRECT3DTEXTUREQ pCubeTex )
{
	// ���ٸ� ���鵵�� ����.
	// FakeSSS ������ ����� ����Ѵ�.
	LPDIRECT3DTEXTUREQ	pBakeMaskTexTemp(NULL);
	LPDIRECT3DSURFACEQ	pBakeMaskSuf(NULL);
	DWORD dwWidth = m_textureRes.GetInfo().Width/nDivision;
	DWORD dwHeight = m_textureRes.GetInfo().Height/nDivision;

	if ( RENDERPARAM::g_emTextureQuality == TEXTURE_QUALITY_MID )
	{
		dwWidth /= 2;
		dwHeight /= 2;
	}
	else if( RENDERPARAM::g_emTextureQuality == TEXTURE_QUALITY_LOW )
	{
		dwWidth /= 4;
		dwHeight /= 4;
	}

	HRESULT hr = pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pBakeMaskTexTemp, NULL );
	if( FAILED(hr) )
	{
		return;
	}
	hr = pBakeMaskTexTemp->GetSurfaceLevel( 0, &pBakeMaskSuf );
	if( FAILED(hr) )
	{
		SAFE_RELEASE( pBakeMaskTexTemp );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//						BeginScene
	//						���� ����
	// pd3dDevice->BeginScene �� ������ ȣ�� �Ǿ������� hr �� �߸��� ���� ���� �� �ִ�.
	HRESULT hr_BeginPass = pd3dDevice->BeginScene();




	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	pd3dDevice->SetRenderTarget( 0, pBakeMaskSuf );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );

	float fMicroOffsetW = 0.5f/(float)dwWidth;
	float fMicroOffsetH = 0.5f/(float)dwHeight;

	VERTEXRHW	sVertBase[4];
	sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
	sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
	sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
	sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );

	sVertBase[0].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetW, 0.f+fMicroOffsetH );
	sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetW, 0.f+fMicroOffsetH );
	sVertBase[2].vTex = D3DXVECTOR2 ( 0.f+fMicroOffsetW, 1.f+fMicroOffsetH );
	sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+fMicroOffsetW, 1.f+fMicroOffsetH );

	pFX->SetTexture( "g_BaseTexture", m_textureRes.GetTexture() );

	//// Specular ���� ã�Ƽ� �� ������ �Ǻΰ� �ƴ� ������ �صд�.
	//TextureResource stextureRes_maskS;
	//TSTRING strMaskTexNameS = sc::util::GetSpecularName( m_textureRes.GetFileName().c_str(), _T("_s") );
	//if ( TextureManager::GetInstance().IsPathFile( strMaskTexNameS ) )
	//{
	//	stextureRes_maskS = TextureManager::GetInstance().LoadTexture(
	//		strMaskTexNameS,
	//		false,
	//		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
	//		0,
	//		D3DFMT_UNKNOWN,
	//		TEXTURE_RESIZE_NONE,
	//		TEXTURE_CHAR,
	//		false,
	//		FALSE );

	//	pFX->SetTexture( "g_SpecTexture", stextureRes_maskS.GetTexture() );
	//}
	//else
	//{
	//	pFX->SetTexture( "g_SpecTexture", NSMaterialManager::GetBlackTexture() );
	//}

	pFX->SetTexture( "g_SpecTexture", pLevelTex );
	pFX->SetTexture( "g_CubeTexture", pCubeTex );

	DWORD dwZWriteEnable;
	DWORD dwZEnable;
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,	    &dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_ZENABLE,		    &dwZEnable );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,		&dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );


	D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("Base");
	if ( hTechniqueHandle )
	{
		//////////////////////////////////////////////////////////////////////////
		//						SetTechnique
		pFX->SetTechnique(hTechniqueHandle);

		//////////////////////////////////////////////////////////////////////////
		// PixelShader�� �� ��� �̰� ��������Ѵ�.
		// DrawPrimitiveUP �� �� ��� SetFVF �� �ʿ��Ѱ� ����.
		DWORD dwFVF;
		pd3dDevice->GetFVF(&dwFVF);
		pd3dDevice->SetFVF(VERTEXRHW::FVF);

		//	Level	Cube
		// 0 false, false
		// 1 true, false
		// 2 false, true
		// 3 true, true
		UINT unPass(0);
		if ( pLevelTex )
		{
			if ( pCubeTex )
			{
				unPass = 3;
			}
			else
			{
				unPass = 1;
			}
		}
		else if ( pCubeTex )
		{
			unPass = 2;
		}

		UINT numPasses;
		pFX->Begin( &numPasses, 0 );
		if( unPass < numPasses )
		{
			pFX->BeginPass( unPass );
			{
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
			}
			pFX->EndPass();
		}
		pFX->End();

		pd3dDevice->SetFVF(dwFVF);
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    dwZEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		dwFogEnable );

	

	//////////////////////////////////////////////////////////////////////////
	//						EndScene
	//						���� ����
	if ( SUCCEEDED( hr_BeginPass ) )
	{
		pd3dDevice->EndScene();
	}
	else
	{
		int a=0;
	}



	SAFE_RELEASE( pBakeMaskSuf );

	//////////////////////////////////////////////////////////////////////////
	//					�ӵ��� �������� �뷮�� ũ��. Release ������ �ִ�.
	//
	// [shhan][2014.09.04] �������� �ٸ� ������ �� �� ���� ���� ����ٰ� ������.
	//						NSTexCompress::Compress ���� ���� ���ϰ� �����Ƿ� ��¿ �� ���� D3DPOOL_DEFAULT �� ���.
	//
	SAFE_RELEASE( pBakeMaskTex );
	pBakeMaskTex = reinterpret_cast<LPDIRECT3DTEXTURE9>( pBakeMaskTexTemp );

	//////////////////////////////////////////////////////////////////////////
	//					�ӵ��� ������. �뷮�� �پ���. Release ������ ����.
	//
	//LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
	//NSTexCompress::Compress
	//(
	//	pd3dDevice,
	//	dwWidth,
	//	dwHeight,
	//	0,
	//	1,
	//	D3DFMT_DXT1,
	//	pBakeMaskTexTemp,
	//	pTexNew
	//);

	//SAFE_RELEASE ( pBakeMaskTexTemp );

	//SAFE_RELEASE( pBakeMaskTex );
	//pBakeMaskTex = reinterpret_cast<LPDIRECT3DTEXTURE9>( pTexNew );
}

void SMATERIAL_PIECE::CreateBakeMask( LPDIRECT3DDEVICEQ pd3dDevice,
									 LPDIRECT3DTEXTUREQ pLevelTex,
									 LPDIRECT3DTEXTUREQ pCubeTex )
{
	if ( !m_textureRes.GetTexture() )
	{
		LoadTexture();
	}

	// PixelShader_2_0 �̻� ���鵵�� �Ѵ�.
	if( m_textureRes.GetTexture() && RENDERPARAM::g_bPixelShader_2 && g_pFX_mask1 )
	{
		if ( !m_textureRes_mask.GetTexture() )
		{
			CreateBakeMaskDetail( pd3dDevice, g_pFX_mask1, m_pBakeMaskTex, 4, pLevelTex, pCubeTex );
			
			//// �ؽ��ĸ� ������.
			//TSTRING strMaskTexName = sc::util::GetName_ExtErase( m_textureRes.GetFileName() );
			//strMaskTexName += _T("_mask.dds");
			//D3DXSaveTextureToFile( strMaskTexName.c_str(), D3DXIFF_DDS, m_pBakeMask2Tex, NULL );
			//D3DXSaveTextureToFile( m_textureRes.GetFileName().c_str(), D3DXIFF_DDS, m_textureRes.GetTexture(), NULL );
		}
	}
}

void SMATERIAL_PIECE::CreateBakeMask2( LPDIRECT3DDEVICEQ pd3dDevice,
									 LPDIRECT3DTEXTUREQ pLevelTex,
									 LPDIRECT3DTEXTUREQ pCubeTex )
{
	if ( !m_textureRes.GetTexture() )
	{
		LoadTexture();
	}

	// PixelShader_2_0 �̻� ���鵵�� �Ѵ�.
	if( m_textureRes.GetTexture() && RENDERPARAM::g_bPixelShader_2 && g_pFX_mask2 )
	{
		if ( !m_textureRes_mask2.GetTexture() )
		{
			CreateBakeMaskDetail( pd3dDevice, g_pFX_mask2, m_pBakeMask2Tex, 4, pLevelTex, pCubeTex );

			//// �ؽ��ĸ� ������.
			//TSTRING strMaskTexName = sc::util::GetName_ExtErase( m_textureRes.GetFileName() );
			//strMaskTexName += _T("_mask.dds");
			//D3DXSaveTextureToFile( strMaskTexName.c_str(), D3DXIFF_DDS, m_pBakeMask2Tex, NULL );
			//D3DXSaveTextureToFile( m_textureRes.GetFileName().c_str(), D3DXIFF_DDS, m_textureRes.GetTexture(), NULL );
		}
	}
}

void SMATERIAL_PIECE::CreateBakeMaskALL ( LPDIRECT3DDEVICEQ pd3dDevice,
										 LPDIRECT3DTEXTUREQ pLevelTex,
										 LPDIRECT3DTEXTUREQ pCubeTex )
{
	//////////////////////////////////////////////////////////////////////////
	//	mask
	TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTexture, _T("_mask1") );
	strMaskTexName = sc::util::ChangeExtName( strMaskTexName, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName ) )
	{
		m_textureRes_mask = TextureManager::GetInstance().LoadTexture(
			strMaskTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}
	else
	{
		CreateBakeMask( pd3dDevice, pLevelTex, pCubeTex );
	}

	//////////////////////////////////////////////////////////////////////////
	//	mask2
	TSTRING strMaskTexName2 = sc::util::GetSpecularName( m_strTexture, _T("_mask2") );
	strMaskTexName2 = sc::util::ChangeExtName( strMaskTexName2, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName2 ) )
	{
		m_textureRes_mask2 = TextureManager::GetInstance().LoadTexture(
			strMaskTexName2,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}
	else
	{
		CreateBakeMask2( pd3dDevice, pLevelTex, pCubeTex );
	}

	//////////////////////////////////////////////////////////////////////////
	//	maskh
	strMaskTexName = sc::util::GetSpecularName( m_strTexture, _T("_maskh") );
	strMaskTexName = sc::util::ChangeExtName( strMaskTexName, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName ) )
	{
		m_textureRes_HairMask = TextureManager::GetInstance().LoadTexture(
			strMaskTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}
	else
	{
		strMaskTexName = _T("HairShiftTex.dds");
		if ( TextureManager::GetInstance().IsPathFile( strMaskTexName ) )
		{
			m_textureRes_HairMask = TextureManager::GetInstance().LoadTexture(
				strMaskTexName,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_CHAR,
				false,
				FALSE );
		}
	}
}

void SMATERIAL_PIECE::LoadMaskTex ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//////////////////////////////////////////////////////////////////////////
	//	mask
	TSTRING strMaskTexName = sc::util::GetSpecularName( m_strTexture, _T("_mask1") );
	strMaskTexName = sc::util::ChangeExtName( strMaskTexName, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName ) )
	{
		m_textureRes_mask = TextureManager::GetInstance().LoadTexture(
			strMaskTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}

	//////////////////////////////////////////////////////////////////////////
	//	mask2
	TSTRING strMaskTexName2 = sc::util::GetSpecularName( m_strTexture, _T("_mask2") );
	strMaskTexName2 = sc::util::ChangeExtName( strMaskTexName2, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName2 ) )
	{
		m_textureRes_mask2 = TextureManager::GetInstance().LoadTexture(
			strMaskTexName2,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}

	//////////////////////////////////////////////////////////////////////////
	//	maskh
	strMaskTexName = sc::util::GetSpecularName( m_strTexture, _T("_maskh") );
	strMaskTexName = sc::util::ChangeExtName( strMaskTexName, _T("dds") );
	if ( TextureManager::GetInstance().IsPathFile( strMaskTexName ) )
	{
		m_textureRes_HairMask = TextureManager::GetInstance().LoadTexture(
			strMaskTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_CHAR,
			true,
			FALSE );
	}
}

void SMATERIAL_PIECE::PrecalculateForMaterialSystem( EffectMaterialData* pEffMaterialData, DWORD dwMaterialID ) const
{
	if ( m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_CUBEMAP )
	{
		pEffMaterialData->m_bCubeBlend = TRUE;

		if ( dwMaterialID < pEffMaterialData->m_vecEnableCubeBlend.size() )
		{
			pEffMaterialData->m_vecEnableCubeBlend[dwMaterialID] = TRUE;
		}
		else
		{
			pEffMaterialData->m_vecEnableCubeBlend.push_back( TRUE );
		}
	}
	else
	{
		if ( dwMaterialID < pEffMaterialData->m_vecEnableCubeBlend.size() )
		{
			// DxEffCharOverlay ���� TRUE �� ���� �Ȱ� �̰����� �����鼭 FALSE �� �Ǵ� ������ �־���.
			//pEffMaterialData->m_vecEnableCubeBlend[dwMaterialID] = FALSE;
		}
		else
		{
			pEffMaterialData->m_vecEnableCubeBlend.push_back( FALSE );
		}
	}

	if ( m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_NORMALMAP )
	{
		if ( dwMaterialID < pEffMaterialData->m_vecEnableNormalMap.size() )
		{
			pEffMaterialData->m_vecEnableNormalMap[dwMaterialID] = TRUE;
		}
		else
		{
			pEffMaterialData->m_vecEnableNormalMap.push_back( TRUE );
		}
	}
	else
	{
		if ( dwMaterialID < pEffMaterialData->m_vecEnableNormalMap.size() )
		{
			pEffMaterialData->m_vecEnableNormalMap[dwMaterialID] = FALSE;
		}
		else
		{
			pEffMaterialData->m_vecEnableNormalMap.push_back( FALSE );
		}
	}

	if ( m_dwMaterialSystemFlags & SMATERIAL_PIECE::SKIN_MS_TYPE_HAIR )
	{
		pEffMaterialData->m_nEnableHairIndex = dwMaterialID;

		//pEffMaterialData->m_rHairSpecColor_TangentOffsetTRT = &m_vHairSpecColor_DirectionIntensity;
		//pEffMaterialData->m_rHairShiftUV_SpecExp_R_TRT = &m_vHairShiftUV_SpecExp_DL_CL;
	}
	else
	{
		//pEffMaterialData->m_rHairSpecColor_TangentOffsetTRT = NULL;
		//pEffMaterialData->m_rHairShiftUV_SpecExp_R_TRT = NULL;
	}
}

HRESULT SMATERIAL_PIECE::LoadTexture()
{
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_strTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        EMMM_CHAR_MIPMAP,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	// ���� �ؽ��İ� ���̴°� �⺻ �������� ���� �ʵ��� �Ѵ�.
	TSTRING strName = sc::util::GetName_ExtErase( m_strTexture.c_str() );
	sc::string::lowerString(strName);
	if ( strName == _T("extrawing_a") )	//||
		//strName == _T("black_a") )
	{
		m_bBaseRenderOff = TRUE;
	}

	// Material System ���� ���������� ������ �ؼ� �̷��� ��.
    if( m_textureRes.GetTexture() )
	{
		//	Note : �ؽ��� �б⿡ �����ÿ� �� ���͸����� �ʱ� ���� ����.
		TEXTURE_ALPHA emType = m_textureRes.GetAlphaType();
		if( emType==EMTT_ALPHA_HARD )			m_dwFlags |= DXMATERIAL::FLAG_ALPHA_HARD;
		else if( emType==EMTT_ALPHA_SOFT )		m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
		else if( emType==EMTT_ALPHA_SOFT01 )	m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
		else if( emType==EMTT_ALPHA_SOFT02 )	m_dwFlags |= DXMATERIAL::FLAG_ALPHA_SOFT;
	}

	return S_OK;
}

BOOL SMATERIAL_PIECE::IsLoadingOK_MainTexLoad()
{
	if ( m_strTexture.empty() )
		return TRUE;

	if ( m_textureRes.GetTexture() )
		return TRUE;

	// [shhan][2014.10.01] �ؽ��� ��ü�� ���� ������ ��� �Ҽ��� ����.
	//						��� Ÿ�� ���� �׳� ������ �ϵ��� �Ѵ�.
	if ( !TextureManager::GetInstance().IsPathFile( m_strTexture ) )
		return TRUE;

	LoadTexture();

	return FALSE;
}

void SMATERIAL_PIECE::InvalidateDeviceObjects ()
{
	SAFE_RELEASE ( m_pBakeMaskTex );
	SAFE_RELEASE ( m_pBakeMask2Tex );
}

void SMATERIAL_PIECE::SaveFile ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		//SFile << m_fSkinSpecIntensity;	// �Ǻ� Spec Intensity
		//SFile << m_fSkinSpecPower;		// �Ǻ� Spec Power
		//SFile << m_fSSS_Intensity;

		SFile << m_dwFlags;
		//SFile << m_dwZBias;
		//SFile << m_dwBlendFlag;

		SFile << m_dwMaterialSystemFlags;
		SFile.WriteBuffer( &m_vHairSpecColor_DirectionIntensity, sizeof(D3DXVECTOR4) );
		SFile.WriteBuffer( &m_vHairShiftUV_SpecExp_DL_CL, sizeof(D3DXVECTOR4) );

		SFile << m_fClothDampingCoefficient;
		SFile << m_nClothSolverFrequency;
		SFile.WriteBuffer( &m_vClothConfiguration, sizeof(D3DXVECTOR4) );

		DWORD dwLength = static_cast<DWORD> ( m_strTexture.length() + 1 );
		SFile << dwLength;
		SFile.WriteBuffer ( m_strTexture.c_str(), dwLength );

	}
	SFile.EndBlock();
}

void SMATERIAL_PIECE::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwLength;
	SFile >> dwLength;
	char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
	SFile.ReadBuffer ( szBuffer, dwLength );
	m_strTexture = szBuffer;

	SFile >> m_dwFlags;
	SFile >> m_dwZBias;

	m_fZBias = (float)m_dwZBias*0.00001f;
}

void SMATERIAL_PIECE::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwLength;

	SFile >> m_dwFlags;
	SFile >> m_dwZBias;

	m_fZBias = (float)m_dwZBias*0.00001f;

	SFile >> dwLength;
	char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
	SFile.ReadBuffer ( szBuffer, dwLength );
	m_strTexture = szBuffer;
}

void SMATERIAL_PIECE::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwLength;

	SFile >> m_dwFlags;
	SFile >> m_dwZBias;
	SFile >> m_dwBlendFlag;

	m_fZBias = (float)m_dwZBias*0.00001f;

	SFile >> dwLength;
	char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
	SFile.ReadBuffer ( szBuffer, dwLength );
	m_strTexture = szBuffer;
}


void SMATERIAL_PIECE::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwLength;

	SFile >> m_dwFlags;
	//SFile >> m_dwZBias;
	//SFile >> m_dwBlendFlag;

	SFile >> m_dwMaterialSystemFlags;
	SFile.ReadBuffer( &m_vHairSpecColor_DirectionIntensity, sizeof(D3DXVECTOR4) );
	SFile.ReadBuffer( &m_vHairShiftUV_SpecExp_DL_CL, sizeof(D3DXVECTOR4) );

	//m_fZBias = (float)m_dwZBias*0.00001f;

	SFile >> dwLength;
	char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
	SFile.ReadBuffer ( szBuffer, dwLength );
	m_strTexture = szBuffer;
}

void SMATERIAL_PIECE::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwLength;

	SFile >> m_dwFlags;

	SFile >> m_dwMaterialSystemFlags;
	SFile.ReadBuffer( &m_vHairSpecColor_DirectionIntensity, sizeof(D3DXVECTOR4) );
	SFile.ReadBuffer( &m_vHairShiftUV_SpecExp_DL_CL, sizeof(D3DXVECTOR4) );

	SFile >> m_fClothDampingCoefficient;
	SFile >> m_nClothSolverFrequency;
	SFile.ReadBuffer( &m_vClothConfiguration, sizeof(D3DXVECTOR4) );

	SFile >> dwLength;
	char* szBuffer = (char*) _alloca ( sizeof(char)*dwLength );
	SFile.ReadBuffer ( szBuffer, dwLength );
	m_strTexture = szBuffer;
}

void SMATERIAL_PIECE::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVERSION, dwBLOCKSIZE;
	SFile >> dwVERSION;
	SFile >> dwBLOCKSIZE;

	switch ( dwVERSION )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case 0x0102:	LoadFile_0102( SFile, pd3dDevice );	break;
	case 0x0103:	LoadFile_0103( SFile, pd3dDevice );	break;
	case VERSION:	LoadFile_0104( SFile, pd3dDevice );	break;
	default:
		{
			GASSERT("SMATERIAL_PIECE::LoadFile() This version is not supported.");
			SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
		}
		break;
	};		

	LoadTexture ();
}


DxSkinPieceContainer& DxSkinPieceContainer::GetInstance()
{
	static DxSkinPieceContainer Instance;
	return Instance;
}

const DWORD	DxSkinPiece::VERSION = 0x00000116;
const char	DxSkinPiece::m_szPIECETYPE[PIECE_SIZE][64] =
{
	"head",
	"upper body",
	"lower body",
	"hand",
	"Basic tools",
	"Additional tools",
	"foot",
	"hair",
	"hat",
	"S.R.C",
	"Cape",
    "Gride_Up",
	"Gride_Down",
    "Gride_Hand",
	"Gride_Foot",
	"Basic tools2",
	"Additional tools2",
	"hat's hair",
};

const char	DxSkinPiece::m_szATTBONETYPE[ATTBONE_SIZE][64] =
{
	"BODY",
	"PART0",
	"PART1",
	"PART2",
	"PART3",
	"PART4",
	"PART5",
	"PART6",
	"PART7",
};

const char	DxSkinPiece::m_szV_PARTSTYPE[PART_V_SIZE][64] =
{
	"Not used",
	"entity",
	"head",
	"tire",
	"Mahura",
	"back sheet02",
	"accessories01",
	"accessories02",
};


DxSkinPiece::DxSkinPiece ()
	: m_szFileName(NULL)
	
	, m_dwRef(0)
	
	, m_szXFileName(NULL)

	, m_szSkeleton(NULL)

	, m_emPieceType(PIECETYPE_CHARACTER)

	, m_emType(PIECE_HEAD)
	, m_emWeaponWhereBack(EMPEACE_WEAPON_NONE)
	, m_szMeshName(NULL)

	, m_dwFlags(NULL)
	, m_dwFlag(0L)
	, m_pmcMesh(NULL)
	, m_dwVertexNUM(0)

	, m_dwMaterialNum(0)
	, m_arrayMaterialPiece(NULL)

	, m_strUserBackSlot(_T(""))
	, m_bKeepUserBackSlot(FALSE)

	, m_bCompletedLOAD(FALSE)
	, m_bActiveErrorLog(FALSE)

	, m_bMaterialSystem(TRUE)

	, m_apOverlayTex(NULL)

	, m_scpEffectMaterialData(new EffectMaterialData)
	, m_scpSkeleton(new DxSkeletonMaintain)
	, m_scpPhysXCloth(new DxPhysXCloth)

	, m_bMakeMaskMap(FALSE)
	, m_emHairHairOption(HAT_HAIR_BASE)
{
	m_dwFlag |= EMCF_SHADOW;

	D3DXMatrixIdentity( &m_matUserBackSlot );
}

DxSkinPiece::~DxSkinPiece ()
{
	SAFE_DELETE_ARRAY(m_szFileName);
	SAFE_DELETE_ARRAY(m_szXFileName);
	SAFE_DELETE_ARRAY(m_szSkeleton);
	SAFE_DELETE_ARRAY(m_szMeshName);

	SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT.erase ( m_vecEFFECT.begin(), m_vecEFFECT.end() );
	m_vecEFFECT.clear();

	//std::for_each ( m_vecEFFECT_CharPart.begin(), m_vecEFFECT_CharPart.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT_CharPart.erase ( m_vecEFFECT_CharPart.begin(), m_vecEFFECT_CharPart.end() );

	// don't delete m_pmcMesh

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE( m_apOverlayTex[i] );
	}
	SAFE_DELETE_ARRAY ( m_apOverlayTex );
}

void DxSkinPiece::SumEffFlag ()
{
	m_dwFlags = NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		m_dwFlags |= m_vecEFFECT[n]->GetFlag();
	}
}

void DxSkinPiece::AddEffList ( DxEffChar* pEffChar )
{
	GASSERT(pEffChar);

	m_vecEFFECT.push_back ( pEffChar );
	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	//	flag�� ������ ȿ���� ��� flag�� �ջ�.
	m_dwFlags |= pEffChar->GetFlag();
}

void DxSkinPiece::DelEffList ( LPDIRECT3DDEVICEQ pd3dDevice, DxEffChar* pEffChar )
{
	if ( !pEffChar )
		return;

	DWORD dwFlag = pEffChar->GetFlag();
	DWORD dwOrder = pEffChar->GetStateOrder();

	//
	//mjeon.migration
	//
	//GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), pEffChar/*dwOrder*/, DXEFFCHAR_OPER() );	
	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), dwOrder, DXEFFCHAR_OPER() );	

	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > dwOrder )	break;	//	�������� �ʴ´�.
		if ( pEFFECT!=pEffChar )					continue;

		//	����.
		SAFE_DELETE(pEFFECT);
		m_vecEFFECT.erase(pos);
		break;
	}

    //BOOL bUsedVisualMaterial = dwFlag & EMECF_VISUAL_MATERIAL;

	//	flag�� �־��� ȿ���� ������ ��� flag�� �ٽ� �ջ�.
	if ( dwFlag )		
        SumEffFlag ();

//    if ( bUsedVisualMaterial )
//    {
//        // VisualMaterial �� �����ȰŶ��..
//        if ( !(m_dwFlags & EMECF_VISUAL_MATERIAL) )
//        {
//            if ( m_pmcMesh )
//                m_pmcMesh->GenerateSkinnedMeshNON_public( pd3dDevice );
//        }
//    }
}

void DxSkinPiece::ClearEffList ()
{
	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );	
	m_vecEFFECT.clear();
}

// Bone Matrix�� �����ش�.
D3DXMATRIX DxSkinPiece::GetInfluenceMatrix()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	if( !m_skeletonRes.IsValid() )	
		return matWorld;

	DxBoneTrans* pBoneTran = m_skeletonRes.get()->FindBone_t( m_strUserBackSlot );
	if( !pBoneTran )
		return matWorld;

	D3DXMatrixMultiply( &matWorld, &m_matUserBackSlot, &pBoneTran->matCombined );
	return matWorld;
}

// Set Bone Matrix
void DxSkinPiece::SetChangeMatrix( const D3DXMATRIX& matWorld )
{
	if( !m_skeletonRes.IsValid() )	
		return;

	DxBoneTrans* pBoneTran = m_skeletonRes.get()->FindBone_t( m_strUserBackSlot );
	if( !pBoneTran )
		return;

	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &pBoneTran->matCombined );
	D3DXMatrixMultiply( &m_matUserBackSlot, &matWorld, &matInverse );
}

HRESULT DxSkinPiece::SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	MIN_ASSERT(szXFileName);
	MIN_ASSERT(szSkeleton);
	MIN_ASSERT(pd3dDevice);

	SAFE_DELETE_ARRAY(m_szSkeleton);
	int nStrLen = (int) (_tcslen(szSkeleton)+1);
	m_szSkeleton = new TCHAR[nStrLen];
	StringCchCopy( m_szSkeleton, nStrLen, szSkeleton );
	m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeleton( m_szSkeleton, TRUE );
	if ( !m_skeletonRes.IsValid() )
	{
		CDebugSet::ToLogFile( sc::string::format( _T("%1% Calling Bone - %2% There is no file."), m_szFileName, szSkeleton ) );
        return E_FAIL;
	}

	CString strFileName(szXFileName);	
	strFileName.MakeLower();

	{
		CString strEncName;
		STRUTIL::ChangeExt( strFileName, strEncName, _T( ".mxf" ) );

		if ( DxSkinMeshManager::GetInstance().ExistFile( TSTRING(strEncName) ) )
		{
			strFileName = strEncName;
		}
	}

	SAFE_DELETE_ARRAY(m_szXFileName);
	nStrLen = (int) (_tcslen(strFileName)+1);
	m_szXFileName = new TCHAR[nStrLen];
	StringCchCopy( m_szXFileName, nStrLen, strFileName.GetString() );

	m_skinMeshRes = DxSkinMeshManager::GetInstance().LoadDxSkinMesh( m_szXFileName, m_szSkeleton, bMeshThread, bTexThread );

    if ( !bMeshThread && !m_skinMeshRes.IsValid() )
	{
		return E_FAIL;
	}

	if ( m_skinMeshRes.IsValid() )
	{
		CString strFile1( m_skinMeshRes.get()->GetBoneFileName() );
		strFile1.MakeLower();

		CString strFile2( m_szSkeleton );
		strFile2.MakeLower();

		if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
		{
			CDebugSet::ToLogFile
			( 
				sc::string::format( _T("Base Bone : %1%, Error skin : %2%, Error Bone : %3% \n"), m_skinMeshRes.get()->GetBoneFileName(), m_szXFileName, m_szSkeleton ) 
			);

			GASSERT( 0 && _T("Skin.x in multiple Bone.x is in use. Please refer to log.") );

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT DxSkinPiece::ReSetSkinMesh (LPDIRECT3DDEVICEQ pd3dDevice)
{
	if ( !m_szSkeleton )
		return S_FALSE;

	if ( !m_szXFileName )
		return S_FALSE;

	DxSkeletonManager::GetInstance().ReLoadSkeleton( m_szSkeleton, m_skeletonRes );
   
    DxSkinMeshManager::GetInstance().ReLoadDxSkinMesh( m_szXFileName, m_szSkeleton, m_skinMeshRes, m_skeletonRes );

	ReAttachSkinMesh( pd3dDevice );

	// ȿ�� �� ����.
	{
		// �������� ȿ�� ���
		GLEFFCHAR_VEC vecEFFECT;
		{
			size_t nSIZE = m_vecEFFECT.size();
			for ( size_t n=0; n<nSIZE; ++n )
			{
				DxEffChar* pEFFECT = m_vecEFFECT[n];
				DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance( pd3dDevice, NULL, this, m_dwMaterialNum );

				if( pNEW_EFFECT )
				{
					vecEFFECT.push_back ( pNEW_EFFECT );
				}
			}
		}

		m_vecEFFECT = vecEFFECT;
		std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );
	}

	// Fixed <-> Shader �� ��ȯ�Ǹ鼭 Material ������ ���� ���ɼ��� �ִ�.
	SetMaterialSetting( pd3dDevice, FALSE, FALSE );

	return S_OK;
}

bool DxSkinPiece::IsValidSkinMesh()
{
	if (m_skinMeshRes.IsValid() == false)
	{
		return false;
	}

	if (m_skinMeshRes.get()->FindMeshContainer(m_szMeshName) == m_pmcMesh)
	{
		return true;
	}

	return false;
}

void DxSkinPiece::ReAttachSkinMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if (m_skinMeshRes.IsValid() == false)
	{
		return;
	}

	if( m_skinMeshRes.Update() )
	{
		m_pmcMesh = m_skinMeshRes.get()->FindMeshContainer ( m_szMeshName );

		if ( !m_pmcMesh )
		{
			if( !m_bActiveErrorLog )
			{
				m_bActiveErrorLog = TRUE;

				// ~.cps ������ ~.x �� ���ӻ� ~.x ���� ������ ���� �ʴ� ������ ����.
				CDebugSet::ToLogFile( sc::string::format( _T("There is no %1%, %2% inside  %3%."), m_szFileName, m_szXFileName, m_szMeshName ) );
			}
			return;
		}

		SetMaterialSetting( pd3dDevice, FALSE, FALSE );
		

		//m_dwVertexNUM = m_pmcMesh->m_dwVertexNUM;
	}
}

// m_pmcMesh ���� ���� ��� FALSE �� ����.
// m_vecEFFECT ���� �ؽ��ĵ��� �ε��� �ȵǾ��� ��� FALSE �� ����.
// �Ϲ� �ؽ��Ĵ� �ε��� �ȵǾ TRUE �� ����.
//
// m_vecEFFECT �� �ؽ��� �ε��� �ȵǸ� FALSE �� �ִ� ������ Mask ���� ������ �� �س���
// �� �Ǿ����� Mask ���� �� �����ؾ� �ϴ��� ���� ���� �����̴�.
//
BOOL DxSkinPiece::SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bSetBaseTexture, BOOL bTexThread )
{
	if ( !m_pmcMesh )
		return FALSE;

	// [shhan][2014.10.22] DxEffCharOverlay �� �ؽ��İ� ��� �ε��Ǿ� �ؼ� �׷���.
	//						DxCharPart �� Overlay ����� Mask ���� �ʿ��ѵ�, �ε��� �Ϸ� �Ǿ� �־�� �Ѵ�.
	//						�׷��� �̰����� ����� �δ� ���̴�.
	//
	// Effect �� �ؽ��� ���� ��ȿ���� Ȯ���Ѵ�.
	// Mask ���� ����� ���ؼ� �׷���.
	for ( DWORD j=0; j<m_vecEFFECT.size(); ++j )
	{
		if ( !m_vecEFFECT[j]->IsEnableEffTexture( bTexThread ) )
			return FALSE;
	}

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE( m_apOverlayTex[i] );
	}

	// ������ �ִ� ���¶� ������ �ϵ��� �Ѵ�.
	if ( m_dwMaterialNum != m_pmcMesh->GetNumMaterials() || bSetBaseTexture )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_dwMaterialNum = m_pmcMesh->GetNumMaterials();
		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];

		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].m_strTexture = m_pmcMesh->m_pMaterialEx[i].m_textureRes.GetFileName();
			//m_pMaterialPiece[i].m_textureRes = m_pmcMesh->m_pMaterialEx[i].m_textureRes;
		}
	}
	else
	{
		// ������ ���� �����ϸ鼭 ����ִ� ���� ���� �ֵ��� �Ѵ�.
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			if ( m_arrayMaterialPiece[i].m_strTexture.empty() )
			{
				m_arrayMaterialPiece[i].m_strTexture = m_pmcMesh->m_pMaterialEx[i].m_textureRes.GetFileName();
			}
		}
	}

	SAFE_DELETE_ARRAY ( m_apOverlayTex );
	m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_apOverlayTex[i] = NULL;
	}

	// Texture Loading
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		if ( !m_arrayMaterialPiece[i].IsLoadingOK_MainTexLoad() )
			return FALSE;
	}

	//// Texture Loading
	//for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	//{
	//	m_arrayMaterialPiece[i].LoadTexture();
	//}

	// ������ �׷��� �����̳ʰ� ����� MaskMap�� ����ϴ� �Ŷ��, �ڵ����� ������ �ʰ� �ִ°� ������ �Ѵ�.
	if ( !m_bMakeMaskMap )
		return TRUE;

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		const TCHAR* pTextureName(NULL);
		LPDIRECT3DTEXTUREQ pLevelTex(NSMaterialManager::GetBlackTexture());
		LPDIRECT3DTEXTUREQ pCubeTex(NSMaterialManager::GetBlackTexture());
		float fTexUpDownPower(1.f);
		for ( DWORD j=0; j<m_vecEFFECT.size(); ++j )
		{
			m_vecEFFECT[j]->ExportEffTexture( pd3dDevice, i, &pLevelTex, &pCubeTex, bTexThread?true:false, pTextureName );
		}

		m_arrayMaterialPiece[i].CreateBakeMaskALL( pd3dDevice, pLevelTex, pCubeTex );
	}

	return TRUE;
}

BOOL DxSkinPiece::SelectMesh ( LPDIRECT3DDEVICEQ pd3dDevice, PSMESHCONTAINER pmcMesh )
{
	GASSERT(pmcMesh);

	m_pmcMesh = (PSMESHCONTAINER) pmcMesh;

	if( !m_pmcMesh->Name )	return FALSE;

	// ���� ���̶�� ���� ������ �ʴ´�.
	TSTRING strName = m_pmcMesh->Name;
	if( m_szMeshName && (strName==m_szMeshName) && m_arrayMaterialPiece )
	{
		return FALSE;
	}

	SAFE_DELETE_ARRAY(m_szMeshName);
	int nStrLen = static_cast<int>( strlen(m_pmcMesh->Name)+1 );
	m_szMeshName = new char[nStrLen];
	StringCchCopy( m_szMeshName, nStrLen, m_pmcMesh->Name );

	//m_pmcMesh->GetNumMaterials();
	//if ( m_pmcMesh->GetNumMaterials() == m_dwMaterialNum && m_pMaterialPiece )
	//{
	//	return;
	//}

	SetMaterialSetting( pd3dDevice, TRUE, FALSE );

	// Tool ������ ���鵵�� �Ѵ�.
	if ( g_bCHAR_EDIT_RUN )
	{
		m_scpPhysXCloth->Import( this );
	}

	return TRUE;
}

SVERTEXINFLU* DxSkinPiece::GetTracePos ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(), STRACOR_OPER() );	
	if ( found == m_vecTrace.end() )
		return NULL;

	if (!strcmp((*found).m_szName, strTrace.c_str()))
		return &(*found).m_sVertexInflu;

	return NULL;
}

void DxSkinPiece::SetTracePos ( std::string strTrace, SVERTEXINFLU &sVertInflu )
{
	DelTracePos( strTrace );

	m_vecTrace.push_back ( STRACOR(strTrace.c_str(),sVertInflu) );
	std::sort( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );
}

void DxSkinPiece::DelTracePos ( std::string strTrace )
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(), STRACOR_OPER() );
	if ( found==m_vecTrace.end() )
		return;

	if ( !strcmp((*found).m_szName,strTrace.c_str()) )
	{
		m_vecTrace.erase( found );
	}
}

HRESULT DxSkinPiece::LoadPiece_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SVERTEXINFLU sTraceArray[10];
	for ( int i=0; i<8; i++ )
	{
		sTraceArray[i].LoadFile ( SFile );
	}

	if ( sTraceArray[0].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE01,sTraceArray[0]) );
	if ( sTraceArray[1].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE02,sTraceArray[1]) );

	if ( sTraceArray[2].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE11,sTraceArray[2]) );
	if ( sTraceArray[3].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE12,sTraceArray[3]) );

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;
		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_dwFlag |= pEffChar->GetFlag();
		m_vecEFFECT.push_back(pEffChar);
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	// Note : Add for Version.106
	m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SVERTEXINFLU sTraceArray[10];
	for ( int i=0; i<10; i++ )
	{
		sTraceArray[i].LoadFile ( SFile );
	}

	if ( sTraceArray[0].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE01,sTraceArray[0]) );
	if ( sTraceArray[1].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE02,sTraceArray[1]) );

	if ( sTraceArray[2].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE11,sTraceArray[2]) );

	if ( sTraceArray[3].m_dwIndex != COL_ERR )
		m_vecTrace.push_back ( STRACOR(STRACE_IMAGE12,sTraceArray[3]) );

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;
		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_dwFlag |= pEffChar->GetFlag();
		m_vecEFFECT.push_back(pEffChar);
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	// Note : Add for Version.106
	m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;
		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_dwFlag |= pEffChar->GetFlag();
		m_vecEFFECT.push_back(pEffChar);
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	// Note : Add for Version.106
	m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;
		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_dwFlag |= pEffChar->GetFlag();
		m_vecEFFECT.push_back(pEffChar);
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	// Note : Add for Version.106
	m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	// Note : Add for Version.106
	m_emWeaponWhereBack = EMPEACE_WEAPON_RSLOT;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}
	
	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}
	
	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );


	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	m_emType = (EMPIECECHAR) dwSize;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	char *szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	char *szXFileName = new char[dwSize];
	SFile.ReadBuffer ( szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	//	Note : ��Ų �޽� �б�.
	//
	hr = SetSkinMesh ( szXFileName, szSkeleton, pd3dDevice, bMeshThread, bTexThread );
	SAFE_DELETE_ARRAY(szXFileName);
	SAFE_DELETE_ARRAY(szSkeleton);
	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	if( m_emPieceType == PIECETYPE_CHARACTER )
		m_emType = (EMPIECECHAR) dwSize;
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		m_emV_PartType = (EMPIECEVEHICLE) dwSize;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0111 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
    HRESULT hr;

    SFile >> m_dwRef;

    DWORD dwSize;

    SFile >> dwSize;
    m_szSkeleton = new char[dwSize];
    SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );

    SFile >> dwSize;
    m_szXFileName = new char[dwSize];
    SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );

    SFile >> dwSize;
    m_szMeshName = new char[dwSize];
    SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );
  
    // LOD �ε�
    SFile >> dwSize;
    m_kLODInfo.resize(dwSize);

    float fDistance = 0.f;
    char temp[MAX_PATH] = {0};

    for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
    {
        SFile >> fDistance;
        m_kLODInfo.kDistance[i] = fDistance;

        SFile >> dwString;
        SFile.ReadBuffer ( temp, sizeof(char) * dwString );
        m_kLODInfo.kMeshName[i] = temp;
    }

    //	Note : ��Ų �޽� �б�.
    //
    TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
    hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );

    if ( FAILED(hr) )	return hr;

    SFile >> m_dwMaterialNum;
    if ( m_dwMaterialNum!=0 )
    {
        SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

        m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
        for ( DWORD i=0; i<m_dwMaterialNum; ++i )
        {
            m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
        }

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

    SFile >> dwSize;
    if( m_emPieceType == PIECETYPE_CHARACTER )
		m_emType = (EMPIECECHAR) dwSize;
    else if( m_emPieceType == PIECETYPE_ATTACHBONE )
        m_emAttBoneType = (EMPIECEATTBONE) dwSize;
    else if( m_emPieceType == PIECETYPE_V_PARTS )
        m_emV_PartType = (EMPIECEVEHICLE) dwSize;

    m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

    SFile >> dwSize;
    m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

    std::string strBuffer;
    SVERTEXINFLU sVertexInflu;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; i++ )
    {
        SFile >> strBuffer;
        sVertexInflu.LoadFile ( SFile );

        m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
    }

    std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

    dwSize = 0;
    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; i++ )
    {
        DWORD TypeID;
        SFile >> TypeID;

        DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
        if ( !pEffChar )	return E_FAIL;

        pEffChar->SetLinkObj ( NULL, this );
        hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
        if (FAILED(hr))
        {
            SAFE_DELETE(pEffChar);
            continue;
        }

        m_vecEFFECT.push_back ( pEffChar );
    }

    std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

    SFile >> m_dwFlag;

    return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0112_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	m_szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szXFileName = new char[dwSize];
	SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	// LOD �ε�
	SFile >> dwSize;
	m_kLODInfo.resize(dwSize);

	float fDistance = 0.f;
	char temp[MAX_PATH] = {0};

	for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
	{
		SFile >> fDistance;
		m_kLODInfo.kDistance[i] = fDistance;

		SFile >> dwString;
		SFile.ReadBuffer ( temp, sizeof(char) * dwString );
		m_kLODInfo.kMeshName[i] = temp;
	}

	//	Note : ��Ų �޽� �б�.
	//
	TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
	hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );

	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	if( m_emPieceType == PIECETYPE_CHARACTER )
	{
		m_emType = (EMPIECECHAR) dwSize;

		// �߰��� PIECE_CAPE �߰��� ���� �� ���� �ʿ�.
		if ( static_cast<int>( m_emType ) >= 10 )
		{
			m_emType = static_cast<EMPIECECHAR>( m_emType + 1 );
		}
	}
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		m_emV_PartType = (EMPIECEVEHICLE) dwSize;

	m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile >> m_strUserBackSlot;
	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile >> m_bKeepUserBackSlot;

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 1 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0114 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	m_szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szXFileName = new char[dwSize];
	SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	// LOD �ε�
	SFile >> dwSize;
	m_kLODInfo.resize(dwSize);

	float fDistance = 0.f;
	char temp[MAX_PATH] = {0};

	for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
	{
		SFile >> fDistance;
		m_kLODInfo.kDistance[i] = fDistance;

		SFile >> dwString;
		SFile.ReadBuffer ( temp, sizeof(char) * dwString );
		m_kLODInfo.kMeshName[i] = temp;
	}

	//	Note : ��Ų �޽� �б�.
	//
	TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
	hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );

	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	if( m_emPieceType == PIECETYPE_CHARACTER )
		m_emType = (EMPIECECHAR) dwSize;
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		m_emV_PartType = (EMPIECEVEHICLE) dwSize;

	m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile >> m_strUserBackSlot;
	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile >> m_bKeepUserBackSlot;

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		pEffChar->SetCharSingleHellMatrixMode( 2 );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0115 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	m_szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szXFileName = new char[dwSize];
	SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	// LOD �ε�
	SFile >> dwSize;
	m_kLODInfo.resize(dwSize);

	float fDistance = 0.f;
	char temp[MAX_PATH] = {0};

	for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
	{
		SFile >> fDistance;
		m_kLODInfo.kDistance[i] = fDistance;

		SFile >> dwString;
		SFile.ReadBuffer ( temp, sizeof(char) * dwString );
		m_kLODInfo.kMeshName[i] = temp;
	}

	//	Note : ��Ų �޽� �б�.
	//
	TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
	hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );

	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	if( m_emPieceType == PIECETYPE_CHARACTER )
		m_emType = (EMPIECECHAR) dwSize;
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		m_emV_PartType = (EMPIECEVEHICLE) dwSize;

	m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile >> m_strUserBackSlot;
	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile >> m_bKeepUserBackSlot;

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	return S_OK;
}

HRESULT DxSkinPiece::LoadPiece_0116 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
{
	HRESULT hr;

	SFile >> m_dwRef;

	DWORD dwSize;

	SFile >> dwSize;
	m_szSkeleton = new char[dwSize];
	SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szXFileName = new char[dwSize];
	SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );

	SFile >> dwSize;
	m_szMeshName = new char[dwSize];
	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );

	// LOD �ε�
	SFile >> dwSize;
	m_kLODInfo.resize(dwSize);

	float fDistance = 0.f;
	char temp[MAX_PATH] = {0};

	for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
	{
		SFile >> fDistance;
		m_kLODInfo.kDistance[i] = fDistance;

		SFile >> dwString;
		SFile.ReadBuffer ( temp, sizeof(char) * dwString );
		m_kLODInfo.kMeshName[i] = temp;
	}

	//	Note : ��Ų �޽� �б�.
	//
	TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
	hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );

	if ( FAILED(hr) )	return hr;

	SFile >> m_dwMaterialNum;
	if ( m_dwMaterialNum!=0 )
	{
		SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

		m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
		}

		SAFE_DELETE_ARRAY ( m_apOverlayTex );
		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_apOverlayTex[i] = NULL;
		}
	}

	SFile >> dwSize;
	if( m_emPieceType == PIECETYPE_CHARACTER )
		m_emType = (EMPIECECHAR) dwSize;
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		m_emV_PartType = (EMPIECEVEHICLE) dwSize;

	m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

	SFile >> dwSize;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// ���������� ����� �� �۾��� �ʿ� ����.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile >> m_strUserBackSlot;
	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile >> m_bKeepUserBackSlot;

	std::string strBuffer;
	SVERTEXINFLU sVertexInflu;

	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		SFile >> strBuffer;
		sVertexInflu.LoadFile ( SFile );

		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu) );
	}

	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );

	dwSize = 0;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD TypeID;
		SFile >> TypeID;

		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffChar )	return E_FAIL;

		pEffChar->SetLinkObj ( NULL, this );
		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pEffChar);
			continue;
		}

		m_vecEFFECT.push_back ( pEffChar );
	}

	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

	SFile >> m_dwFlag;

	DWORD dwTemp;
	SFile >> dwTemp;			m_emHairHairOption = static_cast<HAT_HAIR_OPTION>( dwTemp );
	SFile >> m_strAddHairCps;

	return S_OK;
}
//
//HRESULT DxSkinPiece::LoadPiece_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread )
//{
//	HRESULT hr;
//
//	SFile >> m_dwRef;
//
//	DWORD dwSize;
//
//	SFile >> dwSize;
//	m_szSkeleton = new char[dwSize];
//	SFile.ReadBuffer ( m_szSkeleton, sizeof(char)*dwSize );
//
//	SFile >> dwSize;
//	m_szXFileName = new char[dwSize];
//	SFile.ReadBuffer ( m_szXFileName, sizeof(char)*dwSize );
//
//	SFile >> dwSize;
//	m_szMeshName = new char[dwSize];
//	SFile.ReadBuffer ( m_szMeshName, sizeof(char)*dwSize );
//
//	// LOD �ε�
//	SFile >> dwSize;
//	m_kLODInfo.resize(dwSize);
//
//	float fDistance = 0.f;
//	char temp[MAX_PATH] = {0};
//
//	for (DWORD i = 0, dwString = 0; i < dwSize; ++i)
//	{
//		SFile >> fDistance;
//		m_kLODInfo.kDistance[i] = fDistance;
//
//		SFile >> dwString;
//		SFile.ReadBuffer ( temp, sizeof(char) * dwString );
//		m_kLODInfo.kMeshName[i] = temp;
//	}
//
//	//	Note : ��Ų �޽� �б�.
//	//
//	TSTRING strXFileName = m_szXFileName, strSkeleton = m_szSkeleton;
//	hr = SetSkinMesh ( strXFileName.c_str(), strSkeleton.c_str(), pd3dDevice, bMeshThread, bTexThread );
//
//	if ( FAILED(hr) )	return hr;
//
//	SFile >> m_dwMaterialNum;
//	if ( m_dwMaterialNum!=0 )
//	{
//		SAFE_DELETE_ARRAY(m_pMaterialPiece);
//
//		m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
//		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
//		{
//			m_pMaterialPiece[i].LoadFile ( SFile, pd3dDevice );
//		}
//
//		SAFE_DELETE_ARRAY ( m_apOverlayTex );
//		m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
//		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
//		{
//			m_apOverlayTex[i] = NULL;
//		}
//	}
//
//	if( !bMeshThread )
//	{
//		if ( !NSThreadCORE_UpdateData( pd3dDevice ) )
//		{
//			return S_FALSE;
//		}
//	}
//
//	SFile >> dwSize;
//	if( m_emPieceType == PIECETYPE_CHARACTER )
//		m_emType = (EMPIECECHAR) dwSize;
//	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
//		m_emAttBoneType = (EMPIECEATTBONE) dwSize;
//	else if( m_emPieceType == PIECETYPE_V_PARTS )
//		m_emV_PartType = (EMPIECEVEHICLE) dwSize;
//
//	m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;
//
//	SFile >> dwSize;
//	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
//
//	SFile >> m_strUserBackSlot;
//	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
//	SFile >> m_bKeepUserBackSlot;
//
//	std::string strBuffer;
//	SVERTEXINFLU sVertexInflu;
//
//	SFile >> dwSize;
//	for ( DWORD i=0; i<dwSize; i++ )
//	{
//		SFile >> strBuffer;
//		sVertexInflu.LoadFile ( SFile );
//
//		m_vecTrace.push_back ( STRACOR(strBuffer.c_str(),sVertexInflu,D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)) );
//	}
//
//	std::sort ( m_vecTrace.begin(), m_vecTrace.end(), STRACOR_OPER() );
//
//	dwSize = 0;
//	SFile >> dwSize;
//	for ( DWORD i=0; i<dwSize; i++ )
//	{
//		DWORD TypeID;
//		SFile >> TypeID;
//
//		DxEffChar* pEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( TypeID );
//		if ( !pEffChar )	return E_FAIL;
//
//		pEffChar->SetLinkObj ( NULL, this );
//		hr = pEffChar->LoadFile ( SFile, pd3dDevice );
//		if (FAILED(hr))
//		{
//			SAFE_DELETE(pEffChar);
//			continue;
//		}
//
//		m_vecEFFECT.push_back ( pEffChar );
//	}
//
//	std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );
//
//	SFile >> m_dwFlag;
//
//	return S_OK;
//}

//void ConvertEffChar_CharPart( LPDIRECT3DDEVICEQ pd3dDevice, 
//							 GLEFFCHAR_VEC& vecEffCharOUT, 
//							 GLEFFCHAR_VEC& vecEffChar, 
//							 DxCharPart* pCharPart, 
//							 DxSkinPiece* pSkinPiece )
//{
//	std::for_each( vecEffCharOUT.begin(), vecEffCharOUT.end(), std_afunc::DeleteObject() );
//	vecEffCharOUT.clear();
//
//	// Note : ������ �۾����� �ʴ°͸� �����Ѵ�.
//	for( DWORD i=0; i<vecEffChar.size(); ++i )
//	{
//		DxEffChar* pNew = vecEffChar[i]->CloneInstance ( pd3dDevice, pCharPart, pSkinPiece );
//		if( pNew )
//		{
//			vecEffCharOUT.push_back ( pNew );
//		}
//	}
//}

HRESULT DxSkinPiece::LoadPiece(const std::string& FileName, 
							   LPDIRECT3DDEVICEQ pd3dDevice, 
							   BOOL bMeshThread, 
							   BOOL bTexThread, 
							   boost::function<void (const TCHAR*)> pArchive )
{
	HRESULT hr;

	m_bCompletedLOAD = FALSE;

	SAFE_DELETE_ARRAY(m_szMeshName);

	if (FileName.empty())
        return E_FAIL;

	SAFE_DELETE_ARRAY(m_szFileName);
	int nStrLen = static_cast<int>( FileName.length()+1 );
	m_szFileName = new char[nStrLen];
	StringCchCopy(m_szFileName, nStrLen, FileName.c_str());

    std::string PathName(DxSkinPieceContainer::GetInstance().GetPath());
	PathName.append(m_szFileName);

    /* ��ī�̺� ��� �߰��� ���� �� �ڵ�, ���� ����ȭ�Ǹ� ���� ����
	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strSKINOBJ_ZIPFILE, 
            PathName.c_str(), 
            FileName));

	if (!pBStream.get())
		return E_FAIL;

    sc::BaseStream &SFile = *pBStream;
    */

    SPBaseStream spFile;

    if (Archive::CreateFileBaseStream(PathName.c_str(), FileName.c_str(), GLOGIC::strSKINOBJ_ZIPFILE.c_str(), pArchive, spFile) == false)
    {
        return E_FAIL;
    }

    sc::BaseStream& SFile = *spFile;

	char ext[_MAX_EXT] = {0};
	_splitpath(FileName.c_str(), NULL, NULL, NULL, ext);

	CString strExt(ext);
	strExt.MakeUpper();

	if( strExt == _T(".CPS") )
	{
		m_emPieceType = PIECETYPE_CHARACTER;
	}else if( strExt == _T(".APS") )
	{
		m_emPieceType = PIECETYPE_ATTACHBONE;
	}else if( strExt == _T(".VPS") )
	{
		m_emPieceType = PIECETYPE_V_PARTS;
	}

	//SerialMemoryEngine* pSFile = dynamic_cast<SerialMemoryEngine*>( &SFile );

	DWORD dwVer;
	SFile >> dwVer;

	// ���� �������� Material System ���� ���۵��� �ʴ´�.
	if ( dwVer <= 0x0113 )
	{
		m_bMaterialSystem = FALSE;
	}
	else
	{
		m_bMaterialSystem = TRUE;
	}

	switch ( dwVer )
	{
	case 0x0101:	hr = LoadPiece_0101( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0102:	hr = LoadPiece_0102( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0103:	hr = LoadPiece_0103( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0104:	hr = LoadPiece_0104( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0105:	hr = LoadPiece_0105( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0106:	hr = LoadPiece_0106( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0107:	hr = LoadPiece_0107( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0108:	hr = LoadPiece_0108( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0109:	hr = LoadPiece_0109( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
    case 0x0110:	hr = LoadPiece_0110( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0111:	hr = LoadPiece_0111( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0112:	hr = LoadPiece_0112_0113( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0113:	hr = LoadPiece_0112_0113( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0114:	hr = LoadPiece_0114( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0115:	hr = LoadPiece_0115( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	case 0x0116:	hr = LoadPiece_0116( SFile, pd3dDevice, bMeshThread, bTexThread );	break;
	default:
		{
			char szMsg[MAX_PATH] = "";
			StringCchCopy( szMsg, MAX_PATH, m_szFileName );
			StringCchCat( szMsg, MAX_PATH, _T(" / Can't Load Skin object!!!") );

			MessageBox ( NULL, szMsg, "ERROR", MB_OK );

			return E_FAIL;
		}
		break;
	};	

	if ( FAILED( hr ) ) 
		return hr;

	// 0x0113 ���������� mask ���� ���鵵�� �Ѵ�.
	if ( dwVer <= 0x0113 )
	{
		m_bMakeMaskMap = TRUE;

		// Texture Loading
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			LPDIRECT3DTEXTUREQ pLevelTex(NULL);
			LPDIRECT3DTEXTUREQ pCubeTex(NULL);
			float fTexUpDownPower(1.f);
			for ( DWORD j=0; j<m_vecEFFECT.size(); ++j )
			{
				const TCHAR* pTextureName(NULL);
				m_vecEFFECT[j]->ExportEffTexture( pd3dDevice, i, &pLevelTex, &pCubeTex, bTexThread?true:false, pTextureName );

				// ���� ������ ��Ͻ�Ű���� �Ѵ�.
				if ( pTextureName )
				{
					DxSkinPieceContainer::GetInstance().Insert_ExportEffTexture_NoHave_TextureName( pTextureName );
				}
			}

			m_arrayMaterialPiece[i].CreateBakeMaskALL( pd3dDevice, pLevelTex, pCubeTex );
		}
	}
	else
	{
		m_bMakeMaskMap = FALSE;

		// Texture Loading
		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].LoadMaskTex( pd3dDevice );
		}
	}

	if( !bMeshThread )
	{
		if ( !NSThreadCORE_UpdateData( pd3dDevice, bTexThread ) )
		{
			return S_FALSE;
		}
	}

	//ConvertEffChar_CharPart( pd3dDevice, m_vecEFFECT_CharPart, m_vecEFFECT, NULL, this );

	//if ( m_dwMaterialNum!=m_pmcMesh->GetNumMaterials() )
	//{
	//	m_dwMaterialNum = m_pmcMesh->GetNumMaterials();

	//	SAFE_DELETE_ARRAY(m_pMaterialPiece);
	//	m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
	//}

	//// Note : �������ʹ� ���� ������ ������ ���� ���̴�.
	//m_dwVertexNUM = m_pmcMesh->m_dwVertexNUM;

	//// Note : Cartoon Mesh�� �����Ѵ�.
	//m_skinMeshRes.get()->Load( m_szXFileName, pd3dDevice );

	// [ Thread Work shhan ]

	// cache�� �� �ʿ䰡 ��� ������ ���� ���ش�.
	SFile.CloseFile();

	return S_OK;
}

HRESULT DxSkinPiece::SavePiece ( const char* szFile, BOOL bCharTool, boost::function<void (const TCHAR*)> pArchive )
{
    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return E_FAIL;
    }

    sc::SerialFile& SFile = *spFile;

	if( bCharTool )
	{
		char szPathName[MAX_PATH] = "";
		StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
		StringCchCat( szPathName, MAX_PATH, szFile );
		if ( !SFile.OpenFile ( FOT_WRITE, (char*)szPathName ) )	return E_FAIL;
	}
	else
	{
		SAFE_DELETE_ARRAY(m_szFileName);
		int nStrLen = static_cast<int>( strlen(szFile)+1 );
		m_szFileName = new char[nStrLen];
		StringCchCopy( m_szFileName, nStrLen, szFile );

		char szPathName[MAX_PATH] = "";
		StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
		StringCchCat( szPathName, MAX_PATH, m_szFileName );

		if ( !SFile.OpenFile ( FOT_WRITE, (char*)szPathName ) )	return E_FAIL;
	}

	SFile << VERSION;

	SFile << m_dwRef;

	DWORD dwSize;

	if ( !m_szSkeleton )	return E_FAIL;

	dwSize = static_cast<int>( strlen(m_szSkeleton)+1 );
	SFile << dwSize;
	SFile.WriteBuffer ( m_szSkeleton, sizeof(char)*dwSize );

	if ( !m_szXFileName )	return E_FAIL;

	dwSize = static_cast<int>( strlen(m_szXFileName)+1 );
	SFile << dwSize;
	SFile.WriteBuffer ( m_szXFileName, sizeof(char)*dwSize );

	if ( !m_szMeshName )	return E_FAIL;

	dwSize = static_cast<int>( strlen(m_szMeshName)+1 );
	SFile << dwSize;
	SFile.WriteBuffer ( m_szMeshName, sizeof(char)*dwSize );

    dwSize = m_kLODInfo.size();
    SFile << dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        SFile << m_kLODInfo.kDistance[i];
        MIN_ASSERT(m_kLODInfo.kDistance[i] > 0 && _T("LOD �Ÿ� ���� ����!!!"));

        const TCHAR* pTemp = m_kLODInfo.kMeshName[i].c_str();
        MIN_ASSERT(_tcsclen(pTemp) > 0 && _T("LOD �޽� ���� ����!!!"));

        DWORD dwString = m_kLODInfo.kMeshName[i].size() + 1;
        SFile << dwString;
        SFile.WriteBuffer ( pTemp, dwString * sizeof(TCHAR) );
    }

	SFile << m_dwMaterialNum;
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_arrayMaterialPiece[i].SaveFile ( SFile );
	}

	if( m_emPieceType == PIECETYPE_CHARACTER )
		SFile << (DWORD)m_emType;
	else if( m_emPieceType == PIECETYPE_ATTACHBONE )
		SFile << (DWORD)m_emAttBoneType;
	else if( m_emPieceType == PIECETYPE_V_PARTS )
		SFile << (DWORD)m_emV_PartType;
	
	SFile << (DWORD)m_emWeaponWhereBack;

	SFile << m_strUserBackSlot;
	SFile.WriteBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile << m_bKeepUserBackSlot;

	//	Note : trace point
	//
	{
		SFile << DWORD(m_vecTrace.size());
		for ( size_t n=0; n<m_vecTrace.size(); ++n )
		{
			SFile << std::string(m_vecTrace[n].m_szName);
			m_vecTrace[n].m_sVertexInflu.SaveFile ( SFile );
		}
	}

	//	Note : Effect skin Map
	//
	{
		SFile << (DWORD) m_vecEFFECT.size ();
		for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
		{
			DxEffChar* pEffChar = m_vecEFFECT[n];
			pEffChar->SaveFile ( SFile );
		}
	}


	SFile << m_dwFlag;

	SFile << static_cast<DWORD>( m_emHairHairOption );
	SFile << m_strAddHairCps;

	// Note : Cartoon Mesh�� �����Ѵ�.	( �Ϲݰ� HLSL �� 2���� �����Ѵ�. )
	m_skinMeshRes.get()->SaveNORMAL( m_szXFileName );
	m_skinMeshRes.get()->SaveHLSL( m_szXFileName );
	m_skinMeshRes.get()->SaveNormalMesh( m_szXFileName );

//	m_skinMeshRes.get()->SaveLOD_SW( m_szXFileName );

	return S_OK;
}

void DxSkinPiece::LoadUBS( const TSTRING& strFile )
{
	if ( strFile.empty() )
		return;			// �̸����°��� �ε����� �ʴ´�.

	SAFE_DELETE_ARRAY(m_szFileName);
	size_t nStrLen = strFile.length()+1;
	m_szFileName = new TCHAR[nStrLen];
	StringCchCopy( m_szFileName, nStrLen, strFile.c_str() );

	TCHAR szPathName[MAX_PATH] = {0};
	StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, m_szFileName );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	std::auto_ptr<sc::BaseStream> pBStream( 
		GLOGIC::openfile_basestream(
		GLOGIC::UseEngineZipFile(), 
		GLOGIC::strSKINOBJ_ZIPFILE, 
		szPathName, 
		strFile.c_str() ) );
	if ( !pBStream.get() )								
		return;
	sc::BaseStream &SFile = *pBStream;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_strUserBackSlot;
	SFile.ReadBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile >> m_bKeepUserBackSlot;

	// cache�� �� �ʿ䰡 ��� ������ ���� ���ش�.
	SFile.CloseFile( FALSE );
}

void DxSkinPiece::SaveUBS( const TCHAR* szFile )
{
	MIN_ASSERT(szFile);

	SAFE_DELETE_ARRAY(m_szFileName);
	int nStrLen = (int) (_tcslen(szFile)+1);
	m_szFileName = new TCHAR[nStrLen];
	StringCchCopy( m_szFileName, nStrLen, szFile );

	TCHAR szPathName[MAX_PATH] = _T("");
	StringCchCopy( szPathName, MAX_PATH, DxSkinPieceContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, m_szFileName );

	sc::SerialFile SFile;
	if ( !SFile.OpenFile( FOT_WRITE, (TCHAR*)szPathName ) )	
		return;

	SFile << static_cast<DWORD>(0x100);

	SFile.BeginBlock();

	SFile << m_strUserBackSlot;
	SFile.WriteBuffer( &m_matUserBackSlot, sizeof(D3DXMATRIX) );
	SFile << m_bKeepUserBackSlot;

	SFile.EndBlock();
}

HRESULT DxSkinPiece::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->InitDeviceObjects ( pd3dDevice );
	}

	//for ( size_t n=0; n<m_vecEFFECT_CharPart.size(); ++n )
	//{
	//	DxEffChar* pEffChar = m_vecEFFECT_CharPart[n];
	//	pEffChar->InitDeviceObjects ( pd3dDevice );
	//}

	return S_OK;
}

HRESULT DxSkinPiece::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->RestoreDeviceObjects ( pd3dDevice );
	}

	//for ( size_t n=0; n<m_vecEFFECT_CharPart.size(); ++n )
	//{
	//	DxEffChar* pEffChar = m_vecEFFECT_CharPart[n];
	//	pEffChar->RestoreDeviceObjects ( pd3dDevice );
	//}

	//for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	//{
	//	// ���� Overlay�� CharEdit ������ ���δ�.
	//	//m_apOverlayTex[i] = NULL;

	//	//m_arrayMaterialPiece[i].RestoreDeviceObjects( pd3dDevice );
	//}

	// m_arrayMaterialPiece[i].RestoreDeviceObjects( pd3dDevice ); ������  SetMaterialSetting ���� ����ϰ� �ȴ�.
	SetMaterialSetting( pd3dDevice, FALSE, FALSE );

	return S_OK;
}

HRESULT DxSkinPiece::InvalidateDeviceObjects ()
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->InvalidateDeviceObjects ();
	}

	//for ( size_t n=0; n<m_vecEFFECT_CharPart.size(); ++n )
	//{
	//	DxEffChar* pEffChar = m_vecEFFECT_CharPart[n];
	//	pEffChar->InvalidateDeviceObjects ();
	//}

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE ( m_apOverlayTex[i] );

		m_arrayMaterialPiece[i].InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT DxSkinPiece::DeleteDeviceObjects ()
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEffChar = m_vecEFFECT[n];
		pEffChar->DeleteDeviceObjects ();
	}

	//for ( size_t n=0; n<m_vecEFFECT_CharPart.size(); ++n )
	//{
	//	DxEffChar* pEffChar = m_vecEFFECT_CharPart[n];
	//	pEffChar->DeleteDeviceObjects ();
	//}

	return S_OK;
}

BOOL DxSkinPiece::NSThreadCORE_UpdateData( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTexThread )
{
	if ( !m_szMeshName )
		return FALSE;

	// �۾��ؾ� �Ұ͵��� �ִ�.~ �ε��� �Ϸ� �Ǿ����� �۵��ȵǾ�ȴ�.
	if( !m_bCompletedLOAD )
	{
		// Thread Load���� �۾��� �Ϸ�Ǹ� ���� �ش�.
		// = DxSkinMeshManager::GetInstance()->FindSkinMesh( m_szXFileName );
		if( m_skinMeshRes.Update() )
		{
			m_pmcMesh = m_skinMeshRes.get()->FindMeshContainer ( m_szMeshName );
			if ( !m_pmcMesh )
			{
				if( !m_bActiveErrorLog )
				{
					m_bActiveErrorLog = TRUE;

					// ~.cps ������ ~.x �� ���ӻ� ~.x ���� ������ ���� �ʴ� ������ ����.
					CDebugSet::ToLogFile( sc::string::format( _T("%1%, %2%���ο� %3%�� �����ϴ�."), m_szFileName, m_szXFileName, m_szMeshName ) );
				}
				return FALSE;
			}

			// [shhan][2015.01.06] ������ �ε��� �Ͱ� ���� �ε��Ѱ��� �� ������ �������� Ȯ���Ѵ�.
			//						�������� ������ �۾��Ұ��̴�.
			//
			//						�̺κ��� ���� �����鼭 CharPartBoneMatrices::CaculateBoneMatrices �� 
			//						sSkeleton.FindBone_Index( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] )->m_matCombined ���� ������ �߻��Ͽ���.
			//						cps ������ skin : s_m_transformer.x, bone : b_effet_char.X 
			//						�ٸ� cps skin : s_m_transformer.x, bone : b_m.X  ���� �����Ͽ� ���׸� �߻����״�.
			//
			// [shhan][2015.04.29] Skin �� ��츸 üũ�ϵ��� �Ѵ�.
			//						��ũ�� ���� �� �ʿ䰡 ����.
			//
			if ( m_pmcMesh->pSkinInfo )
			{
				CString strFile1( m_skinMeshRes.get()->GetBoneFileName() );
				strFile1.MakeLower();

				CString strFile2( m_szSkeleton );
				strFile2.MakeLower();

				if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
				{
					CDebugSet::ToLogFile
					( 
						sc::string::format( _T("Base Bone : %1%, Error skin : %2%, Error Bone : %3% \n"), m_skinMeshRes.get()->GetBoneFileName(), m_szXFileName, m_szSkeleton ) 
					);

					GASSERT( 0 && _T("Multiple Bone.x are being used in Skin.x. Please refer to log.") );

					return FALSE;
				}
			}

			// ���� m_vecEFFECT �� �ؽ��� ���� �ε��� �ȵǾ���.
			// ������ ��ٸ����� ����.
			if ( !SetMaterialSetting( pd3dDevice, FALSE, bTexThread ) )
			{
				m_pmcMesh = NULL;
				return TRUE;
			}

			//// ������ CPU�� ���� �� ���� ���ش�.
			//switch( RENDERPARAM::emCharRenderTYPE )
			//{
			//case EMCRT_SOFTWARE:
			//	{
			//		SAFE_DELETE( m_pMeshVB );
			//		m_pMeshVB = new DxCharPartVB;
			//		m_pMeshVB->CreateVB( pd3dDevice, m_pmcMesh->MeshData.pMesh );

			//		SAFE_DELETE( m_pMeshVB_SHADER );
			//		m_pMeshVB_SHADER = new DxCharPartVB;
			//		m_pMeshVB_SHADER->CreateVB_SHADER( pd3dDevice, m_pmcMesh );
			//	}
			//	break;
			//};

			// Note : �������ʹ� ���� ������ ������ ���� ���̴�.
			//m_dwVertexNUM = m_pmcMesh->m_dwVertexNUM;

			// Note : Cartoon Mesh�� �����Ѵ�.
			m_skinMeshRes.get()->Load( m_szXFileName, pd3dDevice );

			// Tool ������ ���鵵�� �Ѵ�.
			if ( g_bCHAR_EDIT_RUN )
			{
				m_scpPhysXCloth->Import( this );
			}

			m_bCompletedLOAD = TRUE;
		}
	}

	return TRUE;
}

BOOL DxSkinPiece::ImmediatelyLoad( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �۾��ؾ� �Ұ͵��� �ִ�.~ �ε��� �Ϸ� �Ǿ����� �۵��ȵǾ�ȴ�.
	if( !m_bCompletedLOAD )
	{
	    m_skinMeshRes = DxSkinMeshManager::GetInstance().LoadDxSkinMesh( m_szXFileName, m_szSkeleton, FALSE, FALSE );
	    if( m_skinMeshRes.IsValid() )
	    {
		    if ( !NSThreadCORE_UpdateData( pd3dDevice, FALSE ) )
			    return FALSE;
	    }
	}
	return TRUE;
}

HRESULT DxSkinPiece::FrameMove ( float fTime, float fElapsedTime )
{
	if( m_bCompletedLOAD )
	{
		m_scpEffectMaterialData->InitForTool();

		//	Note : ����Ʈ ������Ʈ.
		//
		for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
		{
			DxEffChar* pEffChar = m_vecEFFECT[n];

			pEffChar->FrameMove ( fTime, fElapsedTime );
		}

		SumEffFlag();
	}

	return S_OK;
}

HRESULT DxSkinPiece::DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw, BOOL bAlpha, BOOL bShadow, BOOL bAlphaTex )
{
	if ( !m_pmcMesh )
		return E_FAIL;

	HRESULT hr = S_OK;

	if ( m_skinMeshRes.IsValid() )
	{
		// ���� ����ϴ� ������ Import �Ѵ�.
		m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.

		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= m_arrayMaterialPiece;
		sCharSetting.pmtrlSpecular		= NULL;
		sCharSetting.bWorldIdentity		= FALSE;
		sCharSetting.emRDOP				= CTOP_BASE;

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );

		m_skinMeshRes.get()->SetDrawState ( bOriginDraw, bAlpha, bShadow, bAlphaTex );
		m_skinMeshRes.get()->DrawMeshContainer ( pd3dDevice, sCharSetting, m_scpSkeleton.get(), matIdentity, DxSkinMesh9::g_pBoneMatrices );

		return S_OK;
	}

	return E_FAIL;
}




HRESULT DxSkinPiece::MeshEditRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;
	if ( !m_skeletonRes.IsValid() || !m_skinMeshRes.IsValid() )		return S_FALSE;

	BOOL	bOriginDraw	= TRUE;
	BOOL	bAlphaTex	= TRUE;
	BOOL	bAlpha		= FALSE;
	BOOL bCrashRender(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// ������ �浹�� �Ͼ��.
//	if( (m_dwFlags&EMEFFCHAR_OVERLAY) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// ������ �浹�� �Ͼ��.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
//		if( m_dwFlags & EMEFFCHAR_OVERLAY )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
    {
        bAlphaTex	= FALSE;
	//if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
        bAlpha		= TRUE;
    }

	//	Note : �Ϲ� �޽��� ��� Bone�� Ʋ�� ���ɼ��� �����.
	//
	if ( !pSkeleton )	
		pSkeleton = m_skeletonRes.get();

	//	Note : ���� ��Ʈ���� ������Ʈ.
	//
	if ( pSkeleton )
	{
        DxSkeletonPart* pSkeletonPart = &pSkeleton->m_skeletonPart;
		m_skeletonRes.get()->UpdateBones_PureThread( pSkeletonPart->pBoneRoot, matRot, 1.f, 1.f );
	}

	DWORD dwOldFill, dwOldLight;
	
	pd3dDevice->GetRenderState( D3DRS_FILLMODE, &dwOldFill );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwOldLight );

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );


	//	Note : ĳ���� �ؽ��� ���Ͽ� ����ŧ�� ���� ��� ���� ��� 1���� �Ѹ��� ���ؼ� �⺻ ���������� �Ѹ��� �ʴ´�.
	//
	if ( bOriginDraw )
	{
		//	Note : SkinMesh ���úκ� �׸���.
		//
		if ( !m_pmcMesh )	return S_FALSE;
		if ( !m_pmcMesh->m_pBoneCombinationBuf ) return S_FALSE;

	
		
		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );
		pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

		LPD3DXBONECOMBINATION pBoneComb;
		LPD3DXBONECOMBINATION pBoneCur;
		UINT				  iMatrixIndex;
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(m_pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());


		for( DWORD iAttrib = 0; iAttrib < m_pmcMesh->m_dwNumAttributeGroups; iAttrib++ )
		{
			pBoneCur = &pBoneComb[iAttrib];

			for( DWORD i = 0; i < m_pmcMesh->m_dwNumInfl; i++ )
			{				
				iMatrixIndex = pBoneCur->BoneId[i];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &mat, &m_pmcMesh->m_pBoneOffsetMatrices[iMatrixIndex], &pSkeleton->FindBone_Index_Skin( m_pmcMesh->m_arrayBoneIndex, iMatrixIndex )->matCombined );
					pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &mat );
				}
			}
			
//			D3DXMatrixMultiply( &mat, &m_pmcMesh->pBoneOffsetMatrices[i], m_pmcMesh->ppBoneMatrixPtrs[i] );
			m_pmcMesh->MeshData.pMesh->DrawSubset(iAttrib);
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FILLMODE, dwOldFill );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, dwOldLight );

	return S_OK;
}

HRESULT DxSkinPiece::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIXA16 &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY )
{
	NSThreadCORE_UpdateData( pd3dDevice, FALSE );

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		RenderFixed( pd3dDevice, matRot, bCHAR_EDIT_PIECE_RENDER_IDENTITY );
	}
	else
	{
		RenderMaterialSystem( pd3dDevice, matRot, bCHAR_EDIT_PIECE_RENDER_IDENTITY );
	}

	return S_OK;
}

HRESULT DxSkinPiece::RenderFixed( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY )
{
	HRESULT hr = S_OK;    

    //UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&matRot._41), pd3dDevice);

	if ( !m_skeletonRes.IsValid() || !m_skinMeshRes.IsValid() || !m_pmcMesh )
		return S_FALSE;        

	BOOL	bOriginDraw	= TRUE;
	BOOL	bAlphaTex	= TRUE;
	BOOL	bAlpha		= FALSE;
	BOOL bCrashRender(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	bCrashRender = TRUE;	// ������ �浹�� �Ͼ��.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
    {
        bAlphaTex	= FALSE;
	//if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
        bAlpha		= TRUE;
    }

	//	Note : ���� ��Ʈ���� ������Ʈ.
	{
		m_skeletonRes.get()->ResetBone_PureThread();
		m_skeletonRes.get()->UpdateBones_PureThread( m_skeletonRes.get()->m_skeletonPart.pBoneRoot, matRot, 1.f, 1.f );
	}

	// ���� ����ϴ� ������ Import �Ѵ�.
	m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.

	//if ( !m_skinMeshRes.IsValid() )
	//{
	//	//if ( m_pmcMesh->m_numBoneComb == 3 )
	//	//{
	//	//	//	Note : ���� �޴� ���� matrix �����͸� �ٽ� ã�´�.
	//	//	//

	//	//	//	m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
	//	//	//	m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
	//	//	//	m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- �̰͸� ������ BONE���� ������ �޴°�.

	//	//	DxBoneTrans* pBoneTrans = pSkeleton->FindBone ( m_pmcMesh->m_szBoneNames[2].c_str() );
	//	//	if ( pBoneTrans )	m_pmcMesh->m_pBoneMatrix[2] = &pBoneTrans->matCombined;
	//	//	else				m_pmcMesh->m_pBoneMatrix[2] = NULL;
	//	//}
	//}

	DxEffChar*	pEffAmbient = NULL;
	DxEffChar*	pEffToon = NULL;

	//	Note : Setting State.
	//
	{
		SKINEFFDATA sSkinEffData;

		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];
			if ( pEFFECT->GetStateOrder() > EMEFFSO_RENDERSTATE )	break;

			pEFFECT->SettingState( pd3dDevice, sSkinEffData );
		}
	}

	// ���� ���� ���ÿ����� �������� Identity �� �ϵ��� �Ѵ�.
	g_bCHAR_EDIT_PIECE_RENDER_FIXED = bCHAR_EDIT_PIECE_RENDER_IDENTITY;


	//	Note : ĳ���� �ؽ��� ���Ͽ� ����ŧ�� ���� ��� ���� ��� 1���� �Ѹ��� ���ؼ� �⺻ ���������� �Ѹ��� �ʴ´�.
	//
	if ( bOriginDraw )
	{
		//	Note : SkinMesh ���úκ� �׸���.
		//
		if ( !m_pmcMesh )	return S_FALSE;

		m_skinMeshRes.get()->SetDrawState( bOriginDraw, bAlpha, FALSE, bAlphaTex );

		CHARSETTING sCharSetting;
		sCharSetting.pMeshContainerBase = m_pmcMesh;
		sCharSetting.pmtrlPiece			= m_arrayMaterialPiece;
		sCharSetting.pmtrlSpecular		= NULL;
		sCharSetting.bWorldIdentity		= TRUE;
		sCharSetting.emRDOP				= CTOP_BASE;

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );

		m_skinMeshRes.get()->DrawMeshContainer( pd3dDevice, sCharSetting, m_scpSkeleton.get(), matIdentity, DxSkinMesh9::g_pBoneMatrices );
	}

	//	Note : render effect skin
	//
	{
        // ������ Ambient Material �� �����´�.
        SKINEFFDATA sSKINEFFDATA;
        {
            SKINEFF_LEVEL_DATA* pLevelData(NULL);
		    GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
		    for ( ; pos!=m_vecEFFECT.end(); ++pos )
		    {
			    (*pos)->GetLevelData( pd3dDevice, &pLevelData );
		    }
		    sSKINEFFDATA.m_rLevelData = pLevelData;
        }


		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), (DWORD)EMEFFSO_NORMAL, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_NORMAL )	break;	//	EMEFFSO_NORMAL�� �ƴϸ� �ߴ�.

			if( bCrashRender )
			{
				switch( RENDERPARAM::emCharRenderTYPE )
				{
				case EMCRT_SOFTWARE:
				case EMCRT_NORMAL:
				case EMCRT_VERTEX:
					if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader �� ���ٰ� ������ ������ �۵� ���Ѵ�.
					break;
				case EMCRT_PIXEL:
					if( pEFFECT->GetFlag() & EMECF_NONHLSL )	continue;	// �浹�� �Ͼ�� ���� �Ѹ��� �ʴ´�.
					break;
				}
			}

			pEFFECT->SetWorldMatrix( matRot );
			pEFFECT->Render( pd3dDevice, DxSkinMesh9::g_pBoneMatrices, sSKINEFFDATA );
		}

		pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), (DWORD)EMEFFSO_ADD, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_NORMAL�� �ƴϸ� �ߴ�.

			pEFFECT->SetWorldMatrix( matRot );
			pEFFECT->Render( pd3dDevice, DxSkinMesh9::g_pBoneMatrices, sSKINEFFDATA );
		}
	}

	//	Note : Setting State.
	//
	{
		GLEFFCHAR_VEC_RITER pos = std::lower_bound ( m_vecEFFECT.rbegin(), m_vecEFFECT.rend(), EMEFFSO_RENDERSTATE, DXEFFCHAR_DISOPER() );
		for ( ; pos!=m_vecEFFECT.rend(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);

			pEFFECT->RestoreState ( pd3dDevice );
		}
	}


	//	Note : Glow
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_GLOW )	break;	//	EMEFFSO_GLOW�� �ƴϸ� �ߴ�.

			pEFFECT->RenderGlow( pd3dDevice, DxSkinMesh9::g_pBoneMatrices, TRUE, (m_dwFlags & EMECF_VISUAL_MATERIAL) );
		}
	}

	//	Note : Single Eff
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF�� �ƴϸ� �ߴ�.

			pEFFECT->SetSkeleton( m_scpSkeleton.get() );
			pEFFECT->SetWorldMatrix( matRot );
			pEFFECT->RenderEff( pd3dDevice, NULL, 1.f );
		}
	}    

	return S_OK;
}

HRESULT DxSkinPiece::RenderMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIXA16 &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY )
{
	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&matRot._41), pd3dDevice);

	if ( !m_skeletonRes.IsValid() || !m_skinMeshRes.IsValid() || !m_pmcMesh )
		return S_FALSE;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_skinMeshRes.get() );
	if ( !pSkinMesh )
		return S_OK;

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );

	// ���� ���� ���ÿ����� �������� Identity �� �ϵ��� �Ѵ�.
	g_bCHAR_EDIT_PIECE_RENDER_FIXED = bCHAR_EDIT_PIECE_RENDER_IDENTITY;

	// ABL ���ý� cps�� ������ �� ��� �� ��ġ���� ������ �Ǿ� �ؼ� �̷��� ����Ǿ���.
	if ( !bCHAR_EDIT_PIECE_RENDER_IDENTITY )
	{
		matWorld = matRot;
	}

	//	Note : ���� ��Ʈ���� ������Ʈ.
	//
	//if ( !pSkeleton )
	{
		m_skeletonRes.get()->ResetBone_PureThread();
		m_skeletonRes.get()->UpdateBones_PureThread( m_skeletonRes.get()->m_skeletonPart.pBoneRoot, matRot, 1.f, 1.f );
	}

	m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.

	m_scpEffectMaterialData->InitForPrecalculateForMaterialSystem();
	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			m_vecEFFECT[i]->SetSkeleton( m_scpSkeleton.get() );
			m_vecEFFECT[i]->SetWorldMatrix( matRot );
			m_vecEFFECT[i]->PrecalculateForMaterialSystem( pd3dDevice, m_scpEffectMaterialData.get(), &matRot, 1.f );
			m_vecEFFECT[i]->PrecalculateForMaterialSystemHardAlpha( pd3dDevice, m_scpEffectMaterialData.get() );
			m_vecEFFECT[i]->PrecalculateForMaterialSystemSoftAlpha( pd3dDevice, m_scpEffectMaterialData.get() );
		}

		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			m_arrayMaterialPiece[i].PrecalculateForMaterialSystem( m_scpEffectMaterialData.get(), i );
		}
	}

	// VisualMaterial �̶�� VM ���� �������ϰ� Material System ���� ���������� �ʵ��� �Ѵ�.
	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	{
		// ������ Ambient Material �� �����´�.
		SKINEFFDATA sSKINEFFDATA;
		{
			SKINEFF_LEVEL_DATA* pLevelData(NULL);
			GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
			for ( ; pos!=m_vecEFFECT.end(); ++pos )
			{
				(*pos)->GetLevelData( pd3dDevice, &pLevelData );
			}
			sSKINEFFDATA.m_rLevelData = pLevelData;
		}

		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
			{
				m_vecEFFECT[i]->RenderVM( pd3dDevice, &sSKINEFFDATA, EMTTF_OPAQUE );
				m_vecEFFECT[i]->RenderVM( pd3dDevice, &sSKINEFFDATA, EMTTF_ALPHA_HARD );
				m_vecEFFECT[i]->RenderVM( pd3dDevice, &sSKINEFFDATA, EMTTF_ALPHA_SOFT );
			}

			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_LEVEL )
			{
				m_vecEFFECT[i]->RenderMaterialGlow( pd3dDevice, m_arrayMaterialPiece, matWorld, *pSkinMesh, m_scpPhysXCloth.get() );
			}
		}

		//	Note : Single Eff
		//
		{
			GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
			for ( ; pos!=m_vecEFFECT.end(); ++pos )
			{
				DxEffChar* pEFFECT = (*pos);
				if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )
					break;	//	EMEFFSO_SINGLE_EFF�� �ƴϸ� �ߴ�.

				pEFFECT->SetSkeleton( m_scpSkeleton.get() );
				pEFFECT->SetWorldMatrix( matRot );
				pEFFECT->RenderEff( pd3dDevice, NULL, 1.f );
			}
		}

		return S_OK;
	}
	else if ( m_dwFlags & EMECF_TOTAL_SHADER )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_TOTALSHADER )
			{
				m_vecEFFECT[i]->Render( pd3dDevice, DxSkinMesh9::g_pBoneMatrices, FALSE );
			}
		}

		return S_OK;
	}
	else if ( m_dwFlags & EMECF_NORMALMAP )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_NORMALMAP )
			{
				m_vecEFFECT[i]->Render( pd3dDevice, DxSkinMesh9::g_pBoneMatrices, FALSE );
			}
		}

		return S_OK;
	}

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE( m_apOverlayTex[i] );
	}

	// Texture �� ��ȭ�� �ش�.
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];

			switch ( pEFFECT->GetTypeID() )
			{
			case EMEFFCHAR_USERCOLOR:
				{
					DxEffCharUserColor* pUserColor = MIN_BOOST::polymorphic_downcast<DxEffCharUserColor*>( pEFFECT );
					NSTexCombine::CreateUserColorForEdit
					( 
						pd3dDevice,
						i,
						&m_arrayMaterialPiece[i].m_textureRes,
						pUserColor, 
						m_apOverlayTex[i]
					);
				}
				break;

			case EMEFFCHAR_OVERLAY:
				{
					DxEffCharOverlay* pOverlay = MIN_BOOST::polymorphic_downcast<DxEffCharOverlay*>( pEFFECT );
					NSTexCombine::CreateOverlayForEdit
					( 
						pd3dDevice,
						i,
						&m_arrayMaterialPiece[i].m_textureRes,
						pOverlay, 
						m_apOverlayTex[i]
					);
				}
				break;
			};
		}
	}

	//size_t nSIZE = m_vecEFFECT.size();
	//for ( size_t n=0; n<nSIZE; ++n )
	//{
	//	m_vecEFFECT[n]->SetWorldBattleFlag( pd3dDevice, m_apOverlayTex );
	//}

	SKIN_SPEC_SSS_INFO sSkinInfo;
	SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoReference(&sSkinInfo.m_vBodySpecColor_Power,&sSkinInfo.m_vBodySpec2Color_Power,&sSkinInfo.m_vSSS_Color);

	CaculateBoneMatrices( *m_pmcMesh, DxSkinMesh9::g_pBoneMatrices, *m_scpSkeleton.get() );





	{
		//// �⺻ ��带 ������ ����.
		//for (UINT i = 0; i < DxSkinMesh9::g_NumBoneMatricesMax; ++i)
		//{
		//	D3DXMatrixIdentity ( &DxSkinMesh9::g_pBoneMatrices[i] );
		//}

		//D3DXMATRIXA16 matWorld;
		//D3DXMatrixIdentity( &matWorld );

		DxClothColl sClothColl;
		pSkinMesh->RenderOpaque
		( 
			pd3dDevice, 
			m_pmcMesh, 
			DxSkinMesh9::g_pBoneMatrices, 
			m_arrayMaterialPiece, 
			sSkinInfoReference, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex, 
			&matRot,
			&sClothColl,
			NULL,
			1.f,
			matWorld,
			m_scpPhysXCloth.get(),
			1.f
		);

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
		pSkinMesh->RenderHardAlpha( pd3dDevice, m_pmcMesh, DxSkinMesh9::g_pBoneMatrices, m_arrayMaterialPiece, sSkinInfoReference, m_scpEffectMaterialData.get(), m_apOverlayTex, 1.f, matWorld, m_scpPhysXCloth.get() );
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );

		DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
		pSkinMesh->RenderSoftAlpha( pd3dDevice, m_pmcMesh, DxSkinMesh9::g_pBoneMatrices, m_arrayMaterialPiece, sSkinInfoReference, m_scpEffectMaterialData.get(), m_apOverlayTex, 1.f, matWorld, m_scpPhysXCloth.get() );
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
	}

	// Note : Add
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_ADD, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_ADD�� �ƴϸ� �ߴ�.

			pEFFECT->RenderMaterialAdd( pd3dDevice, m_arrayMaterialPiece, matWorld, *pSkinMesh, m_scpPhysXCloth.get() );
		}
	}

	//	Note : Glow
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_LEVEL )	break;	//	EMEFFSO_GLOW, EMEFFSO_LEVEL�� �ƴϸ� �ߴ�.

			pEFFECT->RenderMaterialGlow( pd3dDevice, m_arrayMaterialPiece, matWorld, *pSkinMesh, m_scpPhysXCloth.get() );
		}
	}


	//	Note : Single Eff
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF�� �ƴϸ� �ߴ�.

			pEFFECT->SetSkeleton( m_scpSkeleton.get() );
			pEFFECT->SetWorldMatrix( matRot );
			pEFFECT->RenderEff( pd3dDevice, NULL, 1.f );
		}
	}

	return S_OK;
}

// CharPartBoneMatrices �� ���� ���� ������ ���� �Լ��� private ���� �س��� ���⿡ �ٽ� �����ߴ�.
void DxSkinPiece::CaculateBoneMatrices( const SMeshContainer& sMeshContainer, 
											D3DXMATRIXA16* arrayBoneMatrices,
											const DxSkeletonMaintain& sSkeleton )
{
	UINT iAttrib;
	UINT iMatrixIndex;
	UINT iPaletteEntry;
	LPD3DXBONECOMBINATION pBoneComb;

	// first check for skinning
	if ( sMeshContainer.pSkinInfo )
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(sMeshContainer.m_pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < sMeshContainer.m_dwNumAttributeGroups; ++iAttrib)
		{ 
			// first calculate all the world matrices
			for (iPaletteEntry = 0; iPaletteEntry < sMeshContainer.m_dwNumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					//if ( !sSkeleton.FindBone( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] ) )
					//	continue;	// �̰��� ���� ������ �ȵȴ�.

					D3DXMatrixMultiply( &arrayBoneMatrices[ (iAttrib*sMeshContainer.m_dwNumPaletteEntries) + iPaletteEntry], 
										&sMeshContainer.m_pBoneOffsetMatrices[iMatrixIndex], 
										&m_scpSkeleton->FindBone_Index( sMeshContainer.m_arrayBoneIndex[iMatrixIndex] )->m_matCombined );
				}
			}
		}
	}
}

HRESULT DxSkinPiece::RenderTEST( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton )
{
	//	Note : Single Eff
	//
	{
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF�� �ƴϸ� �ߴ�.

			pEFFECT->RenderEDIT( pd3dDevice, NULL, 1.f );
		}
	}

	return S_OK;
}

HRESULT DxSkinPiece::DrawTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bIdentity )
{
	if ( !m_pmcMesh )	return S_FALSE;
	
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.
	
	VECTRACE_ITER iter = m_vecTrace.begin ();
	VECTRACE_ITER iter_end = m_vecTrace.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		SVERTEXINFLU* pVertInflu = &((*iter).m_sVertexInflu);

		D3DXVECTOR3 vVecOutput;
		D3DXVECTOR3 vNorOutput;
		m_pmcMesh->CalculateVertexInfluences( *pVertInflu, vVecOutput, vNorOutput, NULL, bIdentity, *m_scpSkeleton.get(), matIdentity );

		pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
		EDITMESHS::RENDERSPHERE ( pd3dDevice, vVecOutput, 0.2f );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );
	}

	return S_OK;
}

HRESULT DxSkinPiece::DrawSelTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szSelVert, BOOL bIdentity )
{
	if ( !m_pmcMesh )	return S_FALSE;

	SVERTEXINFLU* pVertInflu = GetTracePos(szSelVert);
	if ( !pVertInflu )	return S_FALSE;

	D3DXVECTOR3 vVecOutput;
	D3DXVECTOR3 vNorOutput;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.

	m_pmcMesh->CalculateVertexInfluences ( *pVertInflu, vVecOutput, vNorOutput, NULL, bIdentity, *m_scpSkeleton.get(), matIdentity );

	pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
	EDITMESHS::RENDERSPHERE ( pd3dDevice, vVecOutput, 0.35f );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

	return S_OK;
}

void DxSkinPiece::ClearAll ()
{
	SAFE_DELETE_ARRAY(m_szFileName);
	SAFE_DELETE_ARRAY(m_szXFileName);
	SAFE_DELETE_ARRAY(m_szSkeleton);
	SAFE_DELETE_ARRAY(m_szMeshName);

	SAFE_DELETE_ARRAY(m_arrayMaterialPiece);

	m_skinMeshRes.Clear();
	m_skeletonRes.Clear();
	
	m_emType = PIECE_HEAD;
	m_emAttBoneType = ATTBONE_BODY;
	m_emV_PartType = PART_V_BODY;
    m_kLODInfo.clear();    

	m_pmcMesh = NULL;

	m_vecTrace.clear ();

	m_dwFlags = NULL;

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT.erase ( m_vecEFFECT.begin(), m_vecEFFECT.end() );
	m_vecEFFECT.clear();

	//std::for_each ( m_vecEFFECT_CharPart.begin(), m_vecEFFECT_CharPart.end(), std_afunc::DeleteObject() );
	//m_vecEFFECT_CharPart.erase ( m_vecEFFECT_CharPart.begin(), m_vecEFFECT_CharPart.end() );

	m_dwMaterialNum = 0;

	m_emHairHairOption = HAT_HAIR_BASE;
	m_strAddHairCps = _T("");
}

HRESULT DxSkinPiece::CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if ( !m_skinMeshRes.IsValid() || !m_pmcMesh )		return E_FAIL;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	//	Note : ���� ��Ʈ���� ������Ʈ.
	//
	if ( m_skeletonRes.IsValid() )
	{
		m_skeletonRes.get()->ResetBone_PureThread();
		m_skeletonRes.get()->UpdateBones_PureThread( m_skeletonRes.get()->m_skeletonPart.pBoneRoot, matIdentity, 1.f, 1.f );
	}

	HRESULT hr = m_skinMeshRes.get()->CalculateBoundingBox ( pd3dDevice, m_pmcMesh, vMax, vMin );

	return S_OK;
}

//	pBoneMatrice -> (����) ������ �޴� ���� ������ �������� ��쿡 ������ ����.
//
HRESULT DxSkinPiece::CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice )
{
	SVERTEXINFLU* pVertInflu = GetTracePos(strTrace);
	if ( !pVertInflu )	return E_FAIL;

	// Thread Loading �۾����� ���� ���� m_pmcMesh �� �ε� �ȵ� ������ ���� �ִ�.
	// �� �� return ���� E_FAIL �� �ָ� �ȵǰ� S_OK �� �Ѱܾ� �Ѵ�.
	if ( !m_pmcMesh )	
		return S_OK;

	////	TODO::
	////	Note : �Ϲ� �޽��� ��� Bone�� ��ũ�Ǿ� �ִ°� ����.
	////		���� ������ ���� �ٸ� ���� ���ȴٴ°� �����Ͽ� ���� �ٽ� ����	������.
	////
	//if ( !m_pmcMesh->pSkinInfo )
	//{
	//	//	Note : ���� �޴� ���� matrix �����͸� �ٽ� ã�´�.
	//	//
	//	//	m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- �̰͸� ������ BONE���� ������ �޴°�.

	//	DxBoneTrans* pHandHeld = m_skeletonRes.get()->FindBone ( m_pmcMesh->szBoneName );

	//	if ( pHandHeld )	m_pmcMesh->ppBoneMatrixPtrs[2] = &pHandHeld->matCombined;
	//	else				m_pmcMesh->ppBoneMatrixPtrs[2] = NULL;
	//}

	//return m_pmcMesh->CalculateVertexInfluences ( pVertInflu, vVert, vNormal, pBoneMatrice );

	if ( !m_pmcMesh->pSkinInfo )
	{
		vVert = pVertInflu->m_vVector;
		vNormal = pVertInflu->m_vNormal;

		return S_OK;
	}
	else
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );

		m_scpSkeleton->Import_PureThread( m_skeletonRes.get() );		// ������� ����Ǹ鼭 ������ ���� �� �ִ�.

		return m_pmcMesh->CalculateVertexInfluences ( *pVertInflu, vVert, vNormal, pBoneMatrice, FALSE, *m_scpSkeleton.get(), matIdentity );
	}
}

void DxSkinPiece::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
        m_vecEFFECT[i]->ReloadVMFX( pd3dDevice );
	}
}

void DxSkinPiece::CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD )
{
	if( !m_pmcMesh )	return;
	m_pmcMesh->IsCartoon_CreateMesh( pd3dDevice, fThickness, fSizeADD );
}

void DxSkinPiece::CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pmcMesh )	return;
	m_pmcMesh->CreateNormalMapMESH( pd3dDevice );
}

float DxSkinPiece::GetDistanceFromCamera(const D3DXVECTOR3& vPos)
{
    const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
    const D3DXVECTOR3 vDiff = vPos - vFromPt;
    return D3DXVec3Length(&vDiff);
}

int DxSkinPiece::GetLODLevel(const D3DXVECTOR3& vPos)
{
    if (m_kLODInfo.empty() == false)
    {
        float fDist = GetDistanceFromCamera(vPos);
        size_t dwSize = m_kLODInfo.size();

        for (size_t i = dwSize; i > 0; --i)
        {
            if (m_kLODInfo.kDistance[i - 1] <= fDist)
            {
                return i - 1;
            }
        }
    }

    return NO_LOD;
}

HRESULT DxSkinPiece::UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	return S_OK;

    //int nLevel = GetLODLevel(vPos);

    //if (m_kLODInfo.nLevel != nLevel)
    //{
    //    if (UpdateLODData(nLevel, pd3dDevice))
    //    {
    //        m_kLODInfo.nLevel = nLevel;
    //        return S_OK;
    //    }
    //}

    //return S_FALSE;
}

bool IsTexture(TextureResource& kRes)
{
    return kRes.GetFileName().empty() != true;
}

bool IsLoadedTexture(TextureResource& kRes)
{
    kRes.Update();

    if (kRes.IsValid() == false)
    {
        return false;
    }

    return true;
}

bool IsTextureLoading(DxSkinPiece* pTemp)
{
    if (pTemp && pTemp->m_pmcMesh)
    {
        for (DWORD i = 0; i < pTemp->m_dwMaterialNum; ++i)
        {
            TextureResource& kPieceRes = pTemp->m_pmcMesh->m_pMaterialEx[i].m_textureRes;

            if (IsTexture(kPieceRes))
            {
                if (IsLoadedTexture(kPieceRes) == false)
                {
                    return false;
                }
            }

            TextureResource& kCharRes = pTemp->m_arrayMaterialPiece[i].m_textureRes;

            if (IsTexture(kCharRes))
            {
                if (IsLoadedTexture(kCharRes) == false)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

BOOL DxSkinPiece::UpdateLODData(int nLevel, LPDIRECT3DDEVICEQ pd3dDevice)
{
	return TRUE;
   // // �۾��ؾ� �Ұ͵��� �ִ�.~ �ε��� �Ϸ� �Ǿ����� �۵��ȵǾ�ȴ�. There are things to work on. ~ Once loading is complete, it won't work.
   // if (m_bCompletedLOAD)
   // {
   //     if (m_skinMeshRes.Update())
   //     {
   //         if (IsTextureLoading(this) == false)
   //         {
   //             return FALSE;
   //         }

   //         const TCHAR* pMeshName = 0;

   //         if (nLevel == NO_LOD || m_kLODInfo.empty())
   //         {
   //             pMeshName = m_szMeshName;
   //         }
   //         else
   //         {
   //             pMeshName = m_kLODInfo.kMeshName[nLevel].c_str();
   //         }

   //         //m_pmcMesh = m_skinMeshRes.get()->FindMeshContainer (const_cast<TCHAR*>(pMeshName));
			//m_pmcMesh = m_skinMeshRes.get()->FindMeshContainer(pMeshName);

   //         if (m_pmcMesh == 0)
   //         {
   //             if (m_bActiveErrorLog == FALSE)
   //             {
   //                 m_bActiveErrorLog = TRUE;

   //                 // ~.cps ������ ~.x �� ���ӻ� ~.x ���� ������ ���� �ʴ� ������ ����. When creating, a problem occurs where the ~.x and in-game ~.x file information do not match.
   //                 CDebugSet::ToLogFile( sc::string::format( _T("%1%, %2% does not contain %3%."), m_szFileName, m_szXFileName, m_szMeshName ) );
   //             }

   //             return FALSE;
   //         }

			//SetMaterialSetting( pd3dDevice, FALSE, TRUE );

   //         // Note : �������ʹ� ���� ������ ������ ���� ���̴�. From now on, we will have the number of vertices.
   //         //m_dwVertexNUM = m_pmcMesh->m_dwVertexNUM;

   //         // Note : Cartoon Mesh�� �����Ѵ�.
   //         m_skinMeshRes.get()->Load(m_szXFileName, pd3dDevice);            

   //         return TRUE;
   //     }
   // }

   // return FALSE;
}
 
// Material ���� ���� (Previous settings) DxEffCharNone �� ���� ��� (If you wrote), ���ο� new Material Rendering �ʿ��� �⺻������ ���ؾ� �Ѵٰ� �˾������� �Ѵ�.You should be aware that you should not do basic rendering on the side.
void DxSkinPiece::SetBaseRenderOff()
{
	if ( !m_arrayMaterialPiece )
		return;

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_arrayMaterialPiece[i].m_bBaseRenderOff = TRUE;
	}
}

const TCHAR* DxSkinPiece::GetLinkBoneName() const
{
	if ( !m_pmcMesh )
		return NULL;

	if ( m_pmcMesh->m_strBoneName.empty() )
		return NULL;

	return m_pmcMesh->m_strBoneName.c_str();
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxSkinPieceContainer::DxSkinPieceContainer() :
	m_pd3dDevice(NULL)
{	
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxSkinPieceContainer::~DxSkinPieceContainer()
{
}

void DxSkinPieceContainer::SetPath(char* szPath)
{ 
	StringCchCopy( m_szPath, MAX_PATH, szPath ); 
}

DxSkinPiece* DxSkinPieceContainer::LoadPiece(const std::string& strFile, DWORD dwFlags )
{
	if ( strFile.empty() )
		return NULL;

	HRESULT hr;
	
	DxSkinPiece* pSkinPiece = NULL;

	BOOL bMeshThread(FALSE);
	if( dwFlags & EMSCD_MESH_THREAD )
	{
		bMeshThread = TRUE;
	}

	pSkinPiece = FindPiece( strFile );
	if ( pSkinPiece )
	{
		if( !bMeshThread )
		{
			if ( !pSkinPiece->ImmediatelyLoad( m_pd3dDevice ) )
				return NULL;
		}
		return pSkinPiece;
	}

	BOOL bTexThread(FALSE);
	if ( dwFlags & EMSCD_TEXTURE_THREAD )
	{
		bTexThread = TRUE;
	}

	//	Note : SkinPiece Load ����.
	//
	pSkinPiece = new DxSkinPiece;

	hr = pSkinPiece->LoadPiece( strFile, m_pd3dDevice, bMeshThread, bTexThread );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pSkinPiece);
		return NULL;
	}

	if( pSkinPiece->m_szFileName )
	{
		int nStrLen = (int) (_tcslen(pSkinPiece->m_szFileName)+1);
		StringCchCopy( pSkinPiece->m_szFileName, nStrLen, strFile.c_str() );
		pSkinPiece->m_emSkinPieceType = DxSkinPieceBase::SKIN_PIECE;

		// ����Ʈ�� ����.
		//size_t HashValue = m_StringHash( pSkinPiece->m_szFileName );
		m_mapLoading.insert(PIECEMAP_VALUE(pSkinPiece->m_szFileName, pSkinPiece));
	}
	else	
	{
		SAFE_DELETE(pSkinPiece);
		return NULL;
	}

	return pSkinPiece;
}

HRESULT DxSkinPieceContainer::ReleasePiece(const std::string& FileName)
{
	//	Note : ���� ���� ��ä �ı� �����󿡼� �ļ����϶� ���� ���� ���ɼ��� ����. Prevents the possibility of causing an error if the global object is of lower priority in the destruction order.
	if (m_mapSkinPiece.empty())
        return E_FAIL;

	PIECEMAP_ITER iter = m_mapSkinPiece.find( FileName );
	if (iter == m_mapSkinPiece.end())
        return E_FAIL;

	iter->second->m_dwRef--;
	if ( iter->second->m_dwRef == 0 )
	{
		delete iter->second;
		m_mapSkinPiece.erase ( iter );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeletePiece ( const char* szFile )
{
	if ( NULL == szFile )
		return S_FALSE;
	else
		return DeletePiece( TSTRING(szFile) );
}

HRESULT DxSkinPieceContainer::DeletePiece(const TSTRING& strFile)
{	
	//size_t HashValue = m_StringHash( strFile );
	PIECEMAP_ITER iter = m_mapSkinPiece.find(strFile);
	if (iter != m_mapSkinPiece.end())
	{
		delete iter->second;
		m_mapSkinPiece.erase( iter );
		return S_OK;
	}

	iter = m_mapLoading.find(strFile);
	if (iter != m_mapLoading.end())
	{
		delete iter->second;
		m_mapLoading.erase( iter );
		return S_OK;
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeletePiece(DxSkinPiece* pDeletePiece)
{
    PIECEMAP_ITER it = m_mapSkinPiece.begin();

    for ( ; it != m_mapSkinPiece.end(); ++it)
    {
        if (it->second == pDeletePiece)
        {
            delete it->second;
            m_mapSkinPiece.erase(it);
            return S_OK;
        }
    }

    it = m_mapLoading.begin();

    for ( ; it != m_mapLoading.end(); ++it)
    {
        if (it->second == pDeletePiece)
        {
            delete it->second;
            m_mapLoading.erase(it);
            return S_OK;
        }
    }

    return S_OK;
}

DxSkinPiece* DxSkinPieceContainer::FindPiece(const TSTRING& strFile)
{
	//MIN_ASSERT(szFile);

	//size_t HashValue = m_StringHash( strFile );
	PIECEMAP_ITER iter = m_mapSkinPiece.find(strFile);
	if (iter != m_mapSkinPiece.end())
	{
		iter->second->m_dwRef++;
		return iter->second;
	}

	iter = m_mapLoading.find(strFile);
	if (iter != m_mapLoading.end())
	{
		iter->second->m_dwRef++;
		return iter->second;
	}

	return NULL;
}

HRESULT DxSkinPieceContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->InitDeviceObjects ( pd3dDevice );
	}

	iter = m_mapLoading.begin();
	iter_end = m_mapLoading.end();

	for( ; iter!=iter_end; iter++ )
	{
		iter->second->InitDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->RestoreDeviceObjects ( pd3dDevice );
	}

	iter = m_mapLoading.begin();
	iter_end = m_mapLoading.end();
	for( ; iter!=iter_end; iter++ )
	{
		iter->second->RestoreDeviceObjects ( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::InvalidateDeviceObjects ()
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; ++iter )
	{
		iter->second->InvalidateDeviceObjects ();
	}

	iter = m_mapLoading.begin();
	iter_end = m_mapLoading.end();
	for( ; iter!=iter_end; iter++ )
	{
		iter->second->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::DeleteDeviceObjects()
{
	PIECEMAP_ITER iter = m_mapSkinPiece.begin();
	PIECEMAP_ITER iter_end = m_mapSkinPiece.end();

	for ( ; iter!=iter_end; iter++ )
	{
		iter->second->DeleteDeviceObjects ();
	}

	iter = m_mapLoading.begin();
	iter_end = m_mapLoading.end();
	for( ; iter!=iter_end; iter++ )
	{
		iter->second->DeleteDeviceObjects ();
	}

	return S_OK;
}

HRESULT DxSkinPieceContainer::FinalCleanup ()
{
	std::for_each ( m_mapSkinPiece.begin(), m_mapSkinPiece.end(), std_afunc::DeleteMapObject() );
	m_mapSkinPiece.clear ();

	std::for_each ( m_mapLoading.begin(), m_mapLoading.end(), std_afunc::DeleteMapObject() );
	m_mapLoading.clear ();

	return S_OK;
}

HRESULT DxSkinPieceContainer::CleanUp ()
{
	InvalidateDeviceObjects ();
	DeleteDeviceObjects ();
	FinalCleanup ();

	return S_OK;
}

void DxSkinPieceContainer::NSThreadCORE_UpdateData( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	PIECEMAP_ITER iter = m_mapLoading.begin();
	PIECEMAP_ITER iter_end = m_mapLoading.end();
	for ( ; iter!=iter_end; )
	{
		if ( !(*iter).second->NSThreadCORE_UpdateData( pd3dDevice, TRUE ) )
		{
			// [shhan][2015.01.08] ������ �ϰ� �Ǹ� DxCharPart::SetPart_Detail �۾��� m_rSkinPiece ���� ������ ���� �Ǿ ������ �����. 
			// If deleted, a problem occurs because the m_rSkinPiece value becomes a garbage value when working with DxCharPart::SetPart_Detail.
			//						������ ���� ����, m_bCompletedLOAD �� FALSE �� ���·� ����ϵ��� �Ѵ�. Do not delete it, but register it with m_bCompletedLOAD set to FALSE.
			//
			//// �߸��� �����Ͷ�� ������ �Ѵ�. If the data is incorrect, delete it.
			//SAFE_DELETE ( (*iter).second );
			//iter = m_mapLoading.erase( iter );

			m_mapSkinPiece[(*iter).first] = (*iter).second;
			iter = m_mapLoading.erase( iter );
			continue;
		}

		if( (*iter).second->m_bCompletedLOAD )
		{
			m_mapSkinPiece[(*iter).first] = (*iter).second;
			iter = m_mapLoading.erase( iter );
		}
		else
		{
			++iter;
		}
	}		
}

//
static void FindDontHaveTexture( const TCHAR* FileName, const TCHAR* pExt )
{
	TSTRING strName = FileName;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );
	
	strName = sc::file::getFileExt( strName );
	if ( strName != pExt )
		return;

	DxSkinPieceContainer::GetInstance().LoadPiece( FileName, 0L );
}

// �ε��� ���� �غ����� �Ѵ�. Let's try loading everything.
void DxSkinPieceContainer::OnFindDontHaveTexture()
{
	sc::CFileFindTree	m_sFileTree;
	m_sFileTree.CreateTree( m_szPath, FALSE );
	m_sFileTree.SomeWorkSameExt( FindDontHaveTexture, "cps" );
}

void DxSkinPieceContainer::Insert_ExportEffTexture_NoHave_TextureName( const TSTRING& strName )
{
	m_setExportEffTexture_NoHave_TextureName.insert( strName );
}

void DxSkinPieceContainer::Dialog_ExportEffTexture_NoHave_TextureName()
{
	if ( m_setExportEffTexture_NoHave_TextureName.empty() )
		return;

	TSTRING strName;
	BOOST_FOREACH( std::set<TSTRING>::value_type& iter, m_setExportEffTexture_NoHave_TextureName )
	{
		strName += _T("\r\n");
		strName += iter;
	}
	m_setExportEffTexture_NoHave_TextureName.clear();

	AfxMessageBox( strName.c_str() );
}

void DxSkinPieceContainer::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(PIECEMAP_VALUE& it, m_mapSkinPiece)
    {
        fun( (m_szPath + it.first).c_str() );
    }
}
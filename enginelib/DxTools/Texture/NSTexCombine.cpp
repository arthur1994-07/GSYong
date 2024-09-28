#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"

#include "../../DxEffect/Char/DxEffCharUserColor.h"
#include "../../DxEffect/Char/DxEffCharOverlay.h"

#include "../Material/NSMaterialManager.h"
#include "../DxBackUpRendTarget.h"
#include "../TextureManager.h"
#include "../DxVertexFVF.h"
#include "../RENDERPARAM.h"

#include "./NSTexCompress.h"

#include "NSTexCombine.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSTexCombine
{
	LPDIRECT3DVERTEXDECLARATION9	g_pDCRT;
	LPD3DXEFFECT					g_pFX(NULL);

	LPD3DXEFFECT SetFX_Overlay( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		TSTRING strFX = 
			"//-------------------------------------------------------------------------------------- \r\n"
			"//       shhan \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			" \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"// Global variables \r\n"
			"//-------------------------------------------------------------------------------------- \r\n"
			"float4 g_vColor1; \r\n"
			"float4 g_vColor2; \r\n"

			"struct VS_QUAD \r\n"
			"{\r\n"
			"	float4  m_vPos				: POSITION;\r\n"
			"	float2  m_vTex				: TEXCOORD0;\r\n"
			"}; \r\n"

			"texture g_BaseTexture; \r\n"
			"sampler BaseTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_BaseTexture); \r\n"
			"}; \r\n"

			"texture g_MaskTexture; \r\n"
			"sampler MaskTexSampler = sampler_state  \r\n"
			"{ \r\n"
			"	Texture = (g_MaskTexture); \r\n"
			"}; \r\n"

			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"
			"// 원본 \r\n"
			"float4 PS( VS_QUAD In ) : COLOR0  \r\n"
			"{ \r\n"
			"   // Texture \r\n"
			"   float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"   float4 vMask = tex2D( MaskTexSampler, In.m_vTex ); \r\n"

			"	float3 vColor1, vColor2;       \r\n"

			"	vColor1 = ( 2 * vAlbedo.rgb ) * g_vColor1.rgb;\r\n"
			"	vColor2 = ( 2 * vAlbedo.rgb ) * g_vColor2.rgb;\r\n"

			"	if ( vAlbedo.r > 0.5 )\r\n"
			"		vColor1.r = 1 - ( 1 - ( 2 * (vAlbedo.r-0.5) ) ) * (1-g_vColor1.r);\r\n"

			"	if ( vAlbedo.g > 0.5 )\r\n"
			"		vColor1.g = 1 - ( 1 - ( 2 * (vAlbedo.g-0.5) ) ) * (1-g_vColor1.g);\r\n"

			"	if ( vAlbedo.b > 0.5 )\r\n"
			"		vColor1.b = 1 - ( 1 - ( 2 * (vAlbedo.b-0.5) ) ) * (1-g_vColor1.b);\r\n"

			"	if ( vAlbedo.r > 0.5 )\r\n"
			"		vColor2.r = 1 - ( 1 - ( 2 * (vAlbedo.r-0.5) ) ) * (1-g_vColor2.r);\r\n"

			"	if ( vAlbedo.g > 0.5 )\r\n"
			"		vColor2.g = 1 - ( 1 - ( 2 * (vAlbedo.g-0.5) ) ) * (1-g_vColor2.g);\r\n"

			"	if ( vAlbedo.b > 0.5 )\r\n"
			"		vColor2.b = 1 - ( 1 - ( 2 * (vAlbedo.b-0.5) ) ) * (1-g_vColor2.b);\r\n"

			"	float3 vDestColor = vAlbedo.rgb - ( ( vAlbedo.rgb * vMask.r ) + ( vAlbedo.rgb * vMask.b ) );\r\n"

			"	float3 vOverlay = ( vColor1.rgb * vMask.r ) + ( vColor2.rgb * vMask.b );\r\n"

			"	float4 vColor;\r\n"
			"	vColor.xyz = vDestColor.rgb + vOverlay.rgb;\r\n"
			"	vColor.w = vAlbedo.w;    	\r\n"

			"   return vColor; \r\n"
			"} \r\n"

			"//---------------------------------------------------------------------------------------------------------- \r\n"
			"//					P	S \r\n"	// UserColor
			"// 원본 \r\n"
			"float4 PS_UserColor( VS_QUAD In ) : COLOR0  \r\n"
			"{ \r\n"
			"   // Texture \r\n"
			"   float4 vAlbedo = tex2D( BaseTexSampler, In.m_vTex ); \r\n"
			"   float4 vMask = tex2D( MaskTexSampler, In.m_vTex ); \r\n"

			"	float3 vColor1;       \r\n"

			"	vColor1 = ( 2 * vAlbedo.rgb ) * g_vColor1.rgb;\r\n"

			"	if ( vAlbedo.r > 0.5 )\r\n"
			"		vColor1.r = 1 - ( 1 - ( 2 * (vAlbedo.r-0.5) ) ) * (1-g_vColor1.r);\r\n"

			"	if ( vAlbedo.g > 0.5 )\r\n"
			"		vColor1.g = 1 - ( 1 - ( 2 * (vAlbedo.g-0.5) ) ) * (1-g_vColor1.g);\r\n"

			"	if ( vAlbedo.b > 0.5 )\r\n"
			"		vColor1.b = 1 - ( 1 - ( 2 * (vAlbedo.b-0.5) ) ) * (1-g_vColor1.b);\r\n"

			"	float3 vDestColor = vAlbedo.rgb - ( ( vAlbedo.rgb * vMask.a ) );\r\n"

			"	float3 vOverlay = ( vColor1.rgb * vMask.a );\r\n"

			"	float4 vColor;\r\n"
			"	vColor.xyz = vDestColor.rgb + vOverlay.rgb;\r\n"
			"	vColor.w = vAlbedo.w;    	\r\n"

			"   return vColor; \r\n"
			"} \r\n"

			"technique Base \r\n"
			"{ \r\n"
			"   //------------------------------------------------------------------------------------- \r\n"
			"   // 기본 _ 0 - PS_2_0 이상 지원 \r\n"
			"   pass p0 \r\n"	// Overlay
			"   { \r\n"
			"		PixelShader = compile ps_2_0 PS(); \r\n"
			"   } \r\n"
			"   pass p1 \r\n"	// USerColor
			"   { \r\n"
			"		PixelShader = compile ps_2_0 PS_UserColor(); \r\n"
			"   } \r\n"
			"} \r\n";

		LPD3DXEFFECT pFX(NULL);
		UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
		D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );

		return pFX;
	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		SAFE_RELEASE ( g_pFX );
		g_pFX = SetFX_Overlay( pd3dDevice );
	//	D3DVERTEXELEMENT9 VertexDecl[] =
	//	{
	//		{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

	//		D3DDECL_END()
	//	};

	//	SAFE_RELEASE ( g_pDCRT );
	//	pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT );
	}

	void OnLostDevice()
	{
		//SAFE_RELEASE ( g_pDCRT );
		SAFE_RELEASE ( g_pFX );
	}

	void CreateBakeTexDetail( LPDIRECT3DDEVICEQ pd3dDevice,
								TextureResource* ptextureRes, 
								LPDIRECT3DTEXTUREQ pMaskTex, 
								const D3DXVECTOR4* pColor1, 
								const D3DXVECTOR4* pColor2, 
								LPDIRECT3DTEXTUREQ& pResultTex, 
								int nDivision,
								UINT unPass )
	{
		//if ( !g_pDCRT )
		//	return;

		if ( !g_pFX )
			return;

		if ( !ptextureRes->GetTexture() )
		{
			return;
		}

		// 없다면 만들도록 하자.
		// FakeSSS 때문에 만들어 줘야한다.
		LPDIRECT3DTEXTUREQ	pBakeMaskTexTemp(NULL);
		LPDIRECT3DSURFACEQ	pBakeMaskSuf(NULL);
		DWORD dwWidth = ptextureRes->GetInfo().Width/nDivision;
		DWORD dwHeight = ptextureRes->GetInfo().Height/nDivision;

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
		//						사용상 주의
		// pd3dDevice->BeginScene 이 이전에 호출 되어있으면 hr 에 잘못된 값이 나올 수 있다.
		HRESULT hr_BeginPass = pd3dDevice->BeginScene();





		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			pd3dDevice->SetRenderTarget( 0, pBakeMaskSuf );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,128,128,128), 1.0f, 0L );

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

			g_pFX->SetTexture( "g_BaseTexture", ptextureRes->GetTexture() );
			g_pFX->SetTexture( "g_MaskTexture", pMaskTex );

			//DWORD dwAlphaBlendEnable;
			//DWORD dwSeparateAlphaBlendEnable;
			DWORD dwZWriteEnable;
			DWORD dwZEnable;
			DWORD dwFogEnable;
			//pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,			&dwAlphaBlendEnable );
			//pd3dDevice->GetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	&dwSeparateAlphaBlendEnable );
			pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,				&dwZWriteEnable );
			pd3dDevice->GetRenderState( D3DRS_ZENABLE,					&dwZEnable );
			pd3dDevice->GetRenderState( D3DRS_FOGENABLE,				&dwFogEnable );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,				FALSE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,					FALSE );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,				FALSE );

			g_pFX->SetVector( "g_vColor1", pColor1 );
			g_pFX->SetVector( "g_vColor2", pColor2 );

			//if ( RENDERPARAM::g_bSeparateAlphaBlend )
			//{
			//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
			//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_ONE );
			//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_ZERO );
			//	pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	TRUE );
			//	pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA,			D3DBLEND_ONE );
			//	pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA,			D3DBLEND_ZERO );
			//}

			//pd3dDevice->SetVertexDeclaration( g_pDCRT );

			////////////////////////////////////////////////////////////////////////
			// PixelShader만 쓸 경우 이건 맞춰줘야한다.
			// DrawPrimitiveUP 를 쓸 경우 SetFVF 가 필요한거 같음.
			DWORD dwFVF;
			pd3dDevice->GetFVF(&dwFVF);
			pd3dDevice->SetFVF(VERTEXRHW::FVF);

			//D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("Base");
			//if ( hTechniqueHandle )
			{
				//////////////////////////////////////////////////////////////////////////
				////						SetTechnique
				//pFX->SetTechnique(hTechniqueHandle);

				UINT numPasses;
				g_pFX->Begin( &numPasses, 0 );
				if( unPass < numPasses )
				{
					g_pFX->BeginPass( unPass );
					{
						// FX 마무리
						g_pFX->CommitChanges();

						//pd3dDevice->SetFVF( VERTEXRHW::FVF );
						pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertBase, sizeof(VERTEXRHW) );
					}
					g_pFX->EndPass();
				}
				g_pFX->End();
			}

			pd3dDevice->SetFVF(dwFVF);

			//if ( RENDERPARAM::g_bSeparateAlphaBlend )
			//{
			//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			dwAlphaBlendEnable );
			//	pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	dwSeparateAlphaBlendEnable );
			//}

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    dwZWriteEnable );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    dwZEnable );
			pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		dwFogEnable );
		}
		


		//////////////////////////////////////////////////////////////////////////
		//						EndScene
		//						사용상 주의
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
		//					속도가 빠르지만 용량이 크다. Release 문제가 있다.
		//
		SAFE_RELEASE( pResultTex );
		pResultTex = reinterpret_cast<LPDIRECT3DTEXTURE9>( pBakeMaskTexTemp );

		//////////////////////////////////////////////////////////////////////////
		//					속도가 느리다. 용량이 줄어든다. Release 문제는 없다.
		//
		//LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
		//NSTexCompress::Compress
		//(
		//	pd3dDevice,
		//	dwWidth,
		//	dwHeight,
		//	0,
		//	1,
		//	ptextureRes->GetInfo().Format,
		//	pBakeMaskTexTemp,
		//	pTexNew
		//);

		//SAFE_RELEASE ( pBakeMaskTexTemp );

		//SAFE_RELEASE( pResultTex );
		//pResultTex = reinterpret_cast<LPDIRECT3DTEXTURE9>( pTexNew );
	}

	void CreateOverlay( LPDIRECT3DDEVICEQ pd3dDevice, 
						int nMaterial,
						TextureResource* ptextureRes, 
						DxEffCharOverlay* pOverlay, 
						DWORD dwUserColor1, 
						DWORD dwUserColor2, 
						LPDIRECT3DTEXTUREQ& pResultTex )
	{
		if ( !pOverlay )
		{
			return;
		}

		if ( !ptextureRes->GetTexture() )
			return;

		pOverlay->LoadOverlayMaskTexture( nMaterial );

		LPDIRECT3DTEXTUREQ pOverlayMaskTex = pOverlay->GetOverlayMaskTexture( nMaterial );
		if ( !pOverlayMaskTex )
		{
			return;
		}

		//if ( pOverlay->m_nSelectOverlay == nMaterial )
		{
			D3DXVECTOR4 sColor1( static_cast<float>((dwUserColor1&0xff0000)>>16)/255.f, 
								static_cast<float>((dwUserColor1&0xff00)>>8)/255.f, 
								static_cast<float>((dwUserColor1&0xff))/255.f,
								1.f );

			D3DXVECTOR4 sColor2( static_cast<float>((dwUserColor2&0xff0000)>>16)/255.f, 
								static_cast<float>((dwUserColor2&0xff00)>>8)/255.f, 
								static_cast<float>((dwUserColor2&0xff))/255.f,
								1.f );

			CreateBakeTexDetail( pd3dDevice, ptextureRes, pOverlayMaskTex, &sColor1, &sColor2, pResultTex, 1, 0 );
		}
		//else
		//{
		//	const D3DXVECTOR4* pColor1(NULL);
		//	const D3DXVECTOR4* pColor2(NULL);
		//	pOverlay->GetOverlayColor_1_2( nMaterial, &pColor1, &pColor2 );

		//	CreateBakeTexDetail( pd3dDevice, ptextureRes, pOverlayMaskTex, pColor1, pColor2, pResultTex, 1, 0 );
		//}
	}

	void CreateOverlayForEdit( LPDIRECT3DDEVICEQ pd3dDevice, 
								int nMaterial,
								TextureResource* ptextureRes, 
								DxEffCharOverlay* pOverlay,
								LPDIRECT3DTEXTUREQ& pResultTex )
	{
		if ( !pOverlay )
			return;

		if ( !ptextureRes->GetTexture() )
			return;

		pOverlay->LoadOverlayMaskTexture( nMaterial );

		LPDIRECT3DTEXTUREQ pOverlayMaskTex = pOverlay->GetOverlayMaskTexture( nMaterial );
		if ( !pOverlayMaskTex )
			return;

		{
			const D3DXVECTOR4* pColor1(NULL);
			const D3DXVECTOR4* pColor2(NULL);
			pOverlay->GetOverlayColor_1_2( nMaterial, &pColor1, &pColor2 );

			CreateBakeTexDetail( pd3dDevice, ptextureRes, pOverlayMaskTex, pColor1, pColor2, pResultTex, 1, 0 );
		}
	}

	void CreateUserColor( LPDIRECT3DDEVICEQ pd3dDevice, 
						int nMaterial,
						TextureResource* ptextureRes, 
						DxEffCharUserColor* pUserColor, 
						DWORD dwUserColor, 
						LPDIRECT3DTEXTUREQ& pResultTex )
	{
		if ( !pUserColor )
			return;

		if ( !ptextureRes->GetTexture() )
			return;

		pUserColor->LoadUserColorMaskTexture( nMaterial );

		LPDIRECT3DTEXTUREQ pUserColorMaskTex = pUserColor->GetUserColorMaskTexture( nMaterial );
		if ( !pUserColorMaskTex )
		{
			pUserColorMaskTex = ptextureRes->GetTexture();
			//return;
		}

		D3DXVECTOR4 sColor;
		pUserColor->GetUserColor( nMaterial, sColor );

		CreateBakeTexDetail( pd3dDevice, ptextureRes, pUserColorMaskTex, &sColor, &sColor, pResultTex, 1, 1 );
	}

	void CreateUserColorForEdit( LPDIRECT3DDEVICEQ pd3dDevice, 
								int nMaterial,
								TextureResource* ptextureRes, 
								DxEffCharUserColor* pUserColor,
								LPDIRECT3DTEXTUREQ& pResultTex )
	{
		if ( !pUserColor )
			return;

		if ( !ptextureRes->GetTexture() )
			return;

		pUserColor->LoadUserColorMaskTexture( nMaterial );

		LPDIRECT3DTEXTUREQ pUserColorMaskTex = pUserColor->GetUserColorMaskTexture( nMaterial );
		if ( !pUserColorMaskTex )
		{
			pUserColorMaskTex = ptextureRes->GetTexture();
			//return;
		}

		D3DXVECTOR4 sColor;
		pUserColor->GetUserColor( nMaterial, sColor );

		CreateBakeTexDetail( pd3dDevice, ptextureRes, pUserColorMaskTex, &sColor, &sColor, pResultTex, 1, 1 );
	}
}
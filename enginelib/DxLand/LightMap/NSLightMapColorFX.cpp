#include "pch.h"
//
//#include "../../DxTools/DxViewPort.h"
//#include "../../DxTools/DxFogMan.h"
//#include "../../G-Logic/GLPeriod.h"
//
//#include "NSLightMapColorFX.h"
//
//// ----------------------------------------------------------------------------
//#include "../../../SigmaCore/DebugInclude.h"
//// ----------------------------------------------------------------------------
//
//
//namespace NSLightMapColorFX
//{
//    enum RENDER_MODE
//    {
//        ENRM_ZERO       = 0,
//        ENRM_VS         = 1,
//        ENRM_VS_PS_1_1  = 2,
//        ENRM_VS_PS_1_4  = 3,
//    };
//
//    ID3DXEffect*						g_pFX;
//    TSTRING								g_strFX;
//    RENDER_MODE							g_emMode(ENRM_ZERO);
//	NSLIGHTMAPDEF::LIGHTMAP_RENDERTYPE	g_emRenderType(NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP);
//    LPDIRECT3DVERTEXDECLARATION9		g_pDCRT(NULL);
//
//    void CleanUp()
//    {
//        SAFE_RELEASE( g_pFX );
//    }
//
//    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
//    {
//        HRESULT hr(S_OK);
//
//        const D3DVERTEXELEMENT9 VertexDecl[] =
//        {
//		    { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,   0 },
//		    { 0, 12, D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,     0 },
//            { 0, 24, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,      0 },
//		    { 0, 28, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   0 },
//            { 0, 36, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   1 },
//    		        
//            D3DDECL_END()
//        };
//
//	    pd3dDevice->CreateVertexDeclaration( VertexDecl,  &g_pDCRT );
//
//        if( g_pFX )	
//            V_RETURN( g_pFX->OnResetDevice() );
//
//        return hr;
//    }
//
//    HRESULT OnLostDevice()
//    {
//        HRESULT hr(S_OK);
//
//        if( g_pFX )	
//            V_RETURN( g_pFX->OnLostDevice() );
//
//        SAFE_RELEASE( g_pDCRT );
//
//        return hr;
//    }
//
//    ID3DXEffect* GetFX()
//    {
//        return g_pFX;
//    }
//
//    void SetFX()
//    {
//        g_strFX = 
//        "//-------------------------------------------------------------------------------------- \r\n"
//        "// File: LightMap_Piece.fx \r\n"
//        "//       shhan \r\n"
//        "//-------------------------------------------------------------------------------------- \r\n"
//        " \r\n"
//        "//-------------------------------------------------------------------------------------- \r\n"
//        "// Global variables \r\n"
//        "//-------------------------------------------------------------------------------------- \r\n"
//        "float4x4 g_matWVP; \r\n"
//        "float2   g_vFOG;	// FogEnd, FogDist \r\n"
//        "float2   g_vLightMapUV_Offset; \r\n"
//        "float2   g_vDayNightWeight; \r\n"
//        //"float    g_fAlpha; \r\n"
//        " \r\n"
//        "texture BaseTex; \r\n"
//        "texture LightMapDayTex; \r\n"
//        "texture LightMapNightTex; \r\n"
//        " \r\n"
//        "sampler BaseTexSampler = sampler_state  \r\n"
//        "{ \r\n"
//        "    Texture = (BaseTex); \r\n"
//        "}; \r\n"
//        "  \r\n"
//        "sampler LightMapDayTexSampler = sampler_state  \r\n"
//        "{ \r\n"
//        "    Texture = (LightMapDayTex); \r\n"
//        "}; \r\n"
//        " \r\n"
//        "sampler LightMapNightTexSampler = sampler_state  \r\n"
//        "{ \r\n"
//        "    Texture = (LightMapNightTex); \r\n"
//        "}; \r\n"
//        " \r\n"
//        "//-------------------------------------------------------------------------------------- \r\n"
//        "// Basic vertex transformation \r\n"
//        "//-------------------------------------------------------------------------------------- \r\n"
//        "struct VS_INPUT { \r\n"
//        "    float4 m_vPosition  : POSITION; \r\n"
//        "    float3 m_vNormal    : NORMAL; \r\n"
//        "    float4 m_vColor0    : COLOR0; \r\n"
//        "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
//        "    float2 m_vTexCoord1 : TEXCOORD1; \r\n"
//        "}; \r\n"
//        " \r\n"
//        "struct VS_OUTPUT  \r\n"
//        "{ \r\n"
//        "    float4 m_vPosition  : POSITION; \r\n"
//        "    float4 m_vColor0      : COLOR0; \r\n"
//        "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
//        "    float2 m_vTexCoord1 : TEXCOORD1; \r\n"
//        "    float  m_fFog      : FOG; \r\n"
//        "}; \r\n"
//        " \r\n"
//        "struct VS_OUTPUT_BLEND \r\n"
//        "{ \r\n"
//        "    float4 m_vPosition  : POSITION; \r\n"
//        "    float4 m_vColor0      : COLOR0; \r\n"
//        "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
//        "    float2 m_vTexCoord1 : TEXCOORD1; \r\n"
//        "    float2 m_vTexCoord2 : TEXCOORD2; \r\n"
//        "    float  m_fFog      : FOG; \r\n"
//        "}; \r\n"
//        " \r\n"
//        "//---------------------------------------------------------------------------------------------------------- \r\n"
//        "//						V	S \r\n"
//        "VS_OUTPUT VS( VS_INPUT In )  \r\n"
//        "{ \r\n"
//        "    VS_OUTPUT Out; \r\n"
//        "     \r\n"
//        "    // Now that we have the calculated weight, add in the final influence \r\n"
//        "    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
//        " \r\n"
//        "    // Pass the texture coordinate \r\n"
//        "    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
//        "    Out.m_vTexCoord1 = In.m_vTexCoord1; \r\n"
//        "    Out.m_vTexCoord1 += g_vLightMapUV_Offset; \r\n"
//        " \r\n"
//        "    Out.m_vColor0 = In.m_vColor0; \r\n"
//        " \r\n"
//        "    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
//        "    \r\n"
//        "    return Out; \r\n"
//        "} \r\n"
//        " \r\n"
//        "//---------------------------------------------------------------------------------------------------------- \r\n"
//        "//						V	S	BLEND \r\n"
//        "VS_OUTPUT_BLEND VS_BLEND( VS_INPUT In )  \r\n"
//        "{ \r\n"
//        "    VS_OUTPUT_BLEND Out; \r\n"
//        "     \r\n"
//        "    // Now that we have the calculated weight, add in the final influence \r\n"
//        "    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
//        " \r\n"
//        "    // Pass the texture coordinate \r\n"
//        "    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
//        "    Out.m_vTexCoord1 = In.m_vTexCoord1; \r\n"
//        "    Out.m_vTexCoord1 += g_vLightMapUV_Offset; \r\n"
//        "    Out.m_vTexCoord2 = Out.m_vTexCoord1; \r\n"
//        " \r\n"
//        "    Out.m_vColor0 = In.m_vColor0; \r\n"
//        " \r\n"
//        "    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
//        "     \r\n"
//        "    return Out; \r\n"
//        "} \r\n"
//        " \r\n"
//        "//---------------------------------------------------------------------------------------------------------- \r\n"
//        "//					P	S	_	1	1 \r\n"
//        "// 원본 \r\n"
//        "float4 PS_1_1( VS_OUTPUT_BLEND In ) : COLOR0  \r\n"
//        "{ \r\n"
//        "    // Texture \r\n"
//        "    float4 vColor = tex2D( BaseTexSampler, In.m_vTexCoord0 ); \r\n"
//        "    float4 vLightMapDay = tex2D( LightMapDayTexSampler, In.m_vTexCoord1 ); \r\n"
//        "    float4 vLightMapNight = tex2D( LightMapNightTexSampler, In.m_vTexCoord2 ); \r\n"
//        " \r\n"
//        "    // Basic N dot L lighting \r\n"
//        "    vColor *= In.m_vColor0; \r\n"              // 순서를 밑으로 두면 이상하게 Render 된다. PS.2.0 이면 문제없이 잘된다.
//        "    float3 vLightColor = ( vLightMapDay*g_vDayNightWeight.x + vLightMapNight*g_vDayNightWeight.y ); \r\n"
//        "    vColor.xyz *= vLightColor.xyz * 2.f; \r\n" // 숫자를 2를 초과하면 이상하게 Render 된다. PS.2.0 이면 문제없이 잘된다.
//        " \r\n"
//        "    return vColor; \r\n"
//        "} \r\n"
//        " \r\n"
//        "//---------------------------------------------------------------------------------------------------------- \r\n"
//        "//					P	S	_	1	4 \r\n"
//        "// 원본 \r\n"
//        "float4 PS_1_4( VS_OUTPUT In ) : COLOR0  \r\n"
//        "{ \r\n"
//        "    // Texture \r\n"
//        "    float4 vColor = tex2D( BaseTexSampler, In.m_vTexCoord0 ); \r\n"
//        "    float4 vLightMapDay = tex2D( LightMapDayTexSampler, In.m_vTexCoord1 ); \r\n"
//        "    float4 vLightMapNight = tex2D( LightMapNightTexSampler, In.m_vTexCoord1 ); \r\n"
//        " \r\n"
//        "    // Basic N dot L lighting \r\n"
//        "    vColor *= In.m_vColor0; \r\n"              // 순서를 밑으로 두면 이상하게 Render 된다. PS.2.0 이면 문제없이 잘된다.
//        "    float3 vLightColor = ( vLightMapDay*g_vDayNightWeight.x + vLightMapNight*g_vDayNightWeight.y ); \r\n"
//        "    vColor.xyz *= vLightColor.xyz * 2.f; \r\n" // 숫자를 2를 초과하면 이상하게 Render 된다. PS.2.0 이면 문제없이 잘된다.
//        " \r\n"
//        "    return vColor; \r\n"
//        "} \r\n"
//		" \r\n"
//		"//---------------------------------------------------------------------------------------------------------- \r\n"
//		"//					P	S	_	Only LightMap \r\n"
//		"// 원본 \r\n"
//		"float4 PS_ONLY_LIGHTMAP( VS_OUTPUT In ) : COLOR0  \r\n"
//		"{ \r\n"
//		"    // Texture \r\n"
//		"    float4 vColor = 1.f; \r\n"
//		"    float4 vLightMapDay = tex2D( LightMapDayTexSampler, In.m_vTexCoord1 ); \r\n"
//		"    float4 vLightMapNight = tex2D( LightMapNightTexSampler, In.m_vTexCoord1 ); \r\n"
//		" \r\n"
//		"    // Basic N dot L lighting \r\n"
//		"    float4 vLightColor = vLightMapDay*g_vDayNightWeight.x + vLightMapNight*g_vDayNightWeight.y; \r\n"
//		"    vColor.xyz *= vLightColor*2.f; \r\n"
//		" \r\n"
//		"    return vColor; \r\n"
//		"} \r\n"
//		" \r\n"
//        "technique Base \r\n"
//        "{ \r\n"
//        "    //------------------------------------------------------------------------------------- \r\n"
//        "    // 기본 _ 0	- VS_1_1 이상 지원 \r\n"
//        "    pass p0  \r\n"
//        "    { \r\n"
//        "        VertexShader = compile vs_1_1 VS(); \r\n"
//        "    } \r\n"
//        " \r\n"
//        "    //------------------------------------------------------------------------------------- \r\n"
//        "    // 기본 _ 1 - VS_1_1 이상 지원 \r\n"
//        "    //          - PS_1_1 이상 지원 \r\n"
//        "    pass p1 \r\n"
//        "    { \r\n"
//        "        VertexShader = compile vs_1_1 VS_BLEND(); \r\n"
//        "        PixelShader = compile ps_1_1 PS_1_1(); \r\n"
//        "    } \r\n"
//        " \r\n"
//        "    //------------------------------------------------------------------------------------- \r\n"
//        "    // 기본 _ 2 - VS_1_1 이상 지원 \r\n"
//        "    //          - PS_1_4 이상 지원 \r\n"
//        "    pass p2 \r\n"
//        "    { \r\n"
//        "        VertexShader = compile vs_1_1 VS(); \r\n"
//        "        PixelShader = compile ps_1_4 PS_1_4(); \r\n"
//        "    } \r\n"
//		" \r\n"
//		"    //------------------------------------------------------------------------------------- \r\n"
//		"    // Only LightMap (Edit시에만 사용)	- VS_1_1 이상 지원 \r\n"
//		"    //									- PS_1_4 이상 지원 \r\n"
//		"    pass p3 \r\n"
//		"    { \r\n"
//		"        VertexShader = compile vs_1_1 VS(); \r\n"
//		"        PixelShader = compile ps_1_4 PS_ONLY_LIGHTMAP(); \r\n"
//		"    } \r\n"
//		"} \r\n";
//    }
//
//    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
//    {
//        HRESULT hr(S_OK);
//
//        CleanUp();
//
//	    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
//
//	    #ifdef DEBUG_VS
//		    dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
//	    #endif
//	    #ifdef DEBUG_PS
//		    dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
//	    #endif
//
//        SetFX();
//
//        UINT dwBufferSize = static_cast<UINT>( g_strFX.size() ) + 1; 
//        V_RETURN( D3DXCreateEffect( pd3dDevice, g_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &g_pFX, NULL ) );
//
//
//        {
//            g_emMode = ENRM_ZERO;
//
//            D3DCAPSQ d3dCaps;
//	        pd3dDevice->GetDeviceCaps ( &d3dCaps );
//
//	        D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
//	        pd3dDevice->GetCreationParameters ( &d3dCreateParam );
//
//            //	Note : MIXED
//            BOOL bUseMIXED(TRUE);
//	        if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
//	        {
//		        bUseMIXED = FALSE;
//	        }
//
//	        //	Note : Mode Check
//	        if ( bUseMIXED && d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
//	        {
//		        g_emMode = ENRM_VS;
//	        }
//            if ( (g_emMode == ENRM_VS) && d3dCaps.PixelShaderVersion >= D3DVS_VERSION(1, 1) )
//	        {
//		        g_emMode = ENRM_VS_PS_1_1;
//	        }
//            if ( (g_emMode == ENRM_VS_PS_1_1) && d3dCaps.PixelShaderVersion >= D3DVS_VERSION(1, 4) )
//	        {
//		        g_emMode = ENRM_VS_PS_1_4;
//	        }
//        }
//
//	    return hr;
//    }
//
//    void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh )
//    {
//        UINT nIndex(0);
//        BOOL bActiveFX(FALSE);
//        if ( g_pFX )
//        {
//			if ( g_emRenderType == NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP )
//			{
//				switch( g_emMode )
//				{
//				case ENRM_VS:
//					nIndex = 0;
//					bActiveFX = TRUE;
//					break;
//				case ENRM_VS_PS_1_1:
//					nIndex = 1;
//					bActiveFX = TRUE;
//					break;
//				case ENRM_VS_PS_1_4:
//					nIndex = 2;
//					bActiveFX = TRUE;
//					break;
//				}
//			}
//			else if ( g_emRenderType == NSLIGHTMAPDEF::LMRT_LIGHTMAP && g_emMode == ENRM_VS_PS_1_4 )
//			{
//				nIndex = 3;
//				bActiveFX = TRUE;
//			}
//			else if ( g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER && g_emMode == ENRM_VS_PS_1_4 )
//			{
//				nIndex = 4;
//				bActiveFX = TRUE;
//			}
//        }
//
//        if ( bActiveFX )
//        {
//            pd3dDevice->SetFVF( 0 );
//            pd3dDevice->SetVertexDeclaration( g_pDCRT );                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
//
//            UINT numPasses;
//            g_pFX->Begin( &numPasses, 0 );
//            if( nIndex < numPasses )
//            {
//	            g_pFX->BeginPass( nIndex );
//                {
//                    pMesh->DrawSubset( 0 );
//                }
//	            g_pFX->EndPass();
//            }
//            g_pFX->End();
//        }
//        else
//        {
//            pMesh->DrawSubset( 0 );
//        }
//    }
//
//    void FrameMove()
//    {
//        NSLightMapCommonFX::FrameMove( g_pFX );
//    }
//
//    void ShortBlackFog()
//    {
//        NSLightMapCommonFX::ShortBlackFog( g_pFX );
//    }
//
//    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture )
//    {
//        NSLightMapCommonFX::SetBaseTexture( g_pFX, pTexture );
//    }
//
//    void SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight )
//    {
//		NSLightMapCommonFX::SetLightMapTexture( g_pFX, pTextureDay, pTextureNight );
//    }
//
//    void SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset )
//    {
//		NSLightMapCommonFX::SetLightMapUV_Offset( g_pFX, vLightMapUV_Offset );
//    }
//
//    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha )
//    {
//		NSLightMapCommonFX::SetWorld( matWorld );
//    }
//}
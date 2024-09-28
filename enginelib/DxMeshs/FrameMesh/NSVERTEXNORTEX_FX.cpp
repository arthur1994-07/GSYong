#include "pch.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"
#include "../../../InternalCommonLib/dxstdafx.h"

#include "../../DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"

#include "NSVERTEXNORTEX_FX.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

namespace NSVERTEXNORTEX_FX
{
    //----------------------------------------------------------------------------------------------------- BASE_FX
    class BASE_FX
    {
    protected:
        ID3DXEffect*                    m_pFX;
        TSTRING                         m_strFX;
        LPDIRECT3DVERTEXDECLARATION9	m_pDCRT;

    public:
        void CleanUp()
        {
            SAFE_RELEASE( m_pFX );
        }

        HRESULT OnResetDeviceBASE( LPDIRECT3DDEVICEQ pd3dDevice )
        {
            HRESULT hr(S_OK);

            if( m_pFX )	
                V_RETURN( m_pFX->OnResetDevice() );

            return hr;
        }

        HRESULT OnLostDevice()
        {
            HRESULT hr(S_OK);

            if( m_pFX )	
                V_RETURN( m_pFX->OnLostDevice() );

            SAFE_RELEASE( m_pDCRT );

            return hr;
        }

        void FrameMove()
        {
            if ( !m_pFX )
                return;

            DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
            if ( !pLight )
                return;

            const D3DLIGHT9& sLight = pLight->m_Light;

            D3DXVECTOR4 vLightDir4;
            D3DXVECTOR3 vLightDir3;

            D3DXVECTOR4 vLightDiffuse(sLight.Diffuse.r,sLight.Diffuse.g,sLight.Diffuse.b,1.f);
            D3DXVECTOR4 vLightAmbient(sLight.Ambient.r,sLight.Ambient.g,sLight.Ambient.b,1.f);
            vLightDir3.x = -sLight.Direction.x;
            vLightDir3.y = -sLight.Direction.y;
            vLightDir3.z = -sLight.Direction.z;
            D3DXVec3Normalize( &vLightDir3, &vLightDir3 );
            vLightDir4.x = vLightDir3.x;
            vLightDir4.y = vLightDir3.y;
            vLightDir4.z = vLightDir3.z;
            vLightDir4.w = 1.f;

            // Diffuser Color Power 적용
            {
                float fLM_DDPower = (DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower()-1.f)*GLPeriod::GetInstance().GetBlendFact();
                vLightDiffuse += vLightDiffuse*fLM_DDPower;
            }

            m_pFX->SetVector( "g_vLightDiffuse", &vLightDiffuse );
		    m_pFX->SetVector( "g_vLightAmbient", &vLightAmbient );
            m_pFX->SetVector( "g_vLightDir", &vLightDir4 );

            // Light 설정
            {
                // 캐릭터에서는 최대 4개까지 PointLight에 연관된다.
                #define POINT_LIGHT_MAX 7

                D3DXVECTOR4 vPos_Range_PL[ POINT_LIGHT_MAX ];
                D3DXVECTOR4 vDiff_PL[ POINT_LIGHT_MAX ];
                D3DXVECTOR4 vAtt_PL[ POINT_LIGHT_MAX ];
                DWORD dwSize = NSNearPointLight::GetPointLightDataSize();
                if ( dwSize > POINT_LIGHT_MAX )
                    dwSize = POINT_LIGHT_MAX;

                for ( DWORD i=0; i<dwSize && i<POINT_LIGHT_MAX; ++i )
                {
                    const NSNearPointLight::PointLightData* pPointLight = NSNearPointLight::GetPointLightData(i);
                    vDiff_PL[i] = D3DXVECTOR4( pPointLight->m_cDiffuse.r, pPointLight->m_cDiffuse.g, pPointLight->m_cDiffuse.b, 1.f );
                    vPos_Range_PL[i] = D3DXVECTOR4( pPointLight->m_vPosition.x, pPointLight->m_vPosition.y, pPointLight->m_vPosition.z, pPointLight->m_fRange );
                    vAtt_PL[i] = D3DXVECTOR4( pPointLight->m_vAttenuation.x, pPointLight->m_vAttenuation.y, pPointLight->m_vAttenuation.z, 1.f );
                }

                m_pFX->SetInt( "g_nPointLightNum", dwSize );
                m_pFX->SetVectorArray( "g_vPos_Range_PL", &vPos_Range_PL[0], POINT_LIGHT_MAX );
                m_pFX->SetVectorArray( "g_vDiff_PL", &vDiff_PL[0], POINT_LIGHT_MAX );
                m_pFX->SetVectorArray( "g_vAtt_PL", &vAtt_PL[0], POINT_LIGHT_MAX );
            }

			// Fog
			D3DXVECTOR4 vFog;
			if ( g_bWIREFRAME_VIEW )
			{
				vFog = D3DXVECTOR4
				(
					2.f, 
					1.f,
					1.f,
					1.f
				);
			}
			else
			{
				vFog = D3DXVECTOR4
					(
					DxFogMan::GetInstance().GetFogEnd_RealTime(), 
					DxFogMan::GetInstance().GetFogEnd_RealTime() - DxFogMan::GetInstance().GetFogStart_RealTime(),
					1.f,
					1.f
					);
			}
			m_pFX->SetVector( "g_vFOG", &vFog );
        }

        BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD AttribId )
        {
            if ( !m_pFX )
                return FALSE;

            pd3dDevice->SetFVF( 0 );
            pd3dDevice->SetVertexDeclaration( m_pDCRT );

            UINT numPasses;
            m_pFX->Begin( &numPasses, 0 );
            if( 0 < numPasses )
            {
	            m_pFX->BeginPass( 0 );
                {
                    pMesh->DrawSubset( AttribId );
                }
	            m_pFX->EndPass();
            }
            m_pFX->End();

            return TRUE;
        }

        BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                        LPDIRECT3DVERTEXBUFFERQ	pVB,
                        LPDIRECT3DINDEXBUFFERQ pIB,
                        int nFVFSize,
                        DWORD dwVertNum, 
                        DWORD dwFaceNum )
        {
            if ( !m_pFX )
                return FALSE;

            pd3dDevice->SetFVF( 0 );
            pd3dDevice->SetVertexDeclaration( m_pDCRT );

			if ( NSOcclusionQueryFastRender::g_bRendering )
			{
				NSOcclusionQueryFastRender::Render( pd3dDevice, pVB, pIB, nFVFSize, dwVertNum, dwFaceNum );
			}
			else
			{
				UINT numPasses;
				m_pFX->Begin( &numPasses, 0 );
				if( 0 < numPasses )
				{
					m_pFX->BeginPass( 0 );
					{
						pd3dDevice->SetFVF( 0L );
						pd3dDevice->SetStreamSource( 0, pVB, 0, nFVFSize );
						pd3dDevice->SetIndices( pIB );
						pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, dwVertNum, 0, dwFaceNum );
					}
					m_pFX->EndPass();
				}
				m_pFX->End();
			}

            return TRUE;
        }

        BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                    UINT PrimitiveCount,
                    CONST void* pVertexStreamZeroData,
                    UINT VertexStreamZeroStride )
        {
            if ( !m_pFX )
                return FALSE;

            pd3dDevice->SetFVF( 0 );
            pd3dDevice->SetVertexDeclaration( m_pDCRT );

            UINT numPasses;
            m_pFX->Begin( &numPasses, 0 );
            if( 0 < numPasses )
            {
	            m_pFX->BeginPass( 0 );
                {
                    pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
                }
	            m_pFX->EndPass();
            }
            m_pFX->End();

            return TRUE;
        }

        void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture )
        {
            if ( !m_pFX )
                return;

            m_pFX->SetTexture( "BaseTex", pTexture );
        }

        void SetWorld( const D3DXMATRIX& matWorld )
        {
            if ( !m_pFX )
                return;

            const D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView();
            const D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj();

            D3DXMATRIX matWV, matWVP;
	        D3DXMatrixMultiply( &matWV, &matWorld, &matView );
	        D3DXMatrixMultiply( &matWVP, &matWV, &matProj );
            m_pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
            m_pFX->SetMatrixTranspose( "g_matWorld", &matWorld );
        }

        void SetAlpha( float fAlpha )
        {
            if ( !m_pFX )
                return;

            m_pFX->SetFloat( "g_fAlpha", fAlpha );
        }

    public:
        virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
        virtual void SetFX( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

    public:
        BASE_FX()
            : m_pFX(NULL)
            , m_strFX(_T(""))
            , m_pDCRT(NULL)
        {
        }
        virtual ~BASE_FX() {}
    };

    //----------------------------------------------------------------------------------------------------- VERTEXNORTEX_FX
    class VERTEXNORTEX_FX : public BASE_FX
    {
    public:
        virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
        {
            const D3DVERTEXELEMENT9 VertexDecl[] =
            {
		        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        		        
                D3DDECL_END()
            };

	        pd3dDevice->CreateVertexDeclaration( VertexDecl,  &m_pDCRT );

            return BASE_FX::OnResetDeviceBASE( pd3dDevice );
        }

        virtual void SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
        {
            m_strFX = 
            "//-------------------------------------------------------------------------------------- \r\n"
            "// File: FrameMesh.fx \r\n"
            "//       shhan \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            " \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "// Global variables \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "float4x4 g_matWorld; \r\n"
            "float4x4 g_matWVP; \r\n"
            "float4   g_vLightDiffuse; \r\n"
            "float3   g_vLightAmbient; \r\n"
            "float3   g_vLightDir; \r\n"
            "float2   g_vFOG;	// FogEnd, FogDist \r\n"
            "int      g_nPointLightNum; \r\n"
            "float4   g_vPos_Range_PL[7]; \r\n"
            "float4   g_vDiff_PL[7]; \r\n"
            "float4   g_vAtt_PL[7]; \r\n"
            "float    g_fAlpha; \r\n"
            " \r\n"
            "texture BaseTex; \r\n"
            " \r\n"
            "sampler BaseTexSampler = sampler_state  \r\n"
            "{ \r\n"
            "    Texture = (BaseTex); \r\n"
            "}; \r\n"
            "  \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "// Basic vertex transformation \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "struct VS_INPUT { \r\n"
            "    float4 m_vPosition  : POSITION; \r\n"
            "    float3 m_vNormal    : NORMAL; \r\n"
            "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
            "}; \r\n"
            " \r\n"
            "struct VS_OUTPUT  \r\n"
            "{ \r\n"
            "    float4 m_vPosition  : POSITION; \r\n"
            "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
            "    float3 m_vNormal    : TEXCOORD1; \r\n"
            "    float3 m_vColor     : TEXCOORD2; \r\n"
            "    float  m_fFog       : FOG; \r\n"
            "}; \r\n"
            " \r\n"

            /*
            //---------------------------------------------------------------------
        // Point Light #1 추가
        float3 afPosView = mul(g_matWorldView, In.Position);
        float3 afPosDir = g_vPL0_Pos_Range.xyz - afPosView;
        float fLength = length( afPosDir );

        // 내적 구함.
        afPosDir = normalize( afPosDir );
        float fDotPL = saturate( dot( afPosDir, Normal ) );

        // 선형 감쇠, 2차 감쇠, 지수 감쇠 적용
        float fAttenuationSub = g_vPL0_Att.x;
        fAttenuationSub += g_vPL0_Att.y * fLength;
        fAttenuationSub += g_vPL0_Att.z * fLength * fLength;
        // 최종 감쇠값
        float fAttenuation = 1.f / fAttenuationSub;
        // 거리 체크
        float fEnableLength = step(fLength, g_vPL0_Pos_Range.w);
        // 마지막
        Out.Color.xyz += g_vPL0_Diff.xyz * fAttenuation * fDotPL * fEnableLength;
            */
            "//---------------------------------------------------------------------------------------------------------- \r\n"
            "//						V	S \r\n"
            "VS_OUTPUT VS( VS_INPUT In )  \r\n"
            "{ \r\n"
            "    VS_OUTPUT Out; \r\n"
            "     \r\n"
            "    // Now that we have the calculated weight, add in the final influence \r\n"
            "    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
            "    Out.m_vNormal = normalize( mul(g_matWorld, In.m_vNormal) ); \r\n"
            " \r\n"
            "    // Pass the texture coordinate \r\n"
            "    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
            " \r\n"
            "    float3 vPos = mul(g_matWorld, In.m_vPosition); \r\n"
            "    float3 vPointColor = 0.f; \r\n"
            "    for ( int i=0; i<g_nPointLightNum; ++i ) \r\n"
            "    { \r\n"
            "        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
            "        float fLength = length( afPosDir ); \r\n"
            "        // 내적 구함. \r\n"
            "        afPosDir = normalize( afPosDir ); \r\n"
            "        float fDotPL = saturate( dot( afPosDir, Out.m_vNormal ) ); \r\n"
            "        // 선형 감쇠, 2차 감쇠, 지수 감쇠 적용 \r\n"
            "        float fAttenuationSub = g_vAtt_PL[i].x; \r\n"
            "        fAttenuationSub += g_vAtt_PL[i].y * fLength; \r\n"
            "        fAttenuationSub += g_vAtt_PL[i].z * fLength * fLength; \r\n"
            "        // 최종 감쇠값 \r\n"
            "        float fAttenuation = 1.f / fAttenuationSub; \r\n"
            "        // 거리 체크 \r\n"
            "        float fEnableLength = step(fLength, g_vPos_Range_PL[i].w); \r\n"
            "        // 마지막 \r\n"
            "        vPointColor += g_vDiff_PL[i].xyz * fAttenuation * fDotPL * fEnableLength; \r\n"
            "    } \r\n"
            "    Out.m_vColor.xyz = vPointColor; \r\n"
            " \r\n"
            "    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
            "    \r\n"
            "    return Out; \r\n"
            "} \r\n"
            " \r\n"
            "//---------------------------------------------------------------------------------------------------------- \r\n"
            "//					P	S \r\n"
            "// 원본 \r\n"
            "float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
            "{ \r\n"
            "    // Texture \r\n"
            "    float4 vColor = tex2D( BaseTexSampler, In.m_vTexCoord0 ); \r\n"
            " \r\n"
            "    // 내적 계산 \r\n"
            "    float fDiffuseDot = saturate( dot( g_vLightDir, normalize( In.m_vNormal ) ) ); \r\n"
            "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot; \r\n"
            " \r\n"
            "    // Ambient + Specular \r\n"
            "    float3 vColorTemp = (g_vLightAmbient) + vDiffuse; // Diffuse + Ambient\r\n"
            "    vColorTemp += In.m_vColor.xyz;                    // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
            "    vColorTemp = min( vColorTemp, 2.f );              // 최대값을 2로 제한한다. 현재 LightMap OverLighting 강도가 2배임 \r\n"
            "    vColor.xyz *= vColorTemp;                         // Texture * (Diff+Ambient+Point) \r\n"
            "    vColor.w *= g_fAlpha;                             // Picking 시 알파값이 변경될 수 있다. \r\n"
            " \r\n"
            "    return vColor; \r\n"
            "} \r\n"
            " \r\n"
            "technique Base \r\n"
            "{ \r\n"
            "    //------------------------------------------------------------------------------------- \r\n"
            "    // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
            "    //          - PS_2_0 이상 지원 \r\n"
            "    pass p0 \r\n"
            "    { \r\n"
            "        VertexShader = compile vs_2_0 VS(); \r\n"
            "        PixelShader = compile ps_2_0 PS(); \r\n"
            "    } \r\n"
            "} \r\n";

            UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
            D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
        }

    public:
        VERTEXNORTEX_FX()
            : BASE_FX()
        {
        }
        virtual ~VERTEXNORTEX_FX() {}
    };

    //----------------------------------------------------------------------------------------------------- VERTEXNORCOLORTEX_FX
    class VERTEXNORCOLORTEX_FX : public BASE_FX
    {
    public:
        virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
        {
            const D3DVERTEXELEMENT9 VertexDecl[] =
            {
		        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
                { 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		        { 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        		        
                D3DDECL_END()
            };

	        pd3dDevice->CreateVertexDeclaration( VertexDecl,  &m_pDCRT );

            return BASE_FX::OnResetDeviceBASE( pd3dDevice );
        }

        virtual void SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
        {
            m_strFX = 
            "//-------------------------------------------------------------------------------------- \r\n"
            "// File: FrameMesh.fx \r\n"
            "//       shhan \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            " \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "// Global variables \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "float4x4 g_matWorld; \r\n"
            "float4x4 g_matWVP; \r\n"
            "float4   g_vLightDiffuse; \r\n"
            "float3   g_vLightAmbient; \r\n"
            "float3   g_vLightDir; \r\n"
            "float2   g_vFOG;	// FogEnd, FogDist \r\n"
            "int      g_nPointLightNum; \r\n"
            "float4   g_vPos_Range_PL[7]; \r\n"
            "float4   g_vDiff_PL[7]; \r\n"
            "float4   g_vAtt_PL[7]; \r\n"
            "float    g_fAlpha; \r\n"
            " \r\n"
            "texture BaseTex; \r\n"
            " \r\n"
            "sampler BaseTexSampler = sampler_state  \r\n"
            "{ \r\n"
            "    Texture = (BaseTex); \r\n"
            "}; \r\n"
            "  \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "// Basic vertex transformation \r\n"
            "//-------------------------------------------------------------------------------------- \r\n"
            "struct VS_INPUT { \r\n"
            "    float4 m_vPosition  : POSITION; \r\n"
            "    float3 m_vNormal    : NORMAL; \r\n"
            "    float4 m_vColor     : COLOR0; \r\n"
            "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
            "}; \r\n"
            " \r\n"
            "struct VS_OUTPUT  \r\n"
            "{ \r\n"
            "    float4 m_vPosition  : POSITION; \r\n"
            "    float4 m_vColor1    : COLOR0; \r\n"
            "    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
            "    float3 m_vNormal    : TEXCOORD1; \r\n"
            "    float3 m_vColor     : TEXCOORD2; \r\n"
            "    float  m_fFog       : FOG; \r\n"
            "}; \r\n"
            " \r\n"
            "//---------------------------------------------------------------------------------------------------------- \r\n"
            "//						V	S \r\n"
            "VS_OUTPUT VS( VS_INPUT In )  \r\n"
            "{ \r\n"
            "    VS_OUTPUT Out; \r\n"
            "     \r\n"
            "    // Now that we have the calculated weight, add in the final influence \r\n"
            "    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
            "    Out.m_vNormal = normalize( mul(g_matWorld, In.m_vNormal) ); \r\n"
            " \r\n"
            "    // Pass the texture coordinate \r\n"
            "    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
            " \r\n"
            "    float3 vPos = mul(g_matWorld, In.m_vPosition); \r\n"
            "    float3 vPointColor = 0.f; \r\n"
            "    for ( int i=0; i<g_nPointLightNum; ++i ) \r\n"
            "    { \r\n"
            "        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
            "        float fLength = length( afPosDir ); \r\n"
            "        // 내적 구함. \r\n"
            "        afPosDir = normalize( afPosDir ); \r\n"
            "        float fDotPL = saturate( dot( afPosDir, Out.m_vNormal ) ); \r\n"
            "        // 선형 감쇠, 2차 감쇠, 지수 감쇠 적용 \r\n"
            "        float fAttenuationSub = g_vAtt_PL[i].x; \r\n"
            "        fAttenuationSub += g_vAtt_PL[i].y * fLength; \r\n"
            "        fAttenuationSub += g_vAtt_PL[i].z * fLength * fLength; \r\n"
            "        // 최종 감쇠값 \r\n"
            "        float fAttenuation = 1.f / fAttenuationSub; \r\n"
            "        // 거리 체크 \r\n"
            "        float fEnableLength = step(fLength, g_vPos_Range_PL[i].w); \r\n"
            "        // 마지막 \r\n"
            "        vPointColor += g_vDiff_PL[i].xyz * fAttenuation * fDotPL * fEnableLength; \r\n"
            "    } \r\n"
            "    Out.m_vColor.xyz = vPointColor; \r\n"
            "    Out.m_vColor1 = In.m_vColor; \r\n"
            " \r\n"
            "    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
            "    \r\n"
            "    return Out; \r\n"
            "} \r\n"
            " \r\n"
            "//---------------------------------------------------------------------------------------------------------- \r\n"
            "//					P	S \r\n"
            "// 원본 \r\n"
            "float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
            "{ \r\n"
            "    // Texture \r\n"
            "    float4 vColor = tex2D( BaseTexSampler, In.m_vTexCoord0 ); \r\n"
            "    vColor *= In.m_vColor1; \r\n"
            " \r\n"
            "    // 내적 계산 \r\n"
            "    float fDiffuseDot = saturate( dot( g_vLightDir, normalize( In.m_vNormal ) ) ); \r\n"
            "    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot; \r\n"
            " \r\n"
            "    // Ambient + Specular \r\n"
            "    float3 vColorTemp = (g_vLightAmbient) + vDiffuse; // Diffuse + Ambient\r\n"
            "    vColorTemp += In.m_vColor.xyz;                    // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
            "    vColorTemp = min( vColorTemp, 2.f );              // 최대값을 2로 제한한다. 현재 LightMap OverLighting 강도가 2배임 \r\n"
            "    vColor.xyz *= vColorTemp;                         // Texture * (Diff+Ambient+Point) \r\n"
            "    vColor.w *= In.m_vColor1.w * g_fAlpha;            // Color Alpha \r\n"
            " \r\n"
            "    return vColor; \r\n"
            "} \r\n"
            " \r\n"
            "technique Base \r\n"
            "{ \r\n"
            "    //------------------------------------------------------------------------------------- \r\n"
            "    // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
            "    //          - PS_2_0 이상 지원 \r\n"
            "    pass p0 \r\n"
            "    { \r\n"
            "        VertexShader = compile vs_2_0 VS(); \r\n"
            "        PixelShader = compile ps_2_0 PS(); \r\n"
            "    } \r\n"
            "} \r\n";

            UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
            D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
        }

    public:
        VERTEXNORCOLORTEX_FX()
            : BASE_FX()
        {
        }
        virtual ~VERTEXNORCOLORTEX_FX() {}
    };

	//----------------------------------------------------------------------------------------------------- VERTEXNORCOLORTEX_FX
	class VERTEXNORCOLORTEX2_FX : public BASE_FX
	{
	public:
		virtual HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
		{
			const D3DVERTEXELEMENT9 VertexDecl[] =
			{
				{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
				{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				{ 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },

				D3DDECL_END()
			};

			pd3dDevice->CreateVertexDeclaration( VertexDecl,  &m_pDCRT );

			return BASE_FX::OnResetDeviceBASE( pd3dDevice );
		}

		virtual void SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
		{
			m_strFX = 
				"//-------------------------------------------------------------------------------------- \r\n"
				"// File: FrameMesh.fx \r\n"
				"//       shhan \r\n"
				"//-------------------------------------------------------------------------------------- \r\n"
				" \r\n"
				"//-------------------------------------------------------------------------------------- \r\n"
				"// Global variables \r\n"
				"//-------------------------------------------------------------------------------------- \r\n"
				"float4x4 g_matWorld; \r\n"
				"float4x4 g_matWVP; \r\n"
				"float4   g_vLightDiffuse; \r\n"
				"float3   g_vLightAmbient; \r\n"
				"float3   g_vLightDir; \r\n"
				"float2   g_vFOG;	// FogEnd, FogDist \r\n"
				"int      g_nPointLightNum; \r\n"
				"float4   g_vPos_Range_PL[7]; \r\n"
				"float4   g_vDiff_PL[7]; \r\n"
				"float4   g_vAtt_PL[7]; \r\n"
				"float    g_fAlpha; \r\n"
				" \r\n"
				"texture BaseTex; \r\n"
				" \r\n"
				"sampler BaseTexSampler = sampler_state  \r\n"
				"{ \r\n"
				"    Texture = (BaseTex); \r\n"
				"}; \r\n"
				"  \r\n"
				"//-------------------------------------------------------------------------------------- \r\n"
				"// Basic vertex transformation \r\n"
				"//-------------------------------------------------------------------------------------- \r\n"
				"struct VS_INPUT { \r\n"
				"    float4 m_vPosition  : POSITION; \r\n"
				"    float3 m_vNormal    : NORMAL; \r\n"
				"    float4 m_vColor     : COLOR0; \r\n"
				"    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
				"    float2 m_vTexCoord1 : TEXCOORD1; \r\n"
				"}; \r\n"
				" \r\n"
				"struct VS_OUTPUT  \r\n"
				"{ \r\n"
				"    float4 m_vPosition  : POSITION; \r\n"
				"    float4 m_vColor1    : COLOR0; \r\n"
				"    float2 m_vTexCoord0 : TEXCOORD0; \r\n"
				"    float3 m_vNormal    : TEXCOORD1; \r\n"
				"    float3 m_vColor     : TEXCOORD2; \r\n"
				"    float  m_fFog       : FOG; \r\n"
				"}; \r\n"
				" \r\n"
				"//---------------------------------------------------------------------------------------------------------- \r\n"
				"//						V	S \r\n"
				"VS_OUTPUT VS( VS_INPUT In )  \r\n"
				"{ \r\n"
				"    VS_OUTPUT Out; \r\n"
				"     \r\n"
				"    // Now that we have the calculated weight, add in the final influence \r\n"
				"    Out.m_vPosition = mul(g_matWVP, In.m_vPosition); \r\n"
				"    Out.m_vNormal = normalize( mul(g_matWorld, In.m_vNormal) ); \r\n"
				" \r\n"
				"    // Pass the texture coordinate \r\n"
				"    Out.m_vTexCoord0 = In.m_vTexCoord0; \r\n"
				" \r\n"
				"    float3 vPos = mul(g_matWorld, In.m_vPosition); \r\n"
				"    float3 vPointColor = 0.f; \r\n"
				"    for ( int i=0; i<g_nPointLightNum; ++i ) \r\n"
				"    { \r\n"
				"        float3 afPosDir = g_vPos_Range_PL[i].xyz - vPos; \r\n"
				"        float fLength = length( afPosDir ); \r\n"
				"        // 내적 구함. \r\n"
				"        afPosDir = normalize( afPosDir ); \r\n"
				"        float fDotPL = saturate( dot( afPosDir, Out.m_vNormal ) ); \r\n"
				"        // 선형 감쇠, 2차 감쇠, 지수 감쇠 적용 \r\n"
				"        float fAttenuationSub = g_vAtt_PL[i].x; \r\n"
				"        fAttenuationSub += g_vAtt_PL[i].y * fLength; \r\n"
				"        fAttenuationSub += g_vAtt_PL[i].z * fLength * fLength; \r\n"
				"        // 최종 감쇠값 \r\n"
				"        float fAttenuation = 1.f / fAttenuationSub; \r\n"
				"        // 거리 체크 \r\n"
				"        float fEnableLength = step(fLength, g_vPos_Range_PL[i].w); \r\n"
				"        // 마지막 \r\n"
				"        vPointColor += g_vDiff_PL[i].xyz * fAttenuation * fDotPL * fEnableLength; \r\n"
				"    } \r\n"
				"    Out.m_vColor.xyz = vPointColor; \r\n"
				"    Out.m_vColor1 = In.m_vColor; \r\n"
				" \r\n"
				"    Out.m_fFog = saturate((g_vFOG.x - Out.m_vPosition.z) / g_vFOG.y); \r\n"
				"    \r\n"
				"    return Out; \r\n"
				"} \r\n"
				" \r\n"
				"//---------------------------------------------------------------------------------------------------------- \r\n"
				"//					P	S \r\n"
				"// 원본 \r\n"
				"float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
				"{ \r\n"
				"    // Texture \r\n"
				"    float4 vColor = tex2D( BaseTexSampler, In.m_vTexCoord0 ); \r\n"
				"    vColor *= In.m_vColor1; \r\n"
				" \r\n"
				"    // 내적 계산 \r\n"
				"    float fDiffuseDot = saturate( dot( g_vLightDir, normalize( In.m_vNormal ) ) ); \r\n"
				"    float3 vDiffuse = g_vLightDiffuse * fDiffuseDot; \r\n"
				" \r\n"
				"    // Ambient + Specular \r\n"
				"    float3 vColorTemp = (g_vLightAmbient) + vDiffuse; // Diffuse + Ambient\r\n"
				"    vColorTemp += In.m_vColor.xyz;                    // (Diff+Ambient+Point) PointLight 색 추가 \r\n"
				"    vColorTemp = min( vColorTemp, 2.f );              // 최대값을 2로 제한한다. 현재 LightMap OverLighting 강도가 2배임 \r\n"
				"    vColor.xyz *= vColorTemp;                         // Texture * (Diff+Ambient+Point) \r\n"
				"    vColor.w *= In.m_vColor1.w * g_fAlpha;            // Color Alpha \r\n"
				" \r\n"
				"    return vColor; \r\n"
				"} \r\n"
				" \r\n"
				"technique Base \r\n"
				"{ \r\n"
				"    //------------------------------------------------------------------------------------- \r\n"
				"    // 기본 _ 0 - VS_1_1 이상 지원 \r\n"
				"    //          - PS_2_0 이상 지원 \r\n"
				"    pass p0 \r\n"
				"    { \r\n"
				"        VertexShader = compile vs_2_0 VS(); \r\n"
				"        PixelShader = compile ps_2_0 PS(); \r\n"
				"    } \r\n"
				"} \r\n";

			UINT dwBufferSize = static_cast<UINT>( m_strFX.size() ) + 1; 
			D3DXCreateEffect( pd3dDevice, m_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
		}

	public:
		VERTEXNORCOLORTEX2_FX()
			: BASE_FX()
		{
		}
		virtual ~VERTEXNORCOLORTEX2_FX() {}
	};


    //----------------------------------------------------------------------------------------------------- MAIN
    BOOL                    g_bActive(FALSE);
    VERTEXNORTEX_FX         g_sVERTEXNORTEX_FX;
    VERTEXNORCOLORTEX_FX    g_sVERTEXNORCOLORTEX_FX;
	VERTEXNORCOLORTEX2_FX   g_sVERTEXNORCOLORTEX2_FX;

    enum RENDER_MODE
    {
        ENRM_ZERO       = 0,
        ENRM_VS_PS_2_0  = 1,
    };
    RENDER_MODE                     g_emMode(ENRM_ZERO);
    

    void CleanUp()
    {
        g_sVERTEXNORTEX_FX.CleanUp();
        g_sVERTEXNORCOLORTEX_FX.CleanUp();
		g_sVERTEXNORCOLORTEX2_FX.CleanUp();
    }

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        g_sVERTEXNORTEX_FX.OnResetDevice( pd3dDevice );
        g_sVERTEXNORCOLORTEX_FX.OnResetDevice( pd3dDevice );
		g_sVERTEXNORCOLORTEX2_FX.OnResetDevice( pd3dDevice );
        return S_OK;
    }

    HRESULT OnLostDevice()
    {
        g_sVERTEXNORTEX_FX.OnLostDevice();
        g_sVERTEXNORCOLORTEX_FX.OnLostDevice();
		g_sVERTEXNORCOLORTEX2_FX.OnLostDevice();
        return S_OK;
    }

    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        HRESULT hr(S_OK);

        CleanUp();

        g_sVERTEXNORTEX_FX.SetFX( pd3dDevice );
        g_sVERTEXNORCOLORTEX_FX.SetFX( pd3dDevice );
		g_sVERTEXNORCOLORTEX2_FX.SetFX( pd3dDevice );

        {
            g_emMode = ENRM_ZERO;

            D3DCAPSQ d3dCaps;
	        pd3dDevice->GetDeviceCaps ( &d3dCaps );

	        D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	        pd3dDevice->GetCreationParameters ( &d3dCreateParam );

            //	Note : MIXED
            BOOL bUseMIXED(TRUE);
	        if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	        {
		        bUseMIXED = FALSE;
	        }

	        //	Note : Mode Check
	        if ( bUseMIXED && d3dCaps.VertexShaderVersion >= D3DVS_VERSION(2, 0) && d3dCaps.PixelShaderVersion >= D3DVS_VERSION(2, 0) )
	        {
		        g_emMode = ENRM_VS_PS_2_0;
	        }
        }

	    return hr;
    }

    void SetEnable()
    {
        g_bActive = TRUE;
    }

    void SetDisable()
    {
        g_bActive = FALSE;
    }

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, DWORD dwMatrial )
    {
        BOOL bRenderOK(FALSE);
        if ( g_emMode == ENRM_VS_PS_2_0 && g_bActive )
        {
            switch ( pMesh->GetFVF() )
            {
            case VERTEX::FVF:
                bRenderOK = g_sVERTEXNORTEX_FX.Render( pd3dDevice, pMesh, dwMatrial );
                break;

            case VERTEXNORCOLORTEX::FVF:
                bRenderOK = g_sVERTEXNORCOLORTEX_FX.Render( pd3dDevice, pMesh, dwMatrial );
                break;

			case VERTEXNORCOLORTEX2::FVF:
				bRenderOK = g_sVERTEXNORCOLORTEX2_FX.Render( pd3dDevice, pMesh, dwMatrial );
				break;
            }
        }
        
        if ( !bRenderOK )
        {
            pMesh->DrawSubset( dwMatrial );
        }
    }

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                    LPDIRECT3DVERTEXBUFFERQ	pVB,
                    LPDIRECT3DINDEXBUFFERQ pIB,
                    int nFVFSize,
                    DWORD dwFVF,
                    DWORD dwVertNum, 
                    DWORD dwFaceNum )
    {
		if ( !pVB )
			return;

		if ( !pIB )
			return;

        BOOL bRenderOK(FALSE);
        if ( ( g_emMode == ENRM_VS_PS_2_0 && g_bActive ) || NSOcclusionQueryFastRender::g_bRendering )
        {
            switch ( dwFVF )
            {
            case VERTEX::FVF:
                bRenderOK = g_sVERTEXNORTEX_FX.Render( pd3dDevice, pVB, pIB, nFVFSize, dwVertNum, dwFaceNum);
                break;

            case VERTEXNORCOLORTEX::FVF:
                bRenderOK = g_sVERTEXNORCOLORTEX_FX.Render( pd3dDevice, pVB, pIB, nFVFSize, dwVertNum, dwFaceNum );
                break;

			case VERTEXNORCOLORTEX2::FVF:
				bRenderOK = g_sVERTEXNORCOLORTEX2_FX.Render( pd3dDevice, pVB, pIB, nFVFSize, dwVertNum, dwFaceNum );
				break;
            }
        }
        
        if ( !bRenderOK )
        {
			HRESULT hr = pd3dDevice->SetFVF( dwFVF );
			if ( hr != S_OK )
				return;

			hr = pd3dDevice->SetStreamSource( 0, pVB, 0, nFVFSize );
			if ( hr != S_OK )
				return;

			hr = pd3dDevice->SetIndices( pIB );
			if ( hr != S_OK )
				return;

			hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, dwVertNum, 0, dwFaceNum );
			if ( hr != S_OK )
				return;
		}
    }

    void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                DWORD dwFVF,
                UINT PrimitiveCount,
                CONST void* pVertexStreamZeroData,
                UINT VertexStreamZeroStride )
    {
        BOOL bRenderOK(FALSE);
        if ( g_emMode == ENRM_VS_PS_2_0 && g_bActive )
        {
            switch ( dwFVF )
            {
            case VERTEX::FVF:
                bRenderOK = g_sVERTEXNORTEX_FX.Render( pd3dDevice, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
                break;

            case VERTEXNORCOLORTEX::FVF:
                bRenderOK = g_sVERTEXNORCOLORTEX_FX.Render( pd3dDevice, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
                break;

			case VERTEXNORCOLORTEX2::FVF:
				bRenderOK = g_sVERTEXNORCOLORTEX2_FX.Render( pd3dDevice, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
				break;
            }
        }
        
        if ( !bRenderOK )
        {
            pd3dDevice->SetFVF( dwFVF );
	        pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
        }
    }

    void FrameMove()
    {
        g_sVERTEXNORTEX_FX.FrameMove();
        g_sVERTEXNORCOLORTEX_FX.FrameMove();
		g_sVERTEXNORCOLORTEX2_FX.FrameMove();
    }

    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture )
    {
        if ( !g_bActive )
            return;

        g_sVERTEXNORTEX_FX.SetBaseTexture( pTexture );
        g_sVERTEXNORCOLORTEX_FX.SetBaseTexture( pTexture );
		g_sVERTEXNORCOLORTEX2_FX.SetBaseTexture( pTexture );
    }

    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha )
    {
        if ( !g_bActive )
            return;

        g_sVERTEXNORTEX_FX.SetWorld( matWorld );
        g_sVERTEXNORCOLORTEX_FX.SetWorld( matWorld );
		g_sVERTEXNORCOLORTEX2_FX.SetWorld( matWorld );

        g_sVERTEXNORTEX_FX.SetAlpha( fAlpha );
        g_sVERTEXNORCOLORTEX_FX.SetAlpha( fAlpha );
		g_sVERTEXNORCOLORTEX2_FX.SetAlpha( fAlpha );
    }
}
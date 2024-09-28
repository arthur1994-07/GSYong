#include "pch.h"

#include "NSOcclusionQueryFastRender.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSOcclusionQueryFastRender
{
	BOOL g_bRendering = FALSE;		// 현재 OcclusionQuery 렌더링 중인가~?

	TSTRING					g_strFX;
	ID3DXEffect*            g_pFX(NULL);
	D3DXMATRIX				g_matVP;

	void CleanUp()
	{
		SAFE_RELEASE( g_pFX );
	}

	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);

		if( g_pFX )	
			V_RETURN( g_pFX->OnResetDevice() );

		return hr;
	}

	HRESULT OnLostDevice()
	{
		HRESULT hr(S_OK);

		if( g_pFX )	
			V_RETURN( g_pFX->OnLostDevice() );

		return hr;
	}

	void SetFX( LPDIRECT3DDEVICEQ pd3dDevice )
    {
        g_strFX = 
        "//-------------------------------------------------------------------------------------- \r\n"
        "// File: FrameMesh.fx \r\n"
        "//       shhan \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        " \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "// Global variables \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "float4x4 g_matWVP; \r\n"
        " \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "// Basic vertex transformation \r\n"
        "//-------------------------------------------------------------------------------------- \r\n"
        "struct VS_INPUT { \r\n"
        "    float4 m_vPosition  : POSITION; \r\n"
        "}; \r\n"
        " \r\n"
        "struct VS_OUTPUT  \r\n"
        "{ \r\n"
        "    float4 m_vPosition  : POSITION; \r\n"
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
        " \r\n"
        "    return Out; \r\n"
        "} \r\n"
        " \r\n"
        "//---------------------------------------------------------------------------------------------------------- \r\n"
        "//					P	S \r\n"
        "// 원본 \r\n"
        "float4 PS( VS_OUTPUT In ) : COLOR0  \r\n"
        "{ \r\n"
        "    // Texture \r\n"
        "    float4 vColor = 1.f; \r\n"
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
        "        VertexShader = compile vs_1_1 VS(); \r\n"
        "        PixelShader = compile ps_1_1 PS(); \r\n"
        "    } \r\n"
        "} \r\n";

        UINT dwBufferSize = static_cast<UINT>( g_strFX.size() ) + 1; 
        D3DXCreateEffect( pd3dDevice, g_strFX.c_str(), dwBufferSize, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &g_pFX, NULL );
    }

	HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		CleanUp();
		SetFX( pd3dDevice );

		return S_OK;
	}

	void SetWorld( const D3DXMATRIX& matWorld )
	{
		D3DXMATRIX matWVP;
		D3DXMatrixMultiply( &matWVP, &matWorld, &g_matVP );
		g_pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
	}

	void SetVP( const D3DXMATRIX& matVP )
	{
		g_matVP = matVP;
	}

	void SetWVP( const D3DXMATRIX& matWVP )
	{
		g_pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
	}

	void Render( LPD3DXMESH pMesh )
	{
		if ( !g_pFX )
			return;

		DWORD dwPassIndex(0);
		UINT numPasses;
		g_pFX->Begin( &numPasses, 0 );
		if( dwPassIndex < numPasses )
		{
			g_pFX->BeginPass( dwPassIndex );
			{
				pMesh->DrawSubset( 0 );
			}
			g_pFX->EndPass();
		}
		g_pFX->End();
	}

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				LPDIRECT3DVERTEXBUFFERQ	pVB,
				LPDIRECT3DINDEXBUFFERQ pIB,
				int nFVFSize,
				DWORD dwVertNum, 
				DWORD dwFaceNum )
	{
		if ( !g_pFX )
			return;

		pd3dDevice->SetTexture( 0, NULL );
		pd3dDevice->SetFVF( 0L );
		pd3dDevice->SetStreamSource( 0, pVB, 0, nFVFSize );
		pd3dDevice->SetIndices( pIB );

		DWORD dwPassIndex(0);
		UINT numPasses;
		g_pFX->Begin( &numPasses, 0 );
		if( dwPassIndex < numPasses )
		{
			g_pFX->BeginPass( dwPassIndex );
			{
				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, dwVertNum, 0, dwFaceNum );
			}
			g_pFX->EndPass();
		}
		g_pFX->End();
	}
}
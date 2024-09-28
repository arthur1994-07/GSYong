#include "pch.h"

#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/TextureManager.h"
#include "../DxLoadShader.h"

#include "DxEffProjTextureShader.h"



LPDIRECT3DSTATEBLOCK9 DxEffProjTextureShader::g_pSavedRS_STATIC(NULL);
LPDIRECT3DSTATEBLOCK9 DxEffProjTextureShader::g_pDrawRS_STATIC(NULL);
std::map<TSTRING,ID3DXEffect*>	DxEffProjTextureShader::g_mapCustomFX;



DxEffProjTextureShader::DxEffProjTextureShader( const TCHAR* szName, const TCHAR* szShader, float fCreatingTime, float fDestroyingTime, BOOL bEnableON )
	: DxEffProjBase( bEnableON, fCreatingTime, fDestroyingTime )
	, m_strTexName(szName)
	, m_strShader(szShader)
	, m_rFX(NULL)
{
}

DxEffProjTextureShader::~DxEffProjTextureShader()
{
}



//////////////////////////////////////////////////////////////////////////
//								STATIC
void DxEffProjTextureShader::OnResetDevice_STATIC( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( g_pSavedRS_STATIC );
	SAFE_RELEASE( g_pDrawRS_STATIC );

	// Create the state blocks for rendering text
	for( int which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );
		//pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
		//pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &g_pSavedRS_STATIC );
		else			pd3dDevice->EndStateBlock( &g_pDrawRS_STATIC );
	}
}

void DxEffProjTextureShader::OnLostDevice_STATIC()
{
	SAFE_RELEASE( g_pSavedRS_STATIC );
	SAFE_RELEASE( g_pDrawRS_STATIC );

	DeleteCustomFX_STATIC();
}

void DxEffProjTextureShader::DeleteCustomFX_STATIC()
{
	std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.begin();
	for ( ; iter!=g_mapCustomFX.end(); ++iter )
	{
		SAFE_RELEASE( (*iter).second );
	}
	g_mapCustomFX.clear();
}
//								STATIC
//////////////////////////////////////////////////////////////////////////



BOOL DxEffProjTextureShader::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffProjBase::OnCreateDevice( pd3dDevice );

	if ( !m_strTexName.empty() )
	{
		m_textureRes = TextureManager::GetInstance().LoadTexture
		(
			m_strTexName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			1,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT,
			true,
			FALSE 
		);
	}

	return TRUE;
}

void DxEffProjTextureShader::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffProjBase::OnResetDevice( pd3dDevice );

	if ( !m_strShader.empty() )
	{
		std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.find( m_strShader );
		if ( iter != g_mapCustomFX.end() )
		{
			m_rFX = (*iter).second;
			return;
		}

		char szPathName[MAX_PATH] = _T("");
		StringCchCopy( szPathName, MAX_PATH, DXShaderMan::GetInstance().GetPath() );
		StringCchCat( szPathName, MAX_PATH, m_strShader.c_str() );

		ID3DXEffect* pFX(NULL);
		HRESULT hr = D3DXCreateEffectFromFile( pd3dDevice, szPathName, NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );
		if( FAILED( hr ) )
		{
			CDebugSet::ToLogFile( sc::string::format( "Error : %1%", szPathName ) );
			return;
		}

		m_rFX = pFX;
		g_mapCustomFX.insert( std::make_pair( m_strShader, pFX ) );
	}
}

void DxEffProjTextureShader::OnLostDevice()
{
	m_rFX = NULL;
}

void DxEffProjTextureShader::OnDestroyDevice()
{
	m_textureRes.Clear();
}

BOOL DxEffProjTextureShader::RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSurface, LPDIRECT3DTEXTUREQ pTexture )
{
	if( !m_bPlayON )
        return FALSE;

	if( !m_rFX )
		return FALSE;

    if( !m_textureRes.Update() )
        return FALSE;

	pd3dDevice->SetRenderTarget( 0, pSurface );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	g_pSavedRS_STATIC->Capture();
	g_pDrawRS_STATIC->Apply();

	m_rFX->SetFloat( "g_fAlpha", m_fAlpha );
	m_rFX->SetTexture( "g_BackGroundTexture", pTexture );
	m_rFX->SetTexture( "g_BaseTexture", m_textureRes.GetTexture() );

	UINT numPasses;
	m_rFX->Begin( &numPasses, 0 );
	if( 0 < numPasses )
	{
		m_rFX->BeginPass( 0 );
		{
			// FX ¸¶¹«¸®
			m_rFX->CommitChanges();

			RenderEX( pd3dDevice );
		}
		m_rFX->EndPass();
	}
	m_rFX->End();

	g_pSavedRS_STATIC->Apply();

	return TRUE;
}

void DxEffProjTextureShader::RenderEX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize();
	if( !DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
		return;

	UINT uiStartVertex = DxDynamicVB::g_sVB_I_24BYTE.CopyToVB(
		m_sVertex,
		dwVertexSizeFULL,
		4 );

	pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize() );

	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, uiStartVertex, 2 );
}

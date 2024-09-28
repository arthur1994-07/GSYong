#include "pch.h"

//#include "../../EngineBaseLib/Common/StlFunctions.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "../DxTools/TextureManager.h"

#include "./WaterSurfPointMan.h"
#include "./WaterSurfDef.h"
#include "./WaterSurf.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
//							W	a	t	e	r		S	u	r	f							
//---------------------------------------------------------------------------------------------
WaterSurf::WaterSurf()
    : m_pd3dDevice( NULL )
    , m_pSavedSB( NULL )
    , m_pDrawSB( NULL )

    , m_nSELECT_INDEX( -1 )
{
    D3DXMatrixIdentity( &m_matIdentity );
}

WaterSurf::~WaterSurf()
{
	CleanUp();
}

void WaterSurf::CleanUp()
{
	m_nSELECT_INDEX = -1;

	std::for_each( m_vecWaterSurfPointMan.begin(), m_vecWaterSurfPointMan.end(), std_afunc::DeleteObject() );
	m_vecWaterSurfPointMan.clear();
}

void WaterSurf::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

	for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->OnCreateDevice( pd3dDevice );
	}
}

void WaterSurf::OnDestroyDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->OnDestroyDevice();
	}
}

void WaterSurf::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

    for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}

    for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->OnResetDevice( pd3dDevice );
	}
}

void WaterSurf::OnLostDevice()
{
    SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

    for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->OnLostDevice();
	}
}

void WaterSurf::CreateSurf( LPDIRECT3DDEVICEQ pd3dDevice, DxSetLandMan* pSetLandMan, const D3DXVECTOR3& vPos, float fRadius, BOOL bLeft )
{
	WaterSurfPointMan* pNew = new WaterSurfPointMan;
	pNew->Create( pSetLandMan, vPos, fRadius, bLeft );
	pNew->OnCreateDevice( pd3dDevice );
    pNew->OnResetDevice( pd3dDevice );
	m_vecWaterSurfPointMan.push_back( pNew );

	m_nSELECT_INDEX = -1;
}

void WaterSurf::DeleteSurf( int nIndex )
{
	int nCount(0);
	VEC_WATERSURFPOINTMAN_ITER iter = m_vecWaterSurfPointMan.begin();
	for( ; iter!=m_vecWaterSurfPointMan.end(); ++iter, ++nCount )
	{
		if( nCount == nIndex )
		{
			m_nSELECT_INDEX = -1;

            WaterSurfPointMan* pPointMan = *iter;
            pPointMan->OnLostDevice();
            pPointMan->OnDestroyDevice();
            SAFE_DELETE( pPointMan );

			m_vecWaterSurfPointMan.erase( iter );
			return;
		}
	}
}

void WaterSurf::MoveCamera( LPDIRECT3DDEVICEQ pd3dDevice, int nIndex )
{
	int nCount(0);
	VEC_WATERSURFPOINTMAN_ITER iter = m_vecWaterSurfPointMan.begin();
	for( ; iter!=m_vecWaterSurfPointMan.end(); ++iter, ++nCount )
	{
		if( nCount == nIndex )
		{
			m_nSELECT_INDEX = nIndex;	// ¼±ÅÃµÈ Index

			(*iter)->MoveCamera( pd3dDevice );
			return;
		}
	}
}

void WaterSurf::ClearIndex()
{
	m_nSELECT_INDEX = -1;
}

void WaterSurf::SetObjRotate90( const D3DXMATRIX& matRotate )
{
    for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->SetObjRotate90( matRotate );
	}
}

void WaterSurf::FrameMove( float fElapsedTime )
{
	for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->FrameMove( fElapsedTime );
	}
}

void WaterSurf::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( m_vecWaterSurfPointMan.empty() )
        return;

    m_pSavedSB->Capture();
	m_pDrawSB->Apply();

    pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );
    pd3dDevice->SetFVF( WATERSURFVERTEX::FVF );
	pd3dDevice->SetIndices( NULL );

    for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		m_vecWaterSurfPointMan[i]->Render( pd3dDevice );
	}

    m_pSavedSB->Apply();
}

void WaterSurf::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecWaterSurfPointMan.size(); ++i )
	{
		if( i == m_nSELECT_INDEX )
		{
			m_vecWaterSurfPointMan[i]->RenderEdit( pd3dDevice, 5.f, 0xffffffff );
		}
		else
		{
			m_vecWaterSurfPointMan[i]->RenderEdit( pd3dDevice, 3.f, 0xbbbbbbbb );
		}
	}
}

void WaterSurf::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = (DWORD) m_vecWaterSurfPointMan.size();
		SFile << dwSize;

		for( DWORD i=0; i<dwSize; ++i )
		{
			m_vecWaterSurfPointMan[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void WaterSurf::Load( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize;
	SFile >> dwSize;

	for( DWORD i=0; i<dwSize; ++i )
	{
		WaterSurfPointMan* pNew = new WaterSurfPointMan;
		pNew->Load( SFile, pd3dDevice );
		m_vecWaterSurfPointMan.push_back( pNew );
	}
}

void WaterSurf::Import( LPDIRECT3DDEVICEQ pd3dDevice, WaterSurf* pWaterSurf )
{
	CleanUp();

	for ( DWORD i=0; i<pWaterSurf->m_vecWaterSurfPointMan.size(); ++i )
	{
		WaterSurfPointMan* pNew = new WaterSurfPointMan;
		pNew->Import( pd3dDevice, pWaterSurf->m_vecWaterSurfPointMan[i] );
		m_vecWaterSurfPointMan.push_back( pNew );
	}
}
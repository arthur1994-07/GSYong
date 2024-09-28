#include "pch.h"

#include "DxEffProjBase.h"



DxEffProjBase::DxEffProjBase( BOOL bEnableON, float fCreatingTime, float fDestroyingTime )
	: m_bEnableON(bEnableON)
	, m_fCreatingTime(fCreatingTime)
	, m_fDestroyingTime(fDestroyingTime)
	, m_bPlayON(FALSE)
	, m_fAlpha(0.f)
{
}

DxEffProjBase::~DxEffProjBase()
{
}

BOOL DxEffProjBase::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<4; ++i )
	{
		m_sVertex[i].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
	}
	m_sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f);
	m_sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	m_sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	m_sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	return TRUE; 
}

void DxEffProjBase::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPDIRECT3DSURFACEQ	pBackBuffer;
	D3DSURFACE_DESC		d3dsdBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pBackBuffer->GetDesc( &d3dsdBackBuffer );
	float dwWidth = (float)d3dsdBackBuffer.Width;
	float dwHeight = (float)d3dsdBackBuffer.Height;
	pBackBuffer->Release();

	m_sVertex[0].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
	m_sVertex[1].vPos = D3DXVECTOR4( dwWidth, 0.f, 1.f, 1.f );
	m_sVertex[2].vPos = D3DXVECTOR4( 0.f, dwHeight, 1.f, 1.f );
	m_sVertex[3].vPos = D3DXVECTOR4( dwWidth, dwHeight, 1.f, 1.f );
}

void DxEffProjBase::FrameMove( float fElapsedTime )
{
	if( m_bEnableON )	// 활성화 하라
	{
		m_bPlayON = TRUE;		// Play 해라.

		if ( m_fCreatingTime <= 0.f )
		{
			m_fAlpha = 1.f;
		}
		else
		{
			m_fAlpha += fElapsedTime * (1.f/m_fCreatingTime);
			if( m_fAlpha > 1.f )
				m_fAlpha = 1.f;
		}
	}
	else				// 비활성화 하라.
	{
		if ( m_fDestroyingTime <= 0.f )
		{
			m_bPlayON = FALSE;	// Play 하지 않음.
			m_fAlpha = 0.f;
		}
		else
		{
			m_fAlpha -= fElapsedTime * (1.f/m_fDestroyingTime);
			if( m_fAlpha < 0.f )
			{
				m_bPlayON = FALSE;	// Play 하지 않음.
				m_fAlpha = 0.f;
			}
		}
	}
}

void DxEffProjBase::EnableEFF( BOOL bUse )
{
	m_bEnableON = bUse;
}

BOOL DxEffProjBase::IsPlayOFF() const
{
	return m_bPlayON ? FALSE : TRUE;
}
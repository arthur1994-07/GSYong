#include "pch.h"

#include "../../../InternalCommonLib/Define/TextureAddData.h"

#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "./DxPieceStaticAni.h"
#include "./DxPiece.h"

#include "DxPieceInstancingForShadow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Shadow 에서만 사용되는 Instancing
DxPieceInstancingForShadow::DxPieceInstancingForShadow( LPDIRECT3DDEVICEQ pd3dDevice, const DxStaticAniFrame* pStaticAniFrame, DWORD dwPieceMax )
	: m_rStaticAniFrame(pStaticAniFrame)
	, m_pInstanceDataVB(NULL)
	, m_dwPieceMax(dwPieceMax)
{
	CreateVB( pd3dDevice );
}

DxPieceInstancingForShadow::~DxPieceInstancingForShadow()
{
	SAFE_RELEASE( m_pInstanceDataVB );
	m_rStaticAniFrame = NULL;
}

void DxPieceInstancingForShadow::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CreateVB( pd3dDevice );
}

void DxPieceInstancingForShadow::OnLostDevice()
{
	SAFE_RELEASE ( m_pInstanceDataVB );
}

void DxPieceInstancingForShadow::CreateVB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Create a  VB for the Instancing data
	SAFE_RELEASE ( m_pInstanceDataVB );
	pd3dDevice->CreateVertexBuffer( m_dwPieceMax * sizeof( INSTANCEDATA_POS ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pInstanceDataVB, 0 );
	if ( !m_pInstanceDataVB )
	{
		pd3dDevice->CreateVertexBuffer( m_dwPieceMax * sizeof( INSTANCEDATA_POS ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_SYSTEMMEM, &m_pInstanceDataVB, 0 );
	}
}

//////////////////////////////////////////////////////////////////////////
//							Thread
void DxPieceInstancingForShadow::Insert_THREAD( const DxPiece* pPiece )
{
	m_vecrPiece.push_back( pPiece );
}

void DxPieceInstancingForShadow::Reset_THREAD()
{
	m_vecrPiece.clear();
}

BOOL DxPieceInstancingForShadow::IsInsertDataToVB_THREAD()
{
	DWORD dwPieceNum = m_vecrPiece.size();

	if ( dwPieceNum < EM_INSTANCING_PIECE_MIN )
		return FALSE;

	// 위치좌표를 넣는다.
	{
		m_sDefaultLock_VB.lockOn();
		{
			INSTANCEDATA_POS* pLocked = NULL;
			m_pInstanceDataVB->Lock( 0, 0, (VOID**)&pLocked, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<dwPieceNum; ++i )
			{
				memcpy( &pLocked[i].m_vMatrix1.x, &m_vecrPiece[i]->m_matWorld._11, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix2.x, &m_vecrPiece[i]->m_matWorld._21, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix3.x, &m_vecrPiece[i]->m_matWorld._31, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix4.x, &m_vecrPiece[i]->m_matWorld._41, sizeof(D3DXVECTOR4) );
			}
			m_pInstanceDataVB->Unlock();
		}
		m_sDefaultLock_VB.lockOff();
	}

	return TRUE;
}

void DxPieceInstancingForShadow::InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing )
{
	if ( !IsInsertDataToVB_THREAD() )
	{
		for ( DWORD i=0; i<m_vecrPiece.size(); ++i )
		{
			vecExceptInstancing.push_back( const_cast<DxPiece*>( m_vecrPiece[i] ) );	
		}
		m_vecrPiece.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
//								Render
void DxPieceInstancingForShadow::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_OPAQUE|EMTTF_UNKNOWN) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		//V( pd3dDevice->SetStreamSource( 0, g_pVBBox, 0, sizeof( BOX_VERTEX ) ) );
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA_POS ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderCascadeShadowOpaque_Instancing( pd3dDevice );
	}
	m_sDefaultLock_VB.lockOff();
}

void DxPieceInstancingForShadow::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_HARD) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		//V( pd3dDevice->SetStreamSource( 0, g_pVBBox, 0, sizeof( BOX_VERTEX ) ) );
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA_POS ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderCascadeShadowHardAlpha_Instancing( pd3dDevice );
	}
	m_sDefaultLock_VB.lockOff();
}
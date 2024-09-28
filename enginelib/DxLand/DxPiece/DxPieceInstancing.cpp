#include "pch.h"

#include "../../../InternalCommonLib/Define/TextureAddData.h"

#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../../DxTools/DxViewPort.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceDef.h"
#include "./DxPiece.h"

#include "DxPieceInstancing.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Shadow ������ ���Ǵ� Instancing
DxPieceInstancing::DxPieceInstancing( LPDIRECT3DDEVICEQ pd3dDevice, const DxStaticAniFrame* pStaticAniFrame, DWORD dwPieceMax )
	: m_rStaticAniFrame(pStaticAniFrame)
	, m_pInstanceDataVB(NULL)
	, m_dwPieceMax(dwPieceMax)
{
	CreateVB( pd3dDevice );
}

DxPieceInstancing::~DxPieceInstancing()
{
	SAFE_RELEASE( m_pInstanceDataVB );
	m_rStaticAniFrame = NULL;
}

void DxPieceInstancing::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CreateVB( pd3dDevice );
}

void DxPieceInstancing::OnLostDevice()
{
	m_vecrPiece.clear();
	SAFE_RELEASE ( m_pInstanceDataVB );
}

void DxPieceInstancing::CreateVB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Create a  VB for the Instancing data
	SAFE_RELEASE ( m_pInstanceDataVB );
	pd3dDevice->CreateVertexBuffer( m_dwPieceMax * sizeof( INSTANCEDATA ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pInstanceDataVB, 0 );
	if ( !m_pInstanceDataVB )
	{
		pd3dDevice->CreateVertexBuffer( m_dwPieceMax * sizeof( INSTANCEDATA ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_SYSTEMMEM, &m_pInstanceDataVB, 0 );
	}

}
//
//BOOL DxPieceInstancing::FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
//{
//	//for ( DWORD i=0; i<m_vecrPiece.size(); ++i )
//	//{
//	//	m_vecrPiece[i]->FrameMove_Instancing( fElapsedTime );
//	//}
//
//	//// �ʼ� ������ Update �ϴµ� �ʿ��ϴ�.
//	//m_vecrPiece[0]->FrameMoveRAD( TRUE );
//	//m_vecrPiece[0]->FrameMoveStep( pd3dDevice, FALSE );
//
//	//// Soft Alpha �� �ִٸ� Instancing �� ���� �� ����.
//	//if ( m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_SOFT) )
//	//	return FALSE;
//
//	return TRUE;
//
//	////// Alpha �� ��� ���� �����ϵ��� �Ѵ�.
//	////{
//	////	float InvisibleRadius = m_vecrPiece[0]->GetInvisibleRadius();
//
//	////	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
//	////	D3DXVECTOR3 vDir = m_vCenter - vLookatPt;
//	////	float fLengthLookat = D3DXVec3Length( &vDir );	// ī�޶� ������ �Ÿ�
//	////	fLengthLookat -= m_fRadius;						// ��ü�� ���������� ����� �ش�.
//
//	////	float fAlphaSpeed = m_fAlpha;
//	////	fAlphaSpeed -= 0.5f;
//	////	fAlphaSpeed = fabsf( fAlphaSpeed );
//	////	fAlphaSpeed += 0.05f;		// 0.5 ~ 0.05 ~ 0.5					// ��ȭ �ӵ� ���� ����.
//	////	fAlphaSpeed *= 1.5f;		// ��ġ2�� �뷫 1�� ���� �ɸ���.	// ��ȭ �ӵ� ���� ����.
//	////	
//	////	
//	////	if ( InvisibleRadius >= fLengthLookat )
//	////	{
//	////		m_fAlpha = m_fAlpha + (fElapsedTime*fAlphaSpeed);	// �þ��.
//	////	
//	////		if ( m_fAlpha > 1.f )
//	////		{
//	////			m_fAlpha = 1.f;
//	////		}
//	////	}
//	////	else
//	////	{
//	////		m_fAlpha = m_fAlpha - (fElapsedTime*fAlphaSpeed);	// ������.
//	////	
//	////		if ( m_fAlpha < 0.f )
//	////		{
//	////			m_fAlpha = 0.f;
//	////		}
//	////	}
//	////}
//
//	////PieceParameter* pPieceParameterNoneConst = m_vecrPiece[0]->GetPieceParameterNoneConst();
//	////pPieceParameterNoneConst->m_fAlpha = m_fAlpha;
//}
//



//////////////////////////////////////////////////////////////////////////
//							Thread
void DxPieceInstancing::Insert_THREAD( const DxPiece* pPiece )
{
	m_vecrPiece.push_back( pPiece );
}

void DxPieceInstancing::Reset_THREAD()
{
	m_vecrPiece.clear();
}

BOOL DxPieceInstancing::IsInsertDataToVB_THREAD()
{
	DWORD dwPieceNum = m_vecrPiece.size();

	if ( dwPieceNum < EM_INSTANCING_PIECE_MIN )
		return FALSE;

	// ��ġ��ǥ�� �ִ´�.
	{
		m_sDefaultLock_VB.lockOn();
		{
			INSTANCEDATA* pLocked = NULL;
			m_pInstanceDataVB->Lock( 0, 0, (VOID**)&pLocked, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<dwPieceNum; ++i )
			{
				const PieceParameter* pPieceParameter = m_vecrPiece[i]->GetPieceParameter();

				memcpy( &pLocked[i].m_vMatrix1_AC0W.x,		&m_vecrPiece[i]->m_matWorld._11, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix2_LMOffsetX.x,	&m_vecrPiece[i]->m_matWorld._21, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix3_LMOffsetY.x,	&m_vecrPiece[i]->m_matWorld._31, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix4.x,			&m_vecrPiece[i]->m_matWorld._41, sizeof(D3DXVECTOR4) );

				pLocked[i].m_vVC_AC0XY.x = pPieceParameter->m_sLMBCData.m_pVoxelColor->x;
				pLocked[i].m_vVC_AC0XY.y = pPieceParameter->m_sLMBCData.m_pVoxelColor->y;
				pLocked[i].m_vVC_AC0XY.z = pPieceParameter->m_sLMBCData.m_pVoxelColor->z;
				pLocked[i].m_vVC_AC0XY.w = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[0].x/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[0].y/3 );

				pLocked[i].m_vMatrix1_AC0W.w = pPieceParameter->m_sLMBCData.m_pAmbientCube[0].w;
				pLocked[i].m_vMatrix2_LMOffsetX.w = pPieceParameter->m_vLightMapUV_Offset.x;
				pLocked[i].m_vMatrix3_LMOffsetY.w = pPieceParameter->m_vLightMapUV_Offset.y;

				pLocked[i].m_vAmbientCube_0Z_12_3X.x = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[0].z/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[1].x/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.y = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[1].y/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[1].z/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.z = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[2].x/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[2].y/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.w = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[2].z/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[3].x/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.x = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[3].y/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[3].z/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.y = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[4].x/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[4].y/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.z = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[4].z/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[5].x/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.w = NSMATH::ExportVector2ToFloat( pPieceParameter->m_sLMBCData.m_pAmbientCube[5].y/3, pPieceParameter->m_sLMBCData.m_pAmbientCube[5].z/3 );
			}
			m_pInstanceDataVB->Unlock();
		}
		m_sDefaultLock_VB.lockOff();
	}

	return TRUE;
}

BOOL DxPieceInstancing::IsInsertDataToVB_Reflect_THREAD( float fHeight )
{
	DWORD dwPieceNum = m_vecrPiece.size();

	if ( dwPieceNum < EM_INSTANCING_PIECE_MIN )
		return FALSE;

	// ��ġ��ǥ�� �ִ´�.
	{
		m_sDefaultLock_VB.lockOn();
		{
			D3DXMATRIX matWorldMirror;
			const D3DXVECTOR4* arrayAmbientCube(NULL);

			INSTANCEDATA* pLocked = NULL;
			m_pInstanceDataVB->Lock( 0, 0, (VOID**)&pLocked, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<dwPieceNum; ++i )
			{
				const PieceParameter* pPieceParameter = m_vecrPiece[i]->GetPieceParameter();

				matWorldMirror = m_vecrPiece[i]->m_matWorld;
				matWorldMirror._42 = matWorldMirror._42 - (fHeight*2.f);
				D3DXMatrixMultiply_MIRROR( matWorldMirror );

				memcpy( &pLocked[i].m_vMatrix1_AC0W.x,		&matWorldMirror._11, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix2_LMOffsetX.x,	&matWorldMirror._21, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix3_LMOffsetY.x,	&matWorldMirror._31, sizeof(D3DXVECTOR3) );
				memcpy( &pLocked[i].m_vMatrix4.x,			&matWorldMirror._41, sizeof(D3DXVECTOR4) );

				// �ݻ�� TnL_PS_3_0_DF �θ� ���۵ȴ�.
				arrayAmbientCube = m_vecrPiece[i]->GetAmbientCube_LightMapBoxColorFullData();

				pLocked[i].m_vVC_AC0XY.x = pPieceParameter->m_sLMBCData.m_pVoxelColor->x;
				pLocked[i].m_vVC_AC0XY.y = pPieceParameter->m_sLMBCData.m_pVoxelColor->y;
				pLocked[i].m_vVC_AC0XY.z = pPieceParameter->m_sLMBCData.m_pVoxelColor->z;
				pLocked[i].m_vVC_AC0XY.w = NSMATH::ExportVector2ToFloat( arrayAmbientCube[0].x/3, arrayAmbientCube[0].y/3 );

				pLocked[i].m_vMatrix1_AC0W.w = arrayAmbientCube[0].w;
				pLocked[i].m_vMatrix2_LMOffsetX.w = pPieceParameter->m_vLightMapUV_Offset.x;
				pLocked[i].m_vMatrix3_LMOffsetY.w = pPieceParameter->m_vLightMapUV_Offset.y;

				pLocked[i].m_vAmbientCube_0Z_12_3X.x = NSMATH::ExportVector2ToFloat( arrayAmbientCube[0].z/3, arrayAmbientCube[1].x/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.y = NSMATH::ExportVector2ToFloat( arrayAmbientCube[1].y/3, arrayAmbientCube[1].z/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.z = NSMATH::ExportVector2ToFloat( arrayAmbientCube[2].x/3, arrayAmbientCube[2].y/3 );
				pLocked[i].m_vAmbientCube_0Z_12_3X.w = NSMATH::ExportVector2ToFloat( arrayAmbientCube[2].z/3, arrayAmbientCube[3].x/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.x = NSMATH::ExportVector2ToFloat( arrayAmbientCube[3].y/3, arrayAmbientCube[3].z/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.y = NSMATH::ExportVector2ToFloat( arrayAmbientCube[4].x/3, arrayAmbientCube[4].y/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.z = NSMATH::ExportVector2ToFloat( arrayAmbientCube[4].z/3, arrayAmbientCube[5].x/3 );
				pLocked[i].m_vAmbientCube_3YZ_45.w = NSMATH::ExportVector2ToFloat( arrayAmbientCube[5].y/3, arrayAmbientCube[5].z/3 );
			}
			m_pInstanceDataVB->Unlock();
		}
		m_sDefaultLock_VB.lockOff();
	}

	return TRUE;
}

void DxPieceInstancing::InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing )
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

void DxPieceInstancing::InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight )
{
	if ( !IsInsertDataToVB_Reflect_THREAD( fHeight ) )
	{
		for ( DWORD i=0; i<m_vecrPiece.size(); ++i )
		{
			vecExceptInstancing.push_back( const_cast<DxPiece*>( m_vecrPiece[i] ) );	
		}
		m_vecrPiece.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
//								FrameMove
BOOL DxPieceInstancing::FrameMoveStep( float fElapsedTime )
{
	if ( m_vecrPiece.empty() )
		return TRUE;

	DxPiece* pPiece = const_cast<DxPiece*>( m_vecrPiece[0] );
	pPiece->FrameMoveMAT_OnlyStaticAniFrame( fElapsedTime );

	// ���� �ؽ��İ� �� �������� ������ Instancing �� ���� �ǵ��� ���ش�.
	if ( m_rStaticAniFrame->IsTexType(EMTTF_UNKNOWN) )
		return TRUE;

	// Opaque, AlphaHard �Ѵ� ���ٸ� Instancing �� ���۵Ǹ� �ȵȴ�.
	if ( !m_rStaticAniFrame->IsTexType(EMTTF_OPAQUE) && !m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_HARD) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//								Render
void DxPieceInstancing::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_OPAQUE|EMTTF_UNKNOWN) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		const PieceParameter* pPieceParameter = m_vecrPiece[0]->GetPieceParameter();

		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderOpaque_Instancing( *pPieceParameter );
	}
	m_sDefaultLock_VB.lockOff();
}

void DxPieceInstancing::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_HARD) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		const PieceParameter* pPieceParameter = m_vecrPiece[0]->GetPieceParameter();

		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderHardAlpha_Instancing( *pPieceParameter );
	}
	m_sDefaultLock_VB.lockOff();
}

void DxPieceInstancing::RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_DECAL_BEFORE) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		const PieceParameter* pPieceParameter = m_vecrPiece[0]->GetPieceParameter();

		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderDecal_Instancing( *pPieceParameter );
	}
	m_sDefaultLock_VB.lockOff();
}

void DxPieceInstancing::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( m_fAlpha <= 0.f )
	//	return;

	if ( m_vecrPiece.empty() )
		return;

	if ( !m_rStaticAniFrame->IsTexType(EMTTF_DECAL_AFTER) )
		return;

	m_sDefaultLock_VB.lockOn();
	{
		const PieceParameter* pPieceParameter = m_vecrPiece[0]->GetPieceParameter();

		DWORD dwPieceNum = m_vecrPiece.size();

		// Stream zero is our model, and its frequency is how we communicate the number of instances required,
		// which in this case is the total number of boxes
		pd3dDevice->SetStreamSourceFreq( 0, D3DSTREAMSOURCE_INDEXEDDATA | dwPieceNum );

		// Stream one is the Instancing buffer, so this advances to the next value
		// after each box instance has been drawn, so the divider is 1.
		pd3dDevice->SetStreamSource( 1, m_pInstanceDataVB, 0, sizeof( INSTANCEDATA ) );
		pd3dDevice->SetStreamSourceFreq( 1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );

		DxStaticAniFrame* pStaticAniFrame = const_cast<DxStaticAniFrame*>( m_rStaticAniFrame );
		pStaticAniFrame->RenderDecalPass2_Instancing( *pPieceParameter );
	}
	m_sDefaultLock_VB.lockOff();
}

#pragma once

#include "./DxPieceInstancingForShadow.h"
#include "./DxPieceInstancing.h"
#include "./DxPieceStaticAni.h"


// Shadow ������ ���Ǵ� Instancing
template <typename T>
DxPieceInstancingLodVB<T>::DxPieceInstancingLodVB( const DxStaticAniFrame* pStaticAniFrame )
	: m_rStaticAniFrame(pStaticAniFrame)
	, m_dwPieceMax(0)
	, m_dwPieceCV(0)
	, m_dwLOD(0)
{
}

template <typename T>
DxPieceInstancingLodVB<T>::~DxPieceInstancingLodVB()
{
	m_rStaticAniFrame = NULL;
}

template <typename T>
void DxPieceInstancingLodVB<T>::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecInstancingLOD.size(); ++i )
	{
		m_vecInstancingLOD[i]->OnResetDevice( pd3dDevice );
	}
}

template <typename T>
void DxPieceInstancingLodVB<T>::OnLostDevice()
{
	for ( DWORD i=0; i<m_vecInstancingLOD.size(); ++i )
	{
		m_vecInstancingLOD[i]->OnLostDevice();
	}
}

//////////////////////////////////////////////////////////////////////////
//	�̰� �ܷ� �������� ���ϴ� ���� DxPiece::IsInstancingDynamicOption() ���� Ȯ���� �ȴ�.
//////////////////////////////////////////////////////////////////////////
template <typename T>
void DxPieceInstancingLodVB<T>::Import_AddRef()
{
	// Material System �� �ƴϸ� Instnacing �Ұ�.
	if ( !m_rStaticAniFrame->IsMaterialMesh() )
		return;

	//// Soft Alpha �� �ִٸ� Instancing �� ���� �� ����.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_SOFT) )
	//	return;

	//// Animation �� �ִٸ� Instancing �� ���� �� ����.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_OPAQUE) )
	//	return;

	//// Animation �� �ִٸ� Instancing �� ���� �� ����.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_ALPHA_HARD) )
	//	return;

	//// Animation �� �ִٸ� Instancing �� ���� �� ����.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_ALPHA_SOFT) )
	//	return;

	++m_dwPieceMax;
}

template <typename T>
BOOL DxPieceInstancingLodVB<T>::CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �ʱ�ȭ
	m_vecPieceSizeLOD.clear();
	m_vecInstancingLOD.clear();

	// 2�� �̻��� �ȵǸ� Instancing �� ���� �ʴ´�.
	if ( m_dwPieceMax < EM_INSTANCING_PIECE_MIN )
		return FALSE;

	// 1�ܰ�� �����.
	m_vecPieceSizeLOD.push_back( m_dwPieceMax );
	
	// ���� m_vecrPieceAll.size() �� 26 �� ���
	// 1�ܰ迡 26
	// �̰����� 26/2=13 >= 2 -> 2�ܰ�
	// �̰����� 13/2=6 >= 2 -> 3�ܰ�
	// �̰����� 6/2=3 >= 2 -> 4�ܰ�
	// �̰����� 3/2=1 > 2 -> ���̻� �ܰ踦 ������ �ʴ´�.
	DWORD dwPieceSize = m_dwPieceMax / 2;
	while ( dwPieceSize >= EM_INSTANCING_PIECE_MIN )
	{
		m_vecPieceSizeLOD.push_back( dwPieceSize );
		dwPieceSize = dwPieceSize / 2;
	}

	// ������ �����͸� �����.
	m_vecInstancingLOD.reserve( m_vecPieceSizeLOD.size() );
	for ( DWORD i=0; i<m_vecPieceSizeLOD.size(); ++i )
	{
		boost::shared_ptr<T> spNew(new T(pd3dDevice, m_rStaticAniFrame, m_vecPieceSizeLOD[i]));
		m_vecInstancingLOD.push_back( spNew );
	}
	
	return TRUE;
}

//template <typename T>
//BOOL DxPieceInstancingLodVB<T>::FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
//{
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	// �ϴ� �̰͵��� FrameMove ������ �����Ѵ�.
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//{
//	//	for ( DWORD i=0; i<m_vecrPieceAll.size(); ++i )
//	//	{
//	//		m_vecrPieceAll[i]->FrameMove_Instancing( fElapsedTime );
//	//	}
//
//	//	// �ʼ� ������ Update �ϴµ� �ʿ��ϴ�.
//	//	m_vecrPieceAll[0]->FrameMoveRAD( TRUE );
//	//	m_vecrPieceAll[0]->FrameMoveStep( pd3dDevice, FALSE );
//	//}
//
//	// Soft Alpha �� �ִٸ� Instancing �� ���� �� ����.
//	if ( m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_SOFT) )
//		return FALSE;
//
//	return TRUE;
//}

//////////////////////////////////////////////////////////////////////////
//								Thread
template <typename T>
void DxPieceInstancingLodVB<T>::ResetRef_THREAD()
{
	m_dwPieceCV = 0;
}

template <typename T>
void DxPieceInstancingLodVB<T>::AddRef_THREAD()
{
	++m_dwPieceCV;
}

template <typename T>
void DxPieceInstancingLodVB<T>::Reset_THREAD()
{
	m_dwLOD = 0;
	for ( DWORD i=1; i<m_vecPieceSizeLOD.size(); ++i )
	{
		// i �� 1 ���� ������ LOD �� 0
		// �̰� ������ i �� 3 ���� ������ LOD �� 2 �� ����Ѵ�. (LOD�Ѱ���4����)
		// �̰� ���н� i �� 3 ���� ������ LOD �� 3 �� ����Ѵ�. (LOD�Ѱ���4����)
		if ( m_dwPieceCV > m_vecPieceSizeLOD[i] )
		{
			break;
		}

		++m_dwLOD;
	}

	m_vecInstancingLOD[m_dwLOD]->Reset_THREAD();
}

template <typename T>
void DxPieceInstancingLodVB<T>::Insert_THREAD( const DxPiece* pPiece )
{
	m_vecInstancingLOD[m_dwLOD]->Insert_THREAD( pPiece );
}

template <typename T>
void DxPieceInstancingLodVB<T>::InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing )
{
	m_vecInstancingLOD[m_dwLOD]->InsertDataToVB_ExceptPiece_THREAD( vecExceptInstancing );
}

template <typename T>
void DxPieceInstancingLodVB<T>::InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight )
{
	m_vecInstancingLOD[m_dwLOD]->InsertDataToVB_ExceptPiece_Reflect_THREAD( vecExceptInstancing, fHeight );
}

//////////////////////////////////////////////////////////////////////////
//								FrameMove
template <typename T>
BOOL DxPieceInstancingLodVB<T>::FrameMoveStep( float fElapsedTime )
{
	return m_vecInstancingLOD[m_dwLOD]->FrameMoveStep( fElapsedTime );
}

//////////////////////////////////////////////////////////////////////////
//								Render
template <typename T>
void DxPieceInstancingLodVB<T>::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_vecInstancingLOD[m_dwLOD]->RenderOpaque( pd3dDevice );
}

template <typename T>
void DxPieceInstancingLodVB<T>::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_vecInstancingLOD[m_dwLOD]->RenderHardAlpha( pd3dDevice );
}

template <typename T>
void DxPieceInstancingLodVB<T>::RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_vecInstancingLOD[m_dwLOD]->RenderDecal( pd3dDevice );
}

template <typename T>
void DxPieceInstancingLodVB<T>::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_vecInstancingLOD[m_dwLOD]->RenderDecalPass2( pd3dDevice );
}

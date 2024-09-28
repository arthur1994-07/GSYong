#pragma once

#include "./DxPieceInstancingForShadow.h"
#include "./DxPieceInstancing.h"
#include "./DxPieceStaticAni.h"


// Shadow 에서만 사용되는 Instancing
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
//	이것 외로 동적으로 변하는 것은 DxPiece::IsInstancingDynamicOption() 에서 확인이 된다.
//////////////////////////////////////////////////////////////////////////
template <typename T>
void DxPieceInstancingLodVB<T>::Import_AddRef()
{
	// Material System 이 아니면 Instnacing 불가.
	if ( !m_rStaticAniFrame->IsMaterialMesh() )
		return;

	//// Soft Alpha 가 있다면 Instancing 에 들어올 수 없다.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_ALPHA_SOFT) )
	//	return;

	//// Animation 이 있다면 Instancing 에 들어올 수 없다.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_OPAQUE) )
	//	return;

	//// Animation 이 있다면 Instancing 에 들어올 수 없다.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_ALPHA_HARD) )
	//	return;

	//// Animation 이 있다면 Instancing 에 들어올 수 없다.
	//if ( m_rStaticAniFrame->IsTexType(EMTTF_FRAMEANI_ALPHA_SOFT) )
	//	return;

	++m_dwPieceMax;
}

template <typename T>
BOOL DxPieceInstancingLodVB<T>::CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 초기화
	m_vecPieceSizeLOD.clear();
	m_vecInstancingLOD.clear();

	// 2개 이상이 안되면 Instancing 을 하지 않는다.
	if ( m_dwPieceMax < EM_INSTANCING_PIECE_MIN )
		return FALSE;

	// 1단계는 만든다.
	m_vecPieceSizeLOD.push_back( m_dwPieceMax );
	
	// 만약 m_vecrPieceAll.size() 가 26 개 라면
	// 1단계에 26
	// 이곳에서 26/2=13 >= 2 -> 2단계
	// 이곳에서 13/2=6 >= 2 -> 3단계
	// 이곳에서 6/2=3 >= 2 -> 4단계
	// 이곳에서 3/2=1 > 2 -> 더이상 단계를 만들지 않는다.
	DWORD dwPieceSize = m_dwPieceMax / 2;
	while ( dwPieceSize >= EM_INSTANCING_PIECE_MIN )
	{
		m_vecPieceSizeLOD.push_back( dwPieceSize );
		dwPieceSize = dwPieceSize / 2;
	}

	// 실제로 데이터를 만든다.
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
//	// 일단 이것들은 FrameMove 쪽으로 빼야한다.
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	//{
//	//	for ( DWORD i=0; i<m_vecrPieceAll.size(); ++i )
//	//	{
//	//		m_vecrPieceAll[i]->FrameMove_Instancing( fElapsedTime );
//	//	}
//
//	//	// 필수 정보들 Update 하는데 필요하다.
//	//	m_vecrPieceAll[0]->FrameMoveRAD( TRUE );
//	//	m_vecrPieceAll[0]->FrameMoveStep( pd3dDevice, FALSE );
//	//}
//
//	// Soft Alpha 가 있다면 Instancing 에 들어올 수 없다.
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
		// i 가 1 에서 끝나면 LOD 는 0
		// 이곳 성공시 i 가 3 에서 끝나면 LOD 는 2 를 사용한다. (LOD총갯수4개시)
		// 이곳 실패시 i 가 3 에서 끝나면 LOD 는 3 을 사용한다. (LOD총갯수4개시)
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

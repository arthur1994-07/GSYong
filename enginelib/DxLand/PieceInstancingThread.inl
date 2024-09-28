#pragma once

#include <algorithm>

#include "./DxPiece/DxPieceInstancingLodVB.h"
#include "./DxPiece/DxPiece.h"



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//							PieceInstancingSwap
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////							
template <typename T>
PieceInstancingSwap<T>::PieceInstancingSwap( DxStaticAniFrame* pStaticAniFrame )
	: m_nUSED(0)
	, m_nBackUp(1)
	, m_bDelete(FALSE)
{
	for ( DWORD i=0; i<2; ++i )
	{
		m_mapPieceInstancingSwap[i].reset( new DxPieceInstancingLodVB<T>(pStaticAniFrame) );
	}
}

template <typename T>
PieceInstancingSwap<T>::~PieceInstancingSwap()
{
	CleanUp();
}

template <typename T>
void PieceInstancingSwap<T>::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<2; ++i )
	{
		m_mapPieceInstancingSwap[i]->OnResetDevice( pd3dDevice );
	}
}

template <typename T>
void PieceInstancingSwap<T>::OnLostDevice()
{
	for ( DWORD i=0; i<2; ++i )
	{
		m_mapPieceInstancingSwap[i]->OnLostDevice();
	}
}

template <typename T>
void PieceInstancingSwap<T>::CleanUp()
{
}

template <typename T>
void PieceInstancingSwap<T>::Import_AddRef( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPiece* pPiece )
{
	m_mapPieceInstancingSwap[0]->Import_AddRef();
	m_mapPieceInstancingSwap[1]->Import_AddRef();
	pPiece->SetInstancingLink( emCullingRenderType, this );
	m_vecrPiece.push_back( pPiece );
}

template <typename T>
BOOL PieceInstancingSwap<T>::CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_mapPieceInstancingSwap[0]->CreateInstancingLOD( pd3dDevice ) )
		return FALSE;

	if ( !m_mapPieceInstancingSwap[1]->CreateInstancingLOD( pd3dDevice ) )
		return FALSE;

	return TRUE;
}

template <typename T>
void PieceInstancingSwap<T>::ResetInstancingLinkForPiece()
{
	for ( DWORD i=0; i<m_vecrPiece.size(); ++i )
	{
		m_vecrPiece[i]->ResetInstancingLink();
	}
}

template <typename T>
void PieceInstancingSwap<T>::Swap()
{
	if ( m_nUSED == 0 )
	{
		m_nUSED = 1;
		m_nBackUp = 0;
	}
	else
	{
		m_nUSED = 0;
		m_nBackUp = 1;
	}
}

//////////////////////////////////////////////////////////////////////////
//							Thread
template <typename T>
void PieceInstancingSwap<T>::ResetRef_THREAD()
{
	m_mapPieceInstancingSwap[m_nBackUp]->ResetRef_THREAD();
}

template <typename T>
void PieceInstancingSwap<T>::AddRef_THREAD()
{
	m_mapPieceInstancingSwap[m_nBackUp]->AddRef_THREAD();
}

template <typename T>
void PieceInstancingSwap<T>::Reset_THREAD()
{
	return m_mapPieceInstancingSwap[m_nBackUp]->Reset_THREAD();
}

template <typename T>
void PieceInstancingSwap<T>::Insert_THREAD( const DxPiece* pPiece )
{
	m_mapPieceInstancingSwap[m_nBackUp]->Insert_THREAD( pPiece );
}

template <typename T>
void PieceInstancingSwap<T>::InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing )
{
	m_mapPieceInstancingSwap[m_nBackUp]->InsertDataToVB_ExceptPiece_THREAD( vecExceptInstancing );
}

template <typename T>
void PieceInstancingSwap<T>::InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight )
{
	m_mapPieceInstancingSwap[m_nBackUp]->InsertDataToVB_ExceptPiece_Reflect_THREAD( vecExceptInstancing, fHeight );
}

//////////////////////////////////////////////////////////////////////////
//								FrameMove
template <typename T>
void PieceInstancingSwap<T>::FrameMoveStep( float fElapsedTime )
{
	if ( !m_mapPieceInstancingSwap[m_nUSED]->FrameMoveStep( fElapsedTime ) )		
	{
		m_bDelete = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////////
//								Render
template <typename T>
void PieceInstancingSwap<T>::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_mapPieceInstancingSwap[m_nUSED]->RenderOpaque( pd3dDevice );
}

template <typename T>
void PieceInstancingSwap<T>::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_mapPieceInstancingSwap[m_nUSED]->RenderHardAlpha( pd3dDevice );
}

template <typename T>
void PieceInstancingSwap<T>::RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_mapPieceInstancingSwap[m_nUSED]->RenderDecal( pd3dDevice );
}

template <typename T>
void PieceInstancingSwap<T>::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_mapPieceInstancingSwap[m_nUSED]->RenderDecalPass2( pd3dDevice );
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//							PieceInstancingThread
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
template <typename T, typename T2>
PieceInstancingThread<T,T2>::PieceInstancingThread()
	: m_dwFrameMoveStepIndex(0)
{

}

template <typename T, typename T2>
PieceInstancingThread<T,T2>::~PieceInstancingThread()
{
	CleanUp();
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->OnResetDevice( pd3dDevice );
	}
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::OnLostDevice()
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->OnLostDevice();
	}
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::Import_MeshLM( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPieceAABB* pPieceAABB )
{
	for ( DWORD i=0; i<pPieceAABB->GetVecPiece().size(); ++i )
	{ 
		DxPiece* pPiece = pPieceAABB->GetVecPiece()[i];
		PIECE_INSTANCING_FIRST sFirst( pPiece->GetStaticAniFrame(), pPiece->GetLightMapCombined() );
		MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( sFirst );
		if ( iter == m_mapPieceInstancingSwap.end() )
		{
			boost::shared_ptr<PieceInstancingSwap<T2>> spSecond(new PieceInstancingSwap<T2>(pPiece->GetStaticAniFrame()));
			spSecond->Import_AddRef( emCullingRenderType, pPiece );
			m_mapPieceInstancingSwap.insert( std::make_pair( sFirst, spSecond ) );
		}
		else
		{
			(*iter).second->Import_AddRef( emCullingRenderType, pPiece );
		}
	}

	// Lod 단계들을 만든다.
	CreateInstancingLOD( pd3dDevice );
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::Import_PureMesh( LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, DxPieceAABB* pPieceAABB )
{
	for ( DWORD i=0; i<pPieceAABB->GetVecPiece().size(); ++i )
	{ 
		DxPiece* pPiece = pPieceAABB->GetVecPiece()[i];
		MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( pPiece->GetStaticAniFrame() );
		if ( iter == m_mapPieceInstancingSwap.end() )
		{
			boost::shared_ptr<PieceInstancingSwap<T2>> spSecond(new PieceInstancingSwap<T2>(pPiece->GetStaticAniFrame()));
			spSecond->Import_AddRef( emCullingRenderType, pPiece );
			m_mapPieceInstancingSwap.insert( std::make_pair( pPiece->GetStaticAniFrame(), spSecond ) );
		}
		else
		{
			(*iter).second->Import_AddRef( emCullingRenderType, pPiece );
		}
	}

	// Lod 단계들을 만든다.
	CreateInstancingLOD( pd3dDevice );
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 갯수가 1개라면 Instancing 을 하지 않는다.
	MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.begin();
	for ( ; iter!=m_mapPieceInstancingSwap.end(); )
	{
		if ( (*iter).second->CreateInstancingLOD( pd3dDevice ) )
		{
			++iter;
		}
		else
		{
			(*iter).second->ResetInstancingLinkForPiece();
			iter = m_mapPieceInstancingSwap.erase( iter );
		}
	}
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::CleanUp()
{
	m_mapPieceInstancingSwap.clear();
}

//////////////////////////////////////////////////////////////////////////
//			m_mapPieceInstancingSwap 삭제가 가능한 안전지대
template <typename T, typename T2>
void PieceInstancingThread<T,T2>::Swap()
{
	MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.begin();
	for ( ; iter!=m_mapPieceInstancingSwap.end(); ++iter )
	{
		(*iter).second->Swap();
	}

	// Opaque 도 HardAlpha 도 없으면 삭제하도록 한다.
	iter = m_mapPieceInstancingSwap.begin();
	for ( ; iter!=m_mapPieceInstancingSwap.end();  )
	{
		if ( (*iter).second->IsDelete() )
		{
			(*iter).second->ResetInstancingLinkForPiece();
			iter = m_mapPieceInstancingSwap.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//								Thread
template <typename T, typename T2>
void PieceInstancingThread<T,T2>::ResetRef_THREAD()
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->ResetRef_THREAD();
	}
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::AddRefMeshLM_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece )
{
	//PIECE_INSTANCING_FIRST sFirst( pPiece->GetStaticAniFrame(), pPiece->GetLightMapCombined() );

	//MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( sFirst );
	//if ( iter != m_mapPieceInstancingSwap.end() )
	//{
	//	(*iter).second->AddRef_THREAD();
	//}

	PieceInstancingSwap<DxPieceInstancing>* pInstancingSwap( pPiece->GetInstancingLink( emCullingRenderType ) );
	if ( !pInstancingSwap )
		return;

	pInstancingSwap->AddRef_THREAD();
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::AddRefPureMesh_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece )
{
	//MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( pPiece->GetStaticAniFrame() );
	//if ( iter != m_mapPieceInstancingSwap.end() )
	//{
	//	(*iter).second->AddRef_THREAD();
	//}

	PieceInstancingSwap<DxPieceInstancingForShadow>* pInstancingSwap( pPiece->GetInstancingLinkForShadow( emCullingRenderType ) );
	if ( !pInstancingSwap )
		return;

	pInstancingSwap->AddRef_THREAD();
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::Reset_THREAD()
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->Reset_THREAD();
	}
}

template <typename T, typename T2>
BOOL PieceInstancingThread<T,T2>::InsertInstancingMeshLM_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece )
{
	//PIECE_INSTANCING_FIRST sFirst( pPiece->GetStaticAniFrame(), pPiece->GetLightMapCombined() );

	//MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( sFirst );
	//if ( iter == m_mapPieceInstancingSwap.end() )
	//{
	//	return FALSE;
	//}

	//(*iter).second->Insert_THREAD( pPiece );

	PieceInstancingSwap<DxPieceInstancing>* pInstancingSwap( pPiece->GetInstancingLink( emCullingRenderType ) );
	if ( !pInstancingSwap )
		return FALSE;

	pInstancingSwap->Insert_THREAD( pPiece );

	return TRUE;
}

template <typename T, typename T2>
BOOL PieceInstancingThread<T,T2>::InsertInstancingPureMesh_THREAD( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const DxPiece* pPiece )
{
	//MAP_PIECEINSTANCINGSWAP::iterator iter = m_mapPieceInstancingSwap.find( pPiece->GetStaticAniFrame() );
	//if ( iter == m_mapPieceInstancingSwap.end() )
	//{
	//	return FALSE;
	//}

	//(*iter).second->Insert_THREAD( pPiece );

	PieceInstancingSwap<DxPieceInstancingForShadow>* pInstancingSwap( pPiece->GetInstancingLinkForShadow( emCullingRenderType ) );
	if ( !pInstancingSwap )
		return FALSE;

	pInstancingSwap->Insert_THREAD( pPiece );

	return TRUE;
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->InsertDataToVB_ExceptPiece_THREAD( vecExceptInstancing );
	}
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->InsertDataToVB_ExceptPiece_Reflect_THREAD( vecExceptInstancing, fHeight );
	}
}

//////////////////////////////////////////////////////////////////////////
//								FrameMove
template <typename T, typename T2>
void PieceInstancingThread<T,T2>::FrameMoveStep( float fElapsedTime )
{
	//++m_dwFrameMoveStepIndex;
	//if ( m_dwFrameMoveStepIndex >= m_mapPieceInstancingSwap.size() )
	//{
	//	m_dwFrameMoveStepIndex = 0;
	//}

	//DWORD dwCount(0);
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		//if ( dwCount == m_dwFrameMoveStepIndex )
		//{
			val.second->FrameMoveStep( fElapsedTime );
		//	break;
		//}
		//++dwCount;
	}
}

//////////////////////////////////////////////////////////////////////////
//								Render
template <typename T, typename T2>
void PieceInstancingThread<T,T2>::RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->RenderOpaque( pd3dDevice );
	}

	pd3dDevice->SetStreamSource( 1, NULL, 0, 0 );

	pd3dDevice->SetStreamSourceFreq( 0, 1 );
	pd3dDevice->SetStreamSourceFreq( 1, 1 );
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->RenderHardAlpha( pd3dDevice );
	}

	pd3dDevice->SetStreamSource( 1, NULL, 0, 0 );

	pd3dDevice->SetStreamSourceFreq( 0, 1 );
	pd3dDevice->SetStreamSourceFreq( 1, 1 );
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->RenderDecal( pd3dDevice );
	}

	pd3dDevice->SetStreamSource( 1, NULL, 0, 0 );

	pd3dDevice->SetStreamSourceFreq( 0, 1 );
	pd3dDevice->SetStreamSourceFreq( 1, 1 );
}

template <typename T, typename T2>
void PieceInstancingThread<T,T2>::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOST_FOREACH( MAP_PIECEINSTANCINGSWAP::value_type& val, m_mapPieceInstancingSwap )
	{
		val.second->RenderDecalPass2( pd3dDevice );
	}

	pd3dDevice->SetStreamSource( 1, NULL, 0, 0 );

	pd3dDevice->SetStreamSourceFreq( 0, 1 );
	pd3dDevice->SetStreamSourceFreq( 1, 1 );
}
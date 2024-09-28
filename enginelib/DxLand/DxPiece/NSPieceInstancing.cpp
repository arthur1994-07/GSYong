#include "pch.h"

#include <map>
#include <boost/tr1/memory.hpp>

#include "../../DxTools/Material/NSShadowRenderForMaterial.h"
#include "./DxPieceInstancingForShadow.h"

#include "NSPieceInstancing.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//CPieceInstancing::CPieceInstancing()
//{
//
//}
//
//CPieceInstancing::~CPieceInstancing()
//{
//
//}
//
//void CPieceInstancing::CleanUp()
//{
//	m_mapPieceInstancing.clear();
//}
//
//void CPieceInstancing::Init( DxStaticAniFrame* pStaticAniFrame, const PieceParameter* pParameter )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.find( pStaticAniFrame );
//	if ( iter == m_mapPieceInstancing.end() )
//	{
//		// 새로운 것을 추가하도록 한다.
//		std::tr1::shared_ptr<DxPieceInstancingForShadow> spNew = std::tr1::shared_ptr<DxPieceInstancingForShadow>( new DxPieceInstancingForShadow(pStaticAniFrame) );
//		m_mapPieceInstancing.insert( std::make_pair( pStaticAniFrame, spNew ) );
//
//		spNew->Insert( pParameter );
//	}
//	else
//	{
//		// 몇개나 셋팅되었는지 확인한다.
//		(*iter).second->AddRef();
//		(*iter).second->Insert( pParameter );
//	}
//}
//
//void CPieceInstancing::CreateVB( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->CreateVB( pd3dDevice );
//	}
//}
//
//void CPieceInstancing::Insert( DxStaticAniFrame* pStaticAniFrame, const PieceParameter* pParameter )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.find( pStaticAniFrame );
//	if ( iter != m_mapPieceInstancing.end() )
//	{
//		(*iter).second->Insert( pParameter );
//	}
//}
//
//void CPieceInstancing::Reset()
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->Reset();
//	}
//}
//
//void CPieceInstancing::InsertDataToVB()
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->InsertDataToVB();
//	}
//}
//
//void CPieceInstancing::RenderCascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->RenderCascadeShadowOpaque( pd3dDevice );
//	}
//}
//
//void CPieceInstancing::RenderCascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->RenderCascadeShadowHardAlpha( pd3dDevice );
//	}
//}
//
//void CPieceInstancing::RenderCascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	NSShadowRenderForMaterial::SetWorld( matIdentity );
//
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->RenderCascadeShadowOpaque_Instancing( pd3dDevice );
//	}
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void CPieceInstancing::RenderCascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	NSShadowRenderForMaterial::SetWorld( matIdentity );
//
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->RenderCascadeShadowHardAlpha_Instancing( pd3dDevice );
//	}
//
//	pd3dDevice->SetStreamSourceFreq( 0, 1 );
//	pd3dDevice->SetStreamSourceFreq( 1, 1 );
//}
//
//void CPieceInstancing::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->OnResetDevice( pd3dDevice );
//	}
//}
//
//void CPieceInstancing::OnLostDevice()
//{
//	MAP_PIECE_INSTANCING_ITER iter = m_mapPieceInstancing.begin();
//	for ( ; iter != m_mapPieceInstancing.end(); ++iter )
//	{
//		(*iter).second->OnLostDevice();
//	}
//}
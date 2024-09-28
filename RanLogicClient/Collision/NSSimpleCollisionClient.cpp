#include "pch.h"

#include "../../enginelib/NaviMesh/Collision/SimpleCollision.h"
#include "../../enginelib/NaviMesh/Collision/LayerCollision.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../enginelib/DxTools/EditMeshs.h"
#include "../../enginelib/DxTools/DebugSet.h"

#include "NSSimpleCollisionClient.h"


namespace NSSimpleCollisionClient
{
	//////////////////////////////////////////////////////////////////////////
	//						SimpleCollisionPlane
	//						Y 는 무한대로 뻗어 있는 Plane
	//////////////////////////////////////////////////////////////////////////
	void RenderPlane( LPDIRECT3DDEVICEQ pd3dDevice, const SimpleCollisionPlane* pSrc )
	{
		if ( !pd3dDevice )
			return;

		D3DXVECTOR3 vPos[4];
		vPos[0] = D3DXVECTOR3 ( pSrc->GetMinusX(), 10000.f, 0.f );
		vPos[1] = D3DXVECTOR3 ( pSrc->GetPlusX(), 10000.f, 0.f );
		vPos[2] = D3DXVECTOR3 ( pSrc->GetMinusX(), -10000.f, 0.f );
		vPos[3] = D3DXVECTOR3 ( pSrc->GetPlusX(), -10000.f, 0.f );

		D3DXMATRIX matRotateY;
		D3DXMatrixRotationY( &matRotateY, D3DX_PI*pSrc->GetRotationY()/180 );
		matRotateY._41 = pSrc->GetPosition().x;
		matRotateY._42 = pSrc->GetPosition().y;
		matRotateY._43 = pSrc->GetPosition().z;

		for ( DWORD i=0; i<4; ++i )
		{
			D3DXVec3TransformCoord( &vPos[i], &vPos[i], &matRotateY );
		}

		EDITMESHS::RENDERPLANE( pd3dDevice, vPos, 0xffffffff, TRUE );
		EDITMESHS::RENDERPLANE( pd3dDevice, vPos, 0x80ffffff );
	}


	//////////////////////////////////////////////////////////////////////////
	//						SimpleCollisionBox
	//						Y가 제외된 4면인 박스
	//////////////////////////////////////////////////////////////////////////
	void RenderBox( LPDIRECT3DDEVICEQ pd3dDevice, const SimpleCollisionBox* pSrc )
	{
		if ( !pd3dDevice )
			return;

		D3DXVECTOR3 vPos[4][4];

		//////////////////////////////////////////////////////////////////////////
		vPos[0][0] = D3DXVECTOR3 ( pSrc->GetMinusX(), 10000.f, pSrc->GetMinusZ() );
		vPos[0][1] = D3DXVECTOR3 ( pSrc->GetPlusX(), 10000.f, pSrc->GetMinusZ() );
		vPos[0][2] = D3DXVECTOR3 ( pSrc->GetMinusX(), -10000.f, pSrc->GetMinusZ() );
		vPos[0][3] = D3DXVECTOR3 ( pSrc->GetPlusX(), -10000.f, pSrc->GetMinusZ() );

		D3DXMATRIX matRotateY;
		D3DXMatrixRotationY( &matRotateY, D3DX_PI*pSrc->GetRotationY()/180 );
		matRotateY._41 = pSrc->GetPosition().x;
		matRotateY._42 = pSrc->GetPosition().y;
		matRotateY._43 = pSrc->GetPosition().z;

		for ( DWORD i=0; i<4; ++i )
		{
			D3DXVec3TransformCoord( &vPos[0][i], &vPos[0][i], &matRotateY );
		}

		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[0], 0xffffffff, TRUE );

		//////////////////////////////////////////////////////////////////////////
		vPos[1][0] = D3DXVECTOR3 ( pSrc->GetMinusX(), 10000.f, pSrc->GetPlusZ() );
		vPos[1][1] = D3DXVECTOR3 ( pSrc->GetPlusX(), 10000.f, pSrc->GetPlusZ() );
		vPos[1][2] = D3DXVECTOR3 ( pSrc->GetMinusX(), -10000.f, pSrc->GetPlusZ() );
		vPos[1][3] = D3DXVECTOR3 ( pSrc->GetPlusX(), -10000.f, pSrc->GetPlusZ() );

		for ( DWORD i=0; i<4; ++i )
		{
			D3DXVec3TransformCoord( &vPos[1][i], &vPos[1][i], &matRotateY );
		}

		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[1], 0xffffffff, TRUE );

		//////////////////////////////////////////////////////////////////////////
		vPos[2][0] = D3DXVECTOR3 ( pSrc->GetMinusX(), 10000.f, pSrc->GetMinusZ() );
		vPos[2][1] = D3DXVECTOR3 ( pSrc->GetMinusX(), 10000.f, pSrc->GetPlusZ() );
		vPos[2][2] = D3DXVECTOR3 ( pSrc->GetMinusX(), -10000.f, pSrc->GetMinusZ() );
		vPos[2][3] = D3DXVECTOR3 ( pSrc->GetMinusX(), -10000.f, pSrc->GetPlusZ() );

		for ( DWORD i=0; i<4; ++i )
		{
			D3DXVec3TransformCoord( &vPos[2][i], &vPos[2][i], &matRotateY );
		}

		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[2], 0xffffffff, TRUE );

		//////////////////////////////////////////////////////////////////////////
		vPos[3][0] = D3DXVECTOR3 ( pSrc->GetPlusX(), 10000.f, pSrc->GetMinusZ() );
		vPos[3][1] = D3DXVECTOR3 ( pSrc->GetPlusX(), 10000.f, pSrc->GetPlusZ() );
		vPos[3][2] = D3DXVECTOR3 ( pSrc->GetPlusX(), -10000.f, pSrc->GetMinusZ() );
		vPos[3][3] = D3DXVECTOR3 ( pSrc->GetPlusX(), -10000.f, pSrc->GetPlusZ() );

		for ( DWORD i=0; i<4; ++i )
		{
			D3DXVec3TransformCoord( &vPos[3][i], &vPos[3][i], &matRotateY );
		}

		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[3], 0xffffffff, TRUE );

		// wireframe
		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[0], 0x80ffffff );
		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[1], 0x80ffffff );
		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[2], 0x80ffffff );
		EDITMESHS::RENDERPLANE( pd3dDevice, vPos[3], 0x80ffffff );
	}


	//////////////////////////////////////////////////////////////////////////
	//						SimpleCollisionCylinder
	//						Y 는 무한대로 뻗어 있는 Cylinder
	//////////////////////////////////////////////////////////////////////////
	void RenderCylinder( LPDIRECT3DDEVICEQ pd3dDevice, const SimpleCollisionCylinder* pSrc )
	{
		D3DXMATRIX matRotateX;
		D3DXMatrixRotationX( &matRotateX, D3DX_PI*0.5f );

		EDITMESHS::RENDERCYLINDER( pSrc->GetPosition(), pSrc->GetRadius(), 10000.f, 0xffffffff, &matRotateX );
		EDITMESHS::RENDERCYLINDER( pSrc->GetPosition(), pSrc->GetRadius(), 10000.f, 0x80ffffff, &matRotateX, FALSE );
	}


	//////////////////////////////////////////////////////////////////////////
	//						Render
	//////////////////////////////////////////////////////////////////////////
	void RenderB( LPDIRECT3DDEVICEQ pd3dDevice, const SimpleCollisionBase* pSrc )
	{
		switch( pSrc->GetType() )
		{
		case EMSCT_PLANE:
			RenderPlane( pd3dDevice, MIN_BOOST::polymorphic_downcast<const SimpleCollisionPlane*>( pSrc ) );
			break;

		case EMSCT_BOX:
			RenderBox( pd3dDevice, MIN_BOOST::polymorphic_downcast<const SimpleCollisionBox*>( pSrc ) );
			break;

		case EMSCT_CYLINDER:
			RenderCylinder( pd3dDevice, MIN_BOOST::polymorphic_downcast<const SimpleCollisionCylinder*>( pSrc ) );
			break;

		default:
			break;
		};
	}

	void RenderA( LPDIRECT3DDEVICEQ pd3dDevice, const LayerCollision* pSrc )
	{
		if ( !pSrc->IsActiveLayer() )
			return;

		const std::vector<boost::shared_ptr<SimpleCollisionBase>>& vecSimpleCollision = pSrc->GetvecSimpleCollision();

		for ( DWORD i=0; i<vecSimpleCollision.size(); ++i )
		{
			RenderB( pd3dDevice, vecSimpleCollision[i].get() );
		}
	}

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<boost::shared_ptr<LayerCollision>>& vecSrc )
	{
		for ( DWORD i=0; i<vecSrc.size(); ++i )
		{
			RenderA( pd3dDevice, vecSrc[i].get() );	
		}
	}



	//////////////////////////////////////////////////////////////////////////
	//						RenderCollisionName
	//////////////////////////////////////////////////////////////////////////
	void RenderCollisionNameB( LPDIRECT3DDEVICEQ pd3dDevice, const SimpleCollisionBase* pSrc )
	{
		D3DXVECTOR3 vOut;
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		D3DXVec3Project( &vOut, &pSrc->GetPosition(), &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
			&DxViewPort::GetInstance().GetMatView(), &matIdentity );
		CDebugSet::ToPos( vOut.x, vOut.y, "%s", pSrc->GetName() );
	}

	void RenderCollisionNameA( LPDIRECT3DDEVICEQ pd3dDevice, const LayerCollision* pSrc )
	{
		const std::vector<boost::shared_ptr<SimpleCollisionBase>>& vecSimpleCollision = pSrc->GetvecSimpleCollision();

		for ( DWORD i=0; i<vecSimpleCollision.size(); ++i )
		{
			NSSimpleCollisionClient::RenderCollisionNameB( pd3dDevice, vecSimpleCollision[i].get() );
		}
	}

	void RenderCollisionName( LPDIRECT3DDEVICEQ pd3dDevice, const std::vector<boost::shared_ptr<LayerCollision>>& vecSrc, size_t unLayer )
	{
		if ( unLayer >= vecSrc.size() )
			return;

		RenderCollisionNameA( pd3dDevice, vecSrc[unLayer].get() );
	}
}
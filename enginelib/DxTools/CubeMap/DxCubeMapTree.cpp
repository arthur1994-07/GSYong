#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../NsBakeCommon.h"
#include "../EditMeshs.h"
#include "../Collision.h"
#include "./NSCubeMapUtil.h"

#include "DxCubeMapTree.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


DxCubeMapTree::DxCubeMapTree()
	: m_pCubeTex(NULL)
	, m_vMax(1.f,1.f,1.f)
	, m_vMin(0.f,0.f,0.f)
	, m_pLeftChild(NULL)
	, m_pRightChild(NULL)
{
}

DxCubeMapTree::~DxCubeMapTree()
{
	SAFE_RELEASE ( m_pCubeTex );

	SAFE_DELETE( m_pLeftChild );
	SAFE_DELETE( m_pRightChild );
}

void DxCubeMapTree::LastBoxNum( DWORD& dwBoxNum )
{
	if ( !m_vecPoint.empty() )
	{
		++dwBoxNum;
		//dwBoxNum += m_vecPoint.size();
	}

	if ( m_pLeftChild )
		m_pLeftChild->LastBoxNum( dwBoxNum );
	if ( m_pRightChild )
		m_pRightChild->LastBoxNum( dwBoxNum );
}

void DxCubeMapTree::RenderBox( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	if ( !m_vecPoint.empty() )
	{
		EDITMESHS::RENDERAABB( pd3dDevice, m_vMax, m_vMin );
	}

	if ( m_pLeftChild )
		m_pLeftChild->RenderBox( pd3dDevice, vPos );
	if ( m_pRightChild )
		m_pRightChild->RenderBox( pd3dDevice, vPos );
}

void DxCubeMapTree::Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
						 DxLandMan* pDxLandMan, 
						 DWORD dwCubeMapSize,
						 int& nCountCUR )
{
	BakeOneCell
	( 
		pd3dDevice, 
		pDxLandMan,
		dwCubeMapSize
	);

	if ( !m_vecPoint.empty() )
	{
		++nCountCUR;
	}

	if ( m_pLeftChild )
		m_pLeftChild->Bake( pd3dDevice, pDxLandMan, dwCubeMapSize, nCountCUR );
	if ( m_pRightChild )
		m_pRightChild->Bake( pd3dDevice, pDxLandMan, dwCubeMapSize, nCountCUR );
}

void DxCubeMapTree::BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
								  DxLandMan* pDxLandMan, 
								  DWORD dwCubeMapSize )
{
	if ( m_vecPoint.empty() )
		return;

	// 강제로 Bake를 껐다면 렌더링 안하도록 작업한다.
	if ( NSBakeCommon::g_bForceDisable )
		return;

	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	for ( DWORD i=0; i<m_vecPoint.size(); ++i )
	{
		vPos += m_vecPoint[i];
	}
	vPos /= static_cast<float>( m_vecPoint.size() );
	vPos.y += 15.f;				// 카메라 Lookat 은 지면에서 15.f(1.5m) 만큼 위쪽에 있다.

	NSCubeMapUtil::CreateLocalCubeMapForGlobal( pd3dDevice, dwCubeMapSize, vPos, pDxLandMan, m_pCubeTex );
}

void DxCubeMapTree::GetCubeMapTexture( LPDIRECT3DCUBETEXTUREQ& pCubeTexOUT, const D3DXVECTOR3& vPos )
{
	if ( !COLLISION::IsWithInPoint ( m_vMax, m_vMin, vPos ) )
		return;

	if ( m_pCubeTex )
	{
		pCubeTexOUT = m_pCubeTex;
		return;
	}

	//D3DXVECTOR3 vPosNEW(0.f,0.f,0.f);
	//for ( DWORD i=0; i<m_vecPoint.size(); ++i )
	//{
	//	vPosNEW += m_vecPoint[i];
	//}
	//vPosNEW /= static_cast<float>( m_vecPoint.size() );
	//vPosNEW.y += 10.f;				// 카메라 Lookat 은 지면에서 10.f 만큼 위쪽에 있다.

	//EDITMESHS::RENDERSPHERE( vPosNEW, D3DXVECTOR3(1.f,1.f,1.f) );

	if ( m_pLeftChild )
		m_pLeftChild->GetCubeMapTexture( pCubeTexOUT, vPos );
	if ( m_pRightChild )
		m_pRightChild->GetCubeMapTexture( pCubeTexOUT, vPos );
}

void DxCubeMapTree::SaveMain( sc::SerialFile& SFile, const TSTRING& strMakeTimeWLD )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << strMakeTimeWLD;

		Save( SFile );
	}
	SFile.EndBlock();
}

void DxCubeMapTree::Save( sc::SerialFile& SFile )
{
	SFile << m_vMax;
	SFile << m_vMin;

	if ( m_pCubeTex )
	{
		LPD3DXBUFFER pBuffer = NULL;
		if( D3DXSaveTextureToFileInMemory ( &pBuffer, D3DXIFF_DDS, m_pCubeTex, NULL ) == D3D_OK )
		{
			SFile << static_cast<BOOL>( TRUE );
			SFile << static_cast<DWORD>( pBuffer->GetBufferSize() );
			SFile.WriteBuffer( pBuffer->GetBufferPointer(), pBuffer->GetBufferSize() );
			SAFE_RELEASE( pBuffer );
		}
		else
		{
			SFile << static_cast<BOOL>( FALSE );
		}
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}

	if ( m_pLeftChild )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pLeftChild->Save( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}

	if ( m_pRightChild )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pRightChild->Save( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}
}

void DxCubeMapTree::LoadMain( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, TSTRING& strMakeTimeWLD, DWORD dwCubeMapSize )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == 0x100 )
	{
		SFile >> strMakeTimeWLD;

		Load( pd3dDevice, SFile, dwCubeMapSize );
	}
}

void DxCubeMapTree::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwCubeMapSize )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		DWORD dwDataSize;
		SFile >> dwDataSize;

		BYTE* pData = new BYTE[dwDataSize];
		SFile.ReadBuffer( pData, dwDataSize );

		D3DXIMAGE_INFO imageInfo;
		D3DXCreateCubeTextureFromFileInMemoryEx
		(
			pd3dDevice,
			pData,
			dwDataSize,
			dwCubeMapSize,
			1,
			0,
			D3DFMT_DXT1,
			D3DPOOL_MANAGED,
			D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			D3DX_FILTER_BOX|D3DX_FILTER_MIRROR,
			0,
			&imageInfo,
			NULL,
			&m_pCubeTex 
		);

		SAFE_DELETE_ARRAY ( pData );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pLeftChild = new DxCubeMapTree;
		m_pLeftChild->Load( pd3dDevice, SFile, dwCubeMapSize );
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pRightChild = new DxCubeMapTree;
		m_pRightChild->Load( pd3dDevice, SFile, dwCubeMapSize );
	}
}
#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../NaviMesh/navigationmesh.h"
#include "../../DxLand/DxLandMan.h"
#include "../NsBakeCommon.h"
#include "../DxViewPort.h"
#include "../EditMeshs.h"
#include "./NSCubeMapUtil.h"

#include "./DxCubeMapTree.h"

#include "NSCubeMapCulling.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL					g_bWORLD_TOOL;

namespace NSNavigationCullingCommon
{
	void SeperaterTriangle( std::vector<D3DXVECTOR3>& vecPositionSmall, D3DXVECTOR3 vPos[], int nSimulationLength )
	{
		D3DXVECTOR3 vDir[3];
		vDir[0] = vPos[2] - vPos[1];
		vDir[1] = vPos[2] - vPos[0];
		vDir[2] = vPos[0] - vPos[1];

		float fLength0 = D3DXVec3Length( &vDir[0] );
		float fLength1 = D3DXVec3Length( &vDir[1] );
		float fLength2 = D3DXVec3Length( &vDir[2] );
		if ( (fLength0 > nSimulationLength) || (fLength1 > nSimulationLength) || (fLength2 > nSimulationLength) )
		{
			D3DXVECTOR3 vMid[3];
			vMid[0] = (vPos[2] + vPos[1]) * 0.5f;
			vMid[1] = (vPos[2] + vPos[0]) * 0.5f;
			vMid[2] = (vPos[0] + vPos[1]) * 0.5f;

			// 4개의 삼각형으로 만든다.
			D3DXVECTOR3 vPosSmall[3];

			vPosSmall[0] = vPos[0];
			vPosSmall[1] = vMid[1];
			vPosSmall[2] = vMid[2];
			SeperaterTriangle( vecPositionSmall, &vPosSmall[0], nSimulationLength );

			vPosSmall[0] = vMid[0];
			vPosSmall[1] = vMid[1];
			vPosSmall[2] = vMid[2];
			SeperaterTriangle( vecPositionSmall, &vPosSmall[0], nSimulationLength );

			vPosSmall[0] = vPos[1];
			vPosSmall[1] = vMid[0];
			vPosSmall[2] = vMid[2];
			SeperaterTriangle( vecPositionSmall, &vPosSmall[0], nSimulationLength );

			vPosSmall[0] = vPos[2];
			vPosSmall[1] = vMid[1];
			vPosSmall[2] = vMid[0];
			SeperaterTriangle( vecPositionSmall, &vPosSmall[0], nSimulationLength );
		}
		else
		{
			// 정점을 등록한다.
			vecPositionSmall.push_back( vPos[0] );
			vecPositionSmall.push_back( vPos[1] );
			vecPositionSmall.push_back( vPos[2] );
		}
	}

	void SimulationPosition( NavigationMesh* pNavigationMesh, std::vector<D3DXVECTOR3>& vecPoint, int nSimulationLength )
	{
		vecPoint.clear();

		if ( !pNavigationMesh )
			return;

		std::vector<D3DXVECTOR3> vecPosition;
		pNavigationMesh->GetVertexData( vecPosition );

		D3DXVECTOR3 vPos[3];
		DWORD dwFaceNum = vecPosition.size() / 3;
		for ( DWORD i=0; i<dwFaceNum; ++i )
		{
			vPos[0] = vecPosition[i*3+0];
			vPos[1] = vecPosition[i*3+1];
			vPos[2] = vecPosition[i*3+2];

			// 사이즈에 맞는 삼각형 생성
			std::vector<D3DXVECTOR3> vecPositionSmall;
			SeperaterTriangle( vecPositionSmall, &vPos[0], nSimulationLength );

			// 정점들 생성
			DWORD dwPointNUM = vecPositionSmall.size() / 3;
			for ( DWORD j=0; j<dwPointNUM; ++j )
			{
				vecPoint.push_back( ( vecPositionSmall[j*3+0] + vecPositionSmall[j*3+1] + vecPositionSmall[j*3+2] ) / 3 );
			}
		}
	}
}

namespace NSCubeMapCulling
{
	enum
	{
		VERSION = 0x100,
	};

	float						g_fOctreeBoxSize(100.f);
	std::vector<D3DXVECTOR3>	g_vecSimulationPoint;		// 카메라 렌더링 위치를 구하기 위한 Point
	D3DXVECTOR3					g_vOctreeMax(0.f,0.f,0.f);
	D3DXVECTOR3					g_vOctreeMin(0.f,0.f,0.f);
	DxCubeMapTree*				g_pCubeMapTree(NULL);			// Save, Load에 필요함.
	int							g_nBakeCountCUR(0);
	DWORD						g_dwBakeCountALL(0L);
	TSTRING						g_strMakeTimeWLD;				// wld가 만들어진 시간.
	BOOL						m_bActiveCubeMap(FALSE);
	DWORD						m_dwCubeMapSize(64);			// CubeMap 사이즈

	void CleanUp()
	{
		SAFE_DELETE ( g_pCubeMapTree );
	}

	void SimulationPosition( NavigationMesh* pNavigationMesh, int nLength )
	{
		pNavigationMesh->GetAABB( g_vOctreeMax, g_vOctreeMin );

		// 카메라 Lookat 은 캐릭터 위치에서 10.f 높게 잡기 때문에 넉넉하게 100.f 넓게 한다.
		{
			g_vOctreeMax.x += 100.f;
			g_vOctreeMax.y += 100.f;
			g_vOctreeMax.z += 100.f;
			g_vOctreeMin.x -= 100.f;
			g_vOctreeMin.y -= 100.f;
			g_vOctreeMin.z -= 100.f;
		}

		NSNavigationCullingCommon::SimulationPosition( pNavigationMesh, g_vecSimulationPoint, nLength );
	}

	void DrawSimulationPoint( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		DWORD dwListCnt = g_vecSimulationPoint.size();
		D3DXVECTOR3* pVector = new D3DXVECTOR3[dwListCnt];
		for ( DWORD i=0; i<dwListCnt; ++i )
		{
			pVector[i] = g_vecSimulationPoint[i];
		}

		EDITMESHS::RENDERBOX_LIST( pVector, dwListCnt, 1.f, 0xffffffff );

		SAFE_DELETE_ARRAY( pVector );
	}

	void SetOctreeBoxSize( int nLength )
	{
		g_fOctreeBoxSize = static_cast<float>( nLength );
	}

	void DrawOctreeBox( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( g_pCubeMapTree )
		{
			g_pCubeMapTree->RenderBox( pd3dDevice, DxViewPort::GetInstance().GetLookatPt() );
		}
		else
		{
			D3DXVECTOR3* pVector = new D3DXVECTOR3[10*10];
			for ( int x=0; x<10; ++x )
			{
				for ( int z=0; z<10; ++z )
				{
					pVector[(x*10)+z] = D3DXVECTOR3( (x-5)*g_fOctreeBoxSize, 0.f, (z-5)*g_fOctreeBoxSize );
					pVector[(x*10)+z] += DxViewPort::GetInstance().GetLookatPt();
				}
			}

			DWORD dwListCnt = 100;
			EDITMESHS::RENDERBOX_LIST( pVector, dwListCnt, g_fOctreeBoxSize, 0xffffffff );

			SAFE_DELETE_ARRAY( pVector );
		}
	}

	BOOL Bake( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxLandMan* pDxLandMan, 
				int nSumulationDetailLength, 
				int nOctreeBoxLength,
				DWORD dwCubeMapSize )
	{
		m_dwCubeMapSize = dwCubeMapSize;
		NSCubeMapUtil::NSCubeMapBlend::CleanUp();
		NSCubeMapUtil::NSCubeMapBlend::OnNewSizeDevice( pd3dDevice, dwCubeMapSize );

		// NSOcclusionCulling 쪽을 참조하여서 만듬.
		NSBakeCommon::g_bBaking = TRUE;
		NSBakeCommon::g_bForceDisable = FALSE;

		boost::shared_ptr<NavigationMesh> spNavigationMesh = pDxLandMan->GetNaviMesh();
		if ( !spNavigationMesh )
		{
			AfxMessageBox("NavigationMesh 가 존재하지 않습니다.");
			return FALSE;
		}

		SimulationPosition( spNavigationMesh.get(), nSumulationDetailLength );
		NSCubeMapMaker::MakeTree( &g_pCubeMapTree, g_vecSimulationPoint, g_vOctreeMax, g_vOctreeMin, static_cast<float>(nOctreeBoxLength) );
		if ( !g_pCubeMapTree )
		{
			AfxMessageBox("CubeMapTree 가 생성되지 않았습니다.");
			return FALSE;
		}

		g_nBakeCountCUR = 0;
		g_dwBakeCountALL = 0;
		g_pCubeMapTree->LastBoxNum( g_dwBakeCountALL );

		g_pCubeMapTree->Bake
		( 
			pd3dDevice, 
			pDxLandMan,
			dwCubeMapSize,
			g_nBakeCountCUR
		);

		// 강제로 Bake를 껐다면 리턴값을 FALSE로 준다.
		if ( NSBakeCommon::g_bForceDisable )
		{
			NSBakeCommon::g_bBaking = FALSE;
			NSBakeCommon::g_bForceDisable = TRUE;

			return FALSE;
		}

		// NSOcclusionCulling 쪽을 참조하여서 만듬.
		NSBakeCommon::g_bBaking = FALSE;
		NSBakeCommon::g_bForceDisable = TRUE;

		return TRUE;
	}

	LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture( const D3DXVECTOR3& vPos )
	{
		if ( !g_pCubeMapTree )
			return NULL;

		// Bake 중이라면 CubeMap을 얻어오지는 않는다.
		if ( NSBakeCommon::g_bBaking )
			return NULL;

		LPDIRECT3DCUBETEXTUREQ pTexture(NULL);
		g_pCubeMapTree->GetCubeMapTexture( pTexture, vPos );
		return pTexture;
	}

	void Save( const TSTRING& strMakeTimeWLD, const char* szName )
	{
		if ( !g_pCubeMapTree )
			return;

		std::string strName = sc::util::ChangeExtName( szName, _T("cbm") );

		sc::SerialFile SFile;
		if ( SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )
		{
			SFile << VERSION;
			SFile.BeginBlock();
			{
				// 큐브맵 사이즈 저장.
				SFile << m_dwCubeMapSize;

				g_pCubeMapTree->SaveMain( SFile, strMakeTimeWLD );
			}
			SFile.EndBlock();
			SFile.CloseFile();
		}
	}

	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
	{
		NSCubeMapUtil::NSCubeMapBlend::CleanUp();
		SAFE_DELETE ( g_pCubeMapTree );

		std::string strName = sc::util::ChangeExtName( szName, _T("cbm") );

		sc::SerialFile SFile;
		if ( SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		{
			DWORD dwVer, dwBufferSize;
			SFile >> dwVer;
			SFile >> dwBufferSize;

			// 큐브맵 사이즈 조정.
			SFile >> m_dwCubeMapSize;
			NSCubeMapUtil::NSCubeMapBlend::OnNewSizeDevice( pd3dDevice, m_dwCubeMapSize );

			SAFE_DELETE ( g_pCubeMapTree );
			g_pCubeMapTree = new DxCubeMapTree;
			g_pCubeMapTree->LoadMain( pd3dDevice, SFile, g_strMakeTimeWLD, m_dwCubeMapSize );
			SFile.CloseFile();
		}
	}

	void IntegrityCheck( const TSTRING& strMakeTimeWLD )
	{
		if ( !m_bActiveCubeMap )
			return;

		if ( g_strMakeTimeWLD != strMakeTimeWLD )
		{
			if ( g_bWORLD_TOOL )
			{
				AfxMessageBox( _T("wld 데이터가 최신으로 업데이트 되어서 OcclusionData 가 비정상 작동할 수 있습니다..") );
			}

			CDebugSet::ToLogFile( sc::string::format( _T("wld 데이터가 최신으로 업데이트 되어서 %s 가 비정상 작동할 수 있습니다.."),
				strMakeTimeWLD.c_str() ) );
		}
	}
}

namespace NSCubeMapMaker
{
	template<class TREE>
	BOOL OptimizeDivide( TREE* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, int& nDivide, float& fHalf )
	{
		float fLengthX = vMax.x - vMin.x;
		float fLengthY = vMax.y - vMin.y;
		float fLengthZ = vMax.z - vMin.z;
		fLengthX = fabsf(fLengthX);
		fLengthY = fabsf(fLengthY);
		fLengthZ = fabsf(fLengthZ);

		float fHalfX = (vMax.x + vMin.x)*0.5f;
		float fHalfY = (vMax.y + vMin.y)*0.5f;
		float fHalfZ = (vMax.z + vMin.z)*0.5f;

		// 가장 긴 선분을 분할 한다.
		nDivide = 0;
		if( fLengthX>=fLengthY && fLengthX>=fLengthZ )	nDivide = 1;
		else if( fLengthY>=fLengthZ )					nDivide = 2;
		else											nDivide = 3;

		int nLeft = 0;
		int nRight = 0;
		D3DXVECTOR3 vMaxAABB, vMinAABB;
		for (size_t i=0; i<pTree->m_vecPoint.size(); ++i )
		{
			D3DXVECTOR3 vPos = pTree->m_vecPoint[i];
			switch ( nDivide )
			{
			case 1:
				{
					fHalf = fHalfX;
					if( vPos.x > fHalfX )	++nRight;
					else					++nLeft;
				}
				break;

			case 2:
				{
					fHalf = fHalfY;
					if( vPos.y > fHalfY )	++nRight;
					else					++nLeft;
				}
				break;

			case 3:
				{
					fHalf = fHalfZ;
					if( vPos.z > fHalfZ )	++nRight;
					else					++nLeft;
				}
				break;
			};
		}

		// 균일하게 분할이 안되어서 그만 만든다.
		if( !nRight || !nLeft )		return FALSE;

		// Note : 작업
		pTree->m_pLeftChild = new TREE;
		pTree->m_pRightChild = new TREE;
		for (size_t i=0; i<pTree->m_vecPoint.size(); ++i)
		{
			D3DXVECTOR3 vPos = pTree->m_vecPoint[i];
			switch( nDivide )
			{
			case 1:
				if( vPos.x > fHalfX )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			case 2:
				if( vPos.y > fHalfY )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			case 3:
				if( vPos.z > fHalfZ )
				{
					pTree->m_pRightChild->m_vecPoint.push_back( vPos );
				}
				else
				{
					pTree->m_pLeftChild->m_vecPoint.push_back( vPos );
				}
				break;
			};
		}

		// Note : 옮겨가서 지워준다.
		pTree->m_vecPoint.clear();

		return TRUE;
	}

	template<class TREE>
	void MakeTreeChild( TREE* pTree, float fSize )
	{
		D3DXVECTOR3 vMaxPoint = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinPoint = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		for ( size_t i=0; i<pTree->m_vecPoint.size(); ++i )
		{
			const D3DXVECTOR3& vPos = pTree->m_vecPoint[i];
			if( vMaxPoint.x < vPos.x )	vMaxPoint.x = vPos.x;
			if( vMaxPoint.y < vPos.y )	vMaxPoint.y = vPos.y;
			if( vMaxPoint.z < vPos.z )	vMaxPoint.z = vPos.z;

			if( vMinPoint.x > vPos.x )	vMinPoint.x = vPos.x;
			if( vMinPoint.y > vPos.y )	vMinPoint.y = vPos.y;
			if( vMinPoint.z > vPos.z )	vMinPoint.z = vPos.z;
		}

		// FALSE 일 경우 더 이상 분할 불가
		int nDivide(0);
		float fHalf(0.f);
		float fLength(0.f);
		if( !OptimizeDivide( pTree, vMaxPoint, vMinPoint, nDivide, fHalf ) )
			return;

		// Left Right AABB 박스를 생성해준다.
		pTree->m_pLeftChild->m_vMax		= pTree->m_vMax;
		pTree->m_pLeftChild->m_vMin		= pTree->m_vMin;
		pTree->m_pRightChild->m_vMax	= pTree->m_vMax;
		pTree->m_pRightChild->m_vMin	= pTree->m_vMin;
		switch( nDivide )
		{
		case 1:
			pTree->m_pLeftChild->m_vMax.x	= fHalf;
			pTree->m_pRightChild->m_vMin.x	= fHalf;
			pTree->m_pLeftChild->m_vMax.x	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.x	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.x	- fHalf;
			break;
		case 2:
			pTree->m_pLeftChild->m_vMax.y = fHalf;
			pTree->m_pRightChild->m_vMin.y = fHalf;
			pTree->m_pLeftChild->m_vMax.y	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.y	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.y	- fHalf;
			break;
		case 3:
			pTree->m_pLeftChild->m_vMax.z = fHalf;
			pTree->m_pRightChild->m_vMin.z = fHalf;
			pTree->m_pLeftChild->m_vMax.z	+= 0.0001f;
			pTree->m_pRightChild->m_vMin.z	-= 0.0001f;
			fLength = pTree->m_pRightChild->m_vMax.z	- fHalf;
			break;
		}

		// 박스가 기준수치에 도달했다면 더 이상 분할 하지 않는다.
		if ( fLength < fSize )
		{
			return;
		}

		// 박스를 계속 해서 분할한다.
		MakeTreeChild( pTree->m_pLeftChild, fSize );
		MakeTreeChild( pTree->m_pRightChild, fSize );
	}

	template<class TREE>
	void MakeTree( TREE** ppTree, 
					const std::vector<D3DXVECTOR3>& vecPoint, 
					const D3DXVECTOR3& vMax, 
					const D3DXVECTOR3& vMin, 
					float fSize )
	{
		SAFE_DELETE( (*ppTree) );
		(*ppTree) = new TREE;
		(*ppTree)->m_vecPoint = vecPoint;

		// Note : 충돌을 위해서
		(*ppTree)->m_vMax	= vMax;
		(*ppTree)->m_vMin	= vMin;
		(*ppTree)->m_vMax	+= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		(*ppTree)->m_vMin	-= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		MakeTreeChild( (*ppTree), fSize );
	}
};
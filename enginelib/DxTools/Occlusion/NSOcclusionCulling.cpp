#include "pch.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../NaviMesh/navigationmesh.h"
#include "../../DxLand/DxLandMan.h"

#include "../DxViewPort.h"
#include "../EditMeshs.h"
#include "./DxOcclusionQuery.h"
#include "./DxOcclusionTree.h"

#include "NSOcclusionCulling.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL					g_bWORLD_TOOL;

namespace NSOcclusionCulling
{
	enum
	{
		VERSION = 0x100,
	};

	float g_fOctreeBoxSize(100.f);
	std::vector<D3DXVECTOR3> g_vecSimulationPoint;		// 카메라 렌더링 위치를 구하기 위한 Point
	std::vector<D3DXVECTOR3> g_vecCharSimulationPoint;	// 캐릭터 렌더링을 하기 위한 Point
	D3DXVECTOR3 g_vOctreeMax(0.f,0.f,0.f);
	D3DXVECTOR3 g_vOctreeMin(0.f,0.f,0.f);
	DxOcclusionTree*	g_pOcclusionTree(NULL);			// Save, Load에 필요함.
	BOOL				g_bRendering(TRUE);
	BOOL				g_bBaking(FALSE);			
	int					g_nBakeCountCUR(0);
	DWORD				g_dwBakeCountALL(0L);
	TSTRING				g_strMakeTimeWLD;				// wld가 만들어진 시간.
	BOOL				m_bFieldOcclusion(FALSE);
	BOOL				m_bPieceOcclusion(FALSE);
	BOOL				m_bCharOcclusion(FALSE);

	VEC_DWORD g_vecPieceIndex;
	VEC_DWORD g_vecOctreeIndex;
	VEC_DWORD g_vecOccCharZoneIndex[NSOcclusionCharZone::HEIGHT_TYPE_SIZE];

	void CleanUp()
	{
		SAFE_DELETE ( g_pOcclusionTree );

		NSOcclusionCharZone::CleanUp();
	}

	BOOL IsActive()
	{
		if ( g_pOcclusionTree && g_bRendering )
			return TRUE;
		else
			return FALSE;
	}

	BOOL IsActiveField()
	{
		if ( IsActive() && m_bFieldOcclusion )
			return TRUE;
		else
			return FALSE;
	}

	BOOL IsActivePiece()
	{
		if ( IsActive() && m_bPieceOcclusion )
			return TRUE;
		else
			return FALSE;
	}

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

	void SimulationPosition2( NavigationMesh* pNavigationMesh, std::vector<D3DXVECTOR3>& vecPoint, int nSimulationLength )
	{
		vecPoint.clear();

		if ( !pNavigationMesh )
			return;

		std::vector<D3DXVECTOR3> vecPosition;
		pNavigationMesh->GetVertexData( vecPosition );
		pNavigationMesh->GetAABB( g_vOctreeMax, g_vOctreeMin );

		// 카메라가 외부로 나갈 수도 있기 때문에 ZoomOut 거리까지 늘려준다.
		{
			g_vOctreeMax.x += 200.f;
			g_vOctreeMax.y += 200.f;
			g_vOctreeMax.z += 200.f;
			g_vOctreeMin.x -= 200.f;
			g_vOctreeMin.y -= 200.f;
			g_vOctreeMin.z -= 200.f;
		}

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

	void SimulationPosition( NavigationMesh* pNavigationMesh, int nLength )
	{
		SimulationPosition2( pNavigationMesh, g_vecSimulationPoint, nLength );
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
		if ( g_pOcclusionTree )
		{
			g_pOcclusionTree->RenderBox( pd3dDevice, DxViewPort::GetInstance().GetLookatPt() );
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
				BOOL bFieldOcclusion,
				BOOL bPieceOcclusion,
				BOOL bCharOcclusion )
	{
		boost::shared_ptr<NavigationMesh> spNavigationMesh = pDxLandMan->GetNaviMesh();
		if ( !spNavigationMesh )
		{
			AfxMessageBox("NavigationMesh 가 존재하지 않습니다.");
			return FALSE;
		}

		NSOcclusionCulling::SimulationPosition( spNavigationMesh.get(), nSumulationDetailLength );
		NSOcclusionCulling::SimulationPosition2( spNavigationMesh.get(), g_vecCharSimulationPoint, 20 );
		NSOcclusiontreeMaker::MakeTree( &g_pOcclusionTree, g_vecSimulationPoint, g_vecCharSimulationPoint, g_vOctreeMax, g_vOctreeMin, static_cast<float>(nOctreeBoxLength) );
		if ( !g_pOcclusionTree )
		{
			AfxMessageBox("OcclusionTree 가 생성되지 않았습니다.");
			return FALSE;
		}

		// g_vecSimulationPoint 값을 최적화 한다. 한 셀당 최대 점 갯수를 27개로 제한한다. 또한 최적의 위치를 찾는다.
		g_pOcclusionTree->OptimizePoint( pDxLandMan, nSumulationDetailLength );

		//BOOL bOK(FALSE);
		//while ( !bOK )
		//{
		//	NSOcclusiontreeMaker::MakeTree( &g_pOcclusionCharTree, g_vecCharSimulationPoint, g_vOctreeMax, g_vOctreeMin, fOctreeBoxLength_CHAR );
		//	if ( !g_pOcclusionCharTree )
		//	{
		//		AfxMessageBox("OcclusionTree 가 생성되지 않았습니다.");
		//		return FALSE;
		//	}

		//	int nID_Count(0);
		//	g_pOcclusionCharTree->SetOcclusionCharZone( pd3dDevice, nID_Count );
		//	if ( nID_Count <= 1024 )
		//	{
		//		bOK = TRUE;
		//	}

		//	// 일정수준의 값이 나올 수 있도록 크기를 키운다.
		//	fOctreeBoxLength_CHAR *= 1.1f;
		//}
		//

		m_bFieldOcclusion = bFieldOcclusion;
		m_bPieceOcclusion = bPieceOcclusion;
		m_bCharOcclusion = bCharOcclusion;

		DxOcclusionQuery::g_dwCullingDelay = 0;
		DxOcclusionQuery::g_bForceDisable = FALSE;
		g_bRendering = FALSE;
		g_bBaking = TRUE;
		{
			D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
			D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();

			DWORD dwBoxNum;
			g_nBakeCountCUR = 0;
			g_dwBakeCountALL = 0;
			g_pOcclusionTree->LastBoxNum( g_dwBakeCountALL, dwBoxNum );

			int nID_Count(0);
			g_pOcclusionTree->SetOcclusionCharZone( pd3dDevice, nID_Count );

			DxOcclusionQuery::InitDeviceObjects( pd3dDevice );
			DxOcclusionQuery::RestoreDeviceObjects();

			NSOcclusionCharZone::MakeTree();
			NSOcclusionCharZone::CreateQuery( pd3dDevice );
			pDxLandMan->CreateQuery( pd3dDevice );
			g_pOcclusionTree->Bake
			( 
				pd3dDevice, 
				pDxLandMan, 
				g_nBakeCountCUR,
				bFieldOcclusion,
				bPieceOcclusion,
				bCharOcclusion
			);
			pDxLandMan->DestroyQuery();
			NSOcclusionCharZone::DestroyQuery();

			DxOcclusionQuery::InvalidateDeviceObjects();
			DxOcclusionQuery::DeleteDeviceObjects();

			// Note : View Port를 설정한다.
			D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

			LPDIRECT3DSURFACEQ pBackBuffer;
			D3DSURFACE_DESC d3dsdBackBuffer;
			pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &d3dsdBackBuffer );
			pBackBuffer->Release();

			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)d3dsdBackBuffer.Width, 
				(float)d3dsdBackBuffer.Height, 5.0f, pDxLandMan->GetFogProperty().m_fFogEnd );
			DxViewPort::GetInstance().SetToolCamera();

			// 강제로 Bake를 껐다면 리턴값을 FALSE로 준다.
			if ( DxOcclusionQuery::g_bForceDisable )
			{
				g_bBaking = FALSE;
				g_bRendering = FALSE;
				DxOcclusionQuery::g_bForceDisable = TRUE;

				return FALSE;
			}
		}
		g_bBaking = FALSE;
		g_bRendering = TRUE;
		DxOcclusionQuery::g_bForceDisable = TRUE;

		g_pOcclusionTree->CompressData();

		return TRUE;
	}

	BOOL BakeOneCell( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pDxLandMan, 
						const D3DXVECTOR3& vPos,
						int nSumulationDetailLength )
	{
		boost::shared_ptr<NavigationMesh> spNavigationMesh = pDxLandMan->GetNaviMesh();
		if ( !spNavigationMesh )
		{
			AfxMessageBox("NavigationMesh 가 존재하지 않습니다.");
			return FALSE;
		}

		if ( !g_pOcclusionTree )
			return FALSE;

		g_pOcclusionTree->DeCompressData();

		DxOcclusionQuery::g_dwCullingDelay = 0;
		DxOcclusionQuery::g_bForceDisable = FALSE;
		g_bRendering = FALSE;
		g_bBaking = TRUE;
		{
			SimulationPosition( spNavigationMesh.get(), nSumulationDetailLength );
			NSOcclusionCulling::SimulationPosition2( spNavigationMesh.get(), g_vecCharSimulationPoint, 20 );

			// 계산이 필요없는 옵션일 수 있다.
			if ( !m_bFieldOcclusion && !m_bPieceOcclusion )
			{
				g_vecSimulationPoint.clear();
			}
			if ( !m_bCharOcclusion )
			{
				g_vecCharSimulationPoint.clear();
			}

			g_pOcclusionTree->ReInsertPoint( g_vecSimulationPoint, g_vecCharSimulationPoint );

			// g_vecSimulationPoint 값을 최적화 한다. 한 셀당 최대 점 갯수를 27개로 제한한다. 또한 최적의 위치를 찾는다.
			g_pOcclusionTree->OptimizePoint( pDxLandMan, nSumulationDetailLength );

			D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
			D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt();

			DxOcclusionQuery::InitDeviceObjects( pd3dDevice );
			DxOcclusionQuery::RestoreDeviceObjects();

			NSOcclusionCharZone::CreateQuery( pd3dDevice );
			pDxLandMan->CreateQuery( pd3dDevice );
			//COLLISION::g_bCheckAddPlane = TRUE;
			g_pOcclusionTree->BakePosition
			( 
				pd3dDevice, 
				pDxLandMan,
				vPos,
				m_bFieldOcclusion,
				m_bPieceOcclusion,
				m_bCharOcclusion
			);
			//COLLISION::g_bCheckAddPlane = FALSE;
			pDxLandMan->DestroyQuery();
			NSOcclusionCharZone::DestroyQuery();

			DxOcclusionQuery::InvalidateDeviceObjects();
			DxOcclusionQuery::DeleteDeviceObjects();

			// Note : View Port를 설정한다.
			D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
			DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

			LPDIRECT3DSURFACEQ pBackBuffer;
			D3DSURFACE_DESC d3dsdBackBuffer;
			pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &d3dsdBackBuffer );
			pBackBuffer->Release();

			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)d3dsdBackBuffer.Width, 
				(float)d3dsdBackBuffer.Height, 5.0f, pDxLandMan->GetFogProperty().m_fFogEnd );
			DxViewPort::GetInstance().SetToolCamera();

			// 강제로 Bake를 껐다면 리턴값을 FALSE로 준다.
			if ( DxOcclusionQuery::g_bForceDisable )
			{
				g_bBaking = FALSE;
				g_bRendering = FALSE;
				DxOcclusionQuery::g_bForceDisable = TRUE;

				return FALSE;
			}
		}
		g_bBaking = FALSE;
		g_bRendering = TRUE;
		DxOcclusionQuery::g_bForceDisable = TRUE;

		g_pOcclusionTree->CompressData();

		return TRUE;
	}

	BOOL GetRenderIndex( const D3DXVECTOR3& vPos, const VEC_DWORD** ppvecPieceIndex, const VEC_DWORD** ppvecOctreeIndex )
	{
		if ( !g_pOcclusionTree )
			return FALSE;

		// 2012.10.04 
		//
		// #1
		// 속도 체크 테스트 함.
		// prison_undercave_000 맵에서,
		// vector 0.35ms, list 0.56ms, set 1.96ms 로 vector를 선택하게 되었음.
		// set으로 참조값을 넘기면 속도 걱정은 안되지만, memory 크기 때문에 데이터를 모으는 방식을 쓰게 됨.
		// 
		// #2
		// VEC_PSET_DWORD 방식으로 SET_DWORD 값을 포인터로 가지고 있고, GetRenderIndexPos쪽 는 부하를 덜 주고,
		// find 시 검색시간이 좀 길어지는 방식으로 테스트를 해보았다. ( g_vecOctreeIndex 만 적용해 봄. )
		// 기존 335fps, VEC_PSET_DWORD  317fps 로 set 이 여러개로 되니, 결국은 느려졌다.

		g_vecPieceIndex.clear();
		g_vecOctreeIndex.clear();
		for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
		{
			g_vecOccCharZoneIndex[i].clear();
		}

		g_pOcclusionTree->GetRenderIndexPos
		( 
			vPos,
			g_vecPieceIndex, 
			g_vecOctreeIndex, 
			&g_vecOccCharZoneIndex[0]
		);

		std::sort( g_vecPieceIndex.begin(), g_vecPieceIndex.end() );
		std::sort( g_vecOctreeIndex.begin(), g_vecOctreeIndex.end() );
		for ( DWORD i=0; i<NSOcclusionCharZone::HEIGHT_TYPE_SIZE; ++i )
		{
			std::sort( g_vecOccCharZoneIndex[i].begin(), g_vecOccCharZoneIndex[i].end() );
		}

		(*ppvecPieceIndex) = &g_vecPieceIndex;
		(*ppvecOctreeIndex) = &g_vecOctreeIndex;

		return TRUE;
	}

	BOOL VisibleOcclusionCharZone( const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		// Occlusion 이 동작되지 않는다면 무조건 렌더링 하도록 한다.
		if ( !g_bRendering )
			return TRUE;

		// Occlusion 이 동작되지 않는다면 무조건 렌더링 하도록 한다.
		if ( !g_pOcclusionTree )
			return TRUE;

		// Char Occlusion 이 동작되지 않는다면 무조건 렌더링 하도록 한다.
		if ( !m_bCharOcclusion )
			return TRUE;

		// Max Min 와 충돌된 CharZone 을 얻는다.
		VEC_DWORD vecCollCharZone;
		NSOcclusionCharZone::GetCollCharZoneIndex( vMax, vMin, vecCollCharZone );

		float fHeight = vMax.y - vMin.y;

		DWORD dwType(0);
		if ( fHeight > NSOcclusionCharZone::g_fCharHeightHigh )
		{
			dwType = 2;
		}
		if ( fHeight >= NSOcclusionCharZone::g_fCharHeightAvg )
		{
			dwType = 1;
		}

		SET_DWORD_CITER iter;
		// 충돌되는게 있는지 찾는다.
		for ( DWORD i=0; i<vecCollCharZone.size(); ++i )
		{
			for ( DWORD z=0; z<NSOcclusionCharZone::HEIGHT_TYPE_SIZE && z<=dwType; ++z )
			{
				if ( std::binary_search( g_vecOccCharZoneIndex[z].begin(), g_vecOccCharZoneIndex[z].end(), vecCollCharZone[i] ) )
				{
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	BOOL FindChar( BOOL i, DWORD dwID )
	{
		if ( std::binary_search( g_vecOccCharZoneIndex[i].begin(), g_vecOccCharZoneIndex[i].end(), dwID ) )
		{
			return TRUE;
		}

		return FALSE;
	}

	void Save( const TSTRING& strMakeTimeWLD, const char* szName )
	{
		if ( !g_pOcclusionTree )
			return;

		std::string strName = sc::util::ChangeExtName( szName, _T("occ") );

		sc::SerialFile SFile;
		if ( SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )
		{
			SFile << VERSION;
			SFile.BeginBlock();
			{
				SFile << m_bFieldOcclusion;
				SFile << m_bPieceOcclusion;
				SFile << m_bCharOcclusion;

				g_pOcclusionTree->SaveMain( SFile, strMakeTimeWLD );
			}
			SFile.EndBlock();
			SFile.CloseFile();
		}
	}

	void Load( const char* szName )
	{
		m_bFieldOcclusion = FALSE;
		m_bPieceOcclusion = FALSE;
		m_bCharOcclusion = FALSE;
		SAFE_DELETE ( g_pOcclusionTree );

		std::string strName = sc::util::ChangeExtName( szName, _T("occ") );

		sc::SerialFile SFile;
		if ( SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		{
			DWORD dwVer, dwBufferSize;
			SFile >> dwVer;
			SFile >> dwBufferSize;

			SFile >> m_bFieldOcclusion;
			SFile >> m_bPieceOcclusion;
			SFile >> m_bCharOcclusion;

			SAFE_DELETE ( g_pOcclusionTree );
			g_pOcclusionTree = new DxOcclusionTree;
			g_pOcclusionTree->LoadMain( SFile, g_strMakeTimeWLD );
			SFile.CloseFile();
		}
	}

	void IntegrityCheck( const TSTRING& strMakeTimeWLD )
	{
		if ( !m_bFieldOcclusion && !m_bPieceOcclusion && !m_bCharOcclusion )
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
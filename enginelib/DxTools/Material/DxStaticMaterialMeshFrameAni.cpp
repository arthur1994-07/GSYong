#include "pch.h"

#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "./DxStaticMaterialMeshAniMan.h"

#include "DxStaticMaterialMeshFrameAni.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		S	t	a	t	i	c		M	e	s	h
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMaterialMeshFrameAni::DxStaticMaterialMeshFrameAni()
	: m_vTreeMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
	, m_vTreeMin(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_pframeFromAnimate(NULL)
	, m_dwStaticMeshMaterialAniID(-1)
	, m_pframeSibling(NULL)
	, m_pframeFirstChild(NULL)
{
	D3DXMatrixIdentity( &m_matRot );
	D3DXMatrixIdentity( &m_matCombined );
}

DxStaticMaterialMeshFrameAni::~DxStaticMaterialMeshFrameAni()
{
	CleanUp();
}

void DxStaticMaterialMeshFrameAni::CleanUp()
{
	SAFE_DELETE( m_pframeFromAnimate );

	SAFE_DELETE( m_pframeSibling );
	SAFE_DELETE( m_pframeFirstChild );
}

void DxStaticMaterialMeshFrameAni::UpdateMatrix( float fGlobalTime, const D3DXMATRIX &matCur, bool bAniLoop )
{
	// 애니메이션을 돌린다.
	if ( m_pframeFromAnimate )
	{
		if ( bAniLoop )
		{
			m_matRot = *m_pframeFromAnimate->SetTime( fGlobalTime );
		}
		else
		{
			m_matRot = *m_pframeFromAnimate->SetTime( fGlobalTime, 0L );
		}
	}

	//	Note : pframeCur의 메트릭스를 계산. matCombined = matRot * matParent
	D3DXMatrixMultiply( &m_matCombined, &m_matRot, &matCur );


	//	Note : 자식 프레임의 메트릭스 모두 계산.
	DxStaticMaterialMeshFrameAni *pChild = m_pframeFirstChild;
	while( pChild )
	{
		pChild->UpdateMatrix ( fGlobalTime, m_matCombined, bAniLoop );
		pChild = pChild->m_pframeSibling;
	}
}

void DxStaticMaterialMeshFrameAni::UpdateMirrorMatrix( float fHeight )
{
	// 반사일 경우 현재 Matrix를 Mirror로 변경한다.
	m_matCombined._42 = m_matCombined._42 - (fHeight*2.f);
	D3DXMatrixMultiply_MIRROR( m_matCombined );

	//	Note : 자식 프레임의 메트릭스 모두 계산.
	DxStaticMaterialMeshFrameAni *pChild = m_pframeFirstChild;
	while( pChild )
	{
		pChild->UpdateMirrorMatrix( fHeight );
		pChild = pChild->m_pframeSibling;
	}
}

void DxStaticMaterialMeshFrameAni::GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if ( vMax.x < m_vTreeMax.x )
		vMax.x = m_vTreeMax.x;
	if ( vMax.y < m_vTreeMax.y )
		vMax.y = m_vTreeMax.y;
	if ( vMax.z < m_vTreeMax.z )
		vMax.z = m_vTreeMax.z;

	if ( vMin.x > m_vTreeMin.x )
		vMin.x = m_vTreeMin.x;
	if ( vMin.y > m_vTreeMin.y )
		vMin.y = m_vTreeMin.y;
	if ( vMin.z > m_vTreeMin.z )
		vMin.z = m_vTreeMin.z;

	// 자식을 작업한다.
	if ( m_pframeFirstChild )
	{
		m_pframeFirstChild->GetAABBBox( vMax, vMin );
	}

	// 형제를 작업한다.
	if ( m_pframeSibling )
	{
		m_pframeSibling->GetAABBBox( vMax, vMin );
	}
}

// 목적
// 1. DxFrame 의 Tree 구조는 그대로 가져와야 한다.
// 2. Mesh 정보를 DxStaticMaterialMeshAni 로 바꾸어 놓아야 한다.
// 3. DxStaticMaterialMeshAniMan 로 연결이 되어있기 때문에 그 연결선(m_dwStaticMeshMaterialAniID)을 정확히 알아야 한다.
void DxStaticMaterialMeshFrameAni::Import( LPDIRECT3DDEVICEQ pd3dDevice, 
										  DxFrame* pframeCur, 
										  float fUNITTIME, 
										  DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, 
										  const CMaterialSetting *pMaterialSetting, 
										  std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer,
										  float fCellSize,
										  BOOL bParentAnimation )
{
	BOOL bChildAnimation = bParentAnimation;
	if ( pframeCur->pframeFromAnimate || bParentAnimation )
	{
		// 자식도 애니메이션에 영향 받는다고 셋팅한다.
		bChildAnimation = TRUE;

		//	Note : 에니메이션을 반영한 반경으로.
		D3DXVECTOR3	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vMin(FLT_MAX,FLT_MAX,FLT_MAX);
		pframeCur->CalculateBoundingBox ( &m_matRot, vMax, vMin, fUNITTIME, TRUE );

		// 기본 정보
		m_vTreeMax = vMax;
		m_vTreeMin = vMin;
		m_matRot = m_matCombined = pframeCur->matRotOrig;



		//////////////////////////////////////////////////////////////////////////
		//	다른곳에서 m_matCombined 를 제어할 수 있게 하기 위함. (Shadow같은곳)
		vecAnimationMatrixPointer.push_back( &m_matCombined );



		// 애니메이션 정보 복제
		SAFE_DELETE( m_pframeFromAnimate );
		if ( pframeCur->pframeFromAnimate )
		{
			m_pframeFromAnimate = new DxAnimation;
			*m_pframeFromAnimate = *pframeCur->pframeFromAnimate;
		}

		// Mesh 정보 : 아이디만 받고, data는 다른곳에서 관리하도록 한다.
		if ( pframeCur->pmsMeshs )
		{
			m_dwStaticMeshMaterialAniID = pStaticMaterialMeshAniMan->Create( pd3dDevice, pframeCur->pmsMeshs, pMaterialSetting, &m_matCombined, fCellSize );
		}

	}

	// 자식을 작업한다.
	{
		DxFrame* pframeChildSRC = pframeCur->pframeFirstChild;
		if ( pframeChildSRC )
		{
			m_pframeFirstChild = new DxStaticMaterialMeshFrameAni();
			m_pframeFirstChild->Import ( pd3dDevice, pframeChildSRC, fUNITTIME, pStaticMaterialMeshAniMan, pMaterialSetting, vecAnimationMatrixPointer, fCellSize, bChildAnimation );
		}
	}

	// 형제를 작업한다.
	{
		DxFrame* pframSiblingSRC = pframeCur->pframeSibling;
		if ( pframSiblingSRC )
		{
			m_pframeSibling = new DxStaticMaterialMeshFrameAni();
			m_pframeSibling->Import ( pd3dDevice, pframSiblingSRC, fUNITTIME, pStaticMaterialMeshAniMan, pMaterialSetting, vecAnimationMatrixPointer, fCellSize, bParentAnimation );
		}
	}
}

void DxStaticMaterialMeshFrameAni::Save ( sc::SerialFile& SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SaveTree( SFile );
	}
	SFile.EndBlock();
}

void DxStaticMaterialMeshFrameAni::SaveTree( sc::SerialFile& SFile )
{
	// 주요데이터 Save
	{
		SFile << m_vTreeMax;
		SFile << m_vTreeMin;

		SFile.WriteBuffer( &m_matRot, sizeof(D3DXMATRIX) );

		if ( m_pframeFromAnimate )
		{
			SFile << static_cast<BOOL>( TRUE );
			m_pframeFromAnimate->SaveFile( SFile );
		}
		else
		{
			SFile << static_cast<BOOL>( FALSE );
		}

		SFile << m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni 과 연결된 ID ( 나중에 Load 시 연결해주기 위함. )
	}

	// 형제
	if ( m_pframeSibling )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pframeSibling->SaveTree( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}

	// 자식
	if ( m_pframeFirstChild )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pframeFirstChild->SaveTree( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}
}

void DxStaticMaterialMeshFrameAni::Load ( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	LoadTree( SFile, pStaticMaterialMeshAniMan, vecAnimationMatrixPointer );
}

void DxStaticMaterialMeshFrameAni::LoadTree( sc::SerialFile& SFile, DxStaticMaterialMeshAniMan* pStaticMaterialMeshAniMan, std::vector<D3DXMATRIX*>& vecAnimationMatrixPointer )
{
	// 주요데이터 Save
	{
		SFile >> m_vTreeMax;
		SFile >> m_vTreeMin;

		SFile.ReadBuffer( &m_matRot, sizeof(D3DXMATRIX) );

		BOOL bExist(FALSE);
		SFile >> bExist;
		if ( bExist )
		{
			SAFE_DELETE( m_pframeFromAnimate );
			m_pframeFromAnimate = new DxAnimation;
			m_pframeFromAnimate->LoadFile( SFile );
		}

		SFile >> m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni 과 연결된 ID ( 나중에 Load 시 연결해주기 위함. )

		pStaticMaterialMeshAniMan->SetLinkMatrix( m_dwStaticMeshMaterialAniID, &m_matCombined );



		//////////////////////////////////////////////////////////////////////////
		//	다른곳에서 m_matCombined 를 제어할 수 있게 하기 위함. (Shadow같은곳)
		vecAnimationMatrixPointer.push_back( &m_matCombined );
	}

	// 형제
	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pframeSibling = new DxStaticMaterialMeshFrameAni;
		m_pframeSibling->LoadTree( SFile, pStaticMaterialMeshAniMan, vecAnimationMatrixPointer );
	}

	// 자식
	SFile >> bExist;
	if ( bExist )
	{
		m_pframeFirstChild = new DxStaticMaterialMeshFrameAni;
		m_pframeFirstChild->LoadTree( SFile, pStaticMaterialMeshAniMan, vecAnimationMatrixPointer );
	}
}
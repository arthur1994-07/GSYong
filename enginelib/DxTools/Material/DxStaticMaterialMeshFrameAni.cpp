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
	// �ִϸ��̼��� ������.
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

	//	Note : pframeCur�� ��Ʈ������ ���. matCombined = matRot * matParent
	D3DXMatrixMultiply( &m_matCombined, &m_matRot, &matCur );


	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
	DxStaticMaterialMeshFrameAni *pChild = m_pframeFirstChild;
	while( pChild )
	{
		pChild->UpdateMatrix ( fGlobalTime, m_matCombined, bAniLoop );
		pChild = pChild->m_pframeSibling;
	}
}

void DxStaticMaterialMeshFrameAni::UpdateMirrorMatrix( float fHeight )
{
	// �ݻ��� ��� ���� Matrix�� Mirror�� �����Ѵ�.
	m_matCombined._42 = m_matCombined._42 - (fHeight*2.f);
	D3DXMatrixMultiply_MIRROR( m_matCombined );

	//	Note : �ڽ� �������� ��Ʈ���� ��� ���.
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

	// �ڽ��� �۾��Ѵ�.
	if ( m_pframeFirstChild )
	{
		m_pframeFirstChild->GetAABBBox( vMax, vMin );
	}

	// ������ �۾��Ѵ�.
	if ( m_pframeSibling )
	{
		m_pframeSibling->GetAABBBox( vMax, vMin );
	}
}

// ����
// 1. DxFrame �� Tree ������ �״�� �����;� �Ѵ�.
// 2. Mesh ������ DxStaticMaterialMeshAni �� �ٲپ� ���ƾ� �Ѵ�.
// 3. DxStaticMaterialMeshAniMan �� ������ �Ǿ��ֱ� ������ �� ���ἱ(m_dwStaticMeshMaterialAniID)�� ��Ȯ�� �˾ƾ� �Ѵ�.
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
		// �ڽĵ� �ִϸ��̼ǿ� ���� �޴´ٰ� �����Ѵ�.
		bChildAnimation = TRUE;

		//	Note : ���ϸ��̼��� �ݿ��� �ݰ�����.
		D3DXVECTOR3	vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX), vMin(FLT_MAX,FLT_MAX,FLT_MAX);
		pframeCur->CalculateBoundingBox ( &m_matRot, vMax, vMin, fUNITTIME, TRUE );

		// �⺻ ����
		m_vTreeMax = vMax;
		m_vTreeMin = vMin;
		m_matRot = m_matCombined = pframeCur->matRotOrig;



		//////////////////////////////////////////////////////////////////////////
		//	�ٸ������� m_matCombined �� ������ �� �ְ� �ϱ� ����. (Shadow������)
		vecAnimationMatrixPointer.push_back( &m_matCombined );



		// �ִϸ��̼� ���� ����
		SAFE_DELETE( m_pframeFromAnimate );
		if ( pframeCur->pframeFromAnimate )
		{
			m_pframeFromAnimate = new DxAnimation;
			*m_pframeFromAnimate = *pframeCur->pframeFromAnimate;
		}

		// Mesh ���� : ���̵� �ް�, data�� �ٸ������� �����ϵ��� �Ѵ�.
		if ( pframeCur->pmsMeshs )
		{
			m_dwStaticMeshMaterialAniID = pStaticMaterialMeshAniMan->Create( pd3dDevice, pframeCur->pmsMeshs, pMaterialSetting, &m_matCombined, fCellSize );
		}

	}

	// �ڽ��� �۾��Ѵ�.
	{
		DxFrame* pframeChildSRC = pframeCur->pframeFirstChild;
		if ( pframeChildSRC )
		{
			m_pframeFirstChild = new DxStaticMaterialMeshFrameAni();
			m_pframeFirstChild->Import ( pd3dDevice, pframeChildSRC, fUNITTIME, pStaticMaterialMeshAniMan, pMaterialSetting, vecAnimationMatrixPointer, fCellSize, bChildAnimation );
		}
	}

	// ������ �۾��Ѵ�.
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
	// �ֿ䵥���� Save
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

		SFile << m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni �� ����� ID ( ���߿� Load �� �������ֱ� ����. )
	}

	// ����
	if ( m_pframeSibling )
	{
		SFile << static_cast<BOOL>( TRUE );
		m_pframeSibling->SaveTree( SFile );
	}
	else
	{
		SFile << static_cast<BOOL>( FALSE );
	}

	// �ڽ�
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
	// �ֿ䵥���� Save
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

		SFile >> m_dwStaticMeshMaterialAniID;	// DxStaticMaterialMeshAni �� ����� ID ( ���߿� Load �� �������ֱ� ����. )

		pStaticMaterialMeshAniMan->SetLinkMatrix( m_dwStaticMeshMaterialAniID, &m_matCombined );



		//////////////////////////////////////////////////////////////////////////
		//	�ٸ������� m_matCombined �� ������ �� �ְ� �ϱ� ����. (Shadow������)
		vecAnimationMatrixPointer.push_back( &m_matCombined );
	}

	// ����
	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_pframeSibling = new DxStaticMaterialMeshFrameAni;
		m_pframeSibling->LoadTree( SFile, pStaticMaterialMeshAniMan, vecAnimationMatrixPointer );
	}

	// �ڽ�
	SFile >> bExist;
	if ( bExist )
	{
		m_pframeFirstChild = new DxStaticMaterialMeshFrameAni;
		m_pframeFirstChild->LoadTree( SFile, pStaticMaterialMeshAniMan, vecAnimationMatrixPointer );
	}
}
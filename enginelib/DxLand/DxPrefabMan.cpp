#include "pch.h"

#include "../Common/StlFunctions.h"

#include "../DxTools/Light/DxLightMan.h"
#include "../DxTools/DxViewPort.h"
#include "./DxPiece/DxPieceManagerPSF.h"
#include "./DxSetLandMan.h"

#include "./DxPrefabMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// ---------------------------------------------------------------- Prefab
//
DxPrefab::DxPrefab(void)
	: m_scpSetLandMan(new DxSetLandMan(TRUE))
{
}

DxPrefab::~DxPrefab (void)
{
}

void DxPrefab::FrameMove( float fTime, float fElapsedTime )
{
	m_scpSetLandMan->FrameMove( fTime, fElapsedTime );
}

void DxPrefab::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove )
{
	m_scpSetLandMan->Render( pd3dDevice, &m_matWorld, bFrameMove );

	m_scpSetLandMan->Render_EFF( pd3dDevice, bFrameMove );
}

BOOL DxPrefab::IsCollision ( D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor ) const
{
	DxPiece* pPiece(NULL);

	if ( pNor )
	{
		return m_scpSetLandMan->GetPieceManager()->IsCollisionLine2( vStart, vEnd, *pNor, vEnd, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking
	}
	else
	{
		D3DXVECTOR3 vNor(0.f,1.f,0.f);
		return m_scpSetLandMan->GetPieceManager()->IsCollisionLine2( vStart, vEnd, vNor, vEnd, &pPiece, EMCC_CULL_CCW );	// PieceManager Picking
	}
}

void DxPrefab::MakeTree()
{
	m_scpSetLandMan->GetPieceManager()->MakeTree( m_matWorld );
}

void DxPrefab::RefreshPrefab( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath )
{
	m_scpSetLandMan->CleanUp( pd3dDevice );

	// 로딩됨.
	TSTRING strFullName = strPath;
	strFullName += m_strFileName;

	m_scpSetLandMan->LoadSetPrefab( strFullName.c_str(), pd3dDevice );
}

void DxPrefab::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	m_scpSetLandMan->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece );
}

void DxPrefab::Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, const D3DXMATRIX& matWorld )
{
	// 
	m_matWorld = matWorld;

	// 확장자를 포함한 파일 이름만 돌려준다
	sc::string::getFullFileName( strName, m_strFileName );

	// 로딩됨.
	m_scpSetLandMan->LoadSetPrefab( strName.c_str(), pd3dDevice );

	m_scpSetLandMan->GetPieceManager()->MakeTree( m_matWorld );
}

// ---------------------------------------------------------------- PrefabMan
//
TSTRING DxPrefabMan::m_strPath(_T(""));

DxPrefabMan::DxPrefabMan(void)
{
}

DxPrefabMan::~DxPrefabMan (void)
{
	CleanUp();
}

void DxPrefabMan::CleanUp()
{
	m_vecPrefab.clear();
}

void DxPrefabMan::FrameMove( float fTime, float fElapsedTime )
{
	BOOST_FOREACH( VEC_PREFAB::value_type& iter, m_vecPrefab )
	{
		iter->FrameMove( fTime, fElapsedTime );
	}
}

void DxPrefabMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove )
{
	BOOST_FOREACH( VEC_PREFAB::value_type& iter, m_vecPrefab )
	{
		iter->Render( pd3dDevice, bFrameMove );
	}
}

void DxPrefabMan::Insert( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, const D3DXMATRIX& matWorld )
{
	std::tr1::shared_ptr<DxPrefab> spNew = std::tr1::shared_ptr<DxPrefab>(new DxPrefab);
	spNew->Load( pd3dDevice, strName, matWorld );
	m_vecPrefab.push_back( spNew );
}

void DxPrefabMan::Delete( int nIndex )
{
	int nCount(0);
	VEC_PREFAB_ITER iter = m_vecPrefab.begin();
	for ( ; iter!=m_vecPrefab.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			m_vecPrefab.erase( iter );
			break;
		}
	}
}

size_t DxPrefabMan::GetSize()
{
	return m_vecPrefab.size();
}

BOOL DxPrefabMan::IsGetName( size_t i, TSTRING& pPieceName )
{
	if ( i>=m_vecPrefab.size() )
		return FALSE;

	pPieceName = m_vecPrefab[i]->m_strFileName;
	return TRUE;
}

BOOL DxPrefabMan::IsCollision ( const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt, DxPrefab** ppPrefab ) const
{
	D3DXVECTOR3 vStart = vFromPt;
	D3DXVECTOR3 vEnd = vTargetPt;

	BOOL bCollsion(FALSE);
	for ( size_t i=0; i<m_vecPrefab.size(); ++i )
	{
		if ( m_vecPrefab[i]->IsCollision ( vStart, vEnd, NULL ) )
		{
			(*ppPrefab) = m_vecPrefab[i].get();
			bCollsion = TRUE;
		}
	}

	return bCollsion;
}

D3DXMATRIX* DxPrefabMan::MoveCamera( DWORD dwItem )
{
	if ( dwItem >= m_vecPrefab.size() )
	{
		const D3DXVECTOR3& vFrom = DxViewPort::GetInstance().GetFromPt();

		static D3DXMATRIX matTemp;
		D3DXMatrixIdentity( &matTemp );
		D3DXMatrixTranslation( &matTemp, vFrom.x, vFrom.y, vFrom.z );
		return &matTemp;
	}

	D3DXVECTOR3 vPos;
	vPos.x = m_vecPrefab[dwItem]->m_matWorld._41;
	vPos.y = m_vecPrefab[dwItem]->m_matWorld._42;
	vPos.z = m_vecPrefab[dwItem]->m_matWorld._43;

	// Note : 카메라를 이동해야지.
	DxViewPort::GetInstance().CameraJump( vPos );

	return &m_vecPrefab[dwItem]->m_matWorld;
}

DxPrefab* DxPrefabMan::GetPrefab( DWORD dwItem )
{
	if ( dwItem >= m_vecPrefab.size() )
		return NULL;

	return m_vecPrefab[dwItem].get();
}

int DxPrefabMan::GetIndex( DxPrefab* pSrc )
{
	for ( size_t i=0; i<m_vecPrefab.size(); ++i )
	{
		if ( m_vecPrefab[i].get() == pSrc )
		{
			return static_cast<int>( i );
		}
	}
	return -1;
}

void DxPrefabMan::MakeTree()
{
	for ( size_t i=0; i<m_vecPrefab.size(); ++i )
	{
		m_vecPrefab[i]->MakeTree();
	}
}

void DxPrefabMan::RefreshPrefab( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( size_t i=0; i<m_vecPrefab.size(); ++i )
	{
		m_vecPrefab[i]->RefreshPrefab( pd3dDevice, m_strPath );
	}
}

void DxPrefabMan::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	for ( size_t i=0; i<m_vecPrefab.size(); ++i )
	{
		m_vecPrefab[i]->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece );
	}
}

void DxPrefabMan::Save( sc::SerialFile& SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = static_cast<DWORD>( m_vecPrefab.size() );
		SFile << dwSize;

		BOOST_FOREACH( VEC_PREFAB::value_type& iter, m_vecPrefab )
		{
			SFile << iter->m_strFileName;
			SFile.WriteBuffer ( &iter->m_matWorld, sizeof(iter->m_matWorld) );
		}
	}
	SFile.EndBlock();
}

void DxPrefabMan::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	TSTRING strName;
	D3DXMATRIX matWorld;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> strName;
		SFile.ReadBuffer ( &matWorld, sizeof(matWorld) );

		TSTRING strFullName = m_strPath;
		strFullName += strName;
		Insert( pd3dDevice, strFullName, matWorld );
	}
}

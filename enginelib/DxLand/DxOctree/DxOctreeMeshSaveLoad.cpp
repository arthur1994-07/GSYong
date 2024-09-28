#include "pch.h"

#include "DxOctreeMesh.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../DxTools/Collision.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL	DxOctreeMesh::SaveFile ( sc::SerialFile &SFile )
{
	HRESULT hr(S_OK);

	SFile << VERSION;

	SFile << m_dwFVF;
	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_dwFVF );

	//Note : VB
	BYTE	*ppbDataVB = NULL;
	SFile << m_dwNumVertices;	
	hr = m_pVB->Lock( 0, 0, (VOID**)&ppbDataVB, NULL );
	if (FAILED(hr))
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::SaveFile() -- m_pVB->Lock() -- Failed"));

	SFile.WriteBuffer ( ppbDataVB, dwFVFSize * m_dwNumVertices );
	m_pVB->Unlock();

	//Note : IB
	WORD			*ppbDataIB = NULL;
	SFile << m_dwNumFaces;
	hr = m_pIB->Lock( 0, 0, (VOID**)&ppbDataIB, NULL );
	if (FAILED(hr))
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::SaveFile() -- m_pIB->Lock() -- Failed"));

	SFile.WriteBuffer ( ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
	m_pIB->Unlock();

	//Note : D3DXATTRIBUTERANGE
	SFile << m_dwAttribTableSize;
	SFile.WriteBuffer ( m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

	if ( m_pAABBTreeRoot )
	{
		SFile << BOOL ( TRUE );
		m_pAABBTreeRoot->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	return TRUE;
}

BOOL	DxOctreeMesh::LoadFile_NEW ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	DWORD dwVer;
	SFile >> dwVer;

	BOOL	bExist = FALSE;
	BOOL	bDataOK = TRUE;

	SFile >> m_dwFVF;

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_dwFVF );

	BYTE	*ppbDataVB = NULL;
	SFile >> m_dwNumVertices;
	if( m_dwNumVertices > 0 )
	{
		ppbDataVB = new BYTE [ dwFVFSize * m_dwNumVertices ];
		SFile.ReadBuffer ( ppbDataVB, dwFVFSize * m_dwNumVertices );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- VB"));
	}

	WORD* ppbDataIB = NULL;	
	SFile >> m_dwNumFaces;
	if( m_dwNumFaces > 0 )
	{
		ppbDataIB = new WORD [ m_dwNumFaces * 3 ];
		SFile.ReadBuffer ( ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- IB"));
	}

	// Note : D3DXATTRIBUTERANGE
	SFile >> m_dwAttribTableSize;
	if( m_dwAttribTableSize > 0 )
	{
		CreateAttribute ( m_dwAttribTableSize );
		SFile.ReadBuffer ( m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

		// 유효성 검사를 한다.
		for ( DWORD i=0; i<m_dwAttribTableSize; ++i )
		{
			if ( (m_pAttribTable[i].VertexStart + m_pAttribTable[i].VertexCount) > m_dwNumVertices )
			{
				bDataOK = FALSE;
				GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
				CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
				break;
			}

			if ( (m_pAttribTable[i].FaceStart + m_pAttribTable[i].FaceCount) > m_dwNumFaces )
			{
				bDataOK = FALSE;
				GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
				CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
				break;
			}
		}
	}
	else
	{
		bDataOK = FALSE;
		GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
	}

	if( bDataOK )
	{
		// Note : 
		CreateDxOctreeMesh ( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );

		// Note : VB
		VERTEX	*ppbDataLockVB = NULL;
		hr = m_pVB->Lock( 0, 0, (VOID**)&ppbDataLockVB, NULL );
		if (FAILED(hr))
			CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- m_pVB->Lock() -- Failed"));

		memcpy ( ppbDataLockVB, ppbDataVB, dwFVFSize * m_dwNumVertices );			
		m_pVB->Unlock();

		// Note : IB
		WORD			*ppbDataLockIB = NULL;
		hr = m_pIB->Lock( 0, 0, (VOID**)&ppbDataLockIB, NULL );
		if (FAILED(hr))
			CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- m_pIB->Lock() -- Failed"));

		memcpy ( ppbDataLockIB, ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
		m_pIB->Unlock();
	}
	else
	{
		SAFE_DELETE_ARRAY( m_pAttribTable );
	}

	SAFE_DELETE_ARRAY ( ppbDataVB );
	SAFE_DELETE_ARRAY ( ppbDataIB );

	SFile >> bExist;
	if ( bExist )
	{
		m_pAABBTreeRoot = new DxAABBNode;
		m_pAABBTreeRoot->LoadFile_PREV( SFile );

		// [shhan][2014.12.11] 새로 만들도록 한다.
		SAFE_DELETE ( m_pAABBTreeRoot );
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		MakeAABBTree( matIdentity );
	}

	return TRUE;
}

BOOL	DxOctreeMesh::LoadFile_OLD( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	BOOL	bExist = FALSE;
	BOOL	bDataOK = TRUE;

	SFile >> m_dwFVF;	

	VERTEX	*ppbDataVB = NULL;
	SFile >> m_dwNumVertices;
	if( m_dwNumVertices > 0 )
	{
		ppbDataVB = new VERTEX [ m_dwNumVertices ];
		SFile.ReadBuffer ( ppbDataVB, sizeof ( VERTEX ) * m_dwNumVertices );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- VB"));
	}

	WORD* ppbDataIB = NULL;	
	SFile >> m_dwNumFaces;
	if( m_dwNumFaces > 0 )
	{
		ppbDataIB = new WORD [ m_dwNumFaces * 3 ];
		SFile.ReadBuffer ( ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
	}
	else
	{
		bDataOK = FALSE;
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- IB"));
	}

	// Note : D3DXATTRIBUTERANGE
	SFile >> m_dwAttribTableSize;
	if( m_dwAttribTableSize > 0 )
	{
		CreateAttribute ( m_dwAttribTableSize );
		SFile.ReadBuffer ( m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );

		// 유효성 검사를 한다.
		for ( DWORD i=0; i<m_dwAttribTableSize; ++i )
		{
			if ( (m_pAttribTable[i].VertexStart + m_pAttribTable[i].VertexCount) > m_dwNumVertices )
			{
				bDataOK = FALSE;
				GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
				CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
				break;
			}

			if ( (m_pAttribTable[i].FaceStart + m_pAttribTable[i].FaceCount) > m_dwNumFaces )
			{
				bDataOK = FALSE;
				GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
				CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
				break;
			}
		}
	}
	else
	{
		bDataOK = FALSE;
		GASSERT( 0 && _T("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE") );
		CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- Data Error -- D3DXATTRIBUTERANGE"));
	}

	if( bDataOK )
	{
		// Note : 
		CreateDxOctreeMesh_OLD( pd3dDevice, m_dwNumFaces, m_dwNumVertices, m_dwFVF );

		// Note : VB
		VERTEX	*ppbDataLockVB = NULL;
		hr = m_pVB->Lock( 0, 0, (VOID**)&ppbDataLockVB, NULL );
		if (FAILED(hr))
			CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- m_pVB->Lock() -- Failed"));

		memcpy ( ppbDataLockVB, ppbDataVB, sizeof ( VERTEX ) * m_dwNumVertices );			
		m_pVB->Unlock();

		// Note : IB
		WORD			*ppbDataLockIB = NULL;
		hr = m_pIB->Lock( 0, 0, (VOID**)&ppbDataLockIB, NULL );
		if (FAILED(hr))
			CDebugSet::ToLogFile(std::string("DxOctreeMesh::LoadFile() -- m_pIB->Lock() -- Failed"));

		memcpy ( ppbDataLockIB, ppbDataIB, sizeof ( WORD ) * m_dwNumFaces * 3 );
		m_pIB->Unlock();
	}
	else
	{
		SAFE_DELETE_ARRAY( m_pAttribTable );
	}

	SAFE_DELETE_ARRAY ( ppbDataVB );
	SAFE_DELETE_ARRAY ( ppbDataIB );

	SFile >> bExist;
	if ( bExist )
	{
		m_pAABBTreeRoot = new DxAABBNode;
		m_pAABBTreeRoot->LoadFile_PREV( SFile );

		// [shhan][2014.12.11] 새로 만들도록 한다.
		SAFE_DELETE ( m_pAABBTreeRoot );
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		MakeAABBTree( matIdentity );
	}

	return TRUE;
}


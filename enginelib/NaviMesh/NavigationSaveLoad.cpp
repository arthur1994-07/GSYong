#include "pch.h"

#include "NavigationCell.h"
#include "NavigationMesh.h"
#include "../../SigmaCore/File/SerialFile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void NavigationMesh::SaveFile( sc::SerialFile& SFile )
{	
	SFile << m_nNaviVertex;
	if( m_nNaviVertex )
	{
		SFile.WriteBuffer( m_pNaviVertex, sizeof ( D3DXVECTOR3 ) * m_nNaviVertex );
	}

	int	DACellCount  = TotalCells();
	SFile << DACellCount;

	for( int i=0; i<DACellCount; i++ )
	{
		NavigationCell* pCell = m_pDACell[ i ];
		pCell->SaveFile( SFile );
	}

	for( int i=0; i<DACellCount ; i++ )
	{
		NavigationCell* pCell = m_pDACell[ i ];

		//	<--	Link를 저장하기 위함
		for( int j=0; j<3; j++ )
		{
			NavigationCell* LinkCell = pCell->Link( j );

			if( LinkCell )
			{
				SFile << BOOL ( TRUE );

				DWORD LinkID = LinkCell->CellID();
				SFile << LinkID;
			}
			else
			{
				SFile << BOOL( FALSE );
			}
		}
		//	-->	Link를 저장하기 위함
	}
}

void NavigationMesh::LoadFile( sc::SerialFile& SFile )
{
	SFile >> m_nNaviVertex;
	if( m_nNaviVertex )
	{
        m_pNaviVertex = new D3DXVECTOR3[ m_nNaviVertex ];
		SFile.ReadBuffer( m_pNaviVertex, sizeof ( D3DXVECTOR3 ) * m_nNaviVertex );
	}

	DWORD dwDACellCount = 0;
	SFile >> dwDACellCount;

	if( dwDACellCount )
	{
		m_pDACell = new NavigationCell*[ dwDACellCount ];
	}

	for( DWORD i=0; i<dwDACellCount; i++ )
	{
		NavigationCell* NewCell = new NavigationCell;		
		NewCell->LoadFile( SFile );

		m_pDACell[ i ] = NewCell;
		m_CellArray.push_back( NewCell );
	}

	BOOL bExist = FALSE;
	for ( DWORD i=0; i<dwDACellCount; i++ )
	{
		NavigationCell* pCell = m_pDACell[ i ];

		//	<--	Link를 저장하기 위함
		for( int j=0; j<3; j++ )
		{
			SFile >> bExist;		
			if ( bExist )
			{
				DWORD LinkID;
				SFile >> LinkID;
				pCell->SetLink( NavigationCell::CELL_SIDE( j ), GetCell( LinkID ) );
			}
			else
			{
				pCell->SetLink( NavigationCell::CELL_SIDE( j ), NULL );
			}
		}
		//	-->	Link를 저장하기 위함
	}

	MakeAABBTree();
}

void NavigationMesh::SaveFile( const TCHAR* pName )
{
	sc::SerialFile	SFile;

	TSTRING strName = DxLandMan::GetPath();
	strName += pName;

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, strName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	}

	DWORD dwVer(0x100);
	SFile << dwVer;
	SFile.BeginBlock();
	{
		SaveFile( SFile );
	}
	SFile.EndBlock();

	SFile.CloseFile();
}

BOOL NavigationMesh::LoadFile( const TCHAR* pName )
{
	sc::SerialFile	SFile;

	TSTRING strName = DxLandMan::GetPath();
	strName += pName;

	BOOL bOpened = SFile.OpenFile( FOT_READ, strName.c_str() );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
		return FALSE;
	}

	DWORD dwVer(0x100);
	DWORD dwBufferSize(0L);
	SFile >> dwVer;
	SFile >> dwBufferSize;
	
	LoadFile( SFile );

	SFile.CloseFile();

	return TRUE;
}

void NavigationCell::SaveFile( sc::SerialFile &SFile )
{
    SFile << m_CellID;
	SFile.WriteBuffer( m_Vertex, sizeof( DWORD ) * 3 );
	SFile.WriteBuffer( m_Side, sizeof( Line2D ) * 3 );
	SFile.WriteBuffer( &m_CellPlane, sizeof( Plane ) );
	SFile.WriteBuffer( &m_CenterPoint, sizeof( D3DXVECTOR3 ) );
    SFile.WriteBuffer( m_WallMidpoint, sizeof( D3DXVECTOR3 ) * 3 );
	SFile.WriteBuffer( m_WallDistance, sizeof( float ) * 3 );	
}

void NavigationCell::LoadFile( sc::SerialFile &SFile )
{
    SFile >> m_CellID;
	SFile.ReadBuffer( m_Vertex, sizeof( DWORD ) * 3 );
	SFile.ReadBuffer( m_Side, sizeof( Line2D ) * 3 );
	SFile.ReadBuffer( &m_CellPlane, sizeof( Plane ) );
	SFile.ReadBuffer( &m_CenterPoint, sizeof( D3DXVECTOR3 ) );
    SFile.ReadBuffer( m_WallMidpoint, sizeof( D3DXVECTOR3 ) * 3 );
	SFile.ReadBuffer( m_WallDistance, sizeof( float ) * 3 );
}
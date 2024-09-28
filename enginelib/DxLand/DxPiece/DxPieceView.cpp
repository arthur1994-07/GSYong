#include "pch.h"

#include "../../../SigmaCore/String/StringUtil.h"

#include "./DxPiece.h"
#include "./DxPieceContainer.h"

#include "DxPieceView.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	class PIECE_VIEW_CMP
	{
	public:
		bool operator() ( const TSTRING &lvalue, const TSTRING &rvalue )
		{
			return bool( lvalue.compare( rvalue ) < 0 );
		}
	};
}

DxPieceView::DxPieceView() :
	m_pd3dDevice(NULL)
{
}

DxPieceView::~DxPieceView()
{
	CleanUp();
}

void DxPieceView::CleanUp()
{
	m_vecPIE.clear();
	m_vecSPT.clear();
}

void DxPieceView::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
}

//-------------------------------------------------------------
//							E	D	I	T
BOOL DxPieceView::InsertPIE( const TCHAR* pName )
{
	for( DWORD i=0; i<m_vecPIE.size(); ++i )
	{
		if( m_vecPIE[i] == pName )
		{
			return FALSE;
		}

		if ( sc::string::getFileExt( m_vecPIE[i].c_str() ) != sc::string::getFileExt( pName ) )
		{
			AfxMessageBox( _T("ERROR - pie, pie15 가 동시에 쓰이고 있습니다.") );
			return FALSE;
		}
	}

	m_vecPIE.push_back( pName );
	std::sort( m_vecPIE.begin(), m_vecPIE.end(), PIECE_VIEW_CMP() );

	return TRUE;
}

BOOL DxPieceView::InsertSPT( const TCHAR* pName )
{
	for( DWORD i=0; i<m_vecSPT.size(); ++i )
	{
		if( m_vecSPT[i] == pName )
		{
			return FALSE;
		}
	}

	m_vecSPT.push_back( pName );
	return TRUE;
}

void DxPieceView::DeletePIE( int nIndex )
{
	int nCount(0);
	VEC_TSTRING_ITER iter = m_vecPIE.begin();
	for( ; iter!=m_vecPIE.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_vecPIE.erase( iter );
			return;
		}
	}
}

void DxPieceView::DeleteSPT( int nIndex )
{
	int nCount(0);
	VEC_TSTRING_ITER iter = m_vecSPT.begin();
	for( ; iter!=m_vecSPT.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_vecSPT.erase( iter );
			return;
		}
	}
}

void DxPieceView::DeletePIE_ALL()
{
	m_vecPIE.clear();
}

void DxPieceView::DeleteSPT_ALL()
{
	m_vecSPT.clear();
}

DxPieceEdit* DxPieceView::GetPiece( DWORD dwIndex )
{
	if( dwIndex >= m_vecPIE.size() )	return NULL;

	return DxPieceContainer::GetInstancePSF().LoadPiece( m_pd3dDevice, m_vecPIE[dwIndex].c_str() );
}

const TCHAR* DxPieceView::GetPieceName( DWORD dwIndex )
{
	if( dwIndex >= m_vecPIE.size() )	return NULL;

	return m_vecPIE[dwIndex].c_str();
}

const TCHAR* DxPieceView::GetSPTName( DWORD dwIndex )
{
	if( dwIndex >= m_vecSPT.size() )	return NULL;

	return m_vecSPT[dwIndex].c_str();
}

//-------------------------------------------------------------
//			S	A	V	E		&&		L	O	A	D
void DxPieceView::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << (UINT) m_vecPIE.size();
		for( DWORD i=0; i<m_vecPIE.size(); ++i )
		{
			SFile << m_vecPIE[i];
		}

		SFile << (UINT) m_vecSPT.size();
		for( DWORD i=0; i<m_vecSPT.size(); ++i )
		{
			SFile << m_vecSPT[i];
		}
	}
	SFile.EndBlock();
}

void DxPieceView::Load( sc::SerialFile &SFile )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		DWORD dwSize;
		TSTRING strName;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			m_vecPIE.push_back( strName.c_str() );
		}

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			m_vecSPT.push_back( strName.c_str() );
		}
	}
	else if( dwVer==0x101 )
	{
		DWORD dwSize;
		TSTRING strName;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			m_vecPIE.push_back( strName.c_str() );
		}
		std::sort( m_vecPIE.begin(), m_vecPIE.end(), PIECE_VIEW_CMP() );

		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			m_vecSPT.push_back( strName.c_str() );
		}
	}
	else if( dwVer==0x100 )
	{
		DWORD dwSize;
		TSTRING strName;
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			m_vecPIE.push_back( strName.c_str() );
		}
		std::sort( m_vecPIE.begin(), m_vecPIE.end(), PIECE_VIEW_CMP() );
	}
}
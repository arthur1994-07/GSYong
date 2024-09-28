#include "pch.h"

#include "../../Common/StlFunctions.h"

#include "./DxPiece.h"
#include "./DxPieceEdit.h"

#include "DxPieceContainer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


DxPieceContainer& DxPieceContainer::GetInstance()
{
	static DxPieceContainer Instance;
	return Instance;
}

DxPieceContainer& DxPieceContainer::GetInstancePSF()
{
	static DxPieceContainer InstancePSF;
	return InstancePSF;
}


// -----------------------------------------------------------------------------------------------------------------------------------------
//							D	x			S	t	a	t	i	c	M	e	s	h			C	o	n	t	a	i	n	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
char DxPieceContainer::m_szPath[] = "";

DxPieceContainer::DxPieceContainer()
{
}

DxPieceContainer::~DxPieceContainer()
{
	CleanUp ();
}

void DxPieceContainer::CleanUp ()
{
	std::for_each ( m_mapStaticPis.begin(), m_mapStaticPis.end(), std_afunc::DeleteMapObject() );
	m_mapStaticPis.clear ();
}

DxPieceEdit* DxPieceContainer::FindPiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter!=m_mapStaticPis.end() )	return (*iter).second;

	return NULL;
}

DxPieceEdit* DxPieceContainer::LoadPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile )
{
	HRESULT hr=S_OK;

	DxPieceEdit* pPieceEdit;
	if ( pPieceEdit = FindPiece ( szFile ) )
	{
		pPieceEdit->m_nRefCount++;
		return pPieceEdit;
	}

	std::string szFullName;
	szFullName = m_szPath;
	szFullName += szFile;

	DxPieceEdit* pNewPieceEdit = new DxPieceEdit;
	hr = pNewPieceEdit->LoadPIE( pd3dDevice, szFullName.c_str(), m_strPieceEditPath, szFile );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile(
			sc::string::format(
			_T(" file open fail : %1%"),
			szFile ) );

		SAFE_DELETE(pNewPieceEdit);
		return NULL;
	}

	pNewPieceEdit->m_nRefCount++;

	//	Note : 리스트에 등록.
	//
	m_mapStaticPis.insert ( std::make_pair(szFile,pNewPieceEdit) );

	return pNewPieceEdit;
}

DxPieceEdit* DxPieceContainer::ReLoadPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pPiece, const char *szFile )
{
	HRESULT hr=S_OK;

	DxPieceEdit* pPieceEdit = FindPiece ( szFile );

	if ( pPieceEdit == 0 || pPiece == 0 )
	{
		return 0;
	}

	if ( pPieceEdit->m_pStaticAniFrame != pPiece->GetStaticAniFrame() )
	{
		return pPieceEdit;
	}

	std::string szFullName;
	szFullName = m_szPath;
	szFullName += szFile;

	hr = pPieceEdit->LoadPIE( pd3dDevice, szFullName.c_str(), m_strPieceEditPath, szFile );

	if ( FAILED(hr) )
	{
		return 0;
	}

	return pPieceEdit;
}

void DxPieceContainer::ReleasePiece ( const char *szFile )
{
	MAPSTATICPIS_ITER iter = m_mapStaticPis.find ( szFile );
	if ( iter==m_mapStaticPis.end() )	return;

	(*iter).second->m_nRefCount--;
	if ( (*iter).second->m_nRefCount <= 0 )
	{
		delete (*iter).second;
		m_mapStaticPis.erase ( iter );
	}

	return;
}
#include "pch.h"
#include <algorithm>

#include "../../G-Logic/GLogic.h"
#include "../../DxTools/DxMethods.h"
#include "./DxEffCharAmbient.h"

#include "./dxeffchardata.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffcharDataMan& DxEffcharDataMan::GetInstance()
{
	static DxEffcharDataMan Instance;
	return Instance;
}

DxEffCharData::DxEffCharData(void)
	: m_emType(PIECE_HEAD)
{
}

DxEffCharData::~DxEffCharData(void)
{
	CleanUp ();
}

BOOL DxEffCharData::Import ( EMPIECECHAR _emType, GLEFFCHAR_VEC &vecEFFECT )
{
	m_emType = _emType;

	size_t nSIZE = vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = vecEFFECT[n];

		SEFFDATA* pEFFDATA = new SEFFDATA;
		pEFFECT->SaveFile ( pEFFDATA->m_byteStream );

		m_listEffData.push_back ( pEFFDATA );
	}

	return TRUE;
}

void DxEffCharData::ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return;
		pNewEffChar->LoadFile( pEffData->m_byteStream, pd3dDevice );

		pNewEffChar->ConvertTracePoint(); 

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();
		pNewEffChar->SaveFile( pEffData->m_byteStream );

		SAFE_DELETE(pNewEffChar);
	}
}

void DxEffCharData::CleanUp ()
{
	std::for_each ( m_listEffData.begin(), m_listEffData.end(), std_afunc::DeleteObject() );
	m_listEffData.clear ();
}

BOOL DxEffCharData::SetPart ( DxCharPart *pCharPart, LPDIRECT3DDEVICEQ	pd3dDevice, const D3DXVECTOR3 *pDir, float fTarDir )
{
	HRESULT hr;
	// 타입에 구애받지 않는다.
	//if ( pCharPart->m_emType != m_emType )		return FALSE;

	GASSERT(pDir&&"DxEffCharData::SetPart() 방향백터가 null입니다." );
	const D3DXVECTOR3 vDIR = pDir ? *pDir : D3DXVECTOR3(0,0,1);	

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; iter++ )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return FALSE;

		pNewEffChar->SetFileName ( m_strFileName );
		pNewEffChar->SetLinkObj ( pCharPart, NULL );
		pNewEffChar->SetDirection ( &vDIR, fTarDir );
		hr = pNewEffChar->LoadFile ( pEffData->m_byteStream, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pNewEffChar);
			continue;
		}

		pCharPart->AddEffList ( pNewEffChar );

        // Note : Ambient Color를 얻어서 셋팅한다.
        //          Overlay 와 VisualMaterial 등. 형상이 다른 것에서 같은 효과를 내기위해 셋팅한다.
        if ( pNewEffChar->GetTypeID() == EMEFFCHAR_AMBIENT )
        {
            // [2011.03.31] 속도를 생각해서 dynamic_cast -> static_cast로 변경.
            //DxEffCharAmbient* pEffCharAmbient = dynamic_cast<DxEffCharAmbient*>( pNewEffChar );
			DxEffCharAmbient* pEffCharAmbient = MIN_BOOST::polymorphic_downcast<DxEffCharAmbient*>( pNewEffChar );
            pCharPart->m_cEffAmbientType = pEffCharAmbient->GetEffAmbientType();
            pCharPart->m_cEffAmbientColor = pEffCharAmbient->GetEffAmbientColor();
        }
	}

	return TRUE;
}

BOOL DxEffCharData::SetPiece ( DxSkinPiece *pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 *pDir, float fTarDir )
{
	HRESULT hr;
	// Note : 타입에 구애받지 않는다.
	//if ( pSkinPiece->m_emType != m_emType )		return FALSE;

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for ( ; iter!=iter_end; ++iter )
	{
		DWORD dwTypeID;
		SEFFDATA *pEffData = (*iter);

		//	Note : 스트림 초기 위치를 리셋.
		pEffData->m_byteStream.ResetIter ();

		pEffData->m_byteStream >> dwTypeID;

		DxEffChar* pNewEffChar = DxEffCharMan::GetInstance().CreateEffInstance ( dwTypeID );
		if ( !pNewEffChar )		return FALSE;

		pNewEffChar->SetFileName ( m_strFileName );
		pNewEffChar->SetLinkObj ( NULL, pSkinPiece );
		if ( pDir )
			pNewEffChar->SetDirection ( pDir, fTarDir );
		hr = pNewEffChar->LoadFile ( pEffData->m_byteStream, pd3dDevice );
		if (FAILED(hr))
		{
			SAFE_DELETE(pNewEffChar);
			continue;
		}

		pSkinPiece->AddEffList ( pNewEffChar );
	}

	return TRUE;
}

BOOL DxEffCharData::LoadFile ( sc::BaseStream &SFile )
{
	DWORD dwType;
	DWORD dwSize;
	SFile >> dwType;
	m_emType = EMPIECECHAR(dwType);

	SFile >> dwSize;

	for ( DWORD i=0; i<dwSize; i++ )
	{
		DWORD dwBuffSize;
		SFile >> dwBuffSize;
		LPBYTE pBuffer = new BYTE[dwBuffSize];
		
		SFile.ReadBuffer ( pBuffer, dwBuffSize );

		SEFFDATA* pEFFDATA = new SEFFDATA;
		pEFFDATA->m_byteStream.WriteBuffer ( pBuffer, dwBuffSize );
		m_listEffData.push_back ( pEFFDATA );

		delete[] pBuffer;
	}

	return TRUE;
}

BOOL DxEffCharData::SaveFile ( sc::SerialFile &SFile )
{
	SFile << DWORD(m_emType);
	SFile << DWORD(m_listEffData.size());

	EFFDATALIST_ITER iter = m_listEffData.begin();
	EFFDATALIST_ITER iter_end = m_listEffData.end();
	for (; iter!=iter_end; ++iter)
	{
		SEFFDATA *pEffData = (*iter);

		LPBYTE pBuff;
		DWORD dwSize;
		pEffData->m_byteStream.GetBuffer( pBuff, dwSize );

		SFile << dwSize;
		SFile.WriteBuffer ( pBuff, dwSize );
	}

	return TRUE;
}

BOOL DxEffCharData::LoadFile(const std::string& FileName)
{
	m_strFileName = FileName;

	std::string strPathName(DxEffcharDataMan::GetInstance().GetPath());
	strPathName += FileName;

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strEFFECT_CHAR_ZIPFILE,
            strPathName.c_str(), 
            FileName));

	if (!pBStream.get())
		return FALSE;

	sc::BaseStream& SFile = *pBStream;

	return LoadFile(SFile);
}

BOOL DxEffCharData::SaveFile ( const char* szFile )
{
	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	sc::SerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_WRITE, strPathName.c_str() );
	if ( !bOk )		return FALSE;

	return SaveFile ( SFile );
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

BOOL DxEffCharDataArray::SetSkinPart ( PDXCHARPART pCharPart, EMPIECECHAR emType, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pCharPart )	return FALSE;

	if ( pCharPart->IsActiveCharPart() )
	{
		if ( m_pEffCharDataArray[emType] )
		{
			m_pEffCharDataArray[emType]->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
		}
	}
	else
	{
		pCharPart->m_pBackUpData = m_pEffCharDataArray[emType];
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SetSkinChar ( DxSkinChar *pSkinChar, LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 *pDir, float fTarDir )
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
		if ( pCharPart && pCharPart->m_rMeshContainer && m_pEffCharDataArray[i] )
		{
			m_pEffCharDataArray[i]->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
		}
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SetSkinCharData ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		DxSkinPieceBase* pSkinPiece = pSkinCharData->GetPiece( pd3dDevice, i );
		if ( pSkinPiece ) 
		{
			// 알맞는 Slot 찾기
			// cps 로 chf 를 만들어서 셋팅 후 그 cps의 셋팅 부위를 변경하고, chf 를 변경 안하면 변경된 셋팅부위가 아닌 이전 셋팅부위에 설정되는 문제가 발생함.
			// 그걸 보완하기 위해 추가함.
			DWORD dwSlot = pSkinPiece->GetSlot();

			if ( m_pEffCharDataArray[dwSlot] )
			{
				switch( pSkinPiece->m_emSkinPieceType )
				{
				case DxSkinPieceBase::SKIN_PIECE:
					m_pEffCharDataArray[dwSlot]->SetPiece ( MIN_BOOST::polymorphic_downcast<DxSkinPiece*>( pSkinPiece ), pd3dDevice );
					break;
				};
			}
		}
	}

	return TRUE;
}

BOOL DxEffCharDataArray::Import ( DxSkinCharData *pSkinCharData, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp ();

	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		DxSkinPieceBase* pSkinPieceBase = pSkinCharData->GetPiece( pd3dDevice, i );
		if ( pSkinPieceBase )
		{
			// 알맞는 Slot 찾기
			// cps 로 chf 를 만들어서 셋팅 후 그 cps의 셋팅 부위를 변경하고, chf 를 변경 안하면 변경된 셋팅부위가 아닌 이전 셋팅부위에 설정되는 문제가 발생함.
			// 그걸 보완하기 위해 추가함.
			DWORD dwSlot = pSkinPieceBase->GetSlot();

			DxEffCharData *pEffCharData = new DxEffCharData;
			switch( pSkinPieceBase->m_emSkinPieceType )
			{
			case DxSkinPieceBase::SKIN_PIECE:
				DxSkinPiece* pSkinPiece = MIN_BOOST::polymorphic_downcast<DxSkinPiece*>( pSkinPieceBase );
				pEffCharData->Import ( static_cast<EMPIECECHAR>(dwSlot), pSkinPiece->m_vecEFFECT );
				break;
			};
			
			m_pEffCharDataArray[dwSlot] = pEffCharData;
		}
	}

	return TRUE;
}

void DxEffCharDataArray::ConvertTPoint( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		if ( !m_pEffCharDataArray[i] )	continue;

		m_pEffCharDataArray[i]->ConvertTPoint( pd3dDevice );
	}
}

void DxEffCharDataArray::CleanUp ()
{
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		SAFE_DELETE(m_pEffCharDataArray[i]);
	}
}


BOOL DxEffCharDataArray::LoadFile(const std::string& FileName)
{
	m_strFileName = FileName;

	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += FileName;

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strEFFECT_CHAR_ZIPFILE,
            strPathName.c_str(), 
            FileName));

	if (!pBStream.get())
		return FALSE;

	sc::BaseStream& SFile = *pBStream;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; i++ )
	{
		BOOL bExit;
		SFile >> bExit;
		if (!bExit)
            continue;

		DxEffCharData *pEffCharData = new DxEffCharData;
		pEffCharData->LoadFile ( SFile );
		pEffCharData->SetFileName ( m_strFileName );

		EMPIECECHAR emPieceType = pEffCharData->GetPieceType();

		GASSERT(!m_pEffCharDataArray[emPieceType]);
		m_pEffCharDataArray[emPieceType] = pEffCharData;
	}

	return TRUE;
}

BOOL DxEffCharDataArray::SaveFile ( const char* szFile )
{
	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += szFile;

	sc::SerialFile SFile;
	BOOL bOk = SFile.OpenFile ( FOT_WRITE, strPathName.c_str() );
	if ( !bOk )		return FALSE;

	SFile << DWORD(PIECE_SAVE_SIZE);

	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		if ( m_pEffCharDataArray[i] )	SFile << BOOL(TRUE);
		else							SFile << BOOL(FALSE);

		if ( !m_pEffCharDataArray[i] )	continue;

		m_pEffCharDataArray[i]->SaveFile ( SFile );
	}

	return TRUE;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

HRESULT DxEffcharDataMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return TRUE;
}

BOOL DxEffcharDataMan::PutEffect ( DxCharPart *pCharPart, const char* szEffFile,
								  const D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pCharPart )	return FALSE;

	DxEffCharData *pEffCharData = NULL;

	EFFDATA_MAP_ITER iter = m_mapEffData.find ( std::string(szEffFile) );
	if ( iter == m_mapEffData.end() )
	{
		pEffCharData = new DxEffCharData;
		BOOL bOk = pEffCharData->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharData;
			return FALSE;
		}

		m_mapEffData[std::string(szEffFile)] = pEffCharData;
	}
	else
	{
		pEffCharData = iter->second;
	}

	pEffCharData->SetPart ( pCharPart, m_pd3dDevice, pDir, fTarDir );

	return TRUE;
}


BOOL DxEffcharDataMan::PutEffect ( DxAttBoneLink *pAttLink, const char* szEffFile,
								  const D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pAttLink || !pAttLink->GetAttBone() )	return FALSE;

	char ext[_MAX_EXT] = {0};
	_splitpath( szEffFile, NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	// 만일 로드한 파일이 effskin 파일이면...
	if( strExt == _T(".EFFSKIN") )
	{
		PDXATTBONE	  pAttBone      = pAttLink->GetAttBone();
		DxEffCharData *pEffCharData = NULL;

		EFFDATA_MAP_ITER iter = m_mapEffData.find ( std::string(szEffFile) );
		if ( iter == m_mapEffData.end() )
		{
			pEffCharData = new DxEffCharData;
			BOOL bOk = pEffCharData->LoadFile ( szEffFile );
			if ( !bOk )
			{
				delete pEffCharData;
				return FALSE;
			}

			m_mapEffData[std::string(szEffFile)] = pEffCharData;
		}
		else
		{
			pEffCharData = iter->second;
		}
		
		for ( int i=0; i<ATTBONE_SIZE; i++ )
		{
			PDXCHARPART pCharPart = pAttBone->GetAttBonePiece(i);
			if ( pCharPart )
			{
				pEffCharData->SetPart ( pCharPart, m_pd3dDevice, pDir, fTarDir );
			}
		}
	}

	return TRUE;
};

BOOL DxEffcharDataMan::PutEffect ( DxSkinChar *pSkinChar, const char* szEffFile,
								  const D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pSkinChar )	return FALSE;

	DxEffCharDataArray *pEffCharDataArray = NULL;

	EFFDATA_ARRAYMAP_ITER iter = m_mapEffDataArray.find ( std::string(szEffFile) );
	if ( iter == m_mapEffDataArray.end() )
	{
		pEffCharDataArray = new DxEffCharDataArray;
		BOOL bOk = pEffCharDataArray->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharDataArray;
			return FALSE;
		}

		m_mapEffDataArray[std::string(szEffFile)] = pEffCharDataArray;
	}
	else
	{
		pEffCharDataArray = iter->second;
	}

	pEffCharDataArray->SetSkinChar ( pSkinChar, m_pd3dDevice, pDir, fTarDir );

	return TRUE;
}

BOOL DxEffcharDataMan::PutPassiveEffect ( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir )
{
	if ( !pSkinChar )	return FALSE;
	if ( !szEffFile || szEffFile[0]==NULL )	return E_FAIL;

	DxEffCharDataArray *pEffCharDataArray = NULL;

	EFFDATA_ARRAYMAP_ITER iter = m_mapEffDataArray.find ( std::string(szEffFile) );
	if ( iter == m_mapEffDataArray.end() )
	{
		pEffCharDataArray = new DxEffCharDataArray;
		BOOL bOk = pEffCharDataArray->LoadFile ( szEffFile );
		if ( !bOk )
		{
			delete pEffCharDataArray;
			return FALSE;
		}

		m_mapEffDataArray[std::string(szEffFile)] = pEffCharDataArray;
	}
	else
	{
		pEffCharDataArray = iter->second;
	}

	//	Note : 지속 이팩트의 경우 파트가 변경되어 이팩트가 삭제될 경우를 대비해서 개별 확인후 삽입 해준다.
	//
	for ( int i=0; i<PIECE_SAVE_SIZE; i++ )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
//		if ( pCharPart->IsActiveCharPart() )
		{
			BOOL bFind = pCharPart->FindEffList ( szEffFile );
			if ( !bFind )
			{
				pEffCharDataArray->SetSkinPart ( pCharPart, static_cast<EMPIECECHAR>(i), m_pd3dDevice, pDir, fTarDir );
			}
		}
	}

	return TRUE;
}

void DxEffcharDataMan::OutEffect ( DxSkinChar *pSkinChar, std::string strEffFile )
{
	if ( !pSkinChar )	return;

	for ( int i=0; i<PIECE_SAVE_SIZE; ++i )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece(i);
		pCharPart->DelEffList ( strEffFile.c_str() );
	}
}

void DxEffcharDataMan::CleanUp ()
{
	std::for_each ( m_mapEffData.begin(), m_mapEffData.end(), std_afunc::DeleteMapObject() );
	m_mapEffData.clear ();

	std::for_each ( m_mapEffDataArray.begin(), m_mapEffDataArray.end(), std_afunc::DeleteMapObject() );
	m_mapEffDataArray.clear ();
}




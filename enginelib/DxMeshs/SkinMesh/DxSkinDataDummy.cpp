#include "pch.h"
#include <algorithm>
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../G-Logic/GLogic.h"
#include "../../Common/stlFunctions.h"
#include "DxSkinCharData.h"
#include "DxSkinDataDummy.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxSkinDataDummy::DxSkinDataDummy(void)
{
	CleanUp ();
}

DxSkinDataDummy::~DxSkinDataDummy(void)
{
	CleanUp ();
}

void DxSkinDataDummy::CleanUp ()
{
	m_strFileName.clear();
	
	m_fScale = 1.0f;
	m_fHeightYPos = 0.0f;

	for ( int i=0; i<PIECE_SIZE; ++i )
	{
		m_strPIECE[i].clear();
	}

	std::for_each ( m_vecANIMINFO.begin(), m_vecANIMINFO.end(), std_afunc::DeleteObject() );
	m_vecANIMINFO.clear();

	m_strSkeleton.clear();
}

BOOL DxSkinDataDummy::LOAD_0100 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	char szSkeleton[MAX_PATH] = "";
	SFile.ReadBuffer ( szSkeleton, sizeof(char)*MAX_PATH );
	m_strSkeleton = szSkeleton;

	//	Note :에니메이션 읽기.
	char szAniName[ACF_SZNAME] = "";

	DWORD dwLenght;
	SFile >> dwLenght;

	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile.ReadBuffer ( szAniName, sizeof(char)*ACF_SZNAME );

		CString strCfgFile;
		CString strTEMP = szAniName;
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";
		
		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0101 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;

	SFile >> m_fScale;
	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )	continue;
		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";
		
		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0104 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";

		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	SFile >> m_fScale;

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0105 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )		
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";

		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0106 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SIZE_OLD )
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";

		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

BOOL DxSkinDataDummy::LOAD_0107_108 ( sc::BaseStream &SFile )
{
	DWORD dwPieceSize;

	SFile >> dwPieceSize;
	if ( dwPieceSize > PIECE_SAVE_SIZE )		
        return FALSE;

	//	Note : 조각들 로드.
	//
	BOOL bExit;
	for ( DWORD i=0; i<dwPieceSize; ++i )
	{
		SFile >> bExit;
		if ( !bExit )
		{
			m_strPIECE[i].clear();
			continue;
		}

		SFile >> m_strPIECE[i];
	}

	//	Note : 본파일 읽기.
	//
	SFile >> m_strSkeleton;

	//	Note :에니메이션 읽기.
	//
	DWORD dwLenght;
	SFile >> dwLenght;

	std::string strANIFILE;
	CString strCfgFile, strTEMP;
	for ( DWORD i=0; i<dwLenght; ++i )
	{
		SFile >> strANIFILE;

		strTEMP = strANIFILE.c_str();
		int nIndex = strTEMP.ReverseFind ( '.' );
		strCfgFile = strTEMP.Left(nIndex) + ".cfg";

		SANIMCONINFO *pANIMINFO = new SANIMCONINFO;
		if ( pANIMINFO->LoadFile ( strCfgFile.GetString() ) )
		{
			m_vecANIMINFO.push_back ( pANIMINFO );
		}
		else
		{
			SAFE_DELETE(pANIMINFO);
		}
	}

	SFile >> m_fScale;
	SFile >> m_fHeightYPos;

	//	Note : 이후 File Data 는 무시한다.
	//

	return TRUE;
}

// 스킨 파일을 읽음.
// BOOL DxSkinDataDummy::LoadFile () 에서도 같은 구조로 로딩을 행하므로 이 로드/새이브 부분이 바뀔시
// 병행해서 같이 수정이 필요하다.
BOOL DxSkinDataDummy::LoadFile(const std::string& FileName)
{
    std::string PathName(DxSkinCharDataContainer::GetInstance().GetPath());
	PathName.append(FileName);

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strSKINOBJ_ZIPFILE,
            PathName.c_str(), 
            FileName));

	if (!pBStream.get())
    {
        CDebugSet::ToLogFile(
            sc::string::format(
                "DxSkinDataDummy::LoadFile %1% %2% %3%",
                GLOGIC::strSKINOBJ_ZIPFILE,
                PathName,
                FileName));
		return FALSE;
    }

	sc::BaseStream& SFile = *pBStream;

	DWORD dwVer;
	SFile >> dwVer;

	switch (dwVer)
	{
	case 0x0100:
		LOAD_0100(SFile);
		break;

	case 0x0101:
	case 0x0102:
	case 0x0103:
		LOAD_0101(SFile);
		break;
	case 0x0104:
		LOAD_0104(SFile);
		break;
	case 0x0105:
		LOAD_0105(SFile);
		break;
	case 0x0106:
		LOAD_0106(SFile);
		break;
    case 0x0107:
	case 0x0108:
		LOAD_0107_108(SFile);
		break;

	default:
        CDebugSet::ToLogFile(
            sc::string::format(
            "DxSkinDataDummy::LoadFile Unknown Version %1% %2% %3% %4%",
            dwVer,
            GLOGIC::strSKINOBJ_ZIPFILE,
            PathName,
            FileName));
		return FALSE;
	};

	// 스킨 케릭터 이를 설정.
	m_strFileName = FileName;
	return TRUE;
}

SANIMCONINFO* DxSkinDataDummy::FindAniInfo ( const char* szName )
{
    size_t nSIZE = m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		if( m_vecANIMINFO[n]->m_strCfgFile == szName )
		{
			return m_vecANIMINFO[n];
		}
	}

	return NULL;
}

SANIMCONINFO* DxSkinDataDummy::FindAniInfo ( EMANI_MAINTYPE emMain, EMANI_SUBTYPE emSub )
{
	size_t nSIZE = m_vecANIMINFO.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		if( m_vecANIMINFO[n]->m_MainType == emMain && m_vecANIMINFO[n]->m_SubType == emSub )
		{
			return m_vecANIMINFO[n];
		}
	}

	return NULL;
}
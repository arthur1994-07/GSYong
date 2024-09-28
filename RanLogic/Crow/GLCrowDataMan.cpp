#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "./GLCrowData.h"
#include "./CrowAttack.h"
#include "./GLCrowDataMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
//	$1.	2003-01-17 Txx:xx	[최초 릴리즈]
////////////////////////////////////////////////////////////////////////////////////

const char* GLCrowDataMan::_FILEHEAD = "GLCROW";
const char* GLCrowDataMan::_LOGFILE = "_CrowData_Synce.txt";
const char* GLCrowDataMan::_STRINGTABLE = "CrowStrTable.txt";

GLCrowDataMan& GLCrowDataMan::GetInstance()
{
    static GLCrowDataMan Instance;
    return Instance;
}

GLCrowDataMan::GLCrowDataMan()
    : m_ppPCROWDATA(NULL)
    , m_pd3dDevice(NULL)
    , m_bModify(false)
{
    memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLCrowDataMan::~GLCrowDataMan ()
{
    FinalCleanup ();
}

HRESULT GLCrowDataMan::OneTimeSceneInit ()
{
    m_ppPCROWDATA = new PCROWDATA*[MAX_CROW_MID];
    for ( WORD i=0; i<MAX_CROW_MID; i++ )
    {
        m_ppPCROWDATA[i] = new PCROWDATA[MAX_CROW_SID];
        SecureZeroMemory ( m_ppPCROWDATA[i], sizeof(PCROWDATA)*MAX_CROW_SID );
    }

    return S_OK;
}

HRESULT GLCrowDataMan::FinalCleanup ()
{
    if ( m_ppPCROWDATA )
    {
        for ( WORD i=0; i<MAX_CROW_MID; i++ )
        {
            for ( WORD j=0; j<MAX_CROW_SID; j++ )
            {
                SAFE_DELETE(m_ppPCROWDATA[i][j]);
            }
            SAFE_DELETE_ARRAY(m_ppPCROWDATA[i]);
        }
        SAFE_DELETE_ARRAY(m_ppPCROWDATA);
    }

    return S_OK;
}

//! 해당 Talk 가 어디서 사용되는지 조사한다.
//! 전체 검색/속도 느림
SCROWDATA* GLCrowDataMan::IsUsingTalkFile(const std::string& TalkFile)
{
    for (WORD i=0; i<MAX_CROW_MID; ++i)
    {
        for (WORD j=0; j<MAX_CROW_SID; ++j)
        {
            SCROWDATA* pCrowData = m_ppPCROWDATA[i][j];
            if (pCrowData && (TalkFile ==pCrowData->GetTalkFile()))
                return pCrowData;
        }
    }
    return NULL;
}

SCROWDATA* GLCrowDataMan::GetMainCrowData ( WORD wMID )
{
    GASSERT(wMID<MAX_CROW_MID);

    return m_ppPCROWDATA[wMID][0];
}

SCROWDATA* GLCrowDataMan::GetCrowData(DWORD NpcId)
{
    return GetCrowData(SNATIVEID(NpcId));
}

SCROWDATA* GLCrowDataMan::GetCrowData(const SNATIVEID& sNativeID)
{   
    return GetCrowData(sNativeID.wMainID, sNativeID.wSubID);
}

SCROWDATA* GLCrowDataMan::GetCrowData(WORD wMID, WORD wSID)
{
    if (wMID >= MAX_CROW_MID || wSID>=MAX_CROW_SID)
        return NULL;
    else
        return m_ppPCROWDATA[wMID][wSID];
}

const std::string GLCrowDataMan::GetCrowName(SNATIVEID sNativeID)
{
    return GetCrowName(sNativeID.wMainID, sNativeID.wSubID);
}

const std::string GLCrowDataMan::GetCrowName(DWORD NpcId)
{
    return GetCrowName(SNATIVEID(NpcId));
}

const std::string GLCrowDataMan::GetCrowName(WORD wMID, WORD wSID)
{
    SCROWDATA* pCrowData = GetCrowData(wMID, wSID);
    if (pCrowData)
    {
        return pCrowData->GetNameStr();
    }
    else
    {
        static std::string TempStr("Null String");
        return TempStr;
    }
}

WORD GLCrowDataMan::FindFreeCrowDataMID ()
{
    for ( WORD i=0; i<MAX_CROW_MID; i++ )
    {
        if ( !m_ppPCROWDATA[i][0] )			return i;
    }

    return ITEMID_NOTFOUND;
}

WORD GLCrowDataMan::FindFreeCrowDataSID ( WORD wMID )
{
    GASSERT(wMID<MAX_CROW_MID);

    for ( WORD i=0; i<MAX_CROW_SID; i++ )
    {
        if ( !m_ppPCROWDATA[wMID][i] )	return i;
    }

    return ITEMID_NOTFOUND;
}

BOOL GLCrowDataMan::InsertCrowData ( WORD wMID, WORD wSID, PCROWDATA pCrowData, bool binner )
{
    GASSERT(wMID<MAX_CROW_MID);
    GASSERT(wSID<MAX_CROW_SID);

    if ( !binner )		m_bModify = true;

    if ( m_ppPCROWDATA[wMID][wSID] )
    {
        m_ppPCROWDATA[wMID][wSID]->Assign ( *pCrowData );
        return FALSE;
    }

    m_ppPCROWDATA[wMID][wSID] = new SCROWDATA;
    m_ppPCROWDATA[wMID][wSID]->Assign ( *pCrowData );

    return TRUE;
}

BOOL GLCrowDataMan::DeleteCrowData ( WORD wMID, WORD wSID )
{
    GASSERT(wMID<MAX_CROW_MID);
    GASSERT(wSID<MAX_CROW_SID);

    SAFE_DELETE ( m_ppPCROWDATA[wMID][wSID] );

    m_bModify = true;
    return TRUE;
}

BOOL GLCrowDataMan::DeleteCrowData ( SNATIVEID sNativeID )
{
    DeleteCrowData ( sNativeID.wMainID, sNativeID.wSubID );

    return TRUE;
}

HRESULT GLCrowDataMan::LoadFile(const std::string& FileName, BOOL bServer, bool bPastLoad, boost::function<void (std::string)> PercentCallbackFunc)
{
    FinalCleanup ();
    OneTimeSceneInit ();

    StringCchCopy(m_szFileName, MAX_PATH, FileName.c_str());

    std::string FullPath(GLOGIC::GetPath());
    FullPath.append(FileName);

    std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
        GLOGIC::UseLogicZipFile(), 
        GLogicData::GetInstance().GetGlogicZipFile(), 
        FullPath.c_str(), 
        FileName,
        false,
        GLOGIC::UseLogicPackFile()));

    if (!pBStream.get())
    {
        sc::writeLogError(
            sc::string::format(
            "GLCrowDataMan::LoadFile %1% %2% %3%",
            GLogicData::GetInstance().GetGlogicZipFile(),
            FullPath,
            FileName));
        return E_FAIL;
    }

    sc::BaseStream& SFile = *pBStream;

    char szFILEHEAD[FILETYPESIZE];
    DWORD dwFILEVER;
    SFile.GetFileType(szFILEHEAD, FILETYPESIZE, dwFILEVER);
    if (dwFILEVER == 0)
        return E_FAIL;
    SFile.SetEncode(dwFILEVER>=ENCODE_VER);

    DWORD dwVer;
    SFile >> dwVer;

    DWORD dwNumCrow;
    SFile >> dwNumCrow;

    for (DWORD i = 0; i < dwNumCrow; ++i)
    {
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

        SCROWDATA sCrowData;
        if (sCrowData.LoadFile(SFile, bServer, bPastLoad) == E_FAIL)
        {
            sc::writeLogError(
                sc::string::format(
                "GLCrowDataMan::LoadFile %1% %2%",
                dwNumCrow,
                SFile.GetFileName()));
            return E_FAIL;
        }

		if ( PercentCallbackFunc ){
			TCHAR _szOutString[256];
			::wsprintf(_szOutString, "%s ( %3d%% )\n%s", 
				FileName.c_str(), (i * 100) / dwNumCrow, sCrowData.GetName());
			PercentCallbackFunc(_szOutString);
		}

        InsertCrowData(sCrowData.sNativeID, &sCrowData, true);
    }
    return S_OK;
}


HRESULT GLCrowDataMan::SaveFile ( const char* szFile )
{
    StringCchCopy(m_szFileName,MAX_PATH,szFile);

    char szFullPath[MAX_PATH] = {0};
    StringCchCopy ( szFullPath, MAX_PATH, GLOGIC::GetPath() );
    StringCchCat ( szFullPath, MAX_PATH, m_szFileName );

    sc::SerialFile SFile;

    SFile.SetFileType ( _FILEHEAD, VERSION );
    if ( !SFile.OpenFile ( FOT_WRITE, szFullPath ) )	return E_FAIL;
    SFile.SetEncode ( true );

    SFile << VERSION;

    DWORD dwNumCrow = 0;
    int i = 0, j = 0;
    for ( i = 0; i < MAX_CROW_MID; i++ )
    {
        for ( j = 0; j < MAX_CROW_SID; j++ )
        {
            if ( m_ppPCROWDATA[i][j] )	dwNumCrow++;
        }
    }

    SFile << dwNumCrow;

    for ( i = 0; i < MAX_CROW_MID; i++ )
    {
        for ( j = 0; j < MAX_CROW_SID; j++ )
        {
            if ( m_ppPCROWDATA[i][j] )
            {
                m_ppPCROWDATA[i][j]->SaveFile ( SFile );
				const SNATIVEID sNID = m_ppPCROWDATA[i][j]->GetId();
				CString strKey;
				strKey.Format("CN_%03d_%03d", sNID.Mid(), sNID.Sid());
				if ( CGameTextMan::GetInstance().FindData(strKey.GetString(), CGameTextMan::EM_CROW_TEXT) == FALSE )
					CGameTextMan::GetInstance().InsertString(strKey.GetString(), strKey.GetString(), CGameTextMan::EM_CROW_TEXT);
            }
        }
    }

    m_bModify = false;
    return S_OK;
}

bool GLCrowDataMan::ValidData ()
{
    for ( int i = 0; i < MAX_CROW_MID; i++ )
        for ( int j = 0; j < MAX_CROW_SID; j++ )
        {
            if ( m_ppPCROWDATA[i][j] )		return true;
        }

        return false;
}

HRESULT GLCrowDataMan::SyncUpdateData ()
{
    CDebugSet::ClearFile ( _LOGFILE );

    for ( int i = 0; i < MAX_CROW_MID; i++ )
        for ( int j = 0; j < MAX_CROW_SID; j++ )
        {
            if ( m_ppPCROWDATA[i][j] )
            {
                if ( m_ppPCROWDATA[i][j]->GetSkinObjFile()==NULL )
                {
                    CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 스킨 파일이 지정되지 않았습니다.", i, j );
                    continue;
                }

                DxSkinDataDummy DataDummy;
                BOOL bOK = DataDummy.LoadFile ( m_ppPCROWDATA[i][j]->GetSkinObjFile() );
                if ( !bOK )
                {
                    CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 스킨 파일 [%s]이 존재하지 않습니다.", i, j, m_ppPCROWDATA[i][j]->GetSkinObjFile() );
                    continue;
                }

                DWORD dwSize = static_cast<DWORD>(m_ppPCROWDATA[i][j]->m_vecCrowAttack.size());
                for ( DWORD A=0; A<dwSize; A++ )
                {
                    SCROWATTACK &sCrowAttack = m_ppPCROWDATA[i][j]->m_vecCrowAttack[A];
                    if ( !sCrowAttack.bUsed )	continue;

                    if ( sCrowAttack.strAniFile.empty() )
                    {
                        CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d]에 '모션'이 없음.", i, j, A );

                        if ( A > 0 )
                        {
                            CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d]에 '모션'이 없어 '비활성화' 합니다.", i, j, A );
                            sCrowAttack.bUsed = FALSE;
                        }

                        continue;
                    }

                    SANIATTACK &sAniAttack = sCrowAttack.sAniAttack;
                    if ( SANIMCONINFO *pAnimConInfo=DataDummy.FindAniInfo ( sCrowAttack.strAniFile.c_str() ) )
                    {
                        sAniAttack.Assign ( pAnimConInfo );
                    }
                    else
                    {
                        CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s]에 '모션'이 없음.", i, j, A, sCrowAttack.strAniFile.c_str() );
                        if ( A > 0 )
                        {
                            CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s]에 '모션'이 없어 '비활성화' 합니다.", i, j, A, sCrowAttack.strAniFile.c_str() );
                            sCrowAttack.bUsed = FALSE;
                        }
                    }

                    if ( sAniAttack.m_wDivCount==0 )
                    {
                        CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d] 공격샛[%02d/%s] '모션'에 '타격지점'이 없음.", i, j, A, sCrowAttack.strAniFile.c_str() );
                    }
                }
            }
        }

        CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );

        return S_OK;
}

HRESULT	GLCrowDataMan::SyncStringTable()
{
    char	szFullPathFileName[MAX_PATH] = "";
    StringCchCopy ( szFullPathFileName, MAX_PATH, GLOGIC::GetPath() );
    StringCchCat ( szFullPathFileName, MAX_PATH, _STRINGTABLE );

    //DeleteFile ( szFullPathFileName );

    CString strBuffer1, strBuffer;
    //std::string strTemp;
    //std::string::size_type idx;

    strBuffer = "// File : Crow String Table\r\n";
    strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";

    for ( int i = 0; i < MAX_CROW_MID; i++ )
        for ( int j = 0; j < MAX_CROW_SID; j++ )
        {
            if ( !m_ppPCROWDATA[i][j] )		continue;

            //CItemNode *pITEMNODE = m_ppItem[i][j];
            //SITEM &sITEM = pITEMNODE->m_sItem;

            if( m_ppPCROWDATA[i][j]->m_szName[0] != _T('\0') )
            {
                strBuffer1.Format( _T("CN_%03d_%03d\t%s\r\n"), i, j, m_ppPCROWDATA[i][j]->m_szName );
                strBuffer += strBuffer1;
            }

            //if( !sITEM.sBasicOp.strComment.empty() )
            //{
            //	strTemp = sITEM.sBasicOp.strComment;
            //	idx = strTemp.find( "\r\n" );
            //	while ( idx != std::string::npos )
            //	{
            //		strTemp.replace( idx, 2, " " );
            //		idx = strTemp.find( "\r\n" );
            //	}
            //
            //	strBuffer1.Format( _T("CD_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
            //	strBuffer += strBuffer1;
            //}
        }
        //strBuffer.SetAt(strBuffer.GetLength()-1, NULL);
        //strBuffer.SetAt(strBuffer.GetLength()-2, NULL); // 마지막 \n은 제거한다.

        //DWORD dwLength = strBuffer.GetLength() + 1;
        //char * szEncode = new char[dwLength]; // 인코딩배열에 스트링 길이만큼 메모리 할당
        //StringCchCopy( szEncode, dwLength, strBuffer.GetString() ); // 인코딩배열에 스트링 복사
        //compbyte::encode ( (BYTE*)szEncode, dwLength ); // 인코딩

        CFile file;
        file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite ); // 파일 열기
        file.Write( strBuffer.GetString(), strBuffer.GetLength()-2 ); // 파일 쓰기
        file.Close();

        //delete [] szEncode;
        return S_OK;
}

HRESULT GLCrowDataMan::SaveCsvFile ( CWnd* pWnd )
{
    CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        "csv file (*.csv)|*.csv|", pWnd );

    dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
    if ( dlg.DoModal() != IDOK )
        return S_FALSE;

    std::fstream streamFILE;
    streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

    // Csv Head
    SCROWDATA::SaveCsvHead( streamFILE );

    for( int i=0; i<MAX_CROW_MID; ++i )
    {
        for( int j=0; j<MAX_CROW_SID; ++j )
        {
            if( !m_ppPCROWDATA[i][j] )
                continue;

            // Csv Data
            SCROWDATA *pCROWDATA = m_ppPCROWDATA[i][j];
            pCROWDATA->SaveCsv( streamFILE );
        }
    }

    streamFILE.close();

    return S_OK;
}

HRESULT GLCrowDataMan::LoadCsvFile ( CWnd* pWnd )
{
    CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        "csv file (*.csv)|*.csv|", pWnd );

    dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
    if ( dlg.DoModal() != IDOK )
        return S_FALSE;

    sc::CStringFile StrFile( 102400 );
    if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
        return S_FALSE;

    STRUTIL::ClearSeparator();
    STRUTIL::RegisterSeparator( "," );

    CString strLine;
    CStringArray StrArray;

    // Csv Head
    StrFile.GetNextLine( strLine );
    STRUTIL::StringSeparate( strLine, StrArray );
    int iHead = (int)StrArray.GetCount();	

    int iLine = 1;
    char szError[ 256 ];

    while( StrFile.GetNextLine( strLine ) )
    {
        ++iLine;

        STRUTIL::StringSeparate( strLine, StrArray );

        int iCount = (int)StrArray.GetCount();
        if( iCount < iHead )
        {
            StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
            MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
            return S_FALSE;
        }

        int iMID = atoi( StrArray[0] );
        int iSID = atoi( StrArray[1] );

        if( iMID >= MAX_CROW_MID || iSID >= MAX_CROW_SID )
            continue;

        if( !m_ppPCROWDATA[ iMID ][ iSID ] )
        {
            SCROWDATA pCROWDATA;
            if( !InsertCrowData( iMID, iSID, &pCROWDATA ) )
                continue;
        }

        // Csv Data
        SCROWDATA *pCROWDATA = m_ppPCROWDATA[ iMID ][ iSID ];
        pCROWDATA->LoadCsv( StrArray );
    }

    return S_OK;
}

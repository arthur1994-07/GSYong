#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringFormatW.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Hash/CHash.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"

#include "../GLogicData.h"
#include "./GLItemMan.h"
#include "./GLItemGrindingScript.h"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static GLItemMan* g_pInstance(NULL);

GLItemMan& GLItemMan::GetInstance()
{
	if ( !g_pInstance )
	{
		g_pInstance = new GLItemMan;
	}
	return *g_pInstance;
}

void GLItemMan::ReleaseInstance()
{
	SAFE_DELETE( g_pInstance );
}

const char* GLItemMan::_FILEHEAD = "GLITEM";

GLItemMan::GLItemMan () 
	: m_pd3dDevice(NULL)
	, m_ppItem(NULL)
	, m_bModify(false)
	, m_poolSINVENITEM("SINVENITEM")
{	
	m_strMoneyMesh[0] = "Money_1.X";
	m_strMoneyMesh[1] = "Money_2.X";
	m_strMoneyMesh[2] = "Money_3.X";

	//SecureZeroMemory ( m_pMoneyMesh, sizeof(DxSimMesh*)*MONEYMESH_NUM );

	m_sQITEMID[0] = SNATIVEID((WORD) 9,(WORD)1);
	m_sQITEMID[1] = SNATIVEID((WORD) 9,(WORD)2);
	m_sQITEMID[2] = SNATIVEID((WORD) 9,(WORD)3);
	m_sQITEMID[3] = SNATIVEID((WORD) 9,(WORD)4);
	m_sQITEMID[4] = SNATIVEID((WORD) 9,(WORD)6);
	m_sQITEMID[5] = SNATIVEID((WORD) 9,(WORD)29);
	m_sQITEMID[6] = SNATIVEID((WORD) 9,(WORD)30);
	m_sQITEMID[7] = SNATIVEID((WORD) 9,(WORD)31);
}

GLItemMan::~GLItemMan ()
{
	CleanUp ();

	m_vecSrcString.clear();
}

HRESULT GLItemMan::LoadFile(const std::string& FileName, boost::function<void (std::string)> PercentCallbackFunc)
{
	CleanUp();
	OneTimeSceneInit();

	m_FileName = FileName;

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
                "GLItemMan Load file %1% %2% %3%",
                GLogicData::GetInstance().GetGlogicZipFile(),
                FullPath,
                FileName));
		return E_FAIL;
    }

	sc::BaseStream& SFile = *pBStream;

	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( (dwFILEVER>=ENCODE_VER) );

	DWORD dwNum=0;

	SFile >> dwNum;

	m_vecSrcString.reserve( dwNum );

	for (DWORD i=0; i<dwNum; i++)
	{
		CItemNode sItemNode;

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		if ( !sItemNode.m_sItem.LoadFile(SFile) )
			return E_FAIL;			

		SITEM& Item = sItemNode.m_sItem;

		if ( PercentCallbackFunc ){
			TCHAR _szOutString[256];			
			::wsprintf(_szOutString, TEXT("%s ( %3d%% )\n%s"), 
				FileName.c_str(), (i * 100) / dwNum, Item.GetName());
			PercentCallbackFunc(_szOutString);
		}

        if (Item.GETAPPLYNUM() == 0 && 
			( Item.BasicType() != ITEM_STAGE_PASS && Item.BasicType() != ITEM_ENTER_NUM_CHARGE ) )
		{
            sc::writeLogError(
                sc::string::format(
                    "GLItemMan::LoadFile Invalid Apply Num Item %1% %2% %3% %4%/%5%",
                    FullPath,
                    SFile.GetFileName(),
                    Item.GetName(),
                    Item.sBasicOp.sNativeID.wMainID, 
                    Item.sBasicOp.sNativeID.wSubID));
		}

		// 연마 옵련 로드. (grinding option)
		{
			sItemNode.m_rGRINDING_SET = LoadGrindingOptData(Item.GetGrindingOptFile());
			if (!sItemNode.m_rGRINDING_SET && !Item.sGrindingOp.strGRINDER_OPT_FILE.empty() )
			{
				sc::writeLogError(
					sc::string::format(
					"GLItemMan::LoadFile - Item grind option file load failed %1% %2% %3%",
					dwNum, FullPath, Item.sGrindingOp.strGRINDER_OPT_FILE));
			}
		}


		// 랜덤 옵션 로드. (random option)
//		sItemNode.m_pRANDOM_SET = LoadRandomGenData(Item.GetRandomOptFile());
		sItemNode.m_rSetRandomOption = RandomOption::Manager::getInstance().find(Item.GetRandomOptFile());
		if ( (sItemNode.m_rSetRandomOption == NULL) && (strlen(Item.sRandomOpt.szNAME) > 0) )
		{
			sc::writeLogError(
				sc::string::format(
				"Item random option file load failed %1% %2% %3%",
				dwNum, FullPath, Item.sRandomOpt.szNAME));
		}

		// Split;
		{
			// *.genitem;
			if ( strlen(sItemNode.m_sItem.sSplit.szName) > 7 )
			{
				sItemNode.m_spDecomposeGenInfo = boost::shared_ptr<SGENINFO>(new SGENINFO);

				CGENITEM sGenItem;
				sGenItem.GetGenInfo( "GLItemMan::LoadFile", sItemNode.m_sItem.sSplit.szName, *sItemNode.m_spDecomposeGenInfo.get() );
			}
		}

		GASSERT(m_ppItem[Item.sBasicOp.sNativeID.wMainID][Item.sBasicOp.sNativeID.wSubID] == NULL);
		InsertItem(Item.sBasicOp.sNativeID, &sItemNode, true);

		if( Item.sBasicOp.bSearch )
			m_vecSrcString.push_back( Item.GetName() ); 
	}

	ItemGenerateSet();

	return S_OK;
}

HRESULT GLItemMan::SaveFile(const std::string& FileName)
{
	m_FileName = FileName;

    std::string FullPath(GLOGIC::GetPath());
	FullPath.append(m_FileName);

	sc::SerialFile SFile;
	SFile.SetFileType(_FILEHEAD, VERSION);
	if (!SFile.OpenFile(FOT_WRITE, FullPath.c_str()))
        return E_FAIL;
	SFile.SetEncode(true);

	DWORD dwNum=0;	
	for (int i=0; i<MAX_MID; ++i)
    {
	    for (int j=0; j<MAX_SID; ++j)
	    {
		    if (m_ppItem[i][j])
                dwNum++;
        }
	}

	SFile << dwNum;

	for (int i=0; i<MAX_MID; ++i)
	{
		for (int j=0; j<MAX_SID; ++j)
		{
			if (m_ppItem[i][j])	
				m_ppItem[i][j]->m_sItem.SaveFile(SFile);
		}
	}

	m_bModify = false;
	return S_OK;
}

HRESULT GLItemMan::SaveFileLua(const std::string& FileName)
{
    m_FileName = FileName;

    std::string FullPath(GLOGIC::GetPath());
    FullPath.append(m_FileName);
    
    sc::TxtFile LuaFile(FullPath, true, sc::TxtFile::UTF16LE);
    
    // Save Time
    CTime CurTime(CTime::GetCurrentTime());
    CString TimeStr = CurTime.Format("%c");
    std::string strTime(TimeStr.GetString());
    std::wstring wStrTIme = sc::string::ansiToUnicode(strTime);

    wchar_t ModifyComputerName[MAX_PATH] = {0};
    DWORD ModifyComputerNameSize = MAX_PATH;
    if (GetComputerNameW(ModifyComputerName, &ModifyComputerNameSize))
    {
        LuaFile.write(
            sc::string::wformat(
                L"-- Modify date %1% by %2%",
                wStrTIme,
                ModifyComputerName));
    }
    else
    {
        LuaFile.write(
            sc::string::wformat(
            L"-- Modify date %1%",
            wStrTIme));
    }

    LuaFile.write(L"ItemData = ");
    LuaFile.write(L"{");

    /*
    // Version
    LuaFile.write(
        sc::string::wformat(L"  version = %1%,", VERSION));

    DWORD dwNum=0;    
    for (int i=0; i<MAX_MID; ++i)
    {
        for (int j=0; j<MAX_SID; ++j)
        {
            if (m_ppItem[i][j])
                dwNum++;
        }
    }

    // Data Count
    LuaFile.write(sc::string::wformat(L"  count = %1%,", dwNum));
    LuaFile.write(L"  data  = ");
    LuaFile.write(L"  {");
    */

    for (int i=0; i<MAX_MID; ++i)
    {
        for (int j=0; j<MAX_SID; ++j)
        {
            if (m_ppItem[i][j])
            {
                m_ppItem[i][j]->m_sItem.SaveFileLua(LuaFile);
            }
        }
    }
    /*
    LuaFile.write(L"  },");
    */
    LuaFile.write(L"}");
    m_bModify = false;
    return S_OK;
}

HRESULT GLItemMan::SaveSearchList( const char* szFile )
{
	CString strBuffer, strTemp;
	strBuffer = "";

	for ( int i = 0; i < MAX_MID; i++ )
	{
		for ( int j = 0; j < MAX_SID; j++ )
		{
			if ( m_ppItem[i][j] && m_ppItem[i][j]->m_sItem.sBasicOp.bSearch )
			{
				strTemp.Format( _T("%s\r\n"), m_ppItem[i][j]->m_sItem.GetName() );
				strBuffer += strTemp;
			}
		}
	}

    sc::CRijndael oRijndael;
    oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::DEFAULT_KEY_LENGTH );

	int nLen = oRijndael.GetEncryptStringLength( strBuffer );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
    int nVersion = sc::CRijndael::VERSION;

	CFile file;
	if ( !file.Open( _T(szFile), CFile::modeCreate|CFile::modeWrite ) )	return false;	// 파일 열기
	file.Write( &nVersion, sizeof(int) );												// 버젼 쓰기
	file.Write( szBuffer, nLen );														// 파일 쓰기
	file.Close();

	delete [] szBuffer;

	return S_OK;
}


CItemNode* GLItemMan::GetItemNode ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_ppItem);

	return m_ppItem[wMID][wSID];
}

RandomOption::Entity* GLItemMan::GetItemRandomOpt ( SNATIVEID sNativeID )
{
	PITEMNODE pNODE = GetItemNode ( sNativeID.wMainID, sNativeID.wSubID );
	if (pNODE)
        return pNODE->m_rSetRandomOption;
    else
        return NULL;	    
}

SGRINDING_OPT* GLItemMan::GetItemGrindingOpt ( SNATIVEID sNativeID )
{
	PITEMNODE pNODE = GetItemNode ( sNativeID.wMainID, sNativeID.wSubID );
	if (pNODE)
		return pNODE->m_rGRINDING_SET;
	else
		return NULL;	    
}

SITEM* GLItemMan::GetItem(DWORD ItemId)
{
    return GetItem(SNATIVEID(ItemId));
}

SITEM* GLItemMan::GetItem(const SNATIVEID& sNativeID)
{ 
    return GetItem(sNativeID.wMainID, sNativeID.wSubID);
}

SITEM* GLItemMan::GetItem(WORD wMID, WORD wSID)
{
	if (!m_ppItem)
        return NULL;

	if (wMID==0xFFFF || wSID==0xFFFF || wMID>=MAX_MID || wSID>=MAX_SID)
        return NULL;

	if (m_ppItem[wMID][wSID])
        return &(m_ppItem[wMID][wSID]->m_sItem);
    else
	    return NULL;
}

const std::string& GLItemMan::GetItemName(const SNATIVEID& sNativeID)
{
    static std::string TempReturnStr("");
    const SITEM* pItem = GetItem(sNativeID);
    if (pItem)
    {
        TempReturnStr = pItem->GetNameStr();
        return TempReturnStr;
    }
    else
    {
        static std::string TempStr("");
        return TempStr;
    }
}

WORD GLItemMan::FindFreeMID ()
{
	GASSERT(m_ppItem);

	for (WORD i=0; i<MAX_MID; ++i)
	{
		if (!m_ppItem[i][0])
            return i;
	}
	return ITEMID_NOTFOUND;
}

WORD GLItemMan::FindFreeSID(WORD wMID)
{
	GASSERT(m_ppItem);
	GASSERT(wMID<MAX_MID);
	
	for (WORD i=0; i<MAX_SID; ++i)
	{
		if (!m_ppItem[wMID][i])
            return i;
	}
	return ITEMID_NOTFOUND;
}

BOOL GLItemMan::InsertItem ( WORD wMID, WORD wSID, PITEMNODE pItem, bool binner )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);

	if ( !binner )		m_bModify = true;

	if ( m_ppItem[wMID][wSID] )
	{
		*m_ppItem[wMID][wSID] = *pItem;
		return FALSE;
	}

	m_ppItem[wMID][wSID] = new CItemNode;
	*m_ppItem[wMID][wSID] = *pItem;

	//	해당 아이템 분류군에 아이템을 등록함.
	DWORD dwSpecID = pItem->m_sItem.sGenerateOp.dwSpecID;
	if ( dwSpecID < SPECID_NUM )
	{
		m_sGenItems[dwSpecID].m_vecItems.push_back ( pItem->m_sItem.sBasicOp.sNativeID );
	}

	return TRUE;
}

BOOL GLItemMan::ItemGenerateSet()
{
	srand( static_cast<unsigned int>( time( 0 ) ) );

	for (int i=0; i < SPECID_NUM; ++i)
	{
		SGENITEMS& sGenItems = m_sGenItems[i];

		WORD wSize = (WORD) sGenItems.m_vecItems.size();
		for (int s=0; s<SHUFFLE_NUM; ++s)
		{
			for (int ren=0; ren<wSize; ++ren)
                sGenItems.m_sRendom.push_back(ren);
		}

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		std::random_shuffle(sGenItems.m_sRendom.begin(), sGenItems.m_sRendom.end());
	}
	return TRUE;
}

SNATIVEID GLItemMan::RendomGenSpecID( DWORD dwSpecID )
{
	if ( dwSpecID >= SPECID_NUM )
    {
        sc::writeLogError( stringformat( "RendomGenSpecID - dwSpecID range error. MaxNum-%1%, dwSpecID-%2%",
            SPECID_NUM, dwSpecID ) );

        return NATIVEID_NULL();
    }

	SGENITEMS& sGenItems = m_sGenItems[dwSpecID];
	
	if ( sGenItems.m_sRendom.empty() )
        return NATIVEID_NULL();

    if ( sGenItems.m_wNowCur >= static_cast<WORD>(sGenItems.m_sRendom.size()) )
    {
        sc::writeLogError( stringformat( "RendomGenSpecID - sGenItems.m_wNowCur range error. MaxNum-%1%, dwSpecID-%2%",
            sGenItems.m_sRendom.size(), sGenItems.m_wNowCur ) );

        sGenItems.m_wNowCur = 0;

        return NATIVEID_NULL();
    }

	WORD wNow = sGenItems.m_sRendom[sGenItems.m_wNowCur];
	SNATIVEID sNativID = sGenItems.m_vecItems[wNow];
	
	++sGenItems.m_wNowCur;
	if ( sGenItems.m_wNowCur >= sGenItems.m_sRendom.size() )
		sGenItems.m_wNowCur = 0;

	return sNativID;
}

const GLItemMan::SGENITEMS& GLItemMan::GetGenSpecItems(DWORD dwSpecID)
{
	GASSERT(dwSpecID < SPECID_NUM);
	return m_sGenItems[dwSpecID];
}



//DxSkinPiece* GLItemMan::LoadSkinPiece ( WORD wMID, WORD wSID, EMCHARINDEX emIndex )
//{
//	GASSERT(wMID<MAX_MID);
//	GASSERT(wSID<MAX_SID);
//	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");
//
//	PITEMNODE pItem = GetItemNode ( wMID, wSID );
//	if ( !pItem )
//        return NULL;
//
//	if ( !pItem->m_pSkinPiece )
//	{
//		pItem->m_pSkinPiece[emIndex] = DxSkinPieceContainer::GetInstance().LoadPiece( pItem->m_sItem.GetWearingFile(emIndex), EMSCD_ALL_THREAD );
//	}
//
//	return pItem->m_pSkinPiece[emIndex];
//}

DxSimMesh* GLItemMan::GetFieldMesh ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);
	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");

	PITEMNODE pItem = GetItemNode ( wMID, wSID );
	if ( !pItem )	
		return NULL;

	if ( !pItem->m_FieldMeshRes.Update() )
	{
		pItem->m_FieldMeshRes = DxSimpleMeshMan::GetInstance().Load (
            pItem->m_sItem.GetFieldFile(),
            FALSE,
			FALSE,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT );
	}

	return pItem->m_FieldMeshRes.get();
}

//DxSimMesh* GLItemMan::GetInvenMesh ( WORD wMID, WORD wSID )
//{
//	GASSERT(wMID<MAX_MID);
//	GASSERT(wSID<MAX_SID);
//	GASSERT(m_pd3dDevice&&"장치가 초기화되지 않았음!");
//
//	PITEMNODE pItem = GetItemNode ( wMID, wSID );
//	if ( !pItem )	
//		return NULL;
//
//	if ( !pItem->m_InvenMeshRes.Update() )
//	{
//		pItem->m_InvenMeshRes = DxSimpleMeshMan::GetInstance().Load (
//            pItem->m_sItem.GetInventoryFile(),
//			FALSE,
//			TRUE,
//			TEXTURE_RESIZE_NONE,
//			TEXTURE_EFFECT );
//	}
//
//	return pItem->m_InvenMeshRes.get();
//}

HRESULT GLItemMan::OneTimeSceneInit ()
{
	//	Note : 아이탬 포인터 배열 생성.
	m_ppItem = new PITEMNODE*[MAX_MID];
	for ( WORD i=0; i<MAX_MID; i++ )
	{
		m_ppItem[i] = new PITEMNODE[MAX_SID];
		SecureZeroMemory ( m_ppItem[i], sizeof(PITEMNODE)*MAX_SID );
	}

	return S_OK;
}

HRESULT GLItemMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	for ( int i=0; i<MONEYMESH_NUM; i++ )
	{
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		m_MoneyMeshRes[i] = DxSimpleMeshMan::GetInstance().Load (
            m_strMoneyMesh[i].c_str(),
			FALSE,
			FALSE,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT );
	}

	return S_OK;
}

HRESULT GLItemMan::DeleteDeviceObjects ()
{
	if ( !m_ppItem )	return S_OK;

	for ( WORD wMID=0; wMID<MAX_MID; wMID++ )
	for ( WORD wSID=0; wSID<MAX_SID; wSID++ )
	{
		if ( m_ppItem[wMID][wSID] )
		{
			//if ( m_ppItem[wMID][wSID]->m_pSkinPiece )
			//{
			//	for ( int i=0; i<GLCI_NUM_ETC; i++ )
			//	{
			//		DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetWearingFile((EMCHARINDEX)i) );
			//		m_ppItem[wMID][wSID]->m_pSkinPiece[i] = NULL;
			//	}
			//}

			m_ppItem[wMID][wSID]->m_InvenMeshRes.Clear();
			m_ppItem[wMID][wSID]->m_FieldMeshRes.Clear();

			//if ( m_ppItem[wMID][wSID]->m_pInvenMesh )
			//{
			//	DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetInventoryFile() );
			//	m_ppItem[wMID][wSID]->m_pInvenMesh = NULL;
			//}

			//if ( m_ppItem[wMID][wSID]->m_pFieldMesh )
			//{
			//	DxSkinPieceContainer::GetInstance().ReleasePiece ( m_ppItem[wMID][wSID]->m_sItem.GetFieldFile() );
			//	m_ppItem[wMID][wSID]->m_pFieldMesh = NULL;
			//}
		}
	}

	//for ( int i=0; i<MONEYMESH_NUM; i++ )
	//{
	//	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	//	DxSimpleMeshMan::GetInstance().Release ( m_strMoneyMesh[i].c_str(), dwFVF );
	//}

	return S_OK;
}

HRESULT GLItemMan::FinalCleanup ()
{
	if ( !m_ppItem )	return S_OK;

	for ( WORD i=0; i<MAX_MID; i++ )
	{
		for ( WORD j=0; j<MAX_SID; j++ )
		{
			SAFE_DELETE(m_ppItem[i][j]);
		}
		SAFE_DELETE_ARRAY(m_ppItem[i]);
	}
	SAFE_DELETE_ARRAY(m_ppItem);	

	size_t nSIZE = m_sGrinding.size();
	for ( size_t i=0; i<nSIZE; ++i )
	{
		SAFE_DELETE(m_sGrinding[i].pSET);
	}
	m_sGrinding.clear();

	return S_OK;
}

BOOL GLItemMan::DeleteItem ( WORD wMID, WORD wSID )
{
	GASSERT(wMID<MAX_MID);
	GASSERT(wSID<MAX_SID);

	SAFE_DELETE ( m_ppItem[wMID][wSID] );

	m_bModify = true;
	return TRUE;
}

BOOL GLItemMan::DeleteItem ( SNATIVEID sNativeID )
{
	DeleteItem ( sNativeID.wMainID, sNativeID.wSubID );

	return TRUE;
}

const char* GLItemMan::_LOGFILE = "_ItemData_Synce.txt";
const char* GLItemMan::_STRINGTABLE = "ItemStrTable.txt";
const char* GLItemMan::_SEARCHLIST = "SearchItemList.txt"; // by luxes.

static CString	GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}

	return strFullPath;
}

static BOOL ISFILE_OBJ_EXIST ( CString strFile )
{
	CString strFullName;
	strFullName = DxSimpleMeshMan::GetInstance().GetPath().c_str() + strFile;

	BOOL bEXIST = FALSE;
 	FILE *file = _fsopen ( strFullName.GetString(), "r", _SH_DENYNO );
	if ( file )
	{
		bEXIST = TRUE;
		fclose(file);
	}

	return bEXIST;
}

static BOOL ISFILE_CPS_EXIST ( CString strFile )
{
	CString strFullName;
	strFullName = DxSkinPieceContainer::GetInstance().GetPath() + strFile;

	BOOL bEXIST = FALSE;
 	FILE *file = _fsopen ( strFullName.GetString(), "r", _SH_DENYNO );
	if ( file )
	{
		bEXIST = TRUE;
		fclose(file);
	}

	return bEXIST;
}

bool GLItemMan::ValidData() const
{
	if (!m_ppItem)
        return false;

	for (int i=0; i<MAX_MID; ++i)
    {
	    for (int j=0; j<MAX_SID; ++j)
	    {
		    if (m_ppItem[i][j])
                return true;
        }
	}
	return false;
}

HRESULT GLItemMan::SyncUpdateData(const SUBPATH* pPath)
{
	//	Note : 데이터 동기화 점검을 위한 초기화.
	//
	CString strAppPath;
	char szAppPath[MAX_PATH], szFullPath[MAX_PATH];
	strAppPath = GetAppPath ();
	StringCchCopy ( szAppPath, MAX_PATH, strAppPath.GetString () );

	// Note : SimpleMesh 기본 폴더 지정
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileObject().c_str()); //SUBPATH::OBJ_FILE_OBJECT );
	DxSimpleMeshMan::GetInstance().SetPath ( szFullPath );

	//	Note : SkinObject 기본 폴더 지정.
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, pPath->ObjFileSkinObject().c_str()); //SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneLink::SetPath ( szFullPath );

	int nCHAR_FLAGS[GLCI_NUM_ACTOR] =
	{
		GLCC_FIGHTER_M,
		GLCC_ARMS_M,
		GLCC_ARCHER_W,
		GLCC_SPIRIT_W,
		GLCC_EXTREME_M,
		GLCC_EXTREME_W,
		GLCC_FIGHTER_W,
		GLCC_ARMS_W,
		GLCC_ARCHER_M,
		GLCC_SPIRIT_M,
		GLCC_SCIENTIST_M,
		GLCC_SCIENTIST_W,
		GLCC_ASSASSIN_M,
		GLCC_ASSASSIN_W,
		GLCC_TRICKER_M,
		GLCC_TRICKER_W,
        GLCC_ETC_M,
        GLCC_ETC_W,
		GLCC_ACTOR_M,
		GLCC_ACTOR_W,
	};

	char strCHAR_FLAGS[GLCI_NUM_ACTOR][256] =
	{
		"FighterM",		//격투부
		"KnightM",		//검도부
		"ArcherW",		//양궁부
		"MagicianW",	//기예부
		"ExtremeM",		//극강부 남
		"ExtremeW",	//극강부 여
		"FighterW",		//격투부
		"KnightW",		//검도부
		"ArcherM",		//양궁부
		"MagicianM",	//기예부
		"SicentistM",	//과학부
		"SicentistW",	//과학부
        "AssassinM",	//인술부
        "AssassinW",	//인술부
		"TrickerM",		//마술부
		"TrickerW",		//마술부
        "EtcM",			//기타
        "EtcW",			//기타
		"ActorM",		//연극부
		"ActorW",		//연극부
	};

	GASSERT ( m_ppItem );
	if ( !m_ppItem )	return E_FAIL;

	CDebugSet::ClearFile ( _LOGFILE );

	for ( int i = 0; i < MAX_MID; i++ )
	{
		for ( int j = 0; j < MAX_SID; j++ )
		{
			if ( !m_ppItem[i][j] )		continue;

			CItemNode *pITEMNODE = m_ppItem[i][j];
			SITEM &sITEM = pITEMNODE->m_sItem;

			if ( sITEM.GetFieldFile() )
			{	//필드 메시가 설정되지 않음
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not setting field mesh", i, j, sITEM.GetName() );
			}
			else if ( !ISFILE_OBJ_EXIST(sITEM.GetFieldFile()) )
			{	//설정한 필드메시가 없음
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not search field mesh [%s]", i, j, sITEM.GetName(), sITEM.GetFieldFile() );
			}

			if ( sITEM.GetInventoryFile() )
			{	//'인벤메시'가 설정되지 않음
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not setting inven mesh", i, j, sITEM.GetName() );
			}
			else if ( !ISFILE_OBJ_EXIST(sITEM.GetInventoryFile()) )
			{	//설정한 '인벤메시' 없음
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - not search inven mesh [%s]", i, j, sITEM.GetName(), sITEM.GetInventoryFile() );
			}

			if ( sITEM.sSuitOp.emHand==HAND_LEFT )
			{	//왼손 도구
				CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s] - left hand equip", i, j, sITEM.GetName() );
			}

			if ( sITEM.sBasicOp.emItemType!=ITEM_SUIT )	continue;

			for ( int nClass=0; nClass<GLCI_NUM_ACTOR; ++nClass )
			{
				if ( sITEM.sBasicOp.dwReqCharClass & nCHAR_FLAGS[nClass] )
				{
					if ( sITEM.GetWearingFile((EMCHARINDEX)nClass) )
					{	//'착용메시'가 설정되지 않음
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s][%s] - not setting wear mesh ", i, j, sITEM.GetName(), strCHAR_FLAGS[nClass] );
					}
					else if ( !ISFILE_CPS_EXIST(sITEM.GetWearingFile((EMCHARINDEX)nClass)) )
					{	//설정한 '착용메시' 없음
						CDebugSet::ToFile ( _LOGFILE, "[%03d][%03d][%s][%s] - not search wear mesh [%s]", i, j, sITEM.GetName(), strCHAR_FLAGS[nClass], sITEM.GetWearingFile((EMCHARINDEX)nClass) );
					}
				}
			}
		}
	}

	CDebugSet::ToFile ( _LOGFILE, "---------------------sync-completion--------------------------------" );	

	return S_OK;
}

HRESULT GLItemMan::SyncStringTable()
{
	GASSERT(m_ppItem);
	if (!m_ppItem)
        return E_FAIL;

	char szFullPathFileName[MAX_PATH] = "";
	StringCchCopy(szFullPathFileName, MAX_PATH, GLOGIC::GetPath());
	StringCchCat(szFullPathFileName, MAX_PATH, _STRINGTABLE);

	//DeleteFile ( szFullPathFileName );

	CString strBuffer1, strBuffer;
	std::string strTemp;
	std::string::size_type idx;

	strBuffer = "// File : Item String Table\r\n";
	strBuffer += "// Note : 키(ID)와 내용은 반드시 탭으로 구분되어야 합니다.\r\n//\r\n";
	
	for ( int i = 0; i < MAX_MID; i++ )
	for ( int j = 0; j < MAX_SID; j++ )
	{
		if ( !m_ppItem[i][j] )		continue;

		CItemNode *pITEMNODE = m_ppItem[i][j];
		SITEM &sITEM = pITEMNODE->m_sItem;

		if( !sITEM.sBasicOp.strName.empty() )
		{
			strBuffer1.Format( _T("IN_%03d_%03d\t%s\r\n"), i, j, sITEM.sBasicOp.strName.c_str() );
			strBuffer += strBuffer1;
		}
		
		if( !sITEM.sBasicOp.strComment.empty() )
		{
			strTemp = sITEM.sBasicOp.strComment;
			idx = strTemp.find( "\r\n" );
			while ( idx != std::string::npos )
			{
				strTemp.replace( idx, 2, " " );
				idx = strTemp.find( "\r\n" );
			}
		
			strBuffer1.Format( _T("ID_%03d_%03d\t%s\r\n"), i, j, strTemp.c_str() );
			strBuffer += strBuffer1;
		}
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

std::string GLItemMan::SaveSqlFile()
{
    if (!m_ppItem)
        return std::string("Item data null");

    std::string Path(sc::getAppPath());
    Path.append("\\ItemList.sql");    
    
    sc::TxtFile SqlFile(Path, true, sc::TxtFile::ANSI);
    
    SqlFile.write("DELETE ItemList");
    SqlFile.write("GO");
    
    for (int i=0; i<MAX_MID; ++i)
    {
        for (int j=0; j<MAX_SID; ++j)
        {
            CItemNode* pItem = m_ppItem[i][j];
            if (!pItem)
                continue;
            
            std::string ItemName(pItem->m_sItem.GetName());
            sc::string::replaceForDb(ItemName);
            SNATIVEID ItemId = pItem->m_sItem.BasicId();
            SqlFile.write(
                sc::string::format(
                    "INSERT INTO ItemList(ItemMain, ItemSub, ItemName) VALUES(%1%, %2%, '%3%')",
                    ItemId.wMainID,
                    ItemId.wSubID,
                    ItemName));
        }
    }
    SqlFile.write("GO");
    std::string FileName = SqlFile.FileName();
    return FileName;
}

std::string GLItemMan::SaveSqlFileWithPileNum()
{
	if ( !m_ppItem )
		return std::string( "Item data null" );

	std::string Path( sc::getAppPath() );
	Path.append( "\\ItemListWithPileNum.sql" );    

	sc::TxtFile SqlFile( Path, true, sc::TxtFile::ANSI );

	SqlFile.write( "DELETE ItemListWithPileNum" );
	SqlFile.write( "GO" );

	for ( int i = 0; i < MAX_MID; ++i )
	{
		for ( int j = 0; j < MAX_SID; ++j )
		{
			CItemNode* pItem = m_ppItem[i][j];
			if ( !pItem )
				continue;

			std::string ItemName( pItem->m_sItem.GetName() );
			sc::string::replaceForDb( ItemName );
			SNATIVEID ItemId = pItem->m_sItem.BasicId();
			WORD PileNum = pItem->m_sItem.sDrugOp.wPileNum;
			SqlFile.write( sc::string::format(
				"INSERT INTO ItemListWithPileNum( ItemMain, ItemSub, ItemName, ItemPileNum ) VALUES( %1%, %2%, '%3%', %4% )",
				ItemId.wMainID, 
				ItemId.wSubID, 
				ItemName, 
				PileNum ) );
		}
	}
	SqlFile.write( "GO" );
	std::string FileName = SqlFile.FileName();
	return FileName;
}

HRESULT GLItemMan::SaveCsvFile(CWnd* pWnd)
{
	CFileDialog dlg(
        FALSE,
        ".csv",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|",
        pWnd);

    // 2011-06-07 Jgkim
    // 폴더나 파일명이 한글이면 fstream 문제발생함
    // 임시로 setlocale 을 이용해서 문제 해결
    // 추후에 fstream 을 이용하지 않는 방식으로 수정해야 함
    setlocale(LC_ALL, "Korean");

    std::string InitPath(GLOGIC::GetPath());
	dlg.m_ofn.lpstrInitialDir = InitPath.c_str();
	if (dlg.DoModal() != IDOK)
		return S_FALSE;

    std::string FileName(dlg.GetPathName().GetString());
	std::fstream streamFILE;
    // 폴더나 파일명이 한글이면 fstream 문제발생함
	streamFILE.open(FileName.c_str(), std::ios_base::out);

	// Csv Head
	SITEM::SaveCsvHead( streamFILE );

	for (int i=0; i<MAX_MID; ++i)
	{
		for (int j=0; j<MAX_SID; ++j)
		{
			if (m_ppItem[i][j])
            {
                // Csv Data
			    CItemNode *pITEMNODE = m_ppItem[i][j];
			    SITEM &sITEM = pITEMNODE->m_sItem;
			    sITEM.SaveCsv(streamFILE);
            }
		}
	}

	streamFILE.close();
	return S_OK;
}

HRESULT GLItemMan::LoadCsvFile ( CWnd* pWnd )
{
	CleanUp();
	OneTimeSceneInit();

	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", pWnd );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return S_FALSE;
	
	sc::CStringFile StrFile( 10240 );
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

		STRUTIL::StringSeparate( strLine, StrArray, TRUE );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			StringCchPrintf( szError, 256, "%d Line = include blank column, next item load failed", iLine );
			MessageBox( pWnd->GetSafeHwnd(), _T(szError), _T("Fail"), MB_OK );
			return S_FALSE;
		}

		int iMID = atoi( StrArray[0] );
		int iSID = atoi( StrArray[1] );

		if( iMID >= MAX_MID || iSID >= MAX_SID )
			continue;

		if( !m_ppItem[ iMID ][ iSID ] )
		{
			CItemNode pItemNode;
			if( !InsertItem( iMID, iSID, &pItemNode ) )
				continue;
		}

		// Csv Data
		CItemNode *pITEMNODE = m_ppItem[ iMID ][ iSID ];
		SITEM &sITEM = pITEMNODE->m_sItem;
		sITEM.LoadCsv( StrArray );
	}

	return S_OK;
}

// #define USAGE_RANDOM_OPT_SE
// #ifdef USAGE_RANDOM_OPT_SE
// //#ifdef DAUMTESTPARAM	// 한국 테섭에서만 2차 랜덤 옵션 파일 사용
// 	RandomOption::Entity* GLItemMan::LoadRandomGenData(const char* szName)
// 	{
// 		if (!szName)
// 			return NULL;
// 
// 		const unsigned int _sizeName(::strlen(szName));
// 		if ( _sizeName == 0 )
// 			return NULL;
// 
// 		RandomDataListMap& _mapRandomDataList(this->mapRandomDataList);
// 
// 		const unsigned int _hashKey(HASH::generateHashKey(szName, _sizeName));
// 		RandomDataListMapIterator _iteratorRandomData(_mapRandomDataList.find(_hashKey));
// 		if ( _iteratorRandomData != _mapRandomDataList.end() )
// 		{
// 			RandomDataList& _listRandomData(_iteratorRandomData->second);
// 			for ( RandomDataListIterator _iteratorRandomData(_listRandomData.begin()); _iteratorRandomData != _listRandomData.end(); ++_iteratorRandomData )
// 			{
// 				SRANDOM_DATA& _randomData(*_iteratorRandomData);
// 				if ( ::strcmp(szName, _randomData.szNAME) == 0 )
// 					return _randomData.pSET;
// 			}
// 			return NULL;
// 		}
// 		else
// 		{
// 			RandomDataList _listRandomData;
// 			_mapRandomDataList.insert(RandomDataListValue(_hashKey, _listRandomData));
// 		}
// 		_iteratorRandomData = _mapRandomDataList.find(_hashKey);
// 		RandomDataList& _listRandomData(_iteratorRandomData->second);
// 
// 		std::string strPath(GLOGIC::GetServerPath());
// 		strPath.append(szName);
// 
// 		SRANDOM_DATA sDATA;
// 		StringCchCopy( sDATA.szNAME, RandomOption::LENGTH_NAME, szName );
// 		sDATA.pSET = new SRANDOM_GEN;
// 		SRANDOM_GEN &sGEN = *sDATA.pSET;
// 
// 		gltexfile cFILE;
// 		cFILE.reg_sep("\t ,");
// 
// 		if (GLOGIC::UseLogicZipFile())
// 			cFILE.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);
// 
// 		if (!cFILE.open(strPath, true))
// 		{
//             sc::writeLogError(
//                 sc::string::format(
//                     "GLItemMan::LoadRandomGenData %1%", strPath));			
// 			SAFE_DELETE(sDATA.pSET);
// 			return NULL;
// 		}
// 
// 		cFILE.getflag( "fRATE", 1, 1, sGEN.fRATE, true );
// //		cFILE.getflag( "dwSET_NUM", 1, 1, sGEN.dwSET_NUM, true );
// 		cFILE.getflag( "D_point", 1, 1, sGEN.fD_point, true );
// 		cFILE.getflag( "S_value", 1, 1, sGEN.fS_value, true );
//         cFILE.getflag( "bShufle", 1, 1, sGEN.bShufle, true );
// 
// 		TCHAR strObj[2][4] = { "NEW", "REB" };
// 		TCHAR szOPTION[64] = {0};
// 		TCHAR szOPT_SE[64] = {0};
// 
// 		INT iRandomOp = 0;
//  		for ( INT i=0; i<2; ++i )
//  		{
// 			iRandomOp = 0;
// 			do 
// 			{
// 				_snprintf_s( szOPTION, 64, _TRUNCATE, "%s_OPTION_%u", strObj[i], iRandomOp+1 );
// 				if ( cFILE.findflag(szOPTION) == NULL )
// 					break; // 키워드가 존재 하지 않으면 빠져 나간다;
// 
// 				// SRANDOM_OPT_SET* pSET = i == 1 ? &sGEN.sSETR[iRandomOp] : &sGEN.sSET[iRandomOp];
// 				SRANDOM_OPT_SET sSettingData;
// 				cFILE.getflag( szOPTION, 1, 3, sSettingData.fRATE, true );
// 				cFILE.getflag( szOPTION, 2, 3, sSettingData.wTYPE, true );
// 				cFILE.getflag( szOPTION, 3, 3, sSettingData.wSECT, true );
// 
// 				if( sSettingData.wSECT <= 0 )
// 				{
// 					sc::writeLogError(
// 						sc::string::format(
// 						"GLItemMan::LoadRandomGenData(), %1%", szName));
// 					SAFE_DELETE( sDATA.pSET );
// 					return NULL;
// 				}
// 
// 				sSettingData.pSECT = new SRANDOM_OPT_SECTION[ sSettingData.wSECT ];
// 				for( WORD k=0; k<sSettingData.wSECT; ++k )
// 				{
// 					_snprintf_s( szOPT_SE, 64, _TRUNCATE, "%s_OPT_SE_%u_%u", strObj[i], iRandomOp+1, k+1 );
// 					cFILE.getflag( szOPT_SE, 1, 3, sSettingData.pSECT[k].fRATE, true );
// 					cFILE.getflag( szOPT_SE, 2, 3, sSettingData.pSECT[k].fHIGH, true );
// 					cFILE.getflag( szOPT_SE, 3, 3, sSettingData.pSECT[k].fLOW, true );
// 				}
// 
// 				if ( i == 1 )
// 					sGEN.sSETR.push_back( sSettingData );
// 				else
// 					sGEN.sSET.push_back( sSettingData );
// 
// 			} while ( ++iRandomOp );
// 			
// 			if ( sGEN.dwSET_NUM != 0 && sGEN.dwSET_NUM != iRandomOp )
// 			{
// 				char szErrorMsg[128] = "";
// 				StringCchPrintf( szErrorMsg, 128, "file : %s,  keyword : NEW_OPTION, REB_OPTION Not matching",
// 					szName );
// 				MessageBox( NULL, szErrorMsg, "Info", MB_OK );
// 			}
// 			sGEN.dwSET_NUM = iRandomOp;
// 		}
// 		
// 		_listRandomData.push_back(sDATA);		
// 
// 		return sDATA.pSET;
// 	}
// 	#undef USAGE_RANDOM_OPT_SE
// #else
// 	SRANDOM_GEN* GLItemMan::LoadRandomGenData ( const char* szName )
// 	{
// 		if( !szName )
// 			return FALSE;
// 		if( strlen(szName) == 0 )
// 			return FALSE;
// 
// 		VEC_RANDOM::iterator found = std::lower_bound( m_sRandom.begin(), m_sRandom.end(), szName, SRANDOM_DATA_OPER() );
// 		if( found != m_sRandom.end() && !strcmp( (*found).szNAME,szName ) )
// 			return (*found).pSET;
// 
// 		std::string strPath;
// 		strPath = GLOGIC::GetServerPath();
// 		strPath += szName;
// 
// 		SRANDOM_DATA sDATA;
// 		StringCchCopy( sDATA.szNAME, SRANDOM_DATA::NAME_LEN, szName );
// 		sDATA.pSET = new SRANDOM_GEN;
// 		SRANDOM_GEN &sGEN = *sDATA.pSET;
// 
// 		gltexfile cFILE;
// 		cFILE.reg_sep("\t ,");
// 
// 		if (GLOGIC::UseLogicZipFile())
// 			cFILE.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);
// 
// 		if( !cFILE.open( strPath, true ) )
// 		{
// 			sc::writeLogError( "ERROR : GLItemMan::LoadRandomGenData(), %s", szName );
// 			SAFE_DELETE( sDATA.pSET );
// 			return NULL;
// 		}
// 
// 		cFILE.getflag( "fRATE", 1, 1, sGEN.fRATE, true );
// 		cFILE.getflag( "dwSET_NUM", 1, 1, sGEN.dwSET_NUM, true );
// 
// 		TCHAR szTemp[64] = {0};
// 
// 		for( DWORD i=0; i<sGEN.dwSET_NUM && i<SRANDOM_GEN::MAX_SET; ++i )
// 		{
// 			SRANDOM_OPT_SET &sSET = sGEN.sSET[i];
// 
// 			//std::strstream strSTREAM;
// 			//strSTREAM << "OPTION_" << i+1 << std::ends;
// 
// 			_snprintf( szTemp, 64, "OPTION_%u", i+1 );
// 
// 			cFILE.getflag( szTemp, 1, 4, sSET.fRATE, true );
// 			cFILE.getflag( szTemp, 2, 4, sSET.wTYPE, true );
// 			cFILE.getflag( szTemp, 3, 4, sSET.fHIGH, true );
// 			cFILE.getflag( szTemp, 4, 4, sSET.fLOW, true );
// 
// 			//strSTREAM.freeze( false );
// 		}
// 
// 		m_sRandom.push_back( sDATA );
// 		std::sort( m_sRandom.begin(), m_sRandom.end(), SRANDOM_DATA_OPER() );
// 
// 		return sDATA.pSET;
// 	}
// #endif

SGRINDING_OPT* GLItemMan::LoadGrindingOptData ( const std::string& strName, const bool IsRegister )
{
	if ( strName.empty() )
	{
		return NULL;
	}

	if ( IsRegister )
	{
		using namespace boost::lambda;
		VEC_GRINDING::iterator found = std::lower_bound( m_sGrinding.begin(), m_sGrinding.end(), strName, SGRINDING_DATA_OP() );
		if ( found != m_sGrinding.end() && ((*found).strNAME == strName) )
		{
			return (*found).pSET;
		}
	}

	std::string strPath(GLOGIC::GetPath());
	strPath.append( "scripts\\" );
	strPath.append(strName);

	SGRINDING_DATA sDATA;
	sDATA.strNAME = strName;
	sDATA.pSET = new SGRINDING_OPT;
	SGRINDING_OPT &sGEN = *sDATA.pSET;
	sGEN.spSCRIPT = std::tr1::shared_ptr<CGrindingScript>(new CGrindingScript);

	// nut OpenFile
	{
		CGrindingScript* pScript = sGEN.spSCRIPT.get();
		pScript->LoadScript( strPath.c_str() );

		if ( !pScript->IsLoaded() )
		{
			sc::writeLogError( sc::string::format(
				"GLItemMan::LoadGrindingOptData - %1%", pScript->GetError()
				)
			);

			SAFE_DELETE(sDATA.pSET);
			return NULL;
		}

		//! 옵션 테이블을 읽는다.
		if ( pScript->IsExistOption() )
		{
			pScript->GetOptionSlot("Type", sGEN.emTYPE);
			pScript->GetOptionSlot("Level", sGEN.emLEVEL);
			pScript->GetOptionSlot("Purpose", sGEN.emPURPOSE);
			pScript->GetOptionSlot("Attribute", sGEN.dwATTRIBUTE);
			pScript->GetOptionSlot("Consume", sGEN.wCONSUME);
			pScript->GetOptionSlot("Enchant_Max", sGEN.wENCHANT_MAX);
			pScript->GetOptionSlot("Enchant_Min", sGEN.wENCHANT_MIN);
			pScript->GetOptionSlot("Range_Min", sGEN.wRANGE_MIN);
			pScript->GetOptionSlot("Range_Max", sGEN.wRANGE_MAX);
			pScript->GetOptionSlot("Chance", sGEN.mapCHANCE);
		}

		//! 확률 테이블을 세팅한다
		//	1. 확률 총합을 구한다.
		//	2. 빈 확률이 있다면 채워넣는다.
		//	3. 채워넣는 과정에서 100%가 넘는다면 0%, 넘지 않는다면 남는 확률을 분배한다 
		if ( !sGEN.ISCHANCELISTNULL() )
		{
			bool  bRANDOM	 = sGEN.dwATTRIBUTE&EMGRINDING_ATTRIBUTE_RANDOM ? true : false;
			bool  bEMPTYRATE = false;
			float fTOTALRATE = 0.0f;
			INT	  nPREVGRADE = -1;

			//! 랜덤일때만 0포함 총 16개를 사용한다.(0~15)
			WORD wMAXGRADE = bRANDOM ? GRADE_LIMIT_MAX+1 : GRADE_LIMIT_MAX;

			SGRINDING_OPT::MAP_CHANCE_ITER iter = sGEN.mapCHANCE.begin();
			for ( iter; iter != sGEN.mapCHANCE.end();  )
			{
				INT nGRADE = static_cast<INT>(iter->first);

				if ( wMAXGRADE <= static_cast<WORD>(nGRADE) )
				{
					iter = sGEN.mapCHANCE.erase( iter );
					continue;
				}

				if ( 1 < (nGRADE - nPREVGRADE) )
				{
					bEMPTYRATE = true;
				}

				nPREVGRADE = nGRADE;

				fTOTALRATE += iter->second;

				++iter;
			}

			if ( nPREVGRADE < wMAXGRADE-1 )
			{
				bEMPTYRATE = true;
			}

			//! 빈공간을 찾아서 넣어준다
			if ( bEMPTYRATE )
			{
				float fAVRATE = 0.0f;

				if ( fTOTALRATE < 100.0f )
				{
					float fLeakNum = static_cast<float>(wMAXGRADE - static_cast<WORD>(sGEN.mapCHANCE.size()));

					if ( fLeakNum != 0.0f )
					{
						fAVRATE = (100.0f - fTOTALRATE) / fLeakNum;
					}
				}

				nPREVGRADE = -1;

				iter = sGEN.mapCHANCE.begin();
				for ( iter; iter != sGEN.mapCHANCE.end(); ++iter)
				{
					INT nGRADE = static_cast<INT>(iter->first);

					while ( 1 < (nGRADE - nPREVGRADE) )
					{
						std::pair<SGRINDING_OPT::MAP_CHANCE_ITER,bool> retPair = sGEN.mapCHANCE.insert( std::make_pair(nPREVGRADE+1, fAVRATE) );

						if ( retPair.second )
						{
							iter = retPair.first;
							++iter;
						}

						++nPREVGRADE;
					}

					nPREVGRADE = nGRADE;
				}

				while ( nPREVGRADE < wMAXGRADE-1 )
				{
					sGEN.mapCHANCE.insert( std::make_pair(nPREVGRADE+1, fAVRATE) );
					++nPREVGRADE;
				}
			}

			//! 100%로 확률을 강제한다
			using namespace boost::lambda;

			float fRATIO = 100.0f/fTOTALRATE;
			std::for_each( sGEN.mapCHANCE.begin(), sGEN.mapCHANCE.end(), 
				bind(&SGRINDING_OPT::MAP_CHANCE::value_type::second, boost::lambda::_1) *= fRATIO );
		}
	}

	if ( IsRegister )
	{
		using namespace boost::lambda;
		m_sGrinding.push_back( sDATA );
		std::sort( m_sGrinding.begin(), m_sGrinding.end(), SGRINDING_DATA_OP() );
	}

	return sDATA.pSET;
}

void GLItemMan::DeleteGrindingOptData( const std::string& strName )
{
	if ( strName.empty() )
	{
		return;
	}

	VEC_GRINDING::iterator found = std::lower_bound( m_sGrinding.begin(), m_sGrinding.end(), strName, SGRINDING_DATA_OP() );
	if ( found != m_sGrinding.end() )
	{
		SAFE_DELETE( (*found).pSET );
		m_sGrinding.erase(found);
	}
}

SNATIVEID GLItemMan::GetRandomQItemID( SNATIVEID sNativeID, int nMaxNum )
{
    if ( nMaxNum > MAX_NUM )
        nMaxNum = MAX_NUM;

	// 막아놓은 번호와 일치하면 
	for (int idx = 0; idx < nMaxNum; idx++)
	{
		if (sNativeID == m_sQITEMID[idx])
		{
			//srand(unsigned int(time(NULL)));
			WORD idx = (WORD)(rand() % MAX_NUM);
			return m_sQITEMID[idx];
		}
	}
	return NATIVEID_NULL();
}

// GLItemMan::m_vecSrcString 에서 strItemName 과 동일한 이름이 있는 아이템 이름을 vecOut 에 넘겨준다.
void GLItemMan::GetList_SameWord( std::vector<CString>& vecOut, const CString& strItemName )
{
	vecOut.clear();

	for( DWORD i=0; i<m_vecSrcString.size(); ++i )
	{
		const CString& stringA = m_vecSrcString[i];
		if( stringA.Find( strItemName.GetString() ) != -1 )
		{
			vecOut.push_back( stringA );
		}
	}
}
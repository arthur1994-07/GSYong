#include "../pch.h"
#include <algorithm>

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/StlFunctions.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "NpcDialogueSet.h"
#include "NpcDialogue.h"
#include "NpcTalk.h"
#include "NpcDialogueCase.h"
#include "NpcTalkControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CString CNpcDialogueSet::m_strPath;

CNpcDialogueSet::CNpcDialogueSet () :
	m_bModify(false)
{
	memset( m_bBasicTalk, 0, sizeof(bool) * SNpcTalk::EM_BASIC_SIZE );
}

CNpcDialogueSet::~CNpcDialogueSet ()
{
	RemoveAllDialogue ();
}


void CNpcDialogueSet::RegistTalk(std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{
	if (!spNpcTalk)
        return;
	m_vecTALK.push_back(spNpcTalk);	
	DWORD dwID = DWORD(m_vecTALK.size()-1);
	spNpcTalk->SetGlobID(dwID);
}

std::tr1::shared_ptr<SNpcTalk> CNpcDialogueSet::GetTalk(DWORD dwTalkID) const
{
	if (m_vecTALK.size() <= dwTalkID)
    {
        static std::tr1::shared_ptr<SNpcTalk> NullPtr;
        return NullPtr;
    }
    else
    {
	    return m_vecTALK[dwTalkID];
    }
}


BOOL CNpcDialogueSet::SimpleEdit( CWnd* pWnd )
{
//	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "csv file (*.csv)|*.csv|", pWnd );
//
//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
//	if ( dlg.DoModal() != IDOK )
//		return S_FALSE;
//
//	std::fstream SFile;
//	SFile.open ( dlg.GetPathName().GetString(), std::ios_base::out );
//
//	// Csv Head
//	//SITEM::SaveCsvHead( SFile );
//
//
//	//SFile << (WORD) VERSION << ",";
//
//	//SFile << m_bStorage << ",";
//	//SFile << m_bMarket << ",";
//	//SFile << m_bCure << ",";
//	//SFile << m_bStartPoint << ",";
//	//SFile << m_bCharReset << ",";
//	//SFile << m_bBusStation << ",";
//	//SFile << m_bClubNew << ",";
//	//SFile << m_bClubRankUp << ",";
//	//SFile << m_bClubStorage << ",";
//	//SFile << m_bItemRebuild << ",";	// ITEMREBUILD_MARK
//	//SFile << m_bOddEven << ",";
//
//	int nCount = (int)m_mapDialogue.size ();
////	SFile << nCount << ",";
//
//	DIALOGMAP_IT iter = m_mapDialogue.begin();
//	DIALOGMAP_IT iter_end = m_mapDialogue.end();	
//	for ( ; iter!=iter_end; ++iter )
//	{
//		CNpcDialogue* pDialogue = (*iter).second;
//		pDialogue->SaveCSVFile( SFile );
//		SFile << "\n" << ",";
//	}


//	SFile.close();

	return TRUE;
}


BOOL CNpcDialogueSet::Save ( const char* szFileName )
{
	sc::SerialFile SFile;
	if ( !SFile.OpenFile ( FOT_WRITE, szFileName ) )
	{
		GASSERT ( 0 && "오류발생, 파일 저장하기중 열기에 실패" );
		return FALSE;
	}
	CString strBuffer, strAnswerBuffer;
	Save(SFile, strBuffer, strAnswerBuffer);
	return TRUE;
}

BOOL CNpcDialogueSet::Save(sc::SerialFile& SFile, CString& strTalkBuffer, CString& strAnswerBuffer)
{
	CString strName, strFind = "\r\n", strTemp;
	strName.Format( "NN\t%s\r\n", m_TalkName.c_str() );


    // 저장전에 dlg set 의 기능을 리포팅해둠.
    ReportAction();

    SFile << (WORD) VERSION;
    SFile << m_TalkName;

    SFile.WriteBuffer(&m_bBasicTalk, sizeof( bool ) * SNpcTalk::EM_BASIC_SIZE);

    int nCount = (int) m_mapDialogue.size ();
    SFile << nCount;

	strTalkBuffer += strName;      
	strAnswerBuffer += strName;
    for (DIALOGMAP_ITER iter=m_mapDialogue.begin(); iter!=m_mapDialogue.end(); ++iter)
    {
		// Normal Save
        std::tr1::shared_ptr<CNpcDialogue> spDialogue = iter->second;

		// Answer;
		CString strAnserName;
		// strAnserName.Format( "NN\t%s%s%d\r\n", m_TalkName.c_str(), "_", spDialogue->GetDlgNID() );
		// strAnserName.Format( "NN\t%s\r\n", m_TalkName.c_str() );
		// strAnswerBuffer += strAnserName;

		// Normal Save
        spDialogue->SaveFile(SFile, strTalkBuffer, strAnswerBuffer );		

    }

    ResetModified();
    return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0001 (sc::BaseStream& SFile)
{
	int nCount = 0;
    SFile >> nCount;
	for ( int i = 0; i < nCount; i++ )
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0002 (sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}

	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0003 (sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];	

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0004 (sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];	

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0005 (sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];	

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0006 (sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];	

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0007(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];

	int nCount = 0;
    SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0008(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0009(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0010(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RANDOM_PAGE];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0011(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RANDOM_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEMSEARCH_PAGE];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0012(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RANDOM_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEMSEARCH_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ATTENDANCE_BOOK];


	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0013(sc::BaseStream& SFile)	// ITEMREBUILD_MARK
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RANDOM_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEMSEARCH_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ATTENDANCE_BOOK];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_MIX];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0014(sc::BaseStream& SFile)
{
	SFile >> m_bBasicTalk[SNpcTalk::EM_STARTPOINT];
	SFile >> m_bBasicTalk[SNpcTalk::EM_MARKET];
	SFile >> m_bBasicTalk[SNpcTalk::EM_BUSSTATION];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CURE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_STORAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RANDOM_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_RECOVERY_EXP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ATTENDANCE_BOOK];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEMSEARCH_PAGE];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_MIX];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ITEM_REBUILD];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CHAR_RESET];	
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_NEW];
	SFile >> m_bBasicTalk[SNpcTalk::EM_CLUB_UP];
	SFile >> m_bBasicTalk[SNpcTalk::EM_ODDEVEN];

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0015(sc::BaseStream& SFile)
{
	SFile.ReadBuffer(m_bBasicTalk, sizeof( bool ) * 19);

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0016(sc::BaseStream& SFile)
{
    SFile >> m_TalkName;
	sc::string::lowerString(m_TalkName);
    SFile.ReadBuffer(m_bBasicTalk, sizeof( bool ) * 19);

    int nCount = 0;
    SFile >> nCount;
    for (int i=0; i<nCount; ++i)
    {
        std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
        spDialogue->LoadFile(SFile);
        AddDialogue(spDialogue);
    }
    return TRUE;
}

BOOL CNpcDialogueSet::LOAD_0017(sc::BaseStream& SFile)
{
	SFile >> m_TalkName;
	sc::string::lowerString(m_TalkName);
	SFile.ReadBuffer(m_bBasicTalk, sizeof( bool ) * SNpcTalk::EM_BASIC_SIZE );

	int nCount = 0;
	SFile >> nCount;
	for (int i=0; i<nCount; ++i)
	{
		std::tr1::shared_ptr<CNpcDialogue> spDialogue(new CNpcDialogue);
		spDialogue->LoadFile(SFile);
		AddDialogue(spDialogue);
	}
	return TRUE;
}

BOOL CNpcDialogueSet::Load(const std::string& FileName)
{
	if (FileName.empty())
        return FALSE;

	CString strTemp;
	strTemp.Format("%s%s", GetPath(), FileName.c_str());

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(), 
            GLogicData::GetInstance().GetNpcTalkZipFile(), 
            strTemp.GetString(), 
            FileName,
            false,
            GLOGIC::UseLogicPackFile()));

	if (!pBStream.get())
		return FALSE;

	m_TalkName = FileName;
	sc::BaseStream& SFile = *pBStream;

	return Load(SFile);
}

BOOL CNpcDialogueSet::Load(sc::BaseStream& SFile)
{
    ResetModified();
    RemoveAllDialogue();
    
    WORD wVer;
    SFile >> wVer;

    switch (wVer)
    {
    case 0x0001:	LOAD_0001(SFile);	break;
    case 0x0002:	LOAD_0002(SFile);	break;
    case 0x0003:	LOAD_0003(SFile);	break;
    case 0x0004:	LOAD_0004(SFile);	break;
    case 0x0005:	LOAD_0005(SFile);	break;
    case 0x0006:	LOAD_0006(SFile);	break;
    case 0x0007:	LOAD_0007(SFile);	break;	// ITEMREBUILD_MARK
    case 0x0008:	LOAD_0008(SFile);	break;
    case 0x0009:	LOAD_0009(SFile);	break;
    case 0x0010:	LOAD_0010(SFile);	break;
    case 0x0011:	LOAD_0011(SFile);	break;
    case 0x0012:	LOAD_0012(SFile);	break;
    case 0x0013:	LOAD_0013(SFile);	break;
    case 0x0014:	LOAD_0014(SFile);	break;
    case 0x0015:	LOAD_0015(SFile);	break;
	case 0x0016:	LOAD_0016(SFile);	break;
    case VERSION:   LOAD_0017(SFile);   break;
    default:
        {
            std::string ErrMsg(
                sc::string::format(
                "Ntk FileName %1% unknown version %2%",
                m_TalkName, wVer));
            sc::writeLogError(ErrMsg);
            AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONEXCLAMATION);
        }
        return FALSE;
        break;
    };

    //	talk 전역 ID 배정.
    AssignTalkGlobID();
    return TRUE;
}

void CNpcDialogueSet::RemoveAllDialogue()
{
	//std::for_each(m_mapDialogue.begin(), m_mapDialogue.end(), std_afunc::DeleteMapObject());
    m_mapDialogue.clear();
}

DWORD CNpcDialogueSet::MakeDlgNID ()
{
	//	가장 큰 ID보다 1큰 ID를 리턴한다.
	//	일반적으로는 잘 동작하겠지만,
	//	만약 가장 큰 ID를 가진 노드를 삭제할 경우
	//	문제가 발생할 수 있다.	
	//  -> 비어있는 가장 작은 ID 리턴으로 변경
	
	for (DWORD NativeID=1; NativeID<UINT_MAX; ++NativeID)
	{
		DIALOGMAP_ITER found = m_mapDialogue.find(NativeID);
		if (found == m_mapDialogue.end())
			return NativeID;
	}

/*
	DIALOGMAP_IT iter = m_mapDialogue.begin();
	DIALOGMAP_IT iter_end = m_mapDialogue.end();	
	for ( ; iter!=iter_end; ++iter )
	{
		CNpcDialogue* pDialogue = (*iter).second;
		DWORD dlgNativeID = pDialogue->GetDlgNID();
		if ( NativeID < dlgNativeID )
		{
			NativeID = dlgNativeID;
		}
	}

	return (NativeID + 1);
*/

	return 0;
}

void CNpcDialogueSet::DelDialogue(DWORD NativeID)
{
	DIALOGMAP_ITER found = m_mapDialogue.find(NativeID);
	if (found != m_mapDialogue.end())
	{
		//CNpcDialogue* pNpcDialogue = (*found).second;
		//SAFE_DELETE(pNpcDialogue);
		m_mapDialogue.erase(found);
	}
}

std::tr1::shared_ptr<CNpcDialogue> CNpcDialogueSet::GetDialogue2(int nDialogue)
{
	int iCount = 0;
	for (DIALOGMAP_ITER iter=m_mapDialogue.begin(); iter!=m_mapDialogue.end(); ++iter)
	{
		if (iCount == nDialogue)
			return iter->second;
		iCount++;
	}

    static std::tr1::shared_ptr<CNpcDialogue> TempPtr;
	return TempPtr;
}

std::tr1::shared_ptr<CNpcDialogue> CNpcDialogueSet::GetDialogue(DWORD NativeID)
{
	DIALOGMAP_ITER iter = m_mapDialogue.find(NativeID);
	if (iter != m_mapDialogue.end())
    {
		return iter->second;
    }
    else
    {
        static std::tr1::shared_ptr<CNpcDialogue> TempPtr;
        return TempPtr;
    }
}

void CNpcDialogueSet::GetDlgText(DWORD dwIndex, std::string& strName)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(dwIndex);
	if (!spDlg)
	{
		strName = "";
		return;
	}

	DWORD dwCaseID= spDlg->FindNonCondition();
	if ( dwCaseID==UINT_MAX )	dwCaseID = 0;

	std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = spDlg->GetDlgCase(dwCaseID);
	if ( !spDlgCase )
	{
		strName = "";
		return;
	}

	TCHAR szTemp[20] = {0};
	_snprintf_s( szTemp, 20, _TRUNCATE, "[%u]", spDlg->GetDlgNID() );

	strName = szTemp;
	strName += spDlgCase->GetBasicTalk ();

	//std::strstream strStream;
	//strStream << "[" << pDlg->GetDlgNID() << "] " << pDlgCase->GetBasicTalk ();

	//strName = strStream.str();
	//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

CNpcDialogueSet::DIALOGMAP*	CNpcDialogueSet::GetDialogueMap ()
{
	return &m_mapDialogue;
}

void CNpcDialogueSet::ResetAction ()
{
	memset ( m_bBasicTalk , 0, sizeof( bool ) * SNpcTalk::EM_BASIC_SIZE );
}

void CNpcDialogueSet::ReportAction ()
{
	ResetAction();

	for (DIALOGMAP_ITER iter=m_mapDialogue.begin(); iter!=m_mapDialogue.end(); ++iter)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDlg = iter->second;
		const CNpcDialogue::DLGCASE& vecCase = spDlg->GetDlgCaseVec();
		CNpcDialogue::DLGCASE_CITER citer = vecCase.begin();
		CNpcDialogue::DLGCASE_CITER citer_end = vecCase.end();
		for ( ; citer!=citer_end; ++citer )
		{
			std::tr1::shared_ptr<CNpcDialogueCase> spCase = (*citer);
			std::tr1::shared_ptr<CNpcTalkControl> spTalkControl = spCase->GetTalkControl();
			std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spCase->GetCondition ();
			if ( spTalkControl )
			{
				if (spCondition && !spCondition->IsUse())
                    continue;
				const CNpcTalkControl::NPCMAP &mapTalk = *spTalkControl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTalk.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTalk.end();
				for ( ; iter!=iter_end; ++iter )
				{
                    std::tr1::shared_ptr<SNpcTalk> spTalk = iter->second;
                    std::tr1::shared_ptr<SNpcTalkCondition> spCondition2 = spTalk->GetCondition();

					if (spTalk->m_nACTION == SNpcTalk::EM_BASIC)
					{
						if (spCondition2 && !spCondition2->IsUse())
                            continue;

						if (!m_bBasicTalk[spTalk->m_dwACTION_NO])
                            m_bBasicTalk[spTalk->m_dwACTION_NO] = true;
					}
				}
			}
		}
	}
}

void CNpcDialogueSet::AssignTalkGlobID()
{
	m_vecTALK.clear();	
	
	for (DIALOGMAP_ITER iter=m_mapDialogue.begin(); iter!=m_mapDialogue.end(); ++iter)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDlg = iter->second;
		const CNpcDialogue::DLGCASE &vecCase = spDlg->GetDlgCaseVec();
		CNpcDialogue::DLGCASE_CITER citer = vecCase.begin();
		CNpcDialogue::DLGCASE_CITER citer_end = vecCase.end();
		for ( ; citer!=citer_end; ++citer )
		{
			std::tr1::shared_ptr<CNpcDialogueCase> spCase = (*citer);
			std::tr1::shared_ptr<CNpcTalkControl> spTalkControl = spCase->GetTalkControl();
			if (spTalkControl)
			{
				const CNpcTalkControl::NPCMAP &mapTalk = *spTalkControl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTalk.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTalk.end();
				for ( ; iter!=iter_end; ++iter )
				{
                    std::tr1::shared_ptr<SNpcTalk> spTalk = iter->second;
					RegistTalk(spTalk);
				}
			}
		}
	}
}

CNpcDialogueSet& CNpcDialogueSet::operator= (CNpcDialogueSet& rvalue)
{
	RemoveAllDialogue();
		
	for (DIALOGMAP_ITER iter=rvalue.m_mapDialogue.begin(); iter!=rvalue.m_mapDialogue.end(); ++iter)
	{
        std::tr1::shared_ptr<CNpcDialogue> spDlg = iter->second;
		//CNpcDialogue* pNewDlg = new CNpcDialogue;
        //*pNewDlg = *spDlg;
		AddDialogue(spDlg);
	}

	memcpy( m_bBasicTalk, rvalue.m_bBasicTalk, sizeof( bool ) * SNpcTalk::EM_BASIC_SIZE );

	//	talk 전역 ID 배정.
	AssignTalkGlobID ();

	return *this;
}

bool CNpcDialogueSet::IsBasicTalk(int nAction) const
{
    if (nAction < 0 || nAction >= SNpcTalk::EM_BASIC_SIZE)
        return false;
    else
        return m_bBasicTalk[nAction];
}


BOOL CNpcDialogueSet::AddDialogue(std::tr1::shared_ptr<CNpcDialogue> spDialogue)
{
    DIALOGMAP_ITER found = m_mapDialogue.find(spDialogue->GetDlgNID());
    if (found == m_mapDialogue.end())
    {
        m_mapDialogue.insert(DIALOGMAP_VALUE(spDialogue->GetDlgNID(), spDialogue));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool CNpcDialogueSet::AddDialogueCase(DWORD DlgId, std::tr1::shared_ptr<CNpcDialogueCase> spCase)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        spDlg->AddCase(spCase);
        return true;
    }
    else
    {
        return false;
    }
}

std::tr1::shared_ptr<CNpcDialogueCase> CNpcDialogueSet::GetCase(DWORD DlgId, int CaseNum)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        return spDlg->GetDlgCase(CaseNum);
    }
    else
    {
        static std::tr1::shared_ptr<CNpcDialogueCase> NullPtr;
        return NullPtr;
    }
}

bool CNpcDialogueSet::AddDialogueCaseCondition(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<SNpcTalkCondition> spCondition)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<CNpcDialogueCase> spCase = GetCase(DlgId, CaseNum);
    if (spCase)
    {
        spCase->SetCondition(spCondition);
        bReturn = true;
    }
    return bReturn;
}

std::tr1::shared_ptr<SNpcTalkCondition> CNpcDialogueSet::GetCondition(DWORD DlgId, int CaseNum)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        std::tr1::shared_ptr<CNpcDialogueCase> spCase = spDlg->GetDlgCase(CaseNum);
        if (spCase)
            return spCase->GetCondition();
    }
    static std::tr1::shared_ptr<SNpcTalkCondition> NullPtr;
    return NullPtr;
}

bool CNpcDialogueSet::AddDialogueCaseConditionCompleteQuest(DWORD DlgId, int CaseNum, SNATIVEID QuestId)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);
    if (spCondition)
    {
        spCondition->AddCompleteQuest(QuestId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseConditionHaveItem(DWORD DlgId, int CaseNum, SNATIVEID ItemId, int ItemNum)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);
    if (spCondition)
    {
        spCondition->AddHaveItem(ItemId, ItemNum);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseConditionLearnSkill(DWORD DlgId, int CaseNum, SNATIVEID SkillId, int SkillLevel)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);
    if (spCondition)
    {
        spCondition->AddLearnSkill(SkillId, SkillLevel);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseConditionDisQuest(DWORD DlgId, int CaseNum, SNATIVEID QuestId)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);
    if (spCondition)
    {
        spCondition->AddDisQuest(QuestId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseConditionDisSkill(DWORD DlgId, int CaseNum, SNATIVEID SkillId)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);    
    if (spCondition)
    {
        spCondition->AddDisSkill(SkillId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseConditionSkillFact(DWORD DlgId, int CaseNum, SNATIVEID SkillId, int SkillLevel)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum);
    if (spCondition)
    {
        spCondition->AddSkillFact(SkillId, SkillLevel);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControl(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<CNpcTalkControl> spControl)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<CNpcDialogueCase> spCase = GetCase(DlgId, CaseNum);
    if (spCase)
    {
        spCase->SetTalkControl(spControl);
        bReturn = true;
    }
    return bReturn;
}

std::tr1::shared_ptr<CNpcTalkControl> CNpcDialogueSet::GetControl(DWORD DlgId, int CaseNum)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        std::tr1::shared_ptr<CNpcDialogueCase> spCase = spDlg->GetDlgCase(CaseNum);
        if (spCase)
            return spCase->GetTalkControl();
    }
    static std::tr1::shared_ptr<CNpcTalkControl> NullPtr;
    return NullPtr;
}

bool CNpcDialogueSet::AddDialogueCaseControlTalk(DWORD DlgId, int CaseNum, std::tr1::shared_ptr<SNpcTalk> spTalk)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<CNpcTalkControl> spControl = GetControl(DlgId, CaseNum);
    if (spControl)
    {
        spControl->AddTalk(spTalk);
        bReturn = true;
    }
    return bReturn;
}

std::tr1::shared_ptr<SNpcTalk> CNpcDialogueSet::GetTalk(DWORD DlgId, int CaseNum, int TalkId)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        std::tr1::shared_ptr<CNpcDialogueCase> spCase = spDlg->GetDlgCase(CaseNum);
        if (spCase)
        {
            std::tr1::shared_ptr<CNpcTalkControl> spControl = spCase->GetTalkControl();
            if (spControl)
            {
                return spControl->GetTalk(TalkId);
            }
        }
    }
    static std::tr1::shared_ptr<SNpcTalk> NullPtr;
    return NullPtr;
}

bool CNpcDialogueSet::AddDialogueCaseControlTalkNeedItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<SNpcTalk> spTalk = GetTalk(DlgId, CaseNum, TalkId);
    if (spTalk)
    {
        spTalk->AddNeedItem(ItemId, ItemNum);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlTalkTradeItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<SNpcTalk> spTalk = GetTalk(DlgId, CaseNum, TalkId);
    if (spTalk)
    {
        spTalk->AddTradeItem(ItemId, ItemNum);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlCondition(DWORD DlgId, int CaseNum, int TalkId, std::tr1::shared_ptr<SNpcTalkCondition> spCondition)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<SNpcTalk> spTalk = GetTalk(DlgId, CaseNum, TalkId);
    if (spTalk)
    {
        spTalk->SetCondition(spCondition);
        bReturn = true;
    }
    return bReturn;
}

std::tr1::shared_ptr<SNpcTalkCondition> CNpcDialogueSet::GetCondition(DWORD DlgId, int CaseNum, int TalkId)
{
    std::tr1::shared_ptr<CNpcDialogue> spDlg = GetDialogue(DlgId);
    if (spDlg)
    {
        std::tr1::shared_ptr<CNpcDialogueCase> spCase = spDlg->GetDlgCase(CaseNum);
        if (spCase)
        {
            std::tr1::shared_ptr<CNpcTalkControl> spControl = spCase->GetTalkControl();
            if (spControl)
            {
                std::tr1::shared_ptr<SNpcTalk> spTalk = spControl->GetTalk(TalkId);
                if (spTalk)
                    return spTalk->GetCondition();
            }
        }
    }
    static std::tr1::shared_ptr<SNpcTalkCondition> NullPtr;
    return NullPtr;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionCompleteQuest(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID QuestId)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddCompleteQuest(QuestId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionHaveItem(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID ItemId, int ItemNum)
{
    bool bReturn = false;    
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddHaveItem(ItemId, ItemNum);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionLearnSkill(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId, int SkillLevel)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddLearnSkill(SkillId, SkillLevel);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionDisQuest(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID QuestId)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddDisQuest(QuestId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionDisSkill(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddDisSkill(SkillId);
        bReturn = true;
    }
    return bReturn;
}

bool CNpcDialogueSet::AddDialogueCaseControlConditionSkillFact(DWORD DlgId, int CaseNum, int TalkId, SNATIVEID SkillId, int SkillLevel)
{
    bool bReturn = false;
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition = GetCondition(DlgId, CaseNum, TalkId);
    if (spCondition)
    {
        spCondition->AddSkillFact(SkillId, SkillLevel);
        bReturn = true;
    }
    return bReturn;
}

std::string CNpcDialogueSet::ExportText( const std::string& strFileName )
{
    FILE *pFile = _fsopen( strFileName.c_str(), "wt", _SH_DENYNO );
	if( pFile == NULL )
		return "파일 오픈 에러. Text파일이 열려있으면 닫아주세요";

    char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

	for( DIALOGMAP_ITER it = m_mapDialogue.begin(); it != m_mapDialogue.end(); ++it )
	{
        std::tr1::shared_ptr<CNpcDialogue> spNpcDialogue = it->second;

		fprintf( pFile, "<START>\n" );
		sprintf( szTempChar, "<ID> %d\n\n", spNpcDialogue->GetDlgNID() );
		fprintf( pFile, szTempChar );

		// 텍스트 리스트
		for( UINT i = 0; i < spNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			sprintf( szTempChar, "<TEXT> %d\n", i + 1 );
			fprintf( pFile, szTempChar );
			sprintf( szTempChar, "%s\r\n", spNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );

			fprintf( pFile, szTempChar );
			fprintf( pFile, "</TEXT>\n\n" );

			// 대답 리스트
			if( spNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL )
                continue;
			if( spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 )
                continue;

			fprintf( pFile, "<ANSWER>\n" );
			CNpcTalkControl::NPCMAP_IT iter     = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = spNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iter->second;
				sprintf( szTempChar, "%s\n", spNpcTalk->GetTalk() );
				fprintf( pFile, szTempChar );
			}
			fprintf( pFile, "</ANSWER>\n\n" );
		}

		fprintf( pFile, "<END>\n\n\n" );
	}

	fclose( pFile );

    return "";
}

std::string CNpcDialogueSet::ImportText( const std::string& strFileName )
{
    FILE *pFile = _fsopen( strFileName.c_str(), "rt", _SH_DENYNO );
	if( pFile == NULL )
		return "익스포트된 텍스트 데이터가 없습니다.";

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	bool bAddText = FALSE;
	bool bAddAnswer = FALSE;

    // 데이터 로드용 구조체
    struct SAnswerTextData 
    {
	    std::string	answerTalkText;
	    int			dwNpcNumber;
	    int			dwTextNumber;
	    int			dwAnswerNumber;
    };

    struct SNpcTextData 
    {
	    std::string TalkText;
	    int			dwNpcNumber;
	    int			dwTextNumber;
    };

	std::vector<SNpcTextData> npcTextData;
	std::vector<SAnswerTextData> npcAnswerTextData;

    char szSectionChar[1024];
	ZeroMemory( szSectionChar, 1024 );

    char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

    char szTempAddChar[1024];
	ZeroMemory( szTempAddChar, 1024 );
	int iLineNumber = 0;

	bool bEndText = TRUE;
	while( !feof(pFile) )
	{
		ZeroMemory( szSectionChar, 512 );
		ZeroMemory( szTempChar, 512 );

		fgets( szTempChar, 1024, pFile );
		sscanf( szTempChar, "%s", szSectionChar );

		iLineNumber++;

		if (_stricmp(szSectionChar, "//") == 0)
            continue;

		if (bAddAnswer == FALSE && _stricmp(szSectionChar, "</ANSWER>") == 0)
		{
			sprintf( szTempChar, "<ANSWER>가 없습니다.\n %d번째 줄", iLineNumber );
			fclose( pFile );
			return szTempChar;
		}

		if (bAddText == FALSE && _stricmp(szSectionChar, "</TEXT>") == 0)
		{
			sprintf( szTempChar, "<TEXT>가 없습니다.\n %d번째 줄", iLineNumber );
			fclose( pFile );
			return szTempChar;
		}

		if( bAddText )
		{
			if (_stricmp(szSectionChar, "<ANSWER>") == 0)
			{
				sprintf( szTempChar, "</TEXT>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}

			if (_stricmp(szSectionChar, "</TEXT>") == 0)
			{
				bAddText = FALSE;
				char szTemp[1024];
				ZeroMemory( szTemp, 1024 );
				strncpy( szTemp, szTempAddChar, strlen(szTempAddChar) );

				SNpcTextData inputData;
				inputData.dwNpcNumber  = iNpcNumber;
				inputData.TalkText     = szTemp;
				inputData.dwTextNumber = iTextNumber;
				npcTextData.push_back( inputData );
				continue;
			}
			
			if( strlen( szSectionChar ) == 0 )
			{
				strcat( szTempAddChar, "\r\n" );
			}else{		
				if( strlen( szTempAddChar ) != 0 )
				{
					strcat( szTempAddChar, "\r\n" );
				}
				if( strstr( szTempChar, "\r\n" ) != NULL )
				{
					strncat( szTempAddChar, szTempChar, strlen(szTempChar) - 2 );
				}else{
					strncat( szTempAddChar, szTempChar, strlen(szTempChar) - 1 );
				}
				
			}
			continue;
		}

		if( bAddAnswer )
		{
			if (_stricmp(szSectionChar, "<TEXT>") == 0)
			{
				sprintf( szTempChar, "</ANSWER>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}

			if (_stricmp(szSectionChar, "</ANSWER>") == 0)
			{
				bAddAnswer = FALSE;
				if( iAnswerNumber != GetDialogue2(iNpcNumber)->GetDlgCase(iTextNumber)->GetTalkControl()->GetTalkNum() )
				{
					sprintf( szTempChar, "데이터에 있는 Answer 갯수보다 Import된 Answer 갯수가 다릅니다.\n %d번째 줄", iLineNumber );
					fclose( pFile );
					return szTempChar;
				}

				iAnswerNumber = 0;
				continue;
			}

            ZeroMemory( szTempAddChar, 1024 );
			strncpy( szTempAddChar, szTempChar, strlen(szTempChar)-1 );
			SAnswerTextData inputAnswerData;
			inputAnswerData.dwNpcNumber = iNpcNumber;
			inputAnswerData.dwTextNumber = iTextNumber; 
			inputAnswerData.dwAnswerNumber = iAnswerNumber;
			inputAnswerData.answerTalkText	= szTempAddChar;
			npcAnswerTextData.push_back( inputAnswerData );

			iAnswerNumber++;
		}

		// 전체 NPC 대화 갯수 증가
		if (_stricmp(szSectionChar, "<START>") == 0)
		{
			if( bEndText )
			{
				iNpcNumber++;
				iTextNumber = -1;
				bEndText = FALSE;
				if( iNpcNumber >= static_cast<int>( GetDialogueMap()->size() ) )
				{
					sprintf( szTempChar, "데이터에 있는 NPC 대화 갯수보다 Import된 NPC 대화 갯수가 더 많습니다.\n %d번째 줄", iLineNumber );
					fclose( pFile );
					return szTempChar;
				}
			}
            else
            {
				sprintf( szTempChar, "<END>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
		}
		// 텍스트 리스트 추가
		if (_stricmp(szSectionChar, "<TEXT>") == 0 && bAddAnswer == FALSE)
		{
			if( bEndText )
			{
				sprintf( szTempChar, "<START>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
			bAddText = TRUE;
			iTextNumber++;

			sscanf( szTempChar, "%s %s", szSectionChar, szTempAddChar );
			if( iTextNumber + 1 != atoi( szTempAddChar ) )
			{
				sprintf( szTempChar, "TEXT ID가 잘못 되었습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}

			if( iTextNumber >= static_cast<int>( GetDialogue2(iNpcNumber)->GetDlgCaseVec().size() ) )
			{
				sprintf( szTempChar, "데이터에 있는 Text 갯수보다 Import된 Text 갯수가 더 많습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
			ZeroMemory( szTempAddChar, 1024 );
		}
		// 대답 리스트 추가
		if (_stricmp(szSectionChar, "<ANSWER>") == 0 &&  bAddText == FALSE)
		{
			if( GetDialogue2(iNpcNumber)->GetDlgCase(iTextNumber)->GetTalkControl() == NULL )
			{
				sprintf( szTempChar, "Answer를 추가할 수 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
			if( bEndText )
			{
				sprintf( szTempChar, "<START>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
			ZeroMemory( szTempAddChar, 1024 );
			bAddAnswer = TRUE;
			iAnswerNumber = 0;
		}
		// END 체크
		if (_stricmp(szSectionChar, "<END>") == 0)
		{
			if( GetDialogue2(iNpcNumber)->GetDlgCaseVec().size() != iTextNumber + 1 )
			{
				sprintf( szTempChar, "데이터에 있는 대화 갯수가 Import된 대화 갯수와 다릅니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}

			if( bEndText == FALSE )
			{
				bEndText = TRUE;
			}
            else
            {
				sprintf( szTempChar, "<START>가 없습니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
		}
		// ID 뒤의 갯수와 npc 갯수 체크
		if( strcmp( szSectionChar, "<ID>" ) == 0 )
		{
			sscanf( szTempChar, "%s %s", szSectionChar, szTempAddChar );
			if( GetDialogue2(iNpcNumber)->GetDlgNID() != atoi(szTempAddChar) )
			{
				sprintf( szTempChar, "Import된 ID와 데이터의 ID가 다릅니다.\n %d번째 줄", iLineNumber );
				fclose( pFile );
				return szTempChar;
			}
		}
	}
	fclose( pFile );

	if( iNpcNumber + 1  != GetDialogueMap()->size() )
	{
		sprintf( szTempChar, "데이터에 있는 NPC 대화 갯수가 Import된 NPC 대화 갯수와 다릅니다.\n %d번째 줄", iLineNumber );
		return szTempChar;
	}

	int i;
	for( i = 0; i < static_cast<int>( npcTextData.size() ); i++ )
	{
		GetDialogue2(npcTextData[i].dwNpcNumber)->GetDlgCase(npcTextData[i].dwTextNumber)->SetBasicTalk(npcTextData[i].TalkText.c_str());
	}

	for( i = 0; i < static_cast<int>( npcAnswerTextData.size() ); i++ )
	{

		int test1 = npcAnswerTextData[i].dwNpcNumber;
		int test2 = npcAnswerTextData[i].dwTextNumber;
		int test3 = npcAnswerTextData[i].dwAnswerNumber;
		std::string test4 = npcAnswerTextData[i].answerTalkText.c_str();

		GetDialogue2(npcAnswerTextData[i].dwNpcNumber)->GetDlgCase(npcAnswerTextData[i].dwTextNumber)->
		  			  GetTalkControl()->GetTalk2(npcAnswerTextData[i].dwAnswerNumber)->SetTalk(npcAnswerTextData[i].answerTalkText.c_str());
	}	

    return "";
}
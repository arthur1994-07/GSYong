#include "../pch.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../Quest/GLQUEST.h"
#include "../Quest/GLQUESTMAN.h"
#include "../GLogicEx.h"
#include "../Item/GLItemMan.h"
#include "../GLogicDataMan.h"
#include "NpcTalk.h"
#include "NpcTalkCondition.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::string	SNpcTalk::strACTIONTYPE[EM_TALK_SIZE];
//{
//    "선택되지 않음",
//	"페이지 이동",
//	"기본 기능",
//	"퀘스트 시작",
//	"퀘스트 진행",
//};
//
std::string	SNpcTalk::szBASICDESC[EM_BASIC_SIZE];
//{
//    "창고 열기",
//	"상점 열기",
//	"치료",
//	"시작 위치 지정",
//	"stats, skill 초기화",
//	"아이템 교환",
//	"버스 정류장",
//
//	"클럽 생성",
//	"클럽 랭크 업",
//	
//	"시디 인증하기",
//	"상업 수수료",
//	"클럽 창고",
//
//	"아이템 개조"	// ITEMREBUILD_MARK
//};

SNpcTalk::SNpcTalk () :
	m_dwNID(UINT_MAX),
	m_dwGLOB_ID(0),
	m_nACTION(EM_DO_NOTHING),
	m_dwACTION_NO(UINT_MAX),
	
	m_dwACTION_PARAM1(UINT_MAX),
	m_dwACTION_PARAM2(UINT_MAX),
	m_dwRandomTime ( 0 )
{
	int i;

	m_vecNeedItem.clear();
	m_vecTradeItem.clear();

	for ( i = 0; i < MAX_QUEST_START; ++i )
	{
		m_dwQuestStartID[i] = UINT_MAX;
	}

	for ( i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		m_dwRandomPageID[i] = UINT_MAX;
	}
}

SNpcTalk::~SNpcTalk ()
{
	ResetCondition ();
}

void SNpcTalk::SetGlobID ( DWORD dwID )
{
	m_dwGLOB_ID = dwID;
}
/*
BOOL SNpcTalk::SAVECSV ( std::fstream &SFile )
{	

//	SFile << (WORD)VERSION <<","; 

//	SFile << m_dwNID <<",";
	// 텍스트 출력
//	SFile << m_strTalk <<",";

//	SFile << m_nACTION <<",";
//	SFile << m_dwACTION_NO <<",";
	
//	SFile << m_dwACTION_PARAM1 <<",";
//	SFile << m_dwACTION_PARAM2 <<",";

//	SFile << m_strBusFile <<",";

//	SFile << BOOL(m_pCondition!=NULL);
//	if ( m_pCondition )		m_pCondition->SAVECSVFILE ( SFile );

	return TRUE;
}
*/

// Need Add New Item Con.
BOOL SNpcTalk::SAVE ( sc::SerialFile& SFile )
{	
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	SFile << (WORD)VERSION;

	SFile << m_dwNID;
	SFile << m_strTalk;

	SFile << m_nACTION;
	SFile << m_dwACTION_NO;

	SFile << m_dwACTION_PARAM1;
	SFile << m_dwACTION_PARAM2;

	DWORD dwSize = (DWORD)m_vecNeedItem.size();

	SFile << dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum = m_vecNeedItem[i];
		SFile.WriteBuffer( &sItemNum, sizeof ( SITEM_NUM ) );
	}

	dwSize = (DWORD)m_vecTradeItem.size();

	SFile << dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum = m_vecTradeItem[i];
		SFile.WriteBuffer( &sItemNum, sizeof ( SITEM_NUM ) );
	}
	
	for ( int i = 0; i < MAX_QUEST_START; ++i ) 
	{
		SFile << m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i ) 
	{
		SFile << m_dwRandomPageID[i];
	}

	SFile << m_dwRandomTime;

	SFile << m_strBusFile;

	SFile << m_sHireSummon.dwID;

	SFile << BOOL(m_spCondition.get() != NULL);
	if (m_spCondition)
        m_spCondition->SAVE ( SFile );

	return TRUE;
}


BOOL SNpcTalk::LOAD_0001 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	return TRUE;
}

BOOL SNpcTalk::LOAD_0002 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	m_dwACTION_PARAM1 = UINT_MAX;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
	{
		SITEM_NUM sItemNum;
		sItemNum.sNativeID.dwID = m_dwACTION_PARAM1;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{
			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

			if ( pITEM ) 
			{
				sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
			}
			else
			{
				sItemNum.dwNum = 1;
			}

			m_vecNeedItem.push_back( sItemNum );
		}
	}

		

	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;


	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0003 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
	{
		SITEM_NUM sItemNum;
		sItemNum.sNativeID.dwID = m_dwACTION_PARAM1;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{
			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

			if ( pITEM ) 
			{
				sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
			}
			else
			{
				sItemNum.dwNum = 1;
			}

			m_vecNeedItem.push_back( sItemNum );
		}

		sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{		
			sItemNum.dwNum = 1;

			m_vecTradeItem.push_back( sItemNum );
		}
	}

	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0004 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
	{
		SITEM_NUM sItemNum;
		sItemNum.sNativeID.dwID = m_dwACTION_PARAM1;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{
			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

			if ( pITEM ) 
			{
				sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
			}
			else
			{
				sItemNum.dwNum = 1;
			}

			m_vecNeedItem.push_back( sItemNum );
		}
	

		sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{		
			sItemNum.dwNum = 1;

			m_vecTradeItem.push_back( sItemNum );
		}
	}


	if ( m_nACTION == EM_QUEST_START )
		m_dwQuestStartID[0] = m_dwACTION_PARAM1;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0005 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	if( m_nACTION == EM_BASIC && m_dwACTION_NO == EM_ITEM_TRADE )
	{
		SITEM_NUM sItemNum;
		for( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
		{
			DWORD dwNeedItem;
			SFile >> dwNeedItem;
			
			sItemNum.sNativeID.dwID = dwNeedItem;

			if ( sItemNum.sNativeID == NATIVEID_NULL() )	continue;
			
			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

			if ( pITEM ) 
			{
				sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
			}
			else
			{
				sItemNum.dwNum = 1;
			}

			m_vecNeedItem.push_back( sItemNum );
		}


		SFile >> m_dwACTION_PARAM2;

		sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

		if ( sItemNum.sNativeID != NATIVEID_NULL() )
		{		
			sItemNum.dwNum = 1;

			m_vecTradeItem.push_back( sItemNum );
		}
	}
	else
	{
		SFile >> m_dwACTION_PARAM1;
		SFile >> m_dwACTION_PARAM2;

		if ( m_nACTION == EM_QUEST_START )
		{
			m_dwQuestStartID[0] = m_dwACTION_PARAM1;
		}
	}
	

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0006 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;


	SITEM_NUM sItemNum;

	for( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		DWORD dwNeedItem;
		SFile >> dwNeedItem;
		
		sItemNum.sNativeID.dwID = dwNeedItem;

		if ( sItemNum.sNativeID == NATIVEID_NULL() )	continue;
		
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

		if ( pITEM ) 
		{
			sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		}
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecNeedItem.push_back( sItemNum );
	}

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}
	
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

	if ( sItemNum.sNativeID != NATIVEID_NULL() )
	{		
		sItemNum.dwNum = 1;

		m_vecTradeItem.push_back( sItemNum );
	}


	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0007 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	SITEM_NUM sItemNum;

	for( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		DWORD dwNeedItem;
		SFile >> dwNeedItem;
		
		sItemNum.sNativeID.dwID = dwNeedItem;

		if ( sItemNum.sNativeID == NATIVEID_NULL() )	continue;
		
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

		if ( pITEM ) 
		{
			sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		}
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecNeedItem.push_back( sItemNum );
	}

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		SFile >> m_dwRandomPageID[i];
	}

	SFile >> m_dwRandomTime;
	
	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

	if ( sItemNum.sNativeID != NATIVEID_NULL() )
	{		
		sItemNum.dwNum = 1;

		m_vecTradeItem.push_back( sItemNum );
	}

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0008 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	SITEM_NUM sItemNum;

	for( int i = 0; i < MAX_NEEDITEM_COUNT; ++i )
	{
		DWORD dwNeedItem;
		SFile >> dwNeedItem;
		
		sItemNum.sNativeID.dwID = dwNeedItem;

		if ( sItemNum.sNativeID == NATIVEID_NULL() )	continue;
		
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID );

		if ( pITEM ) 
		{
			sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		}
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecNeedItem.push_back( sItemNum );
	}

	sItemNum.sNativeID.dwID = m_dwACTION_PARAM2;

	if ( sItemNum.sNativeID != NATIVEID_NULL() )
	{		
		sItemNum.dwNum = 1;

		m_vecTradeItem.push_back( sItemNum );
	}

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		SFile >> m_dwRandomPageID[i];
	}

	SFile >> m_dwRandomTime;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_0009 ( sc::BaseStream& SFile )
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	DWORD dwSize = 0;

	SFile >> dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum;

		SFile.ReadBuffer( &sItemNum, sizeof ( SITEM_NUM ) );

		m_vecNeedItem.push_back( sItemNum );
	}

	SFile >> dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum;

		SFile.ReadBuffer( &sItemNum, sizeof ( SITEM_NUM ) );

		m_vecTradeItem.push_back( sItemNum );
	}	


	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		SFile >> m_dwRandomPageID[i];
	}

	SFile >> m_dwRandomTime;

	SFile >> m_strBusFile;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD_000A(sc::BaseStream& SFile)
{
	SFile >> m_dwNID;
	SFile >> m_strTalk;

	SFile >> m_nACTION;
	SFile >> m_dwACTION_NO;

	SFile >> m_dwACTION_PARAM1;
	SFile >> m_dwACTION_PARAM2;

	DWORD dwSize = 0;

	SFile >> dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum;

		SFile.ReadBuffer( &sItemNum, sizeof ( SITEM_NUM ) );

		m_vecNeedItem.push_back( sItemNum );
	}

	SFile >> dwSize;

	for ( DWORD i = 0; i < dwSize; ++i ) 
	{
		SITEM_NUM sItemNum;

		SFile.ReadBuffer( &sItemNum, sizeof ( SITEM_NUM ) );

		m_vecTradeItem.push_back( sItemNum );
	}	


	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		SFile >> m_dwQuestStartID[i];
	}

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
	{
		SFile >> m_dwRandomPageID[i];
	}

	SFile >> m_dwRandomTime;

	SFile >> m_strBusFile;

	SFile >> m_sHireSummon.dwID;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
		m_spCondition = spCondition;
		m_spCondition->LOAD ( SFile );
	}

	return TRUE;
}

BOOL SNpcTalk::LOAD ( sc::BaseStream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer = 0;
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001 ( SFile );	break;
	case 0x0002:	LOAD_0002 ( SFile );	break;
	case 0x0003:	LOAD_0003 ( SFile );	break;
	case 0x0004:	LOAD_0004 ( SFile );	break;
	case 0x0005:	LOAD_0005 ( SFile );	break;
	case 0x0006:	LOAD_0006 ( SFile );	break;
	case 0x0007:	LOAD_0007 ( SFile );	break;
	case 0x0008:	LOAD_0008 ( SFile );	break;
	case 0x0009:	LOAD_0009 ( SFile );	break;
	case VERSION:	LOAD_000A ( SFile );	break;

	default:
		MessageBox ( NULL, "SNpcTalk::LOAD() unknown data version.", "ERROR", MB_OK );
		break;
	};

	return TRUE;
}

SNpcTalk&	SNpcTalk::operator= ( const SNpcTalk& rNpcTalk )
{
	m_dwNID = rNpcTalk.m_dwNID;
	m_strTalk = rNpcTalk.m_strTalk;

    m_nACTION = rNpcTalk.m_nACTION;
	m_dwACTION_NO = rNpcTalk.m_dwACTION_NO;


	m_vecNeedItem = rNpcTalk.m_vecNeedItem;
	m_vecTradeItem = rNpcTalk.m_vecTradeItem;

	for ( int i = 0 ; i < MAX_QUEST_START; ++i )
		m_dwQuestStartID[i]  = rNpcTalk.m_dwQuestStartID[i];

	for ( int i = 0 ; i < MAX_RANDOM_PAGE; ++i )
		m_dwRandomPageID[i]  = rNpcTalk.m_dwRandomPageID[i];

	m_dwRandomTime = rNpcTalk.m_dwRandomTime;

	m_dwACTION_PARAM1 = rNpcTalk.m_dwACTION_PARAM1;
	m_dwACTION_PARAM2 = rNpcTalk.m_dwACTION_PARAM2;

	m_strBusFile = rNpcTalk.m_strBusFile;
	m_sHireSummon = rNpcTalk.m_sHireSummon;

	ResetCondition ();
	if ( rNpcTalk.m_spCondition )
	{
		SetCondition ( *rNpcTalk.m_spCondition );
	}

	return *this;
}

BOOL SNpcTalk::SetCondition ( const SNpcTalkCondition &sCondition )
{
	ResetCondition();

    std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
	m_spCondition = spCondition;
	*m_spCondition.get() = sCondition;

	return TRUE;
}

BOOL SNpcTalk::SetCondition(std::tr1::shared_ptr<SNpcTalkCondition> spCondition)
{
    ResetCondition();    
    m_spCondition = spCondition;
    return TRUE;
}

void SNpcTalk::ResetCondition ()
{
	//SAFE_DELETE ( m_spCondition );
    m_spCondition.reset();
}

void SNpcTalk::SetTalk( const std::string &strTalk )
{
	m_strTalk = strTalk;
}

std::tr1::shared_ptr<SNpcTalkCondition> SNpcTalk::GetCondition() const
{	
	return m_spCondition;
}

void SNpcTalk::Init ()
{
	m_dwNID = UINT_MAX;
	m_strTalk = "";

    m_nACTION = EM_DO_NOTHING;
	m_dwACTION_NO = UINT_MAX;
	m_dwACTION_PARAM1 = UINT_MAX;
	m_dwACTION_PARAM2 = UINT_MAX;

	m_strBusFile = "";
	m_sHireSummon = SNATIVEID( false );

	m_vecNeedItem.clear();
	m_vecTradeItem.clear();

	for ( int i = 0; i < MAX_QUEST_START; ++i )
		m_dwQuestStartID[i] = UINT_MAX;

	for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
		m_dwRandomPageID[i] = UINT_MAX;

	m_dwRandomTime = 0;

}

BOOL SNpcTalk::DoTEST(GLCHARLOGIC *pCHARLOGIC, DWORD PartyMemberNum, int iSvrHour, DWORD dwIndex)
{
	if (!pCHARLOGIC)
        return FALSE;

	if (m_spCondition)
	{
		BOOL bCONDITION = m_spCondition->DoTEST(pCHARLOGIC, PartyMemberNum, iSvrHour);
		if (!bCONDITION)
            return FALSE;
	}

	BOOL bOK = FALSE;

	switch ( m_nACTION )
	{
	case EM_QUEST_START:
		{
			if ( dwIndex == UINT_MAX ) 
			{
				for ( int i = 0; i < MAX_QUEST_START; ++i )
				{
					if ( m_dwQuestStartID[i] == UINT_MAX ) continue;

					//	이미 진행중인 퀘스트는 새로이 시작할 수 없다.
					GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwQuestStartID[i]);
					if (pQUEST_PROG)
                        continue;

					GLQUESTPROG *pQUEST_END = pCHARLOGIC->m_cQuestPlay.FindEnd(m_dwQuestStartID[i]);
					if ( pQUEST_END )
					{
						GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( m_dwQuestStartID[i] );
						if (!pQUEST)
                            continue;

						if ( pQUEST_END->m_bCOMPLETE )
						{
							//	이미 완료한 퀘스트의 경우에는 반복 수행이 가능할때만 가능.
							if ( !pQUEST->IsREPEAT() )
                                continue;

							if ( pQUEST->IsDaily() )
							{
								CTime cTime = CTime::GetCurrentTime();
								CTime cTimeEnd(pQUEST_END->m_tEndTime);

								if (cTime.GetDay() == cTimeEnd.GetDay() &&
									cTime.GetMonth() == cTimeEnd.GetMonth() &&
									cTime.GetYear() == cTimeEnd.GetYear() )
									continue;
							}
						}
						else
						{
							//	이미 포기한 퀘스트의 경우에는제시도 수행이 가능할때만 가능.
							if ( !pQUEST->IsAGAIN() )
                                continue;
						}
					}

					bOK = TRUE;
				}
			}
			else
			{
				if ( m_dwQuestStartID[dwIndex] == UINT_MAX )
                    return FALSE;

				//	이미 진행중인 퀘스트는 새로이 시작할 수 없다.
				GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwQuestStartID[dwIndex]);
				if ( pQUEST_PROG )
                    return FALSE;

				GLQUESTPROG *pQUEST_END = pCHARLOGIC->m_cQuestPlay.FindEnd(m_dwQuestStartID[dwIndex]);
				if ( pQUEST_END )
				{
					GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( m_dwQuestStartID[dwIndex] );
					if ( !pQUEST )
                        return FALSE;

					if ( pQUEST_END->m_bCOMPLETE )
					{
						//	이미 완료한 퀘스트의 경우에는 반복 수행이 가능할때만 가능.
						if ( !pQUEST->IsREPEAT() )
                            return FALSE;

						// 이미 완료한 퀘스트가 일일 퀘스트 인가
						if ( pQUEST->IsDaily() )
						{
							CTime cTime = CTime::GetCurrentTime();
							CTime cTimeEnd(pQUEST_END->m_tEndTime);

							if (cTime.GetDay() == cTimeEnd.GetDay() &&
								cTime.GetMonth() == cTimeEnd.GetMonth() &&
								cTime.GetYear() == cTimeEnd.GetYear() )
								return FALSE;
						}
					}
					else
					{
						//	이미 포기한 퀘스트의 경우에는제시도 수행이 가능할때만 가능.
						if ( !pQUEST->IsAGAIN() )
                            return FALSE;
					}
				}

				bOK = TRUE;

			}
		}
		break;

	case EM_QUEST_STEP:
		{
			//	진행중인 퀘스트일때만.
			GLQUESTPROG *pQUEST_PROG = pCHARLOGIC->m_cQuestPlay.FindProc(m_dwACTION_PARAM1);
			if ( !pQUEST_PROG )	return FALSE;
			
			if ( pQUEST_PROG->m_dwSTEP!=m_dwACTION_PARAM2 )		return FALSE;

			bOK = TRUE;
		}
		break;
	default:
		{
			bOK = TRUE;
		}
		break;
	};
	
	return bOK;
}

bool SNpcTalk::IsUscCondition()
{    
	if (!m_spCondition)
        return true; // 왜 true 일까?
	if (m_spCondition->m_bUse)
        return true;

	return false;
}

void SNpcTalk::AddNeedItem(SNATIVEID ItemId, int ItemNum)
{
    SITEM_NUM NeedItem(ItemId, ItemNum);
    m_vecNeedItem.push_back(NeedItem);
}

void SNpcTalk::AddTradeItem(SNATIVEID ItemId, int ItemNum)
{
    SITEM_NUM TradeItem(ItemId, ItemNum);
    m_vecTradeItem.push_back(TradeItem);
}
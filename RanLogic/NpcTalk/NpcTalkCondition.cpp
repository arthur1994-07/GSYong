#include "../pch.h"
#include <strstream>
#include <algorithm>
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "../../RanLogicClient/Party/GLPartyClient.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../Item/GLItemMan.h"
#include "../GLogicEx.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../Quest/GLQUEST.h"
#include "../Quest/GLQUESTMAN.h"
#include "NpcTalkCondition.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

std::string	strCONDITIONSIGN[CONDITIONSIGN_SIZE] =
{
	"N",
	"=",
	"<",
	">",
	"<=",
	">=",
};

void SaveEmConditionSignExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);
    int Row = 0;
    int Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "Sign");
    Xls.SetCell(SheetNum, Row, Col++, "Value");

    for (int i=0; i<CONDITIONSIGN_SIZE; ++i)
    {
        ++Row;
        Col = 0;
        Xls.SetCell(SheetNum, Row, Col++, strCONDITIONSIGN[i]);
        Xls.SetCell(SheetNum, Row, Col++, (int) i);
    }
}

SNpcTalkCondition::SNpcTalkCondition () :
	m_bUse( true ),
	m_signLevel ( CONDITIONSIGN_NONE ),
	m_wLevel ( 0 ),

	m_dwClass ( GLCC_NONE ),
	m_wSchool ( MAX_SCHOOL ),

	m_signElemental ( CONDITIONSIGN_NONE ),
	m_nElemental ( 0 ),

	m_signActionPoint ( CONDITIONSIGN_NONE ),
	m_nActionPoint ( 0 ),

	m_bTime ( FALSE ),
	m_dwTimeBegin ( 0 ),
	m_dwTimeEnd ( 0 ),

	m_dwQUEST_NID(UINT_MAX),
	m_dwQUEST_STEP(0),

	m_dwMONEY(0),
	m_dwPARTYMEN_NUM(0)
{
}

/*
BOOL SNpcTalkCondition::LOADCSVFILE( CStringArray &StrArray, int iReadStart, int iLineNumber )
{
	int iCsvCur = iReadStart;
	if( StrArray.GetSize() < iCsvCur ) 
		return TRUE;

	// Use
	m_bUse = (0!=atoi(StrArray[iCsvCur++]));
	// ConditionLevel 
	m_wLevel =			atoi(StrArray[iCsvCur++]);
	// ConditionLevelsize
	m_signLevel =		(EM_CONDITIONSIGN)atoi(StrArray[iCsvCur++]); 
	// ConditionClass
	m_dwClass =			atoi(StrArray[iCsvCur++]); 
	// ConditionElemental 
	m_nElemental =		atoi(StrArray[iCsvCur++]); 
	// ConditionElementalSize 
	m_signElemental =	(EM_CONDITIONSIGN)atoi(StrArray[iCsvCur++]); 
	// ConditionLivingScroe 
	m_nActionPoint =	atoi(StrArray[iCsvCur++]); 
	// ConditionLivingScoreSize 
	m_signActionPoint = (EM_CONDITIONSIGN)atoi(StrArray[iCsvCur++]); 
	// ConditionQuestMID & ConditionQuestSID
    std::string szTemp33 = StrArray[iCsvCur];
	int MID = atoi(StrArray[iCsvCur++]);
	int SID = atoi(StrArray[iCsvCur++]);
	m_dwQUEST_NID = MID | ( SID << 0x10 );

	int i = 0;
    std::string szTemp;
	char szTempChar[128];
	// CondtionDonotQuest 
	m_vecDisQuest.clear();
	szTemp = StrArray[iCsvCur++];
	if( szTemp == "NULL" )
	{		
	}
    else
    {
        std::string strTemp;
		strcpy_s( szTempChar, 128, szTemp.c_str() );
		for( i = 0; i < (int)strlen(szTempChar); i++ )
		{
			if( szTempChar[i] == '|' )
			{
				m_vecDisQuest.push_back( atoi(strTemp.c_str() ) ) ;
				strTemp.clear();
			}
            else
            {
                strTemp += szTempChar[i];
			}
		}		
	}
    std::vector<int> vecMID;
	std::vector<int> vecSID;
	std::vector<int> vecNUM;
	vecMID.clear();
	vecSID.clear();
	vecNUM.clear();
	// ConditionRequestitemMID 
	szTemp = StrArray[iCsvCur++];
	if( szTemp == "NULL" )
	{		
	}
    else
    {

		
        std::string strTemp;
		strcpy_s( szTempChar, 128, szTemp.c_str() );
		for( i = 0; i < (int)strlen(szTempChar); i++ )
		{
			if( szTempChar[i] == '|' )
			{
				vecMID.push_back( atoi(strTemp.c_str() ) ) ;
				strTemp.clear();
			}
            else
            {
				strTemp += szTempChar[i];
			}
		}		
	}
	// ConditionRequestItemSID 
	szTemp = StrArray[iCsvCur++];
	if( szTemp == "NULL" )
	{		
	}
    else
    {
		
        std::string strTemp;
		strcpy_s( szTempChar, 128, szTemp.c_str() );
		for( i = 0; i < (int)strlen(szTempChar); i++ )
		{
			if( szTempChar[i] == '|' )
			{
				vecSID.push_back( atoi(strTemp.c_str() ) ) ;
				strTemp.clear();
			}
            else
            {
				strTemp += szTempChar[i];
			}
		}		
	}
	//	아이템 갯수
	szTemp = StrArray[iCsvCur++];
	if( szTemp == "NULL" )
	{		
	}
    else
    {		
        std::string strTemp;
		strcpy_s( szTempChar, 128, szTemp.c_str() );
		for( i = 0; i < (int)strlen(szTempChar); i++ )
		{
			if( szTempChar[i] == '|' )
			{
				vecNUM.push_back( atoi(strTemp.c_str() ) ) ;
				strTemp.clear();
			}else{
				strTemp += szTempChar[i];
			}
		}		
	}

	if( vecMID.size() != vecSID.size() || vecMID.size() != vecNUM.size() )
	{
		sprintf_s( szTempChar, 128, "CSV의 ConditionRequestItemMID 갯수와 ConditionRequestItemSID 갯수가 다릅니다.\n %d번째 줄", iLineNumber );
		MessageBox( NULL, szTempChar, "오류", MB_OK );
		return FALSE;
	}

	// input ConditionRequest ID
	m_vecHaveItem.clear();
	for( i = 0; i < (int)vecSID.size(); i++ )
	{
		SITEM_NUM sItemNum;

		sItemNum.sNativeID.wMainID = vecMID[i];
		sItemNum.sNativeID.wSubID = vecSID[i];
		sItemNum.dwNum = vecNUM[i];		

		m_vecHaveItem.push_back( sItemNum );
	}
	
	// ConditionCompletionQuest 
	m_vecCompleteQuest.clear();
	szTemp = StrArray[iCsvCur++];
	if( szTemp == "NULL" )
	{		
	}
    else
    {
        std::string strTemp;
		strcpy_s( szTempChar, 128, szTemp.c_str() );
		for( i = 0; i < (int)strlen(szTempChar); i++ )
		{
			if( szTempChar[i] == '|' )
			{
				m_vecCompleteQuest.push_back( atoi(strTemp.c_str() ) ) ;
				strTemp.clear();
			}
            else
            {
				strTemp += szTempChar[i];
			}
		}		
	}
	m_wSchool = atoi(StrArray[iCsvCur++]);
	return TRUE;

	
}
*/
/*
BOOL SNpcTalkCondition::SAVECSVFILE( std::string &strLine )
{

	char szTempChar[128];
	ZeroMemory( szTempChar, 128 );

	
	// Use
	sprintf_s( szTempChar, 128, "%d", (int)m_bUse );
	strLine += szTempChar; strLine += ",";
	// ConditionLevel 
	sprintf_s( szTempChar, 128, "%d", m_wLevel );
	strLine += szTempChar; strLine += ",";
	// ConditionLevelsize
	sprintf_s( szTempChar, 128, "%d", m_signLevel );
	strLine += szTempChar; strLine += ",";
	// ConditionClass
	sprintf_s( szTempChar, 128, "%d", m_dwClass );
	strLine += szTempChar; strLine += ",";
	// ConditionElemental 
	sprintf_s( szTempChar, 128, "%d", m_nElemental );
	strLine += szTempChar; strLine += ",";
	// ConditionElementalSize 
	sprintf_s( szTempChar, 128, "%d", m_signElemental );
	strLine += szTempChar; strLine += ",";
	// ConditionLivingScroe 
	sprintf_s( szTempChar, 128, "%d", m_nActionPoint );
	strLine += szTempChar; strLine += ",";
	// ConditionLivingScoreSize 
	sprintf_s( szTempChar, 128, "%d", m_signActionPoint );
	strLine += szTempChar; strLine += ",";
	// ConditionQuestMID 
	sprintf_s( szTempChar, 128, "%d", m_dwQUEST_NID & 0xffff );
	strLine += szTempChar; strLine += ",";
	// ConditionQuestSID 
	sprintf_s( szTempChar, 128, "%d", m_dwQUEST_NID >> 0x0010 );
	strLine += szTempChar; strLine += ",";

	int j = 0;
	// CondtionDonotQuest 
	if( m_vecDisQuest.size() != 0 )
	{
		for( j = 0; j < (int)m_vecDisQuest.size(); j++ )
		{
			sprintf_s( szTempChar, 128, "%d", m_vecDisQuest[j] );					

			strLine += szTempChar; strLine += "|"; 
		}
		strLine += ",";
	}else{
		strLine += "NULL"; strLine += ",";
	}
	//// ConditionRequestitemMID 
	if( m_vecHaveItem.size() != 0 )
	{
		for( j = 0; j < (int)m_vecHaveItem.size(); j++ )
		{
			SNATIVEID nID = m_vecHaveItem[j].sNativeID;
			sprintf_s( szTempChar, 128, "%d", nID.wMainID );					

			strLine += szTempChar; strLine += "|"; 
		}
		strLine += ",";
	}else{
		strLine += "NULL"; strLine += ",";
	}



	// ConditionRequestItemSID ////////
	if( m_vecHaveItem.size() != 0 )
	{
		for( j = 0; j < (int)m_vecHaveItem.size(); j++ )
		{
			SNATIVEID nID = m_vecHaveItem[j].sNativeID;
			sprintf_s( szTempChar, 128, "%d", nID.wSubID );					

			strLine += szTempChar; strLine += "|"; 
		}
		strLine += ",";
	}else{
		strLine += "NULL"; strLine += ",";
	}

	//	필요 아이템 갯수
	if( m_vecHaveItem.size() != 0 )
	{
		for( j = 0; j < (int)m_vecHaveItem.size(); j++ )
		{
			sprintf_s( szTempChar, 128, "%d", m_vecHaveItem[j].dwNum );					

			strLine += szTempChar; strLine += "|"; 
		}
		strLine += ",";
	}else{
		strLine += "NULL"; strLine += ",";
	}


	// ConditionCompletionQuest 
	if( m_vecCompleteQuest.size() != 0 )
	{
		for( j = 0; j < (int)m_vecCompleteQuest.size(); j++ )
		{
			sprintf_s( szTempChar, 128, "%d", m_vecCompleteQuest[j] );					

			strLine += szTempChar; strLine += "|"; 
		}
		strLine += ",";
	}else{
		strLine += "NULL"; strLine += ",";
	}
	// ConditionSchool
	sprintf_s( szTempChar, 128, "%d", m_wSchool );
	strLine += szTempChar; strLine += ",";

	return TRUE;
}
*/
BOOL SNpcTalkCondition::SAVE ( sc::SerialFile& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}
	SFile << (WORD)VERSION;

	SFile << m_bUse;

	SFile << (DWORD)m_signLevel;
	SFile << m_wLevel;

	SFile << m_dwClass;
	SFile << m_wSchool;

	SFile << (DWORD)m_signElemental;
	SFile << m_nElemental;

	SFile << (DWORD)m_signActionPoint;
	SFile << m_nActionPoint;

	SFile << m_bTime;
	SFile << m_dwTimeBegin;
	SFile << m_dwTimeEnd;


	SFile << m_dwMONEY;
	SFile << m_dwPARTYMEN_NUM;

	SFile << m_dwQUEST_NID;
	SFile << m_dwQUEST_STEP;

	{
		SFile << (int)m_vecCompleteQuest.size();
		PROCSKILL_ITER iter = m_vecCompleteQuest.begin();
		PROCSKILL_ITER iter_end = m_vecCompleteQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SFile << (DWORD) (*iter);
		}
	}

	{
		SFile << (int)m_vecHaveItem.size();
		HAVEITEM_ITER iter = m_vecHaveItem.begin();
		HAVEITEM_ITER iter_end = m_vecHaveItem.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEM_NUM sItemNum = (SITEM_NUM) (*iter);
			SFile.WriteBuffer ( &sItemNum, sizeof( SITEM_NUM ) );
		}
	}

	{
		SFile << (int)m_vecLearnSkill.size();
		LEARNSKILL_ITER iter = m_vecLearnSkill.begin();
		LEARNSKILL_ITER iter_end = m_vecLearnSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const SSkillCondition &sSkillCondition = (*iter);
			SFile.WriteBuffer ( &sSkillCondition, sizeof ( SSkillCondition ) );
		}
	}

	{
		SFile << (int)m_vecDisQuest.size();
		PROCQUEST_ITER iter = m_vecDisQuest.begin();
		PROCQUEST_ITER iter_end = m_vecDisQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SFile << (DWORD) (*iter);
		}
	}


	{
		SFile << (int)m_vecDisSkill.size();
		PROCSKILL_ITER iter = m_vecDisSkill.begin();
		PROCSKILL_ITER iter_end = m_vecDisSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SFile << (DWORD) (*iter);
		}
	}

    {
        SFile << (int)m_vecSkillFact.size();
        SKILLFACT_ITER iter = m_vecSkillFact.begin();
        SKILLFACT_ITER iter_end = m_vecSkillFact.end();
        for ( ; iter!=iter_end; ++iter )
        {
            const SSkillCondition &sSkillCondition = (*iter);
            SFile.WriteBuffer ( &sSkillCondition, sizeof ( SSkillCondition ) );
        }
    }

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0001 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	BOOL bLevel(FALSE);
	BOOL bClass(FALSE);
	BOOL bElemental(FALSE);
	BOOL bActionPoint(FALSE);
	
	BOOL bHaveItem(FALSE);
	BOOL bLearnSkill(FALSE);
	int nCount = 0;

	SFile >> bLevel;
	SFile >> m_wLevel;
	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	if ( !bLevel )		m_signLevel = CONDITIONSIGN_NONE;

	SFile >> bClass;
	SFile >> m_dwClass;
	if ( !bClass )		m_dwClass = GLCC_ALL;

	SFile >> bElemental;
	SFile >> m_nElemental;

	SFile >> bElemental;
	SFile >> m_nElemental;
	SFile >> dwData; m_signElemental = (EM_CONDITIONSIGN) dwData;
	if ( !bElemental )		m_signElemental = CONDITIONSIGN_NONE;

	SFile >> bActionPoint;
	SFile >> m_nActionPoint;
	SFile >> dwData; m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	if ( !bActionPoint )		m_signActionPoint = CONDITIONSIGN_NONE;


	SFile >> bHaveItem;
	SFile >> nCount;
	m_vecHaveItem.clear ();

	if ( bHaveItem )
	{
		for ( int i = 0; i < nCount; i++ )
		{
			SITEM_NUM sItemNum;
			SFile >> sItemNum.sNativeID.dwID;
			
			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
			if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
			else
			{
				sItemNum.dwNum = 1;
			}

			m_vecHaveItem.push_back ( sItemNum );			
		}
	}




	SFile >> bLearnSkill;
	SFile >> nCount;
	m_vecLearnSkill.clear ();
	for ( int i = 0; i < nCount; i++ )
	{
		SSkillCondition sSkillCondition;
		SFile.ReadBuffer ( &sSkillCondition, sizeof ( SSkillCondition ) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}
	if ( !bLearnSkill )		m_vecLearnSkill.clear();

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0002 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	for ( int i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( int i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0003 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	for ( int i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( int i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( int i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}
	
	SFile >> nCount;
	for ( int i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}
	
	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0004 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}
	
	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}
	
	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0005 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;
	SFile >> m_wSchool;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}
	
	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}
	
	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0006 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;
	SFile >> m_wSchool;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}
	
	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}
	
	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0007 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;
	SFile >> m_wSchool;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}


	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0008 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> m_bUse;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;
	SFile >> m_wSchool;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		SFile >> sItemNum.sNativeID.dwID;
			
		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sItemNum.sNativeID.dwID );
		if ( pITEM ) sItemNum.dwNum = pITEM->sDrugOp.wPileNum;
		else
		{
			sItemNum.dwNum = 1;
		}

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}


	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}

/*
	// 클래스 조건 자동 체크 기능...필요할때만 사용
	//	남자 클래스
	//	남자 일반 클래스
	DWORD dwManClass = GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M;
	DWORD dwWomanClass = GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W;
	DWORD dwManClassEx = GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M | GLCC_EXTREME_M;
	DWORD dwWomanClassEx = GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME_W;

	// 모든 클래스
	if ( m_dwClass == GLCC_ALL_NEWSEX )
	{
		m_dwClass = GLCC_ALL_SCIENTIST;			
	}
	else if ( m_dwClass == dwManClass + dwWomanClass )
	{
		m_dwClass += GLCC_SCIENTIST_M + GLCC_SCIENTIST_W;
	}
	// 남자, 여자, 일반부서
	else if ( m_dwClass == dwManClass || m_dwClass == dwManClassEx ) 
	{
		m_dwClass += GLCC_SCIENTIST_M;
	}
	else if ( m_dwClass == dwWomanClass  || m_dwClass == dwWomanClassEx ) 
	{
		m_dwClass += GLCC_SCIENTIST_W;
	}
*/

	return TRUE;
}

BOOL SNpcTalkCondition::LOAD_0009 ( sc::BaseStream& SFile )
{
	DWORD dwData;
	int nCount(0);
	SSkillCondition sSkillCondition;

	SFile >> m_bUse;

	SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
	SFile >> m_wLevel;

	SFile >> m_dwClass;
	SFile >> m_wSchool;

	SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nElemental;

	SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
	SFile >> m_nActionPoint;

	SFile >> m_bTime;
	SFile >> m_dwTimeBegin;
	SFile >> m_dwTimeEnd;

	SFile >> m_dwMONEY;
	SFile >> m_dwPARTYMEN_NUM;

	SFile >> m_dwQUEST_NID;
	SFile >> m_dwQUEST_STEP;

	SFile >> nCount;
	int i = 0;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecCompleteQuest.push_back(dwData);
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i)
	{
		SITEM_NUM sItemNum;
		
		SFile.ReadBuffer ( &sItemNum, sizeof( SITEM_NUM ) );

		m_vecHaveItem.push_back ( sItemNum );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
		m_vecLearnSkill.push_back ( sSkillCondition );
	}

	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisQuest.push_back(dwData);
	}


	SFile >> nCount;
	for ( i=0; i<nCount; ++i )
	{
		SFile >> dwData;
		m_vecDisSkill.push_back(dwData);
	}

	return TRUE;
}


BOOL SNpcTalkCondition::LOAD_0010 ( sc::BaseStream& SFile )
{
    DWORD dwData;
    int nCount(0);
    SSkillCondition sSkillCondition;

    SFile >> m_bUse;

    SFile >> dwData;	m_signLevel = (EM_CONDITIONSIGN) dwData;
    SFile >> m_wLevel;

    SFile >> m_dwClass;
    SFile >> m_wSchool;

    SFile >> dwData;	m_signElemental = (EM_CONDITIONSIGN) dwData;
    SFile >> m_nElemental;

    SFile >> dwData;	m_signActionPoint = (EM_CONDITIONSIGN) dwData;
    SFile >> m_nActionPoint;

    SFile >> m_bTime;
    SFile >> m_dwTimeBegin;
    SFile >> m_dwTimeEnd;

    SFile >> m_dwMONEY;
    SFile >> m_dwPARTYMEN_NUM;

    SFile >> m_dwQUEST_NID;
    SFile >> m_dwQUEST_STEP;

    SFile >> nCount;
    int i = 0;
    for ( i=0; i<nCount; ++i )
    {
        SFile >> dwData;
        m_vecCompleteQuest.push_back(dwData);
    }

    SFile >> nCount;
    for ( i=0; i<nCount; ++i)
    {
        SITEM_NUM sItemNum;

        SFile.ReadBuffer ( &sItemNum, sizeof( SITEM_NUM ) );

        m_vecHaveItem.push_back ( sItemNum );
    }

    SFile >> nCount;
    for ( i=0; i<nCount; ++i )
    {
        SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
        m_vecLearnSkill.push_back ( sSkillCondition );
    }

    SFile >> nCount;
    for ( i=0; i<nCount; ++i )
    {
        SFile >> dwData;
        m_vecDisQuest.push_back(dwData);
    }


    SFile >> nCount;
    for ( i=0; i<nCount; ++i )
    {
        SFile >> dwData;
        m_vecDisSkill.push_back(dwData);
    }

    SFile >> nCount;
    for ( i=0; i<nCount; ++i )
    {
        SFile.ReadBuffer ( &sSkillCondition, sizeof(sSkillCondition) );
        m_vecSkillFact.push_back ( sSkillCondition );
    }

    return TRUE;
}

BOOL SNpcTalkCondition::LOAD ( sc::BaseStream& SFile )
{
	if ( !SFile.IsOpen () )
	{
		GASSERT ( 0 && "파일이 열려있지 않습니다." );
		return FALSE;
	}

	WORD wVer(0);
	SFile >> wVer;

	switch ( wVer )
	{
	case 0x0001:	LOAD_0001(SFile);	break;
	case 0x0002:	LOAD_0002(SFile);	break;
	case 0x0003:	LOAD_0003(SFile);	break;
	case 0x0004:	LOAD_0004(SFile);	break;
	case 0x0005:	LOAD_0005(SFile);	break;
	case 0x0006:	LOAD_0006(SFile);	break;
	case 0x0007:	LOAD_0007(SFile);	break;
	case 0x0008:	LOAD_0008(SFile);	break;
    case 0x0009:	LOAD_0009(SFile);	break;
    case 0x0010:	LOAD_0010(SFile);	break;
	};

	return TRUE;
}

void SNpcTalkCondition::Reset ()
{
	m_bUse = true;
	m_signLevel = CONDITIONSIGN_NONE;
	m_wLevel = 0;

	m_dwClass = GLCC_NONE;
	m_wSchool = MAX_SCHOOL;

	m_signElemental = CONDITIONSIGN_NONE;
	m_nElemental = 0;

	m_signActionPoint = CONDITIONSIGN_NONE;
	m_nActionPoint = 0;

	m_bTime = FALSE;
	m_dwTimeBegin = 0;
	m_dwTimeEnd = 0;

	m_dwQUEST_NID = UINT_MAX;
	m_dwQUEST_STEP = 0;

	m_vecHaveItem.clear ();
	m_vecLearnSkill.clear ();

	m_vecCompleteQuest.clear ();

	m_vecDisSkill.clear ();
	m_vecDisQuest.clear ();
    m_vecSkillFact.clear ();

	m_dwMONEY = 0;
	m_dwPARTYMEN_NUM = 0;
}

SNpcTalkCondition&	SNpcTalkCondition::operator= ( const SNpcTalkCondition& rNpcTalkCondition )
{
	m_bUse = rNpcTalkCondition.m_bUse;
	m_signLevel = rNpcTalkCondition.m_signLevel;
	m_wLevel = rNpcTalkCondition.m_wLevel;

	m_dwClass = rNpcTalkCondition.m_dwClass;
	m_wSchool = rNpcTalkCondition.m_wSchool;

	m_signElemental = rNpcTalkCondition.m_signElemental;
	m_nElemental = rNpcTalkCondition.m_nElemental;

	m_signActionPoint = rNpcTalkCondition.m_signActionPoint;
	m_nActionPoint = rNpcTalkCondition.m_nActionPoint;

	m_bTime = rNpcTalkCondition.m_bTime;
	m_dwTimeBegin = rNpcTalkCondition.m_dwTimeBegin;
	m_dwTimeEnd = rNpcTalkCondition.m_dwTimeEnd;

	m_dwQUEST_NID = rNpcTalkCondition.m_dwQUEST_NID;
	m_dwQUEST_STEP = rNpcTalkCondition.m_dwQUEST_STEP;

	m_vecHaveItem = rNpcTalkCondition.m_vecHaveItem;
	m_vecLearnSkill = rNpcTalkCondition.m_vecLearnSkill;
    m_vecSkillFact = rNpcTalkCondition.m_vecSkillFact;

	m_vecCompleteQuest = rNpcTalkCondition.m_vecCompleteQuest;

	m_vecDisSkill = rNpcTalkCondition.m_vecDisSkill;
	m_vecDisQuest = rNpcTalkCondition.m_vecDisQuest;

	m_dwMONEY = rNpcTalkCondition.m_dwMONEY;
	m_dwPARTYMEN_NUM = rNpcTalkCondition.m_dwPARTYMEN_NUM;

	return *this;
}

BOOL SNpcTalkCondition::ADDHAVEITEM ( DWORD dwNID, DWORD dwNum )
{
	HAVEITEM_ITER iter = m_vecHaveItem.begin();
	HAVEITEM_ITER iter_end = m_vecHaveItem.end();

	for ( ; iter != iter_end; ++iter )
	{
		SITEM_NUM sItemNum = (SITEM_NUM)(*iter);
		if ( sItemNum.sNativeID.dwID == dwNID ) return FALSE;
	}

	SITEM_NUM sItemNumNew;
	sItemNumNew.sNativeID = dwNID;
	sItemNumNew.dwNum = dwNum;

	m_vecHaveItem.push_back ( sItemNumNew );
	std::sort ( m_vecHaveItem.begin(), m_vecHaveItem.end(), HAVE_ITEM_CMP() );

	return TRUE;
}

BOOL SNpcTalkCondition::DELHAVEITEM ( DWORD dwNID )
{
	HAVEITEM_ITER iter = m_vecHaveItem.begin();
	HAVEITEM_ITER iter_end = m_vecHaveItem.end();

	for ( ; iter != iter_end; ++iter )
	{
		SITEM_NUM sItemNum = (SITEM_NUM)(*iter);
		if ( sItemNum.sNativeID.dwID == dwNID ) 
		{
			m_vecHaveItem.erase( iter );

			return TRUE;
		}
	}


	return TRUE;
}

BOOL SNpcTalkCondition::ADDLEARNSKILL ( const SSkillCondition &sSkillCondition )
{
	bool bfound = std::binary_search ( m_vecLearnSkill.begin(), m_vecLearnSkill.end(), sSkillCondition );
	if ( bfound )	return FALSE;

	m_vecLearnSkill.push_back ( sSkillCondition );
	std::sort ( m_vecLearnSkill.begin(), m_vecLearnSkill.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::DELLEARNSKILL ( DWORD dwNID )
{
	SSkillCondition sSkillCondition;
	sSkillCondition.dwNID = dwNID;

	m_vecLearnSkill.erase ( std::remove(m_vecLearnSkill.begin(),m_vecLearnSkill.end(),sSkillCondition), m_vecLearnSkill.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::ADDDISSKILL ( DWORD dwNID )
{
	bool bfound = std::binary_search ( m_vecDisSkill.begin(), m_vecDisSkill.end(), dwNID );
	if ( bfound )	return FALSE;

	m_vecDisSkill.push_back ( dwNID );
	std::sort ( m_vecDisSkill.begin(), m_vecDisSkill.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::DELDISSKILL ( DWORD dwNID )
{
	m_vecDisSkill.erase ( std::remove(m_vecDisSkill.begin(),m_vecDisSkill.end(),dwNID), m_vecDisSkill.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::ADDDISQUEST ( DWORD dwNID )
{
	bool bfound = std::binary_search ( m_vecDisQuest.begin(), m_vecDisQuest.end(), dwNID );
	if ( bfound )	return FALSE;

	m_vecDisQuest.push_back ( dwNID );
	std::sort ( m_vecDisQuest.begin(), m_vecDisQuest.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::DELDISQUEST ( DWORD dwNID )
{
	m_vecDisQuest.erase ( std::remove(m_vecDisQuest.begin(),m_vecDisQuest.end(),dwNID), m_vecDisQuest.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::ADDCOMQUEST ( DWORD dwNID )
{
	bool bfound = std::binary_search ( m_vecCompleteQuest.begin(), m_vecCompleteQuest.end(), dwNID );
	if ( bfound )	return FALSE;

	m_vecCompleteQuest.push_back ( dwNID );
	std::sort ( m_vecCompleteQuest.begin(), m_vecCompleteQuest.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::DELCOMQUEST ( DWORD dwNID )
{
	m_vecCompleteQuest.erase ( std::remove(m_vecCompleteQuest.begin(),m_vecCompleteQuest.end(),dwNID), m_vecCompleteQuest.end() );

	return TRUE;
}

BOOL SNpcTalkCondition::ADDSKILLFACT ( const SSkillCondition &sSkillCondition )
{
    bool bfound = std::binary_search ( m_vecSkillFact.begin(), m_vecSkillFact.end(), sSkillCondition );
    if ( bfound )	return FALSE;

    m_vecSkillFact.push_back ( sSkillCondition );
    std::sort ( m_vecSkillFact.begin(), m_vecSkillFact.end() );

    return TRUE;
}

BOOL SNpcTalkCondition::DELSKILLFACT ( DWORD dwNID )
{
    SSkillCondition sSkillCondition;
    sSkillCondition.dwNID = dwNID;

    m_vecSkillFact.erase ( std::remove(m_vecSkillFact.begin(),m_vecSkillFact.end(),sSkillCondition), m_vecSkillFact.end() );

    return TRUE;
}

BOOL SNpcTalkCondition::DoTEST(GLCHARLOGIC* pCHAR, DWORD PartyMemberNum, int iSvrHour)
{
	BOOL bValid(FALSE);
	SNATIVEID sNID;

	if ( !m_bUse ) return FALSE;
	
	//	레벨
	bValid = NumericCompare ( m_wLevel, pCHAR->m_wLevel, m_signLevel );
	if ( !bValid )	return FALSE;

	//	클래스
	bValid = (m_dwClass&pCHAR->m_emClass)!=NULL;
	if ( !bValid )	return FALSE;

	if ( m_wSchool!=MAX_SCHOOL )
	{
		if ( m_wSchool!=pCHAR->m_wSchool )		return FALSE;
	}

	//	Elemental
	bValid = NumericCompare ( m_nElemental, pCHAR->m_nBright, m_signElemental );
	if ( !bValid )	return FALSE;

	//	생활점수
	bValid = NumericCompare ( m_nActionPoint, pCHAR->m_nBright, m_signActionPoint );
	if ( !bValid )	return FALSE;

	DWORD dwCurHour = iSvrHour;// GLPeriod::GetInstance().GetHour ();
	if ( m_bTime && !((m_dwTimeBegin<=dwCurHour)&&(dwCurHour<=m_dwTimeEnd)) )
	{
		return FALSE;
	}

	//	퀘스트 진행중인지 검사.
	if ( m_dwQUEST_NID!=UINT_MAX )
	{
		GLQUESTPROG *pQPROC = pCHAR->m_cQuestPlay.FindProc ( m_dwQUEST_NID );
		if ( !pQPROC )								return FALSE;
		if ( pQPROC->m_dwSTEP!=m_dwQUEST_STEP )		return FALSE;
	}

	//	소지 아이템 검사.
	{
		HAVEITEM_ITER iter = m_vecHaveItem.begin ();
		HAVEITEM_ITER iter_end = m_vecHaveItem.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			SITEM_NUM sItemNum = (*iter);
			if ( !pCHAR->ISHAVEITEM ( sItemNum.sNativeID, sItemNum.dwNum ) )	return FALSE;
		}
	}
	
	//	소지 스킬 검사.
	{
		LEARNSKILL_ITER iter = m_vecLearnSkill.begin ();
		LEARNSKILL_ITER iter_end = m_vecLearnSkill.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			sNID.dwID = (*iter).dwNID;	
			//										SKILL은 0 == 1레벨이다. 즉 -1 해줘야 한다.
			if ( !pCHAR->ISLEARNED_SKILL ( sNID, (*iter).wLevel-1 ) )	return FALSE;
		}
	}

    //	버프 검사.
    {
        SKILLFACT_ITER iter = m_vecSkillFact.begin ();
        SKILLFACT_ITER iter_end = m_vecSkillFact.end ();
        for ( ; iter!=iter_end; ++iter )
        {
            WORD wSlot = 0;

            sNID.dwID = (*iter).dwNID;
            if ( !pCHAR->IS_HAVE_SKILLFACT ( sNID, (*iter).wLevel, wSlot ) )	return FALSE;
        }
    }

	//	이 퀘스트를 받기 위해서는 이미 수행했어야 하는 퀘스트.
	{
		GLQUESTPROG *pQUEST(NULL);

		PROCQUEST_ITER iter = m_vecCompleteQuest.begin();
		PROCQUEST_ITER iter_end = m_vecCompleteQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			pQUEST = pCHAR->m_cQuestPlay.FindEnd ( (*iter) );
			if ( !pQUEST || !pQUEST->m_bCOMPLETE )	return FALSE;
		}
	}

	//	이 퀘스트를 받기 위해서는 습득하지 않아야할 스킬.
	{
		PROCSKILL_ITER iter = m_vecDisSkill.begin();
		PROCSKILL_ITER iter_end = m_vecDisSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			BOOL bLEARNED = pCHAR->ISLEARNED_SKILL(SNATIVEID(*iter));
			if ( bLEARNED )		return FALSE;
		}
	}

	//	이 퀘스트를 받기 위해서는 수행하지 않아야할 퀘스트.
	{
		GLQUESTPROG *pQUEST(NULL);

		PROCQUEST_ITER iter = m_vecDisQuest.begin();
		PROCQUEST_ITER iter_end = m_vecDisQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			pQUEST = pCHAR->m_cQuestPlay.FindProc ( (*iter) );
			if ( pQUEST )		return FALSE;
			
			pQUEST = pCHAR->m_cQuestPlay.FindEnd ( (*iter) );
			if ( pQUEST && pQUEST->m_bCOMPLETE )	return FALSE;
		}
	}

	//	필요 소지 금액.
	if (pCHAR->GetInvenMoney() < m_dwMONEY)
        return FALSE;

	// 필요 파티 인원.
	DWORD dwPARTYNUM = PartyMemberNum; //GLGaeaClient::GetInstance().GetPartyClient()->GetMemberNum();
	if (dwPARTYNUM < m_dwPARTYMEN_NUM)
        return FALSE;
    else
	    return TRUE;
}

BOOL SNpcTalkCondition::EmptyCondition()
{
	if( m_signLevel != CONDITIONSIGN_NONE )
		return FALSE;
	if( m_wLevel )
		return FALSE;

	if( m_dwClass != GLCC_NONE )
		return FALSE;
	if( m_wSchool != MAX_SCHOOL )
		return FALSE;

	if( m_signElemental != CONDITIONSIGN_NONE )
		return FALSE;
	if( m_nElemental )
		return FALSE;

	if( m_signActionPoint != CONDITIONSIGN_NONE )
		return FALSE;
	if( m_nActionPoint )
		return FALSE;

	if( m_bTime )
		return FALSE;
	if( m_dwTimeBegin )
		return FALSE;
	if( m_dwTimeEnd )
		return FALSE;

	if( m_dwQUEST_NID != UINT_MAX )
		return FALSE;
	if( m_dwQUEST_STEP )
		return FALSE;

	if( !m_vecHaveItem.empty() )
		return FALSE;
	if( !m_vecLearnSkill.empty() )
		return FALSE;

	if( !m_vecCompleteQuest.empty() )
		return FALSE;

	if( !m_vecDisSkill.empty() )
		return FALSE;
	if( !m_vecDisQuest.empty() )
		return FALSE;

    if ( !m_vecSkillFact.empty() )
        return FALSE;

	if( m_dwMONEY )
		return FALSE;
	if( m_dwPARTYMEN_NUM )
		return FALSE;

	return TRUE;
}

std::string SNpcTalkCondition::GetBoolString ( BOOL bVALUE )
{
	const static	char szTRUE[] = "TRUE";
	const static	char szFALSE[] = "FALSE";
	
	if ( bVALUE )	return szTRUE;
	else			return szFALSE;
}

void SNpcTalkCondition::GetConditionText( std::string& strName )
{
	std::strstream strCondition;

	if ( !m_bUse ) 
	{
		strCondition << "USE : FALSE" << "\r\n";
	}

	if ( m_signLevel!=CONDITIONSIGN_NONE )
	{
		strCondition << "Level : " << m_wLevel << strCONDITIONSIGN[m_signLevel] << "Character Value" << "\r\n";
	}

	if ( m_dwClass!=GLCC_ALL_TRICKER)
	{
		strCondition << "Class : FightM[" << GetBoolString(m_dwClass&GLCC_FIGHTER_M) << "], "					 
					 << "FightW[" << GetBoolString(m_dwClass&GLCC_FIGHTER_W) << "], "
					 << "FencingM[" << GetBoolString(m_dwClass&GLCC_ARMS_M) << "], "
					 << "FencingW[" << GetBoolString(m_dwClass&GLCC_ARMS_W) << "], "
					 << "ArcheryM[" << GetBoolString(m_dwClass&GLCC_ARCHER_M) << "], "
					 << "ArcheryW[" << GetBoolString(m_dwClass&GLCC_ARCHER_W) << "], "
					 << "SpiritM[" << GetBoolString(m_dwClass&GLCC_SPIRIT_M) << "], "
					 << "SpiritW[" << GetBoolString(m_dwClass&GLCC_SPIRIT_W) << "], "
					 << "ExtremeM[" << GetBoolString(m_dwClass&GLCC_EXTREME_M) << "], "
					 << "ExtremeW[" << GetBoolString(m_dwClass&GLCC_EXTREME_W) << "]" 
					 << "ScientistM[" << GetBoolString(m_dwClass&GLCC_SCIENTIST_M) << "]" 
					 << "ScientistW[" << GetBoolString(m_dwClass&GLCC_SCIENTIST_W) << "]" 
                     << "AssassinM[" << GetBoolString(m_dwClass&GLCC_ASSASSIN_M) << "]"
                     << "AssassinW[" << GetBoolString(m_dwClass&GLCC_ASSASSIN_W) << "]"
					 << "TrickerM[" << GetBoolString(m_dwClass&GLCC_TRICKER_M) << "]"
					 << "TrickerW[" << GetBoolString(m_dwClass&GLCC_TRICKER_W) << "]"
                     << "EtcM[" << GetBoolString(m_dwClass&GLCC_ETC_M) << "]"
                     << "EtcW[" << GetBoolString(m_dwClass&GLCC_ETC_W) << "]"
					 << "ActorM[" << GetBoolString(m_dwClass&GLCC_ACTOR_M) << "]"
					 << "ActorW[" << GetBoolString(m_dwClass&GLCC_ACTOR_W) << "]"
					 << "\r\n";
	}

	if ( m_wSchool!=MAX_SCHOOL )
	{
		strCondition << "School : " << GLCONST_CHAR::strSCHOOLNAME[m_wSchool] << "\r\n";
	}

	if ( m_signElemental!=CONDITIONSIGN_NONE )
	{
		strCondition << "Elemental : " << m_nElemental << strCONDITIONSIGN[m_signElemental] << "Character Value" << "\r\n";
	}

	if ( m_signActionPoint!=CONDITIONSIGN_NONE )
	{
		strCondition << "Living Score : " << m_nActionPoint << strCONDITIONSIGN[m_signActionPoint] << "Character Value" << "\r\n";
	}

	if ( m_bTime )
	{
		strCondition << "Living Score : " << m_dwTimeBegin << m_dwTimeEnd << "Character Value" << "\r\n";
	}

    if (m_dwQUEST_NID != UINT_MAX)
    {
	    GLQUEST* pQUEST = GLQuestMan::GetInstance().Find(m_dwQUEST_NID);
	    if (pQUEST)
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
				strCondition << "Progressing Quest : " << CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() << " Step : " << m_dwQUEST_STEP << "\r\n";
			}
			else
			{
				strCondition << "Progressing Quest : " << pQUEST->GetTITLE() << " Step : " << m_dwQUEST_STEP << "\r\n";
			}
		}
    }

	if ( !m_vecHaveItem.empty() )
	{
		strCondition << "Own's Item Condition" << "\r\n";

		HAVEITEM_ITER iter = m_vecHaveItem.begin();
		HAVEITEM_ITER iter_end = m_vecHaveItem.end();
		for ( ; iter!=iter_end; ++iter )
		{	
			SITEM_NUM sItemNum = (*iter);

			SNATIVEID sID = sItemNum.sNativeID;

			CString strNAME = "NO ITEM";
			const SITEM *pItem = GLogicData::GetInstance().GetItem ( sID.wMainID, sID.wSubID );
			if ( pItem )	strNAME = pItem->GetName();

			strCondition << "[" << sID.wMainID << "/" << sID.wSubID << "] " << strNAME.GetString() 
				<< " " << sItemNum.dwNum << "EA" << ", ";
  		}

		strCondition << "\r\n";
	}

	if ( !m_vecLearnSkill.empty() )
	{
		strCondition << "Learn Skill Condition" << "\r\n";

		SNpcTalkCondition::LEARNSKILL_ITER iter = m_vecLearnSkill.begin();
		SNpcTalkCondition::LEARNSKILL_ITER iter_end = m_vecLearnSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{		
			SSkillCondition sSkillCondition = (*iter);
			SNATIVEID sID;
			sID.dwID = sSkillCondition.dwNID;

			CString strNAME = "NO SKILL";
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID.wMainID, sID.wSubID );
			if ( pSkill )	strNAME = pSkill->GetName();
			
			strCondition << "[" << sID.wMainID << "/" << sID.wSubID << "] "
				<< strNAME.GetString() << " Lev." << sSkillCondition.wLevel << "\r\n";
		}

		strCondition << "\r\n";
	}

    // 필요한 스킬 버프
    if ( !m_vecSkillFact.empty() )
    {
        strCondition << "SkillFact Condition" << "\r\n";

        SNpcTalkCondition::SKILLFACT_ITER iter = m_vecSkillFact.begin();
        SNpcTalkCondition::SKILLFACT_ITER iter_end = m_vecSkillFact.end();
        for ( ; iter!=iter_end; ++iter )
        {		
            SSkillCondition sSkillCondition = (*iter);
            SNATIVEID sID;
            sID.dwID = sSkillCondition.dwNID;

            CString strNAME = "NO SKILL";
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID.wMainID, sID.wSubID );
            if ( pSkill )	strNAME = pSkill->GetName();

            strCondition << "[" << sID.wMainID << "/" << sID.wSubID << "] "
                << strNAME.GetString() << " Lev." << sSkillCondition.wLevel << "\r\n";
        }

        strCondition << "\r\n";
    }

	//	이미 수행한 퀘스트
	if ( !m_vecCompleteQuest.empty() )
	{
		strCondition << "Completion Quest" << "\r\n";

		PROCQUEST_ITER iter = m_vecCompleteQuest.begin();
		PROCQUEST_ITER iter_end = m_vecCompleteQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwNID = (*iter);

			CString strNAME = "OFF SKILL";
			GLQUEST *pQuest = GLQuestMan::GetInstance().Find ( dwNID );
			if ( pQuest )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strKey;
					strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					strNAME = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
				}
				else
				{
					strNAME = pQuest->GetTITLE();
				}
			}

			strCondition << "[" << dwNID << "]"
				<< strNAME.GetString() << "\r\n";
		}

		strCondition << "\r\n";
	}

	//	이 퀘스트를 받기 위해서는 습득하지 않아야할 스킬.
	if ( !m_vecDisSkill.empty() )
	{
		strCondition << "Don't Gain Skill :" << "\r\n";

		PROCSKILL_ITER iter = m_vecDisSkill.begin();
		PROCSKILL_ITER iter_end = m_vecDisSkill.end();
		for ( ; iter!=iter_end; ++iter )
		{
			SNATIVEID sID = SNATIVEID(*iter);

			CString strNAME = "OFF SKILL";
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID.wMainID, sID.wSubID );
			if ( pSkill )	strNAME = pSkill->GetName();
			
			strCondition << "[" << pSkill->m_sBASIC.sNATIVEID.wMainID << "/" << pSkill->m_sBASIC.sNATIVEID.wSubID << "] "
				<< strNAME.GetString() << "\r\n";
		}

		strCondition << "\r\n";
	}

	//	이 퀘스트를 받기 위해서는 수행하지 않아야할 퀘스트.
	if ( !m_vecDisQuest.empty() )
	{
		strCondition << "Don't Progress Quest" << "\r\n";

		PROCQUEST_ITER iter = m_vecDisQuest.begin();
		PROCQUEST_ITER iter_end = m_vecDisQuest.end();
		for ( ; iter!=iter_end; ++iter )
		{
			DWORD dwNID = (*iter);

			CString strNAME = "OFF SKILL";
			GLQUEST *pQuest = GLQuestMan::GetInstance().Find ( dwNID );
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				strNAME = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
			}
			else
			{
				strNAME = pQuest->GetTITLE();
			}

			strCondition << "[" << dwNID << "]"
				<< strNAME.GetString() << "\r\n";
		}

		strCondition << "\r\n";
	}

	//	필요 소지 금액.
	if ( m_dwMONEY>0 )
		strCondition << "Request Own's Money : " << m_dwMONEY << "\r\n";
	
	if ( m_dwPARTYMEN_NUM>0 )
		strCondition << "Request Party Members : " << m_dwPARTYMEN_NUM << "\r\n";

	strCondition << std::ends;

	strName = strCondition.str();
	strCondition.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

void SNpcTalkCondition::AddCompleteQuest(SNATIVEID QuestID)
{
    m_vecCompleteQuest.push_back(QuestID.Id());
}

void SNpcTalkCondition::AddHaveItem(SNATIVEID ItemID, int ItemNum)
{
    SITEM_NUM HaveItem(ItemID, ItemNum);
    m_vecHaveItem.push_back(HaveItem);
}

void SNpcTalkCondition::AddLearnSkill(SNATIVEID SkillID, int SkillLevel)
{
    SSkillCondition LearnSkill(SkillID, SkillLevel);
    m_vecLearnSkill.push_back(LearnSkill);
}

void SNpcTalkCondition::AddDisQuest(SNATIVEID QuestID)
{
    m_vecDisQuest.push_back(QuestID.Id());
}

void SNpcTalkCondition::AddDisSkill(SNATIVEID SkillID)
{
    m_vecDisSkill.push_back(SkillID.Id());
}

void SNpcTalkCondition::AddSkillFact(SNATIVEID SkillID, int SkillLevel)
{
    SSkillCondition SkillFact(SkillID, SkillLevel);
    m_vecSkillFact.push_back(SkillFact);
}

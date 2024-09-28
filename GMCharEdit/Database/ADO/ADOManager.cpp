//
//mjeon.ado
// 2010.12.29
//

#include "stdAfx.h"
#include "../DBBase.h"
#include "ADOManager.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../RanLogic/Character/CharacterSlot/GLCharSlotMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//mjeon
// ADOManager will be updated for having routines of managing all CjADO objects
//  with such like a object pool.
//

CADOManager* CADOManager::SelfInstance = NULL;

CADOManager::CADOManager(void)
    : IDBManager(gmce::ADO)
    , m_bSetConnStrings(FALSE)
    //,m_nDBType(gmce::ADO)
    , m_adoUser()
    , m_adoGame()
    , m_adoLog()
{
	m_csUserDBConnString.Empty();
	m_csGameDBConnString.Empty();
	m_csLogDBConnString.Empty();
}

CADOManager::~CADOManager(void)
{
}


CADOManager* CADOManager::GetInstance()
{
	static sc::DefaultLock instanceLock;

	if (SelfInstance == NULL)
	{
		instanceLock.lockOn();

		if (SelfInstance == NULL)
		{
			SelfInstance = new CADOManager();

			//
			// in case of Multi-threading, CoInitialize&CoUninitialize MUST be called per a thread.
			//
			if( FAILED( ::CoInitialize(NULL) ) )
			{	
				//PRINT(_T("::CoInitialized() Failure."));
			}
		}

		instanceLock.lockOff();		
	}
	return SelfInstance;	
}

void CADOManager::ReleaseInstance()
{
	static sc::DefaultLock instanceLock;
	//
	// in case of Multi-threading, CoInitialize&CoUninitialize MUST be called per a thread.
	//
	::CoUninitialize();
	
	instanceLock.lockOn();

	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}

	instanceLock.lockOff();
}

void CADOManager::SetConnStrings(CString &csUserDB, CString &csGameDB, CString &csLogDB)
{
	lockOn();

	m_csUserDBConnString = csUserDB;
	m_csGameDBConnString = csGameDB;
	m_csLogDBConnString  = csLogDB;

	m_bSetConnStrings = TRUE;

	EnableConnectionPooling();

	lockOff();
}


BOOL CADOManager::GetConnString(gmce::ENUM_ADO_INDEX iDB, CString &csConnString)
{
	if (!m_bSetConnStrings)
		return FALSE;

	switch( iDB )
	{
	case gmce::ADO_IDX_USERDB:
		csConnString = m_csUserDBConnString;
		break;
		
	case gmce::ADO_IDX_GAMEDB:
		csConnString = m_csGameDBConnString;
		break;

	case gmce::ADO_IDX_LOGDB:
		csConnString = m_csLogDBConnString;
		break;
	}

	return TRUE;
}

BOOL CADOManager::EnableConnectionPooling()
{
	if ( !m_bSetConnStrings )
	{
		return FALSE;
	}

	//
	//ado objects will be implicitly closed and destroyed when CADOManger is destructed.
	//
	BOOL bOpenedUserDB = m_adoUser.Open(m_csUserDBConnString);
	BOOL bOpenedGameDB = m_adoGame.Open(m_csGameDBConnString);
	BOOL bOpenedLogDB  = m_adoLog.Open(m_csLogDBConnString);

	return (bOpenedUserDB && bOpenedGameDB && bOpenedLogDB);
}

int CADOManager::ExecuteStoredProcedure(const std::string& SpName, CjADO& Ado)
{
	CString SpNameTemp;
	SpNameTemp.Format( "%s", SpName.c_str() );

	if (!Ado.Execute4Cmd(SpNameTemp, adCmdStoredProc))
	{
		sc::writeLogError(
			sc::string::format(
			"CAdoManager::ExecuteStoredProcedure %1%", SpName));
		return DB_ERROR;
	}
	else
		return DB_OK;
}

int CADOManager::ExecuteStoredProcedureIntReturn(const std::string& SpName, CjADO& Ado, const std::string& OutPutParamName)
{
    CString SpNameTemp;
    SpNameTemp.Format( "%s", SpName.c_str() );

    if (!Ado.Execute4Cmd(SpNameTemp, adCmdStoredProc))
    {
        sc::writeLogError(
            sc::string::format(
            "CADOManager::ExecuteStoredProcedureIntReturn %1%", SpName.c_str()));
        return DB_ERROR;
    }

    _variant_t varReturn;
    if (!Ado.GetParam(OutPutParamName.c_str(), varReturn))
    {
        sc::writeLogError(
            sc::string::format(
            "CADOManager::ExecuteStoredProcedureIntReturn %1% GetParam", SpName.c_str()));
        return DB_ERROR;
    }
    return varReturn.intVal;
}

void CADOManager::SetUpGmInfo(UINT nUserNum, CString &csUserID, CString &csUserIP)
{
	m_nGmUserNum= nUserNum;
	m_strUserID = csUserID;
	m_strUserIP = csUserIP;
}

int CADOManager::GetCharacterInfoBase(int nUserNumber, int nChaNum, SCHARDATA2* pChaData2)
{    
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum",  nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@UserNum", nUserNumber);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoSelectBase"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    int RowCount = 0;
    do
    {
        int nUserNum = 0; int nSGNum = 0; int nChaTribe = 0; int nChaClass = 0;
        CString ChaName;

        AdoGame.GetCollect("UserNum",  nUserNum);
        AdoGame.GetCollect("SGNum",    nSGNum);
        AdoGame.GetCollect("ChaName",  ChaName);
        AdoGame.GetCollect("ChaTribe", nChaTribe);
        AdoGame.GetCollect("ChaClass", nChaClass);

        int nChaBright = 0; int nChaLevel = 0; int nChaDex = 0; int nChaIntel = 0; int nChaPower = 0;

        AdoGame.GetCollect("ChaBright", nChaBright);
        AdoGame.GetCollect("ChaLevel",  nChaLevel);
        AdoGame.GetCollect("ChaDex",    nChaDex);
        AdoGame.GetCollect("ChaIntel",  nChaIntel);
        AdoGame.GetCollect("ChaPower",  nChaPower);

        int nChaStrong = 0; int nChaSpirit = 0; int nChaStrength = 0; int nChaStRemain = 0; int nChaAttackP = 0;

        AdoGame.GetCollect("ChaStrong",   nChaStrong);
        AdoGame.GetCollect("ChaSpirit",   nChaSpirit);
        AdoGame.GetCollect("ChaStrength", nChaStrength);
        AdoGame.GetCollect("ChaStRemain", nChaStRemain);
        AdoGame.GetCollect("ChaAttackP",  nChaAttackP);

        int nChaDefenseP = 0; int nChaFightA = 0; int nChaShootA = 0; int nChaSkillPoint = 0; int nChaHP = 0;

        AdoGame.GetCollect("ChaDefenseP",   nChaDefenseP);
        AdoGame.GetCollect("ChaFightA",     nChaFightA);
        AdoGame.GetCollect("ChaShootA",     nChaShootA);
        AdoGame.GetCollect("ChaSkillPoint", nChaSkillPoint);
        AdoGame.GetCollect("ChaHP",         nChaHP);

        int nChaMP = 0; int nChaSP = 0; int nChaPK = 0; int nChaStartMap = 0; int nChaStartGate = 0;

        AdoGame.GetCollect("ChaMP",        nChaMP);
        AdoGame.GetCollect("ChaSP",        nChaSP);
        AdoGame.GetCollect("ChaPK",        nChaPK);
        AdoGame.GetCollect("ChaStartMap",  nChaStartMap);
        AdoGame.GetCollect("ChaStartGate", nChaStartGate);

        float fChaPosX = 0; float fChaPosY = 0; float fChaPosZ = 0; LONGLONG llChaMoney = 0; LONGLONG llChaExp = 0;

        AdoGame.GetCollect("ChaPosX",  fChaPosX);
        AdoGame.GetCollect("ChaPosY",  fChaPosY);
        AdoGame.GetCollect("ChaPosZ",  fChaPosZ);
        AdoGame.GetCollect("ChaMoney", llChaMoney);
        AdoGame.GetCollect("ChaExp",   llChaExp);

        int nChaSaveMap = 0; float fChaSavePosX = 0; float fChaSavePosY = 0; float fChaSavePosZ = 0; int nChaSchool = 0;

        AdoGame.GetCollect("ChaSaveMap",  nChaSaveMap);
        AdoGame.GetCollect("ChaSavePosX", fChaSavePosX);
        AdoGame.GetCollect("ChaSavePosY", fChaSavePosY);
        AdoGame.GetCollect("ChaSavePosZ", fChaSavePosZ);
        AdoGame.GetCollect("ChaSchool",   nChaSchool);

        int nChaHair = 0; int nChaFace = 0; int nChaLiving = 0; int nChaInvenLine = 0; int nChaReturnMap  = 0;            

        AdoGame.GetCollect("ChaHair", nChaHair);
        AdoGame.GetCollect("ChaFace", nChaFace);
        AdoGame.GetCollect("ChaLiving", nChaLiving);
        AdoGame.GetCollect("ChaInvenLine", nChaInvenLine);
        AdoGame.GetCollect("ChaReturnMap", nChaReturnMap);

        float fChaReturnPosX = 0; float fChaReturnPosY = 0; float fChaReturnPosZ = 0; int nGuNum = 0; CString ChaGuName;

        AdoGame.GetCollect("ChaReturnPosX", fChaReturnPosX);
        AdoGame.GetCollect("ChaReturnPosY", fChaReturnPosY);
        AdoGame.GetCollect("ChaReturnPosZ", fChaReturnPosZ);
        AdoGame.GetCollect("GuNum", nGuNum);
        AdoGame.GetCollect("ChaGuName", ChaGuName);

        WORD nChaSex = 0; int nChaHairColor = 0; LONGLONG llChaReExp = 0;
        WORD nChaSafeTime = 0; int nChaCP = 0; WORD wSlotIndex = 0;

        AdoGame.GetCollect("ChaSex", nChaSex);
        AdoGame.GetCollect("ChaHairColor", nChaHairColor);
        AdoGame.GetCollect("ChaReExp", llChaReExp);
        AdoGame.GetCollect("ChaSafeTime", nChaSafeTime);
        AdoGame.GetCollect("ChaCP", nChaCP);
		AdoGame.GetCollect("SkillSlotNum", wSlotIndex);

        pChaData2->m_CharDbNum = (DWORD) nChaNum; // 캐릭터번호
        pChaData2->SetUserID((DWORD) nUserNum ); // 사용자번호
        pChaData2->m_ClubDbNum = (DWORD) nGuNum; // 클럽번호

        if (!ChaName.IsEmpty())
            pChaData2->SetName(ChaName.GetString());

        if (!ChaGuName.IsEmpty())
            pChaData2->SetNickName(ChaGuName.GetString());

        pChaData2->m_emTribe			= EMTRIBE(nChaTribe);
        pChaData2->m_emClass			= EMCHARCLASS(nChaClass);

        pChaData2->m_wLevel				= (WORD) nChaLevel;           
        pChaData2->m_sStats.wPow		= (WORD) nChaPower;
        pChaData2->m_sStats.wStr		= (WORD) nChaStrong;
        pChaData2->m_sStats.wSta		= (WORD) nChaStrength;
        pChaData2->m_sStats.wSpi		= (WORD) nChaSpirit;

        pChaData2->m_sStats.wDex		= (WORD) nChaDex;
        pChaData2->m_sStats.wInt		= (WORD) nChaIntel;
        pChaData2->m_wStatsPoint		= (WORD) nChaStRemain;
		pChaData2->SetInvenMoney( llChaMoney );
        pChaData2->m_sExperience.lnNow  = llChaExp;

        pChaData2->m_sHP.nNow			= nChaHP;
        pChaData2->m_sMP.nNow			= nChaMP;
		pChaData2->m_sSP.nNow			= nChaSP;

        pChaData2->m_sStartMapID.dwID	= (DWORD) nChaStartMap;
        pChaData2->m_dwStartGate		= (DWORD) nChaStartGate;
        pChaData2->m_vStartPos.x		= (float) fChaPosX;

        pChaData2->m_vStartPos.y		= (float) fChaPosY;
        pChaData2->m_vStartPos.z		= (float) fChaPosZ;
		pChaData2->m_nBright				= nChaBright;

        pChaData2->m_powerAttack		= nChaAttackP;
        pChaData2->m_powerDefence	= nChaDefenseP;
        pChaData2->m_powerAttribute[SKILL::EMAPPLY_MELEE]		= nChaFightA;
        pChaData2->m_powerAttribute[SKILL::EMAPPLY_RANGE]		= nChaShootA;
//		pChaData2->m_powerAttribute[SKILL::EMAPPLY_MAGIC]		= nChaMagicA;
        pChaData2->m_dwSkillPoint	= (DWORD) nChaSkillPoint;        
        pChaData2->m_wPK				= (WORD) nChaPK;

        pChaData2->m_sSaveMapID.dwID	= (DWORD) nChaSaveMap;
        pChaData2->m_vSavePos.x			= (float) fChaSavePosX; 
        pChaData2->m_vSavePos.y			= (float) fChaSavePosY; 
        pChaData2->m_vSavePos.z			= (float) fChaSavePosZ;

        pChaData2->m_sLastCallMapID.dwID= (DWORD) nChaReturnMap;
        pChaData2->m_vLastCallPos.x		= (float) fChaReturnPosX; 
        pChaData2->m_vLastCallPos.y		= (float) fChaReturnPosY; 
        pChaData2->m_vLastCallPos.z		= (float) fChaReturnPosZ;

        pChaData2->m_wSchool			= (WORD) nChaSchool;

        pChaData2->m_wFace				= (WORD) nChaFace;
        pChaData2->m_wHair				= (WORD) nChaHair;
        pChaData2->m_nLiving			= (int) nChaLiving;
        pChaData2->m_wHairColor         = (WORD) nChaHairColor;
        pChaData2->m_wSex               = (WORD) nChaSex;

        // 인벤토리 라인
        pChaData2->m_wINVENLINE = (WORD) nChaInvenLine; 

        pChaData2->m_lnRestorableExp = llChaReExp;
        pChaData2->m_fCDMSafeTime = (float) nChaSafeTime;

        pChaData2->m_sCP.wNow = (WORD) nChaCP;

		pChaData2->m_wSlotIndex = wSlotIndex;

        ++RowCount;
    } while (AdoGame.Next());

    if (RowCount == 1)
    {
        return DB_OK;
    }
    else
    {
        sc::writeLogError(
            sc::string::format("AdoManager::GetCharacterInfoBase RowCount %1%", RowCount));
        return DB_ERROR;
    }
}

int CADOManager::GetCharacterInfoStats ( int nChaNum, SCHARDATA2* pChaData2 )
{
	if ( NULL == pChaData2 )
		return DB_ERROR;

	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@ChaNum", nChaNum );

	if ( FALSE == AdoGame.Execute4Cmd( CString( "dbo.ChaInfoChaStatsSelect" ), adCmdStoredProc ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	int nChaDex = 0;
	int nChaInt = 0;
	int nChaPow = 0;
	int nChaStr = 0;
	int nChaSpirit = 0;
	int nChaStrength = 0;
	int nChaStatPoint = 0;
	int nChaSkillPoint = 0;

	AdoGame.GetCollect( "ChaDex", nChaDex );
	AdoGame.GetCollect( "ChaIntel", nChaInt );
	AdoGame.GetCollect( "ChaPower", nChaPow );
	AdoGame.GetCollect( "ChaStrong", nChaStr );
	AdoGame.GetCollect( "ChaSpirit", nChaSpirit );
	AdoGame.GetCollect( "ChaStrength", nChaStrength );
	AdoGame.GetCollect( "ChaStRemain", nChaStatPoint );
	AdoGame.GetCollect( "ChaSkillPoint", nChaSkillPoint );

	pChaData2->m_sStats.wPow = static_cast< WORD >( nChaPow );
	pChaData2->m_sStats.wStr = static_cast< WORD >( nChaStr );
	pChaData2->m_sStats.wSta = static_cast< WORD >( nChaStrength );
	pChaData2->m_sStats.wSpi = static_cast< WORD >( nChaSpirit );
	pChaData2->m_sStats.wDex = static_cast< WORD >( nChaDex );
	pChaData2->m_sStats.wInt = static_cast< WORD >( nChaInt );
	pChaData2->m_wStatsPoint = static_cast< WORD >( nChaStatPoint );
	pChaData2->m_dwSkillPoint = static_cast< DWORD >( nChaSkillPoint );

	return DB_OK;
}

int CADOManager::GetCharacterInfoSkill(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaSkillsSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("ChaSkills", vBuffer);

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETEXPSKILLS_BYBUF(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoSkillSlot(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaSkillQSlotSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("ChaSkillSlot", vBuffer);    

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETSKILL_QUICKSLOT(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoActionSlot(int nChaNum, SCHARDATA2* pChaData2)
{    
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaActionSlotSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("ChaActionSlot", vBuffer);

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETACTION_QUICKSLOT(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoQuest(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaQuestSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    {
        se::ByteStream ByteStream;
		AdoGame.GetChunk("ChaQuest", ByteStream);
        pChaData2->SETQUESTPLAY(ByteStream, true);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoPutOnItems( int nChaNum, SCHARDATA2* pChaData2 )
{
	if ( !pChaData2 )
		return DB_ERROR;

	std::vector< SINVENITEM_SAVE > vecItems;
	if ( DB_ERROR == ItemSelect( nChaNum, INVEN_PUTON, vecItems ) )
	{
		return DB_ERROR;
	}

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];
		pChaData2->SETPUTONITEM( static_cast< EMSLOT >( sItem.wPosX ), sItem.sItemCustom );
	}

	return DB_OK;

}

int CADOManager::GetCharacterInfoInven( int nChaNum, SCHARDATA2* pChaData2 )
{
	if ( !pChaData2 )
		return DB_ERROR;

	std::vector< SINVENITEM_SAVE > vecItems;
	if ( DB_ERROR == ItemSelect( nChaNum, INVEN_INVEN, vecItems ) )
	{
		return DB_ERROR;
	}

	pChaData2->SETINVENTORY_BYVECTOR( vecItems );

	return DB_OK;
}

int CADOManager::GetCharacterInfoVTAddInven(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoVTAddInvenSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("VTAddInven", vBuffer);

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETVTADDINVENTORY_BYBUF(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoCoolTime(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaCoolTimeSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("ChaCoolTime", vBuffer);

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETITEMCOOLTIME_BYBUF(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoSkillFact(int nChaNum, SCHARDATA2* pChaData2)
{
    CjADO AdoGame(m_csGameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@ChaNum", nChaNum);

    if (!AdoGame.Execute4Cmd(CString("dbo.ChaInfoChaSkillFactSelect"), adCmdStoredProc) )
        return DB_ERROR;

    if (AdoGame.GetEOF())
        return DB_ERROR;

    se::ByteStream vBuffer;
    AdoGame.GetChunk("ChaSkillFact", vBuffer);

    
    {
        se::ByteStream ByteStream(vBuffer);
        pChaData2->SETSKILLFACT_BYBUF(ByteStream);
    }

    return DB_OK;
}

int CADOManager::GetCharacterInfoSlots ( int nChaNum, SCHARDATA2* pChaData2 )
{
	if ( NULL == pChaData2 )
		return DB_ERROR;

	// 설정에 따른 슬롯 상태를 먼저 세팅한다;
	for ( UINT i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		// 개방 슬롯이 아닐경우 락을 건다;
		if ( GLCharSlotMan::Instance()->GetOpenSlotCount() <= i )
		{
			pChaData2->m_arrLockSlotState[ i ] = true;
		}
		// 기본 활성화 슬롯일 경우 락을 해제한다;
		else if ( i < GLCharSlotMan::Instance()->GetDefaultActiveSlotCount() )
		{
			pChaData2->m_arrLockSlotState[ i ] = false;
		}
	}

	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@ChaNum", nChaNum );

	if ( FALSE == AdoGame.Execute4Cmd( CString( "dbo.ChaInfoChaSlotsSelect" ), adCmdStoredProc ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	do
	{
		int nChaDex = 0;
		int nChaInt = 0;
		int nChaPow = 0;
		int nChaStr = 0;
		int nChaSpirit = 0;
		int nChaStrength = 0;
		int nChaStatPoint = 0;
		int nChaSkillPoint = 0;
		int nLockSlot = 0;
		int nSkillSlotNum = 0;

		AdoGame.GetCollect( "ChaDex", nChaDex );
		AdoGame.GetCollect( "ChaIntel", nChaInt );
		AdoGame.GetCollect( "ChaPower", nChaPow );
		AdoGame.GetCollect( "ChaStrong", nChaStr );
		AdoGame.GetCollect( "ChaSpirit", nChaSpirit );
		AdoGame.GetCollect( "ChaStrength", nChaStrength );
		AdoGame.GetCollect( "ChaStRemain", nChaStatPoint );
		AdoGame.GetCollect( "ChaSkillPoint", nChaSkillPoint );
		AdoGame.GetCollect( "bLockSlot", nLockSlot );
		AdoGame.GetCollect( "SkillSlotNum", nSkillSlotNum );

		if ( nSkillSlotNum < 0 || EMCHAR_SLOT_DATA_SIZE <= nSkillSlotNum )
		{
			continue;
		}

		SCHAR_SLOT_DATA sCharSlotData;

		sCharSlotData.m_sStats.wPow = static_cast< WORD >( nChaPow );
		sCharSlotData.m_sStats.wStr = static_cast< WORD >( nChaStr );
		sCharSlotData.m_sStats.wSta = static_cast< WORD >( nChaStrength );
		sCharSlotData.m_sStats.wSpi = static_cast< WORD >( nChaSpirit );
		sCharSlotData.m_sStats.wDex = static_cast< WORD >( nChaDex );
		sCharSlotData.m_sStats.wInt = static_cast< WORD >( nChaInt );
		sCharSlotData.m_wStatsPoint = static_cast< WORD >( nChaStatPoint );
		sCharSlotData.m_dwSkillPoint = static_cast< DWORD >( nChaSkillPoint );

		// 개방 슬롯이 아닐경우 락을 건다;
		if ( GLCharSlotMan::Instance()->GetOpenSlotCount() <= nSkillSlotNum )
		{
			pChaData2->m_arrLockSlotState[ nSkillSlotNum ] = true;
		}
		// 기본 활성화 슬롯일 경우 락을 해제한다;
		else if ( nSkillSlotNum < GLCharSlotMan::Instance()->GetDefaultActiveSlotCount() )
		{
			pChaData2->m_arrLockSlotState[ nSkillSlotNum ] = false;
		}
		// 일반 슬롯일 경우 DB 에 저장된 정보를 따른다;
		else
		{
			pChaData2->m_arrLockSlotState[ nSkillSlotNum ] = ( nLockSlot >= 1 ) ? true : false;
		}

		{
			se::ByteStream vBuffer;
			AdoGame.GetChunk( "ChaSkills", vBuffer );

			{
				se::ByteStream ByteStream( vBuffer );
				sCharSlotData.SETEXPSKILLS_BYBUF(ByteStream);
			}
		}

		{
			se::ByteStream vBuffer;
			AdoGame.GetChunk( "ChaSkillSlot", vBuffer );

			{
				se::ByteStream ByteStream( vBuffer );
				sCharSlotData.SETSKILL_QUICKSLOT(ByteStream);
			}
		}

		if ( false == GLCharSlotMan::Instance()->SaveCharSlot( nChaNum, nSkillSlotNum, sCharSlotData ) )
		{
			sc::writeLogError( sc::string::format(
				"[ Character Slot ] [ Failed Save Character Slot, Character : %1%, Slot : %2% ]",
				nChaNum,
				nSkillSlotNum ) );
		}

	} while ( AdoGame.Next() );

	return DB_OK;
}

int CADOManager::GetAllPetPutonItems( DWORD ChaDbNum, std::vector< GLPET >& vecPets )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@PetChaNum", ChaDbNum );

	if ( DB_OK != ExecuteStoredProcedure( "dbo.sp_GetAllPetPutOnItems", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_OK;

	do
	{
		GLPET sPet;
		AdoGame.GetCollect( "PetNum", sPet.m_DbNum );

		int nMigrationState = 1;
		AdoGame.GetCollect( "MigrationState", nMigrationState );

		se::ByteStream PutonItems;
		AdoGame.GetChunk( CString( "PetPutOnItems" ), PutonItems );
		if ( !PutonItems.IsEmpty() )
		{
			if ( 0 == nMigrationState )
			{
				SETPUTONITEMS_BYBUF_FOR_MIGRATION( sPet.m_PutOnItems, PutonItems );
			}
			else
			{
				SETPUTONITEMS_BYBUF( sPet.m_PutOnItems, PutonItems );
			}
		}

		vecPets.push_back( sPet );

	} while (AdoGame.Next());

	return DB_OK;
}

int CADOManager::GetAllVehiclePutonItems( DWORD ChaDbNum, std::vector< GLVEHICLE >& vecVehicles )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@VehicleChaNum", ChaDbNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.sp_GetAllVehiclePutOnItems", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_OK;

	do
	{
		GLVEHICLE sVehicle;
		AdoGame.GetCollect( "VehicleNum", sVehicle.m_DbNum );

		int nMigrationState = 1;
		AdoGame.GetCollect( "MigrationState", nMigrationState );

		// Put on items
		se::ByteStream VehicleItems;
		AdoGame.GetChunk( CString( "VehiclePutOnItems" ), VehicleItems );
		if ( !VehicleItems.IsEmpty() )
		{
			if ( 0 == nMigrationState )
			{
				SETPUTONITEMS_BYBUF_FOR_MIGRATION( sVehicle.m_PutOnItems, VehicleItems );
			}
			else
			{
				SETPUTONITEMS_BYBUF( sVehicle.m_PutOnItems, VehicleItems );
			}
		}

		vecVehicles.push_back( sVehicle );

	} while (AdoGame.Next());

	return DB_OK;
}

int CADOManager::GetCharacterBinaryPutOnItems( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@ChaNum", ChaNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.ChaInfoChaPutOnItemsSelect", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	se::ByteStream ByteStream;
	AdoGame.GetChunk( CString( "ChaPutOnItems" ), ByteStream );
	if ( !ByteStream.IsEmpty() )
	{
		if ( MigrationTarget )
		{
			SETPUTONITEMS_BYBUF_FOR_MIGRATION( pChaData2->m_PutOnItems, ByteStream );
		}
		else
		{
			SETPUTONITEMS_BYBUF( pChaData2->m_PutOnItems, ByteStream );
		}
	}

	return DB_OK;
}

int CADOManager::GetCharacterBinaryInvenItems( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@ChaNum", ChaNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.ChaInfoChaInvenSelect", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	se::ByteStream ByteStream;
	AdoGame.GetChunk( CString( "ChaInven" ), ByteStream );
	if ( !ByteStream.IsEmpty() )
	{
		if ( MigrationTarget )
		{
			pChaData2->SETINVENTORY_BYBUF_FOR_MIGRATION( ByteStream );
		} 
		else
		{
			pChaData2->SETINVENTORY_BYBUF( ByteStream );
		}
	}

	return DB_OK;
}

int CADOManager::GetCharacterBinaryQuest( int ChaNum, SCHARDATA2* pChaData2, int MigrationTarget )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@ChaNum", ChaNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.ChaInfoChaQuestSelect", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	se::ByteStream ByteStream;
	AdoGame.GetChunk( CString( "ChaQuest" ), ByteStream );
	if ( !ByteStream.IsEmpty() )
	{
		if ( MigrationTarget )
		{
			pChaData2->SETQUESTPLAY_FOR_MIGRATION( ByteStream, true );
		} 
		else
		{
			pChaData2->SETQUESTPLAY( ByteStream, true );
		}
	}

	return DB_OK;
}

int CADOManager::GetUserBinaryInvenItems( int UserNum, SCHARDATA2* pChaData2 )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@UserNum", UserNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.UserInvenInvenSelectForItemTransfer", AdoGame ) )
		return DB_ERROR;

	if ( AdoGame.GetEOF() )
		return DB_ERROR;

	se::ByteStream ByteStream;
	AdoGame.GetChunk( CString( "UserInven" ), ByteStream );
	if ( !ByteStream.IsEmpty() )
	{
		pChaData2->SETSTORAGE_BYBUF( ByteStream );
	}

	return DB_OK;
}

int CADOManager::SetPetMigrationState( int ChaNum, int PetNum, int MigrationState )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@PetChaNum", ChaNum );
	APPEND_IPARAM_INT( AdoGame, "@PetNum", PetNum );
	APPEND_IPARAM_INT( AdoGame, "@MigrationState", MigrationState );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.UpdatePetMigrationState", AdoGame ) )
	{
		return DB_ERROR;
	}

	return DB_OK;	
}

int CADOManager::SetVehicleMigrationState( int ChaNum, int VehicleNum, int MigrationState )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@VehicleChaNum", ChaNum );
	APPEND_IPARAM_INT( AdoGame, "@VehicleNum", VehicleNum );
	APPEND_IPARAM_INT( AdoGame, "@MigrationState", MigrationState );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.UpdateVehicleMigrationState", AdoGame ) )
	{
		return DB_ERROR;
	}

	return DB_OK;	
}

int CADOManager::SetMigrationTarget( int ChaNum )
{
	CjADO AdoGame( m_csGameDBConnString );
	APPEND_IPARAM_INT( AdoGame, "@nChaNum", ChaNum );

	if ( DB_ERROR == ExecuteStoredProcedure( "dbo.sp_SetMigrationTarget", AdoGame ) )
	{
		return DB_ERROR;
	}

	return DB_OK;	
}
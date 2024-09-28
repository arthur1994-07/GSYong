#include "../pch.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/Item/GLItemDef.h"
#include "../../RanLogic/GLChatLinkDef.h"
#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Character/CharacterSlot/GLCharSlotMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ReferCharMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/CharStateMsg.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/GLContrlCharJoinMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/Skill/SpecialEffect/GLSpecExperiencePoint.h"
#include "../../RanLogic/Skill/GLCountAttackSkill.h"
#include "../../RanLogic/Skill/GLSkillChangeList.h"
#include "../../RanLogic/transformset/GLTransformSet.h"

#include "../TriggerSystem/ServerDamageMessage.h"
#include "../TriggerSystem/ServerHealMessage.h"
#include "../TriggerSystem/ServerSavePosMessage.h"
#include "../TriggerSystem/ServerActionMessage.h"
#include "../Club/GLClubField.h"
#include "../Club/GLClubManField.h"
#include "../Database/DbManager.h"
#include "../Database/DbAction/DbActionGameVehicle.h"
#include "../Database/DbAction/DbActionGamePet.h"
#include "../Database/DbAction/DbActionGameItem.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Guidance/GLGuidance.h"
#include "../Party/GLPartyManagerField.h"
#include "../Party/GLPartyField.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../PK/GLSchoolFreePK.h"
#include "../Util/GLItemLimit.h"
#include "../Vehicle/GLVehicleField.h"
#include "../Net/NetCachePacket.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../Event/EventField/EventField.h"
#include "../AI/GLFSMContainer.h"
#include "../Move/MoveManagerField.h"

#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define BOX_R	(4)


const float GLChar::fLIMIT_TIME_ITEM_SORT = 1.0f;
const float GLChar::fLIMIT_TIME_SUMMON_PET = 1.0f;


GLChar::GLChar(DWORD MaxClient, GLGaeaServer* pGaeaServer, wb::EM_COUNTRY Country)
    : ServerActor(pGaeaServer, CROW_PC)    
    , m_wTARNUM(0)
    , m_dwLastPostSend(0) //mjeon.post.send
    , m_pActivityMan(new ActivityManager(pGaeaServer, *(ActivityBaseData::GetInstance()), m_CharDbNum)) // mjeon.activity
    , m_pTitleMan(new TitleManager())
	, m_pAttendanceMan(new AttendanceManager(pGaeaServer, m_CharDbNum)) // mjeon.attendance
    , m_dwANISUBSELECT(0)
    , m_fIdleTime(0.0f)
    , m_fMBR_TIMER(0.0f)
    , m_fSTATE_TIMER(0.0f)
    , m_fPERIOD_TIMER(0.0f)
    , m_fQUEST_TIMER(60.0f)
    , m_fITEM_TIMER(0.0f)
    , m_fSAVEDB_TIMER(0.0f)
    , m_fPingTraceTimer(0.0f)
    , m_fattTIMER(0.0f)
    , m_fMoveDelay(0.0f)
    , m_fBRIGHT_TIMER(0.0f)
    , m_sAssault(CROW_PC,GAEAID_NULL)
    , m_lnLastSendExp(0)
    , m_wLastSendCP(0)
    , m_lnLastSendVNExp(0)
    , m_dwViewFrame(0)
    , m_fAge(0.0f)
    , m_sResurrectAbleState(EM_RESURRECTION_TYPE_NOT_SET)
    , m_lnDeathPenaltyEXP(0)
    , m_bSetDeathPenalty(false)
    , m_fForcedResurrectTime(0.0f)
    , m_sForcedResurrect_MapID(SNATIVEID(false))
    , m_dwForcedResurrect_GateID(0)
	, m_vForcedResurrect_position(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
    , m_bForcedResurrect_Instance(false)
    , m_fForcedResurrect_Restore(100.0f)
    , m_fSkillResurrect_Restore(100.0f)
    , m_fGenAge(0.0f)
    , m_vBackupPosition(0.0f, 0.0f, 0.0f)
    , m_ClientSlot(0)    
    , m_wHealingNum(0)
    , m_pCharNodeInGaea(NULL)
    , m_pCharNodeInLand(NULL)
    , m_pQuadNode(NULL)
    , m_pCellNode(NULL)
    , m_SKILLMTYPE( AN_SKILL )
    , m_SKILLSTYPE( AN_SUB_00 )
    , m_bUsedStorageCard(false)
    , m_fSkillDelay(0.0f)
    , m_PositionSendTimer(0.0f)
    , m_dwPetGUID(UINT_MAX)
    , m_PetDbNum(0)
    , m_bEntryFailed(FALSE)
    , m_bAcquireEXP(FALSE)
    , m_bGetPetFromDB(false)
    , m_bGetVehicleFromDB(false)
    , m_fVehicleTimer(0.0f)
    , m_ItemRemodelPosX(EInvenX)
    , m_ItemRemodelPosY(EInvenTotalLine)
    , m_ChinaGainType(255)
    , m_dwVietnamGainType(GAINTYPE_MAX)
    , m_bEventStart(FALSE)
    , m_bEventApply(FALSE)
    , m_bItemShopOpen(false)
    , m_bDefenseSkill(false)
    , m_fLimitTimeItemSort(0.0f)
    , m_fLimitTimeSummonPet(0.0f)
    , m_CurrentTime(0)
    , m_eMotionMID(AN_GUARD_N)
    , m_eMotionSID(AN_SUB_NONE)
    , m_fMotionSec(0.0f)
    , m_fnOnMotionFinish(NULL)
    , m_bReferCharPublic(false)
    , m_fReferCharDelay(0.0f)
    , m_Country(Country)
    , m_pVehicle(NULL)
    , m_cMovement(CHAR_STATE_DEFAULT)
	, m_dwLastPremiumExitNotifyTime( 0 )
	, m_fPremiumExitCheckTimer( 0.0f )
	, m_uintViewAround(0)
	, m_fViewAroundSettingReleaseTimer(0)
	, m_bFlyCameraUse(false)
	, m_vFlyCameraPos(0.0f, 0.0f, 0.0f)
	, m_bUsedPMarketSeacrhCard( FALSE )
	, m_fCoolTime_ChangeCharSlot ( 0.f )
	, m_bRanMobileRecognized( false )
	, m_tRanMobileRewardDate( 0 )
	, m_nRanMobilePoint( 0 )
{
    m_pItemLimit = pGaeaServer->GetItemLimit();
    m_pVehicle = new GLVEHICLE;
	m_sSaveMapID = NATIVEID_NULL();
	m_ARoundSlot.SetChar(this);

	m_China15Time = CTime::GetCurrentTime();	

	GASSERT(MaxClient);
	//m_arrayFieldPC = new PSFIELDCROW[MaxClient];

	//memset ( m_arrayFieldPC, 0, sizeof(PSFIELDCROW)*MaxClient );
	//memset ( m_arrayFieldCROW, 0, sizeof(PSFIELDCROW)*MAXCROW );
	//memset ( m_arrayFieldMATERIAL, 0, sizeof(PSFIELDCROW)*MAXCROW );
	//memset ( m_arrayFieldITEM, 0, sizeof(PSFIELDCROW)*MAXITEM );
	//memset ( m_arrayFieldMONEY, 0, sizeof(PSFIELDCROW)*MAXMONEY );
	//memset ( m_arrayFieldSummon, 0, sizeof(PSFIELDCROW)*MAXSUMMON );
	//memset ( m_arrayFieldLandEvent, 0, sizeof(PSFIELDLANDEVENT)*MAXLANDEVENT );

	m_vecLandEffect.clear();

	m_vecMapEffect.clear();

	// PET
	//m_arrayFieldPET = new PSFIELDCROW[MaxClient];
	//memset ( m_arrayFieldPET, 0, sizeof(PSFIELDCROW)*MaxClient );

	ZeroMemory(&m_AUTHFB, sizeof(m_AUTHFB));
	ZeroMemory(&m_AUTHTW, sizeof(m_AUTHTW));


	m_ServiceProvider = GetServiceProvider();

    m_pPrivateMarket = new PrivateMarketField(this, m_pGaeaServer);

	m_pProductField	= new GLProductField( m_pGaeaServer );

	//m_cInventory.SetActivityCallback(&GLChar::ActivityCheckItemTake);

	// 최초에는 예약액션이 존재하지 않는다;
	for ( int i=0; i<EMRESERVED_SIZE; ++i )
		m_bReservedActionFlags[ i ] = false;
}

GLChar::~GLChar ()
{
    m_LandEventMap.clear();
    m_ViewCrow.clear();
    SAFE_DELETE(m_pPrivateMarket);
    SAFE_DELETE(m_pVehicle);
	SAFE_DELETE(m_pProductField);
}

void GLChar::LoadExtraInfo()
{
	//기여도 로드
	LoadContributionPoint();

	// Cache 서버로 PointShop point 전송요청
    cp::NET_CHAR_REQ_POINT_FH MsgFH(CharDbNum());
    SendToCache(&MsgFH);
}

void GLChar::SaveExtraInfo()
{
	//기여도 저장
	SaveContributionPoint();
}

void GLChar::LoadContributionPoint()
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH msg;
	msg.dwChaNum = m_CharDbNum;

	SendToCache(&msg);
}

void GLChar::SaveContributionPoint()
{
	GASSERT(m_nRewardContribution >= 0);

	if (m_nRewardContribution < 0)
	{
		//error !!
		sc::writeLogError(sc::string::format("GLChar::SaveContributionPoint() - m_nRewardContribution(%1%) is less than 0.", m_nRewardContribution));
		m_nRewardContribution = 0;
		return;
	}

	if(m_nRewardContribution > 0)
	{
			GLMSG::SNET_SET_CHA_EXTRA_INFO_FH msg;
			msg.dwChaNum			= m_CharDbNum;	
			msg.nContributionPoint	= m_nRewardContribution;		//누적 기여도가 아니라, CTF 보상을 받을 때 마다 획득한 기여도를 DB에 바로 저장하도록 수정.

			//
			//mjeon.capturethefield.tracing
			//
			sc::writeLogInfo(sc::string::format("(TRACE) Update(+) ContributionPoint(%1%) for Char(%2%). TotalContribution(%3%).", m_nRewardContribution, m_CharDbNum, m_nContributionPoint));

			SendToCache(&msg);

			m_nRewardContribution = 0;	//저장 후에는 0으로 리셋
	}
}

EMSERVICE_PROVIDER GLChar::GetServiceProvider() const
{
    return m_pGaeaServer->GetServiceProvider();
}

BOOL GLChar::IsInInstance()
{
	return m_pGaeaServer->IsInstance();
}

BOOL GLChar::ToggleVehicle(bool bActive)
{
    if (!VehicleActiveValue())
    {
        return FALSE;
    }

    // 맵소환가능여부 체크
    const GLLandMan* const pLandMan(GetLandMan());
    if ( pLandMan == NULL )
        return FALSE;

    GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC NetMsgFB;

    if ( IsValidBody() == false )
    {
		const DWORD dwActState(GetActState());
		if ( (pLandMan->IsPeaceZone() == true) || (dwActState & (EM_ACT_WAITING | EM_ACT_DIE)) )
		{
			NetMsgFB.emFB = EMVEHICLE_SET_FB_FAIL;		
			SendToClient( &NetMsgFB );
			return FALSE;
		}	
    }

    //탈것 아이템을 찾을 수가 없을 때;
    const SITEMCUSTOM& sItemCostom = m_PutOnItems[SLOT_VEHICLE];
    if (sItemCostom.GetNativeID() == NATIVEID_NULL())
    {
        NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
        SendToClient( &NetMsgFB );
        return FALSE;
    }

    //탈것을 탈 때;
    if ( bActive )
    {
        //탈것 금지 맵에서는 탈 수 없다.;
        if ( !pLandMan->IsVehicleActivity() )
        {
            NetMsgFB.emFB = EMVEHICLE_SET_FB_MAP_FAIL;		
            SendToClient( &NetMsgFB );
            return FALSE;
        }
        
        //연료가 다 떨어지면 탈 수 없다.;
        if (VehicleIsNotEnoughFull())
        {
            NetMsgFB.emFB = EMVEHICLE_SET_FB_NOTENOUGH_OIL;
            SendToClient( &NetMsgFB );
            return FALSE;	
        }

        //전투상황이면 탈 수 없다.;
        if( IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
        {
            NetMsgFB.emFB = EMVEHICLE_SET_FB_BATTLING;
            SendToClient( &NetMsgFB );
            return FALSE;	
        }

        //대련중에는 탈 수 없다.;
        if ( m_sCONFTING.IsCONFRONTING() )
        {
            NetMsgFB.emFB = EMVEHICLE_SET_FB_FAIL;
            SendToClient( &NetMsgFB );
            return FALSE;
        }
    }
    //탈것을 내릴때;
    else
    {
        // 뒤에탑승한 캐릭터들도 내리게 한다.;
        if (!bActive &&  VehicleState() == EMVEHICLE_PASSENGER)
        {
            for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
            {
                DWORD PassengerId = VehiclePassengerId(i);
                if (PassengerId == GAEAID_NULL || PassengerId == m_dwGaeaID)
                {
                    continue;
                }

                GLChar* pPassenger = GetChar(PassengerId);
                if (pPassenger)
                    pPassenger->ActiveWithVehicle(bActive, false);
            }
        }
    }
    ActiveVehicle(bActive, false);

	return TRUE;
}

void GLChar::SendToClient(NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLIENT(m_ClientSlot, pMsg);
}

void GLChar::SendToClient(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLIENT(m_ClientSlot, MsgType, Buffer, UseCrc);
}

void GLChar::SendToClient(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLIENT(m_ClientSlot, MsgType, vecSpBuffer, UseCrc);
}

void GLChar::SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg)
{
	if (m_pGaeaServer)
		m_pGaeaServer->SENDTOCLIENT(dwClientSlot, pMsg);
}

void GLChar::SendToClient(const DWORD dwClientSlot, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	if (m_pGaeaServer)
		m_pGaeaServer->SENDTOCLIENT(dwClientSlot, MsgType, Buffer, UseCrc);
}

void GLChar::SendToClient(const DWORD dwClientSlot, EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLIENT(dwClientSlot, MsgType, vecSpBuffer, UseCrc);
}

void GLChar::SendToAgent(NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOAGENT(pMsg);
}

void GLChar::SendToAgent(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOAGENT(MsgType, Buffer, UseCrc);
}

void GLChar::SendToAgent(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOAGENT(MsgType, vecSpBuffer, UseCrc);
}

void GLChar::SendToCache(NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCACHE(pMsg);
}

void GLChar::SendToCache(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCACHE(MsgType, Buffer, UseCrc);
}

void GLChar::SendToCache(EMNET_MSG MsgType, const std::vector<std::tr1::shared_ptr<msgpack::sbuffer>>& vecSpBuffer, bool UseCrc)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCACHE(MsgType, vecSpBuffer, UseCrc);
}

void GLChar::SendToIntegrationCache( NET_MSG_GENERIC* pMsg )
{
	if ( m_pGaeaServer )
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( pMsg );
}

void GLChar::SendToIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc /*= false*/ )
{
	if ( m_pGaeaServer )
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( MsgType, Buffer, UseCrc );
}

void GLChar::SendToIntegrationCache( EMNET_MSG MsgType, const std::vector< std::tr1::shared_ptr< msgpack::sbuffer > >& vecSpBuffer, bool UseCrc /*= false*/ )
{
	if ( m_pGaeaServer )
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( MsgType, vecSpBuffer, UseCrc );
}

//! 나의 Party 에게 전달
void GLChar::SendToParty(NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOPARTYCLIENT(m_PartyID, ServerActor::GetCurrentMap(), pMsg);
}

//! 다른 Party 에게 전달
void GLChar::SendToParty(const GLPartyID& PartyID, NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOPARTYCLIENT(PartyID, ServerActor::GetCurrentMap(), pMsg);
}

//! 나의 Club 에게 전달
void GLChar::SendToClub(NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLUBCLIENT(m_ClubDbNum, pMsg);
}

//! 다른 Club에게 전달
void GLChar::SendToClub(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDTOCLUBCLIENT(ClubDbNum, pMsg);
}

void GLChar::SENDCLUBLOG(club::EM_LOG_TYPE LogType, const std::string& LogMsg)
{
    if (m_pGaeaServer)
        m_pGaeaServer->SENDCLUBLOG(m_ClubDbNum, LogType, LogMsg);
}

void GLChar::AddGameDbJob(db::DbActionPtr spJob)
{
    m_pGaeaServer->AddGameDbJob(spJob);
}

void GLChar::AddGameAdoJob(db::DbActionPtr spJob)
{    
    if (m_pGaeaServer)
        m_pGaeaServer->AddGameAdoJob(spJob);
}

void GLChar::AddLogAdoJob(db::DbActionPtr spJob)
{    
    if (m_pGaeaServer)
        m_pGaeaServer->AddLogAdoJob(spJob);
}

void GLChar::AddShopAdoJob(db::DbActionPtr spJob)
{
    if (m_pGaeaServer)
        m_pGaeaServer->AddShopAdoJob(spJob);
}

DWORD GLChar::GetBonusExp () const
{
	return GLCONST_CHAR::GETEXP_PC(m_wLevel);
}

HRESULT GLChar::CreateChar(GLLandMan* pLandMan, D3DXVECTOR3 &vPos, PCHARDATA2 pCharData, BOOL bNEW)
{
	HRESULT hr;

	m_vPosition = vPos;

	m_emIPBonus = pCharData->m_emIPBonus;

	m_sChinaTime	   = pCharData->m_sChinaTime;
	m_sEventTime	   = pCharData->m_sEventTime;
	m_sVietnamSystem   = pCharData->m_sVietnamSystem;

	//m_bTracingUser	   = pCharData->m_bTracingUser;

//	m_sEventState	   = m_pGLGaeaServer->m_sEventState;

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount(m_szUID);

		CString strTemp;
		if ( pLandMan == NULL )
			strTemp.Format("%s, %s try to Enter the Map(NULL);", m_szUID, m_szName);
		else
			strTemp.Format("%s, %s try to Enter the Map(%d/%d);", m_szUID, m_szName, pLandMan->GetBaseMapID().Mid(), pLandMan->GetBaseMapID().Sid());

		TracingMsg.SetLog(strTemp.GetString());

		SendToAgent(&TracingMsg);
	}*/

	m_bEventStart = FALSE;
	m_bEventApply = FALSE;
	if (m_pGaeaServer->m_sEventState.bEventStart == TRUE)
	{
		// 이벤트가 새로 시작 됨
		CTime	  crtTime     = CTime::GetCurrentTime();
		CTimeSpan crtGameSpan;
		CTime	  loginTime			 = m_sEventTime.loginTime;
		crtGameSpan					 = ( crtTime - loginTime );
		m_sEventTime.currentGameTime = crtGameSpan.GetTimeSpan();

		if( crtGameSpan.GetTotalSeconds() >= m_pGaeaServer->m_sEventState.EventPlayTime )
		{
			m_bEventStart = TRUE;
		}

		if( !(m_pGaeaServer->m_sEventState.MinEventLevel > pCharData->m_wLevel || 
			  m_pGaeaServer->m_sEventState.MaxEventLevel < pCharData->m_wLevel ) )
		{
			m_bEventApply = TRUE;
		}
	}


	hr = SetData(pCharData, bNEW);
	if ( FAILED(hr) )
		return E_FAIL;

	m_bEntryFailed = FALSE;

	// 진입조건 검사
	if (m_dwUserLvl < USER_USER_GM)
	{
		if (pLandMan->GetBaseMapID() != NATIVEID_NULL())
		{
			const SLEVEL_REQUIRE* pLEVEL_REQUIRE = m_pGaeaServer->GetLevelRequire(pLandMan->GetBaseMapID().dwID);
			EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
			
			if( pLEVEL_REQUIRE ) 
			{
				emReqFail = pLEVEL_REQUIRE->ISCOMPLETE(this);
			}
			else
			{
				sc::writeLogInfo(sc::string::format("Level Data Load failed! charID[%1%] mapID[%2%][%3%] EMFail[%4%]", m_szName, pLandMan->GetBaseMapID().wMainID, pLandMan->GetBaseMapID().wSubID, emReqFail));
			}

			if ( emReqFail != EMREQUIRE_COMPLETE )
			{
				sc::writeLogInfo(sc::string::format("Entry failed! charID[%1%] mapID[%2%][%3%] EMFail[%4%]", m_szName, pLandMan->GetBaseMapID().wMainID, pLandMan->GetBaseMapID().wSubID, emReqFail ));
				m_bEntryFailed = TRUE;
			}
		}
		else
		{
			sc::writeLogInfo(sc::string::format("Land ID is NATIVEID NULL! charID[%1%]", m_szName ));
		}
	}

	m_vecLandEffect.clear();
	m_vecMapEffect.clear();

	hr = SetNavi(pLandMan->GetNavi(), m_vPosition);
	if(FAILED(hr))
		return E_FAIL;

	m_lnLastSendExp = m_sExperience.lnNow;

	m_wLastSendCP = m_sCP.wLow;

	m_lnLastSendVNExp = m_lVNGainSysExp;

	m_bServerStorage = TRUE;	

	DWORD Num1 = m_cInventory.GetNumItems();
	DWORD Num2 = m_cVietnamInventory.GetNumItems();

	m_fMoveDelay = GLCONST_CHAR::fPK_SAFE_TIME;
	CTime cTIME_CUR = CTime::GetCurrentTime();

	// 창고의 유효성 검사
	for(WORD i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i)
	{
		CTime cSTORAGE(m_tSTORAGE[i]);

		bool bVALID(true);
		if ( m_tSTORAGE[i]==0 || cSTORAGE<cTIME_CUR )
			bVALID = false;
		
		m_bSTORAGE[i] = bVALID;
	}

	// 프리미엄 여부 검사
	bool bPREMIUM(true);
	CTime tPREMIUM(m_tPREMIUM);
	if ( m_tPREMIUM==0 || tPREMIUM<cTIME_CUR )
		bPREMIUM = false;
	m_bPREMIUM = bPREMIUM;

	m_cInventory.SetAddLine(GetOnINVENLINE(), true);
	
	// 베트남 인벤토리는 세로가 10칸이므로 6칸을 더 추가한다
	if (m_ServiceProvider == SP_VIETNAM)
	{
		m_cVietnamInventory.SetAddLine( 6, true );
	}

	// Note : SkillFact 적용
	for (int i=0; i<SKILLFACT_SIZE; ++i)
	{
		SCHARSAVESKILLFACT &sSAVESKILLFACT = m_sSAVESKILLFACT[i];
		DWORD             dwSelectIndex    = 0;

		if (sSAVESKILLFACT.sNATIVEID == NATIVEID_NULL())
		{
			continue;
		}

		// QQQ;
        EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX(sSAVESKILLFACT.wSLOT);
		RECEIVE_SKILLFACT(sSAVESKILLFACT.sNATIVEID, static_cast<WORD>(sSAVESKILLFACT.cLEVEL), dwSelectIndex, 
			sSAVESKILLFACT.sID, SSKILLFACT::STIME(sSAVESKILLFACT), sSAVESKILLFACT.dwCount, emType, sSAVESKILLFACT.fLaunchTime, sSAVESKILLFACT.fLaunchValue);		

	}

	if (pLandMan->IsCPReset())
	{
		ResetCP();
	}

	SetActState(EM_ACT_WAITING);

	return S_OK;
}


BOOL GLChar::InitSerializedChar(GLLandMan* pLandMan, BOOL bNEW)
{
	HRESULT hr;
	
	// 진입조건 검사
	if (m_dwUserLvl < USER_USER_GM)
	{
		if (pLandMan->GetBaseMapID() != NATIVEID_NULL())
		{
			const SLEVEL_REQUIRE* pLEVEL_REQUIRE = m_pGaeaServer->GetLevelRequire(pLandMan->GetBaseMapID().dwID);
			EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);

			if (pLEVEL_REQUIRE)
			{
				emReqFail = pLEVEL_REQUIRE->ISCOMPLETE(this);
			}
			else
			{
				sc::writeLogInfo(sc::string::format( "Level Data Load failed! charID[%1%] mapID[%2%][%3%] EMFail[%4%]", m_szName, pLandMan->GetBaseMapID().wMainID, pLandMan->GetBaseMapID().wSubID, emReqFail ) );
			}

			if (emReqFail != EMREQUIRE_COMPLETE)
			{
				sc::writeLogInfo(sc::string::format( "Entry failed! charID[%1%] mapID[%2%][%3%] EMFail[%4%]", m_szName, pLandMan->GetBaseMapID().wMainID, pLandMan->GetBaseMapID().wSubID, emReqFail ) );
				m_bEntryFailed = TRUE;
			}
		}
		else
		{
			sc::writeLogInfo(sc::string::format( "Land ID is NATIVEID NULL! charID[%1%]", m_szName ) );
		}
	}

	m_vecLandEffect.clear();
	m_vecMapEffect.clear();	

	hr = SetNavi(pLandMan->GetNavi(), m_vPosition);
	if (FAILED(hr))
	{
		sc::writeLogError(sc::string::format("GLChar::InitSerializedChar - SetNavi failure for the Char(%1%).", CharDbNum()));
		return FALSE;
	}

	m_lnLastSendExp = m_sExperience.lnNow;

	m_wLastSendCP = m_sCP.wLow;

	m_lnLastSendVNExp = m_lVNGainSysExp;

	m_bServerStorage = TRUE;	

	DWORD Num1 = m_cInventory.GetNumItems();
	DWORD Num2 = m_cVietnamInventory.GetNumItems ();

	m_fMoveDelay = GLCONST_CHAR::fPK_SAFE_TIME;
	CTime cTIME_CUR = CTime::GetCurrentTime();

	// 창고의 유효성 검사
	for ( WORD i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
	{
		CTime cSTORAGE(m_tSTORAGE[i]);

		bool bVALID(true);
		if ( m_tSTORAGE[i]==0 || cSTORAGE<cTIME_CUR )
			bVALID = false;
		
		m_bSTORAGE[i] = bVALID;
	}

	// 프리미엄 여부 검사
	bool bPREMIUM(true);
	CTime tPREMIUM(m_tPREMIUM);
	if ( m_tPREMIUM==0 || tPREMIUM<cTIME_CUR )
		bPREMIUM = false;
	
	m_bPREMIUM = bPREMIUM;

	m_cInventory.SetAddLine( GetOnINVENLINE(), true );
	
	// 베트남 인벤토리는 세로가 10칸이므로 6칸을 더 추가한다
	if (m_ServiceProvider == SP_VIETNAM)
	{
		m_cVietnamInventory.SetAddLine( 6, true );
	}

	// Note : SkillFact 적용
	for (int i=0; i<SKILLFACT_SIZE; ++i)
	{
		SCHARSAVESKILLFACT &sSAVESKILLFACT = m_sSAVESKILLFACT[i];
		DWORD             dwSelectIndex    = 0;

		if ( sSAVESKILLFACT.sNATIVEID == NATIVEID_NULL() )
		{
			continue;
		}

        EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX ( sSAVESKILLFACT.wSLOT );

		// QQQ'
        RECEIVE_SKILLFACT(sSAVESKILLFACT.sNATIVEID, static_cast<WORD>(sSAVESKILLFACT.cLEVEL), dwSelectIndex, 
                sSAVESKILLFACT.sID, SSKILLFACT::STIME(sSAVESKILLFACT), sSAVESKILLFACT.dwCount, emType, sSAVESKILLFACT.fLaunchTime, sSAVESKILLFACT.fLaunchValue);

	}

	if (pLandMan->IsCPReset())
	{
		ResetCP();
	}

	SetActState(EM_ACT_WAITING);

	return TRUE;
}


HRESULT GLChar::SetNavi ( boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3 &vPos )
{	
	if ( PathIsActive() )
		StopActorMove();

	CreateActorMove(spNavi, vPos);
	SetPosition(vPos);
	
	SetMaxSpeed(GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO);

	return S_OK;
}

HRESULT GLChar::SetData ( PCHARDATA2 pCharData, BOOL bNEW )
{
	SCHARDATA2::Assign ( *pCharData );

	// 유효한 스킬 검사;
	SET_VALIDATION_EXPSKILL_MAP();
	
	if ( !GLCHARLOGIC::INIT_DATA(bNEW,TRUE) )
        return E_FAIL;

	return S_OK;
}

HRESULT GLChar::SavePosition ()
{
    TriggerSystem::SServerSavePosMessage msg;
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

    pLand->SendTriggerMessage(&msg);

    if (msg.bSave == false)
    {
        return S_OK;
    }

	// 죽은상태일 때는 현재 포지션을 저장하지 않는다.
	//
	if ( IsDie() )
	{
		m_sSaveMapID = NATIVEID_NULL();
	}
	// 현재 위치 저장.
	//
	else
	{
		m_sSaveMapID = pLand->GetGaeaMapID();
		m_vSavePos = m_vPosition;
	}

	return S_OK;
}

// 직전 위치를 저장.
//
HRESULT GLChar::SaveLastCall ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_FALSE;

	// 만약 선도 클럽전용 멥일 경우 직전 위치를 저장하지 않음.
	// 인스턴트 맵인 경우도 저장하지 않음
	if ( !pLand->m_bGuidBattleMap && !pLand->m_bClubDeathMatchMap && !pLand->isMapInstant() )
	{
		m_sLastCallMapID = pLand->GetGaeaMapID();
		m_vLastCallPos = m_vPosition;

		GLMSG::SNETPC_UPDATE_LASTCALL NetMsg;
		NetMsg.sLastCallMapID = m_sLastCallMapID;
		NetMsg.vLastCallPos = m_vLastCallPos;
		SendToClient( &NetMsg );

		return S_OK;
	}

	return S_FALSE;
}

HRESULT GLChar::SaveSkillFact ()
{
	// Note : 초기화
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		m_sSAVESKILLFACT[i] = SCHARSAVESKILLFACT();
	}

	// Note : 현재 LunchBox와 System만 저장한다
    int nSaveIndex = 0;

	// 일반, 버프 스킬의 경우 유지시킨다;
	for ( WORD i=0; i<SKILLFACT_SIZE_NORMAL; ++i )
	{
		WORD nFactSlot = i + SKILLFACT_INDEX_NORMAL_START;
		if ( nFactSlot < SKILLFACT_INDEX_NORMAL_END && m_sSKILLFACT[nFactSlot].sNATIVEID != SNATIVEID(false) )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[nFactSlot].sNATIVEID );
			if ( NULL == pSkill )
				continue;

			SKILL::EMACTION_TYPE emActionType = pSkill->m_sBASIC.emACTION_TYPE;
			if ( SKILL::EMACTION_TYPE_NORMAL == emActionType ||
				SKILL::EMACTION_TYPE_BUFF == emActionType )
				m_sSAVESKILLFACT[ nSaveIndex++ ].Assign( nFactSlot, m_sSKILLFACT[nFactSlot] );
		}
	}

	// 도시락의 경우 유지시킨다;
	for (WORD i=0; i<SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX; ++i)
	{
		WORD nFactSlot = i + SKILLFACT_INDEX_LUNCHBOX_START;    
        if (nFactSlot < SKILLFACT_SIZE && m_sSKILLFACT[nFactSlot].sNATIVEID != SNATIVEID(false))
		{
			
			if ( m_sSKILLFACT[nFactSlot].IsHaveSPECADDON(EMSPECA_RANDOM_EXP_RATE) == TRUE )				
 				m_sSAVESKILLFACT[nSaveIndex].Assign(nFactSlot, m_sSKILLFACT[nFactSlot], m_ExperienceRate);
			else
				m_sSAVESKILLFACT[nSaveIndex].Assign(nFactSlot, m_sSKILLFACT[nFactSlot]);
			++nSaveIndex;
		}
	}

	// 시스템 버프의 경우 유지시킨다;
    for (WORD i=0; i<SKILLFACT_SIZE_SYSTEM; ++i)
    {
        WORD nFactSlot = i + SKILLFACT_INDEX_SYSTEM_START;
        if (nFactSlot < SKILLFACT_SIZE && m_sSKILLFACT[nFactSlot].sNATIVEID != SNATIVEID(false))
            m_sSAVESKILLFACT[nSaveIndex++].Assign(nFactSlot, m_sSKILLFACT[nFactSlot]);
    }

	return S_OK;
}

HRESULT GLChar::LoadStorageFromDB ()
{
	if ( m_bServerStorage )
		return S_OK;

	sc::writeLogError ( sc::string::format( "[ User Inven ] [ Invalid Load User Inven, %1% ]", GetUserID() ) );

	/*db::IDbManager* pDBMan = m_pGaeaServer->GetDBMan();
	if (pDBMan)
        pDBMan->ReadUserInvenNew( &GetCharData2() );
	m_bServerStorage = TRUE;*/
	return S_OK;
}

void GLChar::SetPosition(const D3DXVECTOR3& vPosition)
{	
	if ( SetPositionActor(vPosition) == true )
		StopActorMove();

	RefreshPosition(0.0f);
}


BOOL GLChar::IsCtrlBlockBody () const
{
	if(isAction(GLAT_SHOCK))
		return TRUE;

	if(isAction(GLAT_PUSHPULL))
		return TRUE;

	return FALSE;
}

bool GLChar::IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const
{
	return GLCHARLOGIC::IsCheckedSkillFlagSpecial( dwFlag );
}

const BOOL GLChar::IsFeignDieDetect() const
{
	if ( IsCheckedSkillFlagSpecial(EMSPECA_FEIGN_DEATH) == false )
		return TRUE;

	return FALSE;
}

BOOL GLChar::IsVaildTradeData ()
{
	//	금액 유효성 검사.
	if ( GetInvenMoney() < m_sTrade.GetMoney() )
		return FALSE;

	GLInventory &sTradeBox = m_sTrade.GetTradeBox();
	const GLInventory::CELL_MAP* pItemList = sTradeBox.GetItemList();

	GLInventory::CELL_MAP_CITER iter = pItemList->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemList->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;
		SINVENITEM* pInvenItem = m_cInventory.GetItem( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !pInvenItem )
			return FALSE;

		if ( pTradeItem->sItemCustom != pInvenItem->sItemCustom )
			return FALSE;

/*		if ( memcmp ( &pTradeItem->sItemCustom, &pInvenItem->sItemCustom, sizeof(SITEMCUSTOM) ) )
			return FALSE;*/
	}

	return TRUE;
}

BOOL GLChar::IsVaildTradeInvenSpace ( GLInventory &sTradeBoxTar )
{
	/*
	GLInventory sInvenTemp;
	sInvenTemp.Assign( m_cInventory );

	// 자신의 교환 목록에 올라간 아이템을 인밴에서 제외.
	//
	GLInventory &sTradeBoxMy = m_sTrade.GetTradeBox();
	const GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();

	GLInventory::CELL_MAP_CITER iter = pItemListMy->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemListMy->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		BOOL bOk = sInvenTemp.DeleteItem( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !bOk )
			return FALSE;
	}

	// 상대방의 교환 물품이 들어 갈수 있는지 검사.
	//
	const GLInventory::CELL_MAP* pItemListTar = sTradeBoxTar.GetItemList();

	iter = pItemListTar->begin();
	iter_end = pItemListTar->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pTradeItem->sItemCustom.GetNativeID() );
		if ( !pItem )
			return FALSE;

		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = sInvenTemp.FindInsertable( wPosX, wPosY );
		if ( !bOk )
            return FALSE;

		bOk = sInvenTemp.InsertItem( pTradeItem->sItemCustom, wPosX, wPosY );
		if ( !bOk )
            return FALSE;
	}
	
	return TRUE;
	*/

	// 불필요한 Assign 삭제
	DWORD MyTradeBoxSize = m_sTrade.GetTradeBox().GetNumItems();
	DWORD TarTradeBoxSize = sTradeBoxTar.GetNumItems();
	DWORD InvenEmptyCount = m_cInventory.GetAmountEmpty( false );

	// 최대 넣을 수 있는 공간 개수
	DWORD InvenEmptyTotalCount = InvenEmptyCount + MyTradeBoxSize;

	if ( TarTradeBoxSize > InvenEmptyTotalCount )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL GLChar::DoTrade( GLTrade& sTarTrade, DWORD TarCharDbNum )
{
	GASSERT( m_pGaeaServer->IsField() );

	// 줄 금액.
	LONGLONG lnMyMoney = m_sTrade.GetMoney();
	if ( GetInvenMoney() < lnMyMoney )
        return FALSE;
	CheckMoneyUpdate( GetInvenMoney(), lnMyMoney, FALSE, "Do Trade Out Money." );

	// 받을 금액.
    GLChar* pTarChar = GetCharByDbNum( TarCharDbNum );
    if ( !pTarChar )
    {
		sc::writeLogError( sc::string::format( "DoTrade if ( !pTarChar ), target chanum: %1%", TarCharDbNum ) );
		return FALSE;
    }

	LONGLONG lnTarMoney = sTarTrade.GetMoney();
    if ( pTarChar->GetInvenMoney() < lnTarMoney )
        return FALSE;
	CheckMoneyUpdate( GetInvenMoney(), lnTarMoney, TRUE, "Do Trade In Money." );

	/*if ( m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = GetUserID();
		TracingMsg.SetAccount( m_szUID );

		CString strTemp;
		GLChar* pFromChar = GetCharByDbNum( TarCharDbNum );
		if ( pFromChar )
		{
			strTemp.Format(
				"Do Trade! From, [%s][%s], Target[%s][%s], RecvMoney:[%I64d], SendMoney:[%I64d], HaveMoney:[%I64d], Target HaveMoney:[%I64d]",
				m_szUID,
				m_szName,
				pFromChar->m_szUID,
				pFromChar->m_szName,
				lnTarMoney,
				lnMyMoney,
				GetInvenMoney(),
				pFromChar->GetInvenMoney() );
		}
		else
		{
			strTemp.Format(
				"Do Trade! From, [%s][%s], Target Not Found, RecvMoney:[%I64d], SendMoney:[%I64d], HaveMoney:[%I64d]",
				m_szUID,
				m_szName,
				lnTarMoney,
				lnMyMoney,
				GetInvenMoney() );
		}

		TracingMsg.SetLog( strTemp.GetString() );

		SendToAgent( &TracingMsg );
	}*/

    // 아이템 거래 할 수 있는지 temp inventory를 만들어 확인해본다.
    std::vector< GLMSG::SNETPC_INVEN_DELETE > vecMyInvenDelete;
    std::vector< GLMSG::SNETPC_INVEN_DELETE > vecTarInvenDelete;
    std::vector< SINVENITEM > vecMyInvenInsert;
    std::vector< SINVENITEM > vecTarInvenInsert;

    GLInventory MyTempInventory;
    MyTempInventory.Assign( m_cInventory );

    GLInventory TarTempInventory;
    TarTempInventory.Assign( pTarChar->m_cInventory );

	// 인벤토리 복사해서 확인하는 부분 수정하려했으나
	// 예외처리 할 부분이 너무 많아져서 그냥 원래대로 한다.
    {
        // my temp inventory.

        GLInventory& sTradeBoxMy = m_sTrade.GetTradeBox();
        const GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();
        if ( !pItemListMy )
        {
            sc::writeLogError( std::string( "DoTrade - pItemListMy is null.") );
            return FALSE;
        }

        GLInventory& sTradeBoxTar = sTarTrade.GetTradeBox();
        const GLInventory::CELL_MAP* pItemListTar = sTradeBoxTar.GetItemList();
        if ( !pItemListTar )
        {
            sc::writeLogError( std::string( "DoTrade - pItemListTar is null.") );
            return FALSE;
        }

        // delete items.
        GLInventory::CELL_MAP_CITER iter = pItemListMy->begin();
        GLInventory::CELL_MAP_CITER iter_end = pItemListMy->end();
        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pTradeItem = ( *iter ).second;
			if ( !pTradeItem )
			{
				return FALSE;
			}

			if ( !MyTempInventory.DeleteItem( pTradeItem->wBackX, pTradeItem->wBackY ) )
			{
				return FALSE;
			}
			else
			{
				GLMSG::SNETPC_INVEN_DELETE Msg( pTradeItem->wBackX, pTradeItem->wBackY, FALSE );
				Msg.dwChaNum = CharDbNum();
				vecMyInvenDelete.push_back( Msg );
			}
        }

        // insert items.
        iter = pItemListTar->begin();
        iter_end = pItemListTar->end();	
        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pTradeItem = ( *iter ).second;
			if ( !pTradeItem )
			{
				return FALSE;
			}

			SITEM* pItem = GLogicData::GetInstance().GetItem( pTradeItem->sItemCustom.GetNativeID() );
			if ( !pItem )
				return FALSE;

			WORD wPosX = 0, wPosY = 0;
            BOOL bOk = MyTempInventory.FindInsertable( wPosX, wPosY );
            if ( !bOk )
                return FALSE;

            bOk = MyTempInventory.InsertItem( pTradeItem->sItemCustom, wPosX, wPosY );
            if ( !bOk )
                return FALSE;

            SINVENITEM* pInvenItem = MyTempInventory.GetItem( wPosX, wPosY );
			if ( !pInvenItem )
			{
				return FALSE;
			}

            vecMyInvenInsert.push_back( *pInvenItem );
        }

        // target temp inventory.

        // delete items.
        iter = pItemListTar->begin();
        iter_end = pItemListTar->end();
        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pTradeItem = ( *iter ).second;
            if ( !TarTempInventory.DeleteItem( pTradeItem->wBackX, pTradeItem->wBackY ) )
                return FALSE;
            else
            {
                GLMSG::SNETPC_INVEN_DELETE Msg( pTradeItem->wBackX, pTradeItem->wBackY, FALSE );
                Msg.dwChaNum = TarCharDbNum;
                vecTarInvenDelete.push_back( Msg );
            }
        }

        // insert items.
        iter = pItemListMy->begin();
        iter_end = pItemListMy->end();	
        for ( ; iter!=iter_end; ++iter )
        {
            SINVENITEM* pTradeItem = ( *iter ).second;
			if ( !pTradeItem )
			{
				return FALSE;
			}

            SITEM* pItem = GLogicData::GetInstance().GetItem( pTradeItem->sItemCustom.GetNativeID() );
            if ( !pItem )
                return FALSE;

            WORD wPosX = 0, wPosY = 0;
            BOOL bOk = TarTempInventory.FindInsertable( wPosX, wPosY );
            if ( !bOk )
                return FALSE;

            bOk = TarTempInventory.InsertItem( pTradeItem->sItemCustom, wPosX, wPosY );
            if ( !bOk )
                return FALSE;

            SINVENITEM* pInvenItem = TarTempInventory.GetItem( wPosX, wPosY );
			if ( !pInvenItem )
			{
				return FALSE;
			}

            vecTarInvenInsert.push_back( *pInvenItem );
        }
    }

    // success trade.

    // trade money.
    {
		// 돈 빼줌.
		SubtractInvenMoneyServer( lnMyMoney, SUBTRACT_INVEN_MONEY_TRADE );
		pTarChar->SubtractInvenMoneyServer( lnTarMoney, SUBTRACT_INVEN_MONEY_TRADE );

		AddInvenMoneyServer( lnTarMoney, ADD_INVEN_MONEY_TRADE );
		pTarChar->AddInvenMoneyServer( lnMyMoney, ADD_INVEN_MONEY_TRADE );

        // 금액 로그.
        //
        LONGLONG lnMoneyDx = ( lnTarMoney - lnMyMoney );
        if ( lnMoneyDx != 0 )
        {   
			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, TarCharDbNum,
                gamelog::ID_CHAR, m_CharDbNum,
                lnMoneyDx, ITEM_ROUTE_CHAR, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_TRADE );

			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
        }

        lnMoneyDx = ( lnMyMoney - lnTarMoney );
        if (lnMoneyDx != 0)
        {   
			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, TarCharDbNum,
                lnMoneyDx, ITEM_ROUTE_CHAR, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_TRADE );

			// 통계 및 로그 수정
            LogMoneyExchange(
                gamelog::ID_CHAR, TarCharDbNum,
                gamelog::ID_CHAR, 0,
                pTarChar->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
        }

        // 금액 변화 클라이언트에 알림.
        //
        SendInvenMoneyUpdate();
        pTarChar->SendInvenMoneyUpdate();
    }

    // trade items.
    {
        m_cInventory.Assign( MyTempInventory );
        pTarChar->m_cInventory.Assign( TarTempInventory );

        // send my delete msg.
        for ( size_t i = 0; i < vecMyInvenDelete.size(); i++ )
        {
            GLMSG::SNETPC_INVEN_DELETE& NetMsg = vecMyInvenDelete[i];
            InvenDeleteSend( NetMsg.wPosX, NetMsg.wPosY, NetMsg.bVietnamInven );
        }

        // send target delete msg.
        for ( size_t i = 0; i < vecTarInvenDelete.size(); i++ )
        {
            GLMSG::SNETPC_INVEN_DELETE& NetMsg = vecTarInvenDelete[i];
            pTarChar->InvenDeleteSend( NetMsg.wPosX, NetMsg.wPosY, NetMsg.bVietnamInven );
        }

        // send my insert msg.
        for ( size_t i = 0; i < vecMyInvenInsert.size(); i++)
        {
            SINVENITEM* pInvenItem = &vecMyInvenInsert[i];
            if ( !pInvenItem )
            {
                sc::writeLogError( std::string( "DoTrade - insert my item is null." ) );
                continue;
            }

			InvenUpdateItem( pInvenItem, true );

            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange( 
                pInvenItem->sItemCustom,
                gamelog::ID_CHAR, TarCharDbNum,
                gamelog::ID_CHAR, m_CharDbNum,
                ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_TRADE,
				false );

            // 아이템 변화를 클라이언트에 알림.
            InvenInsertSend( pInvenItem, FALSE, false, true );
        }

        // send target insert msg.
        for ( size_t i = 0; i < vecTarInvenInsert.size(); i++)
        {
            SINVENITEM* pInvenItem = &vecTarInvenInsert[i];
            if ( !pInvenItem )
            {
                sc::writeLogError( std::string( "DoTrade - insert target item is null." ) );
                continue;
            }

			pTarChar->InvenUpdateItem( pInvenItem, true );

            // 아이템의 소유 이전 경로 기록
			// 통계 및 로그 수정
            LogItemExchange(
                pInvenItem->sItemCustom,
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, TarCharDbNum,
                ITEM_ROUTE_CHAR, pInvenItem->sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_TRADE,
				false );

            // 아이템 변화를 클라이언트에 알림.
            pTarChar->InvenInsertSend( pInvenItem, FALSE, false, true );
        }
    }

	return TRUE;
}

// *****************************************************
// Desc: 시간제한 아이템 업데이트
// *****************************************************
void GLChar::RESET_TIMELMT_ITEM()
{
	std::vector< GLInventory::CELL_KEY > vecDELETE;

	const CTime cTIME_CUR = CTime::GetCurrentTime();

	SINVENITEM* pInvenItem = NULL;
	SITEM* pITEM = NULL;
	GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;

	BOOL bValidTime = FALSE;

	// 팻 아이템 슬롯
	PGLPETFIELD pMyPet = GetMyPet();
	if ( pMyPet && pMyPet->IsValid() )
        pMyPet->UpdateTimeLmtItem( this );

	// 탈것 아이템 슬롯 
	if ( m_pVehicle->IsActiveValue() )
    {
        GLVEHICLE_FIELD::UpdateTimeLmtItem( m_pGaeaServer, this, m_pVehicle );
    }

	GLItemMan& glItemMan = GLItemMan::GetInstance();
	
	// 인벤토리 아이템
	const GLInventory::CELL_MAP* pMapItem = m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
	GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
	for ( ; iter!=iter_end; ++iter )
	{
		pInvenItem = ( *iter ).second;
		pITEM = glItemMan.GetItem( pInvenItem->sItemCustom.GetNativeID() );
		if ( !pITEM )
			continue;

		bool bDELETE( false );

		// 시한부 아이템
		if ( pInvenItem->sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
		{
			CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
			CTime cTIME_LMT( pInvenItem->sItemCustom.tBORNTIME );
			cTIME_LMT += cSPAN + pInvenItem->sItemCustom.GetPeriodExBasic();

			if ( cTIME_CUR > cTIME_LMT )
			{
				vecDELETE.push_back ( ( *iter ).first );
				bDELETE = true;

				if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.m_VehicleDbNum != 0 )
				{
					// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
					GLVEHICLE* pNewVehicle = new GLVEHICLE;
					AddGameAdoJob( 
                        db::DbActionPtr( 
                            new CGetVehicle( 
                                pNewVehicle, 
					            pInvenItem->sItemCustom.m_VehicleDbNum, 
					            m_ClientSlot, 
					            m_CharDbNum, 
					            true ) ) );					
				}

				// 팻카드일 경우
				if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.PetDbNum() != 0 )
				{
					// 활동중이면 사라지게 해준다.
					if ( pMyPet && pInvenItem->sItemCustom.PetDbNum() == pMyPet->DbNum() ) 
					{
						m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO( pMyPet->m_dwGUID, true, false ) );

						// 팻의 악세사리를 검사한다. 
						pMyPet->UpdateTimeLmtItem ( this );

						// 넣어줄꺼 넣어준다.
						for ( WORD i = 0; i < ACCETYPESIZE; ++i )
						{
							if ( pMyPet->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
							{
								continue;
							}

							CItemDrop cDropItem;
							cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
							if ( IsInsertToInven ( &cDropItem ) )
							{
								InsertToInven( &cDropItem, false, false, false );
							}
							else
							{
								// 바닥에 떨어뜨린다.
								// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
								// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
								// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
								// 누군가 줍는다면 자기 아이템으로 업데이트된다.
								DropItem( cDropItem.sItemCustom, m_vPosition );
							}
						}
						
						AddGameAdoJob(
                            db::DbActionPtr(
                                new db::PetDelete( m_CharDbNum, pInvenItem->sItemCustom.PetDbNum() ) ) );
						
						// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.						
						AddGameAdoJob(
                            db::DbActionPtr(
                                new db::PetListRestoreGet( m_CharDbNum, m_ClientSlot ) ) );
					}
					else
					{
						// 악세사리를 체크후 팻을 DB에서 삭제해준다.
						GLPetField* pNewPet = new GLPetField( m_pGaeaServer );
						AddGameAdoJob( 
                            db::DbActionPtr( 
                                new db::PetGet( 
                                    pNewPet, 
                                    pInvenItem->sItemCustom.PetDbNum(), 
                                    m_ClientSlot, 
                                    m_CharDbNum, 
                                    pInvenItem->wPosX, 
                                    pInvenItem->wPosY, 
                                    true ) ) );                        
					}
				}
			}
		}

		// 코스튬 능력치가 적용 되어 있는 아이템 만료체크
		if ( !bDELETE && pInvenItem->sItemCustom.IsCSApplied() )
		{
			CTime cTimeLimit( pInvenItem->sItemCustom.costumeUser.getEndTime() );

			bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

			if ( bValidTime )
			{
				if ( cTIME_CUR > cTimeLimit )
				{
					// 능력치 초기화 알림
					GLMSG::SNET_INVEN_COSTUME_STATS_FB netMsgFb;

					netMsgFb.emFB    = ENREQ_CS_FB_NOTICE1;
					netMsgFb.sItemID = pInvenItem->sItemCustom.GetNativeID();
					SendToClient( &netMsgFb );

					// 코스튬에 능력치 초기화
					pInvenItem->sItemCustom.costumeUser.ResetInvest();

					NetItemUpdate.wPosX       = pInvenItem->wPosX;
					NetItemUpdate.wPosY       = pInvenItem->wPosY;
					NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
					SendToClient( &NetItemUpdate );

					// 로그
					LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_COSTUME_STATE_ERASE, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
				}
			}
		}

		if ( !bDELETE && pInvenItem->sItemCustom.IsTempEnchant() )
		{
			CTime cTimeLimit( pInvenItem->sItemCustom.sTLGrind.tFireTime );

			bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

			if ( bValidTime )
			{
				if ( cTIME_CUR > cTimeLimit )
				{
					GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

					netMsgFb.eResult   = EMREQ_TEMP_ENCHANT_FIRETIME;
					netMsgFb.fireItemID = pInvenItem->sItemCustom.GetNativeID();
					SendToClient( &netMsgFb );

					// 강화복구
					pInvenItem->sItemCustom.TempEnchantRecovery();

					NetItemUpdate.wPosX       = pInvenItem->wPosX;
					NetItemUpdate.wPosY       = pInvenItem->wPosY;
					NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
					SendToClient( &NetItemUpdate );

					// 로그
					LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );
				}
			}
		}

		// 코스튬
		if ( !bDELETE && pInvenItem->sItemCustom.nidDISGUISE != SNATIVEID( false ) )
		{
			if ( pInvenItem->sItemCustom.tDISGUISE != 0 )
			{
				CTime cTIME_LMT( pInvenItem->sItemCustom.tDISGUISE + pInvenItem->sItemCustom.GetPeriodExDisguise());

				bValidTime = m_pGaeaServer->IsValidTime( cTIME_LMT );

				if ( bValidTime )
				{
					if ( cTIME_CUR > cTIME_LMT )
					{
						//	시간 제한으로 아이템 삭제 알림.						
						NetMsgInvenDelTimeLmt.nidITEM = pInvenItem->sItemCustom.nidDISGUISE;
						SendToClient( &NetMsgInvenDelTimeLmt );

						// 코스툼 정보 리셋.
						pInvenItem->sItemCustom.tDISGUISE = 0;
						pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );

						// 코스튬 염색값 지움. by luxes.
						//
						pInvenItem->sItemCustom.dwMainColor = 0;
						pInvenItem->sItemCustom.dwSubColor = 0;

						// 코스튬에 능력치 초기화
						pInvenItem->sItemCustom.costumeUser.ResetInvest();

						// 클라이언트에 알림.						
						NetItemUpdate.wPosX = pInvenItem->wPosX;
						NetItemUpdate.wPosY = pInvenItem->wPosY;
						NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
						SendToClient( &NetItemUpdate );

						// 로그 저장                        
						LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_TIMEOUT );
					}
				}
				else
				{
					sc::writeLogError( sc::string::format( "GLChar::RESET_TIMELMT_ITEM() - Invalid CTime is set for an item(%1%, %2%) of Char(%3%)."
						, pInvenItem->sItemCustom.Mid(), pInvenItem->sItemCustom.Sid(), CharDbNum() ) );
				}
			}
		}
	}

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on RESET_TIMELMT_ITEM", m_CharDbNum ) );
	}

	if ( !vecDELETE.empty() )
	{		
		for ( DWORD i=0; i<vecDELETE.size(); ++i )
		{
			const GLInventory::CELL_KEY& cKEY = vecDELETE[i];
			SINVENITEM sINVENITEM = *m_cInventory.GetItem( cKEY.first,cKEY.second );
			SINVENITEM* pINVENTIEM = m_cInventory.GetItem( cKEY.first, cKEY.second );

			SNATIVEID nidITEM = sINVENITEM.sItemCustom.GetNativeID();

			// 시간 제한으로 아이템 삭제 로그 남김.
			// 통계 및 로그 수정
			LogItemExchange(
                sINVENITEM.sItemCustom,
                gamelog::ID_CHAR, m_CharDbNum,
                gamelog::ID_CHAR, 0,
                ITEM_ROUTE_DELETE, sINVENITEM.sItemCustom.wTurnNum, 
				( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,
				false);
			
			// 아이템 삭제
            InvenDeleteItem( cKEY.first, cKEY.second, true, FALSE, DELETE_INVEN_ITEM_RESET_TIMELMT_ITEM, true );

			//	시간 제한으로 아이템 삭제 알림.
			GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
			NetMsgInvenDelTimeLmt.nidITEM = nidITEM;
			SendToClient( &NetMsgInvenDelTimeLmt );

			// 코스툼 복구.
			if ( sINVENITEM.sItemCustom.nidDISGUISE != SNATIVEID( false ) )
			{
				SITEM* pONE = glItemMan.GetItem( sINVENITEM.sItemCustom.nidDISGUISE );
				if ( !pONE )
					continue;

				//	인벤 위치.
				WORD wInsertPosX = cKEY.first, wInsertPosY = cKEY.second;

				SITEMCUSTOM sITEM_NEW( sINVENITEM.sItemCustom.nidDISGUISE );
				CTime cTIME = CTime::GetCurrentTime();
				if ( sINVENITEM.sItemCustom.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
				{
					cTIME = CTime( sINVENITEM.sItemCustom.tDISGUISE );
					CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
					cTIME -= tLMT;
				}

				// 코스튬 염색값 옮김. by luxes.
				//
				sITEM_NEW.dwMainColor = sINVENITEM.sItemCustom.dwMainColor;
				sITEM_NEW.dwSubColor = sINVENITEM.sItemCustom.dwSubColor;

				sITEM_NEW.cPeriodExtensionCount = sINVENITEM.sItemCustom.cCostumePeriodExtensionCount;
				sITEM_NEW.SetPeriodExBasic(sINVENITEM.sItemCustom.GetPeriodExDisguise());
				sITEM_NEW.ResetPeriodExDisguise();
				

				// 능력치 옮김
				sITEM_NEW.costumeUser = sINVENITEM.sItemCustom.costumeUser;

				pINVENTIEM->sItemCustom.dwMainColor = 0;
				pINVENTIEM->sItemCustom.dwSubColor = 0;

				// 아이템 생성.
				//
				sITEM_NEW.tBORNTIME = cTIME.GetTime();
				sITEM_NEW.wTurnNum = 1;
				sITEM_NEW.cGenType = EMGEN_BILLING;
				sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
				sITEM_NEW.GenerateBasicStat( false );
				sITEM_NEW.GenerateLinkSkill();
				sITEM_NEW.GenerateAddOption();

				if ( InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, INSERT_INVEN_ITEM_RESET_TIMELMT_ITEM_RESTORE_COSTUME, true ) )
                {
                    const SINVENITEM* pInertItem = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
					if ( pInertItem )
					{
						// 분리된 코스툼 로그.
						// 통계 및 로그 수정
						LogItemExchange( 
							pInertItem->CustomItem(), 
							gamelog::ID_CHAR, 0, 
							gamelog::ID_CHAR, m_CharDbNum, 
							ITEM_ROUTE_CHAR, 
							pInertItem->TurnNum(), 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_RESTORE_CUSTOME_BY_TIMEOUT,
							false );
					}
                }
			}
		}
	}

	// 아이템 슬롯 정보 갱신
	for ( int i = 0; i < SLOT_TSIZE; ++i )
	{
		EMSLOT emSLOT = static_cast< EMSLOT >( i );
		if ( m_PutOnItems[emSLOT].GetNativeID() == NATIVEID_NULL() )
            continue;

		SITEMCUSTOM sCUSTOM = m_PutOnItems[emSLOT];

		SITEM* pITEM = glItemMan.GetItem( sCUSTOM.GetNativeID() );
		if ( !pITEM )
            continue;

		bool bDELETE( false );

		if ( sCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
		{
			CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
			CTime cTIME_LMT( sCUSTOM.tBORNTIME );
			cTIME_LMT += cSPAN + sCUSTOM.GetPeriodExBasic();

			if ( cTIME_CUR > cTIME_LMT )
			{
				//	시간 제한으로 아이템 삭제 로그 남김.
				// 통계 및 로그 수정
				LogItemExchange(
                    sCUSTOM,
                    gamelog::ID_CHAR, m_CharDbNum,
                    gamelog::ID_CHAR, 0,
                    ITEM_ROUTE_DELETE, sCUSTOM.wTurnNum, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,
					false );

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.GetNativeID();
				SendToClient( &NetMsgInvenDelTimeLmt );

				if ( pITEM->BasicType() == ITEM_VEHICLE && sCUSTOM.m_VehicleDbNum != 0 )
				{
					m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, false );

                	// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
					GLVEHICLE* pNewVehicle = new GLVEHICLE;
					AddGameAdoJob(
                        db::DbActionPtr( 
                            new CGetVehicle( 
                                pNewVehicle, 
                                sCUSTOM.m_VehicleDbNum, 
                                m_ClientSlot, 
                                m_CharDbNum, 
                                true ) ) );
				}

				//	아이템 제거.
				// 시간이 다된 탈것은 착용창에서 바로 아이템 삭제해버린다. 
				// 탈것안에 있는 기타 아이템들은 CGetVehicle 돌아오면서 처리된다.
				RELEASE_SLOT_ITEM_SERVER( emSLOT, true );

				//	[자신에게]  아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release( emSLOT );
				SendToClient( &NetMsg_Release );

				// 주변 사람들에게 아이탬 제거.
				GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsgReleaseBrd;
				NetMsgReleaseBrd.dwGaeaID = m_dwGaeaID;
				NetMsgReleaseBrd.emSlot = emSLOT;
				SendMsgViewAround( &NetMsgReleaseBrd );

				bDELETE = true;

				// 코스툼 복구.
				if ( sCUSTOM.nidDISGUISE != SNATIVEID( false ) )
				{
					SITEM* pONE = glItemMan.GetItem( sCUSTOM.nidDISGUISE );
					if ( !pONE )
						continue;

					SITEMCUSTOM sITEM_NEW( sCUSTOM.nidDISGUISE );
					CTime cTIME = CTime::GetCurrentTime();
					if ( sCUSTOM.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
					{
						cTIME = CTime( sCUSTOM.tDISGUISE );
						CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
						cTIME -= tLMT;
					}

					// 연장 기능 전이;
					sITEM_NEW.cPeriodExtensionCount = sCUSTOM.cCostumePeriodExtensionCount;
					sITEM_NEW.SetPeriodExBasic(sCUSTOM.GetPeriodExDisguise());
					sITEM_NEW.ResetPeriodExDisguise();

					// 코스튬 염색값 옮김. by luxes.
					//
					sITEM_NEW.dwMainColor = sCUSTOM.dwMainColor;
					sITEM_NEW.dwSubColor = sCUSTOM.dwSubColor;

					// 능력치 옮김
					sITEM_NEW.costumeUser = sCUSTOM.costumeUser;

					m_PutOnItems[emSLOT].dwMainColor = 0;
					m_PutOnItems[emSLOT].dwSubColor = 0;

					// 아이템 생성.
					//
					sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

					// 복장 착용.
					SLOT_ITEM_SERVER( sITEM_NEW, emSLOT, true );

					// 자신에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
					NetMsgUpdate.m_Slot = emSLOT;
					NetMsgUpdate.m_Item = sITEM_NEW;
                    msgpack::sbuffer SendBufferA;
                    msgpack::pack( SendBufferA, NetMsgUpdate );
					SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBufferA );

					// 주변 사람들에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsgUpdateBrd;
					NetMsgUpdateBrd.m_GaeaID = m_dwGaeaID;
					NetMsgUpdateBrd.m_Slot   = emSLOT;
					NetMsgUpdateBrd.m_Item.Assign( sITEM_NEW );
                    msgpack::sbuffer SendBufferB;
                    msgpack::pack( SendBufferB, NetMsgUpdateBrd );
					SendMsgViewAround( NET_MSG_GCTRL_PUTON_UPDATE_BRD, SendBufferB );

					// 분리된 코스툼 로그.
					// 통계 및 로그 수정
					LogItemExchange(
                        sITEM_NEW, gamelog::ID_CHAR, 0,
                        gamelog::ID_CHAR, m_CharDbNum,
                        ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_RESTORE_CUSTOME_BY_TIMEOUT, 
						false );
				}
			}
		}

		// 코스튬 능력치가 적용 되어 있는 아이템 만료체크
		if ( !bDELETE && VALID_SLOT_ITEM_ALLWEAPON( emSLOT ) && sCUSTOM.IsCSApplied() )
		{
			CTime cTimeLimit( sCUSTOM.costumeUser.getEndTime() );

			bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

			if ( bValidTime )
			{
				if ( cTIME_CUR > cTimeLimit )
				{
					// 능력치 초기화 알림
					GLMSG::SNET_INVEN_COSTUME_STATS_FB netMsgFb;

					netMsgFb.emFB    = ENREQ_CS_FB_NOTICE1;
					netMsgFb.sItemID = sCUSTOM.GetNativeID();
					SendToClient( &netMsgFb );

					// 코스튬에 능력치 초기화
					sCUSTOM.costumeUser.ResetInvest();

					SLOT_ITEM_SERVER( sCUSTOM, emSLOT );			//	실제로 적용.

					// 자신에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
					NetMsgUpdate.m_Slot = emSLOT;
					NetMsgUpdate.m_Item = sCUSTOM;
					msgpack::sbuffer SendBufferA;
					msgpack::pack( SendBufferA, NetMsgUpdate );
					SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBufferA );

					// 로그
					LogItemExchange( sCUSTOM, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_COSTUME_STATE_ERASE, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
				}
			}
		}

		if ( !bDELETE && VALID_SLOT_ITEM_ALLWEAPON( emSLOT ) && sCUSTOM.IsTempEnchant() )
		{
			CTime cTimeLimit( sCUSTOM.sTLGrind.tFireTime );

			bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

			if ( bValidTime )
			{
				if ( cTIME_CUR > cTimeLimit )
				{
					// 일시강화 해제 알림
					GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

					netMsgFb.eResult    = EMREQ_TEMP_ENCHANT_FIRETIME;
					netMsgFb.fireItemID = sCUSTOM.GetNativeID();
					SendToClient( &netMsgFb );

					// 강화복구
					sCUSTOM.TempEnchantRecovery();

					SLOT_ITEM_SERVER( sCUSTOM, emSLOT );			//	실제로 적용.

					// 자신에게 착용아이템 변경.
					GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
					NetMsgUpdate.m_Slot = emSLOT;
					NetMsgUpdate.m_Item = sCUSTOM;
					msgpack::sbuffer SendBufferA;
					msgpack::pack( SendBufferA, NetMsgUpdate );
					SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBufferA );

					// 로그
					LogItemExchange( sCUSTOM, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
				}
			}
		}
		
		// 위에서 처리한 것 같은데, 위에는 시간제한 아이템을 처리하면서 거기에 붙어 있는 코스튬을 새로 만들어준거고
		// 이건 일반아이템에(시간제한이거나, 아직 시간이 남아있는) 붙어있는 코스튬 시간 제한 체크
		if ( !bDELETE && VALID_SLOT_ITEM_ALLWEAPON( emSLOT ) && sCUSTOM.nidDISGUISE != SNATIVEID( false ) )
		{
			if ( sCUSTOM.tDISGUISE != 0 )
			{
				CTime cTIME_LMT( sCUSTOM.tDISGUISE + sCUSTOM.GetPeriodExDisguise() );
				if ( cTIME_LMT.GetYear() != 1970 )
				{
					if ( cTIME_CUR > cTIME_LMT )
					{
						//	시간 제한으로 아이템 삭제 알림.
						GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
						NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.nidDISGUISE;
						SendToClient( &NetMsgInvenDelTimeLmt );

						// 코스툼 정보 리셋.
						sCUSTOM.tDISGUISE = 0;
						sCUSTOM.nidDISGUISE = SNATIVEID( false );
						sCUSTOM.ResetPeriodExDisguise();

						// 코스튬 염색값 지움. by luxes.
						sCUSTOM.dwMainColor = 0;
						sCUSTOM.dwSubColor = 0;

						// 코스튬 능력치 초기화
						sCUSTOM.ResetInvest();

						SLOT_ITEM_SERVER( sCUSTOM, emSLOT );			//	실제로 적용.

						// 자신에게 착용아이템 변경.
						GLMSG::SNETPC_PUTON_UPDATE NetMsgUpdate;
						NetMsgUpdate.m_Slot = emSLOT;
						NetMsgUpdate.m_Item = sCUSTOM;
                        msgpack::sbuffer SendBufferA;
                        msgpack::pack( SendBufferA, NetMsgUpdate );
						SendToClient( NET_MSG_GCTRL_PUTON_UPDATE, SendBufferA );

						// 주변 사람들에게 착용아이템 변경.
						GLMSG::SNETPC_PUTON_UPDATE_BRD NetMsgUpdateBrd;
						NetMsgUpdateBrd.m_GaeaID = m_dwGaeaID;
						NetMsgUpdateBrd.m_Slot   = emSLOT;
						NetMsgUpdateBrd.m_Item.Assign( sCUSTOM );
                        msgpack::sbuffer SendBufferB;
                        msgpack::pack( SendBufferB, NetMsgUpdateBrd );
						SendMsgViewAround( NET_MSG_GCTRL_PUTON_UPDATE_BRD, SendBufferB );
					}
				}
			}
		}
	}

	// 인벤토리 추가라인 유효성 검사.
	//
	GLMSG::SNETPC_STORAGE_STATE NetMsgStorageState;
	for ( WORD i = 0; i < EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
	{
		bool bVALID( true );
		CTime cSTORAGE( m_tSTORAGE[i] );

		if ( m_tSTORAGE[i] == 0 )
			bVALID = false;

		if ( cSTORAGE < cTIME_CUR )
			bVALID = false;

		m_bSTORAGE[i] = bVALID;
		NetMsgStorageState.bVALID[i] = bVALID;
	}
	SendToClient( &NetMsgStorageState );

	bool bPREMIUM( true );
	CTime cPREMIUM( m_tPREMIUM );
	if ( m_tPREMIUM == 0 )
		bPREMIUM = false;
	if ( cPREMIUM < cTIME_CUR )
		bPREMIUM = false;

	m_bPREMIUM = bPREMIUM;

	GLMSG::SNETPC_PREMIUM_STATE NetMsgPremium;
	NetMsgPremium.bPREMIUM = bPREMIUM;
	SendToClient( &NetMsgPremium );

	// 현재 활성화된 인벤 라인 설정.
	//
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	// 창고 아이템 유효성 갱신
	for ( WORD idx = 0; idx < EMSTORAGE_CHANNEL; ++idx )
	{
		const GLInventory::CELL_MAP* pMapItem = m_cStorage[idx].GetItemList();
		GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
		GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();

		vecDELETE.clear();

		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = ( *iter ).second;

			SITEM* pITEM = glItemMan.GetItem( pInvenItem->sItemCustom.GetNativeID() );
			if ( !pITEM )
				continue;

			bool bDELETE( false );

			// 시한부 아이템
			if ( pInvenItem->sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
			{
				CTimeSpan cSPAN(pITEM->sDrugOp.tTIME_LMT);
				CTime cTIME_LMT( pInvenItem->sItemCustom.tBORNTIME );
				cTIME_LMT += cSPAN + pInvenItem->sItemCustom.GetPeriodExBasic();

				if ( cTIME_CUR > cTIME_LMT )
				{
					vecDELETE.push_back( ( *iter ).first );
					bDELETE = true;

					// 팻카드일 경우
					if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.PetDbNum() != 0 )
					{
						// 악세사리를 체크후 팻을 DB에서 삭제해준다.
						GLPetField* pNewPet = new GLPetField( m_pGaeaServer );						
						AddGameAdoJob( 
                            db::DbActionPtr( 
                                new db::PetGet( 
                                    pNewPet, 
                                    pInvenItem->sItemCustom.PetDbNum(), 
                                    m_ClientSlot, 
                                    m_CharDbNum, 
                                    pInvenItem->wPosX, 
                                    pInvenItem->wPosY, 
                                    true ) ) );
					}

					// 탈것일 경우
					if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.m_VehicleDbNum != 0 )
					{
						// 악세사리를 체크후 탈것을 DB에서 삭제해준다.
						GLVEHICLE* pNewVehicle = new GLVEHICLE;
                        AddGameAdoJob( 
                            db::DbActionPtr( 
						        new CGetVehicle( 
                                    pNewVehicle, 
							        pInvenItem->sItemCustom.m_VehicleDbNum, 
							        m_ClientSlot, 
							        m_CharDbNum, 
							        true ) ) );
					}
				}
			}

			// 코스튬 능력치가 적용 되어 있는 아이템 만료체크
			if ( !bDELETE && pInvenItem->sItemCustom.IsCSApplied() )
			{
				CTime cTimeLimit( pInvenItem->sItemCustom.costumeUser.getEndTime() );

				bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

				if ( bValidTime )
				{
					if ( cTIME_CUR > cTimeLimit )
					{
						// 능력치 초기화 알림
						GLMSG::SNET_INVEN_COSTUME_STATS_FB netMsgFb;

						netMsgFb.emFB    = ENREQ_CS_FB_NOTICE1;
						netMsgFb.sItemID = pInvenItem->sItemCustom.GetNativeID();
						SendToClient( &netMsgFb );

						// 코스튬에 능력치 초기화
						pInvenItem->sItemCustom.costumeUser.ResetInvest();

						GLMSG::SNETPC_STORAGE_ITEM_UPDATE NetItemUpdate;
						NetItemUpdate.dwChannel = idx;
						NetItemUpdate.wPosX = pInvenItem->wPosX;
						NetItemUpdate.wPosY = pInvenItem->wPosY;
						NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
						SendToClient( &NetItemUpdate );

						// 로그
						LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_COSTUME_STATE_ERASE, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
					}
				}
			}

			if ( !bDELETE && pInvenItem->sItemCustom.IsTempEnchant() )
			{
				CTime cTimeLimit( pInvenItem->sItemCustom.sTLGrind.tFireTime );

				bValidTime = m_pGaeaServer->IsValidTime( cTimeLimit );

				if ( bValidTime )
				{
					if ( cTIME_CUR > cTimeLimit )
					{
						// 만료 알림
						GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

						netMsgFb.eResult    = EMREQ_TEMP_ENCHANT_FIRETIME;
						netMsgFb.fireItemID = pInvenItem->sItemCustom.GetNativeID();
						SendToClient( &netMsgFb );

						// 강화복구
						pInvenItem->sItemCustom.TempEnchantRecovery();

						GLMSG::SNETPC_STORAGE_ITEM_UPDATE NetItemUpdate;
						NetItemUpdate.dwChannel = idx;
						NetItemUpdate.wPosX = pInvenItem->wPosX;
						NetItemUpdate.wPosY = pInvenItem->wPosY;
						NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
						SendToClient( &NetItemUpdate );

						// 로그
						LogItemExchange( pInvenItem->sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
					}
				}
			}
			
			// 코스튬
			if ( !bDELETE && pInvenItem->sItemCustom.nidDISGUISE != SNATIVEID( false ) )
			{
				if ( pInvenItem->sItemCustom.tDISGUISE != 0 )
				{
					CTime cTIME_LMT( pInvenItem->sItemCustom.tDISGUISE + pInvenItem->sItemCustom.GetPeriodExDisguise());

					bValidTime = m_pGaeaServer->IsValidTime( cTIME_LMT );
					if ( bValidTime )
					{
						if ( cTIME_CUR > cTIME_LMT )
						{
							//	시간 제한으로 아이템 삭제 알림.
							GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
							NetMsgInvenDelTimeLmt.nidITEM = pInvenItem->sItemCustom.nidDISGUISE;
							SendToClient( &NetMsgInvenDelTimeLmt );

							// 코스툼 정보 리셋.
							pInvenItem->sItemCustom.tDISGUISE = 0;
							pInvenItem->sItemCustom.nidDISGUISE = SNATIVEID( false );
							pInvenItem->sItemCustom.ResetPeriodExDisguise();

							// 코스튬 염색값 지움. by luxes.
							//
							pInvenItem->sItemCustom.dwMainColor = 0;
							pInvenItem->sItemCustom.dwSubColor = 0;

							// 코스튬 능력치 초기화
							pInvenItem->sItemCustom.ResetInvest();

							// 클라이언트에 알림.
							GLMSG::SNETPC_STORAGE_ITEM_UPDATE NetItemUpdate;
							NetItemUpdate.dwChannel = idx;
							NetItemUpdate.wPosX = pInvenItem->wPosX;
							NetItemUpdate.wPosY = pInvenItem->wPosY;
							NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
							SendToClient( &NetItemUpdate );

							// 로그 저장.
							LogItemConversion( pInvenItem->sItemCustom, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_SYSTEM, LOG_ITEM_COSTUME_DEL_BY_TIMEOUT );
						}
					}
					else
					{
						sc::writeLogError( sc::string::format( "GLChar::RESET_TIMELMT_ITEM() - Invalid CTime is set for an item(%1%, %2%) of Char(%3%)."
							, pInvenItem->sItemCustom.Mid(), pInvenItem->sItemCustom.Sid(), CharDbNum() ) );
					}
				}
			}
		}

		if ( !vecDELETE.empty() )
		{
			for ( DWORD i = 0; i< vecDELETE.size(); ++i )
			{
				const GLInventory::CELL_KEY &cKEY = vecDELETE[i];
				SINVENITEM sINVENITEM = *m_cStorage[idx].GetItem( cKEY.first,cKEY.second );
				SINVENITEM* pINVENITEM = m_cStorage[idx].GetItem( cKEY.first,cKEY.second );

				SNATIVEID nidITEM = sINVENITEM.sItemCustom.GetNativeID();

				// 시간 제한으로 아이템 삭제 로그 남김.
				// 통계 및 로그 수정
				LogItemExchange(
                    sINVENITEM.sItemCustom, 
                    gamelog::ID_CHAR, m_CharDbNum, 
                    gamelog::ID_CHAR, 0, 
                    ITEM_ROUTE_DELETE, sINVENITEM.sItemCustom.wTurnNum, 
					( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,
					false );

				// 아이템 삭제.
				AddGameAdoJob(
					db::DbActionPtr( 
					new db::ItemDelete( sINVENITEM.GetGuid(), sINVENITEM.GetDbState(), DELETE_USER_LOCKER_ITEM_TIMELIMIT ) ) );

				m_cStorage[idx].DeleteItem( cKEY.first, cKEY.second );

				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNETPC_STORAGE_DELETE NetMsgStorageDel;
				NetMsgStorageDel.dwChannel = idx;
				NetMsgStorageDel.wPosX = cKEY.first;
				NetMsgStorageDel.wPosY = cKEY.second;
				SendToClient( &NetMsgStorageDel );

				//	시간 제한으로 아이템 삭제 알림.
				GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
				NetMsgInvenDelTimeLmt.nidITEM = nidITEM;
				SendToClient( &NetMsgInvenDelTimeLmt );

				// 코스툼 복구.
				if ( sINVENITEM.sItemCustom.nidDISGUISE != SNATIVEID( false ) )
				{
					SITEM* pONE = glItemMan.GetItem( sINVENITEM.sItemCustom.nidDISGUISE );
					if ( !pONE )
						continue;

					//	인벤 위치.
					WORD wInsertPosX = cKEY.first, wInsertPosY = cKEY.second;

					SITEMCUSTOM sITEM_NEW( sINVENITEM.sItemCustom.nidDISGUISE );
					CTime cTIME = CTime::GetCurrentTime();
					if ( sINVENITEM.sItemCustom.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
					{
						cTIME = CTime( sINVENITEM.sItemCustom.tDISGUISE );

						CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
						cTIME -= tLMT;
					}

					// 연장 기능 전이;
					sITEM_NEW.cPeriodExtensionCount = sINVENITEM.sItemCustom.cCostumePeriodExtensionCount;
					sITEM_NEW.SetPeriodExBasic(sINVENITEM.sItemCustom.GetPeriodExDisguise());
					sITEM_NEW.ResetPeriodExDisguise();

					// 코스튬 염색값 옮김. by luxes.
					//
					sITEM_NEW.dwMainColor = sINVENITEM.sItemCustom.dwMainColor;
					sITEM_NEW.dwSubColor = sINVENITEM.sItemCustom.dwSubColor;

					// 코스튬 능력치 옮김
					sITEM_NEW.costumeUser = sINVENITEM.sItemCustom.costumeUser;

					pINVENITEM->sItemCustom.dwMainColor = 0;
					pINVENITEM->sItemCustom.dwSubColor = 0;

					// 아이템 생성.
					//
					sITEM_NEW.tBORNTIME = cTIME.GetTime();
					sITEM_NEW.wTurnNum = 1;
					sITEM_NEW.cGenType = EMGEN_BILLING;
					sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

					// 인벤에 들어갈 공간 검사.
					BOOL bOk = m_cStorage[idx].IsInsertable ( wInsertPosX, wInsertPosY );
					if ( !bOk )
						continue;	// 여기로 오면 코스튬 그냥 없어짐?

					// 인벤에 넣음.
					if ( !m_cStorage[idx].InsertItem( sITEM_NEW, wInsertPosX, wInsertPosY ) )
					{
						continue;
					}

					SINVENITEM* pINSERT_ITEM = m_cStorage[idx].GetItem( wInsertPosX, wInsertPosY );
					if ( !pINSERT_ITEM )
					{
						continue;
					}

					AddGameAdoJob(
						db::DbActionPtr( 
						new db::ItemInsert( m_dwUserID, INVEN_LOCKER, pINSERT_ITEM->sSaveData, INSERT_USER_LOCKER_ITEM_TIMELIMIT_ITEMDELETE_AND_RESTORE_COSTUME ) ) );
					pINSERT_ITEM->SetDbState( db::DB_UPDATE );

					GLMSG::SNETPC_STORAGE_INSERT NetItemInsert;
					NetItemInsert.dwChannel = idx;
					NetItemInsert.Data = *pINSERT_ITEM;
					SendToClient( &NetItemInsert );

					// 분리된 코스툼 로그.
					// 통계 및 로그 수정
					LogItemExchange( 
                        pINSERT_ITEM->sItemCustom, 
                        gamelog::ID_CHAR, 0, 
                        gamelog::ID_CHAR, m_CharDbNum, 
                        ITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum, 
						( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_RESTORE_CUSTOME_BY_TIMEOUT,
						false );
				}
			}
		}
	}
}

// *****************************************************
// Desc: 아이템 조건 체크 ( 시간제한과 같이 작동 ) 
// *****************************************************
void GLChar::RESET_CHECK_ITEM()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( !pLand )
	{
		return;
	}

	// 아이템 슬롯 착용 조건을 체크한다.
	for ( int i = 0; i < SLOT_HOLD; ++i )
	{
		EMSLOT emSLOT = static_cast< EMSLOT >( i );
		
		if ( m_PutOnItems[emSLOT].GetNativeID() == NATIVEID_NULL() )
			continue;

		SITEMCUSTOM sCUSTOM = m_PutOnItems[emSLOT];

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sCUSTOM.GetNativeID() );
		if ( !pITEM )
			continue;

		bool bRelease( false );

		// 비코스튬
		if ( sCUSTOM.nidDISGUISE == NATIVEID_NULL() )
		{
			if ( !SIMPLE_CHECK_ITEM( sCUSTOM ) )
				bRelease = true;
		}
		// 코스튬
		else 
		{
			if ( !SIMPLE_CHECK_ITEM ( sCUSTOM, sCUSTOM.nidDISGUISE ) )
				bRelease = true;
		}

		if ( bRelease ) 
		{
			EMSLOT emRHand = GetCurRHand();
			EMSLOT emLHand = GetCurLHand();

			if ( emSLOT == emRHand ) 
			{
				SITEM* pITEM_LEFT = NULL, *pITEM_RIGHT = NULL;
				if ( VALID_SLOT_ITEM( emLHand ) )
                    pITEM_LEFT = GLogicData::GetInstance().GetItem( GET_SLOT_ITEM( emLHand ).GetNativeID() );
				if ( VALID_SLOT_ITEM( emRHand ) )
                    pITEM_RIGHT = GLogicData::GetInstance().GetItem( GET_SLOT_ITEM( emRHand ).GetNativeID() );

				if ( pITEM_RIGHT && ( ( pITEM_RIGHT->sSuitOp.emAttack == ITEMATT_BOW ) || ( pITEM_RIGHT->sSuitOp.emAttack == ITEMATT_SPEAR ) ) )
				{
					if ( pITEM_LEFT )	//	화살, 부적 빼내야 함.
					{
						if ( m_PutOnItems[emLHand].GetNativeID() != NATIVEID_NULL() )
						{
							CItemDrop cDropItem;
							cDropItem.sItemCustom = m_PutOnItems[emLHand];

							if ( IsInsertToInven( &cDropItem, true ) )
							{
								InsertToInven( &cDropItem, false, false, false, true );
							}
							else
							{
								// 바닥에 떨어뜨린다.
								// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
								// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
								// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
								// 누군가 줍는다면 자기 아이템으로 업데이트된다.
								DropItem( cDropItem.sItemCustom, m_vPosition );
							}

							//	왼쪽 슬롯 지워줌.
							RELEASE_SLOT_ITEM_SERVER( emLHand );

							//	[자신에게] SLOT에 있었던 아이탬 제거.
							GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release( emLHand );
							SendToClient( &NetMsg_PutOn_Release );
						}
					}
				}
			}
			
			// 탈것 슬롯일경우 슬롯의 아이템 정리
			if ( emSLOT == SLOT_VEHICLE &&  m_pVehicle->IsActiveValue() )
			{
				for ( int loop = 0; loop < ACCE_TYPE_SIZE; ++loop )
				{
					if ( m_pVehicle->m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
                        continue;
					
					// 인벤 삽입
					CItemDrop cDropItem;
					cDropItem.sItemCustom = m_pVehicle->m_PutOnItems[loop];
					
					if ( IsInsertToInven( &cDropItem, true ) )
					{
						InsertToInven( &cDropItem, false, false, false, true );
					}
					else
					{
						// 바닥에 떨어뜨린다.
						// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
						// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
						// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
						// 누군가 줍는다면 자기 아이템으로 업데이트된다.
						DropItem( cDropItem.sItemCustom, m_vPosition );
					}

					m_pVehicle->m_PutOnItems[loop] = SITEMCUSTOM( NATIVEID_NULL() );
					
					// 탈것 릴리즈 
					//	[자신에게] 해당 아이탬을 인밴에서 삭제.
					GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
					NetMsg.m_VehicleDbNum	= m_pVehicle->m_DbNum;
					NetMsg.accetype = ( VEHICLE_ACCESSORYTYPE )loop;
					SendToClient( &NetMsg );

					// 주변에 알림.
					GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
					NetMsgBrd.dwGaeaID = m_dwGaeaID;
					NetMsgBrd.m_VehicleDbNum = m_pVehicle->m_DbNum;
					NetMsgBrd.accetype = ( VEHICLE_ACCESSORYTYPE )loop;
					SendMsgViewAround( &NetMsgBrd );
				}

				m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, false );
			}
			// 탈것 정보가 활성화 되지 않았을경우에는 제거 하지 않는다.
			else if ( emSLOT == SLOT_VEHICLE &&  !m_pVehicle->IsActiveValue() )
			{
				continue;
			}

			//	[자신에게] SLOT에 있었던 아이탬 제거.
			GLMSG::SNETPC_PUTON_RELEASE NetMsg_PutOn_Release( emSLOT );
			SendToClient( &NetMsg_PutOn_Release );

			//	[모두에게] 자신의 착용 복장이 바뀜을 알림.
			GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsg_PutOn_Release_Brd;
			NetMsg_PutOn_Release_Brd.dwGaeaID = m_dwGaeaID;
			NetMsg_PutOn_Release_Brd.emSlot = emSLOT;
			SendMsgViewAround( &NetMsg_PutOn_Release_Brd );	

			// 인벤에 삽입
			CItemDrop cDropItem;
			cDropItem.sItemCustom = m_PutOnItems[emSLOT];

			if ( IsInsertToInven( &cDropItem, true ) )
			{
				InsertToInven( &cDropItem, false, false, false, true );
			}
			else
			{
				// 바닥에 떨어뜨린다.
				// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
				// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
				// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
				// 누군가 줍는다면 자기 아이템으로 업데이트된다.
				DropItem( cDropItem.sItemCustom, m_vPosition );
			}

			RELEASE_SLOT_ITEM_SERVER( emSLOT );			
		}
	}

	// 탈것 악세서리 조건 체크
	if ( m_pVehicle->IsActiveValue() )
	{
		for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
		{
			if ( m_pVehicle->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
				continue;
			
            if ( !SIMPLE_CHECK_ITEM( m_pVehicle->m_PutOnItems[i] ) )
			{
				// 인벤 삽입
				CItemDrop cDropItem;
				cDropItem.sItemCustom = m_pVehicle->m_PutOnItems[i];
				
				if ( IsInsertToInven( &cDropItem, true ) )
				{
					InsertToInven( &cDropItem, false, false, false, true );
				}
				else
				{
					// 바닥에 떨어뜨린다.
					// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
					// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
					// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
					// 누군가 줍는다면 자기 아이템으로 업데이트된다.
					DropItem( cDropItem.sItemCustom, m_vPosition );
				}

				m_pVehicle->m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
				
				// 탈것 릴리즈 
				//	[자신에게] 해당 아이탬을 인밴에서 삭제.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
				NetMsg.m_VehicleDbNum	= m_pVehicle->m_DbNum;
				NetMsg.accetype = ( VEHICLE_ACCESSORYTYPE )i;
				SendToClient( &NetMsg );

				// 주변에 알림.
				GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
				NetMsgBrd.dwGaeaID = m_dwGaeaID;
				NetMsgBrd.m_VehicleDbNum = m_pVehicle->m_DbNum;
				NetMsgBrd.accetype = ( VEHICLE_ACCESSORYTYPE )i;
				SendMsgViewAround( &NetMsgBrd );
			}
		}
	}
}

const bool GLChar::ConfirmItemValidation ()
{
	// 캐릭터 장비 슬롯 아이템 유효성 검사;
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		EMSLOT emSLOT = static_cast< EMSLOT >( i );
		if ( NATIVEID_NULL() == m_PutOnItems[ emSLOT ].GetNativeID() )
			continue;

		// 하나라도 유효하지 않으면 유효하지 않다;
		SITEMCUSTOM sSlotItem = m_PutOnItems[ emSLOT ];
		if ( true == sSlotItem.IsOpItem() )
			return false;
	}

	return true;
}

void GLChar::TurnAction(const EMACTIONTYPE toAction, const bool bMsg)
{
	const EMACTIONTYPE& emAction = GetAction();
	// 이전 액션 취소.
	switch ( emAction )
	{
	case GLAT_ATTACK:
	case GLAT_SKILL:
		{
			m_fIdleTime = 0.0f;
		}
		break;

	case GLAT_MOTION:
		{
			if( m_fnOnMotionFinish )
			{
				FunctionVoid fnOnMotionFinish = m_fnOnMotionFinish;
				m_fnOnMotionFinish = NULL;
				fnOnMotionFinish();
			}
		}
		break;

    // falling 과 Die일 경우 현재 죽어있는 상태라면 Action 전환을 금지;
    case GLAT_FALLING:
        if ( toAction == GLAT_DIE )
            break;
    case GLAT_DIE:
        {
            if ( IsActState(EM_ACT_DIE) )
                return;
        }
        break;
	case GLAT_FEIGN_DIE :
	case GLAT_FEIGN_FALLING :
		{
			if( toAction == GLAT_FALLING ||
				toAction == GLAT_DIE )
			{
				ReSetActState(EM_ACT_FEIGN_DIE);
			}
			else
			if( toAction == GLAT_SHOCK )
			{
				return;
			}
		}
		break;	
	}; //switch

	// 액션 초기화.
	m_fIdleTime = 0.0f;

	if ( IsActState(EM_ACT_OBSERVE) == true )
	{
		switch ( toAction )
		{
		case GLAT_IDLE:
		case GLAT_MOVE:
			SetAction(toAction);
			break;
		default:
			SetAction(GLAT_IDLE);
			break;
		}		
		return;
	}

	SetAction(toAction);

    switch ( toAction )
	{
	case GLAT_IDLE:
	case GLAT_MOVE:
	case GLAT_PUSHPULL:
		if( m_emVehicle != EMVEHICLE_DRIVER )
			break;

		for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
		{
			DWORD dwGaeaID = m_pVehicle->PassengerId(i);
			if (dwGaeaID == GAEAID_NULL)
				continue;

			GLChar* pChar = GetChar(dwGaeaID);
			if ( pChar )
				pChar->TurnAction( toAction );
		} //for
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		m_fattTIMER = 0.0f;

		SendToChildCrowAttack();
		break;

	case GLAT_SHOCK:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_DIE );
		break;
	case GLAT_FEIGN_FALLING:
		TurnAction ( GLAT_FEIGN_DIE );
		break;
	}; //switch
} //GLChar::TurnAction

void GLChar::SetAction ( const EMACTIONTYPE emAction )
{
	TriggerSystem::SServerActionMessage sTriggerMsg;
	sTriggerMsg.dwCharDbNum = GetCharID();
	sTriggerMsg.emCurrentAction = GetAction();
	sTriggerMsg.emNextAction = emAction;
	SendTriggerMessage( &sTriggerMsg );

	ServerActor::SetAction( emAction );
}

/**
 * 두 캐릭터간에 전투중인 상태처리
 *
 * @param dwAttackedCharID				[in] a first attacker's DB id.
 * @param bClubBattle					[in] is this club battle.
 * @param fTime							[in] how long time.
 */
void GLChar::AddPlayHostileEachOther( const DWORD dwAttackedCharID, const BOOL bClubBattle, const float fTime )
{
	GLChar *pCHAR = GetCharByDbNum( dwAttackedCharID );
	if( !pCHAR )
		return;

	pCHAR->AddPlayHostile( m_CharDbNum, TRUE, bClubBattle, fTime );				// 때린 넘.
	AddPlayHostile( pCHAR->m_CharDbNum, FALSE, bClubBattle, fTime );			// 맞은 넘.
} //GLChar::AddPlayHostileEachOther

void GLChar::AddPlayHostile( const DWORD dwCHARID, const BOOL bBAD, const BOOL bClubBattle, const float fTime )
{
	GLChar* pCHAR = GetCharByDbNum(dwCHARID);
	if(!pCHAR)
        return;

	if( pCHAR->IsActState( EM_ACT_DIE ) )
		return;

	if( IsActState( EM_ACT_DIE ) )
		return;

	ADD_PLAYHOSTILE ( dwCHARID, bBAD, fTime );

	// 적대자 등록 알림.
	GLMSG::SNETPC_PLAYERKILLING_ADD		NetMsgAdd;
	NetMsgAdd.dwCharID		= dwCHARID;
	NetMsgAdd.wSchoolID		= pCHAR->m_wSchool;
	NetMsgAdd.bBAD			= bBAD;
	NetMsgAdd.bClubBattle	= bClubBattle;
	NetMsgAdd.fTime			= fTime;

	StringCchCopy( NetMsgAdd.szName, CHAR_SZNAME, pCHAR->m_szName );

	SendToClient( &NetMsgAdd );
}

// *****************************************************
// Desc: 대련중인 대상인지 Check
// *****************************************************
bool GLChar::IsConflictTarget ( ServerActor* pTARGET )
{
	//GASSERT(pTARGET&&"GLChar::IsConflictTarget()");
	if ( !pTARGET )
		return false;

	EMCROW emCROW = pTARGET->GetCrow();
	if ( emCROW!=CROW_PC )
		return false;

	SCONFTING* pCONFTING = pTARGET->GETCONFTING();
	if ( !pCONFTING )
		return false;

	if ( m_sCONFTING.IsFIGHTING() && pCONFTING->IsFIGHTING() )
	{
		if ( m_sCONFTING.emTYPE!=pCONFTING->emTYPE )
			return false;

		switch ( pCONFTING->emTYPE )
		{
		case EMCONFT_ONE:
			return ( m_dwGaeaID==pCONFTING->dwTAR_ID );

		case EMCONFT_PARTY:
			return ( m_PartyID == pCONFTING->dwTAR_ID );

		case EMCONFT_GUILD:
			return ( m_ClubDbNum==pCONFTING->dwTAR_ID );
		};
	}

	return false;
}

// *****************************************************
// Desc: 액션을 취할 수 있는 대상인지 check
// *****************************************************
// EMIMPACT_SIDE_PARTY   : 아군에게 => 파티원에게;
// EMIMPACT_SIDE_ENEMY   : 적에게   => 적대적 대상에게;
// EMIMPACT_SIDE_ANYBODY : 모두에게 => 플레이어에게;
// 
// 대상을 구분하는 방법 자체가 심각한 문제가 있다.;
// 이 함수는 나중에 스킬타겟 시스템 개편과 함께 반드시 바뀌어야 한다.;
// 액션 상대에 대한 검사는 적대적과 비적대적이 이 두가지로만으로 구분해서는 안된다!;
// 소환수에도 동일한 함수가 있지만 변경하지는 않았다. ;
// 소환수는 적,아군 만을 구분하는걸로 아직은 충분하고.;
// 만약 소환수도 변경해야 한다면 전반적으로 다시 만드는 쪽이 깔끔하고 좋을 듯;
const EMREACTIONABLE GLChar::IsReActionable( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return EMREACTIONABLE_FALSE;

	if ( !pTARGET )
		return EMREACTIONABLE_FALSE;
	if ( pTARGET->IsNonTargetable() )
		return EMREACTIONABLE_FALSE;

	if ( FALSE == pTARGET->IsVisibleDetect( this ) )
		return EMREACTIONABLE_FALSE;

    const Faction::ManagerField* const _pFactionMan(pLand->GetFactionManager());
    if ( _pFactionMan )
    {
        return IsReActionableFaction(pTARGET,_emTargetSide);
    }
    else
    {
        return IsReActionableNonFaction( pTARGET, _emTargetSide);
    }
}

const EMREACTIONABLE GLChar::IsReActionableFaction( ServerActor* pTARGET, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || pTARGET == NULL )
		return EMREACTIONABLE_FALSE;

	// Note : 타겟이 소환수라면 대상 타겟검사를 소환수의 주인으로 바꾼다
	//
	if ( pTARGET->GetCrow() == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = pLand->GetSummon( pTARGET->GetGaeaID() );

		if ( !pSummon )
		{
			return EMREACTIONABLE_FALSE;
		}

		pTARGET = GetTarget( pLand,
			STARGETID(
			pSummon->m_sOwnerID.actorType,
			pSummon->m_dwOwnerGaeaID ) );

		if( !pTARGET )
		{
			return EMREACTIONABLE_FALSE;
		}
	}

    Faction::ManagerField* const _pFactionMan = pLand->GetFactionManager();
    if ( _pFactionMan == NULL )
        return EMREACTIONABLE_FALSE;

    const Faction::EMRELATION_TYPE _relation = _pFactionMan->getRelationField(this, pTARGET);
    if ( _relation == Faction::EMRELATION_ERROR )
        return GLChar::IsReActionableNonFaction( pTARGET, _emTargetSide );

    switch ( _emTargetSide )
    {
    case EMIMPACT_SIDE_ENEMY:
        if (_relation == Faction::EMRELATION_ENEMY )
        {
            return EMREACTIONABLE_TRUE;
        }

        //	대련중에는 대련자 공격 가능.;
        if ( m_sCONFTING.IsCONFRONTING() )
        {
            return EMREACTIONABLE_TRUE;
        }            

        // 적대행위자 공격 가능;
        if ( ( pTARGET->GetCrow() == CROW_PC ) && ( IS_PLAYHOSTILE ( pTARGET->GetCharID() ) ) )
        {
            return EMREACTIONABLE_TRUE;
        }

        // 중립적인 대상에게 적대액션를 한다는 뜻은 강제액션이라는 걸로 판단;
        if ( _relation == Faction::EMRELATION_NEUTRAL_ENEMY )
        {
            return EMREACTIONABLE_FORCED_TRUE;
        }
        break;

		// 같은 편에게 쓰는 스킬의 경우 Faction이 추가되어 있다면;
		// Faction에게 사용하도록 한다;
    case EMIMPACT_SIDE_PARTY:
	case EMIMPACT_SIDE_OUR:
        {
			// 팩션으로 판별한다;
			if ( _relation == Faction::EMRELATION_ALLY )
				return EMREACTIONABLE_TRUE;

			return EMREACTIONABLE_FALSE;

			// 파티로 판별하지 않는다;
			/*if ( pTARGET->GetGaeaID() == this->GetGaeaID() )
				return EMREACTIONABLE_TRUE;

            GLPartyField* const pMyParty = GetMyParty();					
            if ( pMyParty == NULL )
                return EMREACTIONABLE_FALSE;

            if ( pMyParty->isMemberByGaeaID( pTARGET->GetGaeaID() ) )
                return EMREACTIONABLE_TRUE;*/
        }
        break;

    case EMIMPACT_SIDE_ANYBODY:
        {
            //코드가 자신을 속일지라도 헷갈리지 말자.. EMIMPACT_SIDE_ANYBODY는 모든대상이 아니다.. 플레이어와 NPC만이다....;
            if ( ( pTARGET->GetCrow() == CROW_PC ) || ( pTARGET->GetCrow() == CROW_NPC ) )
                return EMREACTIONABLE_TRUE;
        }
        break;

    default:
        break;
    }

    return EMREACTIONABLE_FALSE;
}
const EMREACTIONABLE GLChar::IsReActionableNonFaction( ServerActor* pTARGET, const EMIMPACT_SIDE bTargetSide )
{
	GASSERT(pTARGET&&"GLChar::IsReActionable()");

	if ( !pTARGET )
		return EMREACTIONABLE_FALSE;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return EMREACTIONABLE_FALSE;

	// Note : 타겟이 소환수라면 대상 타겟검사를 소환수의 주인으로 바꾼다
	//
	if ( pTARGET->GetCrow() == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = pLand->GetSummon( pTARGET->GetGaeaID() );

		if ( !pSummon )
		{
			return EMREACTIONABLE_FALSE;
		}

		pTARGET = GetTarget( pLand,
			STARGETID(
			pSummon->m_sOwnerID.actorType,
			pSummon->m_dwOwnerGaeaID ) );
		
		if( !pTARGET )
		{
			return EMREACTIONABLE_FALSE;
		}
	}

	// 성향이벤트 중인지
	bool bBRIGHTEVENT	= IsBRIGHTEVENT();
	bool bPKServer		= IsPKServer();

	EMCROW emCROW       = pTARGET->GetCrow();
	bool bPKZone        = pLand->IsPKZone();
	bool bGuidBattleMap = pLand->m_bGuidBattleMap;
	bool bClubDeathMatch= pLand->m_bClubDeathMatchMap;
	bool bSCHOOL_FREEPK = SchoolFreePkOn();
	bool bClubBattleZone= pLand->IsClubBattleZone();
	bool bPKMap			= pLand->IsPKMap();

	bool bCTF			= (IsCTFPlayer() == TRUE) ? true : false; //(m_pGLGaeaServer->IsCTFPvPMap(pLand->GetMapID().dwID) && m_pGLGaeaServer->IsCTFPlaying());

	if ( GetSchool() == pTARGET->GetSchool() )
		bSCHOOL_FREEPK = false;

	bool bREACTION = true;

    switch ( bTargetSide )
    {
    case EMIMPACT_SIDE_PARTY:
	case EMIMPACT_SIDE_OUR:
        {
            switch ( emCROW )
            {
            case CROW_PC:
                {
                    //	pc 에게 비공격 마법 같은 것은 대련자가 아닐 때만 가능.
                    bREACTION = !IsConflictTarget ( pTARGET );

                    //	상대방이 대련중일 때는 자기가 대련중이고 같은 편일때 가능.
                    SCONFTING* pTAR_CONFT = pTARGET->GETCONFTING();

                    if ( pTAR_CONFT->IsCONFRONTING() )
                    {
                        bREACTION = m_sCONFTING.IsSAMENESS(pTAR_CONFT->emTYPE,pTAR_CONFT->dwTAR_ID);
                    }

					GLPartyField* const pMyParty = GetMyParty();
					if ( pMyParty )
						bREACTION = pMyParty->isMemberByGaeaID( pTARGET->GetGaeaID() );
					else
						bREACTION = false;

					if ( GetGaeaID() == pTARGET->GetGaeaID() )
						bREACTION = true;

                    bool bHOSTILE = IS_PLAYHOSTILE ( pTARGET->GetCharID() );

                    if ( bHOSTILE )
                        bREACTION = false;

					if ( bREACTION )
					{
						if ( GetPartyID() != pTARGET->GetPartyID() &&
							GetCharID() != pTARGET->GetCharID())
						{
							bREACTION = false;
							sc::writeLogInfo( "Invalid Reaction Target!!!!!!" );
						}
					}
                }
                break;

            case CROW_SUMMON:
            case CROW_MOB:
                bREACTION = false;
                break;

            case CROW_NPC:
                bREACTION = !m_sCONFTING.IsCONFRONTING();
                break;

            case CROW_MATERIAL:
                bREACTION = true;
                break;

                //case CROW_PET:	break;	// PetData

            default:
                bREACTION = false;
                break;
            }
        }
        break;
    case EMIMPACT_SIDE_ENEMY:
        {
            // 공격 불가능일 경우
            if ( pTARGET->IsNonDamaged() )
                return EMREACTIONABLE_FALSE;


            switch ( emCROW )
            {
            case CROW_PC:
                {
                    GLChar* pChar = GetChar( pTARGET->GetGaeaID() );

                    // 자신을 공격할수 없다
                    if ( !pChar || pChar == this )
                    {
                        return EMREACTIONABLE_FALSE;
                    }

                    // Absolute Non-PK State Check.
                    if ( GLCONST_CHAR::bPKLESS )
                        return EMREACTIONABLE_FALSE;

                    // Desc : 절대 안전한 시간 (10초)
                    if ( !pTARGET->IsSafeTime() && !IsSafeTime() && !pTARGET->IsSafeZone() && !IsSafeZone() )
                    {
                        const DWORD dwTargetGaeaID = pTARGET->GetGaeaID();
                        // 같은 파티의 멤버인지
                        GLPartyField* const pMyParty = GetMyParty();					
                        if ( pMyParty != NULL )
                        {
                            if ( pMyParty->isMemberByGaeaID(pTARGET->GetGaeaID()))
                                return EMREACTIONABLE_FALSE;
                        }					

                        // 서로 대련중인지
                        bREACTION = IsConflictTarget ( pTARGET );

                        if ( !bREACTION )
                        {
                            SCONFTING* pCONFTING = pTARGET->GETCONFTING();

                            if ( m_sCONFTING.IsCONFRONTING() || pCONFTING->IsCONFRONTING() )
                            {
                                return EMREACTIONABLE_FALSE;
                            }
                        }
                        else
                        {
                            return EMREACTIONABLE_TRUE;
                        }

                        if (!bPKZone || !GLCONST_CHAR::bPK_MODE)
                            return EMREACTIONABLE_FALSE;

	                       //
                        //mjeon.CaptureTheField
                        //
                        if (bCTF)
                        {
                            //이 캐릭터가 CTF PvP가 진행 중일 때, CTF PvP맵상에서 해당 행위를 할 수 있는지 판단.
                            //이 때, 캐릭터는 반드시 CTF Player이어야 하나, 위의 사실만으로 이를 판단할 수는 없다.
                            //이 사실은 CTF Mananger에서 별도로 체크될 것이다.

                            if (m_wSchool == pTARGET->GetSchool())
                            {
                                return EMREACTIONABLE_FALSE;
                            }
                            else
                            {
                                return EMREACTIONABLE_TRUE;
                            }
                        }

                        std::tr1::shared_ptr<GLClubField> pMyClub  = GetMyClub();
                        std::tr1::shared_ptr<GLClubField> pTarClub = GetClub(pChar->m_ClubDbNum);

                        // 선도전에서 같은 동맹 클럽이면 공격 불가능
                        if ( bGuidBattleMap )
                        {
                            if ( pMyClub && pTarClub )
                            {
                                BOOL bAlliance = pMyClub->IsAllianceGuild_COMMON ( pTarClub->m_DbNum, pTarClub->m_dwAlliance );
                                if (bAlliance)
                                    return EMREACTIONABLE_FALSE;
                                else
                                    return EMREACTIONABLE_TRUE;
                            }
                        }
                        else if (bClubDeathMatch)
                        {
                            if (m_pGaeaServer->IsClubDeathMatch(pLand->m_dwClubMapID))
                            {
                                if (m_ClubDbNum == pChar->m_ClubDbNum)
                                    return EMREACTIONABLE_FALSE;
                                else
                                    return EMREACTIONABLE_TRUE;
                            }
                        }
                        else if ( bPKMap ) 
                        {
                            if ( m_ClubDbNum != 0 && m_ClubDbNum == pChar->m_ClubDbNum )
                                return EMREACTIONABLE_FALSE;
                            else
                                return EMREACTIONABLE_TRUE;
                        }
                        else if ( bClubBattleZone )
                        {
                            if ( pMyClub && pTarClub )
                            {
                                if ( pMyClub->IsBattle( pTarClub->m_DbNum ) )
                                    return EMREACTIONABLE_TRUE;

                                if ( pMyClub->IsBattleAlliance( pTarClub->m_dwAlliance ) )
                                    return EMREACTIONABLE_TRUE;
                            }
                        }

                        // 적대관계인지 체크
                        DWORD dwCHARID = pTARGET->GetCharID();

                        bREACTION = IS_PLAYHOSTILE ( dwCHARID );

                        if ( bREACTION )
                            return EMREACTIONABLE_TRUE;

                        // 20 Lev 이상일때
                        if ( pTARGET->GetLevel() < GLCONST_CHAR::nPK_LIMIT_LEVEL )
                            return EMREACTIONABLE_FALSE;

                        if ( bPKServer )
                        {
                            //	학원이 다르고 클럽이 없거나 다를때
                            if ( (GetSchool() != pChar->GetSchool()) && 
                                ( !pMyClub || !pTarClub || !pMyClub->IsAllianceGuild_COMMON( pTarClub->m_DbNum, pTarClub->m_dwAlliance ) ))
                                return EMREACTIONABLE_TRUE;
                        }

                        // 성향 이벤트 중이면
                        if ( bBRIGHTEVENT )
                            return EMREACTIONABLE_TRUE;					

                        // PK 가능한 구역 ( 학교간 Free PK )
                        if ( bSCHOOL_FREEPK )
                            return EMREACTIONABLE_TRUE;

                        //	여기까지 오면 강제 공격이다.
                        return EMREACTIONABLE_FORCED_TRUE;
                    } // end of if
                    else
                        bREACTION = false;

                }
                break;

            case CROW_SUMMON:
            case CROW_MOB:
                if ( m_sCONFTING.IsCONFRONTING() )
                {
                    bREACTION = false;
                }
                else if ( IsSafeZone() )
                {
                    bREACTION = false;
                }
                else
                {
                    bREACTION = true;
                }
                break;

            case CROW_NPC:
                bREACTION = false;
                break;

            case CROW_MATERIAL:
                bREACTION = false;
                break;

                //case CROW_PET:	break;	// PetData

            default:
                bREACTION = false;
                break;
            }
        }
        break;
    case EMIMPACT_SIDE_ANYBODY:
        {
            switch ( emCROW )
            {
            case CROW_PC:
                {
                    SCONFTING* pCONFTING = pTARGET->GETCONFTING();
                    //누군가가 대련중이라면?
                    if ( m_sCONFTING.IsCONFRONTING() || pCONFTING->IsCONFRONTING() )
                    {
                        //나와 대련 중인가?;
                        if ( IsConflictTarget ( pTARGET ) )
                        {
                            bREACTION = true;
                            break;
                        }//대련 상대가 아닐때 대련 아군인가?;
                        else if ( m_sCONFTING.IsSAMENESS(pCONFTING->emTYPE,pCONFTING->dwTAR_ID) )
                        {
                            bREACTION = true;
                            break;
                        }
                        else
                        {
                            bREACTION = false;
                            break;
                        }
                    }
                    bREACTION = true;
                }
                break;
            case CROW_SUMMON:
            case CROW_MOB:
                bREACTION = false;
                break;

            case CROW_NPC:
                bREACTION = true;
                break;

            case CROW_MATERIAL:
                bREACTION = false;
                break;

            default:
                bREACTION = false;
                break;
            }
        }
        break;
    }

    return ( bREACTION ? EMREACTIONABLE_TRUE : EMREACTIONABLE_FALSE );
}

void GLChar::ReActionByForcedAction( ServerActor* const pTarget, const EMIMPACT_SIDE _emTargetSide )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    switch ( _emTargetSide )
    {
    case EMIMPACT_SIDE_ENEMY:
        // 적대적인 스킬을 중립에게 강제 사용시;
        {
            const EMCROW _targetCrow = pTarget->GetCrow();
            if ( _targetCrow == CROW_PC || _targetCrow == CROW_SUMMON )
            {
                PGLCHAR pCHAR = NULL;

                // Note : 소환수일 경우 
                if ( pTarget->GetCrow() == CROW_SUMMON )
                {
                    PGLSUMMONFIELD pSummon = pLand->GetSummon( pTarget->GetGaeaID() );
                    if ( !pSummon )
                        return;

                    // Note : 주인을 가져온다.
					if ( pSummon->m_sOwnerID.actorType == CROW_PC )
					{
						pCHAR = static_cast< PGLCHAR >(
							m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID ) );
					}
                    //pCHAR = GetChar(pSummon->m_dwOwnerGaeaID);
                }
                else
                {
					if ( pTarget->GetCrow() == CROW_PC )
						pCHAR = GetChar( pTarget->GetGaeaID() );
                } //if..else

                if( pCHAR == NULL )
				{
					// 통계 및 로그 수정
					// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
					sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on ReActionByForcedAction ", pTarget->GetGaeaID()));
                    return;
				}

                const bool bFreePkMap = pLand->IsFreePK();

                // 지금 현재 그러한 경우 상대방 PC가 범죄자가 아닌경우는 적대행위 시작으로 인한 생활점수 하락;
                // + FreePKMap의 경우 성향수치는 하락하지 않는다.
                if ( !pCHAR->ISOFFENDER() && !bFreePkMap )
                {
                    int nBRIGHT(GLCONST_CHAR::nPK_TRY_BRIGHT_POINT), nLIFE(GLCONST_CHAR::nPK_TRY_LIVING_POINT);

                    //	Note : 속성수치 변화.
                    m_nBright += nBRIGHT;

                    GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
                    NetMsg.nBright = m_nBright;
                    SendToClient( &NetMsg );

                    GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = m_dwGaeaID;
                    NetMsgBrd.nBright = m_nBright;
                    SendMsgViewAround(&NetMsgBrd);

                    //	Note : 생활점수 변화.
                    m_nLiving += nLIFE;

                    GLMSG::SNETPC_UPDATE_LP NetMsgLp;
                    NetMsgLp.nLP = m_nLiving;
                    SendToClient(&NetMsgLp);

                    //	Note : 적대행위 로그 기록.                        
					// 통계 및 로그 수정
                    LogCharAction(
                        m_CharDbNum,					//	당사자.
                        gamelog::ACT_HOSTILE,			//	행위.
                        gamelog::ID_CHAR, pCHAR->m_CharDbNum,	//	상대방.
                        0,							//	exp
                        nBRIGHT,					//	bright
                        nLIFE,						//	life
                        0,							//	money
						(pCHAR == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pCHAR->GetCurrentMap()
                        );
                }

                // 클럽배틀존이라면 메시지출력이 달라져야해서 이부분은 개편을 아직 못함;
                bool bClubBattle = false;
                std::tr1::shared_ptr<GLClubField> pMyClub = GetMyClub();
                std::tr1::shared_ptr<GLClubField> pTarClub = pCHAR->GetMyClub();
                if ( pLand->IsClubBattleZone() )
                {
                    if ( pMyClub && pTarClub )
                    {
                        bool bClub = pMyClub->IsBattle( pCHAR->m_ClubDbNum );
                        bool bAlliance = pMyClub->IsBattleAlliance ( pTarClub->m_dwAlliance );
                        bClubBattle = ( bClub || bAlliance );
                    }
                }

                // 적대 행위에 대한 등록;
                AddPlayHostile( pCHAR->m_CharDbNum, TRUE, bClubBattle, GLCONST_CHAR::fPK_JUSTNESS_TIME );		// 적대 행위자에게 피해자 등록. ( pk )
                pCHAR->AddPlayHostile( m_CharDbNum, FALSE, bClubBattle, GLCONST_CHAR::fPK_JUSTNESS_TIME );		// 적대 피해자에게 적대자 등록. ( PK )

                //////////////////////////////////////////////////////////////////////////
                // 공격자와 공격대상자 모두 팻을 공격모드로 변경
                STARGETID		sTargetID = STARGETID ( CROW_PC, m_dwGaeaID, m_vPosition );
                PGLPETFIELD		pEnemyPet = pCHAR->GetMyPet();
                if( pEnemyPet && pEnemyPet->IsValid () && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
                {
                    D3DXVECTOR3 vOwnerPos, vDist;
                    float fDist;
                    vOwnerPos = pCHAR->GetPosition ();
                    vDist = pEnemyPet->m_vPos - vOwnerPos;
                    fDist = D3DXVec3Length(&vDist);

                    // 일정거리 안에 있으면 
                    if( fDist <= GLCONST_PET::fWalkArea )
                    {
                        pEnemyPet->ReSetAllSTATE ();
                        pEnemyPet->SetSTATE ( EM_PETACT_ATTACK );

                        GLMSG::SNETPET_ATTACK NetMsg(sTargetID);
                        pCHAR->SendToClient( &NetMsg );

                        GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(sTargetID);
                        NetMsgBRD.dwGUID = pEnemyPet->m_dwGUID;
                        pCHAR->SendMsgViewAround(&NetMsgBRD);
                    } //if
                } //if

                PGLPETFIELD pMyPet = GetMyPet();
                if( pMyPet && pMyPet->IsValid () && !pMyPet->IsSTATE ( EM_PETACT_ATTACK ) )
                {
                    D3DXVECTOR3 vDist;
                    float fDist;
                    vDist = pMyPet->m_vPos - m_vPosition;
                    fDist = D3DXVec3Length(&vDist);

                    // 일정거리 안에 있으면 
                    if( fDist <= GLCONST_PET::fWalkArea )
                    {
                        pMyPet->ReSetAllSTATE ();
                        pMyPet->SetSTATE ( EM_PETACT_ATTACK );

                        GLMSG::SNETPET_ATTACK NetMsg(m_TargetID);
                        SendToClient( &NetMsg );

                        GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(m_TargetID);
                        NetMsgBRD.dwGUID = pMyPet->m_dwGUID;
                        SendMsgViewAround(&NetMsgBRD);
                    } //if
                } //if
            }// end if ( _targetCrow == CROW_PC || _targetCrow == CROW_SUMMON )
        }//end case EMIMPACT_SIDE_ENEMY:
        break;

    default:
        break;
    }    
}

DWORD GLChar::ToDamage(const STARGETID &sTargetID, const int nDAMAGE, const DWORD dwDamageFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	ServerActor* pACTOR_TAR = GetTarget ( pLand, sTargetID );
	if ( !pACTOR_TAR )
		return 0;

	// PC에게 damage를 주려면 대련자여야 가능.
	//		대련 종료 직후 damage 유입 차단을 위해서.
	//
	if ( IsReActionable(pACTOR_TAR, EMIMPACT_SIDE_ENEMY) == EMREACTIONABLE_FALSE )
		return 0;

	return pACTOR_TAR->ReceiveDamage ( GETCROW(), m_dwGaeaID, nDAMAGE, dwDamageFlag );
}

DWORD GLChar::ToHeal(const STARGETID& sTargetID, const int nHEAL, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	ServerActor* pACTOR_TAR = GetTarget(pLand, sTargetID);
	if ( !pACTOR_TAR )
		return 0;

	return pACTOR_TAR->ReceiveHeal(GETCROW(), m_dwGaeaID, nHEAL, dwHealFlag);
}

void GLChar::BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID /* = NATIVEID_NULL()*/, const WORD wSkillLevel /* = 1*/, const BOOL bLowSP /* = FALSE*/)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( SkillID==NATIVEID_NULL() )
	{
		if ( !bLowSP )
		{
			WORD wDisSP = m_wSUM_DisSP + GLCONST_CHAR::wBASIC_DIS_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		ServerActor *pACTOR = GetTarget ( pLand, sTargetID );
		if (pACTOR)
            pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, 0 );
	}
	else
	{
		// SP가 부족하지 않을때에는 SP 소모시킴.
		//
		if ( !bLowSP )
		{
			// 캐릭터가 배운 스킬 정보 가져옴.
			SKILL_MAP_ITER learniter = m_ExpSkills.find ( SkillID.dwID );
			if( learniter==m_ExpSkills.end() )
				return;	// 배운 스킬이 아닐 경우.

			SCHARSKILL &sSkill = (*learniter).second;

			//	스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SkillID.wMainID, SkillID.wSubID );
			if ( !pSkill )
				return;

			SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];

			WORD wDisSP = m_wSUM_DisSP + sSKILL_DATA.wUSE_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			STARGETID sTargetID(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);

			ServerActor* pACTOR = GetTarget ( pLand, sTargetID );
			if (pACTOR)
                pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, 0 );
		}
	}

	// 자신에게.
	GLMSG::SNETPC_ATTACK_BLOCKING NetMsg;
	NetMsg.emTarCrow	= sTargetID.emCrow;
	NetMsg.dwTarID		= sTargetID.GaeaId;
	NetMsg.SkillID			= SkillID;
	NetMsg.wSkillLevel	= wSkillLevel;
	SendToClient( &NetMsg );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_BLOCKING_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID	= m_dwGaeaID;
	NetMsgBrd.emTarCrow	= sTargetID.emCrow;
	NetMsgBrd.dwTarID	= sTargetID.GaeaId;
	SendMsgViewAround(&NetMsgBrd);
}

void GLChar::AvoidProc(const SNATIVEID& SkillID /* = NATIVE_NULL()*/, const WORD wSkillLevel /* = 1*/, const BOOL bLowSP /* = FALSE*/)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( SkillID==NATIVEID_NULL() )
	{
		if ( !bLowSP )
		{
			WORD wDisSP = m_wSUM_DisSP + GLCONST_CHAR::wBASIC_DIS_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		ServerActor *pACTOR = GetTarget ( pLand, m_TargetID );
		if (pACTOR)
            pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, 0 );
	}
	else
	{
		// SP가 부족하지 않을때에는 SP 소모시킴.
		//
		if ( !bLowSP )
		{
			// 캐릭터가 배운 스킬 정보 가져옴.
			SKILL_MAP_ITER learniter = m_ExpSkills.find ( SkillID.dwID );
			if( learniter==m_ExpSkills.end() )
				return;	// 배운 스킬이 아닐 경우.

			SCHARSKILL &sSkill = (*learniter).second;

			//	스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SkillID.wMainID, SkillID.wSubID );
			if ( !pSkill )
				return;

			SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];

			WORD wDisSP = m_wSUM_DisSP + sSKILL_DATA.wUSE_SP;
			m_sSP.DECREASE ( wDisSP );
		}

		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			STARGETID sTargetID(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);

			ServerActor* pACTOR = GetTarget ( pLand, sTargetID );
			if (pACTOR)
                pACTOR->ReceiveDamage ( CROW_PC, m_dwGaeaID, 0, 0 );
		}
	}

	// 자신에게.
	GLMSG::SNETPC_ATTACK_AVOID NetMsg;
	NetMsg.emTarCrow	= m_TargetID.emCrow;
	NetMsg.dwTarID		= m_TargetID.GaeaId;
	NetMsg.SkillID			= SkillID;
	NetMsg.wSkillLevel	= wSkillLevel;
	SendToClient( &NetMsg );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_AVOID_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID	= m_dwGaeaID;
	NetMsgBrd.emTarCrow	= m_TargetID.emCrow;
	NetMsgBrd.dwTarID	= m_TargetID.GaeaId;

	SendMsgViewAround(&NetMsgBrd);
	
	if ( GLHIDESET::CheckHideSet(EMHIDE_AVOID) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}
}

void GLChar::Heal_SendMessage(const int nHEAL, const DWORD dwHealFlag, const STARGETID& sTargetID, const bool bBroad)
{
	ToHeal( sTargetID, nHEAL, dwHealFlag );

	// 자신에게.
	GLMSG::SNETPC_ATTACK_HEAL NetMsg;
	NetMsg.emTarCrow	= sTargetID.emCrow;
	NetMsg.dwTarID		= sTargetID.GaeaId;
	NetMsg.nHeal			= nHEAL;
	NetMsg.dwHealFlag	= dwHealFlag;
	NetMsg.wSchool         = GetSchool();
	SendToClient( &NetMsg );

	if ( bBroad )
	{
		// 주변 클라이언트들에게 메세지 전송.
		//
		GLMSG::SNETPC_ATTACK_HEAL_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID	= m_dwGaeaID;
		NetMsgBrd.emTarCrow	= sTargetID.emCrow;
		NetMsgBrd.dwTarID		= sTargetID.GaeaId;
		NetMsgBrd.nHeal			= nHEAL;
		NetMsgBrd.dwHealFlag	= dwHealFlag;
		NetMsgBrd.wSchool		= GetSchool();
		SendMsgViewAround(&NetMsgBrd);
	}	
}

void GLChar::Damage_SendMessage(const int nDAMAGE, const DWORD dwDamageFlag, const STARGETID& sTargetID, const bool bBroad)
{
	// 대상자에게 대미지값 적용.
	//
	const int nActualDamage = (int)ToDamage( sTargetID, nDAMAGE, dwDamageFlag );

	// 자신에게.
	GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= sTargetID.emCrow;
	NetMsg.dwTarID			= sTargetID.GaeaId;
	NetMsg.nDamage			= nActualDamage;
	NetMsg.dwDamageFlag	= dwDamageFlag | DAMAGE_TYPE_STIGMA;
	NetMsg.wSchool          = GetSchool();
	SendToClient( &NetMsg );

	if ( bBroad )
	{
		// 주변 클라이언트들에게 메세지 전송.
		//
		GLMSG::SNETPC_ATTACK_DAMAGE_BRD NetMsgBrd;
		NetMsgBrd.dwGaeaID		= m_dwGaeaID;
		NetMsgBrd.emTarCrow		= sTargetID.emCrow;
		NetMsgBrd.dwTarID			= sTargetID.GaeaId;
		NetMsgBrd.nDamage			= nActualDamage;
		NetMsgBrd.dwDamageFlag	= dwDamageFlag | DAMAGE_TYPE_STIGMA;
		NetMsgBrd.wSchool			= GetSchool();
		SendMsgViewAround(&NetMsgBrd);
	}	
}

void GLChar::StigmaProc_SendMessage(const STARGETID& sTargetID, const float fRate, const bool bBroad)
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	int nDAMAGE = 0;
	if ( fRate < 0.0f )
	{
		const DWORD dwHealFlag = CALCHEAL( nDAMAGE, m_dwGaeaID, sTargetID, pLand);
		const float fResultHeal = -fRate * float(nDAMAGE);
		Heal_SendMessage(DWORD(fResultHeal), dwHealFlag, sTargetID, bBroad);
	}
	else
	{
		const DWORD dwDamageFlag = CALCDAMAGE(nDAMAGE, m_dwGaeaID, sTargetID, pLand);
		const float fResultDamage = fRate * float(nDAMAGE);
		Damage_SendMessage(DWORD(fResultDamage), dwDamageFlag, sTargetID, bBroad);
	}
}


void GLChar::StigmaProc_Normal(const bool bBroad)
{	
	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i )
	{
		SSTIGMA::SSTIGMA_PACK& sStigmaPack = m_sSTIGMA_USER.sSTIGMA_PACK[_i - 1];		
		if ( !sStigmaPack.bUse )
			continue;

		if ( !(sStigmaPack.dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_NORMAL) )
			continue;
		ServerActor* pACTOR = GetTarget(sStigmaPack.sTakerID);
		if ( !pACTOR )
		{
			m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
			continue;
		}

		if ( !pACTOR->IsValidBody() )
			m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);

		switch ( pACTOR->GetCrow() )
		{
		case CROW_PC:
			if ( ((GLChar*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
				continue;
			}
			break;
		case CROW_NPC:					
		case CROW_MOB:
			if ( ((GLCrow*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
				continue;
			}
			break;
		case CROW_SUMMON:
			if ( ((GLSummonField*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
				continue;
			}
			break;
		}

		//if ( sStigmaPack.sTakerID != m_TargetID )
			StigmaProc_SendMessage(sStigmaPack.sTakerID, sStigmaPack.fRate, bBroad);
	}	
}

void GLChar::DamageProc ( int nDAMAGE, const DWORD dwDamageFlag, const BOOL bLowSP )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// 데미지 산출.
	//				( sp 부족시 데미지 감산. )
	float	fDAMAGE_RATE(1.0f);
	if ( bLowSP )
		fDAMAGE_RATE *= (1-GLCONST_CHAR::fLOWSP_DAMAGE);

	nDAMAGE = int(nDAMAGE*fDAMAGE_RATE);
	if ( nDAMAGE < 1 )
		nDAMAGE = 1;

	// 대련 종료후 무적타임이면 공격 무시
	GLChar* pCHAR = GetChar(m_TargetID.GaeaId );
	if ( pCHAR )
	{
		if ( pCHAR->m_sCONFTING.IsPOWERFULTIME () )
			return;
	}

	// SP 소모.
	//
	if ( !bLowSP )
	{
		WORD wDisSP = m_wSUM_DisSP + GLCONST_CHAR::wBASIC_DIS_SP;
		m_sSP.DECREASE(int(wDisSP));

		GLMSG::SNETPC_UPDATE_SP NetMsg(m_sSP.nNow);
		SendToClient( &NetMsg );
	}

	if ( m_TargetID.emCrow==CROW_PC || m_TargetID.emCrow==CROW_SUMMON )
	{
		nDAMAGE = int(nDAMAGE*GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if ( nDAMAGE==0 )
			nDAMAGE = 1;
	}

    //! 인증기일 경우에는 Broadcating을 하지않는다 (ToDamage에서 대신 보냄)
    bool bBroad = true;
    ServerActor* pACTOR = GetTarget ( pLand, m_TargetID );
    if ( pACTOR )
    {
        if ( CROW_MOB == pACTOR->GetCrow() )
        {
            GLCrow* pCrow = dynamic_cast<GLCrow*>(pACTOR);
			if ( NPC_TYPE_AUTHENTICATOR == pCrow->m_pCrowData->m_emNPCType )
			{
				bBroad = false;
			}
        }
    }
	

	Damage_SendMessage(nDAMAGE, dwDamageFlag, m_TargetID, bBroad);	
	StigmaProc_Normal(bBroad);

	if (!pACTOR)
        return;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	// 상태 이상 유발.
	//
	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		ITEM::SSATE_BLOW &sBlow = pRHandItem->sSuitOp.sBLOW;

		// 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBlow.emTYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, GLPeriod::GetInstance().GetWeather(), pLand->IsWeatherActive() );
		const SNATIVEID _baseMapID(pLand->GetBaseMapID());
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( _baseMapID.Mid(), _baseMapID.Sid() );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, dwWeather, pLand->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBlow.emTYPE)) )
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBlow.fRATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );

		if ( bBLOW )
		{
			// 상태이상 발생.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = sBlow.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = sBlow.emTYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = sBlow.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = sBlow.fVAR2;

			if ( pACTOR->STATEBLOW ( sSTATEBLOW ) )
			{
				// 상태이상 발생 Msg.
				//
				GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
				NetMsgState.emCrow = m_TargetID.emCrow;
				NetMsgState.dwID = m_TargetID.GaeaId;
				NetMsgState.emBLOW = sBlow.emTYPE;
				NetMsgState.fAGE = fLIFE;
				NetMsgState.fSTATE_VAR1 = sBlow.fVAR1;
				NetMsgState.fSTATE_VAR2 = sBlow.fVAR2;
                NetMsgState.wSchool     = pACTOR->GetSchool();

				// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				//
				pACTOR->SendMsgViewAround(&NetMsgState);

				// 스킬 대상에게 메세지 전송.
				if (m_TargetID.emCrow==CROW_PC)
                    SendToClient(pACTOR->GETCLIENTID(), &NetMsgState);
			}
		}
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		ITEM::SSATE_BLOW &sBlow = pLHandItem->sSuitOp.sBLOW;

		// 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBlow.emTYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

		const SNATIVEID _baseMapID(pLand->GetBaseMapID());
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( _baseMapID.Mid(), _baseMapID.Sid() );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( sBlow.emTYPE, dwWeather, pLand->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBlow.emTYPE)) )
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBlow.fRATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );

		if ( bBLOW )
		{
			// 상태이상 발생.
			//	 
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = sBlow.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = sBlow.emTYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = sBlow.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = sBlow.fVAR2;

			if ( pACTOR->STATEBLOW ( sSTATEBLOW ) )
			{
				// 상태이상 발생 Msg.
				//
				GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
				NetMsgState.emCrow = m_TargetID.emCrow;
				NetMsgState.dwID = m_TargetID.GaeaId;
				NetMsgState.emBLOW = sBlow.emTYPE;
				NetMsgState.fAGE = fLIFE;
				NetMsgState.fSTATE_VAR1 = sBlow.fVAR1;
				NetMsgState.fSTATE_VAR2 = sBlow.fVAR2;
                NetMsgState.wSchool     = pACTOR->GetSchool();

				// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				//
				pACTOR->SendMsgViewAround(&NetMsgState);

				// 스킬 대상에게 메세지 전송.
				if (m_TargetID.emCrow == CROW_PC)
                    SendToClient(pACTOR->GETCLIENTID(), &NetMsgState);
			}
		}
	}

	// 강한 타격
	if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW ) 
	{
		STARGETID dwActorID = STARGETID(CROW_PC,m_dwGaeaID);
		//	선도 인증중인지 체크해야 됨
		if ( pLand->m_bGuidBattleMap && pLand->m_pGuidance )
		{
			if ( pACTOR->GetCrow() == CROW_PC && pLand->m_pGuidance->m_dwCERTIFY_CHARID == pACTOR->GetCharID() )
			{
				return;
			}				
		}
		
		pLand->PushPullAct( dwActorID, m_TargetID, GLCONST_CHAR::fCRUSH_BLOW_RANGE, GLCONST_CHAR::fPUSHPULL_VELO, AN_SUB_00_SIZE );
	}
}

void GLChar::DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if (emCrow == CROW_PC)
	{
		nDAMAGE = int (nDAMAGE * GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if (nDAMAGE == 0)
            nDAMAGE = 1;
	}

    ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sACTOR );

    if ( !pACTOR )
    {
        return;
    }

    int nActualDamageHP = (int)-nDAMAGE;
    int nActualDamageMP(0);
    int nActualDamageSP(0);
    //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
    VAR_BODY_POINT( sACTOR.emCrow, sACTOR.GaeaId, DAMAGE_TYPE_CURSE, nActualDamageHP, nActualDamageMP, nActualDamageSP );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
	NetMsgBRD.emCrow = GetCrow();
	NetMsgBRD.dwID = m_dwGaeaID;

	//	스킬을 사용한 행위자 마킹.
	NetMsgBRD.sACTOR.emCrow = sACTOR.emCrow;
	NetMsgBRD.sACTOR.dwID = sACTOR.GaeaId;

	NetMsgBRD.dwDamageFlag = DAMAGE_TYPE_CURSE;
	NetMsgBRD.nVAR_HP = nActualDamageHP;
	NetMsgBRD.nVAR_MP = nActualDamageMP;
	NetMsgBRD.nVAR_SP = nActualDamageSP;

    NetMsgBRD.wSchool = pACTOR->GetSchool();

	SendMsgViewAround(&NetMsgBRD);
	SendToClient(&NetMsgBRD);
}

void GLChar::DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR)
{
	if (sACTOR.emCrow == CROW_PC || sACTOR.emCrow == CROW_SUMMON)
	{
		nDAMAGE = int (nDAMAGE*GLCONST_CHAR::fPK_POINT_DEC_PHY);
		if (nDAMAGE == 0)
            nDAMAGE = 1;
	}

	const int nActualDamage = (int)ToDamage(sACTOR, nDAMAGE, 0);

	// 자신에게.
	GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
	NetMsg.emTarCrow		= sACTOR.emCrow;
	NetMsg.dwTarID			= sACTOR.GaeaId;
	NetMsg.nDamage			= nActualDamage;
	NetMsg.dwDamageFlag		= DAMAGE_TYPE_NONE;
    NetMsg.wSchool          = GetSchool();
	SendToClient( &NetMsg );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_ATTACK_DAMAGE_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID		= m_dwGaeaID;
	NetMsgBrd.emTarCrow		= sACTOR.emCrow;
	NetMsgBrd.dwTarID		= sACTOR.GaeaId;
	NetMsgBrd.nDamage		= nActualDamage;
    NetMsgBrd.dwDamageFlag	= DAMAGE_TYPE_NONE;
    NetMsgBrd.wSchool       = GetSchool();
	SendMsgViewAround(&NetMsgBrd);
}

void GLChar::DefenseSkill(const SNATIVEID& sNativeID, WORD wLevel, const STARGETID& sACTOR)
{
	if (sNativeID == NATIVEID_NULL())
        return;

	GLMSG::SNETPC_DEFENSE_SKILL_ACTIVE NetMsg;
	NetMsg.emTarCrow = sACTOR.emCrow;
	NetMsg.dwTarID = sACTOR.GaeaId;
	NetMsg.sNativeID = sNativeID;
	NetMsg.wLevel = wLevel;
	SendToClient( &NetMsg );

	m_bDefenseSkill	= true;

    return;
}

const bool GLChar::IsImmuneToSkill ( BYTE iIndex ) const
{
	if ( RF_IMMUNE( m_EffSkillVarSet ).IsOn() )
	{
		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iIndex ) )
		{
			return true;
		}
	}

	return false;
}

BOOL GLChar::AttackProcess ( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
		return FALSE;

	if ( m_sDOMINATE.bDominate )	
		return FALSE;
	
	const float fAccumElapsed = (GLCHARLOGIC::GETATTVELO() + GLCHARLOGIC::GETATT_ITEM()) * fElapsedTime * UNITANIKEY_PERSEC;
	m_fattTIMER += fAccumElapsed;

	GLCONST_CHARCLASS &ConstCharClass = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	BOOL bDistinguish = RF_DISGUISE( m_EffSkillVarSet ).IsOn();

	GLDistinguishSkinManager::sTransAni* pAniSet = NULL;

	if( bDistinguish ) {
		DWORD dwIndex = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

		SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
		SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
		if ( pItem )
		{
			// Table ID 가 같을 경우에만 Index를 변경한다;
			// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
			if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
				dwIndex = pItem->sPet.sPetID.Sid();
		}

		pAniSet = DISTINGUISH_INST.GetAniSet( m_wSex, dwIndex );
	}

	EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );
	VECANIATTACK &vecAniAttack = ( pAniSet != NULL ) ? pAniSet->m_ANIMATION[AN_ATTACK][emANISUBTYPE] : ConstCharClass.m_ANIMATION[AN_ATTACK][emANISUBTYPE];	
	
	if ( vecAniAttack.empty() )
		return FALSE;

	// 공격 모션 종류가 변경된 것으로 보임. 공격 취소. ( 장비 찰탁. )
	//
	if ( vecAniAttack.size() <= m_dwANISUBSELECT )
	{
		TurnAction ( GLAT_IDLE );

		//GLMSG::SNET_ACTION_BRD NetMsgBrd;
		//NetMsgBrd.emCrow = CROW_PC;
		//NetMsgBrd.dwID = m_dwGaeaID;
		//NetMsgBrd.emAction = GLAT_IDLE;
		//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

		//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

		return FALSE;
	}

	ServerActor *pTARGET = GetTarget ( pLand, m_TargetID );

	if ( !ISLONGRANGE_ARMS() )
	{	
		if ( !pTARGET )
		{
			TurnAction ( GLAT_IDLE );

			// 자신에게.
			GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
			NetMsg.emTarCrow		= m_TargetID.emCrow;
			NetMsg.dwTarID			= m_TargetID.GaeaId;
			NetMsg.nDamage			= 0;
            NetMsg.wSchool          = GetSchool();
			SendToClient( &NetMsg );
			SendMsgViewAround(&NetMsg);

			//// 모션 변경.
			//GLMSG::SNET_ACTION_BRD NetMsgBrd;
			//NetMsgBrd.emCrow = CROW_PC;
			//NetMsgBrd.dwID = m_dwGaeaID;
			//NetMsgBrd.emAction = GLAT_IDLE;
			//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

			//// 모션 변경 (모두에게).
			//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

			return FALSE;
		}

		// 고정된 상태에서의 연속 공격으로 인한 동기화 오차가 발생할 여지가 있으므로 계수를 스킬 쪽과 동일하게 2 -> 20으로 조정;
		int nAttackRange = pTARGET->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + static_cast<int>(GETSUM_RANGE_PSY_LONG()) + 20;
		if ( nAttackRange < 0)
			nAttackRange = pTARGET->GetBodyRadius() + GETBODYRADIUS() + 20;

		nAttackRange *= GLCONST_CHAR::fATTACK_RANGE_ADJUST_RATE;
		if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( nAttackRange, m_vPosition, pTARGET->GetPosition(), 5.f ) )
		{
			TurnAction ( GLAT_IDLE );

			// 자신에게.
			GLMSG::SNETPC_ATTACK_DAMAGE NetMsg;
			NetMsg.emTarCrow		= m_TargetID.emCrow;
			NetMsg.dwTarID			= m_TargetID.GaeaId;
			NetMsg.nDamage			= 0;
            NetMsg.wSchool          = GetSchool();
			SendToClient( &NetMsg );
			SendMsgViewAround(&NetMsg);

			//// 모션 변경.
			//GLMSG::SNET_ACTION_BRD NetMsgBrd;
			//NetMsgBrd.emCrow = CROW_PC;
			//NetMsgBrd.dwID = m_dwGaeaID;
			//NetMsgBrd.emAction = GLAT_IDLE;
			//m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgBrd );

			//// 모션 변경 (모두에게).
			//SendMsgViewAround ( (NET_MSG_GENERIC *) &NetMsgBrd );

			return FALSE;
		}
	}


	// 특정 공격 에니메이션 설정 선택.
	//
	const SANIATTACK &sAniAttack = vecAniAttack[m_dwANISUBSELECT];
	const DWORD dwThisKey = sAniAttack.m_dwSTime + static_cast<DWORD>(m_fattTIMER);
	m_WorkingSkill.HitDamage( dwThisKey, sAniAttack );
	
	return TRUE;
}

void GLChar::SummonCrow ( const SNATIVEID& sFamiliarID
						, float fRemainedTime
						, DWORD dwFamiliarType
						, bool bEventSkill
						, DWORD dwMaxSummon
						, WORD wSkillLevel )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLChar::SummonCrow() - Land is NULL." );
		return;
	}

	// 네비에 세울수 있는지 검사한다;
	D3DXVECTOR3 vEventPos = GetPosition();
	D3DXVECTOR3 vCollision;
	if( pLand->IsCollisionNavi(
		vEventPos + D3DXVECTOR3( 0, +10, 0 ),
		vEventPos + D3DXVECTOR3( 0, -10, 0 ),
		vCollision ) )
	{
		vEventPos = vCollision;
	}
	else
	{
		boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
		if ( spNavi )
		{
			DWORD CurrentCellID = spNavi->FindClosestCell( vEventPos );
			vEventPos = spNavi->SnapPointToCell( CurrentCellID, vEventPos );
		}
	}

	// 주인의 무기 강화 횟수를 구한다;
	// 주인의 무기 속성을 구한다;
	ITEM::SSATE_BLOW sSTATEBLOW;
	EMSLOT emSlot = GetCurRHand();
	SITEMCUSTOM& ItemCustom = m_PutOnItems[ emSlot ];
	BYTE cGrade = ItemCustom.GETGRADE( EMGRINDING_DAMAGE );
	SITEM* pITEM = GET_ELMT_ITEM();
	if( pITEM )
		sSTATEBLOW = pITEM->sSuitOp.sBLOW;

	DWORD dwLandEventGUID = -1;
	FLOAT fSummonTime = 0.5f;
	FLOAT fSummonRemaindTime = fRemainedTime;
	FLOAT fEndTime = fSummonTime;

	CEVENT_NODE_SERVER_SUMMONGATE* pEventInfo =
		pLand->DropLandEvent<CEVENT_NODE_SERVER_SUMMONGATE>(
		dwLandEventGUID, 
		Faction::GLActorID( CROW_PC, m_CharDbNum ),
		vEventPos,
		fEndTime,
		fSummonTime );

	if( !pEventInfo )
	{
		sc::writeLogError( sc::string::format(
			"GLChar::SummonCrow() - pEventInfo is NULL! Char(%1%) on MapID(%2%, %3%).",
			m_CharDbNum,
			pLand->GetBaseMapID().Mid(),
			pLand->GetBaseMapID().Sid() ) );
		return;
	}

	pEventInfo->m_sSkillID = SNATIVEID();
	pEventInfo->m_wLevel = wSkillLevel;
	pEventInfo->m_sSummonNativeID = sFamiliarID;
	pEventInfo->m_dwSummonMax = dwMaxSummon;
	pEventInfo->m_dwType = dwFamiliarType;
	pEventInfo->m_bEventSkill = bEventSkill;
	pEventInfo->m_fSummonRemainedTime = fSummonRemaindTime;

	pEventInfo->m_fOwnerIncHP = m_fBasicRateHP;
	pEventInfo->m_wOwnerAttack = GETSUMMONDAMGE();
	pEventInfo->m_wOwnerDefense = m_nDEFENSE_SKILL;
	pEventInfo->m_cOwnerWeaponsGrade = cGrade;
	pEventInfo->m_sOwnerBlow = sSTATEBLOW;
	pEventInfo->m_wOwnerSchool = m_wSchool;

	pEventInfo->m_sEvent = STARGETID(
		CROW_PC, dwLandEventGUID, vEventPos );
	pEventInfo->m_sTarget = STARGETID(
		CROW_MOB, EMTARGET_NULL, vEventPos );
	//pEventInfo->m_emCrow = emCrow;

	if( !pEventInfo )
	{
		if( !pLand )
		{
			sc::writeLogError( sc::string::format(
				"GLChar::SummonCrow() - Char(%1%)'s pLand is NULL!",
				m_CharDbNum ) );
			return;
		}

		sc::writeLogError( sc::string::format(
			"GLChar::SummonCrow() - pEventInfo is NULL! Char(%1%) on MapID(%2%, %3%).",
			m_CharDbNum,
			pLand->GetBaseMapID().Mid(),
			pLand->GetBaseMapID().Sid() ) );
		return;
	}

	// 주변 클라이언트에 랜드이벤트 등록 및 송신;
	pEventInfo->RegisterViewAroundPC( m_pGaeaServer );
}

void GLChar::ChangeSummonSkillPage ( UINT iPage, SNATIVEID sSkillID )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLChar::ChangeSummonSkillPage() - Land is NULL." );
		return;
	}

	CROWIDLIST_ITER iter = GetChildCrowList().begin();
	CROWIDLIST_ITER iterEnd = GetChildCrowList().end();
	for ( ; iter != iterEnd ; ++iter )
	{
		Faction::GLActorID sSummonID = *iter;
		if ( sSummonID.actorType != CROW_SUMMON )
			continue;

		PGLSUMMONFIELD pSummon =
			pLand->GetSummon( sSummonID.actorID_Num );
		if ( !pSummon )
		{
			sc::writeLogError( "ChildCrowList have a invalid data." );
			continue;
		}

		if ( pSummon->IsEventSkill() )
		{
			if ( pSummon->IsValidEventSkillDistance() )
			{
				pSummon->ChangeSkillPage( iPage );

				//pSummon->ViewSkillTargetEffect( sSkillID );
			}
		}
	}
}

void GLChar::UseSummonEventSkill ( UINT iIdx, SNATIVEID sSkillID )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLChar::UseSummonEventSkill() - Land is NULL." );
		return;
	}

	CROWIDLIST_ITER iter = GetChildCrowList().begin();
	CROWIDLIST_ITER iterEnd = GetChildCrowList().end();
	for ( ; iter != iterEnd ; ++iter )
	{
		Faction::GLActorID sSummonID = *iter;
		if ( sSummonID.actorType != CROW_SUMMON )
			continue;

		PGLSUMMONFIELD pSummon =
			pLand->GetSummon( sSummonID.actorID_Num );
		if ( !pSummon )
		{
			sc::writeLogError( "ChildCrowList have a invalid data." );
			continue;
		}

		if ( pSummon->IsEventSkill() )
		{
			if ( pSummon->IsValidEventSkillDistance() )
			{
				pSummon->UseEventSkill( iIdx );

				//pSummon->ViewSkillTargetEffect( sSkillID );
			}
		}
	}
}

void GLChar::HireBasicSummon ( WORD wMainID, WORD wSubID )
{
	GLMSG::SNETPC_REQ_HIRE_BASICSUMMON_FC msgFC;

	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		msgFC.emFC = EMFAILED_HIRE_SUMMON_FAIL;
		sc::writeLogError( "GLChar::HireBasicSummon() - Land is NULL." );
		SendToClient( &msgFC );
		return;
	}

	/// 변경 가능한 캐릭터인지 검사;

	// 소환 패시브 검사;
	if ( !m_sSummonable.bSummonable )
	{
		msgFC.emFC = EMFAILED_HIRE_SUMMON_DONT_LEARN_PASSIVE;
		sc::writeLogInfo ( "Failed Hire Summon. Summon Passive Skill don't learn." );
		SendToClient( &msgFC );
		return;
	}

	// Crow 검사;
	SNATIVEID sTempSummonID = SNATIVEID( wMainID, wSubID );
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData (
		sTempSummonID );
	if ( !pCrowData )
	{
		msgFC.emFC = EMFAILED_HIRE_SUMMON_INVALID_CROWDATA;
		sc::writeLogInfo ( "Failed Hire Summon. Crow Data is NULL." );
		SendToClient( &msgFC );
		return;
	}

	// 현재 소환 패시브 스킬이 소환할 수 있는 Crow인지 판별;
	DWORD dwCrowLevel = static_cast< DWORD >( pCrowData->GetLevel() );
	DWORD dwMaxCrowLevel = (m_sSummonable.dwSummonLevel+1)*
		AISystem::GLFSMContainer::Instance()->GetHireSummonLevelInterval();
	if ( dwCrowLevel > dwMaxCrowLevel )
	{
		msgFC.emFC = EMFAILED_HIRE_SUMMON_CROWLEVEL_OVERFLOW;
		sc::writeLogInfo ( "Failed Hire Summon. Crow Level is overflow." );
		SendToClient( &msgFC );
		return;
	}

	// 변경될 경우 고용소환수를 죽인다;
	if ( m_sBasicSummonID != SNATIVEID( wMainID, wSubID ) &&
		m_sSummonable.hireInfo.bSummoning )
	{
		KillAllHireChildCrow ();
	}

	// 변경;
	m_sBasicSummonID = SNATIVEID( wMainID, wSubID );

	// 클라이언트의 m_sBasicSummonID를 변경시킨다 ( GaeaID는 NULL );
	MsgSendUpdateHireSummonState( 0, m_sBasicSummonID );

	/*SKILL_MAP_ITER iter = m_ExpSkills.begin();
	SKILL_MAP_ITER iterEnd = m_ExpSkills.end();
	for ( ; iter != iterEnd ; ++iter )
	{
		// 배운 스킬 정보 가져오기;
		SCHARSKILL sSkill = iter->second;
		SNATIVEID sSkillID = sSkill.sNativeID;
		WORD wSkillLevel = sSkill.wLevel;

		// 스킬 정보;
		PGLSKILL pSkill =
			GLSkillMan::GetInstance().GetData(
			sSkillID.wMainID, sSkillID.wSubID );
		if ( !pSkill )
			continue;

		// 소환 스킬 검색;
		if ( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
		{
			if ( wSkillLevel >= 0 && wSkillLevel < MAX_LEVEL )
			{
				SKILL::SCREATURE_LVL sCreatureLVL =
					pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[ wSkillLevel ];

				// 스킬 ID가 NULL이어야만 고용 소환수 관련 스킬이다;
				// 고용 소환수 관련 스킬이 두개 이상이더라도 첫번째 검색되는;
				// 스킬을 사용하도록 한다;
				if ( !sCreatureLVL.sID.IsValidNativeID() )
				{
					// 모든 소환수를 죽인다;
					RemoveAllChildCrow();

					// 소환;
					if ( m_sSummonable.bSummonable &&
						m_sSummonable.hireInfo.bSummoning )
					{
						//pChar->m_sSummonable.hireInfo.bMoveMap = true;
						m_sSummonable.hireInfo.bResummoning = true;
					}
					else
					{
						SummonCrow( SNATIVEID( false ),
							sCreatureLVL.fSEC_ACTIVETIME,
							sCreatureLVL.dwType,
							sCreatureLVL.bEventSkill,
							sCreatureLVL.dwMAX,
							wSkillLevel );
					}
				}
			}
		}
	}*/

	SendToClient( &msgFC );
}

void GLChar::SummonTargeting ( const STARGETID& sTARID, SNATIVEID sSkillID )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLChar::SummonTargeting() - Land is NULL." );
		return;
	}

	CROWIDLIST_ITER iter = GetChildCrowList().begin();
	CROWIDLIST_ITER iterEnd = GetChildCrowList().end();
	for ( ; iter != iterEnd ; ++iter )
	{
		Faction::GLActorID sSummonID = *iter;
		if ( sSummonID.actorType != CROW_SUMMON )
			continue;

		PGLSUMMONFIELD pSummon =
			pLand->GetSummon( sSummonID.actorID_Num );
		if ( !pSummon )
		{
			sc::writeLogError( "ChildCrowList have a invalid data." );
			continue;
		}

		if ( pSummon->IsEventSkill() )
		{
			if ( pSummon->IsValidEventSkillDistance() )
			{
				pSummon->SetTargetID( m_TargetID );
				pSummon->StartTargetAction();

				//pSummon->ViewSkillTargetEffect( sSkillID );
			}
		}
	}
}

void GLChar::SummonRunAway ( SNATIVEID sSkillID )
{
	GLLandMan* const pLand = GetLandMan();
	if ( !pLand )
	{
		sc::writeLogError( "GLChar::SummonRunAway() - Land is NULL." );
		return;
	}

	CROWIDLIST_ITER iter = GetChildCrowList().begin();
	CROWIDLIST_ITER iterEnd = GetChildCrowList().end();
	for ( ; iter != iterEnd ; ++iter )
	{
		Faction::GLActorID sSummonID = *iter;
		if ( sSummonID.actorType != CROW_SUMMON )
			continue;

		PGLSUMMONFIELD pSummon =
			pLand->GetSummon( sSummonID.actorID_Num );
		if ( !pSummon )
		{
			sc::writeLogError( "ChildCrowList have a invalid data." );
			continue;
		}

		if ( pSummon->IsEventSkill() )
		{
			if ( pSummon->IsValidEventSkillDistance() )
			{
				pSummon->RunAwayAction( true );

				//pSummon->ViewSkillTargetEffect( sSkillID );
			}
		}
	}
}

CEVENT_NODE_SERVER_SUMMONGATE* GLChar::GetSummonEventInfo(PGLSKILL pSkill, const SCHARSKILL& sSkill, const D3DXVECTOR3& vEventPos)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

    if (!pLand)
    {
        sc::writeLogError(sc::string::format("GLChar::GetSummonEventInfo() - Char(%1%)'s pLand is NULL!", m_CharDbNum));
        return NULL;
    }

	// 주인의 무기 강화 횟수를 구한다.
	// 주인의 무기 속성을 구한다.
	ITEM::SSATE_BLOW	sSTATEBLOW;
	EMSLOT				emSlot      		= GetCurRHand();
	SITEMCUSTOM&		ItemCustom			= m_PutOnItems[emSlot];
	BYTE				cGrade      		= ItemCustom.GETGRADE( EMGRINDING_DAMAGE );
	SITEM*				pITEM				= GET_ELMT_ITEM();
	if( pITEM )
		sSTATEBLOW = pITEM->sSuitOp.sBLOW;

	DWORD 				dwLandEventGUID    = -1;
	FLOAT 				fSummonTime        = (FLOAT)pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].fSEC_RECALLDELAY;
	FLOAT 				fSummonRemaindTime = (FLOAT)pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].fSEC_ACTIVETIME + m_fSUMMONACTIVETIME;
	FLOAT 				fEndTime           = fSummonTime;

	CEVENT_NODE_SERVER_SUMMONGATE* pEventInfo = pLand->DropLandEvent<CEVENT_NODE_SERVER_SUMMONGATE>(
        dwLandEventGUID, 
		Faction::GLActorID(CROW_PC, m_CharDbNum),
        vEventPos, 
        fEndTime, 
        fSummonTime);

	if (!pEventInfo)
	{
		sc::writeLogError(sc::string::format("GLChar::GetSummonEventInfo() - pEventInfo is NULL! Char(%1%) on MapID(%2%, %3%).", m_CharDbNum, pLand->GetBaseMapID().Mid(), pLand->GetBaseMapID().Sid()));
		return NULL;
	}

	pEventInfo->m_sSkillID            	= sSkill.sNativeID;
	pEventInfo->m_wLevel              	= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].wUSESKILLLEVEL;
	pEventInfo->m_sSummonNativeID     	= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].sID;
	pEventInfo->m_dwSummonMax         	= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].dwMAX;
	pEventInfo->m_dwType				= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].dwType;
	pEventInfo->m_bEventSkill			= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].bEventSkill;
	pEventInfo->m_fSummonRemainedTime 	= fSummonRemaindTime;
	pEventInfo->m_fOwnerIncHP           = m_fBasicRateHP;                						// 시전자의  HP 회복율
	pEventInfo->m_wOwnerAttack          = GETSUMMONDAMGE();		     							// 시전자의  공격력
	pEventInfo->m_wOwnerDefense         = m_nDEFENSE_SKILL;          							// 시전자의  방어력
	pEventInfo->m_cOwnerWeaponsGrade    = cGrade;                    							// 시전자의  무기 강화
	pEventInfo->m_sOwnerBlow            = sSTATEBLOW;                							// 시전자의  무기 속성
	pEventInfo->m_wOwnerSchool          = m_wSchool;                 							// 시전자의  학원
	pEventInfo->m_sEvent                = STARGETID( CROW_PC, dwLandEventGUID, vEventPos );
	pEventInfo->m_sTarget               = STARGETID( CROW_MOB, EMTARGET_NULL, vEventPos );		// 지면위치므로 타겟은 없음
	pEventInfo->m_emNeedItemAtt		    = pSkill->m_sBASIC.emUSE_RITEM;
	return pEventInfo;
} //GLChar::GetSummonEventInfo

void GLChar::SkillSummon( PGLSKILL pSkill, const SCHARSKILL& sSkill )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// 네비에 세울수 있는지 검사한다
	D3DXVECTOR3		vEventPos = m_TargetID.vPos;
	D3DXVECTOR3		vCollision;
	if( pLand->IsCollisionNavi ( vEventPos+D3DXVECTOR3(0,+10,0), vEventPos+D3DXVECTOR3(0,-10,0), vCollision ) )
	{
		vEventPos = vCollision;
	}
	else
	{
		boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
		if ( spNavi )
		{
			DWORD CurrentCellID = spNavi->FindClosestCell(vEventPos);
			vEventPos = spNavi->SnapPointToCell(CurrentCellID, vEventPos);
		}
	} //if..else

	CEVENT_NODE_SERVER_SUMMONGATE* pEventInfo = GetSummonEventInfo(pSkill, sSkill, vEventPos);	

	if(!pEventInfo)
	{
		sc::writeLogError(sc::string::format("GLChar::SkillSummon() - pEventInfo is NULL! Char(%1%) on MapID(%2%, %3%).", m_CharDbNum, pLand->GetBaseMapID().Mid(), pLand->GetBaseMapID().Sid()));
		return;
	}

	// 주변 클라이언트에 랜드이벤트 송신
    pEventInfo->RegisterViewAroundPC(m_pGaeaServer);
    
} //GLChar::SkillSummon

void GLChar::SkillBasicProc_Land( PGLSKILL pSkill, const SCHARSKILL& sSkill )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	switch( pSkill->m_sSPECIAL_SKILL.emSSTYPE )				// 특수발동스킬
	{
	case SKILL::EMSSTYPE_SUMMONS:							// 소환
		SkillSummon( pSkill, sSkill );
		break;
	} //switch
} //GLChar::SkillBasicProc_Land

/**
 * 
 */
void GLChar::SkillBasicProc( PGLSKILL pSkill, const SCHARSKILL& sSkill, const WORD& wTARNUM, const STARID* sTARIDS )
{
	switch( pSkill->m_sBASIC.emIMPACT_TAR )			//  Note : 지면위치 스킬에대한 처리.
	{
	case TAR_LAND:
		SkillBasicProc_Land( pSkill, sSkill );
		break;
	} //switch

} //GLChar::SkillBasicProc

/**
 * 
 */
void GLChar::ReactDamage( ServerActor* pAttacker, DWORD dwDamageFlags )
{
	if( dwDamageFlags & DAMAGE_TYPE_ILLUSION )
	{
		D3DXVECTOR3 vPos = pAttacker->GetPosition();
		if( m_sDamageSpec.m_fIllusionJumpBound > 0 )
			vPos += ( pAttacker->GetDirect() * -m_sDamageSpec.m_fIllusionJumpBound );
		
		DWORD		dwFlags = MOVE_OPTION_SHOW_EFF | MOVE_OPTION_NOT_TURN_IDLE;
		if( !MoveTo( &vPos, true, dwFlags ) )
			MoveTo( &pAttacker->GetPosition(), true, dwFlags );			// 우선은 데미지를 플래그로 이동에 관련해서 정리필요, 필요하다면..
	} //if
} //GLChar::ReactDamage

enum { IDX_HP = 0, IDX_MP, IDX_SP,NUM_HP_MP_SP };

void GLChar::ApplySkillTarget_HP( PGLSKILL			pSkill, 
								  WORD				wSKILL_LVL, 
								  SKILL::CDATA_LVL& sSKILL_DATA, 
								  ServerActor*		pACTOR, 
								  const WORD		wAttackDivCount,
								  const STARGETID&	sTARID,
								  int&				nHP, 
								  DWORD&			dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	//	대상에게 피해를 주는 스킬.
	if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
	{
		const SNATIVEID baseMapID(pLand->GetBaseMapID());
		DWORD		dwWeather = GLPeriod::GetInstance().GetMapWeather( baseMapID.Mid(), baseMapID.Sid());
		dwDamageFlag = CALCDAMAGE( nHP, m_dwGaeaID, sTARID, pLand, pSkill, wSKILL_LVL, dwWeather, wAttackDivCount );
		pACTOR->ReactDamage( this, dwDamageFlag );

		nHP = -nHP;
	}
	else //	대상을 회복시켜 주는 스킬.
	{
		int nDX = pACTOR->GetMaxHP() - pACTOR->GetNowHP();
		nHP += (int) min(nDX,sSKILL_DATA.fBASIC_VAR) + m_nHP_ADD;
	} //if..else
} //GLChar::ApplySkillTarget_HP

void GLChar::ApplySkillTarget_MP( PGLSKILL pSkill, SKILL::CDATA_LVL& sSKILL_DATA, ServerActor* pACTOR, int& nMP )
{
	if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
	{
		short		nRESIST = min( 99, pACTOR->GETRESIST().GetElement(pSkill->m_sAPPLY.emELEMENT) );		//	저항치.
		int			nVAR	= int( -sSKILL_DATA.fBASIC_VAR );
		nMP -= (int)( nVAR - ( nVAR * nRESIST / 100.0f * GLCONST_CHAR::fRESIST_G ) );
	}
	else //	대상을 회복시켜 주는 스킬.
	{
		int			nDX = pACTOR->GetMaxMP() - pACTOR->GetNowMP();
		nMP += (int)min( nDX, sSKILL_DATA.fBASIC_VAR ) + m_nMP_ADD;
	} //if..else
} //GLChar::ApplySkillTarget_MP

void GLChar::ApplySkillTarget_SP( PGLSKILL pSkill, SKILL::CDATA_LVL& sSKILL_DATA, ServerActor* pACTOR, int& nSP )
{
	if ( sSKILL_DATA.fBASIC_VAR < 0.0f )
	{
		short		nRESIST = min( 99, pACTOR->GETRESIST().GetElement(pSkill->m_sAPPLY.emELEMENT) );
		int			nVAR	= int( -sSKILL_DATA.fBASIC_VAR );
		nSP -= (int)( nVAR - ( nVAR * nRESIST / 100.0f * GLCONST_CHAR::fRESIST_G ) );
	}
	else //	대상을 회복시켜 주는 스킬.
	{
		int			nDX = pACTOR->GetMaxSP() - pACTOR->GetNowSP();
		nSP += (int)min( nDX, sSKILL_DATA.fBASIC_VAR ) + m_nSP_ADD;
	} //if..else
} //GLChar::ApplySkillTarget_SP

void GLChar::ApplySkillTarget_SpecToMob( const EMSPEC_ADDON&	emSpec, 
										 const SKILL::SSPEC&	sSKILL_SPEC, 
										 ServerActor*			pACTOR, 
										 DWORD&					dwDamageFlag, 
										 int*					nGather, 
										 int*					Vars )
{
	GLCrow* pCrow = dynamic_cast<GLCrow*>( pACTOR );
	// 몹일 경우 스킬 효과 비율 적용. by luxes.
	switch( emSpec )
	{
	case EMSPECA_PUSHPULL:
		if (!pACTOR->IsValidBody())
            break;
		// if ( pCrow->m_pCrowData->m_bNoAllPenalty )
		if ( pCrow->IsNoAllPenalty() )
			break;

		if (sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR2 * sSKILL_SPEC.fRate2)
            break;	// 발생확율 적용.
		dwDamageFlag |= DAMAGE_TYPE_SHOCK;
		break;

	case EMSPECA_HP_GATHER:
		nGather[IDX_HP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate );
		break;

	case EMSPECA_MP_GATHER:
		nGather[IDX_MP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate );
		break;

	case EMSPECA_SP_GATHER:
		nGather[IDX_SP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate );
		break;
    case EMSPECA_LINK_HP:
        if( !pACTOR->IsValidBody() )
            break;
		
		{			
			// if ( pCrow->m_pCrowData->m_bNoAllPenalty )
			if ( pCrow->IsNoAllPenalty() )
			{
				pACTOR->SetMaxHpRate(1.0f);  //불리한 효과 방지인 몹은 고리효과 방지
				break;
			}
            const float fMaxHP = static_cast<float>(GetMaxHP());
			const float fNoWHP = static_cast<float>(GetNowHP());
            const float fActorOldHP = static_cast<float>(pACTOR->GetNowHP());
            const float fActorMaxHP = static_cast<float>(pACTOR->GetMaxHP());

            const int fDamage = static_cast<int>(fActorOldHP - ( fActorMaxHP * fNoWHP / fMaxHP ));
            if( fDamage > 0 )
            {
                Vars[IDX_HP] -= fDamage;  //최대 HP를 변경하기전에 데미지 효과를 줌으로써 데미지 표시를 할 수 있게함(어차피 실제 깍이는 HP량은 동일)
            }
			pACTOR->SetMaxHpRate( fNoWHP / fMaxHP );
		}
        break;	
	};
} //GLChar::ApplySkillTarget_SpecToMob

void GLChar::ApplySkillTarget_SpecToMobElse( const EMSPEC_ADDON&	emSpec, 
											 const SKILL::SSPEC&	sSKILL_SPEC, 
											 ServerActor*			pACTOR, 
											 DWORD&					dwDamageFlag, 
											 int*					nGather, 
											 int*					Vars )
{
	switch( emSpec )
	{
	case EMSPECA_HP_GATHER:
		nGather[IDX_HP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 );
		break;

	case EMSPECA_MP_GATHER:
		nGather[IDX_MP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 );
		break;

	case EMSPECA_SP_GATHER:
		nGather[IDX_SP] += int ( abs(Vars[IDX_HP]) * sSKILL_SPEC.fVAR1 );
		break;

	case EMSPECA_PUSHPULL:
		if( !pACTOR->IsValidBody() )
            break;

		if( sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR2 )
            break;	// 발생확율 적용.
		
		dwDamageFlag |= DAMAGE_TYPE_SHOCK;
		break;

    case EMSPECA_VEHICLE_GETOFF: //봉주
		if( !pACTOR->IsValidBody() )
            break;

        if( sc::Random::RANDOM_POS() < sSKILL_SPEC.fVAR1 )   // 효과 적용 성공율
        {
			GLChar *pTargetChar = dynamic_cast<GLChar *>(pACTOR);
			if (pTargetChar)
				pTargetChar->ToggleVehicle(false);
        }
        break;

	case EMSPECA_TURN_STATE_BATTLE:
		if( sc::Random::RANDOM_POS() > sSKILL_SPEC.fVAR1 ||
			pACTOR->GetCrow() != CROW_PC )
		{
			break;
		} //if

		if ( IsConflictTarget((GLChar*)pACTOR))
			break;

		((GLChar*)pACTOR)->AddPlayHostileEachOther( m_CharDbNum, FALSE, sSKILL_SPEC.fVAR2 );
		break;

	case EMSPECA_TURN_STATE_PEACE:
		if( pACTOR->GetCrow() != CROW_PC )
			break;

		((GLChar*)pACTOR)->RemovePlayHostileEachOther( m_CharDbNum );
		break;	

    case EMSPECA_LINK_HP:
		if( !pACTOR->IsValidBody() )
			break;

		{
            const float fMaxHP = static_cast<float>(GetMaxHP());
            const float fNoWHP = static_cast<float>(GetNowHP());
            const float fActorOldHP = static_cast<float>(pACTOR->GetNowHP());
            const float fActorMaxHP = static_cast<float>(pACTOR->GetMaxHP());

            const int fDamage = static_cast<int>(fActorOldHP - ( fActorMaxHP * fNoWHP / fMaxHP ));
            if( fDamage > 0 )
            {
                Vars[IDX_HP] -= fDamage;  //최대 HP를 변경하기전에 데미지 효과를 줌으로써 데미지 표시를 할 수 있게함(어차피 실제 깍이는 HP량은 동일)
            }
			pACTOR->SetMaxHpRate( fNoWHP / fMaxHP );
		}
        break;
	case EMSPECA_EXP_RATE:
		m_fEXP_RATE += sSKILL_SPEC.fVAR1;
		break;
	};
} //GLChar::ApplySkillTarget_SpecToMobElse

void GLChar::ApplySkillTarget_Spec( PGLSKILL pSkill, WORD wSKILL_LVL, ServerActor* pACTOR, DWORD& dwDamageFlag, int* nGather, int* Vars )
{
	// 스킬 특수 기능.
	for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )
	{
		const SKILL::SSPEC&		sSKILL_SPEC = pSkill->m_sAPPLY.vecSPEC[k].sSPEC[wSKILL_LVL];
		if ( pACTOR->GetCrow() == CROW_MOB )
			ApplySkillTarget_SpecToMob( pSkill->m_sAPPLY.vecSPEC[k].emSPEC, sSKILL_SPEC, pACTOR, dwDamageFlag, nGather, Vars );
		else
			ApplySkillTarget_SpecToMobElse( pSkill->m_sAPPLY.vecSPEC[k].emSPEC, sSKILL_SPEC, pACTOR, dwDamageFlag, nGather, Vars );
	} //for
}

void GLChar::ApplySkillTargetCrushingBlow( const SKILL::SSPEC&	sSKILL_SPEC, 
										   ServerActor*			pACTOR,
										   BOOL&				bCrushBlowOne, 
										   SSKILLACTEX&			sSKILLACTEX )
{
	if( !bCrushBlowOne )
		return;

    if( !pACTOR->IsValidBody() )
        return;

	if( pACTOR->GetCrow() == CROW_MOB )			// 몹일 경우 스킬 효과 비율 적용. by luxes.
	{
		GLCrow*		pMob = dynamic_cast<GLCrow*>( pACTOR );
		if( pMob->IsCheckedActFlags( EMCROWACT_POSHOLD | EMCROWACT_DIRHOLD ) ||
			pMob->IsNoAllPenalty() )
		{
			return;
		} //if
		sSKILLACTEX.fPUSH_PULL	= sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate;
	}
	else
	{
		sSKILLACTEX.fPUSH_PULL	= sSKILL_SPEC.fVAR1;
	} //if..else

	sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
	sSKILLACTEX.dwAniType	= AN_SUB_00_SIZE;
} //GLChar::ApplySkillTargetCrushingBlowz

void GLChar::ApplySkillTargetRebirth( ServerActor*			pTarget,
									  const SKILL::SSPEC&	sSKILL_SPEC )
{
	if( pTarget->GetCrow() == CROW_PC )
	{
        // 오직 죽은 대상에게만 부활 스킬을 사용할 수 있다.;
		GLChar*		pPC = dynamic_cast<GLChar*>( pTarget );
		if( pPC && pPC->IsActState(EM_ACT_DIE))
		{
			if( pPC->IsCTFPlayer() )
			{
				if( GetSchool() != pPC->GetSchool() )		//CTF에서는 같은 학원생만 부활할 수 있다.
					return;

				CTFRebirthTo();
			} //if

            // 스킬 부활가능 상태로 변경;
            pPC->Receive_ResurrectSkill(sSKILL_SPEC.fVAR1*100.0f);
		} //if
	} //if
} //GLChar::ApplySkillTargetRebirth

void GLChar::ApplySkillTargetStun( ServerActor* pACTOR )
{
	if( !pACTOR->IsValidBody() )
		return;

	pACTOR->DO_STUN_ACTION();
} //GLChar::ApplySkillTargetStun

void GLChar::ApplySkillTargetStigma(const STARGETID& sTARID, const SKILL::SSPEC& sSPEC, const PGLSKILL pSkill){
	STARGETID sSelfID(CROW_PC, m_dwGaeaID);	
	ServerActor* pACTOR = GetTarget(sTARID);
	if ( pACTOR )
	{
        if( !pACTOR->IsValidBody() )
            return;

		switch ( pACTOR->GetCrow() )
		{ // 실제 등록 전에 미리 한번 등록 해주는 이유는 ApplySkill 자체 내애서 낙인 검사를 함;
		case CROW_MOB:
			{
				GLCrow* const pCrow = (GLCrow*)pACTOR;
				if ( pCrow->IsNoAllPenalty() )
					break;

				m_sSTIGMA_USER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
				pCrow->m_sSTIGMA_TAKER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
			}
			break;
		case CROW_SUMMON:
			{
				GLSummonField* const pSummon = (GLSummonField*)pACTOR;
				/*if ( pSummon->IsNoAllPenalty() )
					break;*/

				m_sSTIGMA_USER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
				pSummon->m_sSTIGMA_TAKER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
			}
			break;
		case CROW_PC:
			m_sSTIGMA_USER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
			((GLChar*)pACTOR)->m_sSTIGMA_TAKER.PushStigmaPack(sSelfID, sTARID, pSkill->m_sBASIC.sNATIVEID, sSPEC);
			break;
		}
	}
}

void GLChar::ApplySkillTargetSkillLink(const STARGETID& sSelectTARID, const STARGETID& sTARGETID, const SKILL::SSPEC& sSPEC, const SCHARSKILL& sSkill)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pACTOR = GetTarget(sTARGETID);
	if( !pACTOR )
		return;

	if( !pACTOR->IsValidBody() )
		return;

	switch ( pACTOR->GetCrow() )
	{
	case CROW_MOB:
		{
			GLCrow* const pCrow = (GLCrow*)pACTOR;
			if ( pCrow->IsNoAllPenalty() )
				return;

			if ( !pCrow )
				return;

			if( pCrow->IS_HAVE_SKILL_IN_SKILLFACT( sSPEC.dwLinkID ) == false )
				return;
		}
		break;
	case CROW_PC:
		{
			GLChar* const pTarget = (GLChar*)pACTOR;
			if ( !pTarget )
				return;

			if( pTarget->IS_HAVE_SKILL_IN_SKILLFACT( sSPEC.dwLinkID ) == false )
				return;
		}
		break;
	default:
		return;
	}

	if( sc::Random::RANDOM_POS() > sSPEC.fVAR1 )
		return;

	PGLSKILL			pLinkSkill	= GLSkillMan::GetInstance().GetData( sSPEC.dwNativeID );
	EMIMPACT_TARGET		emTarget	= pLinkSkill->m_sBASIC.emIMPACT_TAR;
	SKILL::SAPPLY&		sSAPPLY		= pLinkSkill->m_sAPPLY;
	SKILL::CDATA_LVL&	sSKILL_DATA	= sSAPPLY.sDATA_LVL[sSkill.wLevel];
	SSKILLACT			sSKILLACT;
	SSKILLACTEX			sSKILLACTEX;
	STARID				sTARID;	// 추가 발동스킬 타겟;

	// 자기 자신 또는 타겟 이외에는 적용되지 않는다;
	if( emTarget == TAR_SELF )
	{
		sTARID.SET( STARGETID( CROW_PC, m_dwGaeaID ) );
		pACTOR = GetTarget(sTARID);
	}
	else
	{
		sTARID.SET( sTARGETID );
	}

	int		nGATHER[NUM_HP_MP_SP]		= { 0 };
	int		nVarsArray[NUM_HP_MP_SP]	= { 0 };
	BOOL	bAsignStigma = false;
	BOOL	bCrushBlowOne = false;
	DWORD	dwDamageFlag = DAMAGE_TYPE_NONE;
	nVarsArray[IDX_HP] = 0;
	nVarsArray[IDX_MP] = 0;
	nVarsArray[IDX_SP] = 0;

	BOOL	bStigmaEff = FALSE;

	if ( pACTOR->GetNowHP() != 0 )
	{
		switch( sSAPPLY.emBASIC_TYPE )
		{
		case SKILL::EMFOR_HP:
			ApplySkillTarget_HP( pLinkSkill, sSkill.wLevel, sSKILL_DATA, pACTOR, 1, sTARID, nVarsArray[IDX_HP], dwDamageFlag );
			break;

		case SKILL::EMFOR_MP:
			ApplySkillTarget_MP( pLinkSkill, sSKILL_DATA, pACTOR, nVarsArray[IDX_MP] );
			break;

		case SKILL::EMFOR_SP:
			ApplySkillTarget_SP( pLinkSkill, sSKILL_DATA, pACTOR, nVarsArray[IDX_SP] );
			break;
		}

		if ( CalcBlocking(pACTOR, nVarsArray[IDX_HP], pLinkSkill, sSkill.wLevel) == true ) // Note : 블로킹 검사
		{
			BlockProc(sTARID, sSPEC.dwNativeID, sSkill.wLevel);
			return;
		}
		else
		{
			if( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
				bCrushBlowOne = true;

			ApplySkillTarget_Spec( pLinkSkill, sSkill.wLevel, pACTOR, dwDamageFlag, nGATHER, nVarsArray );

			// SSKILLACT
			sSKILLACT.sID			= STARGETID(CROW_PC,m_dwGaeaID);
			sSKILLACT.sID_TAR		= STARGETID(sTARGETID.emCrow,sTARGETID.GaeaId);
			sSKILLACT.fDELAY		= pLinkSkill->m_sEXT_DATA.fDELAY4DAMAGE;
			sSKILLACT.emAPPLY		= pLinkSkill->m_sBASIC.emAPPLY;
			sSKILLACT.sSkillID		= sSPEC.dwNativeID;
			sSKILLACT.wSkillLevel	= sSkill.wLevel;
			sSKILLACT.dwDamageFlag	= dwDamageFlag;
			sSKILLACT.nVAR_HP 		= nVarsArray[IDX_HP];
			sSKILLACT.nVAR_MP 		= nVarsArray[IDX_MP];
			sSKILLACT.nVAR_SP 		= nVarsArray[IDX_SP];
			sSKILLACT.nGATHER_HP 	= nGATHER[IDX_HP];
			sSKILLACT.nGATHER_MP 	= nGATHER[IDX_MP];
			sSKILLACT.nGATHER_SP 	= nGATHER[IDX_SP];

			if( sSKILLACT.VALID() )
			{
				pLand->RegSkillAct( sSKILLACT );
				bAsignStigma = true;
			}
		}


		// SSKILLACTEX
		sSKILLACTEX.sID			= STARGETID(CROW_PC,m_dwGaeaID);
		sSKILLACTEX.fDELAY		= pLinkSkill->m_sEXT_DATA.fDELAY4DAMAGE;
		sSKILLACTEX.idSKILL		= sSPEC.dwNativeID;
		sSKILLACTEX.wSKILL_LVL	= sSkill.wLevel;
		sSKILLACTEX.sID_TAR		= STARGETID(sTARID.emCrow,sTARID.dwID);

		if ( bCrushBlowOne )
		{
			sSKILLACTEX.fPUSH_PULL	= GLCONST_CHAR::fCRUSH_BLOW_RANGE;
			sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
		}

		if( ( sSAPPLY.nEffectFrame == 0 ) ||
			( sSAPPLY.nEffectFrame > 0 && sSAPPLY.nEffectFrame-1 == m_WorkingSkill.GetCurStep() ) )
		{
			ApplySkillTarget_Blow( pLinkSkill, sSkill.wLevel, pACTOR, sSKILLACTEX );
			ApplySkillTarget_Special( pLinkSkill, sSkill, sSelectTARID, sTARID, pACTOR, bCrushBlowOne, sSKILLACTEX, bStigmaEff );
		}

		if( sSAPPLY.emBASIC_TYPE == SKILL::EMFOR_CURE )
			sSKILLACTEX.dwCUREFLAG |= sSAPPLY.dwCUREFLAG;

		if ( sSKILLACTEX.VALID() )
		{
			if( ( sSAPPLY.nEffectFrame == 0 ) ||
				( sSAPPLY.nEffectFrame > 0 && sSAPPLY.nEffectFrame-1 == m_WorkingSkill.GetCurStep() ) )
				pLand->RegSkillActEx ( sSKILLACTEX );
		}

		if( pLinkSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
			ApplySkillSummons( pLinkSkill, sSkill, sTARID, pACTOR );

		if ( bAsignStigma )
		{
			SSKILLACT sSKILLACT_STIGMA;
			sSKILLACT_STIGMA.fDELAY			= pLinkSkill->m_sEXT_DATA.fDELAY4DAMAGE;
			sSKILLACT_STIGMA.emAPPLY		= pLinkSkill->m_sBASIC.emAPPLY;
			sSKILLACT_STIGMA.sSkillID		= sSkill.sNativeID;
			sSKILLACT_STIGMA.wSkillLevel	= sSkill.wLevel;
			sSKILLACT_STIGMA.dwDamageFlag	= dwDamageFlag | DAMAGE_TYPE_STIGMA;
			sSKILLACT_STIGMA.nVAR_HP 		= nVarsArray[IDX_HP];
			sSKILLACT_STIGMA.nVAR_MP 		= nVarsArray[IDX_MP];
			sSKILLACT_STIGMA.nVAR_SP	 	= nVarsArray[IDX_SP];
			sSKILLACT_STIGMA.nGATHER_HP 	= nGATHER[IDX_HP];
			sSKILLACT_STIGMA.nGATHER_MP 	= nGATHER[IDX_MP];
			sSKILLACT_STIGMA.nGATHER_SP 	= nGATHER[IDX_SP];

			StigmaProc_Skill(&sSKILLACT_STIGMA, bStigmaEff);
		}
	}
}

void GLChar::ApplySkillTarget_CASpecial( PGLSKILL				pSkill,
									     const SCHARSKILL&		sSkill,
									     const STARGETID&		sSelectTARID,
									     const STARGETID&		sTARID,
									     ServerActor*			pTarget, 
									     BOOL&					bCrushBlowOne,
									     SSKILLACTEX&			sSKILLACTEX,
										 SSKILLACTEX&			sCASKILLACTEX,
										 BOOL& bStigmaEffect )
{
	//if( pTarget->GetCrow() == CROW_PC )
	{
		CSkillCAIdx* pCASkill = NULL;

		switch( pTarget->GetCrow() )
		{
		case CROW_PC :
			{
				GLChar* pTarChar = dynamic_cast<GLChar*>(pTarget);

				pCASkill = pTarChar->GetSkillCA();
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarChar = dynamic_cast<GLCrow*>(pTarget);

				pCASkill = pTarChar->GetSkillCA();
			}
			break;
		default :
			{
				ApplySkillTarget_Special( pSkill, sSkill, sSelectTARID, sTARID, pTarget, bCrushBlowOne, sSKILLACTEX, bStigmaEffect );
			}
			return;
		}
		
		GLSKILL tempSkill = *pSkill;

		tempSkill.m_sAPPLY.vecSPEC.clear();
						
		for( size_t i = 0; i < pCASkill->m_Apply.size(); ++i)
		{
			SKILL::SSPEC_ADDON* pTemp = pCASkill->m_Apply[i];

			if( pTemp != NULL )
				tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
		}

		ApplySkillTarget_Special(&tempSkill, sSkill, sSelectTARID, sTARID, pTarget, bCrushBlowOne, sSKILLACTEX, bStigmaEffect );

		if( GetGaeaID() == pTarget->GetGaeaID() ) return;

		tempSkill.m_sAPPLY.vecSPEC.clear();

		for( size_t i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
		{			
			SKILL::SSPEC_ADDON* pTemp = m_SkillCAIndex.m_Apply[i];

			if( pTemp != NULL )
				tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
		}

		ApplySkillTarget_Special(&tempSkill, sSkill, STARGETID(GetCrow(), GetGaeaID()), STARGETID(GetCrow(), GetGaeaID()), this, bCrushBlowOne, sCASKILLACTEX, bStigmaEffect );
	}

}

void GLChar::ApplySkillTarget_Special( PGLSKILL					pSkill,
									   const SCHARSKILL&		sSkill,
									   const STARGETID&			sSelectTARID,
									   const STARGETID&			sTARID,
									   ServerActor*				pTarget, 
									   BOOL&					bCrushBlowOne,
									   SSKILLACTEX&				sSKILLACTEX, 
									   BOOL& bStigmaEffect  )
{
	for ( DWORD k = 0; k < pSkill->m_sAPPLY.vecSPEC.size(); ++k )					// 스킬 특수 기능.
	{
		const SKILL::SSPEC_ADDON&		sSpecAddOn	= pSkill->m_sAPPLY.vecSPEC[k];
		const SKILL::SSPEC&				sSPEC = sSpecAddOn.sSPEC[sSkill.wLevel];

		if ( pTarget->IsImmuneToSkill( static_cast< BYTE >( sSpecAddOn.emSPEC ) ) )
			continue;

		switch( sSpecAddOn.emSPEC )
		{
		case EMSPECA_RECBLOW:			// 상태 이상 치료.
			sSKILLACTEX.dwCUREFLAG |= sSPEC.dwFLAG;
			break;

		case EMSPECA_EFFECT_REMOVE:
            {
                if ( sSPEC.dwFLAG >= 0 && sSPEC.dwFLAG < SKILL::EMACTION_TYPE_NSIZE )
                    sSKILLACTEX.dwRemoveFlag[sSPEC.dwFLAG] = true;
            }
			break;

		case EMSPECA_CRUSHING_BLOW:
			ApplySkillTargetCrushingBlow( sSPEC, pTarget, bCrushBlowOne, sSKILLACTEX );
			break;

		case EMSPECA_REBIRTH:
			ApplySkillTargetRebirth( pTarget, sSPEC );
			break;

		case EMSPECA_PULLIN:
			if( ApplySkillTargetPullin( this, pTarget, sSPEC, sSKILLACTEX ) )
				bCrushBlowOne = false;
			break;

		case EMSPECA_ONWARD:
			if( ApplySkillTargetOnward( this, pTarget, sSelectTARID, sSPEC, sSKILLACTEX ) )
				bCrushBlowOne = false;
			break;

		case EMSPECA_KNOCKBACK:
			if( ApplySkillTargetKnockback( this, pTarget, sSPEC, sSKILLACTEX ) )
				bCrushBlowOne = false;
			break;

		case EMSPECA_STUN:
			ApplySkillTargetStun( pTarget );
			break;

		case EMSPECA_HP_DIV:
		case EMSPECA_MP_DIV:
		case EMSPECA_SP_DIV:
			break;		
		case EMSPECA_STIGMA:
			ApplySkillTargetStigma(sTARID, sSPEC, pSkill);			
			bStigmaEffect = TRUE;
			break;		
		case EMSPECA_SKILL_LINK:
			ApplySkillTargetSkillLink(sSelectTARID, sTARID, sSPEC, sSkill);
			break;
		case EMSPECA_SUMMON_CHANGE_SKILLPAGE:
			ChangeSummonSkillPage( static_cast< DWORD >( sSPEC.fVAR1 ), pSkill->GetId() );
			break;
		case EMSPECA_SUMMON_EVENT_SKILL:
			UseSummonEventSkill( static_cast< DWORD >( sSPEC.fVAR1 ), pSkill->GetId() );
			break;
		case EMSPECA_SUMMON_TARGETING:
			SummonTargeting( sTARID, pSkill->GetId() );
			break;
		case EMSPECA_SUMMON_RUNAWAY:
			SummonRunAway( pSkill->GetId() );
			break;

		case EMSPECA_REMOVE_SKILL_EFFECT:
			{
				RemoveSkillFactBySpecial( static_cast< EMSPEC_ADDON >( sSPEC.nVAR1 ) );
			}
			break;
		}; //switch		
	} //for
} //GLChar::ApplySkillTarget_Special

void GLChar::ApplySkillSummons( PGLSKILL pSkill, const SCHARSKILL& sSkill, const STARID& _sTARID, ServerActor* pACTOR )
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
        sc::writeLogError(sc::string::format("GLChar::ApplySkillSummons() - Char(%1%)'s pLand is NULL!", m_CharDbNum));
        return;
    }

	D3DXVECTOR3			vEventPos;
	if ( pACTOR->GetCrow() == GetCrow() && pACTOR->GetGaeaID() == GetGaeaID() )			// Note : 대상이 자기 자신이라면 타겟포인트에 소환한다.
	{
		vEventPos = m_TargetID.vPos;
	}
	else
	{
		FLOAT		fRatio		= 1.0f;
		const PCROWDATA	pCrowData	= GLCrowDataMan::GetInstance().GetCrowData ( pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].sID );
		if (pCrowData && 
			(pCrowData->m_emSummonType < SUMMON_TYPE_SIZE && 
			pCrowData->m_emSummonType != SUMMON_TYPE_NONE))
		{
			fRatio = GLCONST_SUMMON::fSummonGateDistanceRatio[pCrowData->m_emSummonType];
		} //if

		D3DXVECTOR3 vDir    = pACTOR->GetPosition() - m_vPosition;
		FLOAT       fLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		vEventPos = m_vPosition + (vDir * (fLength*fRatio));
	} //if..else

	// 충돌Object와 충돌이 된다면, 내 위치에서 생성되도록 한다.
	if ( pLand->IsLayerSimpleCollision( m_vPosition, vEventPos, 5.f ) )
		vEventPos = m_vPosition;

	// Note : 네비에 세울수 있는지 검사한다
	D3DXVECTOR3		vCollision;
	if ( !pLand->IsCollisionNavi ( vEventPos+D3DXVECTOR3(0,+10,0), vEventPos+D3DXVECTOR3(0,-10,0), vCollision ) )
	{
		boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
		if ( spNavi )
		{
			DWORD CurrentCellID = spNavi->FindClosestCell(vEventPos);
			vEventPos = spNavi->SnapPointToCell(CurrentCellID, vEventPos);
		}
	}
	else
	{
		vEventPos = vCollision;
	}

	// Note : 주인의 무기 강화 횟수를 구한다.
	//
	EMSLOT      emSlot      = GetCurRHand();
	SITEMCUSTOM &ItemCustom = m_PutOnItems[emSlot];
	BYTE        cGrade      = ItemCustom.GETGRADE( EMGRINDING_DAMAGE );

	// Note : 주인의 무기 속성을 구한다.
	ITEM::SSATE_BLOW sSTATEBLOW;
	if ( SITEM* pITEM = GET_ELMT_ITEM () )
	{
		sSTATEBLOW = pITEM->sSuitOp.sBLOW;
	}

	DWORD dwLandEventGUID    = -1;
	FLOAT fSummonTime        = (FLOAT)pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].fSEC_RECALLDELAY;
	FLOAT fSummonRemaindTime = (FLOAT)pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].fSEC_ACTIVETIME + m_fSUMMONACTIVETIME;
	FLOAT fEndTime           = fSummonTime;

	CEVENT_NODE_SERVER_SUMMONGATE* pEventInfo = pLand->DropLandEvent<CEVENT_NODE_SERVER_SUMMONGATE>(
        dwLandEventGUID,
		Faction::GLActorID(CROW_PC, m_CharDbNum),
        vEventPos,
        fEndTime,
        fSummonTime);

	if (!pEventInfo)
	{
		sc::writeLogError(sc::string::format("GLChar::ApplySkillSummons() - pEventInfo is NULL! Char(%1%) on MapID(%2%, %3%).", m_CharDbNum, pLand->GetBaseMapID().Mid(), pLand->GetBaseMapID().Sid()));
		return;
	}
	
	pEventInfo->m_sSkillID            = sSkill.sNativeID;
	pEventInfo->m_wLevel              = pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].wUSESKILLLEVEL;
	pEventInfo->m_sSummonNativeID     = pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].sID;
	pEventInfo->m_dwSummonMax         = pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].dwMAX;
	pEventInfo->m_dwType				= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].dwType;
	pEventInfo->m_bEventSkill			= pSkill->m_sSPECIAL_SKILL.sCREATURE_LVL[sSkill.wLevel].bEventSkill;
	pEventInfo->m_fSummonRemainedTime = fSummonRemaindTime;

	pEventInfo->m_fOwnerIncHP         = m_fBasicRateHP;          // 시전자의  HP 회복율
	pEventInfo->m_wOwnerAttack        = GETSUMMONDAMGE();		   // 시전자의  공격력
	pEventInfo->m_wOwnerDefense       = m_nDEFENSE_SKILL;          // 시전자의  방어력
	pEventInfo->m_cOwnerWeaponsGrade  = cGrade;                    // 시전자의  무기 강화
    pEventInfo->m_sOwnerBlow          = sSTATEBLOW;                // 시전자의  무기 속성
    pEventInfo->m_wOwnerSchool        = m_wSchool;                 // 시전자의  학원

	pEventInfo->m_sEvent              = STARGETID(CROW_PC, dwLandEventGUID, vEventPos);
	pEventInfo->m_sTarget             = STARGETID(_sTARID.GETCROW(), _sTARID.GETID(), pACTOR->GetPosition());
	pEventInfo->m_emNeedItemAtt		  = pSkill->m_sBASIC.emUSE_RITEM;

	// Note :주변 클라이언트에 랜드이벤트 송신
	//
	pEventInfo->RegisterViewAroundPC(m_pGaeaServer);
	
	// Note : 해당 타겟이 나를 타겟으로 잡도록 한다
	// 
	ServerActor* pActor = GetTarget( pLand, STARGETID(_sTARID.GETCROW(), _sTARID.GETID()) );

	if ( pActor )
		pActor->SetTargeting( STARGETID(GetCrow(), GetGaeaID()) );
} //GLChar::ApplySkillSummons

void GLChar::ApplySkillTarget_Blow( PGLSKILL pSkill, const WORD wSKILL_LVL, ServerActor* pACTOR, SSKILLACTEX& sSKILLACTEX )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    if( !pACTOR->IsValidBody() )
        return;

	ITEM::SSATE_BLOW sBLOW;
	sBLOW.emTYPE = EMBLOW_NONE;

	if ( pSkill->m_sAPPLY.emELEMENT == EMELEMENT_ARM )
	{
		SITEM*		pITEM = GET_ELMT_ITEM();
		if( pITEM )	
			sBLOW = pITEM->sSuitOp.sBLOW;
	}
	else
	{
		const SKILL::SSTATE_BLOW&	sSKILL_BLOW = pSkill->m_sAPPLY.sSTATE_BLOW[wSKILL_LVL];

		sBLOW.emTYPE = pSkill->m_sAPPLY.emSTATE_BLOW;
		sBLOW.fRATE  = sSKILL_BLOW.fRATE;
		sBLOW.fLIFE  = pSkill->m_sAPPLY.sDATA_LVL[wSKILL_LVL].fLIFE;
		sBLOW.fVAR1  = sSKILL_BLOW.fVAR1;
		sBLOW.fVAR2  = sSKILL_BLOW.fVAR2;
	} //if..else

	if( sBLOW.emTYPE != EMBLOW_NONE )
	{
		// 발생 확율 계산.
		//
		short	nBLOWRESIST = min( 99, pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(sBLOW.emTYPE) ) );
		DWORD	dwWeather	= GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
		float	fPOWER		= GLOGICEX::WEATHER_BLOW_POW ( sBLOW.emTYPE, dwWeather, pLand->IsWeatherActive() );
		BOOL	bBLOW		= FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(sBLOW.emTYPE)) )
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( sBLOW.fRATE * fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );

		if ( bBLOW )		// 상태이상 발생.
		{
			SSTATEBLOW		sSTATEBLOW;
			float	fLIFE = sBLOW.fLIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST/100.0f*GLCONST_CHAR::fRESIST_G ) );
			sSTATEBLOW.emBLOW = sBLOW.emTYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = sBLOW.fVAR1;
			sSTATEBLOW.fSTATE_VAR2 = sBLOW.fVAR2;
			
			if ( pACTOR->STATEBLOW( sSTATEBLOW ) )
			{
				// 상태이상 발생 저장
				sSKILLACTEX.sSTATEBLOW = sSTATEBLOW;
			}
		} //if
	} //if
} //GLChar::ApplySkillTarget_Blow


void GLChar::StigmaProc_Skill(const SSKILLACT* pSkillAct, BOOL bStigmaEffect)
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i )
	{
		SSTIGMA::SSTIGMA_PACK& sStigmaPack = m_sSTIGMA_USER.sSTIGMA_PACK[_i - 1];		
		if ( !sStigmaPack.bUse )
			continue;

		if ( !(sStigmaPack.dwFLAG & EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_SKILL) )
			continue;

		ServerActor* pACTOR = GetTarget(sStigmaPack.sTakerID);
		if ( !pACTOR )
		{
			m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
			continue;
		}
		
		if ( !pACTOR->IsValidBody() )
			m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);

		switch ( pACTOR->GetCrow() )
		{
		case CROW_PC:
			if ( ((GLChar*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);	
				continue;
			}
			break;
		case CROW_NPC:					
		case CROW_MOB:
			if ( ((GLCrow*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
				continue;
			}
			break;
		case CROW_SUMMON:
			if ( ((GLSummonField*)pACTOR)->m_sSTIGMA_TAKER.IsExistStigmaPack(STARGETID(CROW_PC, m_dwGaeaID), sStigmaPack.sSkillID) == SKILLFACT_SIZE )
			{
				m_sSTIGMA_USER.DeleteStigmaPack(_i - 1);
				continue;
			}
			break;
		}

		//if ( sStigmaPack.sTakerID != m_TargetID )
		if( !bStigmaEffect || sStigmaPack.sTakerID != m_TargetID)
		{					
			SSKILLACT sSKILLACT_STIGMA;
			sSKILLACT_STIGMA.sID			= sStigmaPack.sUserID;
			sSKILLACT_STIGMA.sID_TAR	= sStigmaPack.sTakerID;
			sSKILLACT_STIGMA.fDELAY			= pSkillAct->fDELAY;
			sSKILLACT_STIGMA.emAPPLY		= pSkillAct->emAPPLY;
			sSKILLACT_STIGMA.sSkillID		= pSkillAct->sSkillID;
			sSKILLACT_STIGMA.wSkillLevel	= pSkillAct->wSkillLevel;
			sSKILLACT_STIGMA.dwDamageFlag	= pSkillAct->dwDamageFlag | DAMAGE_TYPE_STIGMA;			
			sSKILLACT_STIGMA.nVAR_HP 		= int(float(pSkillAct->nVAR_HP) * sStigmaPack.fRate);
			sSKILLACT_STIGMA.nVAR_MP		= int(float(pSkillAct->nVAR_MP) * sStigmaPack.fRate);
			sSKILLACT_STIGMA.nVAR_SP 		= int(float(pSkillAct->nVAR_SP) * sStigmaPack.fRate);
			sSKILLACT_STIGMA.nGATHER_HP 	= int(float(pSkillAct->nGATHER_HP) * sStigmaPack.fRate);
			sSKILLACT_STIGMA.nGATHER_MP 	= int(float(pSkillAct->nGATHER_MP) * sStigmaPack.fRate);
			sSKILLACT_STIGMA.nGATHER_SP 	= int(float(pSkillAct->nGATHER_SP) * sStigmaPack.fRate);			
			pLand->RegSkillAct( sSKILLACT_STIGMA );
		}			
	}	
}

BOOL GLChar::ApplySkillTarget( PGLSKILL				pSkill,
							   const SCHARSKILL&	sSkill,
							   WORD					wAttackDivCount, 
							   SKILL::CDATA_LVL&	sSKILL_DATA, 
							   BOOL					bLowSP,
							   const STARGETID&		sSelectTargetID,
							   const WORD&			wTARNUM, 
							   const STARID*		sTARIDS )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	int 			nEXP_NUM	= 0;
	int 			nALL_LVL	= 0;
	int 			nALL_VAR[2][NUM_HP_MP_SP]	= { 0 };
	int				nGATHER[NUM_HP_MP_SP]		= { 0 };
	int				nVarsArray[NUM_HP_MP_SP] = {0};
	STARGETID		sTARID;
	BOOL bAsignStigma = false;
	DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
	BOOL bStigmaEff = FALSE;

	for( WORD i = 0; i < wTARNUM; ++i )		// 스킬이 적용되는 타겟 검사.
	{
		const STARID&		_sTARID = sTARIDS[i];

		sTARID.emCrow = _sTARID.GETCROW();
		sTARID.GaeaId = _sTARID.GETID();
		
		ServerActor* pTargetActor = GetTarget ( pLand, sTARID );
		if ( pTargetActor == NULL )
            continue;		

		// 면역스킬효과중에도 스턴등 스킬효과적용되는 문제 수정 [3/5/2014 hsshin];
		DAMAGE_SPEC*	pDamageSpec = pTargetActor->GetDamageSpecPtr();
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
			CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
		{
			BlockProc(sTARID, sSkill.sNativeID, sSkill.wLevel);
			return FALSE;
		}

		BOOL bCrushBlowOne = false;		
		BOOL bTriggerLink = false;			
		SSKILLACT sSKILLACT;

		nVarsArray[IDX_HP] = 0;
		nVarsArray[IDX_MP] = 0;
		nVarsArray[IDX_SP] = 0;

		/*======== 반격 적용 효과 결과 뽑아냄 =========================================================================================*/

		CSkillCAInterface cSkillCA;

		STARGETID sCId   = STARGETID(CROW_PC,m_dwGaeaID);;
		STARGETID sCTaId = STARGETID(_sTARID.GETCROW(),_sTARID.GETID());

		bool bCaDamage = false;

		switch( sTARID.emCrow )
		{
		case CROW_PC :
			{
				GLChar* pTarget = dynamic_cast<GLChar*>(pTargetActor);
				CSkillCAIdx* pTarCAActor = pTarget->GetSkillCA();
				
				cSkillCA.Cal_LastApplyValueII( sSkill.sNativeID, sSkill.wLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( sSkill.sNativeID, sSkill.wLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;

					pTarCAActor->SuccessCA();
				}
			}
			break;
		case CROW_MOB :
			{
				GLCrow* pTarget = dynamic_cast<GLCrow*>(pTargetActor);
				CSkillCAIdx* pTarCAActor = pTarget->GetSkillCA();

				cSkillCA.Cal_LastApplyValueII( sSkill.sNativeID, sSkill.wLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( sSkill.sNativeID, sSkill.wLevel, &m_SkillCAIndex, pTarget->GetSkillCA(), sCId, sCTaId, IsSkillLoopEnd() );

				if( sTemp != sCTaId ) {
					sCId   = sCTaId;
					sCTaId = sTemp;

					bCaDamage = true;

					pTarCAActor->SuccessCA();
				}
			}
			break;
		}
		/*===============================================================================================================================*/

		if ( pTargetActor->GetNowHP() != 0 )
		{
			dwDamageFlag = DAMAGE_TYPE_NONE;				// 물리 데미지 발생.
			switch( pSkill->m_sAPPLY.emBASIC_TYPE )						//	SKILL 기본 적용.
			{
			case SKILL::EMFOR_HP:
				if( wAttackDivCount > 0 )
					ApplySkillTarget_HP( pSkill, sSkill.wLevel, sSKILL_DATA, pTargetActor, wAttackDivCount, sTARID, nVarsArray[IDX_HP], dwDamageFlag );
				break;

			case SKILL::EMFOR_MP:
				ApplySkillTarget_MP( pSkill, sSKILL_DATA, pTargetActor, nVarsArray[IDX_MP] );
				break;

			case SKILL::EMFOR_SP:
				ApplySkillTarget_SP( pSkill, sSKILL_DATA, pTargetActor, nVarsArray[IDX_SP] );
				break;
			} //switch

			if( bLowSP )		// SP 부족시에는 변화 값을 반으로 조정.
			{
				nVarsArray[IDX_HP] >>= 1;
				nVarsArray[IDX_MP] >>= 1;
				nVarsArray[IDX_SP] >>= 1;
			} //if

			if ( CalcBlocking(pTargetActor, nVarsArray[IDX_HP], pSkill, sSkill.wLevel) == true ) // Note : 블로킹 검사
			{
				BlockProc(sTARID, sSkill.sNativeID, sSkill.wLevel);
				return FALSE;
			}
			else
			{
				if( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
					bCrushBlowOne = true;

				ApplySkillTarget_Spec( pSkill, sSkill.wLevel, pTargetActor, dwDamageFlag, nGATHER, nVarsArray );

				if ( sTARID != STARGETID( CROW_PC, m_dwGaeaID ) )		// 자기 자신에게 쓴 스킬은 제외하고 EXP 자료를 수집.
				{
					++nEXP_NUM;
					nALL_LVL += pTargetActor->GetLevel();

					nALL_VAR[0][IDX_HP] += nVarsArray[IDX_HP];
					nALL_VAR[0][IDX_MP] += nVarsArray[IDX_MP];
					nALL_VAR[0][IDX_SP] += nVarsArray[IDX_SP];

					nALL_VAR[1][IDX_HP] += pTargetActor->GetMaxHP();
					nALL_VAR[1][IDX_MP] += pTargetActor->GetMaxMP();
					nALL_VAR[1][IDX_SP] += pTargetActor->GetMaxSP();
				}

				if( bCaDamage ) 
				{
					dwDamageFlag |= DAMAGE_TYPE_CA;
				}

				// 스킬 기본 영향.
				//sSKILLACT.sID			= STARGETID(CROW_PC,m_dwGaeaID);
				//sSKILLACT.sID_TAR		= STARGETID(_sTARID.GETCROW(),_sTARID.GETID());
				sSKILLACT.sID			= sCId;
				sSKILLACT.sID_TAR		= sCTaId;
				sSKILLACT.fDELAY		= pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
				sSKILLACT.emAPPLY		= pSkill->m_sBASIC.emAPPLY;
				sSKILLACT.sSkillID			= sSkill.sNativeID;
				sSKILLACT.wSkillLevel		= sSkill.wLevel;
				sSKILLACT.dwDamageFlag	= dwDamageFlag;			
				sSKILLACT.nVAR_HP 		= nVarsArray[IDX_HP];									// 체력 변화 저장.
				sSKILLACT.nVAR_MP 		= nVarsArray[IDX_MP];
				sSKILLACT.nVAR_SP 		= nVarsArray[IDX_SP];
				sSKILLACT.nGATHER_HP 	= nGATHER[IDX_HP];										// 체력 흡수 저장.
				sSKILLACT.nGATHER_MP 	= nGATHER[IDX_MP];
				sSKILLACT.nGATHER_SP 	= nGATHER[IDX_SP];			

				if( sSKILLACT.VALID() )
				{
					pLand->RegSkillAct( sSKILLACT );
					bAsignStigma = true;
				}
			}
		}
		

		//////////////////////////////////////////////////////////////////////////
		// 스킬 보조 영향.
		SSKILLACTEX			sSKILLACTEX;
		SSKILLACTEX			sCASKILLACTEX;
		sSKILLACTEX.sID			= STARGETID(CROW_PC,m_dwGaeaID);
		sSKILLACTEX.fDELAY		= pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
		sSKILLACTEX.idSKILL		= sSkill.sNativeID;
		sSKILLACTEX.wSKILL_LVL	= sSkill.wLevel;
		sSKILLACTEX.sID_TAR		= STARGETID(_sTARID.GETCROW(),_sTARID.GETID());

		sCASKILLACTEX = sSKILLACTEX;

		//  강한 타격 확률
		if ( bCrushBlowOne )
		{
			sSKILLACTEX.fPUSH_PULL	= GLCONST_CHAR::fCRUSH_BLOW_RANGE;
			sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
		}

		if( ( pSkill->m_sAPPLY.nEffectFrame == 0 ) || //zero mean always to apply
			( pSkill->m_sAPPLY.nEffectFrame > 0 && pSkill->m_sAPPLY.nEffectFrame-1 == m_WorkingSkill.GetCurStep() ) )
		{
			ApplySkillTarget_Blow( pSkill, sSkill.wLevel, pTargetActor, sSKILLACTEX );												// 상태 이상 유발.
			//ApplySkillTarget_Special( pSkill, sSkill, sSelectTargetID, sTARID, pTargetActor, bCrushBlowOne, sSKILLACTEX );			// 스킬 특수 기능.
			sCASKILLACTEX.sSTATEBLOW = sSKILLACTEX.sSTATEBLOW;
			ApplySkillTarget_CASpecial( pSkill, sSkill, sSelectTargetID, sTARID, pTargetActor, bCrushBlowOne, sSKILLACTEX, sCASKILLACTEX, bStigmaEff );			// 스킬 특수 기능.

			if( pSkill->m_sAPPLY.nEffectFrame == 0 && m_WorkingSkill.GetHitSize() > 1 && m_WorkingSkill.GetCurStep() > 0 )
			{
				bStigmaEff = FALSE;
			}
		} //if

		if( pSkill->m_sAPPLY.emBASIC_TYPE == SKILL::EMFOR_CURE )						// 상태 이상 치료.
			sSKILLACTEX.dwCUREFLAG |= pSkill->m_sAPPLY.dwCUREFLAG;

		if ( sSKILLACTEX.VALID() )
		{
			if( ( pSkill->m_sAPPLY.nEffectFrame == 0 ) || //zero mean always to apply
				( pSkill->m_sAPPLY.nEffectFrame > 0 && pSkill->m_sAPPLY.nEffectFrame-1 == m_WorkingSkill.GetCurStep() ) )
					pLand->RegSkillActExII( sSKILLACTEX, sCASKILLACTEX );
			//pLand->RegSkillActEx ( sSKILLACTEX );
		}

		//  Note : 특수발동스킬
		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
			ApplySkillSummons( pSkill, sSkill, _sTARID, pTargetActor );

		// 회복에 기여했을 때만 경험치 발생. (공격시는 제외)
		int nSUM_EXP = 0;        

		// 경험치 획득량 적용.
		//
		if ( nSUM_EXP )
		{
			// 아이템 에디트에서 입력한 경험치 배율 적용
			nSUM_EXP = int( (float)nSUM_EXP * GetExpMultiple() );

			m_sExperience.lnNow += nSUM_EXP;
			m_sExperience.LIMIT();
		}
	}

	if ( bAsignStigma )
	{		
		SSKILLACT sSKILLACT_STIGMA;
		sSKILLACT_STIGMA.fDELAY		= pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
		sSKILLACT_STIGMA.emAPPLY		= pSkill->m_sBASIC.emAPPLY;
		sSKILLACT_STIGMA.sSkillID		= sSkill.sNativeID;
		sSKILLACT_STIGMA.wSkillLevel	= sSkill.wLevel;
		sSKILLACT_STIGMA.dwDamageFlag	= dwDamageFlag | DAMAGE_TYPE_STIGMA;			
		sSKILLACT_STIGMA.nVAR_HP 		= nVarsArray[IDX_HP];	// 체력 변화 저장.
		sSKILLACT_STIGMA.nVAR_MP 	= nVarsArray[IDX_MP];
		sSKILLACT_STIGMA.nVAR_SP	 	= nVarsArray[IDX_SP];
		sSKILLACT_STIGMA.nGATHER_HP 	= nGATHER[IDX_HP];	// 체력 흡수 저장.
		sSKILLACT_STIGMA.nGATHER_MP 	= nGATHER[IDX_MP];
		sSKILLACT_STIGMA.nGATHER_SP 	= nGATHER[IDX_SP];

		StigmaProc_Skill(&sSKILLACT_STIGMA, bStigmaEff);
	}

	return TRUE;
} //GLChar::ApplySkillTarget

void GLChar::ApplySkillSelf_Special( PGLSKILL pSkill, const WORD wSKILL_LVL, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS )
{	
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	for( unsigned int i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); i++ )
	{
		switch( pSkill->m_sAPPLY.vecSPEC[i].emSPEC )
		{
		case EMSPECA_TELEPORT:			
			ApplySkillSelfTeleport(m_pGaeaServer, this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], TargetID, wTARNUM, sTARIDS );
			break;

		case EMSPECA_SWAPPOS:
			ApplySkillSelfSwapPos( this, pSkill ,wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL], wTARNUM, sTARIDS, pLand );
			break;

		case EMSPECA_HALLUCINATE:
			ApplySkillSelfHallucinate(this, pSkill, wSKILL_LVL, pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wSKILL_LVL]);
			break;		
		} //switch
	} //for
} //GLChar::ApplySkillSelf_Special

void GLChar::ApplySkillSelf( PGLSKILL pSkill, const WORD wSKILL_LVL, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS )
{
	if( ( pSkill->m_sAPPLY.nEffectFrame == 0 ) || //zero mean always to apply
		( pSkill->m_sAPPLY.nEffectFrame > 0 && pSkill->m_sAPPLY.nEffectFrame-1 == m_WorkingSkill.GetCurStep() ) )
	{
		ApplySkillSelf_Special( pSkill, wSKILL_LVL, TargetID, wTARNUM, sTARIDS );
	} //if
} //GLChar::ApplySkillSelf

// *****************************************************
// Desc: 스킬 영향을 케릭터에 적용시킨다.
// *****************************************************
BOOL GLChar::SkillProc( const SCHARSKILL&	sSkill,
					    BOOL				bLowSP, 
						const STARGETID&	TargetID, 
						WORD				wTARNUM, 
						const STARID*		sTARIDS, 
						WORD				wAttackDiv )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()) )
		return FALSE;

	// 에니메이션 정보.
	PGLSKILL				pSkill		= GLSkillMan::GetInstance().GetData( sSkill.sNativeID );
	SKILL::CDATA_LVL&		sSKILL_DATA	= pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];

	if( !bLowSP )		// SP가 부족하지 않을때에는 SP 소모시킴.
	{
		// 기본적으로 SP 소모량이 계산되어있는 m_wSUM_DisSP에는;
		// 무기 사용시 sp 소모되는 경우가 있다(과학부무기);
		// 여기서 스킬 사용시 링크 스킬이 있을 경우 스킬, 링크스킬 둘다 동작하여 두번빠지게된다;
		// 스킬 사용시 원본스킬이 아니면 기본적으로 계산되어있는 소모량은 계산되지않도록 한다;
		WORD wDisSP = (sSkill.sNativeID == m_WorkingSkill.GetId() ? m_wSUM_DisSP : 0) + sSKILL_DATA.wUSE_SP;
		m_sSP.DECREASE ( wDisSP );
	} //if

	WORD wMAX_TAR	= 0;
	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
		wMAX_TAR = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1;
	else if( pSkill->m_sBASIC.emIMPACT_REALM == REALM_FANWIZE )
		wMAX_TAR = sSKILL_DATA.wTARNUM * ( sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE() + 1 );
	else
		wMAX_TAR = sSKILL_DATA.wTARNUM;

	if( m_wTARNUM > wMAX_TAR )
		m_wTARNUM = wMAX_TAR;

	SkillBasicProc(pSkill, sSkill, wTARNUM, sTARIDS);
	ApplySkillSelf(pSkill, sSkill.wLevel, TargetID, wTARNUM, sTARIDS);

	// 지면 타겟 스킬이 아니라면;
	if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_LAND )
	{
		if ( ApplySkillTarget( pSkill, sSkill, wAttackDiv, sSKILL_DATA, bLowSP, TargetID, wTARNUM, sTARIDS ) == FALSE )
			return FALSE;
	}

	// 스킬 사용에 따른 소모량 FB MSG.
	GLMSG::SNETPC_SKILLCONSUME_FB		NetMsgFB;
	EMSLOT		emLHand = GetCurLHand();
	SITEM*		pItem	= GET_SLOT_ITEMDATA(emLHand);
	if ( pItem && (pItem->sDrugOp.bInstance) )
		NetMsgFB.wTurnNum = m_PutOnItems[emLHand].wTurnNum;

	NetMsgFB.nNowHP = m_sHP.nNow;
	NetMsgFB.nNowMP = m_sMP.nNow;
	NetMsgFB.nNowSP = m_sSP.nNow;

	SendToClient( &NetMsgFB );

	// 소모성 아이탬 유효성 검사후, 사라질때 메시지 발생.
	//
	CheckInstanceItem ();

	return TRUE;
}

BOOL GLChar::CheckSkillToStart( const SNATIVEID& idSkill, EMSKILLCHECK& emCHECK )
{
	DWORD		dwSkillCheckOption = 0;
	if( IsDefenseSkill() )
		dwSkillCheckOption |= EMSKILLCHECKOPTION_DEFENSESKILL;

	emCHECK = GLCHARLOGIC::CHECHSKILL( idSkill, 1, m_emVehicle, dwSkillCheckOption );
	if ( emCHECK != EMSKILL_OK && emCHECK != EMSKILL_NOTSP )		// 스킬 구동 실패 FB 메시지.
	{
		GLMSG::SNETPC_REQ_SKILL_FB NetMsgFB;
		NetMsgFB.emSKILL_FB = emCHECK;
		SendToClient( &NetMsgFB );
		return FALSE;
	} //if
	return TRUE;
} //GLChar::CheckSkillToStart

BOOL GLChar::GetSkillRange( const SNATIVEID& idSkill, float& fRange )
{
	SCHARSKILL	sSkill;
	if( !GetSkillLevel( idSkill, sSkill ) )
		return FALSE;

	PGLSKILL	pSkill		= GLSkillMan::GetInstance().GetData( idSkill );		//	스킬 정보 가져옴.
	fRange = (float)( GETBODYRADIUS() + GETSKILLRANGE_TAR(*pSkill) +			//	자신의 동체반경 + 스킬타겟영역 + 스킬적용영역 + 20(여유값)
			 GETSKILLRANGE_APPLY( *pSkill, sSkill.wLevel ) + 20 );

	return TRUE;
} //GLChar::GetSkillRange

SANIATTACK* GLChar::GetAniAttack( EMANI_MAINTYPE eSkillMType, EMANI_SUBTYPE eSkillSType )
{
	GLDistinguishSkinManager::sTransAni* pAniSet = NULL;

	BOOL bDistinguish = RF_DISGUISE( m_EffSkillVarSet ).IsOn();

	if( bDistinguish )
	{
		UINT iID = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

		SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
		SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
		if ( pItem )
		{
			// Table ID 가 같을 경우에만 Index를 변경한다;
			// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
			if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
				iID = pItem->sPet.sPetID.Sid();
		}

		pAniSet = GLDistinguishSkinManager::GetInstance().GetAniSet( m_wSex, iID );
	}

	GLCONST_CHARCLASS&	ConstCharClass	= GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];
	VECANIATTACK&		vecAniAttack	= (pAniSet != NULL ) ? pAniSet->m_ANIMATION[eSkillMType][eSkillSType] : ConstCharClass.m_ANIMATION[eSkillMType][eSkillSType];

	if ( vecAniAttack.empty() )
	{
		// 캐릭터의 스킬에니메이션이 없습니다.
// 		sc::writeLogError(
// 			sc::string::format(
// 			"MsgReqSkill skill animation. ChaClass %1% animation %2%/%3%",
// 			m_CHARINDEX,
// 			eSkillMType,
// 			eSkillSType) );

		return NULL;
	} //if

	return &vecAniAttack[0];
} //GLChar::GetAniAttack

/**
 * current set a attack animation.
 */
SANIATTACK* GLChar::GetAniAttack()
{
	return GetAniAttack( m_SKILLMTYPE, m_SKILLSTYPE );
} //GLChar::GetAniAttack

// *****************************************************
// Desc: 시동중인 스킬관련 정보 갱신
// *****************************************************
BOOL GLChar::SkillProcess ( float fElapsedTime )
{
	//CONSOLEMSG_WRITE( "SkillProcess Start [%u]", m_idACTIVESKILL.dwID );

	// 캐릭터가 배운 스킬 정보 가져옴.
	//
	if ( m_sCONFTING.IsPOWERFULTIME() )				// 대련 종료후 무적타임이면 공격 무시
		return FALSE;

	if( m_WorkingSkill.SendToClientNoTarget() )	// 스킬 유효반경 내에 타겟이 없을 경우 (단 지면위치 스킬은 제외)
		return FALSE;

	const float fAccumTime = (GLCHARLOGIC::GETATT_ITEM() + GLCHARLOGIC::GETATTVELO()) * fElapsedTime * UNITANIKEY_PERSEC;	
	m_fattTIMER += fAccumTime;

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	if ( m_WorkingSkill.MoveNext() == FALSE )
	{ //it's over time
		m_fattTIMER = 0.0f;
		return FALSE;
	} //if

	return TRUE;
}

/**
 * 
 */
void GLChar::HitDamage( const SSTRIKE& sStrike )
{
	if( !sStrike.bHit )
	{
		AvoidProc();
		return;
	} //if

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()) )
		return;

	int nDamage = 0;
	DWORD dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, pLand );
	ServerActor* const pTargetActor = GetTarget( pLand, m_TargetID );
	if( pTargetActor != NULL )
		pTargetActor->ReactDamage( this, dwDamageFlag );
	
	if( CalcBlocking(pTargetActor, nDamage) == true )
		BlockProc( m_TargetID );
	else 
		DamageProc( nDamage, dwDamageFlag, sStrike.bLowSP );
} //GLChar::HitDamage


BOOL GLChar::ApplySkill( const SCHARSKILL& sSkill, const SSTRIKE& sStrike, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, WORD wAttackDiv )
{
	if ( sStrike.bHit )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );

		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
			return FALSE;		
		
		return SkillProc( sSkill, sStrike.bLowSP, TargetID, wTARNUM, sTARIDS, wAttackDiv );		
	}

	AvoidProc(sSkill.sNativeID, sSkill.wLevel, sStrike.bLowSP);

	return FALSE;
} //GLChar::ApplySkill

void GLChar::ApplySkillDealy( const SCHARSKILL& sSkill, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, WORD wAttackDiv )
{
	SSTRIKE sStrike;
	sStrike.bLowSP =false;
	sStrike.bHit =true;

	if ( sStrike.bHit )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );

		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
			return;		

		SkillProc( sSkill, sStrike.bLowSP, TargetID, wTARNUM, sTARIDS, wAttackDiv );			
	}
	else
		AvoidProc(sSkill.sNativeID, sSkill.wLevel, sStrike.bLowSP);

} //GLChar::ApplySkill

void GLChar::ACCOUNTBIKESKILL ( SNATIVEID skill_id, WORD wStrikeNum, bool bServer )
{
	// 캐릭터가 배운 스킬 정보 가져옴.
	// 발동스킬은 배운스킬 목록에 없음
	SCHARSKILL sSkill;
	if( !GetSkillLevel( skill_id, sSkill ) )
		return;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( pSkill == NULL )
		return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];


	//	스킬 딜래이 등록.
    SKILL::SKILLDELAY sSkilldelay;
	const float fDecreaseT(sSKILL_DATA.fDELAYTIME*GetDecR_SkillDelayT(pSkill->m_sBASIC.dwGRADE-1))
		, fDelayTime(sSKILL_DATA.fDELAYTIME-fDecreaseT);
	sSkilldelay.fDelayTime = GLOGICEX::SKILLDELAY(pSkill->m_sBASIC.dwGRADE,sSkill.wLevel,GETLEVEL(), fDelayTime);

	sSkilldelay.fDefultMaxDelayTime = sSkilldelay.fDelayTime;
	// 상태이상의 딜래이 감안.
	sSkilldelay.fDelayTime = sSkilldelay.fDelayTime * m_fSTATE_DELAY;
    sSkilldelay.fMAXDelayTime = sSkilldelay.fDelayTime;


	// 서버에서는 메시지 딜래이 많큼 감소시켜 준다.
	if ( bServer )
		APPLY_MSGDELAY ( sSkilldelay.fDelayTime );

	m_SKILLDELAY.insert ( std::make_pair(skill_id.dwID,sSkilldelay) );

	EMSLOT emLHand = GetCurLHand();

	if ( sSKILL_DATA.nUSE_BATTERY != 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(SLOT_VEHICLE);
		if ( pItem )
		{
			//	배터리 남은 갯수는 점검하지 않는다.
			int& nTurnNum(m_pVehicle->m_nFull);

			const int nUseBattery(sSKILL_DATA.nUSE_BATTERY);
			nTurnNum -= (nTurnNum >= nUseBattery ? nUseBattery : 0);
		}
	}

	//	기초 체력 소모.
	m_sHP.DECREASE(int(sSKILL_DATA.wUSE_HP * wStrikeNum));
	m_sMP.DECREASE(int(sSKILL_DATA.wUSE_MP * wStrikeNum));
	m_sSP.DECREASE(int(sSKILL_DATA.wUSE_SP * wStrikeNum));

	//  CP 소모 
	m_sCP.DECREASE(sSKILL_DATA.wUSE_CP * wStrikeNum);
}

// 클라 객체와 동일해야한다;
void GLChar::UpdateActState(void)
{
	if ( IsActState( EM_ACT_EMERGENCY ) )
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GetMaxHP()));
		if ( GLCONST_CHAR::fEmergency_CharHPR < fHPRate )
		{
			ReSetActState( EM_ACT_EMERGENCY );
		}
	}
	else
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GetMaxHP()));
		if ( fHPRate <= GLCONST_CHAR::fEmergency_CharHPR )
		{
			SetActState( EM_ACT_EMERGENCY );
		}
	}
}

HRESULT GLChar::UpdateClientState ( float fElapsedTime )
{
	// 증가수치를 클라이언트가 예측을 하게 되면? 메세지 발생을 안해도 된다.
	//		단 약간의 오차가 발생할 소지가 있음.
	//
	m_fSTATE_TIMER += fElapsedTime;
	if ( m_fSTATE_TIMER > 1.6f )
	{
		m_fSTATE_TIMER = 0.0f;

		// 자신, 대련자에게 hp,mp 값 갱신.
		MsgSendUpdateState();
	}

	m_fMBR_TIMER += fElapsedTime;
	if ( m_fMBR_TIMER > 1.6f )
	{
		m_fMBR_TIMER = 0.0f;
				
		const GLPartyID& PartyID = GetPartyID();
		if ( PartyID.isValidParty() == true )
		{
			GLMSG::SNET_PARTY_MBR_UPDATE NetMsg(PartyID, m_dwGaeaID, m_vPosition, m_sHP, m_wLevel);
			SendToParty(PartyID, &NetMsg);
			// 다른 필드 서버의 파티원에게도 정보를 주기 위해;
			// 에이전트 서버를 경유한다;
			m_pGaeaServer->SENDTOAGENT(&NetMsg);
		}
	}
	return S_OK;
}

void GLChar::UpdateMapEffect()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	//	선도전맵에선 다른 지형효과가 적용된다.
	if ( pLand->m_bGuidBattleMap ) 
	{
		GLGuidance* pGuid = m_pGaeaServer->GuidanceFind(pLand->m_dwClubMapID);
		if ( !pGuid || !pGuid->m_bMapEffectUse )
            return;

		//	선도전 종료
		if ( !pGuid->IsBattle() ) 
		{
			DISABLEALLMAPEFF();
			return;
		}

		std::tr1::shared_ptr<GLClubField> pClub = GetClub( m_ClubDbNum );
		std::tr1::shared_ptr<GLClubField> pGuidClub = GetClub( pGuid->m_dwGuidanceClub );
		
		if ( pClub && pGuidClub ) 
		{
			//	인증 클럽 및 동맹에게 효과 추가한다. 
			//	인증이 바뀔경우 맵에서 다 튕기므로 삭제되는 경우는 없다.
			//	다른 형태의 맵 효과가 추가되면 삭제 코드도 만들어줘야 합니다.
			
			//	같은 클럽이거나 동맹인지 체크
			if ( pClub->IsAllianceGuild_COMMON( pGuidClub->m_DbNum, pGuidClub->m_dwAlliance ) )
			{
				int nMax = pGuid->m_nMapEffectIndex + 1;
				int nSize = (int)m_vecMapEffect.size();

				for ( int i = nSize; i < nMax; ++i ) 
				{
					ADDMAPEFF( pGuid->m_vecMapEffect[i].sMapEffect );

					GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD NetMsg;
					NetMsg.bADD = true;
					NetMsg.sMapEffect = pGuid->m_vecMapEffect[i].sMapEffect;
					NetMsg.dwGaeaID	= m_dwGaeaID;	

					SendToClient( &NetMsg );
					SendMsgViewAround(&NetMsg);
				}

				return;
			}
		}
	}
	
	//	여기까지 오면 해당사항 없는것 이다.
	DISABLEALLMAPEFF();
	return;
}

void GLChar::UpdateLandEffect()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_LANDEFFECT) ) 
	{
		DISABLEALLLANDEFF();
		return;
	}

	if ( m_vPosition == m_vBackupPosition ) return;

	if ( m_vPosition != m_vBackupPosition ) 
		m_vBackupPosition = m_vPosition;
	
	const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();	
	for( DWORD i = 0; i < pLevelEtcFunc->m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT landEffect = pLevelEtcFunc->m_vecLandEffect[i];
		bool bApply = landEffect.IsApplyEffect( m_vPosition );

		SLANDEFFECT_VALUE landEffectValue(landEffect,i);

		VEC_LANDEFF_VALUE_ITER pos = std::find( m_vecLandEffect.begin(), m_vecLandEffect.end(), landEffectValue );

		if ( bApply ) 
		{
			if ( pos == m_vecLandEffect.end() ) 
			{
				// 추가해줘야 된다.
				ADDLANDEFF( landEffectValue );

				// 클라이언트 갱신
				GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD NetMsg;
				NetMsg.bADD = true;
				NetMsg.landEffect = landEffectValue;
				NetMsg.dwGaeaID	= m_dwGaeaID;	

				SendToClient( &NetMsg );
				SendMsgViewAround(&NetMsg);
			}
		}
		else
		{
			if ( pos != m_vecLandEffect.end() ) 
			{

				//	삭제해줘야 한다.
				m_vecLandEffect.erase( pos );

				// 클라이언트 갱신
				GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD NetMsg;
				NetMsg.bADD = false;
				NetMsg.landEffect = landEffectValue;
				NetMsg.dwGaeaID	= m_dwGaeaID;	

				SendToClient( &NetMsg );
				SendMsgViewAround(&NetMsg);
			}
		}
	}

	// 모든 버프를 취소해야할 경우의 처리
	for( DWORD i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			DisableSkillFact();
			break;
		}
	}
}

void GLChar::UpdateMacroState( float fElapsedTime )
{
	if ( IsActState(EM_ACT_MACROMODE) == false )
		return;

	if ( GLUseFeatures::GetInstance().IsUsingMacro_LimitTime() == TRUE )
	{
		m_fRemainMacroTime -= fElapsedTime;
		// 종료를 알려야함;
		if ( m_fRemainMacroTime <= 0.0f )
		{
			m_fRemainMacroTime = 0.0f;
			GLMSG::SNET_MACROMODE_CLOSE_FC msg;
			msg.dwMsg = GLMSG::SNET_MACROMODE_CLOSE_FC::MACROCLOSE_TIMEOVER;
			SendToClient(&msg);

			MacroOnOff(false);
		}
	}
}

void GLChar::ReserveAction( EMRESERVED_ACTION emReservedAction )
{
	if ( emReservedAction < EMRESERVED_ACTION_CTF_QUIT &&
		emReservedAction >= EMRESERVED_SIZE )
	{
		return;
	}

	m_bReservedActionFlags[ emReservedAction ] = true;
}

void GLChar::CancelReserveAction( EMRESERVED_ACTION emReservedAction )
{
	if ( emReservedAction < EMRESERVED_ACTION_CTF_QUIT &&
		emReservedAction >= EMRESERVED_SIZE )
	{
		return;
	}

	m_bReservedActionFlags[ emReservedAction ] = false;
}

void GLChar::CancelAllReserveAction()
{
	for ( int i=0; i<EMRESERVED_SIZE; ++i )
		m_bReservedActionFlags[ i ] = false;
}

BOOL GLChar::ShockProcess ( float fElapsedTime )
{
	SANIATTACK*		pAniShock = GetAniAttack( AN_SHOCK, AN_SUB_NONE );
	if( !pAniShock )
		return FALSE;

	m_fIdleTime += fElapsedTime * GETMOVE_TOTAL();

	int wTotalKeys = int(pAniShock->m_dwETime) - int(pAniShock->m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);
	
	if ( wThisKey >= wTotalKeys )
		return FALSE;

	return TRUE;
}

float GLChar::GetMoveVelo ()
{
    return GLOGICEX::GetMoveVelocity(
        IsActState(EM_ACT_RUN),
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO,
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO,
        m_pVehicle->m_bUseBoost ? true : false,
        GLCHARLOGIC::GETMOVEVELO(),
        GLCHARLOGIC::GETMOVE_ITEM(),
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max,
        IsCDMSafeTime(),
        GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED);
    /*
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = 0.0f;
	if( m_pVehicle->m_bUseBoost )
	{
		fMoveVelo = fDefaultVelo * ( ( GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM() ) * 1.5f ) ;
	}
	else
	{
		fMoveVelo = fDefaultVelo * ( GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM() ) ;
	}
	
	if (IsCDMSafeTime())
        fMoveVelo = fMoveVelo * GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED;
	return fMoveVelo;
    */
}

void GLChar::ResetConfront ( EMCONFRONT_END emEND )
{
	// Note : 자신의 소환수를 해제
	//SummonAllRemove();
	KillAllChildCrow();
	//RemoveAllChildCrow();

	// 대련 종료를 (자신) 에이전트에 알림.
	GLMSG::SNETPC_CONFRONT_END2_AGT NetMsgMyAgt(emEND);
	NetMsgMyAgt.dwChaNum = CharDbNum();
	SendToAgent(&NetMsgMyAgt);

	// (자신) 클라이언트에.
	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgMyClt(emEND);
	SendToClient(&NetMsgMyClt);

	//	파티에 있는 '대련' 참가정보를 리셋.
	GLMSG::SNETPC_CONFRONT_END2_CLT_MBR NetMsgTarMbr(GAEAID_NULL);

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			GLPartyField* const pParty = GetMyParty();
			if ( pParty == NULL )
				break;

			NetMsgTarMbr.dwID = m_dwGaeaID;

			//	대련 리스트에서 제거.
			pParty->MBR_CONFRONT_LEAVE(m_dwGaeaID);

			//	파티원에게 대련에서 빠져 나감을 알림.
			SendToParty(&NetMsgTarMbr);

			//	대련 상대방 파티원에게.
			//	대련 상대방 파티원에게 대련에서 빠져 나감을 알림.
			SendToParty(pParty->GetPartyID(), &NetMsgTarMbr);
		}
		break;

	case EMCONFT_GUILD:
		{			
			std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
			if ( pCLUB )
			{
				NetMsgTarMbr.dwID = m_CharDbNum;

				// 자신의 클럽 정보에서 대련참가 멤버에서 제거.
				pCLUB->DELCONFT_MEMBER ( m_CharDbNum );

				// 자신의 클럽원에게.
				SendToClub(&NetMsgTarMbr);

				// 상대방 클럽원에게.
				std::tr1::shared_ptr<GLClubField> pTAR_CLUB = GetClub(m_sCONFTING.dwTAR_ID);
				if ( pTAR_CLUB )
				{
					SendToClub(m_sCONFTING.dwTAR_ID, &NetMsgTarMbr);
				}
			}
		}
		break;
	};

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_PARTY:
		{
			GLPartyField* pParty = GetMyParty();
			if( !pParty )
				return;

			GLChar *pMaster = GetChar(pParty->GetMasterGaeaID());
			if (!pMaster)
				return;

			// 대련종료 처리
			EndConfting();
            
			if ( m_mapID == pMaster->GetCurrentMap() )
			{	
				// 주변 유닛들에 대련종료 통보
				GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd(emEND);
				NetMsgTarBrd.dwGaeaID = m_dwGaeaID; 
				SendMsgViewAround(&NetMsgTarBrd);

				// 대련 종료, 무적 타임 설정 (자신 & 적)
				m_sCONFTING.SETPOWERFULTIME(10.0f);
			}
		}
		break;
	case EMCONFT_GUILD:
		{			
			std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
			if( !pCLUB )
				return;

			DWORD dwMasterID = pCLUB->MasterCharDbNum();
			GLChar* pMASTER = GetCharByDbNum(dwMasterID );
			if ( !pMASTER )
                return;

			// 대련종료 처리
			EndConfting();
           
			if ( m_mapID == pMASTER->GetCurrentMap() )
			{
				// 주변 유닛들에 대련종료 통보.
				//
				GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd(emEND);
				NetMsgTarBrd.dwGaeaID = m_dwGaeaID;
				SendMsgViewAround(&NetMsgTarBrd);

				m_sCONFTING.SETPOWERFULTIME ( 10.0f );
			}
		}
		break;
	default:
		{
			// 주변 유닛들에 대련종료 통보.
			//
			GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgTarBrd(emEND);
			NetMsgTarBrd.dwGaeaID = m_dwGaeaID;
			SendMsgViewAround(&NetMsgTarBrd);

			// 대련종료 처리
			EndConfting();
            
			m_sCONFTING.SETPOWERFULTIME ( 10.0f );
		}
		break;
	}
}

void GLChar::VietnamGainCalculate()
{
	if ( IsActState(EM_ACT_WAITING) )
		return; 

	if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
		return;

	// 중국 누적 시간 별 수익 변경
	CTimeSpan gameTime( 0, (int)m_sVietnamSystem.gameTime / 60, (int)m_sVietnamSystem.gameTime % 60, 0 );
	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan, crtGame15Span;
	CTime	  loginTime   = m_sVietnamSystem.loginTime;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	LONGLONG totalHours	  = crtGameSpan.GetTotalHours();

	// 5시간 이상이면 계산하지 않고 넘어간다.
	// 수익 등급이 바뀌면 메시지를 보냄
	if( totalHours >= 5 && m_dwVietnamGainType != GAINTYPE_EMPTY )
	{
		m_dwVietnamGainType = GAINTYPE_EMPTY;	

		GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
		NetMsg.dwGainType = (BYTE)GAINTYPE_EMPTY;
		SendToClient( &NetMsg );
		// 수익 50%
	}else if( totalHours >= 3 && m_dwVietnamGainType != GAINTYPE_HALF )
	{	
		m_dwVietnamGainType = GAINTYPE_HALF;	

		GLMSG::SNETPC_VIETNAM_GAINTYPE NetMsg;
		NetMsg.dwGainType = (BYTE)GAINTYPE_HALF;
		SendToClient( &NetMsg );
	}
	m_sVietnamSystem.currentGameTime = totalHours;
}

void GLChar::ChinaGainCalculate()
{
	if ( IsActState(EM_ACT_WAITING) )
		return; 

	// 18세 이상이면 리턴한다.
	if( m_sChinaTime.userAge == 1 )
		return;

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	// 중국 누적 시간 별 수익 변경
	CTimeSpan gameTime( 0, (int)m_sChinaTime.gameTime / 60, (int)m_sChinaTime.gameTime % 60, 0 );
	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan, crtGame15Span;
	CTime	  loginTime   = m_sChinaTime.loginTime;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

	// 수익 0%
	//if( m_ChinaGainType == 2 )
	//{
	//	crtGame15Span = crtTime - m_China15Time;
	//	// 15분이 지나면 메시지를 보냄
	//	if( crtGame15Span.GetTotalMinutes() > 15 )
	//	{
	//		m_China15Time	= CTime::GetCurrentTime();

	//		GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
	//		NetMsg.dwGainType = 3;
	//		m_pGLGaeaServer->SENDTOCLIENT( m_dwClientID, &NetMsg );
	//	}
	//}else
	if( m_ChinaGainType != 2 )
	{
		LONGLONG totalHours	  = crtGameSpan.GetTotalHours();
//		LONGLONG totalMinutes = crtGameSpan.GetTotalMinutes();
//		LONGLONG totalSecond  = crtGameSpan.GetTotalSeconds();

		// 수익 등급이 바뀌면 메시지를 보냄
		if( totalHours >= 5 && m_ChinaGainType != GAINTYPE_EMPTY )
			//if( totalSecond >= 60 && m_ChinaGainType != 2 )
		{
			m_China15Time	= CTime::GetCurrentTime();
			m_ChinaGainType = GAINTYPE_EMPTY;	

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = GAINTYPE_EMPTY;
			SendToClient( &NetMsg );
			// 수익 50%
		}
        else if (totalHours >= 3 && m_ChinaGainType != GAINTYPE_HALF) //}else if( totalSecond >= 50 && m_ChinaGainType != 2 )			
		{	
			m_ChinaGainType = GAINTYPE_HALF;

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = GAINTYPE_HALF;
			SendToClient( &NetMsg );
		}
        else if( totalHours >= 1 && m_ChinaGainType == 255 )
			//}else if( totalSecond >= 40 && m_ChinaGainType != 2 )
		{
			m_ChinaGainType = 0;

			GLMSG::SNETPC_CHINA_GAINTYPE NetMsg;
			NetMsg.dwGainType = 4;
			SendToClient( &NetMsg );
		}
	}
	m_sChinaTime.currentGameTime = crtGameSpan.GetTotalMinutes();
}

void GLChar::EventCalculate()
{
	if ( IsActState(EM_ACT_WAITING) )
		return; 

	SEventState sEventState = m_pGaeaServer->m_sEventState;

	if( sEventState.bEventStart == FALSE )
		return;

	// 이벤트 적용 레벨이 아니면 리턴
	if( m_bEventApply == FALSE )
		return;

	CTime	  crtTime     = CTime::GetCurrentTime();
	CTimeSpan crtGameSpan;
	CTime	  loginTime			 = m_sEventTime.loginTime;
	crtGameSpan					 = ( crtTime - loginTime );
	m_sEventTime.currentGameTime = crtGameSpan.GetTimeSpan();

	// 이벤트가 새로 시작 됨
	if( m_bEventStart == FALSE )
	{
		// 테스트 시엔 Second로 한다.
		if( crtGameSpan.GetTotalSeconds() >= sEventState.EventPlayTime )
		{
			m_bEventStart = TRUE;

			// 자신의 클라이언트에.
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START NetMsg;
			NetMsg.loginTime = m_sEventTime.loginTime;
			SendToClient( &NetMsg );
		}
	}

	if( m_bEventStart == TRUE )
	{
		if( crtGameSpan.GetTotalSeconds() >= sEventState.EventPlayTime + sEventState.EventBusterTime )
		{
			m_bEventStart = FALSE;
			m_sEventTime.loginTime = crtTime.GetTime();

			// 새로 갱신된 이벤트 시간
			GLMSG::SNET_GM_LIMIT_EVENT_RESTART NetMsg;
			NetMsg.dwChaNum = CharDbNum();
			NetMsg.restartTime = crtTime.GetTime();

			SendToAgent(&NetMsg);

			// 자신의 클라이언트에.
			GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END NetMsg2;
			NetMsg2.loginTime = m_sEventTime.loginTime;
			SendToClient(&NetMsg2);
		}
	}
}

/**
 * 적상태의 유지시간을 변경. (수정한 시간이 0 이하이면 삭제)
 *
 * @param dwCharDB					[in] 캐릭터 디비 아이디.
 * @param fExtendTime				[in] 연장시간.
 * @return 성공시 TRUE.
 */
BOOL GLChar::ModifyExpiredTimeHostile( const DWORD dwCharDB, const float fExtendTime )
{
	MAPPLAYHOSTILE_ITER		pos = m_mapPlayHostile.find( dwCharDB );
	if( pos == m_mapPlayHostile.end() )
		return FALSE;

	SPLAYHOSTILE*		pHOSTILE = pos->second;
	pHOSTILE->fTIME += fExtendTime;
	if( pHOSTILE->fTIME < 0 )
	{
		GLChar*		pChar = GetCharByDbNum( pos->first );
		if( !pChar )
			return FALSE;
		
		pChar->RemovePlayHostile( m_CharDbNum );

		GLMSG::SNETPC_PLAYERKILLING_DEL			NetMsgDel;
		NetMsgDel.dwCharID = pos->first;
		SendToClient( &NetMsgDel );
	} //if

	return TRUE;
} //GLChar::ModifyExpiredTimeHostile

void GLChar::RemoveOverTimeHostile( const float fElapsedTime )
{
	GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;		
	SPLAYHOSTILE*					pHOSTILE = NULL;
	MAPPLAYHOSTILE_ITER				pos = m_mapPlayHostile.begin();
	while( pos != m_mapPlayHostile.end() )
	{
		pHOSTILE = pos->second;
		if( !pHOSTILE )
		{
			// 적대자 삭제 알림.				
			NetMsgDel.dwCharID = pos->first;
			SendToClient( &NetMsgDel );

			sc::writeLogError(std::string("GLChar::FrameMove, pos->second = NULL"));
			pos = m_mapPlayHostile.erase( pos );
			continue;
		}

		pHOSTILE->fTIME -= fElapsedTime;
		if ( pHOSTILE->fTIME < 0 )
		{
			// 적대자 삭제 알림.				
			NetMsgDel.dwCharID = pos->first;
			SendToClient( &NetMsgDel );

			SAFE_DELETE( pHOSTILE );
			pos = m_mapPlayHostile.erase( pos );
			continue;
		}
		++pos;
	} //while
} //GLChar::RemoveOverTimeHostile

/**
 * 대련 유효성 검사.
 */
BOOL GLChar::CheckConfronting()
{
	switch( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		if( GetChar( m_sCONFTING.dwTAR_ID ) )
			return TRUE;
		break;

	case EMCONFT_PARTY:
		if ((isValidParty() && GetMyParty()) &&
			(GLPartyID(m_sCONFTING.dwTAR_ID).isValidParty() && GetParty(m_sCONFTING.dwTAR_ID)) )
		{
            return TRUE;
		} //if
		break;

	case EMCONFT_GUILD:
		bool bclubA(false), bclubB(false);
		if( (CLUB_NULL != m_ClubDbNum && GetClub(m_ClubDbNum)) &&
			(CLUB_NULL != m_sCONFTING.dwTAR_ID && GetClub(m_sCONFTING.dwTAR_ID)) )
		{
            return TRUE;
		} //if
		break;
	};

	return FALSE;
} //GLChar::CheckConfronting

void GLChar::Confronting( const float fElapsedTime )
{
	bool	bFIGHT = m_sCONFTING.UPDATE( fElapsedTime );
	if( bFIGHT )
	{ // 대련 FIGHT 알림.
		GLMSG::SNETPC_CONFRONT_FIGHT2_CLT NetMsg;
		SendToClient( &NetMsg );
	} //if

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		{
			GLChar*		pTAR_ONE = GetChar( m_sCONFTING.dwTAR_ID );
			if( !pTAR_ONE )
				return;

			if( m_sCONFTING.sOption.bBOUND )
			{
				D3DXVECTOR3	vDistance = m_vPosition - m_sCONFTING.vPosition;
				float		fDistance = D3DXVec3Length ( &vDistance );

				if( GLCONST_CHAR::fCONFRONT_ONE_DIST < fDistance )
				{
					ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
					ResetConfront ( EMCONFRONT_END_DISLOSS );
					pTAR_ONE->ResetConfront ( EMCONFRONT_END_DISWIN );
				} //if
			} //if

			//	대련이 '조건'에 의해서 취소됫을 경우.
			if( !m_sCONFTING.IsCONFRONTING() )
				break;

			const GLPartyID& PartyID = GetPartyID();
			if ( PartyID.isValidParty() && (pTAR_ONE->GetPartyID() == PartyID) )		// 서로 같은 파티인지 검사.
				ResetConfront ( EMCONFRONT_END_PARTY );
		}
		break;

	case EMCONFT_PARTY:
		if( m_sCONFTING.sOption.bBOUND )
		{
			D3DXVECTOR3 vDistance = m_vPosition - m_sCONFTING.vPosition;
			float		fDistance = D3DXVec3Length ( &vDistance );

			if ( GLCONST_CHAR::fCONFRONT_PY_DIST < fDistance )
			{
				ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
				ResetConfront ( EMCONFRONT_END_DISLOSS );
			} //if
		}
		break;

	case EMCONFT_GUILD:
		if ( m_sCONFTING.sOption.bBOUND )
		{
			D3DXVECTOR3 vDistance = m_vPosition - m_sCONFTING.vPosition;
			float		fDistance = D3DXVec3Length ( &vDistance );
			if ( GLCONST_CHAR::fCONFRONT_CLB_DIST < fDistance )
			{
				ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_CLB_LOSS_LP );
				ResetConfront ( EMCONFRONT_END_DISLOSS );
			} //if
		} //if
		break;
	}; //switch
} //GLChar::Confronting

void GLChar::FrameMoveTAIWAN_HONGKONG()
{
	if( m_ServiceProvider == SP_HONGKONG )
	{
		if( GetTempInvenMoney() != GetInvenMoney() )
		{
			if( m_bMoneyUpdate )
			{
				m_bMoneyUpdate = FALSE;
			}
			else
			{
				// 해킹으로 로그를 남김
				sc::writeLogError(
                    sc::string::format(
					    "Different Update Money!!, Account[%1%], ID[%2%], Money %3%, TempMoney %4%, Money Gap %5%", 
					    m_szUID,
					    m_szName,
					    GetInvenMoney(),
					    GetTempInvenMoney(),
					    GetInvenMoney() - GetTempInvenMoney()));
			}
			SetTempInvenMoney( GetInvenMoney() );
		}
		else
		{
			m_bMoneyUpdate = FALSE;
		}

		if( GetTempStorageMoney() != GetStorageMoney() )
		{
			if( m_bStorageMoneyUpdate )
			{
				m_bStorageMoneyUpdate = FALSE;
			}
			else
			{
				// 해킹으로 로그를 남김
				sc::writeLogError(
                    sc::string::format(
					    "Different Update Storage Money!!, Account[%1%], ID[%2%], Storage Money %3%, TempStorage Money %4%, Storage Money Gap %5%",
					    m_szUID,
					    m_szName,
					    GetStorageMoney(),
					    GetTempStorageMoney(),
					    GetStorageMoney() - GetTempStorageMoney()) );
			}
			SetTempStorageMoney( GetStorageMoney() );
		}
		else
		{
			m_bStorageMoneyUpdate = FALSE;
		}
	}
}

void GLChar::FrameMoveAction( const float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	const EMACTIONTYPE& emAction = GetAction();
	switch( emAction )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		break;

	case GLAT_MOVE:
		{
			
			// 케릭의 이동 업데이트.
			m_actorMove.SetMaxSpeed( GetMoveVelo() );
			m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );

            // 장애물판정 로직은 부하가 크고 현 로직이 비효율적으로 되어 있어서 지금 현재 딱히 사용되는 곳이 없어서 주석 처리함;
            // 만약 사용하고 싶다면 새로 만드는것을 추천;
// 			if (GetLandMan()->IsObstacle(m_actorMove.Position(), m_actorMove.GetPreviousPosition()))
// 			{
// 				SetPositionActor(m_actorMove.GetPreviousPosition());
// 				//SetPosition(m_actorMove.GetPreviousPosition());
// 				m_actorMove.Stop ();
// 				//SendGoingMsg(m_actorMove.GetPreviousPosition());
// 				//SendJumpMsg( m_actorMove.GetPreviousPosition(), true, 0 );
// 			}
// 			else
			{
				if ( !m_actorMove.PathIsActive() )
				{
					m_actorMove.Stop ();
					TurnAction ( GLAT_IDLE );
				}
			}

			// 케릭의 현재 위치 업데이트.
			//
			m_vPosition = m_actorMove.Position();

			// 케릭의 현재 방향 업데이트.
			//
			D3DXVECTOR3 vMovement(m_actorMove.NextPosition());
			if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection = vMovement - m_vPosition;
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					m_vDirect = vDirection;
				}
			}
		}
		break;

	case GLAT_ATTACK:
		{
			if ( !AttackProcess ( fElapsedTime ) )
				TurnAction ( GLAT_IDLE );

			ServerActor* pACTOR = GetTarget ( pLand, m_TargetID );
			if ( pACTOR )
			{
				D3DXVECTOR3 vDirection = pACTOR->GetPosition() - m_vPosition;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDirect = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		if( !SkillProcess ( fElapsedTime ) )		
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
//		if( !ShockProcess ( fElapsedTime ) )
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );

        /**
            GLAT_PUSHPULL 은 스킬 상태의 아류로 이해 해야 한다.
            원래 GLAT_PUSHPULL 와 GLAT_SKILL 는 다른 상태였는데 이렇게 되면
            다발성(타격점이 여러개인) 스킬은 작동이 안되어 개념을 확장 한다.

            자세한 사항은 위키에서 GLAT_PUSHPULL 키워드로 찾아 보자.

         */
        if( !SkillProcess ( fElapsedTime ) )
		{
			m_actorMove.Stop ();
			TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_MOTION:
		if( m_fMotionSec != FLT_MAX )
		{
			m_fMotionSec -= fElapsedTime;
			if( m_fMotionSec <= 0.0f )
			{
				if( m_fnOnMotionFinish )
				{
					FunctionVoid fnOnMotionFinish = m_fnOnMotionFinish;
					m_fnOnMotionFinish = NULL;
					fnOnMotionFinish();
				}
				else
				{
					TurnAction( GLAT_IDLE );
				}
			}
		}
		break;

	case GLAT_DIE:
		if ( GLHIDESET::CheckHideSet(EMHIDE_DEATH) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
		break;
	};

	if( RF_FEIGN_DEATH( m_EffSkillVarSet).IsOn() && IsActState(EM_ACT_FEIGN_DIE) )
	{
		if( emAction != GLAT_FEIGN_FALLING && 
			emAction != GLAT_FEIGN_DIE )
		{
			RemoveSkillFactBySpecialForce( EMSPECA_FEIGN_DEATH );
		}
	}
}

void GLChar::FrameMoveVehicle()
{
	if( m_emVehicle )
	{
		GLChar*		pChar = NULL;
		INT			nPassengerID = -1;
		BOOL		bMaster;
		for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			if( m_pVehicle->PassengerId(i) == m_dwGaeaID )
			{
				nPassengerID = i;
				if( i == 0 )
				{
					bMaster = TRUE;				
				}
				else
				{
					bMaster = FALSE;
					pChar = GetChar( m_pVehicle->PassengerId(0) );
				} //if..else
				break;
			} //if
		} //for

		if( !bMaster && pChar )
		{
			const D3DXVECTOR3& vPos = pChar->GetPosition();			
			SetPosition(vPos);
		} //if
	} //if
} //GLChar::FrameMoveVehicle

void GLChar::FrameMoveForcedExitMap()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// 강제 퇴장되는 맵인지 확인
	const SLEVEL_REQUIRE* pRequire = pLand->GetLevelRequire();	
	if( NULL != pRequire && true == pRequire->m_bForceExit && pRequire->IsCheckItem() )
	{
		// 진입조건 아이템 없으면 퇴장
		std::vector<SNATIVEID> vList;
		std::vector<SNATIVEID>::iterator _iter;

		if( !pRequire->IsHaveItem(this, vList))
		{
			// 시작 위치로 퇴장시킨다;

			/// 이동정보를 세팅한다;
			MoveSystem::SMOVE_INFO sMoveInfo;
			sMoveInfo.dwChaDBNum = CharDbNum();
			sMoveInfo.sMapID = m_sStartMapID;
			sMoveInfo.dwGateID = m_dwStartGate;
			sMoveInfo.vPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );

			/// 이동한다;
			MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

			/*// 퇴장... 시작위치로
			m_pGaeaServer->MoveMap( this, m_sStartMapID, m_dwStartGate, D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) );
			m_dwLastPremiumExitNotifyTime = 0;*/

			return;
		}

		_iter = vList.begin();

		const CTime cTimeCur = CTime::GetCurrentTime();

		CTime tMaxTime = 0;

		while( _iter != vList.end() )
		{
			SINVENITEM* pInvenItem = m_cInventory.FindItem( *(_iter) );

			if( pInvenItem ) {
				const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );

				if( NULL != pItem && pInvenItem->sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) && 0 != pItem->sDrugOp.tTIME_LMT )
				{
					CTimeSpan cSpan = pItem->sDrugOp.tTIME_LMT;
					CTime cTimeLimit = pInvenItem->sItemCustom.tBORNTIME;
					cTimeLimit += cSpan;
					
					if( cTimeLimit > tMaxTime )
						tMaxTime = cTimeLimit;
				}
			}

			++_iter;
		}

		if( tMaxTime == 0 ) return;

		if( tMaxTime < cTimeCur )
		{
			// 시작위치로 퇴장시킨다;

			/// 이동정보를 세팅한다;
			MoveSystem::SMOVE_INFO sMoveInfo;
			sMoveInfo.dwChaDBNum = CharDbNum();
			sMoveInfo.sMapID = m_sStartMapID;
			sMoveInfo.dwGateID = m_dwStartGate;
			sMoveInfo.vPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );

			/// 이동한다;
			MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

			/*// 퇴장... 시작위치로
			m_pGaeaServer->MoveMap( this, m_sStartMapID, m_dwStartGate, D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) );*/
			m_dwLastPremiumExitNotifyTime = 0;
		}
		else
		{
			DWORD dwNotifyTime = 0;

			CTime cTimeRemain = tMaxTime.GetTime() - cTimeCur.GetTime();
			for( size_t i = 0; i < GLCONST_CHAR::vecPremiumMapForcedExitNotify.size(); ++i )
			{
				if( GLCONST_CHAR::vecPremiumMapForcedExitNotify[ i ] == cTimeRemain.GetMinute() &&
					GLCONST_CHAR::vecPremiumMapForcedExitNotify[ i ] != m_dwLastPremiumExitNotifyTime )
				{
					dwNotifyTime = GLCONST_CHAR::vecPremiumMapForcedExitNotify[ i ];
					break;
				}
			}

			if( 0 != dwNotifyTime )
			{
				m_dwLastPremiumExitNotifyTime = dwNotifyTime;

				GLMSG::SNETPC_PREMIUM_MAP_EXIT NetMsg;
				NetMsg.dwRemainTime = m_dwLastPremiumExitNotifyTime;
				SendToClient( &NetMsg );
			}
		}
	}
}

void GLChar::FrameMove_ReservedAction()
{
	// CTF QUIT 예약 처리;
	if ( m_bReservedActionFlags[ EMRESERVED_ACTION_CTF_QUIT ] )
	{
		// 살아있다는 조건 달성 시 예약을 수행한다;
		if ( false == IsDie() )
		{
			// Send Message 'Quit CTF' to Agent
			GLMSG::SNET_CTF_QUIT_CFA msgCTFQuit;
			msgCTFQuit.dwChaNum = CharDbNum();
			SendToAgent( &msgCTFQuit );

			// 예약 해제;
			m_bReservedActionFlags[ EMRESERVED_ACTION_CTF_QUIT ] = false;
		}
	}
}

HRESULT GLChar::FrameMove(float fTime, float fElapsedTime, __time64_t CurrentTime)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

    CheckOneSec(CurrentTime);
    
	//
	//mjeon.attendance
	//
	AttendanceCheckConnection();

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		// 베트남 탐닉 방지 계산
		VietnamGainCalculate();
	}
//#endif

#ifdef CH_PARAM_USEGAIN
	// 중국 탐닉 방지 계산
	ChinaGainCalculate();
#else
	// 리미트 이벤트 계산
	EventCalculate();
#endif

	if ( !IsActState(EM_ACT_WAITING) && m_bEntryFailed )
	{
		//  진입조건이 맞지않아 다른 곳으로 이동 해야하는 경우 메시지를 보낸다.
		GLMSG::SNETENTRY_FAILED NetMsg;
		SendToClient( &NetMsg );

		m_bEntryFailed = FALSE;
	}	

	UpdateBooster( fElapsedTime );

	// 탈것 배터리 체크
	UpdateVehicle( fElapsedTime );

	if (!IsActState(EM_GETVA_AFTER) && !(IsReserveServerStop()))
        return S_FALSE;

    //  아이템 정렬 제한 시간 업데이트.
    m_fLimitTimeItemSort -= fElapsedTime;
    if (m_fLimitTimeItemSort < 0.0f)
        m_fLimitTimeItemSort = 0.0f;

    // 펫 소환 제한 시간 업데이트.
    m_fLimitTimeSummonPet -= fElapsedTime;
    if ( m_fLimitTimeSummonPet < 0.0f )
        m_fLimitTimeSummonPet = 0.0f;

	//
	//mjeon
	//m_bTracePing must be declared in GLGaeaServer and toggle by GM command
	//If m_bTracePing is turned on, pingtrace message will be sent frequently.
	//
	if (m_pGaeaServer->m_bTracePing)
	{
		m_fPingTraceTimer += fElapsedTime;
		
		if (m_fPingTraceTimer > 10.f) //300.f )		//unit: sec
		{
			m_fPingTraceTimer = 0.0f;
			m_pGaeaServer->SENDTOCLIENT_PINGREQ(m_ClientSlot);
		}	
	}

	m_fAge += fElapsedTime;
	m_fGenAge += fElapsedTime;
	m_fSkillDelay += fElapsedTime;

	if(GetViewAroundUsed())
	{
		m_fViewAroundSettingReleaseTimer-=fElapsedTime;
	}

	//m_WorkingSkillDealy.FrameMove( fElapsedTime );

	m_WorkingSkill.FrameMove( fElapsedTime );
	
	// 트레이드 상태 갱신
	if (m_sTrade.Valid())
	{
		m_sTrade.FrameMove(fTime, fElapsedTime);

		GLChar* pChar = GetChar(m_sTrade.GetTarget());		
		if (!pChar)
		{
			GLMSG::SNET_TRADE_CANCEL_TAR NetMsg;
			NetMsg.dwChaNum = CharDbNum();
			NetMsg.emCancel = EMTRADE_CANCEL_NORMAL;
			SendToClient( &NetMsg );
			m_sTrade.Reset();
		}
	}

	//	DB에 정보 저장할 시간이 되었는지 검사후에 저장 수행.
	//
	m_fSAVEDB_TIMER += fElapsedTime;
	if (m_fSAVEDB_TIMER > 900.0f)
	{
		m_fSAVEDB_TIMER = 0.0f;
		m_pGaeaServer->PreSaveCharDB ( m_CharDbNum );
		m_pGaeaServer->SaveCharDB ( m_dwGaeaID, false, true );
	}

	if (m_sCONFTING.IsCONFRONTING())
	{
		if( CheckConfronting() )
			Confronting( fElapsedTime );
		else
			ResetConfront ( EMCONFRONT_END_FAIL );
	}
	else if (m_sCONFTING.IsPOWERFULTIME())				// 대련종료후 딜레이 시간 갱신
	{
		m_sCONFTING.UPDATEPWRFULTIME(fElapsedTime);
	} //if..else

	RemoveOverTimeHostile(fElapsedTime);		// 적대 행위자의 타이머 갱신 및 관리.

	if (IsValidBody() && m_nBright < 0)
	{
		m_fBRIGHT_TIMER += fElapsedTime;
		if (m_fBRIGHT_TIMER >= GLCONST_CHAR::fPK_POINT_DEC_RATE)
		{
			m_fBRIGHT_TIMER = 0.0f;

			// 속성수치 변화.
			m_nBright += 1;

			GLMSG::SNETPC_UPDATE_BRIGHT NetMsg;
			NetMsg.nBright = m_nBright;
			SendToClient( &NetMsg );

			GLMSG::SNETPC_UPDATE_BRIGHT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.nBright = m_nBright;
			SendMsgViewAround(&NetMsgBrd);
		}
	}

	// 시간 갱신
	m_fPERIOD_TIMER += fElapsedTime;
	if (m_fPERIOD_TIMER > 360.0f)
	{
		m_fPERIOD_TIMER = 0.0f;
		MsgSendPeriod();
	}

	m_fITEM_TIMER += fElapsedTime;
	if ( m_fITEM_TIMER > 1800.0f )
	{
		m_fITEM_TIMER = 0.0f;
		RESET_TIMELMT_ITEM ();
		RESET_CHECK_ITEM ();
	}

	m_fMoveDelay -= fElapsedTime;

	if( m_fReferCharDelay > 0.0f )
		m_fReferCharDelay -= fElapsedTime;

	if ( m_sQITEMFACT.IsACTIVE() )
	{
		bool bOK = UPDATE_QITEMFACT(fElapsedTime);
		if ( !bOK )
		{
			GLMSG::SNETPC_QITEMFACT_END_BRD	NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;

			// 종료되었을 경우 자신에게 알림.
			SendToClient(&NetMsgBrd);

			// 종료되었을 경우 주변 사람에게 알림.
			SendMsgViewAround(&NetMsgBrd);
		}
	}

	if ( m_lnLastSendExp != m_sExperience.lnNow )
	{
		m_lnLastSendExp = m_sExperience.lnNow;

		// 경험치 변화 클라이언트에 알려줌.
		//
		GLMSG::SNETPC_UPDATE_EXP NetMsgExp;
		NetMsgExp.lnNowExp = m_sExperience.lnNow;
		SendToClient(&NetMsgExp);

		if ( GLHIDESET::CheckHideSet(EMHIDE_UPDATE_EXP) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}


		if ( m_sExperience.ISOVER() )
		{
			if ( GLHIDESET::CheckHideSet(EMHIDE_LEVELUP) )
			{
				RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				RestoreActStateByInvisible();
			}
			// 랩업 수행. ( 로컬 메시지 발생. )
			//
			GLMSG::SNETPC_REQ_LEVELUP NetMsg;
			MsgReqLevelUp ( (NET_MSG_GENERIC*) &NetMsg );

			m_lnLastSendExp = m_sExperience.lnNow;
		}
		else
		{
			m_bVietnamLevelUp = FALSE; 
		}
	}

	if( m_wLastSendCP != m_sCP.wNow )
	{
		m_wLastSendCP = m_sCP.wNow;

		GLMSG::SNETPC_UPDATE_CP NetMsgCP;
		NetMsgCP.wNowCP = m_sCP.wNow;
		SendToClient(&NetMsgCP);
	}

	if ( m_lnLastSendVNExp != m_lVNGainSysExp )
	{
		m_lnLastSendVNExp = m_lVNGainSysExp;

		// 경험치 변화 클라이언트에 알려줌.
		//
		GLMSG::SNETPC_VIETNAM_GAIN_EXP NetMsgExp;
		NetMsgExp.gainExp = m_lVNGainSysExp;
		SendToClient(&NetMsgExp);
	}

//#if defined(TW_PARAM) || defined(HK_PARAM) //|| defined(_RELEASED)
	FrameMoveTAIWAN_HONGKONG();
//#endif

	// 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	if ( m_TargetID.GaeaId != EMTARGET_NULL )
	{
		if ( !m_pGaeaServer->ValidCheckTarget(pLand, m_TargetID) )
			m_TargetID.GaeaId = EMTARGET_NULL;
	}
	
	if ( m_TargetID.GaeaId == EMTARGET_NULL && isAction(GLAT_ATTACK) )
		TurnAction ( GLAT_IDLE );

	// Note : 도시락 사용 가능한 지역이면 모든 LUNCHBOX를 Enable 한다
	BOOL bLunchBoxForbid = pLand->IsLunchBoxForbid();
	if ( IsValidBody() && !m_sCONFTING.IsCONFRONTING() && !bLunchBoxForbid )
	{
		SETENABLEALLLUNCHBOX ( TRUE );
	}
	else
	{
		// Note : 모든 LUNCHBOX를 Disable 한다
		SETENABLEALLLUNCHBOX ( FALSE );
	}

	if ( IsValidBody () )
	{
		if ( IsCDMSafeTime() )
			m_fCDMSafeTime -= fElapsedTime;

		DoQuestCheckLimitTime ( fTime, fElapsedTime );

		if ( m_sHP.nNow > 0 )  // 살아있다면;
		{
			float fCONFT_POINT_RATE(1.0f);
			if ( m_sCONFTING.IsCONFRONTING() )		fCONFT_POINT_RATE = m_sCONFTING.sOption.fHP_RATE;

			// 지형 이상효과 업데이트
			UpdateLandEffect();
			UpdateMapEffect();

            if ( m_bSTATE_STUN && !isAction( GLAT_PUSHPULL ) ) // 스턴중이라도 밀고당기기 액션 중이라면 움직임을 멈추지 않는다
            {
                if ( m_actorMove.PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
                {
                    m_actorMove.Stop();
                    TurnAction( GLAT_SHOCK );
                }
            }

			// 기초 LOGIC 정보를 업대이트.
			//
			GLCHARLOGIC::UPDATE_DATA ( fTime, fElapsedTime, FALSE, fCONFT_POINT_RATE );

			if( RF_FEIGN_DEATH( m_EffSkillVarSet).IsOn() )	
			{
				if( !IsActState( EM_ACT_FEIGN_DIE) ){
					SetActState( EM_ACT_FEIGN_DIE );
					TurnAction( GLAT_FEIGN_FALLING );
				}
			}
			/*else
			{
				if( IsActState( EM_ACT_FEIGN_DIE) ){
					ReSetActState(EM_ACT_FEIGN_DIE);
					TurnAction( GLAT_IDLE );
				}
			}*/


			UpdateDurDamage ( fTime, fElapsedTime );			

			m_ARoundSlot.Update ( pLand );

			DoQuestReachZone();			
		}
		else
		{
            DoFalling ();
		}		

		UpdateMacroState(fElapsedTime);
	}

	UpdateClientState ( fElapsedTime );
	UpdateActState();
	FrameMoveAction( fElapsedTime );

	// Mob의 현재 위치 업데이트.
	//
	m_vPosition = m_actorMove.Position();

	FrameMoveVehicle();

	UpdateSkillEff( fElapsedTime);

    if (IsCTFPlayer())		// CTF
    {
        m_dwActionLimit |= (EMACTION_LIMIT_PET | EMACTION_LIMIT_DRUG_HP | EMACTION_LIMIT_CARD);
    }

	m_PositionSendTimer += fElapsedTime;
	if (m_PositionSendTimer >= 10.0f)
	{
		m_PositionSendTimer = 0.0f;
		ReportPositionToAgent();
	}

	// 강제 퇴장되는 맵 확인
	m_fPremiumExitCheckTimer += fElapsedTime;
	if( m_fPremiumExitCheckTimer > GLCONST_CHAR::fPremiumMapExitCheckTime )
	{
		m_fPremiumExitCheckTimer = 0.0f;

		FrameMoveForcedExitMap();
	}

	UpdateForcedResurrect( fElapsedTime );

	/// 고용 소환수 관련;
	if ( m_sSummonable.bSummonable )
	{
		// 소환된 상태인데;
		if ( m_sSummonable.hireInfo.bSummoning /*&&
			m_sSummonable.hireInfo.bMoveMap*/ )
		{
			// 랜드에 소환수가 없다면;
			if ( !pLand->GetSummon( m_sSummonable.hireInfo.dwGaeaID ) )
			{
				// 소환;
				if ( IsValidBody() )
				{
					// 재소환을 설정한다;
					m_sSummonable.hireInfo.bResummoning = true;

					SummonCrow( SNATIVEID( false ),
						m_sSummonable.hireInfo.fRemainedTime,
						m_sSummonable.hireInfo.dwFamiliarType,
						m_sSummonable.hireInfo.bEventSkill,
						m_sSummonable.hireInfo.dwMaxSummon,
						m_sSummonable.hireInfo.wSkillLevel );

					//m_sSummonable.hireInfo.bMoveMap = false;
				}
			}
		}
	}
	else
	{
		KillAllHireChildCrow();
	}

	/// 예약 액션 수행;
	FrameMove_ReservedAction();

	/// 캐릭터 슬롯 변경 쿨타임 체크;
	if ( m_fCoolTime_ChangeCharSlot < GLCharSlotMan::Instance()->GetCoolTime_ChangeSlot() )
	{
		m_fCoolTime_ChangeCharSlot += fElapsedTime;
	}

	return S_OK;
}

bool GLChar::STATEBLOW ( const SSTATEBLOW &sStateBlow )
{
	int nIndex = 0;

	if (sStateBlow.emBLOW <= EMBLOW_SINGLE)
        nIndex = 0;
	else
        nIndex = sStateBlow.emBLOW-EMBLOW_SINGLE;

	m_sSTATEBLOWS[nIndex] = sStateBlow;

    if ( IsValidBody() )
    {
        //	상태이상 ( 기절 ) 을 받을때 현재 행위 중단.
        if ( sStateBlow.emBLOW == EMBLOW_STUN )
        {
            // 쓰러짐을 클라이언트 들에게 알림.
            GLMSG::SNET_ACTION_BRD NetMsgBrd;
            NetMsgBrd.emCrow	= CROW_PC;
            NetMsgBrd.dwID		= m_dwGaeaID;
            NetMsgBrd.emAction	= GLAT_IDLE;

            // 주변의 Char 에게.
            SendMsgViewAround(&NetMsgBrd);

            // 자신의 클라이언트에.
            SendToClient(&NetMsgBrd);

            TurnAction ( GLAT_IDLE );
        }
    }

	return true;
}

void GLChar::CURE_STATEBLOW ( DWORD dwCUREFLAG )
{
	BOOL bChanged = FALSE;

	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		if ( m_sSTATEBLOWS[i].emBLOW==EMBLOW_NONE )
			continue;

		bChanged = TRUE;
		if ( STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW)&dwCUREFLAG )
		{
			m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
		}
	}

	if ( !bChanged )
		return;

	// 상태 이상 변화 MsgFB.
	//
	GLMSG::SNETPC_CURESTATEBLOW_BRD NetMsgBRD;
	NetMsgBRD.dwID = m_dwGaeaID;
	NetMsgBRD.emCrow = CROW_PC;
	NetMsgBRD.dwCUREFLAG = dwCUREFLAG;

	// 주변 클라이언트들에게 메세지 전송.
	//
	SendMsgViewAround(&NetMsgBRD);

	// 대상에게 메세지 전송.
	SendToClient(&NetMsgBRD);
}

void GLChar::SKILL_EFFECT_REMOVE ( DWORD dwRemoveFlag )
{
    if (dwRemoveFlag >= SKILL::EMACTION_TYPE_NSIZE)
        return;

    //일반적인 스킬의 효과만 제거한다. (도시락,시스템 버프 등은 제외한다.);
    for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
    {
        if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
        if (!pSkill)
            continue;

        //제거 불가 옵션;
        if (pSkill->m_sBASIC.bNonEffectRemove)
            continue;

        //해당하는 스킬타입인지?
        if (pSkill->m_sBASIC.emACTION_TYPE != (SKILL::EMACTION_TYPE)dwRemoveFlag)
            continue;

        //활성화 여부;
        if ( !m_sSKILLFACT[i].bEnable )
            continue;

		UPDATE_DATA_END(i);        
    }
}

void GLChar::DO_STUN_ACTION ()
{
	// 쓰러짐을 클라이언트 들에게 알림.
	GLMSG::SNET_ACTION_BRD NetMsgBrd;
	NetMsgBrd.emCrow	= CROW_PC;
	NetMsgBrd.dwID		= m_dwGaeaID;
	NetMsgBrd.emAction	= GLAT_IDLE;

	// 주변의 Char 에게.
	SendMsgViewAround(&NetMsgBrd);

	// 자신의 클라이언트에.
	SendToClub(&NetMsgBrd);

	TurnAction ( GLAT_IDLE );

}

void GLChar::VAR_BODY_POINT( const EMCROW emActorCrow, const DWORD dwActorID, const DWORD dwVarTypeFlag, int& nVar_HP, int& nVar_MP, int& nVar_SP )
{
    if ( m_sHP.nNow == 0 )
        return;

    if ( nVar_HP < 0 )
        nVar_HP = -(int)ReceiveDamage(emActorCrow, dwActorID, abs(nVar_HP), dwVarTypeFlag);  // 주의: -부호에 주의해야함;
    else if ( nVar_HP > 0 )
        nVar_HP = ReceiveHeal(emActorCrow, dwActorID, abs(nVar_HP), dwVarTypeFlag);

    m_sMP.VARIATION(nVar_MP);
    m_sSP.VARIATION(nVar_SP);
}

DWORD GLChar::SELECT_SKILLSLOT ( SNATIVEID skill_id, EMSKILLFACT_TYPE emSkillFactType )
{
	DWORD dwSELECT = UINT_MAX;

	// 타입에 따라 스킬이 들어갈 슬롯 선택.
	//
	switch ( emSkillFactType )
	{
	// Note : 일반 스킬
	case EMSKILLFACT_TYPE_NORMAL:
		{
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
				if (GetSkillFactId(i) == skill_id)
				{
					return dwSELECT = i;
				}
			}

			float fAGE = FLT_MAX;
			for ( DWORD i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
			{
				if (GetSkillFactId(i) == SNATIVEID(false))
				{
					return dwSELECT = i;
				}

				if (GetSkillFactAge(i) < fAGE)
				{
					fAGE = GetSkillFactAge(i);
					dwSELECT = i;
				}
			}
		}
		break;

	case EMSKILLFACT_TYPE_ADDITIONAL:
		{
			for ( DWORD i=SKILLFACT_INDEX_ADDITIONAL_START; i<SKILLFACT_INDEX_ADDITIONAL_END; ++i )
			{
				if (GetSkillFactId(i) == skill_id)
				{
					return dwSELECT = i;
				}
			}

			float fAGE = FLT_MAX;
			for ( DWORD i=SKILLFACT_INDEX_ADDITIONAL_START; i<SKILLFACT_INDEX_ADDITIONAL_END; ++i )
			{
				if (GetSkillFactId(i) == SNATIVEID(false))
				{
					return dwSELECT = i;
				}

				if (GetSkillFactAge(i) < fAGE)
				{
					fAGE = GetSkillFactAge(i);
					dwSELECT = i;
				}
			}
		}
		break;

	// Note : 도시락 버프 스킬
	case EMSKILLFACT_TYPE_LUNCHBOX:
		{
			for ( DWORD i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_LUNCHBOX_END; ++i )
			{
				if (GetSkillFactId(i) == skill_id)
				{
					return dwSELECT = i;
				}
			}

			float fAGE = FLT_MAX;
			for ( DWORD i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_LUNCHBOX_END; ++i )
			{
				if (GetSkillFactId(i) == SNATIVEID(false))
				{
					return dwSELECT = i;
				}

				if (GetSkillFactAge(i) < fAGE)
				{
					fAGE = GetSkillFactAge(i);
					dwSELECT = i;
				}
			}
		}
		break;

		// Note : 도시락 버프 스킬
	case EMSKILLFACT_TYPE_EXP_LUNCHBOX:
		return SKILLFACT_INDEX_EXP_LUNCHBOX_START;		

    // Note : 지역 버프 스킬
    case EMSKILLFACT_TYPE_SYSTEM:
        {
            for ( DWORD i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i )
            {
                if (GetSkillFactId(i) == skill_id)
                {
                    return dwSELECT = i;
                }
            }

            float fAGE = FLT_MAX;
            for ( DWORD i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i )
            {
                if (GetSkillFactId(i) == SNATIVEID(false))
                {
                    return dwSELECT = i;
                }

                if (GetSkillFactAge(i) < fAGE)
                {
                    fAGE = GetSkillFactAge(i);
                    dwSELECT = i;
                }
            }
        }
        break;
	}

	return dwSELECT;
}

void GLChar::RestoreActStateByInvisible()
{
	if ( !IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		return;
	
	if ( m_sINVISIBLE.bPrevStateRun )
		SetActState(EM_ACT_RUN);
	else
		ReSetActState(EM_ACT_RUN);
}
void GLChar::RECEIVE_DAMAGE_AFTER(void)
{
	if ( GLHIDESET::CheckHideSet(EMHIDE_ON_HIT) ) // 은신 중이었으면 처리;
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}
}

void GLChar::AddSkillFact_SpecialAddon_Server( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF )
{
	SKILL::SSPEC_ADDON_LEVEL	sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1		= sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
	sAddon.sSPEC.fRate		= sSPEC.fRate;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
} //AddSkillFact_SpecialAddon

const bool GLChar::RECEIVE_SKILLFACT_Domination(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	if ( EMSPEC_DOMINATE_TYPE(sSPEC.dwFLAG) == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		if ( sID.emCrow == CROW_PC	)
		{
			GLChar* _pChar = m_pGaeaServer->GetChar(sID.GaeaId);
			if ( !_pChar )
				return false;

			GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
			NetMsg.sSkillID = sSkillID;
			NetMsg.wLevel = wLevel;
			NetMsg.sTargetID = STARGETID(GETCROW(), m_dwGaeaID);

			_pChar->SendToClient( &NetMsg );
		}		
	}
	return true;
}

void GLChar::RECEIVE_SKILLFACT_SpecialProvocation( const STARGETID& sID, const float fDistance )
{
    this->m_TargetID = sID;

    m_sTaunt.fTauntDistance = fDistance;
    m_sTaunt.sTauntTargetID = sID;

	GLMSG::SNETPC_SELECT_TARGET		NetMsg;
	NetMsg.emTarCrow = sID.emCrow;
    NetMsg.dwTarID = sID.GaeaId;

    SendToClient( &NetMsg );
} //GLChar::RECEIVE_SKILLFACT_SpecialProvocation

void GLChar::RECEIVE_SKILLFACT_ReleasePet(void)
{
	GLPetField* pPet = m_pGaeaServer->GetPET ( m_dwPetGUID );
	if ( pPet && pPet->DbNum () == m_PetDbNum )
	{
		m_pGaeaServer->DropOutPET ( m_dwPetGUID, false, false );
		if ( GLHIDESET::CheckHideSet(EMHIDE_PET_SUMMON_RELEASE) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
	}
} // GLChar::RECEIVE_SKILLFACT_ReleasePet

void GLChar::RECEIVE_SKILLFACT_Invisible(const float fVAR2)
{
	m_sINVISIBLE.bPrevStateRun = IsActState(EM_ACT_RUN);
	switch ( EMSPEC_INVISIBLE_ANI_TYPE(DWORD(fVAR2)) ){
	case EMSPEC_INVISIBLE_ANI_TYPE_WALK:
		ReSetActState(EM_ACT_RUN);
		break;
	case EMSPEC_INVISIBLE_ANI_TYPE_RUN:
		SetActState(EM_ACT_RUN);
		break;
	}
} //GLChar::RECEIVE_SKILLFACT_Invisible

void GLChar::RECEIVE_SKILLFACT_Stigma(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	if ( sID.emCrow == CROW_PC	)
	{
		GLChar* _pChar = m_pGaeaServer->GetChar(sID.GaeaId);
		if ( !_pChar )
			return;

		_pChar->m_sSTIGMA_USER.PushStigmaPack(sID, STARGETID(CROW_PC, m_dwGaeaID), sSkillID, sSPEC);
	}
}

void GLChar::RECEIVE_SKILLFACT_RandomExpRate(const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const float fLife, const WORD wLevel, SSKILLFACT& sSKILLEF, const float* const pLaunchTime, const float* const pLaunchValue)
{	
	const DWORD nMaxSize(sSPEC.fVAR2);

	if ( pLaunchTime == 0)
	{
		const float fRate(SKILL::ExperiencePointSettings::getInstance()->getRate(sSPEC.fVAR1));

		if ( m_ExperienceRate.push_back(sSkillID, nMaxSize, fLife, fRate) == false )
			return;

		GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
		NetMsg.sSkillID = sSkillID;
		NetMsg.wLevel = wLevel;
		NetMsg.fVAR1 = fRate;
		NetMsg.fVAR2 = nMaxSize;
		NetMsg.fLife = fLife;

		SendToClient( &NetMsg );
	}	
	else
	{
		for ( DWORD _i(MAX_SIZE); _i--; )
		{
			const float fLife(pLaunchTime[_i]);
			const float fRate(pLaunchValue[_i]);

			m_ExperienceRate.push_back(sSkillID, nMaxSize, fLife, fRate);
		}
	}
}

const BOOL GLChar::RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const SNATIVEID& sSkillID, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF, const STARGETID sID, const float* const pLaunchTime, const float* const pLaunchValue )
{
	BOOL bHold = FALSE;

	for( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON&		emAddon = pSkill->m_sAPPLY.vecSPEC[i].emSPEC;
		const SKILL::SSPEC&		sSPEC   = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel];

		if ( RF_IMMUNE( m_EffSkillVarSet ).IsOn() )
		{
			if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( static_cast< BYTE >( emAddon ) ) )
			{
				continue;
			}
		}
		
		switch( emAddon )
		{
		case EMSPECA_COUNTATTACK :
			{
				bHold = TRUE;
				sSKILLEF.pcCAData = pSkill->m_sAPPLY.sCAData.sSPEC_CA[wlevel];
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_TAUNT:			
			bHold = TRUE;
			RECEIVE_SKILLFACT_SpecialProvocation( sID, sSPEC.fVAR2 );
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			break;
        case EMSPECA_ENDURE_VARY:   // 봉주
            bHold = TRUE;
            GLCHARLOGIC::SetFactTime( sSPEC.fVAR1, sSPEC.dwFLAG );
            AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
            break;
        case EMSPECA_SKILLDELAY: //봉주
			{
				const SKILL::SSPEC& sSPEC_SkillDelay = sSPEC;
				bHold = TRUE;
				bool bSuccess = GLCHARLOGIC::SET_SKILLDELAY_INC ( sSPEC.fVAR1, sSPEC.fVAR2, sSPEC_SkillDelay.dwFLAG, sSkillID );
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC_SkillDelay, sSKILLEF );

				// 딜레이 감소에 성공했다면 클라에게 통보하자.
				if ( bSuccess )
				{
					GLMSG::SNETPC_SKILL_DELAY NetMsg;
					NetMsg.fSkillDelay = sSPEC.fVAR1;
					NetMsg.sUseSkillID = sSkillID;

					SendToClient( &NetMsg );
				}

			}
            break;
		case EMSPECA_IGNORED_DAMAGE:
			bHold = TRUE;
			GLCHARLOGIC::SET_IGNORED_DAMAGE(WORD(sSPEC.fVAR1), WORD(sSPEC.fVAR2));
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );			
			break;
        case EMSPECA_LINK_HP: //봉주
            bHold = TRUE;
            GLOGICEX::VARIATION( m_sHP.nMax, USHRT_MAX, int (static_cast<float>(m_sHP.nMax) * m_sLINKHP.fRef) );
            m_sHP.LIMIT();
            GLCHARLOGIC::SET_MAXHP_RATE ( emAddon, sSPEC, sSKILLEF, m_sLINKHP.fRef );
            break;
		case EMSPECA_DOMINATION:			
			if ( RECEIVE_SKILLFACT_Domination(sID, sSPEC, sSkillID, wlevel) )
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}			
			break;
		case EMSPECA_INVISIBLE:
			bHold = TRUE;			
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_RELEASE_PET:
			bHold = TRUE;			
			RECEIVE_SKILLFACT_ReleasePet();
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );	
			break;

		case EMSPECA_ILLUSION:
			bHold = TRUE;	
			m_idIllusionSKILL = sSkillID;
			m_sDamageSpec.m_fIllusionJumpBound	= ((float)sSPEC.nVAR1) * 10.0f;
			m_sDamageSpec.m_wIllusionRemain		= sSPEC.nVAR2;
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );	
			break;
		case EMSPECA_STIGMA:
			bHold = TRUE;	
			RECEIVE_SKILLFACT_Stigma(sID, sSPEC, sSkillID, wlevel);
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );	
			break;
		case EMSPECA_RANDOM_EXP_RATE:
			bHold = TRUE;
			RECEIVE_SKILLFACT_RandomExpRate(sSPEC, sSkillID, pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE, wlevel, sSKILLEF, pLaunchTime, pLaunchValue);
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );	
			break;
		case EMSPECA_SCREEN_THROW :
			{
				bHold = TRUE;
				RECEIVE_SKILLFACT_ScreenThrow(sID, sSPEC, sSkillID, wlevel);
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_CAMERA_HIT :
			{
				bHold = TRUE;
				RECEIVE_SKILLFACT_CameraHit(sID, sSPEC, sSkillID, wlevel);
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_ENTRANCE_EXIT :
			{
				if( RECEIVE_SKILLFACT_EntranceExit( sSkillID, sSPEC, wlevel, sID ) )
				{
					RF_ENTRANCE_EXIT( m_EffSkillVarSet ).TurnOn();

					bHold = TRUE;
					AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
				}
			}
			break;
		case EMSPECA_FEIGN_DEATH :
			{
				// 펫을 없앤다.
				RECEIVE_SKILLFACT_ReleasePet();
				RECEIVE_SKILLFACT_FeignDeath( sSkillID, sSPEC, wlevel, sID );
				
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_PROVOKE :
			{
				if( RECEIVE_SKILLFACT_Provoke( sSkillID, sSPEC, wlevel, sID ) )
				{
					bHold = TRUE;
					AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
				}				
			}
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			{
				RF_ACTIVE( m_EffSkillVarSet ).UpdateSkill( sSkillID.dwID, wlevel, sSPEC.nVAR1, sSPEC.nVAR2 );
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_ITEMDROP_RATE:
		case EMSPECA_MONEYDROP_RATE:
		case EMSPECA_EXP_RATE:
		case EMSPECA_NPC_TALK:
		case EMSPECA_ITEMDROP_SPECIFIC:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_VEHICLE_GETOFF: //// 봉주 : Server				
		case EMSPECA_RECVISIBLE:
		case EMSPECA_RELEASE_ENDURE:		
		case EMSPECA_IMMUNE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_TURN_STATE_BATTLE:
		case EMSPECA_TURN_STATE_PEACE:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_VIEW_RANGE_RATE:
		case EMSPECA_AIRBORNE:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
		case EMSPECA_DISGUISE :
			bHold = TRUE;
			AddSkillFact_SpecialAddon_Server( emAddon, sSPEC, sSKILLEF );
			break;
		};
	} //for

	return bHold;
} //GLChar::RECEIVE_SKILLFACT_Special

const BOOL GLChar::RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	const size_t nADDON = pSkill->m_sAPPLY.vecADDON.size();

	for( DWORD i = 0; i < nADDON; ++i )
	{		
		SKILL::SIMPACT_ADDON_LEVEL sAddon;
		
		sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
		sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	} //for

	return nADDON ? TRUE : FALSE;
} //GLChar::RECEIVE_SKILLFACT_Addon

const BOOL GLChar::RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	switch( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
	case SKILL::EMFOR_SUMMON_ACTIVETIME:		
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fBASIC_VAR;;
		return TRUE;
		break;
	};
	return FALSE;
} //GLChar::RECEIVE_SKILLFACT_Basic

const BOOL GLChar::RECEIVE_SKILLFACT( const SNATIVEID&			skill_id,
								const WORD					wlevel,
								DWORD&						dwSELECT,
								const STARGETID&			sID,
								const SSKILLFACT::STIME		sTime,
								const DWORD					dwCount,
								EMSKILLFACT_TYPE			emSkillFactType,
								const float* const pLaunchTime, const float* const fLaunchValue)
{
	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return FALSE;

	SSKILLFACT		sSKILLEF;
	BOOL bHold = FALSE;

	bHold |= RECEIVE_SKILLFACT_Basic( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( pSkill, skill_id, wlevel, dwCount, sSKILLEF, sID, pLaunchTime, fLaunchValue );

	if( bHold )		// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand != NULL && pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ANYBODY )
		{
			ServerActor* pActor = m_pGaeaServer->GetTarget( pLand, sID );

			if( pActor && IsReActionable(pActor, EMIMPACT_SIDE_ENEMY )  == EMREACTIONABLE_TRUE )
			{
				sSKILLEF.bSkillUserEnermy = TRUE;
			}
		}

		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.sID		= sID;
		sSKILLEF.bEnable    = TRUE;

        if ( sTime.IsEXPIREDTIME )
        {
			// 점령전 버프 표기 오류로 인한 임시 코드
			if( int(sTime.nEXPIREDTIME) < 0 )
				return FALSE;

            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
        }
        else
        {
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )
            {
                float fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
                sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
                sSKILLEF.fLIFE = sSKILLEF.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
            }
			// 점령전 버프 표기 오류로 인한 임시 코드
			//else if( sTime.fAGE < 1 || sTime.fAGE >= 10000 )
			else if( sTime.fAGE >= 10000 )
				sSKILLEF.fAGE = _SKILLFACT_INFINITY;
            else
				sSKILLEF.fAGE = sTime.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
        } //if..else

        if ( !sSKILLEF.IsAlive() )
            return FALSE;

		dwSELECT = SELECT_SKILLSLOT ( skill_id, emSkillFactType );
		SetSkillFact(dwSELECT, sSKILLEF);		
	} //if

	return TRUE;
}

const BOOL GLChar::RECEIVE_SKILLFACTII( const SNATIVEID&			skill_id,
									 const WORD					wlevel,
									 DWORD&						dwSELECT,
									 const STARGETID&			sID,
									 const SSKILLFACT::STIME		sTime,
									 const DWORD					dwCount,
									 EMSKILLFACT_TYPE			emSkillFactType,
									 const float* const pLaunchTime, const float* const fLaunchValue)
{
	// Note : 슬롯 최대 사이즈로 초기화한다.
	dwSELECT = UINT_MAX;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return FALSE;

	SSKILLFACT		sSKILLEF;
	BOOL bHold = FALSE;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();
	
	for( size_t i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
	{
		SKILL::SSPEC_ADDON* pTemp = m_SkillCAIndex.m_Apply[i];

		if( pTemp != NULL )
			tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
	}

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;

	for( size_t i = 0; i < m_SkillCAIndex.m_Pact.size(); ++i)
	{
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( m_SkillCAIndex.m_Pact[i]->emADDON ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}
	}

	bHold |= RECEIVE_SKILLFACT_Basic( &tempSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, skill_id, wlevel, dwCount, sSKILLEF, sID, pLaunchTime, fLaunchValue );

	if( bHold )		// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand != NULL && pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ANYBODY )
		{
			ServerActor* pActor = m_pGaeaServer->GetTarget( pLand, sID );

			if( pActor && IsReActionable(pActor, EMIMPACT_SIDE_ENEMY ) == EMREACTIONABLE_TRUE )
			{
				sSKILLEF.bSkillUserEnermy = TRUE;
			}
		}

		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.sID		= sID;
		sSKILLEF.bEnable    = TRUE;

		if ( sTime.IsEXPIREDTIME )
		{
			// 점령전 버프 표기 오류로 인한 임시 코드
			if( int(sTime.nEXPIREDTIME) < 0 )
				return FALSE;

			sSKILLEF.SetAgeInfinite();
			sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
		}
		else
		{
			if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )
			{
				float fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
				sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
				sSKILLEF.fLIFE = sSKILLEF.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1); //봉주
			}
			// 점령전 버프 표기 오류로 인한 임시 코드
			else if( sTime.fAGE < 1 || sTime.fAGE > 10000 )
				return FALSE;
			else
				sSKILLEF.fAGE = sTime.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
		} //if..else

		if ( !sSKILLEF.IsAlive() )
			return FALSE;

		dwSELECT = SELECT_SKILLSLOT ( skill_id, emSkillFactType );
		SetSkillFact(dwSELECT, sSKILLEF);		
	} //if

	return TRUE;
}

const BOOL GLChar::RECEIVE_SKILLCA( const SSKILLACTEX &sACTEX )
{

	STARGETID sTar( GetCrow(), GetGaeaID() );

	if( (sTar != sACTEX.sID) || (sACTEX.sID == sACTEX.sID_TAR) ) return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sACTEX.idSKILL.wMainID, sACTEX.idSKILL.wSubID );
	if ( !pSkill )
		return FALSE;

	SSKILLFACT		sSKILLEF;
	BOOL bHold = FALSE;

	SSKILLACTEX			sSKILLACTEX;


	sSKILLACTEX.sID			= sACTEX.sID_TAR;//STARGETID(CROW_PC,m_dwGaeaID);
	sSKILLACTEX.fDELAY		= pSkill->m_sEXT_DATA.fDELAY4DAMAGE;
	sSKILLACTEX.idSKILL		= sACTEX.idSKILL;
	sSKILLACTEX.wSKILL_LVL	= sACTEX.wSKILL_LVL;
	sSKILLACTEX.sID_TAR		= sACTEX.sID;
	
	bool bAllApply = false;
	
	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;
	
	ServerActor* const pTargetActor = GetTarget(sACTEX.sID_TAR);

	for( size_t i = 0; i < m_SkillCAIndex.m_Apply.size(); ++i)
	{		
		SKILL::SSPEC_ADDON* pTemp = m_SkillCAIndex.m_Apply[i];

		if( pTemp != NULL )
			tempSkill.m_sAPPLY.vecSPEC.push_back( *pTemp );
	}

	for( size_t i = 0; i < m_SkillCAIndex.m_Pact.size(); ++i)
	{
		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( m_SkillCAIndex.m_Pact[i]->emADDON ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}
	}
	
	if( m_SkillCAIndex.IsSimPacAddon() )
	{
		bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, sACTEX.wSKILL_LVL, sSKILLEF );
	}

	if( m_SkillCAIndex.IsSpecddon() )
		bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, sACTEX.idSKILL, sACTEX.wSKILL_LVL, _SKILLFACT_DEFAULTCUSTOMCOUNT, sSKILLEF, sACTEX.sID, NULL, NULL );

	if( pTargetActor ) {
		switch( pTargetActor->GetCrow() )
		{
			case CROW_PC :
				{
					GLChar* pChar = dynamic_cast<GLChar*>(pTargetActor);

					const SKILL::SSPEC_CA* pCA = pChar->GetSkillCA()->GetCaData();					

					if( pCA && pCA->bAllApply )
						bAllApply = true;
				}
				break;
			case CROW_MOB :
				{
					GLCrow*  pChar = dynamic_cast<GLCrow*>(pTargetActor);

					const SKILL::SSPEC_CA* pCA = pChar->GetSkillCA()->GetCaData();					

					if( pCA && pCA->bAllApply )
						bAllApply = true;
				}
				break;
		}
	}

	if( bHold )		// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	{
		const SSKILLFACT::STIME	sTime = SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME);

		sSKILLEF.bSkillUserEnermy = TRUE;
		sSKILLEF.sNATIVEID	= sACTEX.idSKILL;
		sSKILLEF.wLEVEL		= sACTEX.wSKILL_LVL;
		sSKILLEF.sID		= sACTEX.sID_TAR;
		sSKILLEF.bEnable    = TRUE;

		if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )
		{
			float fSkillLifeInc = CheckEndureVaryInSPEC( pSkill->m_sBASIC.emACTION_TYPE );
			sSKILLEF.fAGE = pSkill->m_sAPPLY.sDATA_LVL[sSKILLACTEX.wSKILL_LVL].fLIFE * fSkillLifeInc + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
			sSKILLEF.fLIFE = sSKILLEF.fAGE + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE-1);
		}

		if ( !sSKILLEF.IsAlive() )
			return FALSE;

		DWORD dwSELECT = SELECT_SKILLSLOT ( sACTEX.idSKILL, EMSKILLFACT_TYPE_NORMAL );
		SetSkillFact(dwSELECT, sSKILLEF);

		if( dwSELECT != UINT_MAX )
		{
			GLMSG::SNETPC_SKILLHOLD_BRD		NetMsgBRD;
			NetMsgBRD.emCrow		 = CROW_PC;
			NetMsgBRD.dwID			 = GetGaeaID();
			NetMsgBRD.skill_id		 = sACTEX.idSKILL;
			NetMsgBRD.wLEVEL		 = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT		 = static_cast<WORD>(dwSELECT);
			NetMsgBRD.fPARAM		 = GetMaxHpRate();
			//NetMsgBRD.sID            = sACTEX.sID;
			NetMsgBRD.sID            = sACTEX.sID_TAR;
			NetMsgBRD.sEffInfo.eType = GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_ATTACK;

			GetEffList( dwSELECT, NetMsgBRD.sEffInfo.iRecvEAIdx, NetMsgBRD.sEffInfo.iRecvSAIdx );
			
			m_pGaeaServer->SENDTOCLIENT ( GETCLIENTID(), &NetMsgBRD );

			SendMsgViewAround(&NetMsgBRD);

			if ( pSkill->IsBuff() )
			{
				if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_BUFF) )
				{
					RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					RestoreActStateByInvisible();
				}

			}
			else if ( pSkill->IsLimit() )
			{
				if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_LIMIT) )
				{
					RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					RestoreActStateByInvisible();
				}
			}
			else if ( pSkill->IsHarm() )
			{
				if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_DEBUFF) )
				{
					RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					RestoreActStateByInvisible();
				}
			}
		}		
	}

	if( sACTEX.dwCUREFLAG!=NULL )
		CURE_STATEBLOW ( sACTEX.dwCUREFLAG );

	for ( DWORD i = 0 ; i < SKILL::EMACTION_TYPE_NSIZE; ++i )
	{
		if ( sACTEX.dwRemoveFlag[i] == true && IsSkillEnd(sACTEX.idSKILL))
			 SKILL_EFFECT_REMOVE ( i );
	}

	if( bAllApply )
	{
		if( sACTEX.sSTATEBLOW.emBLOW!=EMBLOW_NONE && STATEBLOW ( sACTEX.sSTATEBLOW ))
		{
			ServerActor* pACTOR = GetTarget ( sACTEX.sID_TAR );
			// 상태이상 발생 Msg.
			GLMSG::SNETPC_STATEBLOW_BRD		NetMsgState;
			NetMsgState.emCrow		= sACTEX.sID.emCrow;
			NetMsgState.dwID		= sACTEX.sID.GaeaId;
			NetMsgState.emBLOW		= sACTEX.sSTATEBLOW.emBLOW;
			NetMsgState.fAGE		= sACTEX.sSTATEBLOW.fAGE;
			NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
			NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

			if( pACTOR )
				NetMsgState.wSchool = pACTOR->GetSchool();

			// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
			//
			SendMsgViewAround(&NetMsgState);

			// 스킬 대상에게 메세지 전송.
			if ( sACTEX.sID_TAR.emCrow == CROW_PC )
				m_pGaeaServer->SENDTOCLIENT( GETCLIENTID (), &NetMsgState );

			if( pTargetActor ) {
				switch( pTargetActor->GetCrow() )
				{
				case CROW_PC :
					{
						GLChar* pChar = dynamic_cast<GLChar*>(pTargetActor);

						pChar->GetSkillCA()->SetUseCA( true );
					}
					break;
				case CROW_MOB :
					{
						GLCrow*  pChar = dynamic_cast<GLCrow*>(pTargetActor);

						pChar->GetSkillCA()->SetUseCA( true );

					}
					break;
				}
			}
		} 
	}

	return TRUE;
}

const BOOL GLChar::REMOVE_SKILLFACT(const SNATIVEID& skill_id)
{
    GLCHARLOGIC::RemoveSkillFactBySkillID(skill_id);
    return true;
}

const BOOL GLChar::REMOVE_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON)
{   
    GLCHARLOGIC::RemoveSkillFactBySpecial(emSPEC_ADDON);
    return true;
}

BOOL GLChar::RESET_SKILLFACT( const SNATIVEID& skill_id, DWORD& dwOutSlot, EMSKILLFACT_TYPE emSkillFactType )
{
    DWORD dwBeginIndex = 0;
    DWORD dwEndIndex   = SKILLFACT_SIZE;

    switch ( emSkillFactType )
    {
    case EMSKILLFACT_TYPE_NONE:
        {
            dwBeginIndex = 0;
            dwEndIndex   = SKILLFACT_SIZE;
        }
        break;

    case EMSKILLFACT_TYPE_NORMAL:
        {
            dwBeginIndex = SKILLFACT_INDEX_NORMAL_START;
            dwEndIndex   = SKILLFACT_INDEX_NORMAL_END;
        }
        break;

    case EMSKILLFACT_TYPE_ADDITIONAL:
        {
            dwBeginIndex = SKILLFACT_INDEX_ADDITIONAL_START;
            dwEndIndex   = SKILLFACT_INDEX_ADDITIONAL_END;
        }
        break;

    case EMSKILLFACT_TYPE_LUNCHBOX:
        {
            dwBeginIndex = SKILLFACT_INDEX_LUNCHBOX_START;
            dwEndIndex   = SKILLFACT_INDEX_LUNCHBOX_END;
        }
        break;

    case EMSKILLFACT_TYPE_SYSTEM:
        {
            dwBeginIndex = SKILLFACT_INDEX_SYSTEM_START;
            dwEndIndex   = SKILLFACT_INDEX_SYSTEM_END;
        }
        break;
    };

    //	스킬 정보 가져옴.
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
    if ( !pSkill )	return FALSE;

    for (DWORD i=dwBeginIndex; i<dwEndIndex; ++i)
    {
        if (GetSkillFactId(i) == skill_id)
        {
            RESETSKEFF(i);
            dwOutSlot = i;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GLChar::RECEIVE_DRUGFACT ( EMITEM_DRUG emDRUG, WORD wCURE, BOOL bRATIO )
{
	//SKILL::EMTYPES emTYPE[3] = { SKILL::EMFOR_VARHP, SKILL::EMFOR_VARMP, SKILL::EMFOR_VARSP };
	//SNATIVEID nidNO[3] = { SNATIVEID(35,USHRT_MAX), SNATIVEID(35,USHRT_MAX), SNATIVEID(35,USHRT_MAX) };

	//if ( bRATIO )
	//{
	//	wCURE = m_sHP.wMax * wCURE / 100;
	//}

	//switch ( emDRUG )
	//{
	//case ITEM_DRUG_HP:
	//	nidNO[0].wSubID = 0;
	//	break;

	//case ITEM_DRUG_MP:
	//	nidNO[1].wSubID = 1;
	//	break;

	//case ITEM_DRUG_SP:
	//	nidNO[2].wSubID = 2;
	//	break;

	//case ITEM_DRUG_HP_MP:
	//	nidNO[0].wSubID = 0;
	//	nidNO[1].wSubID = 1;
	//	break;

	//case ITEM_DRUG_MP_SP:
	//	nidNO[1].wSubID = 1;
	//	nidNO[2].wSubID = 2;
	//	break;

	//case ITEM_DRUG_HP_MP_SP:
	//	nidNO[0].wSubID = 0;
	//	nidNO[1].wSubID = 1;
	//	nidNO[2].wSubID = 2;
	//	break;
	//};

	//for ( int i=0; i<3; ++i )
	//{
	//	const SNATIVEID &skill = nidNO[i];

	//	if ( skill.wSubID == USHRT_MAX )		continue;

	//	SSKILLFACT sSKILLEF;
	//	sSKILLEF.emTYPE = emTYPE[i];
	//	
	//	//	스킬 정보 가져옴.
	//	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill );
	//	if ( !pSkill )				return FALSE;
	//	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[0];

	//	float fRATE = wCURE/float(m_sHP.wMax);
	//	if ( fRATE>1.0f )	fRATE = 1.0f;

	//	float fLIFE = sSKILL_DATA.fLIFE;
	//	if ( fLIFE==0.0f )	fLIFE = 0.01f;

	//	sSKILLEF.sNATIVEID	= skill;
	//	sSKILLEF.wLEVEL		= 0;
	//	sSKILLEF.fAGE		= fLIFE;
	//	sSKILLEF.fMVAR		= fRATE/fLIFE;

	//	WORD wSELECT = SKILLREALFACT_SIZE + i;
	//	m_sSKILLFACT[wSELECT] = sSKILLEF;

	//	GLMSG::SNETPC_REQ_SKILLHOLDEX_BRD NetMsgBRD;
	//	NetMsgBRD.emCrow = m_emCrow;
	//	NetMsgBRD.dwID = m_dwGaeaID;
	//	NetMsgBRD.wSLOT = wSELECT;
	//	NetMsgBRD.sSKILLEF = sSKILLEF;

	//	// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
	//	//
	//	SNDMSGAROUND ( (NET_MSG_GENERIC*) &NetMsgBRD );

	//	// 스킬 대상에게 메세지 전송.
	//	m_pGLGaeaServer->SENDTOCLIENT ( GETCLIENTID(), &NetMsgBRD );
	//}

	return TRUE;
}

/**
 * 지속 효과가 시간 만료 및 무기 교체, 해제 등에 의해 종료된 경우 호출 되어야 함; 
 * 시간 만료의 경우 클라쪽에서도 처리 하고 있으므로 따로 메세지를 보내지 않아도 좋지만,
 * 기타 조건에 의해 만료되는 경우 메세지를 보내주어야 함;
 * 기존 란 온라인의 지속효과 로직의 경우 중간 변화에 대해 유연하지 못함;
 *
 * @ Param dwSkillfactIndex		[in] 만료된 지속효과의 인덱스;
**/
void GLChar::UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel, BOOL bForceEnd, BOOL bOverLabEnd )
{
	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[dwSkillfactIndex];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	GLMSG::SNETPC_END_SKILL_FC NetMsg(dwSkillfactIndex, bForceEnd, bOverLabEnd);
	GLMSG::SNETPC_END_SKILL_BRD NetMsgBrd(CROW_PC, m_dwGaeaID, dwSkillfactIndex, bForceEnd);

	const size_t nSize = sSKILLFACT.vecSPEC.size();
	for ( unsigned int _i = 0; _i < nSize; ++_i )
	{
		const SKILL::SSPEC_ADDON_LEVEL& spec = sSKILLFACT.vecSPEC[_i];
		switch ( spec.emSPEC )
		{
		case EMSPECA_COUNTATTACK :
			{	
				if( bTimeDel )
				{
					if ( false == m_SkillCAIndex.IsSuccess() )
						SelfSkillStart( SNATIVEID(spec.sSPEC.fVAR1,spec.sSPEC.fVAR2), sSKILLFACT.wLEVEL );
				}

				m_SkillCAIndex.AllReset();
			}
			break;
		case EMSPECA_INVISIBLE:			
			if ( m_sINVISIBLE.bPrevStateRun )
				SetActState(EM_ACT_RUN);
			else
				ReSetActState(EM_ACT_RUN);
			break;
		case EMSPECA_DOMINATION:
			TurnAction(GLAT_IDLE);
			ResetTargetID();
			break;
		case EMSPECA_IGNORED_DAMAGE:
			m_sIgnoredDamage.RESET();
			break;
		case EMSPECA_STIGMA:
			break;
        case EMSPECA_TAUNT:
            m_sTaunt.RESET();
			// [shhan][2015.03.30] 타겟과 모션은 초기화 할 필요가 있는가~?
			//						rm #1029 - 괜히 EMSPECA_TAUNT 종료시 다른유저가 보았을 때 움직임이 멈춰보일 수 있다.
            //TurnAction(GLAT_IDLE);
            //ResetTargetID();
            break;
		case EMSPECA_RANDOM_EXP_RATE:
			m_ExperienceRate.clear();
			break;
		case EMSPECA_DELAY_ACTION_SKILL:
			{
				// 정상버프 종료시에만
				if( !sSKILLFACT.IsAlive() )
					DelayActionSkillStart( sSKILLFACT.sID, SNATIVEID(spec.sSPEC.fVAR1,spec.sSPEC.fVAR2), sSKILLFACT.wLEVEL );
			}			
			break;
		case EMSPECA_SCREEN_THROW :
			RF_SCREEN_THROW( m_EffSkillVarSet ).ResetValue(spec.sSPEC.fVAR1);
			break;
		case EMSPECA_CAMERA_HIT :
			RF_CAMERA_HIT( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_FEIGN_DEATH :
			RF_FEIGN_DEATH( m_EffSkillVarSet ).End( dynamic_cast<GLCHARLOGIC*>(this), bForceEnd );
			break;
		case EMSPECA_DISGUISE :
			RF_DISGUISE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_PROVOKE :
			RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
			RF_ACTIVE( m_EffSkillVarSet ).DeleteSkill( sSKILLFACT.sNATIVEID.dwID );				
			break;
		}		
	}
	sSKILLFACT.RESET();	

	SendToClient( &NetMsg );
	SendMsgViewAround(&NetMsgBrd);
}

/*
 * 인술부가 추가 되면서 거리에 의해 해제되는 조건이 생김;
 * 기존 구조가 GLogicPC/NPC 와 GLActor 양쪽을 상속 받는 구조로 되어 있어,
 * GLogicPC/NPC 쪽에서는 위치 값을 참조 할 수가 없음;
 * 해서 아래 UPDATE_DATA_Additional() 함수를 통해 -
 * GLogicPC/NPC단에서 처리 할 수 없는 작업을 하도록 함;
 * 아래 함수의 구조를 올바르다 할 수 없음;
 * 허나 선결 조건으로 GLogicPC/NPC와 GLActor에 대한 정리가 이루어져야 함;
*/
void GLChar::UPDATE_DATA_Additional(const float fElapsedTime)
{	
	// 일단 여기서..
	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID();

		if( !sID.ISNULL() )
		{
			SFIELDCROW* pActor = GetViewCrow( sID.emCrow, sID.GaeaId );

			if( pActor ) {				
				SetTargetID( sID );
			}

			RF_PROVOKE( m_EffSkillVarSet ).ResetPreTarget();
		}
	}

	DWORD dwStigmaIndex = 0;
	for ( int _i = 0; _i < SKILLFACT_SIZE; ++_i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[_i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	
			continue;
		if ( !sSKEFF.bEnable )
			continue;		
		for ( DWORD j = 0; j < sSKEFF.vecSPEC.size(); ++j )
		{
			SKILL::SSPEC& sSPEC = sSKEFF.vecSPEC[j].sSPEC;
			switch ( sSKEFF.vecSPEC[j].emSPEC )
			{
			case EMSPECA_STIGMA:
				GLChar::UPDATE_DATA_Additional_Stigma(sSKEFF, dwStigmaIndex, _i);
				++dwStigmaIndex;
				break;
			case EMSPECA_DOMINATION:
				GLChar::UPDATE_DATA_Additional_Domination(sSPEC, sSKEFF, _i);
				break;
            case EMSPECA_TAUNT:
                GLChar::UPDATE_DATA_Additional_Taunt(sSPEC, sSKEFF, _i);
                break;
			case EMSPECA_RANDOM_EXP_RATE:
				m_ExperienceRate.updateTime(fElapsedTime);
				break;
			case EMSPECA_PROVOKE :
				GLChar::UPDATE_DATA_Additional_Provoke(sSPEC, sSKEFF, _i);
				break;
			}
		}
	}
}

void GLChar::UPDATE_DATA_Additional_Domination(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
{
	if ( m_sDOMINATE.emController == EMSPEC_DOMINATE_TYPE_CASTER )
	{
		ServerActor* pACTOR = GetTarget(sSKEFF.sID);
		if ( !pACTOR )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
		}
		else
		{
			GLLandMan* const pLand( GetLandMan() );
			if ( pLand == NULL )
			{
				UPDATE_DATA_END(dwSkillfactIndex);
				return;
			}

			if ( DWORD(sSPEC.fVAR2) &&
				!pLand->IsAbleAttack_Range_LayerSimpleCollision( m_sDOMINATE.fRadiusMoveable, pACTOR->GetPosition(), GetPosition(), 5.f ) )
			{
				UPDATE_DATA_END(dwSkillfactIndex);
				GLMSG::SNETPC_DOMINATE_MOVETO_CF_FB NetMsg;
				NetMsg.emFB = EMDOMINATE_MOVETO_FB_OUT_OF_DISTANCE;

				GLChar* pChar = m_pGaeaServer->GetChar(pACTOR->GetGaeaID());
				pChar->SendToClient( &NetMsg );
				pChar->m_sDOMINATE.bDominate = false;
			}
		}
	}	
}

void GLChar::UPDATE_DATA_Additional_Stigma(SSKILLFACT& sSKEFF, const DWORD dwStigmaIndex, const DWORD dwSkillfactIndex)
{
	const SSTIGMA::SSTIGMA_PACK* pSSTIGMA_PACK = m_sSTIGMA_TAKER.GetStigmaPack(dwStigmaIndex);
	ServerActor* pACTOR = GetTarget(pSSTIGMA_PACK->sUserID); // 시전자;

	if ( !pACTOR )
	{
		UPDATE_DATA_END(dwSkillfactIndex);
	}
	else
	{
		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
			return;
		}

		if ( pSSTIGMA_PACK->dwAsignDistance && 
			!pLand->IsAbleAttack_Range_LayerSimpleCollision( pSSTIGMA_PACK->dwAsignDistance, pACTOR->GetPosition(), GetPosition(), 5.f ) )
		{
			UPDATE_DATA_END(dwSkillfactIndex);
		}
	}
}

void GLChar::UPDATE_DATA_Additional_Taunt(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
{
    ServerActor* pACTOR = GetTarget( sSKEFF.sID );
    if ( !pACTOR )
    {
        UPDATE_DATA_END( dwSkillfactIndex );
        return;
    }

	SCONFTING* pConfting = pACTOR->GETCONFTING();
    if ( ( pACTOR->IsValidBody() == FALSE ) || ( pConfting && pConfting->IsPOWERFULTIME() ) )   //플레이어 사망,대련종료, 맵이동 등 상황에서 도발 해제;
    {
        UPDATE_DATA_END( dwSkillfactIndex );
        return;
    }

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	if ( DWORD( sSPEC.fVAR2 ) && 
		!pLand->IsAbleAttack_Range_LayerSimpleCollision( m_sTaunt.fTauntDistance, pACTOR->GetPosition(), GetPosition(), 5.f ) )
	{
        UPDATE_DATA_END( dwSkillfactIndex );
    }
}

void GLChar::UPDATE_DATA_Additional_Provoke(SKILL::SSPEC& sSPEC, SSKILLFACT& sSKEFF, const DWORD dwSkillfactIndex)
{
	ServerActor* pACTOR = GetTarget( sSKEFF.sID );
	if ( !pACTOR )
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	SCONFTING* pConfting = pACTOR->GETCONFTING();
	if ( ( pACTOR->IsValidBody() == FALSE ) || ( pConfting && pConfting->IsPOWERFULTIME() ) )   //플레이어 사망,대련종료, 맵이동 등 상황에서 도발 해제;
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}

	GLLandMan* const pLand(GetLandMan());
	if ( pLand == NULL )
	{
		UPDATE_DATA_END( dwSkillfactIndex );
		return;
	}
	
	WORD wDistance = (WORD)sSPEC.fVAR1;

	if ( wDistance == 0xFFFF )	wDistance = 0;

	wDistance = wDistance*10;

	if( !pLand->IsAbleAttack_Range_LayerSimpleCollision( wDistance, pACTOR->GetPosition(), GetPosition(), 5.f ) )
		UPDATE_DATA_END( dwSkillfactIndex );
}

void GLChar::RemoveAllPlayHostiles()
{
//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;
	GLChar*							pCHAR = NULL;
	MAPPLAYHOSTILE_ITER				pos = m_mapPlayHostile.begin();
	while( pos != m_mapPlayHostile.end() )
	{
		// 적대자 삭제 알림.		
		NetMsgDel.dwCharID = pos->first;
		SendToClient( &NetMsgDel );

		pCHAR = GetCharByDbNum( pos->first );
		if( pCHAR )
			pCHAR->RemovePlayHostile( m_CharDbNum );

		if( !pos->second )
            sc::writeLogError( std::string( "GLChar::RemovePlayHostile, pos->second = NULL" ) );

		++pos;
	} //for

	DEL_PLAYHOSTILE_ALL();
} //GLChar::RemoveAllPlayHostiles

void GLChar::RemovePlayHostile( DWORD dwCharID )
{
	if( DEL_PLAYHOSTILE( dwCharID ) )
	{
		// 적대자 삭제 알림.
		GLMSG::SNETPC_PLAYERKILLING_DEL		NetMsgDel;
		NetMsgDel.dwCharID = dwCharID;
		SendToClient(&NetMsgDel);
	} //if
} //GLChar::RemovePlayHostile

void GLChar::RemovePlayHostileEachOther( DWORD dwCharID )
{
	RemovePlayHostile( dwCharID );

	GLChar*		pCHAR = GetCharByDbNum( dwCharID );
	if( pCHAR )
		pCHAR->RemovePlayHostile( m_CharDbNum );
} //GLChar::RemovePlayHostileEachOther

//	특정 클럽의 적대자를 모두 지운다.
void GLChar::RemovePlayHostileClub( DWORD dwClubID )
{
	GLChar* pCHAR = NULL;

	GLMSG::SNETPC_PLAYERKILLING_DEL NetMsgDel;
	
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.begin();
	MAPPLAYHOSTILE_ITER pos_end = m_mapPlayHostile.end();
	MAPPLAYHOSTILE_ITER del;
	
	std::tr1::shared_ptr<GLClubField> pClub = GetMyClub();
	if (!pClub)
        return;
	
	while( pos != pos_end )
	{
		del = pos++;
		// 적대자 삭제 알림.	
		pCHAR = GetCharByDbNum(del->first );
		if (!pCHAR)
            continue;

		std::tr1::shared_ptr<GLClubField> pClub_S = GetClub(pCHAR->m_ClubDbNum);
		if (!pClub_S)
            return;

		if ( pClub->IsBattle( pCHAR->m_ClubDbNum ) || pClub->IsBattleAlliance( pClub_S->m_dwAlliance ) )
		{
			NetMsgDel.dwCharID = del->first;
			SendToClient(&NetMsgDel);
			
			pCHAR->RemovePlayHostile( m_CharDbNum );
		}

		m_mapPlayHostile.erase( del );
	} //while
} //GLChar::RemovePlayHostileClub

/*void GLChar::SummonAllRemove()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	SUMMONID_ITER SummonIter = m_dwSummonGUIDList.begin();

	for( ; SummonIter != m_dwSummonGUIDList.end(); )
	{	
		pLand->DropOutSummon ( (*SummonIter++) );
	}
}*/

/*void GLChar::SummonAllKill()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	SUMMONID_ITER SummonIter = m_dwSummonGUIDList.begin();

	for( ; SummonIter != m_dwSummonGUIDList.end(); )
	{	
		PGLSUMMONFIELD pSummon = pLand->GetSummon( (*SummonIter++) );

		if ( !pSummon || !pSummon->IsValidBody() )
		{
			continue;
		}

		pSummon->Order_Die();
	}
}*/

void GLChar::SummonGateAllRemove ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    LANDEVENTID_ITER LandEventIter = m_dwLandEventList.begin();

    for ( ; LandEventIter != m_dwLandEventList.end(); )
    {
        DWORD dwGUID = (*LandEventIter++);

        CEVENT_NODE_SERVER* pNode = pLand->GetLandEvent ( dwGUID );

        if ( !pNode || pNode->GetType() != EMEVENTNODETYPE_SUMMONGATE )
        {
            continue;
        }

        // 주변 유저에게 삭제 메세지를 보낸다.
        GLMSG::SNETLANDEVENT_REMOVE MsgFc(dwGUID);
        pNode->ProcessViewAroundMsg(&MsgFc, CROW_PC, dwGUID);
        pLand->DropOutLandEvent(dwGUID);
    }
}

BOOL GLChar::IsCTFPlayer()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	return (m_pGaeaServer->IsCTFPvPMap(pLand->GetBaseMapID().dwID) && m_pGaeaServer->IsCTFPlaying());
}

// IsCTFPlayer 함수를 사용해야함
// 버그 확인을 위해서 CTFPlayer으로 검사하는 부분을 확인을 위해서 Map 검사를 제외하고 로그 추가.
BOOL GLChar::IsCTFPlayerNotCheckMap()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		sc::writeLogError( std::string("[CTF BUFF ERROR] CTF Land is null") );
		return FALSE;
	}

	if( m_pGaeaServer->IsCTFPvPMap(pLand->GetBaseMapID().dwID) == false )
		sc::writeLogError( sc::string::format("[CTF BUFF ERROR] IsCTFPvPMap %1%, %2%", pLand->GetBaseMapID().wMainID,  pLand->GetBaseMapID().wSubID) );

	return m_pGaeaServer->IsCTFPlaying();
}


BOOL GLChar::IsCTFMap()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return FALSE;

	return (m_pGaeaServer->IsCTFPvPMap(pLand->GetBaseMapID().dwID));
}

// 감옥맵인지 확인
BOOL GLChar::IsPrisonMap()
{
	const SMAPNODE* pMapNode = m_pGaeaServer->FindMapNode( GetCurrentMap().getBaseMapID() );
	
	if( pMapNode == NULL ) return FALSE;

	if( pMapNode->IsPrisonMap() == true ) return TRUE;

	return FALSE;
}


//
// Recall 처리가 완료되기 직전, 즉, 타겟 Field로의 이동은 완료(해당 필드에 GLChar 생성 완료)되었으나, 아직 Land로의 진입(EnterLand())이 완료되지 않은 시점.
//
void GLChar::ToDoRightBeforeRecallDone( const GLLandMan* const pEnterLand, const MapID& mapIDTo )
{
	//
	// This is a recall for CTF players. (Nobody can play in CTFPvPMap not during CTF.)
	//
	if (m_pGaeaServer->IsCTFPvPMap(mapIDTo.getGaeaMapID()))	//CTF 시스템에 의한 Recall 중인가?
	{
		if (m_pGaeaServer->GetCTFState() >= CTF_STATE_READY && m_pGaeaServer->GetCTFState() < CTF_STATE_DONE && !m_pGaeaServer->IsCTFPvPMap(m_mapID.getGaeaMapID()))	//이 시점에 m_sMapID는 Recall 직전의 MapID를 가르키고 있다.
		{
			SetActState(EM_RECALLED_BY_CTF);
		}
	}
}


//
// Recall 처리가 완료되는 시점, 즉, 타겟 Field로의 이동이 완료되어서 ViewAround를 설정하는 시점에 특별히 처리 해야할 사항이 있다면, 여기에서 하도록 하자.
//
void GLChar::ToDoRightAfterRecallDone()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// Land의 팩션 정보 동기화;
	const Faction::ManagerField* const  pFactionMan(pLand->GetFactionManager());
	if ( pFactionMan )
	{
		pFactionMan->sendRelationInformation(this);
		pFactionMan->sendFactionInformation(this);

		// HP표시 정보 보냄;
		GLMSG::NET_FACTION_HP_VISIBLE_INFO netMsg(pFactionMan->getHPVisible());
		m_pGaeaServer->SENDTOCLIENT(GETCLIENTID(), &netMsg);
	}

	// 고용 소환수 관련 이동 처리;
	if ( m_sSummonable.bSummonable &&
		m_sSummonable.hireInfo.bSummoning )
	{
		m_sSummonable.hireInfo.bResummoning = true;
	}

    // 맵 이동시 이벤트 처리;
    if ( m_mapID != m_mapIDPrevious )
    {
        // 맵의 위치가 바꼈을때 퀘스트 실패 여부를 판단한다.
        DoQuestCheckLeaveMap();

        // 맵이 변경되면 뷰어라운드 세팅도 초기화 시킨다.;
        m_fViewAroundSettingReleaseTimer = 0;

        // 스크립트 이벤트 처리
        InstanceSystem::InstanceField* pInstance = pLand->GetScriptInstance();
        if ( pInstance )
        {
            pInstance->EventMapEnter(CharDbNum(), pLand->GetBaseMapID());
        }

		pLand->AddEnterPlayerCount();
    }

    // CTF 전용 예외처리
    {
        if (IsActState(EM_RECALLED_BY_CTF) && IsCTFPlayer())
        {	
            m_pGaeaServer->CTFNewPlayerHasCome(this);	


            GLMSG::SNET_CTF_START_2PLAYERS_FC msgFC;
            msgFC.dxPos1			= m_pGaeaServer->GetCertPos1();
            msgFC.dxPos2			= m_pGaeaServer->GetCertPos2();
            msgFC.dxPos3			= m_pGaeaServer->GetCertPos3();
            msgFC.wSchoolAuthedCert1= m_pGaeaServer->GetSchoolAuthedCert1();
            msgFC.wSchoolAuthedCert2= m_pGaeaServer->GetSchoolAuthedCert2();
            msgFC.wSchoolAuthedCert3= m_pGaeaServer->GetSchoolAuthedCert3();

            SendToClient(&msgFC);

            CTFResetCounters();	//Kill & Death counter 초기화

            ReSetActState(EM_RECALLED_BY_CTF);
        }
    }

	// 현재 맵의 WorldBattle 국기를 Client 에 보내준다.
	// 또한 4개 선도 지역의 해당 국기를 Client 에 보내준다.
	m_pGaeaServer->SendWorldBattleNationFlag( this );

	// Layer 가 활성화 되어 있다면 Layer Effect Rendering 을 하라고 Client 에게 보내준다.
	pLand->SendLayerActiveInfo();

	if ( GLUseFeatures::GetInstance().IsUsingCheckOverStat() )
	{
		if ( UserLevel() < USER_GM3 )
		{
			// 캐릭터가 오버스탯인지 확인한다;
			if ( CheckOverStatPoint() )
			{
				// 스킬과 스탯을 초기화한다;
				RESET_STATS_SKILL( NULL );

				GLMSG::SNETPC_FORCERESET_STAT_SKILL	sMsgFB;
				SendToClient( &sMsgFB );
			}
		}
	}

	/// 장착 아이템 유효성 검사는 캐릭터 슬롯 변경시에만 수행한다;
	// 캐릭터 장착 아이템 유효성 검사를 한다;
	//CheckEquipedItems();

	// 캐릭터 슬롯 변경 쿨타임을 초기화한다;
	m_fCoolTime_ChangeCharSlot = GLCharSlotMan::Instance()->GetCoolTime_ChangeSlot();

	// 일단 태국을 대상으로 넣는다.
	switch ( GetServiceProvider() )
	{
	case SP_OFFICE_TEST:
	case SP_THAILAND:
		CheckInvalidInventoryLine();
		break;
	default:
		break;
	}
}

const bool GLChar::CheckOverStatPoint ()
{
	if ( m_emClass == GLCC_EXTREME_M || m_emClass == GLCC_EXTREME_W )
	{
		for ( int i=0; i<GLCONST_CHAR::vecExtremeMaxStatPoint.size(); ++i )
		{
			SSTATPOINT_BYLEVEL sStatPointByLevel = GLCONST_CHAR::vecExtremeMaxStatPoint.at( i );
			if ( static_cast< WORD >( sStatPointByLevel.dwLevel ) < GetLevel() )
				continue;

			if ( sStatPointByLevel.dwStatPoint < GetSumStats() )
			{
				sc::writeLogInfo( sc::string::format(
					"[ Check ] [ Checked OverStatPoint!! ChaNum : %1%, Level : %2%, StatPoint : %3% / %4% ]",
					CharDbNum(),
					GetLevel(),
					GetSumStats(),
					sStatPointByLevel.dwStatPoint ) );

				return true;
			}
		}
	}
	else
	{
		for ( int i=0; i<GLCONST_CHAR::vecMaxStatPoint.size(); ++i )
		{
			SSTATPOINT_BYLEVEL sStatPointByLevel = GLCONST_CHAR::vecMaxStatPoint.at( i );
			if ( static_cast< WORD >( sStatPointByLevel.dwLevel ) < GetLevel() )
				continue;

			if ( sStatPointByLevel.dwStatPoint < GetSumStats() )
			{
				sc::writeLogInfo( sc::string::format(
					"[ Check ] [ Checked OverStatPoint!! ChaNum : %1%, Level : %2%, StatPoint : %3% / %4% ]",
					CharDbNum(),
					GetLevel(),
					GetSumStats(),
					sStatPointByLevel.dwStatPoint ) );

				return true;
			}
		}
	}

	return false;
}


const bool GLChar::CheckEquipedItems ( const bool _bPost )
{
	// 유효하지 않은 아이템 정보를 저장한다;
	SITEMCUSTOM arrTempToInvenItems[ SLOT_TSIZE ];
	WORD wCount = 0;

	// 장착한 아이템의 유효성 검사를 한다;
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		SITEMCUSTOM sTempEquipedItem = m_PutOnItems[ i ];
		const SNATIVEID sTempNativeID = sTempEquipedItem.GetNativeID();
		if ( false == sTempNativeID.IsValidNativeID() )
			continue;

		// 슬롯에 장착가능한 아이템인지 확인한다;
		if ( ( TRUE == ACCEPT_ITEM( sTempEquipedItem ) ) &&
			( EMSLOTCHECK_OK == CHECKSLOT_ITEM( sTempNativeID, static_cast< EMSLOT >( i ) ) ) )
			continue;

		// 아직 추가하지 않았는데 존재할 경우 그냥 넘긴다 ( 오른손 아이템을 해제하여 왼손도 이미 해제된 경우가 있다 );
		const SNATIVEID sTempReleaseItemNativeID = arrTempToInvenItems[ i ].GetNativeID();
		if ( true == sTempReleaseItemNativeID.IsValidNativeID() )
			continue;

		// 슬롯에 장착된 아이템이 유효하지 않다면 기록한다;
		arrTempToInvenItems[ i ] = GET_SLOT_ITEM( static_cast< EMSLOT >( i ) );
		wCount++;

		sc::writeLogInfo( sc::string::format(
			"[ Check ] [ Checked EquipedItem!! ChaNum : %1%, EMSLOT : %2% ]",
			CharDbNum(),
			i ) );

		// 오른손의 무기를 뺀다면 왼손도 같이 제거한다;
		if ( static_cast< EMSLOT >( i ) == GetCurRHand() )
		{
			EMSLOT emLHand = GetCurLHand();
			if ( VALID_SLOT_ITEM( emLHand ) )
			{
				SITEM* pLHandItem = GET_SLOT_ITEMDATA( emLHand );
				if ( NULL == pLHandItem )
					continue;

				switch ( pLHandItem->sBasicOp.emItemType )
				{
				case ITEM_ARROW:
				case ITEM_CHARM:
				case ITEM_BULLET: 
				case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
					{
						// LHand 에 장착한 아이템도 기록한다;
						arrTempToInvenItems[ static_cast< int >( emLHand ) ] = GET_SLOT_ITEM( emLHand );
						wCount++;

						sc::writeLogInfo( sc::string::format(
							"[ Check ] [ Checked EquipedItem!! ChaNum : %1%, EMSLOT : %2% ]",
							CharDbNum(),
							static_cast< int >( emLHand ) ) );

						BOOL bOK = m_cInventory.ValidCheckInsrt( wCount, false );
						if ( FALSE == bOK )
						{
							// RHand, LHand 가 동시에 해제될 공간이 없다면 그냥 둔다;
							arrTempToInvenItems[ i ] = SITEMCUSTOM( SNATIVEID( false ) );
							arrTempToInvenItems[ static_cast< int >( emLHand ) ] = SITEMCUSTOM( SNATIVEID( false ) );
							wCount -= 2;

							sc::writeLogInfo( sc::string::format(
								"[ Check ] [ Unchecked EquipedItem!! ChaNum : %1%, EMSLOT : %2% ]",
								CharDbNum(),
								i ) );

							sc::writeLogInfo( sc::string::format(
								"[ Check ] [ Unchecked EquipedItem!! ChaNum : %1%, EMSLOT : %2% ]",
								CharDbNum(),
								static_cast< int >( emLHand ) ) );

							continue;
						}
					}
					break;
				}
			}
		}
	}

	// 유효하지 않은 아이템들을 처리한다;
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		SITEMCUSTOM sTempEquipedItem = arrTempToInvenItems[ i ];
		const SNATIVEID sTempNativeID = sTempEquipedItem.GetNativeID();
		if ( false == sTempNativeID.IsValidNativeID() )
			continue;

		// 인벤토리에 되돌릴 공간을 찾는다;
		bool bInsertable = true;
		WORD wTempInvenPosX = 0;
		WORD wTempInvenPosY = 0;
		BOOL bOK = m_cInventory.FindInsertable( wTempInvenPosX, wTempInvenPosY );
		if ( FALSE == bOK )
		{
			// 인벤토리에 공간이 없는 경우를 체크한다;
			bInsertable = false;

			// 인벤토리에 공간이 없는데 우편으로 보내지 않을 경우 아이템을 그대로 두며 실패시킨다;
			if ( false == _bPost )
				return false;
		}

		// 이 부분에서 0 이하가 나올 수 없다, 크리티컬한 버그;
		if ( wCount <= 0 )
		{
			sc::writeLogError( sc::string::format(
				"[ Check ] [ Critical Error!! CheckEquipedItems!! ChaNum : %1% ]",
				CharDbNum() ) );

			return false;
		}

		// [모두에게] 자신의 착용 복장이 바뀜을 알린다;
		RELEASE_SLOT_ITEM_SERVER( static_cast< EMSLOT >( i ), true );

		GLMSG::SNETPC_PUTON_RELEASE sNetMsg_PutRelease( static_cast< EMSLOT >( i ) );
		SendToClient( &sNetMsg_PutRelease );

		GLMSG::SNETPC_PUTON_RELEASE_BRD sNetMsg_PutRelease_Brd( static_cast< EMSLOT >( i ) );
		sNetMsg_PutRelease_Brd.dwGaeaID = m_dwGaeaID;
		SendMsgViewAround( &sNetMsg_PutRelease_Brd );

		sc::writeLogInfo( sc::string::format(
			"[ Check ] [ Released EquipedItems!! ChaNum : %1%, EMSLOT : %2% ]",
			CharDbNum(),
			i ) );

		bool bJustInvenTypeUpdate = true;
		if ( arrTempToInvenItems[ i ].IsTempEnchant() )
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();

			CTime cTimeLimit( arrTempToInvenItems[ i ].sTLGrind.tFireTime );

			if ( cTIME_CUR > cTimeLimit )
			{
				GLMSG::SNET_INVEN_TL_GRINDING sNetMsgFB;

				sNetMsgFB.eResult   = EMREQ_TEMP_ENCHANT_FIRETIME;
				sNetMsgFB.fireItemID = arrTempToInvenItems[ i ].GetNativeID();
				SendToClient( &sNetMsgFB );

				// 강화를 복구시킨다;
				arrTempToInvenItems[ i ].TempEnchantRecovery();

				bJustInvenTypeUpdate = false;

				// 로그를 남긴다;
				LogItemExchange(
					arrTempToInvenItems[ i ],
					gamelog::ID_CHAR,
					0,
					gamelog::ID_CHAR,
					m_CharDbNum,
					ITEM_ROUTE_TEMP_ENCHANT,
					1,
					GetCurrentMap(),
					LOG_ITEM_TEMP_ENCHANT_ERASE,
					true );
			}
		}

		if ( bInsertable )
		{
			// 인벤토리에 아이템을 삽입한다;
			InvenInsertItem( arrTempToInvenItems[i], wTempInvenPosX, wTempInvenPosY, true, 1, false );

			// DB 를 업데이트한다;
			InvenUpdateItem( m_cInventory.GetItem( wTempInvenPosX, wTempInvenPosY ), bJustInvenTypeUpdate );

			sc::writeLogInfo( sc::string::format(
				"[ Check ] [ Insert a equipped Item in Inventory!! ChaNum : %1%, EMSLOT : %2% ]",
				CharDbNum(),
				i ) );
		}
		else
		{
			// 우편으로 아이템을 보낸다;
			m_pGaeaServer->PostSendSystemMail(
				CharDbNum(),
				std::string( GetName() ),
				arrTempToInvenItems[i],
				0,
				ATTENDANCE_REWARD_POST_DESIGN,
				ATTENDANCE_REWARD_POST_SENDER,
				std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
				std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_TITLE", 0 ) ),
				std::string( ID2SERVERTEXT( "TEXT_FULL_INVENTORY_POST_CONTENTS", 0 ) ) );

			// 시스템 메시지를 출력한다;
			GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
			sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
			sNetMsg.xmlString.strXmlKey = "TEXT_FULL_INVENTORY_POST_PRINT_MESSAGE";
			sNetMsg.xmlString.index = 0;
			sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, sNetMsg );
			m_pGaeaServer->SENDTOCLIENT(
				ClientSlot(),
				NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK,
				SendBuffer );

			sc::writeLogInfo( sc::string::format(
				"[ Check ] [ Post a equipped Item!! ChaNum : %1%, EMSLOT : %2% ]",
				CharDbNum(),
				i ) );
		}

		// 제거할 때 마다 숫자를 줄인다;
		wCount--;

		sc::writeLogInfo( sc::string::format(
			"[ Check ] [ Released EquipedItems!! ChaNum : %1%, EMSLOT : %2% ]",
			CharDbNum(),
			i ) );
	}

	// 아이템 변경 시 값의 재환산 필요하다;
	INIT_DATA( FALSE, FALSE );

	// 무기에 따른 버프를 초기화 한다;
	DisableSkillFact();

	// 모든 아이템이 제거되지 못한 경우에 처리한다;
	if ( wCount > 0 )
	{
		sc::writeLogInfo( sc::string::format(
			"[ Check ] [ Inventory Slot is lacked !! ChaNum : %1%, Clearance : %2%, Count : %3% ]",
			CharDbNum(),
			m_cInventory.GetAmountEmpty( false ),
			wCount ) );
	}

	return true;
}

const bool GLChar::CheckInvalidInventoryLine()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	std::vector< SINVENITEM* > vecInvenItems;
	if ( !m_cInventory.GetInvalidInventoryLineItem( vecInvenItems, m_bPREMIUM ) )
	{
		return false;
	}

	size_t InvenItemsSize = vecInvenItems.size();
	for ( size_t loop = 0; loop < InvenItemsSize; loop++ )
	{
		SINVENITEM* pInvenItem = vecInvenItems[loop];
		if ( !pInvenItem )
		{
			continue;
		}

		SITEMCUSTOM sITEM_CUSTOM = pInvenItem->sItemCustom;

		D3DXVECTOR3 vGenPos = D3DXVECTOR3( m_vPosition ) + D3DXVECTOR3( sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f );
		BOOL bCollision;
		D3DXVECTOR3 vCollision;
		bCollision = pLand->IsCollisionNavi
			(
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
			D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
			vCollision
			);

		if ( bCollision )
		{
			InvenDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, false, 191990, false );

			// 디비 처리
			SINVENITEM_SAVE sItem;
			sItem.sItemCustom = sITEM_CUSTOM;
			int DbState = sItem.sItemCustom.GetDbState();
			if ( db::DB_NONE ==  DbState || db::DB_INSERT == DbState )
			{
				m_pGaeaServer->AddGameAdoJob(
					db::DbActionPtr(
					new db::ItemInsert( m_CharDbNum, INVEN_DELETED, sItem, 797390 ) ) );

				sITEM_CUSTOM.SetDbState( db::DB_UPDATE, true );
			}
			else
			{
				m_pGaeaServer->AddGameAdoJob(
					db::DbActionPtr(
					new db::ItemUpdate( m_CharDbNum, INVEN_DELETED, sItem, 797391 ) ) );
			}

			pLand->DropItem ( vGenPos, &sITEM_CUSTOM );

			std::string strGuid = sc::string::uuidToString( sItem.sItemCustom.GetGuid() );
			sc::writeLogError( sc::string::format( "CheckInvalidInventoryLine drop item chanum: %1% guid: %2%", 
				m_CharDbNum, strGuid.c_str() ) );
		}
	}

	return true;
}

DWORD GLChar::GetCTFRebirthTime()
{
	return m_pGaeaServer->GetCTFRebirthTime(m_wSchool);
}


HRESULT GLChar::TouchQItem ( DWORD dwGlobID )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	PITEMDROP pItemDrop = pLand->GetDropItem ( dwGlobID );
	
	if ( !pItemDrop )
		return S_FALSE;

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;
	GLItemMan & glItemMan = GLItemMan::GetInstance();

	SNATIVEID sNativeID;
	
	sNativeID = pItemDrop->sItemCustom.GetNativeID();

	const SITEM* pItemData = glItemMan.GetItem(sNativeID);
	
	if ( !pItemData )
		return S_FALSE;

	if ( !pItemDrop->IsTakeItem(GetPartyID(), m_dwGaeaID) )
		return S_FALSE;

	if ( pItemData->sBasicOp.emItemType != ITEM_QITEM )
		return S_FALSE;
	

	//
	//mjeon.activity
	//
	//ActivityCheckItemUse(sNativeID.dwID);

	const ITEM::SQUESTIONITEM &sQUESTIONITEM = pItemData->sQuestionItem;
	

	// ?아이템의 영향. 파티일 경우 영향이 파티원에게 미친다.
	//
	switch ( sQUESTIONITEM.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_SPEED_UP_M:
	case QUESTION_CRAZY:
	case QUESTION_MADNESS:	
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_ATTACK_UP:
	case QUESTION_ATTACK_UP_M:
		{
			//SNATIVEID sNativeID;

			// 서비스 프로바이더가 해외인 경우
			if (m_ServiceProvider != SP_KOREA)
			{
                if ( sQUESTIONITEM.emType == QUESTION_SPEED_UP_M ||
				     sQUESTIONITEM.emType == QUESTION_MADNESS    ||
				     sQUESTIONITEM.emType == QUESTION_ATTACK_UP_M )
				{
                    sNativeID = pItemData->sBasicOp.sNativeID;
				}
				else
				{
                    sNativeID = glItemMan.GetRandomQItemID ( pItemDrop->sItemCustom.GetNativeID(), GLItemMan::QITEM_NUM_NOT_SP_KOREA );
				}
			}
			else
			{
                sNativeID = glItemMan.GetRandomQItemID ( pItemDrop->sItemCustom.GetNativeID(), GLItemMan::MAX_NUM );
			}
			
			// 새롭게 추가한 ?아이템일 경우 종전과 같은 방법으로 처리
			if ( sNativeID == NATIVEID_NULL() )
			{
				sNativeID = pItemDrop->sItemCustom.GetNativeID();
			}
			else
			{
				const SITEM* pItem = glItemMan.GetItem( sNativeID );

				if ( pItem == NULL )
					return S_FALSE;
			}

			const GLPartyID& PartyID = GetPartyID();
			if ( PartyID.isValidParty() )
			{ // 원정대 상태라 하더라도 파티만 적용 받음; 기획팀 요청;
				GLPartyManagerField* const pPartyManager = m_pGaeaServer->GetPartyManager();
				GLPartyField* const pParty = pPartyManager->GetParty(PartyID);;
				if ( pParty != NULL )
				{
					PartyMemberGaeaIDs arPartyMemberGaeaIDs;
					const DWORD nMember =  pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);					
					for ( DWORD _i = 0; _i < nMember; ++_i )
					{
						GLChar* const pCharacter = GetChar(arPartyMemberGaeaIDs[_i]);
						if ( !pCharacter )
							continue;

						if ( pCharacter->GetCurrentMap() != m_mapID )
							continue;

						const D3DXVECTOR3 vDist = pCharacter->GetPosition() - m_vPosition;

						const float fDistSq = D3DXVec3LengthSq ( &vDist );
						if ( (MAX_VIEWRANGE*MAX_VIEWRANGE) < fDistSq )
							continue;

						pCharacter->ReceiveQItemFact ( sNativeID );
					}
				}
			}		
			else
				ReceiveQItemFact ( sNativeID );
		}
		if ( GLHIDESET::CheckHideSet(EMHIDE_SPEEDUP) + 
			GLHIDESET::CheckHideSet(EMHIDE_SPEEDUP_MAX) +
			GLHIDESET::CheckHideSet(EMHIDE_CRAZY) +
			GLHIDESET::CheckHideSet(EMHIDE_MADNESS) +
			GLHIDESET::CheckHideSet(EMHIDE_EXP_TIME) +
			GLHIDESET::CheckHideSet(EMHIDE_LUCKY) +
			GLHIDESET::CheckHideSet(EMHIDE_POWERUP) +
			GLHIDESET::CheckHideSet(EMHIDE_POWERUP_MAX) )		
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}

		break;

	case QUESTION_EXP_GET:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPartyField* const pParty = GetMyParty();

			if ( pParty )
			{
				PartyMemberGaeaIDs arPartyMemberGaeaIDs;
				const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);
				
				for ( DWORD _i = 0; _i < nMember; ++_i )
				{
					GLChar* pCHAR = GetChar(arPartyMemberGaeaIDs[_i]);

					if ( !pCHAR )
						continue;

					if ( pCHAR->GetCurrentMap() != m_mapID )
						continue;

					const D3DXVECTOR3 vDistance = pCHAR->GetPosition() - GetPosition();
					const float fDistance = D3DXVec3Length ( &vDistance );

					if ( MAX_VIEWRANGE < fDistance )
						continue;

					int nRecExp = int ( pCHAR->m_sExperience.lnMax * sQUESTIONITEM.fExp/100.0f );
					pCHAR->ReceiveExp ( nRecExp, false );
					pCHAR->SendToClient(&NetMsgBrd);
				}
			}
			else
			{
				int nRecExp = int ( m_sExperience.lnMax * sQUESTIONITEM.fExp/100.0f );
				ReceiveExp ( nRecExp, false );
				SendToClient(&NetMsgBrd);
			}
		}
		if ( GLHIDESET::CheckHideSet(EMHIDE_EXP_GET) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
		break;

	case QUESTION_BOMB:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPartyField* const pParty = GetMyParty();

			if ( pParty != NULL )
			{
				PartyMemberGaeaIDs arPartyMemberGaeaIDs;
				const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

				for ( DWORD _i = 0; _i < nMember; ++_i )
				{
					GLChar* pCHAR = GetChar(arPartyMemberGaeaIDs[_i]);

					if ( !pCHAR )
						continue;

					if ( pCHAR->GetCurrentMap() != m_mapID )
						continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPosition - m_vPosition;

					float fDist = D3DXVec3Length ( &vDist );

					if ( MAX_VIEWRANGE < fDist )
						continue;

					pCHAR->ReceiveDamage ( CROW_ITEM, 0, sQUESTIONITEM.wParam1, 0 );
					pCHAR->SendToClient(&NetMsgBrd);
				}
			}
			else
			{
				ReceiveDamage ( CROW_ITEM, 0, sQUESTIONITEM.wParam1, 0 );
				SendToClient(&NetMsgBrd);
			}
		}
		if ( GLHIDESET::CheckHideSet(EMHIDE_BOMB) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
		break;

	case QUESTION_MOBGEN:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPartyField* const pParty = GetMyParty();

			if ( pParty != NULL )
			{
				PartyMemberGaeaIDs arPartyMemberGaeaIDs;
				const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

				for ( DWORD _i = 0; _i < nMember; ++_i )
				{
					GLChar* pCHAR = GetChar(arPartyMemberGaeaIDs[_i]);

					if (!pCHAR)
                        continue;

					if (pCHAR->GetCurrentMap() != m_mapID)
                        continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPosition - m_vPosition;

					float fDist = D3DXVec3Length ( &vDist );
					
					if (MAX_VIEWRANGE < fDist)
                        continue;

					pCHAR->SendToClient(&NetMsgBrd);
				}
			}
			else
			{
				SendToClient(&NetMsgBrd);
			}

			pLand->DropCrow ( SNATIVEID(sQUESTIONITEM.wParam1,sQUESTIONITEM.wParam2), pItemDrop->vPos.x, pItemDrop->vPos.z );
		}
		if ( GLHIDESET::CheckHideSet(EMHIDE_SUMMON) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
		break;

	case QUESTION_HEAL:
		{
			SQITEM_FACT sFACT;
			sFACT.emType = sQUESTIONITEM.emType;

			GLMSG::SNETPC_QITEMFACT_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.sFACT = sFACT;

			GLPartyField* const pParty = GetMyParty();

            int nVarHP(USHRT_MAX);
            int nVarMP(USHRT_MAX);
            int nVarSP(USHRT_MAX);
			if ( pParty != NULL )
			{
				PartyMemberGaeaIDs arPartyMemberGaeaIDs;
				const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);

				for ( DWORD _i = 0; _i < nMember; ++_i )
				{
					GLChar* pCHAR = GetChar(arPartyMemberGaeaIDs[_i]);
					if ( !pCHAR )
						continue;

					if ( pCHAR->GetCurrentMap() != m_mapID )
						continue;

					D3DXVECTOR3 vDist = pCHAR->m_vPosition - m_vPosition;

					float fDist = D3DXVec3Length ( &vDist );

					if (MAX_VIEWRANGE < fDist)
                        continue;

					pCHAR->SendToClient(&NetMsgBrd);
					pCHAR->VAR_BODY_POINT ( CROW_PC, m_dwGaeaID, HEAL_TYPE_NONE, nVarHP, nVarMP, nVarSP );
				}
			}
			else
			{
				VAR_BODY_POINT ( CROW_PC, m_dwGaeaID, HEAL_TYPE_NONE, nVarHP, nVarMP, nVarSP );
				SendToClient(&NetMsgBrd);
			}
		}
		if ( GLHIDESET::CheckHideSet(EMHIDE_HEAL) )
		{
			RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			RestoreActStateByInvisible();
		}
		break;
	};

	//
	//mjeon.activity
	//
	ActivityCheckItemUse(sNativeID.dwID);
	DoQuestItemUse(sNativeID);


	// 아이탬 Land에서 삭제.
	pLand->DropOutItem ( pItemDrop->dwGlobID );

	return S_OK;
}

void GLChar::SENDBOXITEMINFO(bool bFirstJoin)
{
	GLMSG::NET_GET_CHARGED_ITEM_FC NetMsgFB;

	NetMsgFB.SetType(EMREQ_CHARGEDITEM_FROMDB_FB_OK);

	// 아이템 샾에서 구입한 물품 목록 클라이언트에 전송

//	GLGaeaServer & glGaeaServer = m_pGLGaeaServer;

	MAPSHOP::reverse_iterator pos = m_mapCharged.rbegin();
	MAPSHOP::reverse_iterator end = m_mapCharged.rend();

	for (; pos!=end; ++pos)
	{
		const SHOPPURCHASE& sPURCHASE = (*pos).second;

		NetMsgFB.SetData(sPURCHASE.strPurKey, SNATIVEID(sPURCHASE.wItemMain,sPURCHASE.wItemSub));
		SendToClient( &NetMsgFB);
	}

    if (bFirstJoin)
        NetMsgFB.SetType(EMREQ_CHARGEDITEM_FROMDB_FB_END_FIRST_JOIN);
    else
	    NetMsgFB.SetType(EMREQ_CHARGEDITEM_FROMDB_FB_END);
	SendToClient( &NetMsgFB );
	return;
}

HRESULT GLChar::UpdateVehicle( float fElapsedTime )
{
	if ( !m_pVehicle->IsActiveValue() )
        return S_OK;
	if ( m_emVehicle == EMVEHICLE_OFF )
        return S_OK;
	m_fVehicleTimer += fElapsedTime;
	
	// 팻의 포만도 갱신
	if( m_emVehicle == EMVEHICLE_DRIVER )
	{
		if ( m_fVehicleTimer > 3600.0f/GLPeriod::REALTIME_TO_VBR )
		{
			m_fVehicleTimer = 0;
			m_pVehicle->m_nFull -= GLCONST_VEHICLE::nFullDecrVehicle[m_pVehicle->m_emTYPE];
			if ( m_pVehicle->m_nFull < 0 ) m_pVehicle->m_nFull = 0;

			// 클라이언트 포만도 갱신
			GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY NetMsg;
			NetMsg.nFull = m_pVehicle->m_nFull;

			SendToClient( &NetMsg );

			if ( m_pVehicle->IsNotEnoughFull() )	
			{
				for( INT i = 1; i < MAX_PASSENGER_COUNT; i++ )
				{
					if( m_pVehicle->PassengerId(i) == GAEAID_NULL)
						continue;

					GLChar* pCHAR = GetChar(m_pVehicle->PassengerId(i));
					if ( pCHAR )
					{
						pCHAR->ActiveWithVehicle( false ,false );
					}
				}
				ActiveVehicle( false, false );
			}
		}
	}
	
	return S_OK;
}

HRESULT GLChar::UpdateBooster( float fElapsedTime )
{
	if( m_pVehicle->m_bUseBoost )
	{
		m_pVehicle->m_fBoostTime += fElapsedTime;

		if( m_pVehicle->m_fBoostTime > 6.0f )
		{
			//부스트 끄는 메세지
			GLMSG::SNETPC_USE_BIKE_BOOST_CANCEL_FB MsgFB;
			MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_CANCEL;
			SendToClient( &MsgFB );

			GLMSG::SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.emFB = EMINVEN_USE_BIKE_BOOST_CANCEL;
			SendMsgViewAround(&NetMsgBrd);

			m_pVehicle->BoostReset();	
		}
	}

	if( m_pVehicle->m_bUseBoostDelay )
	{
		m_pVehicle->m_fBoostDelay -= fElapsedTime;

		if( m_pVehicle->m_fBoostDelay <= 0.0f )
		{
			//부스트 딜레이 종료 메세지
			GLMSG::SNETPC_USE_BIKE_BOOST_CANCEL_FB MsgFB;
			MsgFB.emFB = EMINVEN_USE_BIKE_BOOST_DELAY_CANCEL;
			SendToClient( &MsgFB );

			GLMSG::SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD NetMsgBrd;
			NetMsgBrd.dwGaeaID = m_dwGaeaID;
			NetMsgBrd.emFB = EMINVEN_USE_BIKE_BOOST_DELAY_CANCEL;
			SendMsgViewAround(&NetMsgBrd);

			m_pVehicle->m_fBoostDelay = 0.0f;
			m_pVehicle->m_bUseBoostDelay = FALSE;
		}
	}

	return S_OK;
}

HRESULT GLChar::UpdateDurDamage ( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	//	지속 데미지 처리( 데미지 준다. ) 
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].VAILD() ) continue;

		m_sDurDamage[i].m_fTime -= fElapsedTime;			
	
		if ( m_sDurDamage[i].m_fTime < 0.0f ) 
		{
			GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( m_sDurDamage[i].m_sSkillID );
			if ( NULL == pSkill )
				continue;

			float fPK_POINT_DEC = 1.f;
			switch ( pSkill->m_sBASIC.emAPPLY )
			{
			case SKILL::EMAPPLY_MELEE:
			case SKILL::EMAPPLY_RANGE:
				fPK_POINT_DEC = GLCONST_CHAR::fPK_POINT_DEC_PHY;
				break;

			case SKILL::EMAPPLY_MAGIC:
				fPK_POINT_DEC = GLCONST_CHAR::fPK_POINT_DEC_MAGIC;
				break;
			}

			//	데미지 준다.
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;


			//	데미지 계산

			// 전체저항값 = 상대의 저항값 / 100 * 속성저항치 적용비율
			int nRESIST = m_sSUMRESIST.GetElement( (EMELEMENT)i );
			float fResistTotal = (float) ((float) nRESIST * 0.01f * GLCONST_CHAR::fRESIST_G);
			fResistTotal = fResistTotal > GLCONST_CHAR::fREGIST_MAX_G ? GLCONST_CHAR::fREGIST_MAX_G : fResistTotal;

			// 데미지 = 데미지-(데미지*전체저항값)
			int nDamage = int(m_sDurDamage[i].m_fDamage);
			nDamage -= int(m_sDurDamage[i].m_fDamage  * fResistTotal);

            ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_sDurDamage[i].m_sID );

            if ( !pACTOR )
            {
                continue;
            }

			// 캐리겉가 캐릭터에게 지속대미지를 줄 때는 대미지를 감소시킨다;
			if ( CROW_PC == pACTOR->GetCrow() )
			{
				nDamage = static_cast< int >( nDamage * fPK_POINT_DEC );
			}

            int nActualDamageHP = -nDamage;
            int nActualDamageMP(0);
            int nActualDamageSP(0);
            //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
            VAR_BODY_POINT( m_sDurDamage[i].m_sID.emCrow, m_sDurDamage[i].m_sID.GaeaId, DAMAGE_TYPE_DOT, nActualDamageHP, nActualDamageMP, nActualDamageSP );

			GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
			NetMsgBRD.emCrow = GetCrow();
			NetMsgBRD.dwID = m_dwGaeaID;

			//	스킬을 사용한 행위자 마킹.
			NetMsgBRD.sACTOR.emCrow = m_sDurDamage[i].m_sID.emCrow;
			NetMsgBRD.sACTOR.dwID = m_sDurDamage[i].m_sID.GaeaId;

			NetMsgBRD.dwDamageFlag |= DAMAGE_TYPE_DOT;
			NetMsgBRD.nVAR_HP = nActualDamageHP;
			NetMsgBRD.nVAR_MP = nActualDamageMP;
			NetMsgBRD.nVAR_SP = nActualDamageSP;
            NetMsgBRD.wSchool = GetSchool();

			SendToClient(&NetMsgBRD);

			SendMsgViewAround(&NetMsgBRD);

			if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_DAMAGE_BY_DEBUFF) )
			{
				RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				RestoreActStateByInvisible();
			}
		}
	}  

	return S_OK;
}

BOOL GLChar::CheckMoneyUpdate(const LONGLONG lnCurrentMoney, const LONGLONG lnUpdateMoney, const bool bPlus, const char* msg)
{
	if (lnUpdateMoney < 100000000 && lnUpdateMoney >= 0)
		return FALSE;
 
    // 아무런 동작을 하지 않아 주석 2011-09-21 jgkim
    /*
    std::string TempChar(
        sc::string::format(
            "[%1%][%2%] %3%", m_szUID, m_szName, msg));

	std::string str(TempChar);
	
	if (lnUpdateMoney < 0)
	{
		str += " Not Correct Update Money Value!!";
	}

	if (bPlus)
        TempChar = sc::string::format(" Current Money %1% Update Money %2% Plus!!", lnCurrentMoney, lnUpdateMoney);
	else
        TempChar = sc::string::format(" Current Money %1% Update Money %2% Minus!!", lnCurrentMoney, lnUpdateMoney);

	str += TempChar;
	BILLIONUPDATE_WRITE(str.c_str());
    */
	return TRUE;
}

void GLChar::AddInvenMoneyAndUpdate( LONGLONG llMoney, int nCause )
{
    AddInvenMoneyServer( llMoney, nCause );
    SendInvenMoneyUpdate();
}

void GLChar::SubtractInvenMoneyAndUpdate( LONGLONG llMoney, int nCause )
{
    SubtractInvenMoneyServer( llMoney, nCause );
    SendInvenMoneyUpdate();
}

void GLChar::SendInvenMoneyUpdate()
{
    GLMSG::NET_CHAR_UPDATE_MONEY_FC NetMsgMoney(GetInvenMoney());
    SendToClient(&NetMsgMoney );
}

void GLChar::AddInvenMoneyAndPickupMessage( LONGLONG llGetMoney, int nCause )
{
    AddInvenMoneyServer( llGetMoney, nCause );

    GLMSG::SNETPC_PICKUP_MONEY NetMsgMoney;
    NetMsgMoney.lnMoney = GetInvenMoney();
    NetMsgMoney.lnPickUp = llGetMoney;
    SendToClient(&NetMsgMoney );
}

void GLChar::ClubBattleSetPoint( DWORD dwClub_P, DWORD dwClub_S, bool bAlliancBattle )
{	
	std::tr1::shared_ptr<GLClubField> pClub_P = GetClub(dwClub_P);
	std::tr1::shared_ptr<GLClubField> pClub_S = GetClub(dwClub_S);

	if (!pClub_P || !pClub_S)
        return;

	if ( bAlliancBattle ) 
	{
		std::tr1::shared_ptr<GLClubField> pClub_M = GetClub(pClub_P->m_dwAlliance);
		if (!pClub_M)
            return;

		CLUB_ALLIANCE_ITER pos = pClub_M->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pClub_M->m_setAlliance.end();
		for ( ; pos!=end; ++pos)
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;

			std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(sALLIANCE.m_dwID);
			if (!pCLUB)
                continue;
			
			GLCLUBBATTLE* pClubBattle = pCLUB->GetClubBattle( pClub_S->m_dwAlliance );
			if (!pClubBattle)
                continue;

			pClubBattle->m_wDeathPointTemp++;
			pClubBattle->m_bKillUpdate = true;
		}

		std::tr1::shared_ptr<GLClubField> pAllianceClub = GetClub(pClub_S->m_dwAlliance);
		if (!pAllianceClub)
            return;

		pos = pAllianceClub->m_setAlliance.begin();
		end = pAllianceClub->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;

			std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(sALLIANCE.m_dwID);
			if (!pCLUB)
                continue;
			
			GLCLUBBATTLE* pClubBattle = pCLUB->GetClubBattle( pClub_P->m_dwAlliance );
			if (!pClubBattle)
                continue;

			pClubBattle->m_wKillPointTemp++;
			pClubBattle->m_bKillUpdate = true;
		}
	}
	else
	{
		GLCLUBBATTLE* pClubBattle = pClub_P->GetClubBattle( dwClub_S );
		if (!pClubBattle)
            return;

		pClubBattle->m_wDeathPointTemp++;
		pClubBattle->m_bKillUpdate = true;
		
		pClubBattle = pClub_S->GetClubBattle( dwClub_P );
		if (!pClubBattle)
            return;
				
		pClubBattle->m_wKillPointTemp++;
		pClubBattle->m_bKillUpdate = true;
	}
	
	return;
}

const ITEM_COOLTIME& GLChar::GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const
{
    static ITEM_COOLTIME CoolTimeTemp;

	if (emCoolType == EMCOOL_ITEMID)
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeID.find(dwCoolID);
		if (pos == m_mapCoolTimeID.end())
            return CoolTimeTemp;
        else		
		    return pos->second;
	}
	else if ( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeType.find(dwCoolID);
		if (pos == m_mapCoolTimeType.end())
            return CoolTimeTemp;
        else		
		    return pos->second;
	}
	else	
		return CoolTimeTemp;
}

bool GLChar::IsCoolTime(const SNATIVEID& sNativeID) const
{
	__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
	__time64_t tCoolTime = GetMaxCoolTime( sNativeID );
	if (tCurTime < tCoolTime)
        return true;
    else
	    return false;
}

bool GLChar::CheckCoolTime(const SNATIVEID& sNativeID)
{
	if (IsCoolTime(sNativeID))
	{
		GLMSG::SNET_ITEM_COOLTIME_ERROR NetMsg;
		NetMsg.sNativeID = sNativeID;
		SendToClient( &NetMsg );
		return true;
	}
	return false;
}

__time64_t GLChar::GetMaxCoolTime(const SNATIVEID& sNativeID) const
{
	//	아이템 정보 가져옴.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
	if (!pItem)
        return 0;

	const ITEM_COOLTIME& pCoolTimeType = GetCoolTime( (DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE );
	const ITEM_COOLTIME& pCoolTimeID = GetCoolTime( pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID );
	return (pCoolTimeType.tCoolTime > pCoolTimeID.tCoolTime) ? pCoolTimeType.tCoolTime : pCoolTimeID.tCoolTime;
}

bool GLChar::SetCoolTime( SNATIVEID sNativeID , EMCOOL_TYPE emCoolType )
{
	//	아이템 정보 가져옴.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
	if (!pItem)
        return false;

	if (!pItem->sBasicOp.IsCoolTime())
        return false;

	// 쿨타임 셋팅
	__time64_t  tCurTime = CTime::GetCurrentTime().GetTime();
	__time64_t  tCoolTime = tCurTime + pItem->sBasicOp.dwCoolTime;

	const ITEM_COOLTIME& pCoolTime = GetCoolTime( sNativeID.dwID, emCoolType );
	if (tCoolTime < pCoolTime.tCoolTime)
        return false;

	ITEM_COOLTIME sCoolTime;
	sCoolTime.dwID = pItem->sBasicOp.sNativeID.dwID;
	sCoolTime.tUseTime = tCurTime;
	sCoolTime.tCoolTime = tCoolTime;	

	if ( emCoolType == EMCOOL_ITEMID )
	{
		sCoolTime.dwCoolID = pItem->sBasicOp.sNativeID.dwID;
		COOLTIME_MAP_ITER iter = m_mapCoolTimeID.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeID.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeID.insert(COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));;
        }
	}
	else if( emCoolType == EMCOOL_ITEMTYPE )
	{
		sCoolTime.dwCoolID = (DWORD)pItem->sBasicOp.emItemType;
        COOLTIME_MAP_ITER iter = m_mapCoolTimeType.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeType.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeType.insert(COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));;
        }
	}

	GLMSG::SNETPC_ITEM_COOLTIME_UPDATE NetMsg;			
	NetMsg.dwID = sCoolTime.dwID;
	NetMsg.dwCoolID = sCoolTime.dwCoolID;
	NetMsg.tCoolTime = sCoolTime.tCoolTime;
	NetMsg.tUseTime = sCoolTime.tUseTime;
	NetMsg.emCoolType = emCoolType;

	SendToClient( &NetMsg );

	return true;
}

bool GLChar::UpdateNpcCommission( LONGLONG lnCommission )
{
	if (lnCommission < 0)
        return false;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar ) 
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on UpdateNpcCommission", m_CharDbNum ) );
	}
	
	CheckMoneyUpdate( GetStorageMoney(), lnCommission, TRUE, "Add Npc Commission");
	AddStorageMoney( lnCommission );

	// 통계 및 로그 수정
	LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID,     lnCommission, ITEM_ROUTE_NPCCOME, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_NPC_COMMISSION);
	// 통계 및 로그 수정
    LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_USER, m_dwUserID, GetStorageMoney(), MONEY_ROUTE_USER_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_STORAGE_CURRENT);

	GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY NetMsgStorage;
	NetMsgStorage.lnMoney = GetStorageMoney();
	SendToClient( &NetMsgStorage );

	return true;
}

DWORD GLChar::GetChatLinkData(SLINK_DATA_BASIC& sLinkDataBasic, BYTE* pBuffer, size_t BufferSize)
{
	int dwSize = 0;
	if (!sLinkDataBasic.VALID())
        return dwSize;

	switch (sLinkDataBasic.sLinkType.wMType)
	{
	case ITEM_LINK:
        dwSize = GetChatLinkDataItemLink(sLinkDataBasic, pBuffer, BufferSize);
		break;
	case PARTY_LINK:
        dwSize = GetChatLinkDataParty(sLinkDataBasic, pBuffer, BufferSize);
		break;
	case QUEST_LINK:
		break;
	case SKILL_LINK:
		break;
	case CROW_LINK:
		break;
	case MAP_LINK:
		break;
	default:
		break;
	};
	
	return dwSize;
}

int GLChar::GetChatLinkDataParty(SLINK_DATA_BASIC& sLinkDataBasic, BYTE* pBuffer, size_t BufferSize) 
{
    int dwSize = 0;
    switch ( sLinkDataBasic.sLinkType.wSType ) 
    {
    case PARTY_LINK_RECRUIT:
        {
            GLPartyField* const pMyParty = GetMyParty();
			if ( pMyParty == NULL )
			{
				// 맵정보를 넣는다.
				if ((dwSize + sizeof(SNATIVEID)) >= BufferSize)
					break;
				memcpy_s(pBuffer, BufferSize-dwSize, &m_mapID, sizeof(SNATIVEID));
				dwSize = sizeof(SNATIVEID);

				GLPARTY_LINK_MEM PartyMem;

				PartyMem.dwGaeaID = m_dwGaeaID;
				PartyMem.emClass  = m_emClass;
				PartyMem.wLevel = m_wLevel;
				PartyMem.SetName(m_szName);

				if ((dwSize+sizeof(GLPARTY_LINK_MEM)) >= BufferSize)
					break;

				memcpy_s(pBuffer+dwSize, BufferSize-dwSize, &PartyMem, sizeof(GLPARTY_LINK_MEM));
				dwSize += sizeof(GLPARTY_LINK_MEM);
				break;
			}

            if ( pMyParty->isMasterByGaeaID(m_dwGaeaID) == false )
                break;

			// 맵정보를 넣는다.
			if ((dwSize + sizeof(SNATIVEID)) >= BufferSize)
				break;

			memcpy_s(pBuffer, BufferSize-dwSize, &m_mapID, sizeof(SNATIVEID));
			dwSize = sizeof( SNATIVEID );

			GLPARTY_LINK_MEM PartyMem;
			PartyMem.dwGaeaID = m_dwGaeaID;
			PartyMem.emClass  = m_emClass;
			PartyMem.wLevel = m_wLevel;
			PartyMem.SetName(m_szName);

			if ((dwSize+sizeof(GLPARTY_LINK_MEM)) >= BufferSize)
				break;

			memcpy_s(pBuffer+dwSize, BufferSize-dwSize, &PartyMem, sizeof(GLPARTY_LINK_MEM));
			dwSize += sizeof(GLPARTY_LINK_MEM);

			GLPartyField::MEMBER_ITER pos = pMyParty->m_cMEMBER.begin();
			GLPartyField::MEMBER_ITER end = pMyParty->m_cMEMBER.end();
			for ( ; pos!=end; ++pos)
			{
				const GLPARTY_FNET &sPARTY_MEM = (*pos).second;
				if (m_dwGaeaID == sPARTY_MEM.m_dwGaeaID)
					continue;

				PartyMem.dwGaeaID = sPARTY_MEM.m_dwGaeaID;
				PartyMem.emClass  = sPARTY_MEM.m_emClass;
				PartyMem.wLevel = sPARTY_MEM.m_wLevel;
				PartyMem.SetName(sPARTY_MEM.m_szName);

				if ((dwSize+sizeof(GLPARTY_LINK_MEM)) >= BufferSize)
					continue;

				memcpy_s(pBuffer+dwSize, BufferSize-dwSize, &PartyMem, sizeof(GLPARTY_LINK_MEM));
				dwSize += sizeof( GLPARTY_LINK_MEM );
			}
        }
        break;
    case PARTY_LINK_ACCEPT:	// 해당 사항이 없음
        break;
    case PARTY_LINK_REFUSE:	// 해당 사항이 없음
        break;
    }
    return dwSize;
}

int GLChar::GetChatLinkDataItemLink(SLINK_DATA_BASIC &sLinkDataBasic, BYTE* pBuffer, size_t BufferSize)
{
    int dwSize = 0;
    switch (sLinkDataBasic.sLinkType.wSType) 
    {
    case ITEM_LINK_INVEN:
        {
            SINVEN_POS sInvenPos;
            sInvenPos.dwPos = sLinkDataBasic.dwData1;
            SINVENITEM* pInvenItem = m_cInventory.GetItem(sInvenPos.wPosX, sInvenPos.wPosY);
            if (!pInvenItem)
                break;
            if (sizeof(SITEMCUSTOM) >= BufferSize)
                break;
            memcpy_s(pBuffer, BufferSize, &pInvenItem->sItemCustom, sizeof(SITEMCUSTOM));
            dwSize = sizeof(SITEMCUSTOM);
        }
        break;

    case ITEM_LINK_PUTON:
        {
            EMSLOT emSlot = (EMSLOT)sLinkDataBasic.dwData1;
            if (emSlot >= SLOT_TSIZE)
                break;
            if (!VALID_SLOT_ITEM(emSlot))
                break;
            if (sizeof(SITEMCUSTOM) >= BufferSize)
                break;
            const SITEMCUSTOM& sItemCustom = m_PutOnItems[emSlot];
            memcpy_s(pBuffer, BufferSize, &sItemCustom, sizeof(SITEMCUSTOM));
            dwSize = sizeof(SITEMCUSTOM);
        }
        break;

    case ITEM_LINK_USER_STORAGE:
        {
            DWORD dwChannel = sLinkDataBasic.dwData1;
            if (dwChannel >= MAX_CLUBSTORAGE)
                break;

            SINVEN_POS sInvenPos;
            sInvenPos.dwPos = sLinkDataBasic.dwData2;

            SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(sInvenPos.wPosX, sInvenPos.wPosY);
            if (!pInvenItem)
                break;
            if (sizeof(SITEMCUSTOM) >= BufferSize)
                break;
            memcpy_s(pBuffer, BufferSize, &pInvenItem->sItemCustom, sizeof(SITEMCUSTOM));
            dwSize = sizeof(SITEMCUSTOM);
        }
        break;

    case ITEM_LINK_CLUB_STORAGE:
        {
            // club storage 링크는 agent 로 data 가 옮겨짐에 따라 어떻게 구현할지 고민해 보아야 한다.
            /*
            if (m_ClubDbNum == CLUB_NULL)
                break;

            std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
            if (!pCLUB)
                break;

            if (pCLUB->m_MasterCharDbNum != m_CharDbNum)
                break;

            DWORD dwChannel = sLinkDataBasic.dwData1;
            if (dwChannel >= MAX_CLUBSTORAGE)
                break;

            SINVEN_POS sInvenPos;
            sInvenPos.dwPos = sLinkDataBasic.dwData2;

            SINVENITEM* pInvenItem = pCLUB->m_cStorage[dwChannel].FindPosItem(sInvenPos.wPosX, sInvenPos.wPosY);
            if (!pInvenItem)
                break;

            memcpy(pBuffer, &pInvenItem->sItemCustom, sizeof(SITEMCUSTOM));
            dwSize = sizeof(SITEMCUSTOM);
            */
        }
        break;

    case ITEM_LINK_CROW_SALE:
        {
            DWORD dwNpcGlobID = (sLinkDataBasic.dwData1 & 0xFFFF0000) >> 16;
            DWORD dwChannel = (sLinkDataBasic.dwData1 & 0x0000FFFF);

			GLLandMan* const pLand( GetLandMan() );
			if ( pLand == NULL )
				break;

            PGLCROW pCrow = pLand->GetCrow(dwNpcGlobID);
            if ( !pCrow )
                break;
            if (pCrow->m_pCrowData->GetSaleNum() <= dwChannel)
                break;

            const GLInventory* pInven = pCrow->m_pCrowData->GetSaleInven(dwChannel);
            if (!pInven)
                break;

            SINVEN_POS sInvenPos;
            sInvenPos.dwPos = sLinkDataBasic.dwData2;

            SINVENITEM* pSaleItem = pInven->GetItem(sInvenPos.wPosX, sInvenPos.wPosY);
            if (!pSaleItem)
                break;

            if (sizeof(SITEMCUSTOM) >= BufferSize)
                break;
            memcpy_s(pBuffer, BufferSize, &pSaleItem->sItemCustom, sizeof(SITEMCUSTOM));
            dwSize = sizeof(SITEMCUSTOM);
        }
        break;
    default:
        break;
    };
    return dwSize;
}

//	펫 자동 물약 스킬 시전시 사용
HRESULT GLChar::AutoPotionDrug()
{
	if ( !IsValidBody() )
		return false;

	if ( m_sCONFTING.IsFIGHTING() )
		return false;

	//	캐릭터 죽음을 Framemove에서 처리하므로 캐릭터 호출 순서에 따라 아직 죽음이 처리되지 않았을수도 있다.
	//	그럴경우를 대비해서 현재 HP를 체크해준다.
	if ( m_sHP.nNow <= 0 )
        return false;
	
	//	살인마 등급이면 회복약 사용 못한다.
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
	{
		return false;
	}

	DWORD dwMaxOwnersHP = GETMAXHP ();
	DWORD dwMaxOwnersMP = GETMAXMP ();
	DWORD dwMaxOwnersSP = GETMAXSP ();
	DWORD dwHPVar = dwMaxOwnersHP - GETHP ();
	int   nMPVar  = dwMaxOwnersMP - GETMP ();
	int   nSPVar  = dwMaxOwnersSP - GETSP ();

	if ( dwHPVar == 0 && nMPVar == 0 && nSPVar == 0 && !ISSTATEBLOW() ) 
	{
		return true;
	}

	
	// 내부에 메세지 형태로 보낸다.
	GLMSG::SNETPC_REQ_INVENDRUG NetMsg;
	NetMsg.bPetSkill = true;


	// Q, W, E, A, S, D
	for ( WORD wSLOT = 0; wSLOT < EMACTIONQUICK_SIZE; ++wSLOT )
	{
		const SACTION_SLOT &sACTION = m_sACTIONQUICK[wSLOT];
		if ( !sACTION.VALID() )	continue;
		if ( sACTION.wACT != EMACT_SLOT_DRUG )
			continue;

		SINVENITEM* pInvenItem = m_cInventory.FindItem ( sACTION.sNID );
		if ( !pInvenItem )
			continue;

		const SITEM* pITEM = GLogicData::GetInstance().GetItem( sACTION.sNID );
		if ( !pITEM )
			continue;

		if ( pITEM->sBasicOp.emItemType != ITEM_CURE && pITEM->sBasicOp.emItemType != ITEM_EFFECT_RELEASE_CURE  )
			continue;

		if ( IsCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
			continue;

		if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
            continue;

		NetMsg.wPosX = pInvenItem->wPosX;
		NetMsg.wPosY = pInvenItem->wPosY;

		// HP 관련이면
		switch ( pITEM->sDrugOp.emDrug )
		{
		case ITEM_DRUG_HP:
		case ITEM_DRUG_HP_MP:
		case ITEM_DRUG_HP_MP_SP:
		case ITEM_DRUG_HP_CURE:
		case ITEM_DRUG_HP_MP_SP_CURE:
			{
				if ( pITEM->sDrugOp.bRatio )
				{
					if ( pITEM->sDrugOp.dwCureVolume/100.0f*dwMaxOwnersHP <= (float)dwHPVar )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}
				else
				{
					if ( pITEM->sDrugOp.dwCureVolume <= dwHPVar )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}

				// HP가 20% 밑으로 떨어지면 무조건 먹는다.
				if ( (float)(dwMaxOwnersHP-dwHPVar)/dwMaxOwnersHP*100.0f < GLCONST_PET::fMaginotLineHP )
				{
					MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
					return true;
				}
			}
		};

		// MP 관련이면
		switch ( pITEM->sDrugOp.emDrug )
		{
		case ITEM_DRUG_MP:
		case ITEM_DRUG_HP_MP:
		case ITEM_DRUG_MP_SP:
		case ITEM_DRUG_HP_MP_SP:
		case ITEM_DRUG_HP_MP_SP_CURE:
			{
				if ( pITEM->sDrugOp.bRatio )
				{
					if ( pITEM->sDrugOp.dwCureVolume/100.0f*dwMaxOwnersMP <= float(nMPVar) )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}
				else
				{
					if ( pITEM->sDrugOp.dwCureVolume <= nMPVar )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}

				// MP가 20% 밑으로 떨어지면
				if ( (float)(dwMaxOwnersMP-nMPVar)/dwMaxOwnersMP*100.0f < GLCONST_PET::fMaginotLineMP )
				{
					MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
					return true;
				}
			}
		};

		

		// SP 관련이면
		switch ( pITEM->sDrugOp.emDrug )
		{
		case ITEM_DRUG_SP:
		case ITEM_DRUG_MP_SP:
		case ITEM_DRUG_HP_MP_SP:
		case ITEM_DRUG_HP_MP_SP_CURE:
			{
				if ( pITEM->sDrugOp.bRatio )
				{
					if ( pITEM->sDrugOp.dwCureVolume/100.0f*dwMaxOwnersSP <= (float)nSPVar )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}
				else
				{
					if ( pITEM->sDrugOp.dwCureVolume <= nSPVar )
					{
						MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
						return true;
					}
				}

				// SP가 20% 밑으로 떨어지면
				if ( (float)(dwMaxOwnersSP-nSPVar)/dwMaxOwnersSP*100.0f < GLCONST_PET::fMaginotLineSP )
				{
					MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
					return true;
				}
			}
		};

	
		
		// 이상치료
		if ( pITEM->sDrugOp.emDrug == ITEM_DRUG_CURE )
		{
			if ( ISSTATEBLOW() )
			{
				MsgReqInvenDrug(NetMsg.wPosX, NetMsg.wPosY, NetMsg.bPetSkill);
				return true;
			}
		}		
	} // end of for

	return true;
}

EMANI_SUBTYPE GLChar::GetAniSubType()
{
	EMSLOT 				emRHand			= GetCurRHand();
	EMSLOT 				emLHand			= GetCurLHand();
	return CHECK_ATTACK_ANISUB( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );
} //GLChar::GetAniSubType

const bool GLChar::CheckHit(const STARGETID& cTargetID, const BOOL bLowSP, const DWORD dwApplyFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL || ( pLand && pLand->IsPeaceZone()))
		return false;

	ServerActor* const pTargetActor = GetTarget(cTargetID);
	if ( pTargetActor == NULL )
		return false;

	if ( GLUseFeatures::GetInstance().IsUsingAttackSuccessRate_Renew() == TRUE 
		&& pTargetActor->GetCrow() == CROW_PC )
	{
		const int nHitRate( GLOGICEX::GLHITRATE_RENEW(GETHIT(), pTargetActor->GetAvoid(), GetLevel(), pTargetActor->GetLevel()) );
		const bool bSuccess(nHitRate >= (sc::Random::RANDOM_POS()*100));
		return bSuccess;
	}
	else
	{
		const EMBRIGHT emBright = pTargetActor->GetBright();
		const EM_BRIGHT_FB bFB = GLOGICEX::GLSPACEGAP(GETBRIGHT(), emBright, pLand->GETBRIGHT());
		const int nAVOID = pTargetActor->GetAvoid();

		const float fBaseHitRate = float(GLOGICEX::GLHITRATE(GETHIT(), nAVOID, bFB));
		const float fMulHitRate = bLowSP ? (1.0f - GLCONST_CHAR::fLOWSP_HIT_DROP) : 1.0f;
		const int nHitRate = int(fBaseHitRate * fMulHitRate);

		return (nHitRate >= (sc::Random::RANDOM_POS()*100));
	}
}

float GLChar::GET_PK_ITEMDROP_RATE()
{
    DWORD dwLEVEL = GET_PK_LEVEL();
    float fRATE = 0;

    if (dwLEVEL == UINT_MAX)
        fRATE = GLCONST_CHAR::fPK_ITEM_DROP;
    else
        fRATE = GLCONST_CHAR::sPK_STATE[dwLEVEL].fITEM_DROP_RATE;

    if (SchoolFreePkOn())
        fRATE += GLCONST_CHAR::fSCHOOL_FREE_PK_ITEM_DROP;

    return fRATE;
}

void GLChar::EndConfting()
{
    //
    //mjeon.attendance
    //
    if (m_sCONFTING.emTYPE == EMCONFT_ONE)
    {	
        AttendanceCheckSparingSolo();
    }
    else if (m_sCONFTING.emTYPE == EMCONFT_PARTY)
    {
        AttendanceCheckSparingParty();
    }

	//대련 퀘스트
	DoQuestConfting(m_sCONFTING.emTYPE, m_sCONFTING.sOption.bSCHOOL);

	// 체력 환원.
	//
	DoConftStateRestore();

    // 디버프 스킬 삭제
    DisableDebuffSkillFact();

    //  대련 정보 리셋
    m_sCONFTING.RESET ();

    // 소환게이트 전부 삭제
    // 플레이어가 필드서버 이동으로 대련종료시에는 LandMan이 변경된 상태로 들어오게 되어 
    // 이전 LandMan에 있는 EventNode의 삭제에 실패한다.
    // 하지만 어차피 소환게이트와 주인의 MapID가 다르면 소환이 안되므로 문제는 없지만
    // 필요시 나중에 수정해준다.
    SummonGateAllRemove();
}

BOOL GLChar::CHECK_ACTIVITY_POINT ( const SITEM& sItem )
{
    //  활동 요구 포인트 체크
    if ( sItem.sBasicOp.wReqActPointDW || sItem.sBasicOp.wReqActPointUP )
    {
        if ( !m_pActivityMan ) return FALSE;

        ENUM_ACTIVITY_CLASS emType = static_cast<ENUM_ACTIVITY_CLASS>(sItem.sBasicOp.emReqActivityType);
        if ( m_pActivityMan->GetRewardPoint(emType) < sItem.sBasicOp.wReqActPointDW ) return FALSE;
        if ( sItem.sBasicOp.wReqActPointUP && m_pActivityMan->GetRewardPoint(emType) >  sItem.sBasicOp.wReqActPointUP ) return FALSE;
    }

    return TRUE;
}

BOOL GLChar::CHECK_CONTRIBUTION_POINT ( const SITEM& sItem )
{
	//  활동 기여도 체크
	if ( sItem.sBasicOp.dwReqContribution )
	{
		if ( (DWORD)m_nContributionPoint < sItem.sBasicOp.dwReqContribution )
		{
			return FALSE;
		}
	}

	return TRUE;
}

WORD GLChar::GetMaxLevel()
{
	if( m_emClass & GLCC_EXTREME_M || m_emClass & GLCC_EXTREME_W )
	{
		return GLCONST_CHAR::wMAX_EXTREME_LEVEL;
	}
	else
	{
		return GLCONST_CHAR::wMAX_LEVEL;
	}
}

HRESULT GLChar::CheckItemDuration( SINVENITEM& sInvenItemOriginal )
{
	bool bDelete = false;

	// 실제 원본이 레퍼런스로 넘어오기 때문에 복사해서 사용한다.
	// sInvenItemOriginal 을 사용할 때도 있다.
	SINVENITEM sInvenItem = sInvenItemOriginal;

    //	유효기간 초과 점검.
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( sInvenItem.sItemCustom.GetNativeID() );
	if ( !pITEM )
	{
		return E_FAIL;
	}

	if ( sInvenItem.sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
    {
        if ( pITEM->sDrugOp.tTIME_LMT != 0 )
        {
            const CTime cTIME_CUR = CTime::GetCurrentTime();

            CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
            CTime cTIME_LMT( sInvenItem.sItemCustom.tBORNTIME );
            cTIME_LMT += cSPAN + sInvenItem.sItemCustom.GetPeriodExBasic();

            WORD wInsertPosX = sInvenItem.wPosX, wInsertPosY = sInvenItem.wPosY;

            if ( cTIME_CUR > cTIME_LMT )
            {
                // 탈것일 경우
                if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sInvenItem.sItemCustom.m_VehicleDbNum != 0 )
                {
                    GLVEHICLE* pNewVehicle = new GLVEHICLE;
                    AddGameAdoJob(
                        db::DbActionPtr(
                        new CGetVehicle(
                        pNewVehicle, 
                        sInvenItem.sItemCustom.m_VehicleDbNum, 
                        m_ClientSlot, 
                        m_CharDbNum, 
                        true ) ) );					
                }

                // 팻카드일 경우
                if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.PetDbNum() != 0 )
                {
                    AddGameAdoJob(
                        db::DbActionPtr(
                            new db::PetDelete( m_CharDbNum, sInvenItem.sItemCustom.PetDbNum() ) ) );					

                    // 활동중이면 사라지게 해준다.
                    PGLPETFIELD pMyPet = GetMyPet();
                    if ( pMyPet && sInvenItem.sItemCustom.PetDbNum() == pMyPet->DbNum() )
                    {
                        // 팻의 악세사리를 검사한다.
                        pMyPet->UpdateTimeLmtItem( this );

                        // 넣어줄꺼 넣어준다.
                        for ( WORD i = 0; i < ACCETYPESIZE; ++i )
                        {
							if ( pMyPet->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
							{
								continue;
							}

                            CItemDrop cDropItem;
                            cDropItem.sItemCustom = pMyPet->m_PutOnItems[i];
                            if ( IsInsertToInven( &cDropItem ) )
							{
								InsertToInven( &cDropItem, false, false, false );
							}
                            else
                            {
								GLLandMan* const pLand( GetLandMan() );
								if ( pLand )
								{
									// 바닥에 떨어뜨린다.
									// 아이템 상태가 초기상태이거나 디비삽입해야하는 상태라면 삭제된 아이템타입으로 디비에 우선 삽입한다.
									// 그외 디비업데이트 상태라면 디비 인벤타입만 변경한다.
									// 나중에 랜드에서 사라지게 된다면(아무도 줍지 않고) 완전 삭제 타입으로 업데이트 되고(랜드에 작업 필요)
									// 누군가 줍는다면 자기 아이템으로 업데이트된다.
									DropItem( cDropItem.sItemCustom, m_vPosition );
								}
                            }
                        }

                        m_pGaeaServer->ReserveDropOutPet( SDROPOUTPETINFO( pMyPet->m_dwGUID, true, false ) );

                        // 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
                        AddGameAdoJob(
                            db::DbActionPtr(
                                new db::PetListRestoreGet( m_CharDbNum, m_ClientSlot ) ) );						
                    }
                    else
                    {
                        // 악세사리를 체크후 팻을 DB에서 삭제해준다.
                        GLPetField* pNewPet = new GLPetField( m_pGaeaServer );
                        AddGameAdoJob(
                            db::DbActionPtr(
                                new db::PetGet(
                                    pNewPet, 
                                    sInvenItem.sItemCustom.PetDbNum(), 
                                    m_ClientSlot, 
                                    m_CharDbNum,
                                    sInvenItem.wPosX,
                                    sInvenItem.wPosY,
                                    true ) ) );
                    }
                }

				bDelete = true;

                //	시간 제한으로 아이템 삭제 알림.
                GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
                NetMsgInvenDelTimeLmt.nidITEM = sInvenItem.sItemCustom.GetNativeID();
                SendToClient( &NetMsgInvenDelTimeLmt );

                // 아이템 삭제.
                //
                InvenDeleteItem( sInvenItem.wPosX, sInvenItem.wPosY, true, FALSE, DELETE_ITEM_CHECKITEMDURATION, true  );

                // 코스툼 복구.
                if ( sInvenItem.sItemCustom.nidDISGUISE != SNATIVEID( false ) )
                {
                    const SITEM* pONE = GLogicData::GetInstance().GetItem( sInvenItem.sItemCustom.nidDISGUISE );
                    if ( !pONE )
                        return E_FAIL;

                    SITEMCUSTOM sITEM_NEW( sInvenItem.sItemCustom.nidDISGUISE );
                    CTime cTIME = CTime::GetCurrentTime();
                    if ( sInvenItem.sItemCustom.tDISGUISE != 0 && pONE->sDrugOp.tTIME_LMT != 0 )
                    {
                        cTIME = CTime( sInvenItem.sItemCustom.tDISGUISE );

                        CTimeSpan tLMT( pONE->sDrugOp.tTIME_LMT );
                        cTIME -= tLMT;
                    }

					// 연장 기능 전이;
					sITEM_NEW.cPeriodExtensionCount = sInvenItem.sItemCustom.cCostumePeriodExtensionCount;
					sITEM_NEW.SetPeriodExBasic( sInvenItem.sItemCustom.GetPeriodExDisguise() );
					sITEM_NEW.ResetPeriodExDisguise();

                    // 코스튬 염색값 옮김. by luxes.
                    //
                    sITEM_NEW.dwMainColor = sInvenItem.sItemCustom.dwMainColor;
                    sITEM_NEW.dwSubColor = sInvenItem.sItemCustom.dwSubColor;

					// 코스튬 능력치 옮김
					sITEM_NEW.costumeUser = sInvenItem.sItemCustom.costumeUser;

                    sInvenItem.sItemCustom.dwMainColor = 0;	// ?
                    sInvenItem.sItemCustom.dwSubColor = 0;	// ?

                    // 아이템 생성
                    sITEM_NEW.tBORNTIME = cTIME.GetTime();
                    sITEM_NEW.wTurnNum = 1;
                    sITEM_NEW.cGenType = EMGEN_BILLING;
                    sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
					sITEM_NEW.GenerateBasicStat( false );
					sITEM_NEW.GenerateLinkSkill();
					sITEM_NEW.GenerateAddOption();

                    // 인벤에 들어갈 공간 검사.
                    BOOL bOk = m_cInventory.IsInsertable( wInsertPosX, wInsertPosY );
                    if ( !bOk )
					{
						// 위에서 아이템을 삭제하고 그 자리에 넣는건데 자리가 없으면 안될듯
                        return E_FAIL;
					}

                    // 인벤에 넣음.
                    InvenInsertItem( sITEM_NEW, wInsertPosX, wInsertPosY, true, INSERT_ITEM_CHECKITEMDURATION, true );
					SINVENITEM* pINSERT_ITEM = m_cInventory.GetItem( wInsertPosX, wInsertPosY );
					if ( pINSERT_ITEM )
					{
						GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
						if ( !pTarChar )
						{		
							// 통계 및 로그 수정
							// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
							sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on CheckItemDuration", m_CharDbNum ) );
						}

						// 분리된 코스툼 로그.
						// 통계 및 로그 수정
						LogItemExchange( 
							pINSERT_ITEM->sItemCustom, 
							gamelog::ID_CHAR, 0, 
							gamelog::ID_CHAR, m_CharDbNum, 
							ITEM_ROUTE_CHAR, pINSERT_ITEM->sItemCustom.wTurnNum, 
							( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_RESTORE_CUSTOME_BY_TIMEOUT,
							false );
					}
                }
            }
        }
    }

	// 코스튬 능력치가 적용 되어 있는 아이템 만료체크
	if ( !bDelete && ( sInvenItemOriginal.sItemCustom.nidDISGUISE != NATIVEID_NULL() || sInvenItemOriginal.sItemCustom.IsDisguise() ) )
	{
		if ( sInvenItemOriginal.sItemCustom.IsCSApplied())
		{
			const CTime cTIME_CUR = CTime::GetCurrentTime();
			CTime cTimeLimit( sInvenItemOriginal.sItemCustom.costumeUser.getEndTime() );
			
			if ( cTIME_CUR > cTimeLimit )
			{
				// 능력치 초기화 알림
				GLMSG::SNET_INVEN_COSTUME_STATS_FB netMsgFb;

				netMsgFb.emFB    = ENREQ_CS_FB_NOTICE1;
				netMsgFb.sItemID = sInvenItemOriginal.sItemCustom.GetNativeID();
				SendToClient( &netMsgFb );

				// 코스튬에 능력치 초기화
				sInvenItemOriginal.sItemCustom.ResetInvest();

				GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
				NetItemUpdate.wPosX       = sInvenItemOriginal.wPosX;
				NetItemUpdate.wPosY       = sInvenItemOriginal.wPosY;
				NetItemUpdate.sItemCustom = sInvenItemOriginal.sItemCustom;
				SendToClient( &NetItemUpdate );

				// 로그
				LogItemExchange( sInvenItemOriginal.sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_COSTUME_STATE_ERASE, 1, GetCurrentMap(), LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT, true );
				
			}
		}
	}

	if ( !bDelete && sInvenItemOriginal.sItemCustom.IsTempEnchant())
	{
		const CTime cTIME_CUR = CTime::GetCurrentTime();

		CTime cTimeLimit( sInvenItem.sItemCustom.sTLGrind.tFireTime );

		if( cTIME_CUR > cTimeLimit )
		{
			GLMSG::SNET_INVEN_TL_GRINDING netMsgFb;

			netMsgFb.eResult   = EMREQ_TEMP_ENCHANT_FIRETIME;
			netMsgFb.fireItemID = sInvenItemOriginal.sItemCustom.GetNativeID();
			SendToClient( &netMsgFb );

			// 강화복구
			sInvenItemOriginal.sItemCustom.TempEnchantRecovery();

			GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
			NetItemUpdate.wPosX       = sInvenItemOriginal.wPosX;
			NetItemUpdate.wPosY       = sInvenItemOriginal.wPosY;
			NetItemUpdate.sItemCustom = sInvenItemOriginal.sItemCustom;
			SendToClient( &NetItemUpdate );

			// 로그
			LogItemExchange( sInvenItemOriginal.sItemCustom, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, m_CharDbNum, ITEM_ROUTE_TEMP_ENCHANT, 1, GetCurrentMap(), LOG_ITEM_TEMP_ENCHANT_ERASE, true );

		}
	}

    return S_OK;
}

bool GLChar::IsProducting()
{
    if ( IsMotion((EMANI_MAINTYPE)GLCONST_CHAR::sProductMotionID.wMainID, (EMANI_SUBTYPE)GLCONST_CHAR::sProductMotionID.wSubID) )
	{
		return true;
	}

	return false;
}

void GLChar::SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec, FunctionVoid fnCallBack )
{
	TurnAction( GLAT_MOTION );

	m_eMotionMID = eMID;
	m_eMotionSID = eSID;
	m_fMotionSec = fSec;
	m_fnOnMotionFinish = fnCallBack;
}

const bool GLChar::IsMotion(EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID) const
{
    if( isAction( GLAT_MOTION ) &&
        m_eMotionMID == eMID &&
        m_eMotionSID == eSID )
    {
        return true;
    }

    return false;
}

void GLChar::EventMotionFinish()
{
    GLLandMan* const pLand( GetLandMan() );
    if ( pLand == NULL )
        return;

    InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
    if ( _pInstance == NULL )
        return;

    _pInstance->EventMotionFinish(CROW_PC, CharDbNum(), m_eMotionMID, m_eMotionMID, m_fMotionSec);
}

void GLChar::SendMsgReferChar( bool bToClient, DWORD nRequestClientSlot, DWORD nRequestCharDbNum )
{
	// Puton
	{
		ReferChar::SNET_MSG_REFER_CHAR_PUTON NetMsg;
		NetMsg.SetTargetName( m_szName );
		NetMsg.m_dwTargetGaeaId = m_dwGaeaID;
		NetMsg.m_nRequestCharDbNum = nRequestCharDbNum;

		for( BYTE i=0; i<SLOT_TSIZE; ++i )
		{
			if( m_PutOnItems[ i ].GetNativeID() == NATIVEID_NULL() )
				continue;

			NetMsg.Push( ReferChar::SPutonItem( i, m_PutOnItems[ i ] ) );

			if( NetMsg.m_nNum == ReferChar::SNET_MSG_REFER_CHAR_PUTON::EMaxCount )
			{
				NetMsg.Resize();

				if( bToClient )
					SendToClient( nRequestClientSlot, &NetMsg );
				else
					SendToAgent( &NetMsg );

				NetMsg.m_nNum = 0;
			}
		}

		if( NetMsg.m_nNum > 0 )
		{
			NetMsg.Resize();

			if( bToClient )
				SendToClient( nRequestClientSlot, &NetMsg );
			else
				SendToAgent( &NetMsg );
		}
	}

	// Skill
	{
		ReferChar::SNET_MSG_REFER_CHAR_SKILL NetMsg;
		NetMsg.SetTargetName( m_szName );
		NetMsg.m_nRequestCharDbNum = nRequestCharDbNum;

		BOOST_FOREACH( SKILL_MAP::value_type& it, m_ExpSkills )
		{
			NetMsg.Push( it.second );

			if( NetMsg.m_nNum == ReferChar::SNET_MSG_REFER_CHAR_SKILL::EMaxCount )
			{
				NetMsg.Resize();

				if( bToClient )
					SendToClient( nRequestClientSlot, &NetMsg );
				else
					SendToAgent( &NetMsg );

				NetMsg.m_nNum = 0;
			}
		}

		if( NetMsg.m_nNum > 0 )
		{
			NetMsg.Resize();

			if( bToClient )
				SendToClient( nRequestClientSlot, &NetMsg );
			else
				SendToAgent( &NetMsg );
		}
	}

	// Info
	{
		ReferChar::SNET_MSG_REFER_CHAR_INFO NetMsg;
		NetMsg.SetTargetName( m_szName );
		NetMsg.m_nRequestCharDbNum = nRequestCharDbNum;

		TITLEINFO sTitle;
		m_pTitleMan->GetCurrentTitle( sTitle );
		NetMsg.m_sInfo.SetTitle( sTitle.strTitle.c_str() );

		NetMsg.m_sInfo.m_emClass = m_emClass;
		NetMsg.m_sInfo.m_wSchool = m_wSchool;
		NetMsg.m_sInfo.m_wSex = m_wSex;
		NetMsg.m_sInfo.m_wHair = m_wHair;
		NetMsg.m_sInfo.m_wHairColor = m_wHairColor;
		NetMsg.m_sInfo.m_wFace = m_wFace;

		NetMsg.m_sInfo.m_wLevel = m_wLevel;

		NetMsg.m_sInfo.m_sStats = m_sStats;

		NetMsg.m_sInfo.m_powerAttack = m_powerAttack;
		NetMsg.m_sInfo.m_powerDefnece = m_powerDefence;
		NetMsg.m_sInfo.m_powerAttribute[SKILL::EMAPPLY_MELEE] = m_powerAttribute[SKILL::EMAPPLY_MELEE];
		NetMsg.m_sInfo.m_powerAttribute[SKILL::EMAPPLY_RANGE] = m_powerAttribute[SKILL::EMAPPLY_RANGE];
		NetMsg.m_sInfo.m_powerAttribute[SKILL::EMAPPLY_MAGIC] = m_powerAttribute[SKILL::EMAPPLY_MAGIC];

		NetMsg.m_sInfo.m_nHP = m_sHP.nMax;
		NetMsg.m_sInfo.m_nMP = m_sMP.nMax;
		NetMsg.m_sInfo.m_nSP = m_sSP.nMax;

		// 부가정보
		NetMsg.m_sInfoAddition.m_fCritical = m_sSUMITEM.fIncR_Critical;
		NetMsg.m_sInfoAddition.m_fCrushingBlow = m_sSUMITEM.fIncR_CrushingBlow;

		NetMsg.m_sInfoAddition.m_fMoveSpeed = GETMOVE_TOTAL();
		NetMsg.m_sInfoAddition.m_fAtkSpeed = GETATT_TOTAL();

		NetMsg.m_sInfoAddition.m_fExpRate = GETEXPVELO();

		NetMsg.m_sInfoAddition.m_fINCR_HP_TOTAL = m_fVariationRateHP;
		NetMsg.m_sInfoAddition.m_fINCR_MP_TOTAL = m_fVariationRateMP;
		NetMsg.m_sInfoAddition.m_fINCR_SP_TOTAL = m_fVariationRateSP;
		NetMsg.m_sInfoAddition.m_nReceive_CP_Inc = m_sSUMITEM.nReceive_CP_Inc;

		NetMsg.m_sInfoAddition.m_nPotion_HP_Inc = (m_sSUMITEM.nPotion_HP_Inc + m_nPotion_HP_Inc_SKILL);
		NetMsg.m_sInfoAddition.m_nPotion_MP_Inc = (m_sSUMITEM.nPotion_MP_Inc + m_nPotion_MP_Inc_SKILL);
		NetMsg.m_sInfoAddition.m_nPotion_SP_Inc = (m_sSUMITEM.nPotion_SP_Inc + m_nPotion_SP_Inc_SKILL);

		if( bToClient )
			SendToClient( nRequestClientSlot, &NetMsg );
		else
			SendToAgent( &NetMsg );
	}
}

void GLChar::LogMonsterKill(const SNATIVEID& MonsterID, const std::string& MonsterName)
{
	if (GLogicData::GetInstance().IsClubLogKillMonster(MonsterID) == false )
		return;

	if (m_ClubDbNum != CLUB_NULL && m_pGaeaServer)
	{
		// [%1%]님이 [%2%]을(를) 쓰러뜨렸습니다.
		std::string ClubLog(
			sc::string::format(
			ID2GAMEINTEXT("CLUB_LOG_27"),
			m_szName,
			MonsterName));
		m_pGaeaServer->SENDCLUBLOG(m_ClubDbNum, club::LOG_KILL_MONSTER, ClubLog);
	}
}

void GLChar::LogPlayerKill(const std::string& KillCharName, const std::string& DeadCharName, bool bKill)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    if (m_pGaeaServer && pLand->IsClubPkRecord() && m_ClubDbNum != CLUB_NULL)
    {
        if (bKill)
        {
            // [%1%]님이 [%2%]님을 제압했습니다.
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_31"),
                    KillCharName,
                    DeadCharName));
            m_pGaeaServer->SENDCLUBLOG(m_ClubDbNum, club::LOG_KILL_PLAYER, ClubLog);
        }
        else
        {
            // [%1%]님이 [%2%]님에게 제압 당했습니다.
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_32"),
                    DeadCharName,
                    KillCharName));
            m_pGaeaServer->SENDCLUBLOG(m_ClubDbNum, club::LOG_KILL_PLAYER, ClubLog);
        }
    }
}

void GLChar::SendTriggerMessage(TriggerSystem::SMessage* msg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( msg )
	{
		pLand->SendTriggerMessage(msg);
	}
}

void GLChar::PostTriggerMessage(TriggerSystem::SpMessage msg)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	if ( msg )
	{
		pLand->PostTriggerMessage(msg);
	}
}

void GLChar::SetMovement(const CharStateID id, const CharState state)
{
    SetState(id, state, m_cMovement);
    SyncClientState(CHAR_STATE_INDEX_MOVEMENT, state);    
}

void GLChar::SetState(const CharStateID id, const CharState state, GLStateMiniFSM& fsm)
{
	/**
		false 일 때 Add 해주고 true 일 때 Remove 해줌에 주의 한다.
		이는 상태의 기본값은 true 이므로 false 일때는 기본값과 다르므로 저장해주고
		true 는 기본값과 동일하므로 저장할 필요가 없으므로 Remove 해 준다.
		이를 잘 이해해야 이부분을 수정 하는데 문제가 없을 것이다.

	 */
    if (!state)
    {
        fsm.Add(id, state);
    }
    else
    {
        fsm.Remove(id, state);
    }
}

void GLChar::SyncClientState(const CharStateIndex idx, const CharState state)
{
    GLMSG::NET_CHAR_STATE_FC NetMsg;
    NetMsg.idx = idx;
    NetMsg.state = state;

    msgpack::sbuffer buf;
    msgpack::pack(buf, NetMsg);

    SendToClient(NET_MSG_CHAR_STATE_FC, buf);
}

void GLChar::ToAllDamage_PVE(const DWORD dwDamage)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // 트리거 시스템에 데미지 전달
    TriggerSystem::SServerDamageMessage msg;
    msg.dwDamage = dwDamage;
    msg.dwCharDbNum = m_CharDbNum;        
    pLand->SendTriggerMessage(&msg);
}

void GLChar::ToAllHeal_PVE(const DWORD dwHeal)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // 트리거 시스템에 힐 전달
    TriggerSystem::SServerHealMessage msg;
    msg.dwHeal = dwHeal;
    msg.dwCharDbNum = m_CharDbNum;        
    pLand->SendTriggerMessage(&msg);
}

void GLChar::SetAlive_PVE()
{
    // 부활 완료 체크.
    ReSetActState(EM_ACT_DIE);

    // 부활시 체력 회복.
    m_fGenAge = 0.0f;
    GLCHARLOGIC::INIT_RECOVER(100);
    ResetCP();

    TurnAction(GLAT_IDLE);

    m_lnRestorableExp = 0;

    // PET
    // 맵이동시 Pet 삭제
    m_pGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(m_dwPetGUID,true,true) );

    //VEHICLE
    m_pGaeaServer->SaveVehicle( m_ClientSlot, m_dwGaeaID, true );

    // SUMMON
    //SummonAllRemove();
	RemoveAllChildCrow();

	m_sResurrectAbleState.reset();
	m_sResurrectAbleState.setState(EM_RESURRECTION_TYPE_NOT_SET);

    //	부활 확인 메시지.
	GLMSG::NET_PVE_REBIRTH_FC NetMsg;
	NetMsg.dwGaeaID = GetGaeaID();
	msgpack::sbuffer buf;
	msgpack::pack(buf, NetMsg);
	SendToClient(NET_MSG_PVE_REBIRTH_FC, buf);
	SendMsgViewAround(NET_MSG_PVE_REBIRTH_FC, buf);
}

void GLChar::ReportPositionToAgent(void)
{
	NET_CHAR_POS_FA MsgFA(m_CharDbNum);
	MsgFA.m_fPosX = m_vPosition.x;
	MsgFA.m_fPosY = m_vPosition.y;
	MsgFA.m_fPosZ = m_vPosition.z;
	SendToAgent(&MsgFA);
}

void GLChar::SetViewAround(unsigned int _ViewAround)
{
    m_uintViewAround = _ViewAround;
    m_fViewAroundSettingReleaseTimer = 60; // 기본 1분
}

void GLChar::SetViewAround(unsigned int _ViewAround,float ViewAroundDuration)
{
    m_uintViewAround = _ViewAround;
    //안전시간 등록
    m_fViewAroundSettingReleaseTimer = ViewAroundDuration;
}
bool GLChar::GetViewAroundUsed()
{ 
    //안전장치로 풀림
    if(m_fViewAroundSettingReleaseTimer <= 0 )
        return false;

    return m_uintViewAround > MAX_VIEWRANGE; 
}

const WORD GLChar::GetGradeAttack(void) const
{
	const EMSLOT emRHand(GetCurRHand());
	const SITEMCUSTOM& sRHAND(GET_SLOT_ITEM(emRHand));
	if ( sRHAND.GetNativeID() != SNATIVEID(false) )
		return sRHAND.GETGRADE(EMGRINDING_DAMAGE);

	return 0;
}

void GLChar::SetFlyCameraPos(const D3DXVECTOR3 vPos)
{
	m_vFlyCameraPos = vPos;
}

const D3DXVECTOR3& GLChar::GetViewPosition(void) const
{
	return m_bFlyCameraUse ? m_vFlyCameraPos : m_vPosition;
}

void GLChar::DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLevel  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pActorCaster = pLand->GetActor(sCasterID);

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 0;

	pLand->m_SkillTargetSystem.SelectSkillTarget( pActorCaster, this, sSkillID, wLevel, GetPositionActor(), sTARIDS, wTARNUM);

	if ( sCasterID.emCrow == CROW_PC )
	{
		GLChar* pChar = pLand->GetChar(sCasterID.GaeaId);
		if ( pChar == NULL )
			return;

		STARGETID sTargetID;
		sTargetID.emCrow = GetCrow();
		sTargetID.GaeaId = GetGaeaID();
		sTargetID.vPos = GetPosition();

		pChar->ApplySkillDealy( SCHARSKILL( sSkillID, wLevel), sTargetID, wTARNUM, sTARIDS, 1 );
	}

	if ( sCasterID.emCrow == CROW_MOB )
	{
		GLCrow* pCrow = pLand->GetCrow(sCasterID.GaeaId);
		if ( pCrow == NULL )
			return;
		pCrow->ApplySkill(TRUE, sSkillID, wLevel, wTARNUM, sTARIDS);
	}

	if ( sCasterID.emCrow == CROW_SUMMON )
	{
		GLSummonField* pSummon = pLand->GetSummon(sCasterID.GaeaId);
		if ( pSummon == NULL )
			return;
		pSummon->ApplySkill(TRUE, sSkillID, wLevel, wTARNUM, sTARIDS);
	}

	if ( sSkillID.IsValidNativeID() )
	{
		GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION NetMsgDealy;
		NetMsgDealy.emCrow		= GetCrow();
		NetMsgDealy.dwID		= m_dwGaeaID;
		NetMsgDealy.skill_id	= sSkillID;
		NetMsgDealy.wLEVEL		= wLevel;
		NetMsgDealy.vTARPOS		= m_vPosition;

		for( WORD i=0; i<wTARNUM; ++i )
			NetMsgDealy.ADDTARGET ( sTARIDS[i] );

		SendToClient(&NetMsgDealy );

		GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD NetMsgBRD;
		NetMsgBRD.emCrow	= GetCrow();
		NetMsgBRD.dwID		= m_dwGaeaID;
		NetMsgBRD.skill_id	= sSkillID;
		NetMsgBRD.wLEVEL	= wLevel;
		NetMsgBRD.vTARPOS	= m_vPosition;

		for( WORD i=0; i<wTARNUM; ++i )
			NetMsgBRD.ADDTARGET ( sTARIDS[i] );

		SendMsgViewAround(&NetMsgBRD);
	}
}

void GLChar::SelfSkillStart( SNATIVEID sSkillID, WORD wLevel )
{	
	if( sSkillID == NATIVEID_NULL() )
		return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 1;

	STARGETID sTargetID;
	sTargetID.emCrow = GetCrow();
	sTargetID.GaeaId = GetGaeaID();
	sTargetID.vPos = GetPosition();

	SSTRIKE sStrike;
	sStrike.bLowSP =false;
	sStrike.bHit =true;

	sTARIDS[0] = STARID( CROW_PC, GetGaeaID());
	
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );

		if( pSkill == NULL ) return;

		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
			return;		

		SkillProc( SCHARSKILL( sSkillID, wLevel), sStrike.bLowSP, sTargetID, wTARNUM, sTARIDS, 1 );			
	}
}

LONGLONG GLChar::AddInvenMoneyServer( LONGLONG llMoney, int nCause /*= INVEN_MONEY_DEFAULT*/ )
{
	if ( llMoney < 0 )
	{
		return m_lnMoney;
	}

	if ( NULL == m_pGaeaServer )
		return m_lnMoney;
	
	m_pGaeaServer->OnChangeMoney( static_cast< EMGAME_MONEY_CHANGE >( nCause ), llMoney );
	m_pGaeaServer->OnChangeMapMoney( static_cast< EMGAME_MONEY_CHANGE >( nCause ), GetCurrentMap().getBaseMapID().Mid(), llMoney );

	if ( GLCONST_CHAR::NotifyMoney <= llMoney )
	{
		LONGLONG OldMoney = m_lnMoney;
		LONGLONG NewMoney = AddInvenMoney( llMoney );
		MoneyNotify( OldMoney, NewMoney, nCause );
		return NewMoney;
	}
	else
	{
		return AddInvenMoney( llMoney );
	}
}

LONGLONG GLChar::SubtractInvenMoneyServer( LONGLONG llMoney, int nCause /*= INVEN_MONEY_DEFAULT*/ )
{
	if ( llMoney < 0 )
	{
		return m_lnMoney;
	}

	if ( NULL == m_pGaeaServer )
		return m_lnMoney;

	m_pGaeaServer->OnChangeMoney( static_cast< EMGAME_MONEY_CHANGE >( nCause ), llMoney );
	m_pGaeaServer->OnChangeMapMoney( static_cast< EMGAME_MONEY_CHANGE >( nCause ), GetCurrentMap().getBaseMapID().Mid(), llMoney );

	if ( GLCONST_CHAR::NotifyMoney <= llMoney )
	{
		LONGLONG OldMoney = m_lnMoney;
		LONGLONG NewMoney = SubtractInvenMoney( llMoney );
		MoneyNotify( OldMoney, NewMoney, nCause );
		return NewMoney;
	}
	else
	{
		return SubtractInvenMoney( llMoney );
	}
}

void GLChar::MoneyNotify( LONGLONG OldMoney, LONGLONG NewMoney, int nCause )
{
	NET_CHAR_MONEY_NOTIFY_FA NetMsg;
	NetMsg.dwChaDbNum = m_CharDbNum;
	NetMsg.llOldMoney = OldMoney;
	NetMsg.llNewMoney = NewMoney;
	NetMsg.tDate = CTime::GetCurrentTime().GetTime();
	NetMsg.nCause = nCause;
	NetMsg.nChannel = GetServerChannel();
	NetMsg.nField = GetFieldSvrID();

	SendToAgent( &NetMsg );
}

void GLChar::Decompose_GenerateItem(SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult)
{
	float fRate    = sc::Random::getInstance().GetFloatPercent();
	float fNowRate = 0.0f;

	SGENINFO::VEC_SPGENITEM_ITER iter = vecGenItem.begin();
	for (; iter!=vecGenItem.end(); ++iter)
	{
		SGENITEMBASE* pItem = (*iter).get();

		if (!pItem)
			continue;

		float fItemRate = pItem->fRate;
		fNowRate += fItemRate;

		if ( fRate<fNowRate )
		{
			pItem->GetResult(sResult);
			break;
		}
	}
}

BOOL GLChar::ApplyItemSkillRun( const SCHARSKILL& sSkill, PGLSKILL pSkill, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS, std::vector<sItemSkill>& info )
{
	if( pSkill == NULL ) return FALSE;

	GLLandMan* const pLand( GetLandMan() );

	if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))	return FALSE;		

	WORD wEmType = EMITEM_SKILL_SIZE;

	switch( TargetID.emCrow )
	{
	case CROW_PC  :
		{			
			if( m_dwGaeaID == TargetID.GaeaId ) {	// 자신
				wEmType = (WORD)EMITEM_SKILL_SELF;
			}
			else {
				ServerActor* pTargetActor = GetTarget ( pLand, TargetID );

				bool bREACT = ( IsReActionable(pTargetActor, EMIMPACT_SIDE_ENEMY) ? true : false );

				if( bREACT ) {
					// 적캐릭터
					wEmType = (WORD)EMITEM_SKILL_ENERMY;
					
				} else {
					// 아군 캐릭터
					wEmType = (WORD)EMITEM_SKILL_SELF;
				}
			}
		}
		break;
	case CROW_MOB :
		{			
			wEmType = (WORD)EMITEM_SKILL_MOB;
		}
		break;
	}

	BOOL bResult = FALSE;

	if( wEmType != EMITEM_SKILL_SIZE )
	{
		bResult = DoItemSkill( pSkill, wEmType, info );
	}

	return bResult;
}

DWORD GLChar::GetNearestEnemy(void)
{
	const MapID& mapID(GetCurrentMap());
	const SNATIVEID baseMapID(mapID.getBaseMapID());
	const SNATIVEID gaeaMapID(mapID.getGaeaMapID());
	GLLandMan* const pLand(m_pGaeaServer->GetLand(gaeaMapID));
	if ( pLand == NULL )
	{
		sc::writeLogError(sc::string::format("GetNearestEnemy() : pLand == NULL, dbNum:%1% CharMapID(%2%/%3%, %4%/%5%)", 
			m_CharDbNum, 
			baseMapID.Mid(), baseMapID.Sid(), gaeaMapID.Mid(), gaeaMapID.Sid()));
		return NULL;
	}

	const D3DXVECTOR3& vPosition = GetViewPosition();
	const int nPosition[2] = {
		int(vPosition.x), 
		int(vPosition.z)
	};

	const int _maxViewRange( MAX_VIEWRANGE );
	BOUDRECT bRect(nPosition[0]+_maxViewRange, nPosition[1]+_maxViewRange, nPosition[0]-_maxViewRange, nPosition[1]-_maxViewRange);

	float fDistanceFromChar(100000.0f);
	DWORD dwNearestGaeaID(GAEAID_NULL);
	LANDQUADNODE* pQuadHead = NULL;
//	LANDQUADNODE* pQuadArray[EMDIVISION::QUAD_DIV_UNIT];
	pLand->GetLandTree()->FindNodes ( bRect, pLand->GetLandTree()->GetRootNode(), &pQuadHead );
	
	GLCROWLIST crowList;
	this->LandNodeCheckHierarchy(pQuadHead, &crowList);/*crowList.m_pTail*/
	for ( GLCROWNODE* pNode = crowList.m_pHead; pNode != NULL; pNode = pNode->pNext )
	{
		GLCrow* pCrow = pNode->Data;
		D3DXVECTOR3 vDistFromChar(this->GetPosition()-pCrow->GetPosition());
		const float fDist(D3DXVec3Length(&vDistFromChar));
		if ( fDist < fDistanceFromChar &&
			!pLand->IsLayerSimpleCollision( this->GetPosition(), pCrow->GetPosition(), 5.f ) )
		{
			fDistanceFromChar = fDist;
			dwNearestGaeaID = pCrow->GetGaeaID();
		}
	}
	return dwNearestGaeaID;
}

BOOL GLChar::LandNodeCheckHierarchy(LANDQUADNODE* pQuadNode, GLCROWLIST* crowList)
{
	BOOL bFind(FALSE);
	if ( pQuadNode->pNode11/* && pQuadNode->pNode11 != pQuadNode*/ )
	{
		bFind |= LandNodeCheckLowChunk(pQuadNode->pNode11, crowList);
	}
	if ( pQuadNode->pNode12 )
	{
		bFind |= LandNodeCheckLowChunk(pQuadNode->pNode12, crowList);
	}
	if ( pQuadNode->pNode21 )
	{
		bFind |= LandNodeCheckLowChunk(pQuadNode->pNode21, crowList);
	}
	if ( pQuadNode->pNode22 )
	{
		bFind |= LandNodeCheckLowChunk(pQuadNode->pNode22, crowList);
	}

	if ( bFind == FALSE && pQuadNode->pParent )
	{
		return LandNodeCheckHierarchy(pQuadNode->pParent, crowList);
	}
	return bFind;
}

BOOL GLChar::LandNodeCheckLowChunk(LANDQUADNODE* pQuadNode, GLCROWLIST* crowList)
{
	if ( pQuadNode == NULL )
		return FALSE;

	if ( pQuadNode->dwFlag == QUAD_LEAF )
	{
		if ( pQuadNode->pData != NULL )
		{
			GLCROWNODE* pCrowNode = pQuadNode->pData->m_CROWList.m_pHead;
			BOOL bInsert(FALSE);
			for ( ; pCrowNode; pCrowNode = pCrowNode->pNext)
			{
				GLCrow* pGLCrow = pCrowNode->Data;
				if ( pGLCrow->GetCrow() == CROW_MOB && pGLCrow->IsValidBody() == true )
				{
					crowList->ADDTAIL(pCrowNode->Data);
					bInsert = TRUE;
				}
			}
			return bInsert;
		}
		return FALSE;
	}
	
	BOOL bFind(FALSE);
	bFind |= LandNodeCheckLowChunk(pQuadNode->pNode11, crowList);
	bFind |= LandNodeCheckLowChunk(pQuadNode->pNode12, crowList);
	bFind |= LandNodeCheckLowChunk(pQuadNode->pNode21, crowList);
	bFind |= LandNodeCheckLowChunk(pQuadNode->pNode22, crowList);
	return bFind;
}

const bool GLChar::RECEIVE_SKILLFACT_ScreenThrow(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	ResetTargetID();

	GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
	NetMsg.sSkillID = sSkillID;
	NetMsg.wLevel = wLevel;
	NetMsg.sTargetID = STARGETID( GetCrow(), m_dwGaeaID);
	SendToClient(&NetMsg);

	return true;
}

const bool GLChar::RECEIVE_SKILLFACT_CameraHit(const STARGETID& sID, const SKILL::SSPEC& sSPEC, const SNATIVEID& sSkillID, const WORD wLevel)
{
	ResetTargetID();

	GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
	NetMsg.sSkillID = sSkillID;
	NetMsg.wLevel = wLevel;
	NetMsg.sTargetID = STARGETID( GetCrow(), m_dwGaeaID);
	SendToClient(&NetMsg);

	return true;
}

const bool GLChar::RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	if( !CheckStateToGo() ) return false;
	if( RF_ENTRANCE_EXIT( m_EffSkillVarSet ).IsOn() ) return false;

	GLLandMan* const pLand(GetLandMan());

	if ( pLand == NULL ) return false;

	SNATIVEID sTargetMobID = SNATIVEID( (DWORD)sSPEC.fVAR1, (DWORD)sSPEC.fVAR2 );
	
	if ( sID.emCrow != CROW_SUMMON || sTargetMobID == NATIVEID_NULL() ) return false;
	
	PGLSUMMONFIELD pSummon = dynamic_cast<PGLSUMMONFIELD>( GetTarget(sID) );

	if( pSummon == NULL ) return false;

	ServerActor* pOwner = pSummon->GetOwner();

	if( (pOwner == NULL) ||
		(pOwner && pOwner->GetCrow() != CROW_PC) ) return false;
	
	CROWIDLIST_ITER _iter = pOwner->m_listCrowID.begin();

	DWORD dwID = pOwner->GetGaeaID();

	while( _iter != pOwner->m_listCrowID.end() )
	{
		Faction::GLActorID sDestID = *_iter;

		switch( sDestID.actorType )
		{
		case CROW_SUMMON :
			{
				PGLSUMMONFIELD pSummonDst = pLand->GetSummon(sDestID.actorID_Num);
								
				if( pSummonDst && pSummonDst->m_sSummonID == sTargetMobID )
				{
					if( pSummonDst->GetOwner() && pSummonDst->GetOwner()->GetGaeaID() == dwID )
					{						
						ResetTargetID();

						GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
						NetMsg.sSkillID = sSkillID;
						NetMsg.wLevel = wLevel;
						NetMsg.sTargetID = STARGETID( GetCrow(), m_dwGaeaID);
						SendToClient(&NetMsg);

						D3DXVECTOR3 vDest = pSummonDst->GetPosition();
						D3DXVECTOR3 vDestRandomPos;

						GetRandomPos( &vDestRandomPos, &vDest, sc::Random::RandomNumber( 10.f, 30.f ) );

						vDestRandomPos.y = vDest.y;

						// Gen이 아닌 같은맵내 이동.
						if( MoveTo( &vDestRandomPos, true, MOVE_OPTION_NOT_TURN_IDLE ) )
						{
							GLPetField* pPet = m_pGaeaServer->GetPET( m_dwPetGUID );

							if( pPet && pPet->IsValid() ) {
								pPet->SetJumpPos(vDestRandomPos);
							}

							return true;
						}
					}					
				}
			}			
			break;
		}

		++_iter;
	}

	return false;
}

const bool GLChar::RECEIVE_SKILLFACT_FeignDeath( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	//GLLandMan* const pLand(GetLandMan());

	//if ( pLand == NULL ) return false;

	BOOL bTargetEnable = sSPEC.dwFLAG & FEIGN_DEATH_BUF_DEL;

	if( bTargetEnable ) RemoveSkillFactofNormalBuf();

	return true;
}

const bool GLChar::RECEIVE_SKILLFACT_Provoke( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	if( IsCheckedSkillFlagSpecial( EMSPECA_COUNTATTACK ) || IsCheckedSkillFlagSpecial(EMSPECA_DOMINATION ) )
	{
		return false;
	}

	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		if( RF_PROVOKE( m_EffSkillVarSet ).GetPreTargetID().ISNULL() )
			RF_PROVOKE( m_EffSkillVarSet ).SetPreTargetID( GetTargetID() );
	}
	
	return true;
}

void GLChar::End_FeignDeath( BOOL bForce )
{
	ReSetActState(EM_ACT_FEIGN_DIE);

	EMACTIONTYPE eAction = GetAction();

	if( eAction != GLAT_SKILL && eAction != GLAT_SKILL_WAIT )
		TurnAction(GLAT_IDLE);
}

void GLChar::UpdateSkillEff( float fElapsedTime )
{
	/// 변장 ///////////////////////////////////////////////////
	BOOL bOn   = RF_DISGUISE( m_EffSkillVarSet ).IsOn();
	UINT Index = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

	SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
	if ( pItem )
	{
		// Table ID 가 같을 경우에만 Index를 변경한다;
		// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
		if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
			Index = pItem->sPet.sPetID.Sid();
	}

	if( m_SkillDistinguish.AutoResetDisting( Index, bOn ) ) {
		Stop_Disguise();
	}
	else {
		Play_Disguise( Index, bOn );
	}
	/// 도발 ////////////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_TargetID ) SetTargetID( sID );
	}
	/// 피해발동 ////////////////////////////////////////////////
	if( RF_ACTIVE( m_EffSkillVarSet ).IsOn() )
	{
		Play_ActiveDamage();
	}
}

void GLChar::Play_Disguise( int iIdx, BOOL bOn )
{
	if( !bOn || iIdx == 0 ) return;

	if( m_SkillDistinguish.GetDistinguishCharData( iIdx, true ) )
	{
		int dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

		if( dwTableID >= 0 )
		{
			SKILL_MAP_ITER _iter = m_ExpSkills.begin();

			while( _iter != m_ExpSkills.end() )
			{
				GLSKILL* pOrgSkill = GLSkillMan::GetInstance().GetData( _iter->second.sNativeID );

				if( pOrgSkill ) {
					
					DWORD dwID = _iter->first;

					if( GSCL_INST.GetChangeSkill( dwTableID, dwID ) )
					{
						LEARN_CHANGE_SKILL( pOrgSkill, _iter->second.wLevel );
					}
				}

				++_iter;
			}

			INIT_DATA ( FALSE, FALSE );
		}		
	}
}

void GLChar::Stop_Disguise( )
{
	INIT_DATA ( FALSE, FALSE );
}

void GLChar::Play_ActiveDamage()
{
	WORD		wLv		= 0;
	DWORD		dwSkillId = 0;
	STARGETID	sID;

	if( RF_ACTIVE( m_EffSkillVarSet ).Invoke( wLv, dwSkillId, sID ) )
	{
		InvokeActionSkillStart( sID, SNATIVEID(dwSkillId), wLv );
	}
}

void GLChar::InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pTarget = pLand->GetActor(sTargetID);

	if( pTarget == NULL || !sSkillID.IsValidNativeID() ) return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 0;

	pLand->m_SkillTargetSystem.SelectSkillTarget( this, pTarget, sSkillID, wLevel, pTarget->GetPositionActor(), sTARIDS, wTARNUM);

	STARGETID sTarID = STARGETID( sTargetID.emCrow, sTargetID.GaeaId, pTarget->GetPosition() );

	ApplySkillDealy( SCHARSKILL( sSkillID, wLevel), sTarID, wTARNUM, sTARIDS, 1 );

	switch( sTarID.emCrow )
	{
	case CROW_PC :
		{
			GLChar* pChar = dynamic_cast<GLChar*>( pTarget );

			if( pChar ) {
				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION NetMsgDealy;
				NetMsgDealy.emCrow		= sTarID.emCrow;
				NetMsgDealy.dwID		= sTarID.GaeaId;
				NetMsgDealy.skill_id	= sSkillID;
				NetMsgDealy.wLEVEL		= wLevel;
				NetMsgDealy.vTARPOS		= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgDealy.ADDTARGET ( sTARIDS[i] );

				pChar->SendToClient(&NetMsgDealy );

				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD NetMsgBRD;
				NetMsgBRD.emCrow	= sTarID.emCrow;
				NetMsgBRD.dwID		= sTarID.GaeaId;
				NetMsgBRD.skill_id	= sSkillID;
				NetMsgBRD.wLEVEL	= wLevel;
				NetMsgBRD.vTARPOS	= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgBRD.ADDTARGET ( sTARIDS[i] );

				pChar->SendMsgViewAround(&NetMsgBRD);
			}
		}
		break;
	case CROW_MOB :
		{
			GLCrow* pMob = dynamic_cast<GLCrow*>( pTarget );

			if( pMob ) {

				GLMSG::SNETCROW_SKILL_DEALY_ACTION NetMsg(sTarID.GaeaId);

				NetMsg.dwAType   = pMob->m_dwAType;
				NetMsg.skill_id  = sSkillID;
				NetMsg.skill_lev = wLevel;
				NetMsg.vTARPOS   = pMob->GetTarPos();
				NetMsg.vPos      = sTarID.vPos;

				for (WORD i=0; i<wTARNUM; ++i)
					NetMsg.ADDTARGET(sTARIDS[i]);
				pMob->SendMsgViewAround(&NetMsg);
			}
		}
		break;
	case CROW_SUMMON :
		{
			GLSummonField* pSummon = dynamic_cast<GLSummonField*>( pTarget );

			if( pSummon ) {
				
				GLMSG::SNET_SUMMON_SKILL NetMsg( sTarID.GaeaId
					, pSummon->GetTargetID(), pSummon->m_dwAType
					, pSummon->m_bUsingEventSkill
					, sSkillID
					, wLevel
					, pSummon->GetTarPos() );

				for ( WORD i=0; i<wTARNUM; ++i )	
					NetMsg.ADDTARGET ( sTARIDS[i] );

				pSummon->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		break;
	}
}

void GLChar::LockCharSlot ( const WORD _wCharSlotNum )
{
	GLMSG::SNETPC_RESPONSE_LOCK_CHARSLOT_FC sNetMsg( EMCHARSLOT_LOCK_FAIL );
	sNetMsg.dwChaNum = CharDbNum();

	if ( NULL == m_pGaeaServer )
		return;

	if ( false == GLUseFeatures::GetInstance().IsUsingSelectCharSlot() )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 0번 슬롯에 Lock을 거는것은 안된다;
	if ( _wCharSlotNum <= 0 )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	if ( EMCHAR_SLOT_DATA_SIZE <= _wCharSlotNum )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 기본 활성화 슬롯에는 Lock 을 걸 수 없다;
	if ( _wCharSlotNum < GLCharSlotMan::Instance()->GetDefaultActiveSlotCount() )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 이미 Lock 이 걸려있는 경우에는 락을 걸 수 없다;
	if ( true == m_arrLockSlotState[ _wCharSlotNum ] )
	{
		sNetMsg.emFlag = EMCHARSLOT_LOCK_ALREADY;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	m_arrLockSlotState[ _wCharSlotNum ] = true;

	sNetMsg.emFlag = EMCHARSLOT_LOCK_SUCCESS;
	sNetMsg.wCharSlotNum = _wCharSlotNum;
	m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );
}

void GLChar::UnlockCharSlot ( const WORD _wCharSlotNum )
{
	GLMSG::SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC sNetMsg( EMCHARSLOT_UNLOCK_FAIL );
	sNetMsg.dwChaNum = CharDbNum();

	if ( NULL == m_pGaeaServer )
		return;

	if ( false == GLUseFeatures::GetInstance().IsUsingSelectCharSlot() )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	if ( _wCharSlotNum < 0 || EMCHAR_SLOT_DATA_SIZE <= _wCharSlotNum )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 오픈되어 있지 않은 슬롯은 Unlock 할 수 없다;
	if ( GLCharSlotMan::Instance()->GetOpenSlotCount() <= _wCharSlotNum )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	if ( false == m_arrLockSlotState[ _wCharSlotNum ] )
	{
		sNetMsg.emFlag = EMCHARSLOT_UNLOCK_ALREADY;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	m_arrLockSlotState[ _wCharSlotNum ] = false;

	sNetMsg.emFlag = EMCHARSLOT_UNLOCK_SUCCESS;
	sNetMsg.wCharSlotNum = _wCharSlotNum;
	m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );
}

void GLChar::ChangeCharSlot ( const WORD _wCharSlotNum )
{
	GLMSG::SNETPC_RESPONSE_CHANGE_CHARSLOT_FC sNetMsg( EMCHARSLOT_CHANGE_FAIL );
	sNetMsg.dwChaNum = CharDbNum();

	if ( NULL == m_pGaeaServer )
		return;

	if ( false == GLUseFeatures::GetInstance().IsUsingSelectCharSlot() )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	if ( _wCharSlotNum < 0 || EMCHAR_SLOT_DATA_SIZE <= _wCharSlotNum )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 오픈되어 있지 않은 슬롯으로 Change 할 수 없다;
	if ( GLCharSlotMan::Instance()->GetOpenSlotCount() <= _wCharSlotNum )
	{
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 같은 슬롯이라면 변경할 수 없다;
	if ( m_wSlotIndex == _wCharSlotNum )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_SAMESLOT;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 현재 맵이 캐릭터 슬롯 변경불가 맵이라면 변경할 수 없다;
	const SMAPNODE* pMapNode = m_pGaeaServer->FindMapNode( GetCurrentMap().getBaseMapID() );
	if ( NULL == pMapNode || false == pMapNode->IsChangeCharSlot() )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_LIMITMAP;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 쿨타임 중이라면 변경할 수 없다;
	if ( m_fCoolTime_ChangeCharSlot < GLCharSlotMan::Instance()->GetCoolTime_ChangeSlot() )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_COOLTIME;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 캐릭터가 죽은 상태라면 변경할 수 없다;
	if ( false == IsAlive() )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_DIE;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 캐릭터가 액션중이라면 변경할 수 없다;
	if ( false == isAction( GLAT_IDLE ) )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_ACTION;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	WORD wBackUpSlotIndex = m_wSlotIndex;
	if ( false == CHANGE_CHARACTER_SLOT( _wCharSlotNum ) )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_LOCK;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	// 쿨타임을 준다;
	m_fCoolTime_ChangeCharSlot = 0.f;

	// Normal 스킬 제거;
	RemoveSkillFactofNormalAll();

	// 장착아이템 검사;
	if ( false == CheckEquipedItems( false ) )
	{
		if ( false == CHANGE_CHARACTER_SLOT( wBackUpSlotIndex ) )
		{
			// 원래 사용하던 슬롯으로 다시 돌아가는 것을 실패할 수는 없다;
			sc::writeLogError( "[ Character Slot ] [ Critical Error!! Failed restore this character slot! ]" );
		}

		sNetMsg.emFlag = EMCHARSLOT_CHANGE_INVENLACK;
		m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );

		return;
	}

	sNetMsg.emFlag = EMCHARSLOT_CHANGE_SUCCESS;

	// Change Character Solt Num;
	sNetMsg.wCharSlotNum = _wCharSlotNum;

	// Stats;
	sNetMsg.sStats = m_sStats;
	sNetMsg.wStatPoint = m_wStatsPoint;

	// Skill Quick Slot;
	sNetMsg.wSKILLQUICK_ACT = m_wSKILLQUICK_ACT;
	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )
		sNetMsg.sSKILLQUICK[ i ] = m_sSKILLQUICK[ i ];

	// Skill;
	SKILL_MAP_ITER iter = m_ExpSkills.begin();
	for ( ; iter != m_ExpSkills.end(); ++iter )
	{
		sNetMsg.AddSkill( iter->second );
	}
	sNetMsg.dwSkillPoint = m_dwSkillPoint;

	sc::writeLogInfo ( sc::string::format(
		"Size : %1% / %2%",
		sNetMsg.Size(),
		sizeof( GLMSG::SNETPC_RESPONSE_CHANGE_CHARSLOT_FC ) ) );

	m_pGaeaServer->SENDTOCLIENT( ClientSlot(), &sNetMsg );
}

void GLChar::NotifyKillSomeone(DWORD dwGaeaID)
{
	if ( IsAlive() )
	{
		const float fIncR_HPMPSPPerKill(GetIncR_HPMPSPPerKill());
		m_sHP.INCREASE((GetIncR_LifePerKill()+fIncR_HPMPSPPerKill)*100.0f, true);
		m_sMP.INCREASE((GetIncR_MPPerKill()+fIncR_HPMPSPPerKill)*100.0f, true);
		m_sSP.INCREASE((GetIncR_SPPerKill()+fIncR_HPMPSPPerKill)*100.0f, true);
	}
}
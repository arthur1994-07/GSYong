#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "./CapturedTheField/GLCaptureTheFieldClient.h"
#include "./GLGaeaClient.h"
#include "./Land/GLLandManClient.h"
#include "./Crow/GLCrowClient.h"
#include "./Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

//--------------------------------------------------------------------------------------------//

const bool GLGaeaClient::IsCTFPlayer()
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return false;
    }

    return (m_pCaptureTheField->IsPlayer());
}

const bool GLGaeaClient::IsCTFMap()
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return false;
    }

	if ( NULL == m_pCaptureTheField )
	{
		return false;
	}

    return m_pCaptureTheField->IsCTFMap( GetActiveMapID().getGaeaMapID() );
}

const bool GLGaeaClient::IsCTFMap_PVP ( void ) const
{
	if ( false == GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return false;
	}

	if ( NULL == m_pCaptureTheField )
	{
		return false;
	}

	return m_pCaptureTheField->IsCTFMap_PVP( GetActiveMapID().getGaeaMapID() );
}

const bool GLGaeaClient::IsCTFMap_PVE ( void ) const
{
	if ( false == GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return false;
	}

	if ( NULL == m_pCaptureTheField )
	{
		return false;
	}

	return m_pCaptureTheField->IsCTFMap_PVE( GetActiveMapID().getGaeaMapID() );
}

void GLGaeaClient::MsgCaptureTheFieldReady(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_READY_AC *pPacket = (GLMSG::SNET_CTF_READY_AC *)pMsg;

    //! 발전소 점령전으로만 세팅이 되어있습니다
    //  추후에 다른지역에도 필요할 시에 수정을해야합니다.

    
    //! CTF 맵이라면 따로 Warning 메세지가 갑니다
    std::string strAlarm = sc::string::format(ID2GAMEINTEXT("CTF_MSG_READY"), pPacket->nRemainMinutes);
	if  ( false == IsCTFMap_PVP() )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTFNotify_Ready, "-n", pPacket->nRemainMinutes);
	}
    PrintConsoleText ( strAlarm );

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );
    sCTFState.nREMAINMINUTES = pPacket->nRemainMinutes;
	sCTFState.bisConsecutively= pPacket->bConsecutively;

	if ( sCTFState.emSTATE != CTF_STATE_READY )
	{
		CTime tCurTime(GetCurrentTime());
		GetConvertServerTime(tCurTime);

		sCTFState.tSTARTTIME = tCurTime.GetTime() + (static_cast<__time64_t>(pPacket->nRemainMinutes) * 60);
		sCTFState.tSTOPTIME	 = sCTFState.tSTARTTIME + sCTFState.tPLAYTIME;
		sCTFState.emSTATE    = CTF_STATE_READY;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldJoinFB(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_JOIN_RESULT_AC *pPacket = (GLMSG::SNET_CTF_JOIN_RESULT_AC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

	GLCharacter* pchar = GetCharacter();
	if(!pchar)
		return;
	
    if ( sCTFState.IsCANNOTJOIN(pchar->GetCharSchool()) )
    {
        GASSERT(!"MsgCaptureTheFieldJoinFB - CTF state is stop");
        return;
    }

	std::string reason;

	switch(pPacket->emJoinResult)
	{
	case CTF_JOIN_NO_CHAR:
		reason = ID2GAMEINTEXT("CTF_WARNING_JOIN_NO_CHAR");
		break;

	case CTF_JOIN_WRONG_CHANNEL:
		reason = ID2GAMEINTEXT("CTF_WARNING_JOIN_WRONG_CHANNEL");
		break;

	case CTF_JOIN_WRONG_LEVEL:
		reason = ID2GAMEINTEXT("CTF_WARNING_JOIN_WRONG_LEVEL");
		break;

	case CTF_JOIN_WRONG_PVE:
		reason = ID2GAMEINTEXT("CTF_WARNING_JOIN_WRONG_PVE");
		break;
    case CTF_JOIN_WRONG_MATCHING_CONTENTS:
        reason = ID2GAMEINTEXT("CTF_WARNING_JOIN_WRONG_MATCHING_CONTENTS");
        break;
	};

    if ( pPacket->bJoin )
    {
        if ( pPacket->nQueued == 0 )
        {
            PrintConsoleTextDlg ( ID2GAMEINTEXT("CTF_MSG_WAIT") );
        }
        else if (pPacket->nQueued > 0)
        {
            PrintConsoleTextDlg ( sc::string::format(ID2GAMEINTEXT("CTF_MSG_WAIT_QUEUED"), pPacket->nQueued) );
        }
        else if ( pPacket->emJoinResult != CTF_JOIN_OK )
        {
            sCTFState.bWAITJOIN = false;
            sCTFState.nQUEUE = 0;

            PrintConsoleTextDlg (sc::string::format(ID2GAMEINTEXT("CTF_WARNING_JOIN"), reason));

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );

            return;
        }
	
        sCTFState.bWAITJOIN  = true;
        sCTFState.nQUEUE = pPacket->nQueued;
    }
    else
    {
		if ( pPacket->emJoinResult == CTF_JOIN_OK )
			PrintConsoleTextDlg (ID2GAMEINTEXT("CTF_MSG_JOIN_CANCEL"));
		else
			PrintConsoleTextDlg (sc::string::format(ID2GAMEINTEXT("CTF_WARNING_JOIN"), reason));

        sCTFState.bWAITJOIN  = false;
        sCTFState.nQUEUE  = 0;
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldStartToAll(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_START_2ALL_AC *pPacket = (GLMSG::SNET_CTF_START_2ALL_AC *)pMsg;

	//상태 저장하자.
	

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.emAUTHSCHOOL = sCTFState.emAUTHSCHOOL;
	sPrevState.bisConsecutively = sCTFState.bisConsecutively;

	sCTFState.tSTARTTIME = pPacket->tStartTime;
	sCTFState.tSTOPTIME	 = sCTFState.tSTARTTIME + sCTFState.tPLAYTIME;
    sCTFState.emSTATE    = CTF_STATE_START;
    sCTFState.RESETAUTH();

    PrintConsoleTextDlg ( ID2GAMEINTEXT("CTF_MSG_STARTTOALL") );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );
}

void GLGaeaClient::MsgCaptureTheFieldStartToPlayers(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_START_2PLAYERS_FC *pPacket = (GLMSG::SNET_CTF_START_2PLAYERS_FC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );
    sCTFState.bWAITJOIN = false;

    //! 인증기 세팅
    {
        GASSERT( sCTFState.vecAUTHENTICATOR.size() <= 3 );

        sCTFState.vecAUTHENTICATOR[0].vPos = pPacket->dxPos1;
        sCTFState.vecAUTHENTICATOR[1].vPos = pPacket->dxPos2;
        sCTFState.vecAUTHENTICATOR[2].vPos = pPacket->dxPos3;

		sCTFState.vecAUTHENTICATOR[0].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert1);
		sCTFState.vecAUTHENTICATOR[1].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert2);
		sCTFState.vecAUTHENTICATOR[2].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert3);
    }

	// 초기 인증기 정보 세팅, 루아에서 초기값 세팅을 안해주면 갱신이 안되는것같다;
	int nAuthActorSize = 0;
	GASSERT( (nAuthActorSize=sCTFState.vecAUTHENTICATOR.size()) <= 3 );

	// 키 다운 시간 갱신; CTF에서 입력 시간이 없으면 강퇴됨;
	if ( m_pCharacter )
	{
		m_pCharacter->m_fKeyDownCheckTime = 0.0f;
		m_pCharacter->m_nAwayGestureLevel = -1;
	}

	m_pCaptureTheField->SetProgressCTF( PVP::ENUM_CTF_TYPE_POWERSTATION );
	m_pCaptureTheField->ClearResultData();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFDisplayInfo_Open);
	// 인증기 갱신;
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );

    PrintConsoleTextDlg ( ID2GAMEINTEXT("CTF_MSG_STARTTOPLAYERS") );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );




	// 인증기 정보 갱신;
	GASSERT( sCTFState.vecAUTHENTICATOR.size() <= 3 );
	int nAuthenticatorCount[ SCHOOL_NUM ] = {0,0,0};
	for ( int i = SCHOOL_SM; i < SCHOOL_NUM; ++i )
	{		
		ENUM_SCHOOL emSchool = sCTFState.vecAUTHENTICATOR[i].emAuthSchool;
		if ( emSchool < SCHOOL_NUM && emSchool != SCHOOL_NONE )
			nAuthenticatorCount[ emSchool ]++;
	}
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFAuthCountInit,"-n, -n, -n"
		, nAuthenticatorCount[SCHOOL_SM]
		, nAuthenticatorCount[SCHOOL_HA]
		, nAuthenticatorCount[SCHOOL_BH]);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFNotifyInfo, "-n", (int)UI::EMPROGRESSDISPLAY_TYPE_CTF_START);
}

void GLGaeaClient::MsgCaptureTheFieldDoneToAll(NET_MSG_GENERIC* pMsg)
{
	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return;
	}

	GLMSG::SNET_CTF_DONE_AF_AC *pPacket = (GLMSG::SNET_CTF_DONE_AF_AC *)pMsg;

	GLCharacter* pChar = GetCharacter();

	if ( !pChar )
	{
		GASSERT(!"GLGaeaClient::MsgCaptureTheFieldDone - GetCharacter is null");
		return;
	}

	//! 참여중이 아니면
	if ( m_pCaptureTheField->GetProgressCTF() != PVP::ENUM_CTF_TYPE_NULL )
	{
		if ( pChar->IsDie() || pChar->isAction( GLAT_FALLING ) )
		{
            pChar->TurnAction( GLAT_DIE );

			pChar->ReqResurrect(EM_RESURRECTION_NOMAL);

			//!! SF_TODO
            //if ( m_pInterface->UiIsVisibleGroup( FORCE_REBIRTH ) )
            //    m_pInterface->UiHideGroup( FORCE_REBIRTH );
		}
	}

	GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sCTFState.emSTATE	   = CTF_STATE_DONE;
	sCTFState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wSchoolAuthed);

	//! 이전정보를 저장
	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wSchoolAuthed);
	sPrevState.tSTARTTIME	= sCTFState.tSTARTTIME;
	sPrevState.tENDTIME		= GetCurrentTime().GetTime();

	sPrevState.bConsecutivelyCapturel = sCTFState.bConsecutivelyCapturel;
	sPrevState.bisConsecutively = sCTFState.bisConsecutively;
	sPrevState.dwConsecutivelyNumber = sCTFState.dwConsecutivelyNumber;

	sCTFState.bisConsecutively = pPacket->bConsecutively;

	//! 다음시간을 임시로 세팅
	CTime tCurTime(GetCurrentTime());
	GetConvertServerTime( tCurTime );
	sCTFState.tSTARTTIME = tCurTime.GetTime() + sCTFState.tCYCLETIME + sCTFState.tEXTRATIME;
	sCTFState.tSTOPTIME  = tCurTime.GetTime() + sCTFState.tEXTRATIME;

	
	/*제작이 취소되는 문제 
	ctf turn(Start->Done) 명령 동작시 해당구문을 통하여 전투에 입장되어 있지 않는 유저도 동작이 되어서 
	CTF에 참여중인 유저에게만 명령이 유효하도록 변경*/
	if ( m_pCaptureTheField->GetProgressCTF() == PVP::ENUM_CTF_TYPE_POWERSTATION )
	{
		//! 캐릭터의 모든 행동을 멈춘다.
		m_pCharacter->ResetAction();
		m_pCharacter->ResetReservationAction();
	

		// 타겟 초기화
		m_pCharacter->ResetActionID();
		m_pCharacter->ResetCursorID();
		m_pCharacter->ResetSelectID();
		m_pCharacter->ClearReservationAction();
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldReward(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_REWARD_FC *pPacket = (GLMSG::SNET_CTF_REWARD_FC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    if ( sCTFState.IsNULL() )
    {
        GASSERT(!"GLGaeaClient::MsgCaptureTheFieldReward() - sCTFState is null");
        return;
    }

	sCTFState.emSTATE = CTF_STATE_REWARD;

    PrintConsoleTextDlg ( ID2GAMEINTEXT("CTF_MSG_STOP") );

	//! 캐릭터의 모든 행동을 멈춘다.
	m_pCharacter->ResetAction();
	m_pCharacter->ResetReservationAction();

	// 타겟 초기화
	m_pCharacter->ResetActionID();
	m_pCharacter->ResetCursorID();
	m_pCharacter->ResetSelectID();
	m_pCharacter->ClearReservationAction();

	//!! SF_TODO	
	/*switch ( sCTFState.emAUTHSCHOOL )
	{
	case SCHOOL_SM: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM, std::string("" ) ); break;
	case SCHOOL_HA: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA, std::string("" ) ); break;
	case SCHOOL_BH: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH, std::string("" ) ); break;
	};*/

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFNotifyInfo, "-n", UI::EMPROGRESSDISPLAY_TYPE_CTF_STOP);

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", (int)CTF_STATE_REWARD);
	
	
	//CTField 리워드 보상창
	CTime tCurTime( GetCurrentTime() );
	CTimeSpan sTimeLeft( sCTFState.tSTOPTIME - tCurTime.GetTime() );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CTFieldRewardResult_Open, "-n", sTimeLeft );

	//!! SF_TODO
	//! 결과 UI를 띄운다.
	//m_pInterface->CTFResultWindowShow();
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldStop(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;

    }

    GLMSG::SNET_CTF_STOP_AC *pPacket = (GLMSG::SNET_CTF_STOP_AC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    if ( sCTFState.IsNULL() )
    {
        GASSERT(!"GLGaeaClient::MsgCaptureTheFieldStop() - sCTFState is null");
        return;
    }

	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.emAUTHSCHOOL = sCTFState.emAUTHSCHOOL;
	sPrevState.bConsecutivelyCapturel = sCTFState.bConsecutivelyCapturel;
	sPrevState.bisConsecutively = sCTFState.bisConsecutively;
	sPrevState.dwConsecutivelyNumber = sCTFState.dwConsecutivelyNumber;

	//! 현재상태를 세팅
	sCTFState.tSTARTTIME   = static_cast<__time64_t>(pPacket->tNextStartTime);
	sCTFState.tSTOPTIME    = sCTFState.tSTARTTIME + sCTFState.tPLAYTIME;
	sCTFState.emSTATE      = CTF_STATE_STOP;
	sCTFState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wSchoolAuthed);
	sCTFState.bisConsecutively = pPacket->bConsecutively;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );

	m_pCaptureTheField->Reset();
	m_pCaptureTheField->ClearResultData();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldWarning4Drop(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_WARNING4DROP_FC *pPacket = (GLMSG::SNET_CTF_WARNING4DROP_FC *)pMsg;

	PrintConsoleTextDlg ( sc::string::format(ID2GAMEINTEXT("CTF_MSG_WARNING4DROP"), pPacket->nRemainMinutes) );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldUpdateQueue(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_UPDATE_QUEUE_AC *pPacket = (GLMSG::SNET_CTF_UPDATE_QUEUE_AC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

	GLCharacter* pchar = GetCharacter();
	if(!pchar)
		return;

    if ( sCTFState.IsCANNOTJOIN(pchar->GetCharSchool()) )
    {
        GASSERT(!"MsgCaptureTheFieldUpdateQueue - CTF state is stop");
        return;
    }

    if ( sCTFState.nQUEUE )
    {
        --sCTFState.nQUEUE;
        //sCTFState.nQUEUE = pPacket->nQueued;
    }

    PrintConsoleTextDlg (  sc::string::format(ID2GAMEINTEXT("CTF_MSG_UPDATE_QUEUED"), sCTFState.nQUEUE) );
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldAuthed(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_CROW_AUTHED *pPacket = (GLMSG::SNET_CTF_CROW_AUTHED *)pMsg;

    GLLandManClient* pLandMan = this->GetActiveMap();


    if ( !pLandMan )
    {
        GASSERT( !"GLGaeaClient::MsgCaptureTheFieldAuthed() - pLandMan is null" );
        return;
    }

    //! 몹에게 상태를 세팅한다.
    //  하지만 몹이 존재하지 않을수도 있다. ( 멀리떨어져 있을때 )
    std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pPacket->dwID);
    if (spCrow && spCrow->m_pCrowData && spCrow->m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
    {
        spCrow->MsgCrowAuthed(pPacket);
    }

    //! 현재 CTF 맵을 가져온다
    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    SAUTHENTICATORINFO& sInfo = sCTFState.FindAuthenticator( SNATIVEID(pPacket->sNativeID) );

    if ( sInfo.IsNull() )
    {
        GASSERT( !"GLGaeaClient::MsgCaptureTheFieldAuthed() - sInfo is null" );
        return;
    }

	// TODO :  #RM 695 수정		
	BOOL bDisplay = ( sInfo.emAuthSchool !=  pPacket->wSchoolAuthed) ? TRUE : FALSE;
		
    sInfo.emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthed);
    sInfo.vPos         = pPacket->dxPos;

	if ( bDisplay )
	{
		// 인증기 갱신;
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_AuthenticatorUpdate);
	}

	// 인증기 정보 갱신;
	GASSERT( sCTFState.vecAUTHENTICATOR.size() <= 3 );
	int nAuthenticatorCount[ SCHOOL_NUM ] = {0,0,0};
	for ( int i = SCHOOL_SM; i < SCHOOL_NUM; ++i )
	{		
		ENUM_SCHOOL emSchool = sCTFState.vecAUTHENTICATOR[i].emAuthSchool;
		if ( emSchool < SCHOOL_NUM && emSchool != SCHOOL_NONE )
			nAuthenticatorCount[ emSchool ]++;
	}
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFAuthCountInit,"-n, -n, -n"
		, nAuthenticatorCount[SCHOOL_SM]
		, nAuthenticatorCount[SCHOOL_HA]
		, nAuthenticatorCount[SCHOOL_BH]);

	int nDisplayType = 0;
	switch ( pPacket->wSchoolAuthed )
	{
	case SCHOOL_SM: nDisplayType = UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM; break;
	case SCHOOL_HA: nDisplayType = UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA; break;
	case SCHOOL_BH: nDisplayType = UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH; break;
	};

	if ( (pPacket->wSchoolAuthed != SCHOOL_NUM) && (pPacket->wSchoolAuthed != SCHOOL_NONE ) && bDisplay )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTFNotifyInfo, "-n", nDisplayType);
	}

//	GLWidgetScript::GetInstance().LuaCallFunc< void >(
//		NSWIDGET_SCRIPT::g_strFunc_CTFAuthedController, "-n, -n, -n",
//		sInfo.sID.Mid(), sInfo.sID.Sid(), sInfo.emAuthSchool);

	// TODO :  #RM 695 수정
	/*
    switch ( pPacket->wSchoolAuthed )
    {
    case SCHOOL_SM: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM, std::string("" ) ); break;
    case SCHOOL_HA: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA, std::string("" ) ); break;
    case SCHOOL_BH: m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH, std::string("" ) ); break;
    };*/

	//!! SF_TODO
	/*switch ( pPacket->wSchoolAuthed )
	{
	case SCHOOL_SM: if( bDisplay ) m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_SM, std::string("" ) ); break;
	case SCHOOL_HA: if( bDisplay ) m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_HA, std::string("" ) ); break;
	case SCHOOL_BH: if( bDisplay ) m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_CTF_CAPTURED_BY_BH, std::string("" ) ); break;
	};
		
	m_pInterface->CheckCTFSpurt();*/
}

//! 해당 서버 유저들에게
void GLGaeaClient::MsgCaptureTheFieldSyncStatus(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_SYNC_STATUS_AC *pPacket = (GLMSG::SNET_CTF_SYNC_STATUS_AC *)pMsg;

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    sCTFState.emSTATE      = ENUM_CTF_STATE(pPacket->state);
    sCTFState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wSchoolAuthed);
	sCTFState.tSTARTTIME   = static_cast<__time64_t>(pPacket->tStartTime);
    sCTFState.tPLAYTIME    = static_cast<__time64_t>(pPacket->nPlayTimeM)*60;
	sCTFState.tEXTRATIME   = static_cast<__time64_t>(pPacket->nRewardTimeM)*60;
	sCTFState.tCYCLETIME   = static_cast<__time64_t>(pPacket->nPlayCycleH);
	sCTFState.tSTOPTIME	   = sCTFState.tSTARTTIME + sCTFState.tPLAYTIME;
	sCTFState.nMAXPLAYERS  = pPacket->dwMaxPlayerNumPerSchool;
	sCTFState.wLEVELLIMIT  = pPacket->wRequiredLevel;
    sCTFState.sPVPMAPID    = pPacket->dwPvPMapID;

    sCTFState.sPVEMAPID    = pPacket->dwPvEMapID;
	
	sCTFState.bisConsecutively = pPacket->bisConsecutively;
	sCTFState.bConsecutivelyCapturel = pPacket->bConsecutivelyCapturel;
	sCTFState.dwConsecutivelyNumber = pPacket->dwConsecutivelyNumber;
	

	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wPrevSchoolAuthed);
	sPrevState.tSTARTTIME	= static_cast<__time64_t>(pPacket->tPrevStartTime);
	sPrevState.tENDTIME		= static_cast<__time64_t>(pPacket->tPrevEndTime);
	sPrevState.bisConsecutively = pPacket->bPrevConsecutively;
	sPrevState.bConsecutivelyCapturel = pPacket->bConsecutivelyCapturel;


	

    //! 인증기 정보 세팅
    sCTFState.vecAUTHENTICATOR.clear();
    {
        SAUTHENTICATORINFO sAuthenticatorInfo1( pPacket->dwCertificationID1 );
        sAuthenticatorInfo1.Init();
        sCTFState.vecAUTHENTICATOR.push_back( sAuthenticatorInfo1 );

        SAUTHENTICATORINFO sAuthenticatorInfo2( pPacket->dwCertificationID2 );
        sAuthenticatorInfo2.Init();
        sCTFState.vecAUTHENTICATOR.push_back( sAuthenticatorInfo2 );

        SAUTHENTICATORINFO sAuthenticatorInfo3( pPacket->dwCertificationID3 );
        sAuthenticatorInfo3.Init();
        sCTFState.vecAUTHENTICATOR.push_back( sAuthenticatorInfo3 ); 
    }

	// 재 접속시 보상 맵에 있을 경우가 있다;
	if ( this->IsCTFMap() )
	{
		int nAuthenticactorSize;
		GASSERT( (nAuthenticactorSize=sCTFState.vecAUTHENTICATOR.size()) <= 3 );
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFDisplayInfo_Open);
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );
	}

	//! 세팅된 정보를 바탕으로 초기화
	sCTFState.INIT( this );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );
}

void GLGaeaClient::MsgCaptureTheFieldUpdatePlayersPosition(NET_MSG_GENERIC* pMsg)
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC *pPacket = (GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC *)pMsg;

    UINT nSize    = m_pCaptureTheField->GetAllianceInfoSize();
	UINT nRelativeUserIndex = pPacket->nRelativeUserIndex;
    UINT nHowMany = pPacket->nHowMany;
    UINT nIndex   = 0;

	D3DXVECTOR3 vMyPos = pPacket->dvMyPos;

    for ( ; (nRelativeUserIndex + nIndex)<nSize; ++nIndex )
    {
        if ( nHowMany <= nIndex )
        {
            break;
        }

//		if ( vMyPos == pPacket->arrPos[nIndex] )
//		{
//			SALLIANCEINFO& sInfo = m_pCaptureTheField->GetAllianceInfo( nRelativeUserIndex+nIndex );
//			sInfo.bMyInfo = true;
//			continue;
//		}

        m_pCaptureTheField->SetAllianceInfo( nRelativeUserIndex + nIndex, SALLIANCEINFO(pPacket->arrPos[nIndex]) );
		if ( vMyPos == pPacket->arrPos[nIndex] )
		{
			SALLIANCEINFO& sInfo = m_pCaptureTheField->GetAllianceInfo( nRelativeUserIndex+nIndex );
			sInfo.bMyInfo = true;
		}
    }

    while ( nIndex < nHowMany )
	{
		if ( vMyPos == pPacket->arrPos[nIndex] )
		{
			SALLIANCEINFO sInfo(pPacket->arrPos[nIndex]);
			sInfo.bMyInfo = true;
			m_pCaptureTheField->AddAllianceInfo(sInfo);
			nIndex++;
			continue;
		}

        m_pCaptureTheField->AddAllianceInfo( SALLIANCEINFO(pPacket->arrPos[nIndex]) );
        nIndex++;
    }

    //! 공간이 더 남아있다면 마지막을 Null로 만든다.
    if ( (nRelativeUserIndex+nIndex) < nSize )
    {
        SALLIANCEINFO& sInfo = m_pCaptureTheField->GetAllianceInfo( nRelativeUserIndex+nIndex );
        sInfo.SetNull();
    }
}

void GLGaeaClient::MsgCaptureTheFieldStatistics(NET_MSG_GENERIC* pMsg)
{
	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return;
	}

	GLMSG::SNET_CTF_STATISTICS_FC *pPacket = (GLMSG::SNET_CTF_STATISTICS_FC *)pMsg;
	static bool bCheckMyData = false;

	if ( pPacket->wSeq == 1 )
	{
		bCheckMyData = false;
		m_pCaptureTheField->ClearResultData();
	}

	GLCharacter* pCharacter = GetCharacter();

	for ( UINT i=0; i<pPacket->nHowMany; ++i )
	{
		CTF_STATISTICS_4_ONEPLAYER& Data = pPacket->data[i];

		SCTFRESULTDATA sResultData;
		sResultData.strNAME		  = Data.szName;
		sResultData.emSCHOOL      = ENUM_SCHOOL(Data.wSchool);
		sResultData.emCLASS	      = CharClassToIndex(EMCHARCLASS(Data.wClass));
		sResultData.nCONTRIBUTION = Data.pvpperf.nContributionPoint;
		sResultData.nDAMAGE	      = Data.pvpperf.nDamageTo;
		sResultData.nHEAL	      = Data.pvpperf.nHealTo;
		sResultData.nKILL	      = Data.pvpperf.nKill;
		sResultData.nDEATH	      = Data.pvpperf.nDeath;
		sResultData.nREBIRTH      = Data.pvpperf.nRebirthTo;

		//! 자신과 이름이 같다면
		if ( !bCheckMyData && pCharacter->GetName() == sResultData.strNAME )
		{
			//! 실제 기여도를 올려준다.
			pCharacter->m_nContributionPoint += sResultData.nCONTRIBUTION;
			bCheckMyData = true;
		}

		m_pCaptureTheField->AddResultData( sResultData );
	}
	
	//!! SF_TODO
	//m_pInterface->CTFResultWindowUpdate();
}

void GLGaeaClient::MsgCaptureTheFieldRewardToAll(NET_MSG_GENERIC* pMsg)
{
	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return;
	}

	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.vecREWARDBUFF.clear();

	GLMSG::SNET_CTF_REWARD_INFO *pPacket = (GLMSG::SNET_CTF_REWARD_INFO *)pMsg;

	for ( UINT i=0; i<pPacket->nHowMany; ++i )
	{
		sPrevState.vecREWARDBUFF.push_back(pPacket->arrBuffs[i]);
	}
}

void GLGaeaClient::MsgCaptureTheFieldRankingToAll(NET_MSG_GENERIC* pMsg)
{
	if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
	{
		return;
	}

	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.vecRESULTDATA.clear();

	GLMSG::SNET_CTF_RANKING_FAC *pPacket = (GLMSG::SNET_CTF_RANKING_FAC *)pMsg;

	for ( UINT i=0; i<pPacket->nHowMany; ++i )
	{
		sPrevState.vecRESULTDATA.push_back(pPacket->arrRanking[i]);
	}
}

void GLGaeaClient::ReqCaptureTheFieldJoin( const PVP::ENUM_CTF_TYPE emCTFType /*! unused 추후에 타입이 나뉘어질수 있습니다*/ )
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( emCTFType );

    if ( sCTFState.bWAITJOIN )
    {
        GASSERT(!"ReqCaptureTheFieldJoin - be already waiting to join");
        return;
    }

    GLMSG::SNET_CTF_JOIN_CF Packet;
    Packet.bJoin = TRUE;
    NETSENDTOFIELD(&Packet);
}

void GLGaeaClient::ReqCaptureTheFieldCancelJoin( const PVP::ENUM_CTF_TYPE emCTFType /*! unused 추후에 타입이 나뉘어질수 있습니다*/ )
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( emCTFType );

    if ( !sCTFState.bWAITJOIN )
	{
		GASSERT(!"ReqCaptureTheFieldJoin - !sCTFState.bWAITJOIN");
        return;
    }

    GLMSG::SNET_CTF_JOIN_CF Packet;
    Packet.bJoin = FALSE;
    NETSENDTOFIELD(&Packet);
}

void GLGaeaClient::ReqCaptureTheFieldQuit()
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

	if ( m_pCaptureTheField->GetProgressCTF() == PVP::ENUM_CTF_TYPE_NULL )
	{
		//GASSERT(!"ReqCaptureTheFieldQuit - m_pCaptureTheField->GetProgressCTF() == ENUM_CTF_TYPE_NULL");
        return;
    }

    GLMSG::SNET_CTF_QUIT_CFA Packet;
    NETSENDTOFIELD(&Packet);

    //! 바로 처리한다
    m_pCaptureTheField->Reset();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFDisplayInfo_Close);
}

const char* GLGaeaClient::GetCTFProgressTime(const bool bCTFStart)
{
	if ( !m_pLandMClient )
		return NULL;

	const GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState(m_pLandMClient->GetBaseMapID());
	if ( sCTFState.IsNULL() )
		return NULL;

	// 진행 중:CTF종료 이후 남은 시간 출력;
	__time64_t tTime(bCTFStart?sCTFState.tSTARTTIME + sCTFState.tPLAYTIME:sCTFState.tSTARTTIME);
	CTime tCurTime( this->GetCurrentTime() );
	//	서버 타임으로 변환( TimeZone 계산 );
	this->GetConvertServerTime( tCurTime );
	CTimeSpan sTimeLeft( tTime - tCurTime.GetTime() );
	if ( sTimeLeft < 0 )
		sTimeLeft = 0;

	std::string strFormat = sc::string::format("%02d : %02d : %02d", sTimeLeft.GetHours(), sTimeLeft.GetMinutes(), sTimeLeft.GetSeconds() );
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetTime, "-s", strFormat.c_str());
	return strFormat.c_str();
}

int GLGaeaClient::GetCTFState()
{
	GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	return (int)sCTFState.emSTATE;
}

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

    //! ������ ���������θ� ������ �Ǿ��ֽ��ϴ�
    //  ���Ŀ� �ٸ��������� �ʿ��� �ÿ� �������ؾ��մϴ�.

    
    //! CTF ���̶�� ���� Warning �޼����� ���ϴ�
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

	//���� ��������.
	

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

    //! ������ ����
    {
        GASSERT( sCTFState.vecAUTHENTICATOR.size() <= 3 );

        sCTFState.vecAUTHENTICATOR[0].vPos = pPacket->dxPos1;
        sCTFState.vecAUTHENTICATOR[1].vPos = pPacket->dxPos2;
        sCTFState.vecAUTHENTICATOR[2].vPos = pPacket->dxPos3;

		sCTFState.vecAUTHENTICATOR[0].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert1);
		sCTFState.vecAUTHENTICATOR[1].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert2);
		sCTFState.vecAUTHENTICATOR[2].emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthedCert3);
    }

	// �ʱ� ������ ���� ����, ��ƿ��� �ʱⰪ ������ �����ָ� ������ �ȵǴ°Ͱ���;
	int nAuthActorSize = 0;
	GASSERT( (nAuthActorSize=sCTFState.vecAUTHENTICATOR.size()) <= 3 );

	// Ű �ٿ� �ð� ����; CTF���� �Է� �ð��� ������ �����;
	if ( m_pCharacter )
	{
		m_pCharacter->m_fKeyDownCheckTime = 0.0f;
		m_pCharacter->m_nAwayGestureLevel = -1;
	}

	m_pCaptureTheField->SetProgressCTF( PVP::ENUM_CTF_TYPE_POWERSTATION );
	m_pCaptureTheField->ClearResultData();

	GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFDisplayInfo_Open);
	// ������ ����;
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );

    PrintConsoleTextDlg ( ID2GAMEINTEXT("CTF_MSG_STARTTOPLAYERS") );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Competition_UpdateCTF );




	// ������ ���� ����;
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

	//! �������� �ƴϸ�
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

	//! ���������� ����
	GLCaptureTheFieldClient::CTFPREVSTATE& sPrevState = m_pCaptureTheField->GetPrevState( PVP::ENUM_CTF_TYPE_POWERSTATION );
	sPrevState.emAUTHSCHOOL = ENUM_SCHOOL(pPacket->wSchoolAuthed);
	sPrevState.tSTARTTIME	= sCTFState.tSTARTTIME;
	sPrevState.tENDTIME		= GetCurrentTime().GetTime();

	sPrevState.bConsecutivelyCapturel = sCTFState.bConsecutivelyCapturel;
	sPrevState.bisConsecutively = sCTFState.bisConsecutively;
	sPrevState.dwConsecutivelyNumber = sCTFState.dwConsecutivelyNumber;

	sCTFState.bisConsecutively = pPacket->bConsecutively;

	//! �����ð��� �ӽ÷� ����
	CTime tCurTime(GetCurrentTime());
	GetConvertServerTime( tCurTime );
	sCTFState.tSTARTTIME = tCurTime.GetTime() + sCTFState.tCYCLETIME + sCTFState.tEXTRATIME;
	sCTFState.tSTOPTIME  = tCurTime.GetTime() + sCTFState.tEXTRATIME;

	
	/*������ ��ҵǴ� ���� 
	ctf turn(Start->Done) ��� ���۽� �ش籸���� ���Ͽ� ������ ����Ǿ� ���� �ʴ� ������ ������ �Ǿ 
	CTF�� �������� �������Ը� ����� ��ȿ�ϵ��� ����*/
	if ( m_pCaptureTheField->GetProgressCTF() == PVP::ENUM_CTF_TYPE_POWERSTATION )
	{
		//! ĳ������ ��� �ൿ�� �����.
		m_pCharacter->ResetAction();
		m_pCharacter->ResetReservationAction();
	

		// Ÿ�� �ʱ�ȭ
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

	//! ĳ������ ��� �ൿ�� �����.
	m_pCharacter->ResetAction();
	m_pCharacter->ResetReservationAction();

	// Ÿ�� �ʱ�ȭ
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
	
	
	//CTField ������ ����â
	CTime tCurTime( GetCurrentTime() );
	CTimeSpan sTimeLeft( sCTFState.tSTOPTIME - tCurTime.GetTime() );
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CTFieldRewardResult_Open, "-n", sTimeLeft );

	//!! SF_TODO
	//! ��� UI�� ����.
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

	//! ������¸� ����
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

    //! ������ ���¸� �����Ѵ�.
    //  ������ ���� �������� �������� �ִ�. ( �ָ������� ������ )
    std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pPacket->dwID);
    if (spCrow && spCrow->m_pCrowData && spCrow->m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
    {
        spCrow->MsgCrowAuthed(pPacket);
    }

    //! ���� CTF ���� �����´�
    GLCaptureTheFieldClient::CTFGLOBALSTATE& sCTFState = m_pCaptureTheField->GetGlobalState( PVP::ENUM_CTF_TYPE_POWERSTATION );

    SAUTHENTICATORINFO& sInfo = sCTFState.FindAuthenticator( SNATIVEID(pPacket->sNativeID) );

    if ( sInfo.IsNull() )
    {
        GASSERT( !"GLGaeaClient::MsgCaptureTheFieldAuthed() - sInfo is null" );
        return;
    }

	// TODO :  #RM 695 ����		
	BOOL bDisplay = ( sInfo.emAuthSchool !=  pPacket->wSchoolAuthed) ? TRUE : FALSE;
		
    sInfo.emAuthSchool = ENUM_SCHOOL(pPacket->wSchoolAuthed);
    sInfo.vPos         = pPacket->dxPos;

	if ( bDisplay )
	{
		// ������ ����;
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_AuthenticatorUpdate);
	}

	// ������ ���� ����;
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

	// TODO :  #RM 695 ����
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

//! �ش� ���� �����鿡��
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


	

    //! ������ ���� ����
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

	// �� ���ӽ� ���� �ʿ� ���� ��찡 �ִ�;
	if ( this->IsCTFMap() )
	{
		int nAuthenticactorSize;
		GASSERT( (nAuthenticactorSize=sCTFState.vecAUTHENTICATOR.size()) <= 3 );
		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFDisplayInfo_Open);
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CTFieldDI_SetState, "-n", sCTFState.emSTATE );
	}

	//! ���õ� ������ �������� �ʱ�ȭ
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

    //! ������ �� �����ִٸ� �������� Null�� �����.
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

		//! �ڽŰ� �̸��� ���ٸ�
		if ( !bCheckMyData && pCharacter->GetName() == sResultData.strNAME )
		{
			//! ���� �⿩���� �÷��ش�.
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

void GLGaeaClient::ReqCaptureTheFieldJoin( const PVP::ENUM_CTF_TYPE emCTFType /*! unused ���Ŀ� Ÿ���� ���������� �ֽ��ϴ�*/ )
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

void GLGaeaClient::ReqCaptureTheFieldCancelJoin( const PVP::ENUM_CTF_TYPE emCTFType /*! unused ���Ŀ� Ÿ���� ���������� �ֽ��ϴ�*/ )
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

    //! �ٷ� ó���Ѵ�
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

	// ���� ��:CTF���� ���� ���� �ð� ���;
	__time64_t tTime(bCTFStart?sCTFState.tSTARTTIME + sCTFState.tPLAYTIME:sCTFState.tSTARTTIME);
	CTime tCurTime( this->GetCurrentTime() );
	//	���� Ÿ������ ��ȯ( TimeZone ��� );
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

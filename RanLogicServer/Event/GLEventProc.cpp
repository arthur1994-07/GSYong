#include "pch.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Event/GLIPEvent.h"

#include "../FieldServer/CharManField.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "./GLEventProc.h"

#include "../../SigmaCore/DebugInclude.h"


GLEventProc::GLEventProc(GLGaeaServer* pServer)
    : m_pGLGaeaServer(pServer)
{
}

GLEventProc::~GLEventProc(void)
{
}

void GLEventProc::MsgEventExp(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	//GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::NET_GM_EVENT_EXP_CAF* pPacket = (GLMSG::NET_GM_EVENT_EXP_CAF*) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if (dwAgentSlot != dwClientID)
    {
        sc::writeLogError("MsgEventExp not agent slot");
		return;
    }

	GLCONST_CHAR::SET_EVENT_EXP_SCALE(pPacket->m_Scale);

    sc::writeLogInfo(
        sc::string::format(
            "MsgEventExp %1%",
            pPacket->m_Scale));
}

void GLEventProc::MsgEventExpEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::NET_GM_EVENT_EXP_END_CAF* pPacket = (GLMSG::NET_GM_EVENT_EXP_END_CAF*) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if (dwAgentSlot != dwClientID)
		return;

	GLCONST_CHAR::SET_EVENT_EXP_SCALE(1.0f);

    sc::writeLogInfo("MsgEventExpEnd scale 1");

	GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(pPacket->dwChaNum);	
	if (pChar)
	{
        // 변경된 값 회신
		GLMSG::SNET_GM_EVENT_EXP_END_FB NetMsgFB;
		NetMsgFB.wFIELDSVR = (WORD) m_pGLGaeaServer->GetFieldSvrID();
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}
}

void GLEventProc::MsgEventItemGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_GM_EVEN_ITEM_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLCONST_CHAR::SET_EVENT_ITEM_SCALE ( pNetMsg->fRATE );

	GLChar *pChar = m_pGLGaeaServer->GetCharByDbNum(pNetMsg->dwChaNum);

	if (pChar)
	{
		GLMSG::SNET_GM_EVEN_ITEM_GEN_FB NetMsgFB;
		NetMsgFB.fRATE = GLCONST_CHAR::fEVENT_ITEM_DROP_SCALE;
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}
}

void GLEventProc::MsgEventItemGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_GM_EVEN_ITEM_GEN_END *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN_END *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLCONST_CHAR::SET_EVENT_ITEM_SCALE ( 1.0f );

	GLChar *pChar = m_pGLGaeaServer->GetCharByDbNum(pNetMsg->dwChaNum);

	if (pChar)
	{
		GLMSG::SNET_GM_EVEN_ITEM_GEN_END_FB NetMsgFB;
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}
}

void GLEventProc::MsgEventGrade( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_GM_EVENT_GRADE *pNetMsg = (GLMSG::SNET_GM_EVENT_GRADE *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLChar *pChar = m_pGLGaeaServer->GetCharByDbNum(pNetMsg->dwChaNum);

	if ( pNetMsg->fRATE > 90.0f || pNetMsg->fRATE < 0.0f )
	{
		if (pChar)
		{
			GLMSG::SNET_GM_EVENT_GRADE_FB NetMsgFB;
			NetMsgFB.fRATE = GLCONST_CHAR::fGRADE_EVENT_RATE;
			m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
		}
		return;
	}

	GLCONST_CHAR::fGRADE_EVENT_RATE = pNetMsg->fRATE;

	if (pChar)
	{
		GLMSG::SNET_GM_EVENT_GRADE_FB NetMsgFB;
		NetMsgFB.fRATE = GLCONST_CHAR::fGRADE_EVENT_RATE;
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}
}

void GLEventProc::MsgEventGradeEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_GM_EVENT_GRADE_END *pNetMsg = (GLMSG::SNET_GM_EVENT_GRADE_END *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLCONST_CHAR::fGRADE_EVENT_RATE = 0.0f;

	GLChar *pChar = m_pGLGaeaServer->GetCharByDbNum(pNetMsg->dwChaNum);

	if (pChar)
	{
		GLMSG::SNET_GM_EVENT_GRADE_END_FB NetMsgFB;
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}
}

void GLEventProc::MsgEventMoneyGen ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_MONEY_GEN *pNetMsg = (GLMSG::SNET_GM_EVEN_MONEY_GEN *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLCONST_CHAR::SET_EVENT_MONEY_SCALE ( pNetMsg->fRATE );    
}

void GLEventProc::MsgEventMoneyGenEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVEN_MONEY_GEN_END *pNetMsg = (GLMSG::SNET_GM_EVEN_MONEY_GEN_END *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if ( dwAgentSlot != dwClientID )
		return;

	GLCONST_CHAR::SET_EVENT_MONEY_SCALE ( 1.0f );    
}

void GLEventProc::MsgEventEx( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVENT_EX *pNetMsg = (GLMSG::SNET_GM_EVENT_EX *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
		return;
	if( !pNetMsg )
		return;
	if( pNetMsg->emType == EMGM_EVENT_NONE )
		return;

	m_pGLGaeaServer->m_sEVENTFACT.SetEVENT( pNetMsg->emType, pNetMsg->wValue );

	GLChar* pCHAR = NULL;
    const CharManField::CLIENTMAP& mapCharid = m_pGLGaeaServer->GetCharIDMap();
	CharManField::CLIENTMAP_CITER iter = mapCharid.begin();
	CharManField::CLIENTMAP_CITER iterEnd = mapCharid.end();
	for ( ; iter != iterEnd; ++iter)
	{
		pCHAR = m_pGLGaeaServer->GetChar(iter->second);
		if (pCHAR)
		    pCHAR->ReceiveEventEx( pNetMsg->emType, pNetMsg->wValue );
	}
}

void GLEventProc::MsgEventExEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_EVENT_EX_END *pNetMsg = (GLMSG::SNET_GM_EVENT_EX_END *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
		return;
	if( !pNetMsg )
		return;
	if( pNetMsg->emType == EMGM_EVENT_NONE )
		return;

	m_pGLGaeaServer->m_sEVENTFACT.ResetEVENT( pNetMsg->emType );

	GLChar* pCHAR = NULL;
	const CharManField::CLIENTMAP& mapCharid = m_pGLGaeaServer->GetCharIDMap();
	CharManField::CLIENTMAP_CITER iter = mapCharid.begin();
	CharManField::CLIENTMAP_CITER iterEnd = mapCharid.end();
	for( ; iter != iterEnd; ++iter)
	{
		pCHAR = m_pGLGaeaServer->GetChar(iter->second);
		if (pCHAR)
		    pCHAR->ReceiveEventEx(pNetMsg->emType, 0);
	}
}

void GLEventProc::MsgLimitEventBegin ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_BEGIN *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
	{
		sc::writeLogWarn(std::string("/limit_event_begin - This msg is not from AgentServer."));
		return;
	}

	if( !pNetMsg )
	{
		sc::writeLogWarn(std::string("/limit_event_begin - This msg is invalid."));
		return;
	}

	SEventState sEventState;

	sEventState.bEventStart = TRUE;
	sEventState.fItemGainRate = pNetMsg->itemGain_Rate;		
	sEventState.fExpGainRate  = pNetMsg->expGain_Rate;			
	sEventState.MinEventLevel = pNetMsg->start_Lv;		
	sEventState.MaxEventLevel = pNetMsg->end_Lv;	


	sEventState.EventPlayTime   = pNetMsg->play_Time * 60;
	sEventState.EventBusterTime = pNetMsg->buster_Time * 60; 		

	m_pGLGaeaServer->m_sEventState = sEventState;

	PGLCHAR pCHAR = NULL;
	const CharManField::CLIENTMAP& mapCharid = m_pGLGaeaServer->GetCharIDMap();
	CharManField::CLIENTMAP_CITER iter = mapCharid.begin();
	CharManField::CLIENTMAP_CITER iterEnd = mapCharid.end();

	for ( ; iter != iterEnd; ++iter)
	{
		pCHAR = m_pGLGaeaServer->GetChar(iter->second);
		if (pCHAR)
		    pCHAR->ReceiveBeginEvent(sEventState);
	}

	sc::writeLogInfo(std::string("/limit_event_begin - The GM command has been processed properly."));
}

void GLEventProc::MsgLimitEventTimeReset ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
		return;
	if( !pNetMsg )
		return;

	GLChar* pCHAR = m_pGLGaeaServer->GetChar(pNetMsg->dwGaeaID);
    if (pCHAR)
	    pCHAR->m_sEventTime.loginTime = pNetMsg->loginTime;

	return;
}

void GLEventProc::MsgLimitEventEnd ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_TIME_RESET *) nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
		return;
	if( !pNetMsg )
		return;

	SEventState sEventState;
	sEventState.bEventStart = FALSE;

	m_pGLGaeaServer->m_sEventState = sEventState;

	GLChar* pCHAR = NULL;
	const CharManField::CLIENTMAP& mapCharid = m_pGLGaeaServer->GetCharIDMap();
	CharManField::CLIENTMAP_CITER iter = mapCharid.begin();
	CharManField::CLIENTMAP_CITER iterEnd = mapCharid.end();

	GLMSG::SNET_GM_LIMIT_EVENT_END_FB NetMsgEventEndFb;
	for ( ; iter != iterEnd; ++iter)
	{
		pCHAR = m_pGLGaeaServer->GetChar(iter->second);
		if (pCHAR)
		    m_pGLGaeaServer->SENDTOCLIENT(pCHAR->ClientSlot(), &NetMsgEventEndFb);
	}
}

void GLEventProc::MsgClassEvent ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_GM_CLASS_EVENT *pNetMsg = (GLMSG::SNET_GM_CLASS_EVENT *)nmg;

	DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

	if( dwClientID != dwAgentSlot )
		return;
	
	if( !pNetMsg )
		return;

	if( pNetMsg->dwClassIndex > GLCC_ALL_ACTOR )
		return;

	bool bClassEvent[GLCI_NUM_ACTOR];
	
	for( BYTE i = 0; i < GLCI_NUM_ACTOR; i++ )
		bClassEvent[i] = FALSE;

	if( pNetMsg->dwClassIndex & GLCC_FIGHTER_M )
		bClassEvent[GLCI_FIGHTER_M] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_ARMS_M )
		bClassEvent[GLCI_ARMS_M] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_ARCHER_W )
		bClassEvent[GLCI_ARCHER_W] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_SPIRIT_W )
		bClassEvent[GLCI_SPIRIT_W] = TRUE;		
	if( pNetMsg->dwClassIndex & GLCC_EXTREME_M )
		bClassEvent[GLCI_EXTREME_M] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_EXTREME_W )
		bClassEvent[GLCI_EXTREME_W] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_FIGHTER_W )
		bClassEvent[GLCI_FIGHTER_W] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_ARMS_W )
		bClassEvent[GLCI_ARMS_W] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_ARCHER_M )
		bClassEvent[GLCI_ARCHER_M] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_SPIRIT_M )
		bClassEvent[GLCI_SPIRIT_M] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_SCIENTIST_M )
		bClassEvent[GLCI_SCIENTIST_M] = TRUE;	
	if( pNetMsg->dwClassIndex & GLCC_SCIENTIST_W )
		bClassEvent[GLCI_SCIENTIST_W] = TRUE;
    if( pNetMsg->dwClassIndex & GLCC_ASSASSIN_M )
		bClassEvent[GLCI_ASSASSIN_M] = TRUE;
    if( pNetMsg->dwClassIndex & GLCC_ASSASSIN_W )
		bClassEvent[GLCI_ASSASSIN_W] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_TRICKER_M )
		bClassEvent[GLCI_TRICKER_M] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_TRICKER_W )
		bClassEvent[GLCI_TRICKER_W] = TRUE;
    if( pNetMsg->dwClassIndex & GLCC_ETC_M ) 
		bClassEvent[GLCI_ETC_M] = TRUE;
    if( pNetMsg->dwClassIndex & GLCC_ETC_W )
		bClassEvent[GLCI_ETC_W] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_ACTOR_M ) 
		bClassEvent[GLCI_ACTOR_M] = TRUE;
	if( pNetMsg->dwClassIndex & GLCC_ACTOR_W )
		bClassEvent[GLCI_ACTOR_W] = TRUE;

	if( pNetMsg->bStart )
	{
		for( int i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			float fEventRate = 1.0f;

			if( bClassEvent[i] == FALSE )
				continue;
				
			fEventRate = pNetMsg->fEventRate;

			if( pNetMsg->dwEventType == 0 )		 
			{
				m_pGLGaeaServer->GetClassEventData().fExpGainRate[i]  = fEventRate;
				m_pGLGaeaServer->GetClassEventData().dwExpMinLevel[i] = pNetMsg->start_Lv;
				m_pGLGaeaServer->GetClassEventData().dwExpMaxLevel[i] = pNetMsg->end_Lv;
			}else if( pNetMsg->dwEventType == 1 )
			{
				m_pGLGaeaServer->GetClassEventData().fItemGainRate[i]  = fEventRate;
				m_pGLGaeaServer->GetClassEventData().dwItemMinLevel[i] = pNetMsg->start_Lv;
				m_pGLGaeaServer->GetClassEventData().dwItemMaxLevel[i] = pNetMsg->end_Lv;
			}else if( pNetMsg->dwEventType == 2 )
			{
				m_pGLGaeaServer->GetClassEventData().fMoneyGainRate[i]  = fEventRate;
				m_pGLGaeaServer->GetClassEventData().dwMoneyMinLevel[i] = pNetMsg->start_Lv;
				m_pGLGaeaServer->GetClassEventData().dwMoneyMaxLevel[i] = pNetMsg->end_Lv;
			}	
		}
	}
	else
	{
		for( int i = 0; i < GLCI_NUM_ACTOR; i++ )
		{
			float fEventRate = 1.0f;

			if( bClassEvent[i] == FALSE )
				continue;

			if( pNetMsg->dwEventType == 0 )		 
			{
				m_pGLGaeaServer->GetClassEventData().InitExpRate(i);
			}else if( pNetMsg->dwEventType == 1 )
			{
				m_pGLGaeaServer->GetClassEventData().InitItemRate(i);
			}else if( pNetMsg->dwEventType == 2 )
			{
				m_pGLGaeaServer->GetClassEventData().InitMoneyRate(i);
			}	
		}
	}

	GLChar *pChar = m_pGLGaeaServer->GetCharByDbNum(pNetMsg->dwChaNum);

	if (pChar)
	{
		GLMSG::SNET_GM_CLASS_EVENT NetMsgFB;
		m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
	}	
}

void GLEventProc::MsgGmSetIPEvent(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::SNETPC_GM_SET_IPEVENT* pNetMsg = (GLMSG::SNETPC_GM_SET_IPEVENT*) nmg;

    DWORD dwAgentSlot = m_pGLGaeaServer->GetAgentSlot();

    if( dwClientID != dwAgentSlot )
        return;

    if( !pNetMsg )
        return;

    m_pGLGaeaServer->m_sEVENTFACT.bIPEvent = pNetMsg->bEnable;

    CheckIPEventCharacters();
}

void GLEventProc::CheckIPEventCharacters()
{
    GLChar* pCHAR = NULL;
    std::string ClinetIP;
    const CharManField::CLIENTMAP& mapCharid = m_pGLGaeaServer->GetCharIDMap();
    CharManField::CLIENTMAP_CITER iter = mapCharid.begin();
    CharManField::CLIENTMAP_CITER iterEnd = mapCharid.end();

    for ( ; iter != iterEnd; ++iter)
    {
        pCHAR = m_pGLGaeaServer->GetChar(iter->second);
        if (pCHAR)
        {
            if ( pCHAR->m_sEVENTFACT.IsIPEvent() )
                pCHAR->ReceiveIPEvent( false ); // 이미 받고 있는 IP 이벤트는 끄자.

            if ( m_pGLGaeaServer->m_sEVENTFACT.bIPEvent )
            {
                std::string& ClinetIP = m_pGLGaeaServer->GetClientIP( pCHAR->GETCLIENTID() );
                if ( ran_event::GLIPEvent::Instance()->IsAbleToEvent( ClinetIP ) )
                {
                    pCHAR->ReceiveIPEvent( true );
                }
            }
        }
    }
}

/*void GLEventProc::MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID, stdext::hash_map<DWORD,DWORD>& mapCHARID )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GM_EVENT_EXP:					MsgEventExp ( nmg, dwClientID, dwGaeaID );						break;
	case NET_MSG_GM_EVENT_EXP_END:				MsgEventExpEnd ( nmg, dwClientID, dwGaeaID  );					break;
	case NET_MSG_GM_EVENT_ITEM_GEN:				MsgEventItemGen ( nmg, dwClientID, dwGaeaID  );					break;
	case NET_MSG_GM_EVENT_ITEM_GEN_END:			MsgEventItemGenEnd ( nmg, dwClientID, dwGaeaID  );				break;
	case NET_MSG_GM_EVENT_GRADE:				MsgEventGrade ( nmg, dwClientID, dwGaeaID  );					break;
	case NET_MSG_GM_EVENT_GRADE_END:			MsgEventGradeEnd ( nmg, dwClientID, dwGaeaID  );				break;
	case NET_MSG_GM_EVENT_MONEY_GEN:			MsgEventMoneyGen ( nmg, dwClientID, dwGaeaID  );				break;
	case NET_MSG_GM_EVENT_MONEY_GEN_END:		MsgEventMoneyGenEnd ( nmg, dwClientID, dwGaeaID  );				break;
	case NET_MSG_GM_EVENT_EX:					MsgEventEx ( nmg, dwClientID, dwGaeaID );						break;
	case NET_MSG_GM_EVENT_EX_END:				MsgEventExEnd( nmg, dwClientID, dwGaeaID );						break;
	case NET_MSG_GM_LIMIT_EVENT_BEGIN:			MsgLimitEventBegin( nmg, dwClientID, dwGaeaID );				break;
	case NET_MSG_GM_LIMIT_EVENT_TIME_RESET:		MsgLimitEventTimeReset( nmg, dwClientID, dwGaeaID );			break;
	case NET_MSG_GM_LIMIT_EVENT_END:			MsgLimitEventEnd( nmg, dwClientID, dwGaeaID );					break;
	case NET_MSG_GM_CLASS_EVENT:				MsgClassEvent( nmg, dwClientID, dwGaeaID );						break;
	default: break;
	};
}*/

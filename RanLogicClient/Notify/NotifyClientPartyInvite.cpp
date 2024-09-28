#include "pch.h"
#include "./NotifyClientPartyInvite.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/json_spirit_reader.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/DebugInclude.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/GLContrlPartyMsg.h"

#include "../GLGaeaClient.h"
    
NotifyClientPartyInvite::NotifyClientPartyInvite(
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date )
    : INotifyClientBase( pGaeaClient, Guid, Date, gamelogic::notify::PARTY_INVITE )
{
}

NotifyClientPartyInvite::~NotifyClientPartyInvite()
{
}

bool NotifyClientPartyInvite::Parsing( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME* pNetMsg = (GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME*) nmg;

    if( pNetMsg != NULL )
    {
        m_strMasterName = stringformat( "%1%", pNetMsg->szMasterName );
        
        m_PartyID = pNetMsg->partyID;
		m_dwHashKey = pNetMsg->dwHashKey;
        m_dwMasterID = pNetMsg->dwMasterID;
		m_dwTimeKey = pNetMsg->dwTimeKey;
		m_sOption = pNetMsg->partyOption;

        return true;
    }

    return false;
}

std::string NotifyClientPartyInvite::Description() const
{
    return m_strMasterName;
}

std::string NotifyClientPartyInvite::GetMessageBoxText() const
{   
    std::string _messageInviteParty = sc::string::format( 
        "%5% \r\n[%1%:%2%][%3%:%4%]", 
        ID2GAMEWORD( "PARTY_MODAL_CONDITION", 0 ), \
        ID2GAMEWORD( "PARTY_MODAL_ITEM_OPTION", m_sOption.GetItemOption() ),
        ID2GAMEWORD( "PARTY_MODAL_CONDITION", 1 ), 
        ID2GAMEWORD( "PARTY_MODAL_MONEY_OPTION", m_sOption.GetMoneyOption() ),
        sc::string::format(ID2GAMEINTEXT("PARTY_TAR"), m_strMasterName.c_str())
        );
    
    return _messageInviteParty;
}

void NotifyClientPartyInvite::SendAccept()
{
    //	파티 수락 처리
    GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( 
        EMPARTY_LURE_OK,
        m_PartyID, 
		m_dwHashKey,
        m_dwMasterID,
		m_dwTimeKey,
        m_sOption );
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void NotifyClientPartyInvite::SendRefused()
{
    //	파티 거절 처리
    GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( 
        EMPARTY_LURE_REFUSE,
        m_PartyID, 
		0,
        m_dwMasterID,
		0,
        SPartyOption() );

    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

#pragma once

#include "FactionManagerFieldDefine.h"

namespace Faction
{
    class ManagerField : public Faction::Manager
    {
    public:
        BOOL CALLBACK messageProcedure(const DWORD dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage);

        const EMRELATION_TYPE getRelationField(ServerActor* const pActor, ServerActor* const pTargetActor);
		const bool IsFactionMember ( const FactionID& factionID, ServerActor* const pActor );

        void sendFactionInformation(GLChar* const pChar) const;
        void sendRelationInformation(GLChar* const pChar) const;

        void sendFactionInformation() const;
        void sendRelationInformation() const;

		// 특정 Actor가 같은 Faction인 특정 Actor에게 메시지를 보낸다;
		const bool SendToFactionPrivate ( const Faction::GLActorID& sActorID, const Faction::GLActorID& sTargetActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );
		const bool SendToAllyFactionPrivate ( const Faction::GLActorID& sActorID, const Faction::GLActorID& sTargetActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );

		// 특정 Actor의 ViewAround 내의 같은 Faction 전원에게 메시지를 보낸다;
		const bool SendToFactionViewAround ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionViewAround ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// 특정 Actor( 일반적으로 나 )를 제외한 같은 Faction 전원에게 메시지를 보낸다;
		const bool SendToFactionExcludeMe ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );
		const bool SendToAllyFactionExcludeMe ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );

		// 특정 Acotr와 같은 파티이면서 같은 Faction 전원에게 메시지를 보낸다;
		const bool SendToFactionParty ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionParty ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// 특정 Acotr와 같은 클럽이면서 같은 Faction 전원에게 메시지를 보낸다;
		const bool SendToFactionClub ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionClub ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// 같은 Faction 전원에게 메시지를 보낸다;
        const bool sendMessageToFaction(const FactionID& factionID, NET_MSG_GENERIC* nmg);
		const bool sendMessageToAllyFaction(const FactionID& factionID, NET_MSG_GENERIC* nmg);
        const bool sendMessageToFaction(const FactionID& factionID, const EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, const bool bUserCrc = false);
		const bool sendMessageToAllyFaction(const FactionID& factionID, const EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, const bool bUserCrc = false);

    public:
        ManagerField(GLGaeaServer* const _pGaeaServer, GLLandManager* const _pLandMananger);
        virtual ~ManagerField();

	private:
		GLGaeaServer* const m_pGaeaServer;
        GLLandManager* const m_pLandManager;
    };

    
};

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

		// Ư�� Actor�� ���� Faction�� Ư�� Actor���� �޽����� ������;
		const bool SendToFactionPrivate ( const Faction::GLActorID& sActorID, const Faction::GLActorID& sTargetActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );
		const bool SendToAllyFactionPrivate ( const Faction::GLActorID& sActorID, const Faction::GLActorID& sTargetActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );

		// Ư�� Actor�� ViewAround ���� ���� Faction �������� �޽����� ������;
		const bool SendToFactionViewAround ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionViewAround ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// Ư�� Actor( �Ϲ������� �� )�� ������ ���� Faction �������� �޽����� ������;
		const bool SendToFactionExcludeMe ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );
		const bool SendToAllyFactionExcludeMe ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg );

		// Ư�� Acotr�� ���� ��Ƽ�̸鼭 ���� Faction �������� �޽����� ������;
		const bool SendToFactionParty ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionParty ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// Ư�� Acotr�� ���� Ŭ���̸鼭 ���� Faction �������� �޽����� ������;
		const bool SendToFactionClub ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );
		const bool SendToAllyFactionClub ( const Faction::GLActorID& sActorID, const FactionID& factionID, NET_MSG_GENERIC* nmg, bool bExcludeMe = false );

		// ���� Faction �������� �޽����� ������;
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

#pragma once

#include "FactionManagerClientDefine.h"

struct NET_MSG_GENERIC;

class GLGaeaClient;
class ClientActor;

namespace Faction
{
    class ManagerClient : public Faction::Manager
    {
    public:
        void CALLBACK messageProcedure(const NET_MSG_GENERIC* const _pMessage);

        void SetMapID(const unsigned int _nMapID);
        FactionID getMyFaction();
        const EMRELATION_TYPE getRelationClient(ClientActor* const pActor, ClientActor* const pTargetActor);

        void ReqFactionInfo();

    private:
        void CALLBACK _messageFactionInformation(const NET_MSG_GENERIC* const _pMessage);
        void CALLBACK _messageFactionChange(const NET_MSG_GENERIC* const _pMessage);
        void CALLBACK _messageRelationInformation(const NET_MSG_GENERIC* const _pMessage);
        void CALLBACK _messageRelationChange(const NET_MSG_GENERIC* const _pMessage);
        void CALLBACK _messageHpVisible(const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool	SetFactionClient ( const GLActorID& _actorID, const FactionID& _factionID );
		void		CreateFactionClient ( const FactionID& _factionID );
		const bool	SetRelationClient ( const FactionID& _factionID, const FactionID& _targetFactionID, const EMRELATION_TYPE _emRelation );
		const bool	SetRelationToAllClient ( const FactionID& _factionID, const EMRELATION_TYPE _emRelation );
		const bool	SetRelationFromAllClient ( const FactionID& _factionID, const EMRELATION_TYPE _emRelation );

	public:
		inline void	SetLandName ( const std::string& _strLandName ) { m_strLandName = _strLandName; }
		inline void SetShowFactionInfo ( ) { m_bFactionInfo = !m_bFactionInfo; }

    public:
        ManagerClient(GLGaeaClient* const _pGaeaClient);
        virtual ~ManagerClient();

	private:
		GLGaeaClient* const m_pGaeaClient;
        unsigned int nMapID;

		std::string m_strLandName;

		bool m_bFactionInfo;
    };
    
};
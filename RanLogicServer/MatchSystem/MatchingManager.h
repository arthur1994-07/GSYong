#pragma once

#include <vector>
#include <string>


#include "../../RanLogic/MatchSystem/GLCallbackTrigger.h"
#include "../../RanLogic/MatchSystem/MatchModel.h"
#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"
#include "../../RanLogic/Tournament/GLTournamentInfoDef.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"


#include "../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogicServer/Character/GLChar.h"

#include "../TournamentManager/GLMatchingScriptManager.h"
#include "../AgentServer/GLAgentServer.h"
#include "../FieldServer/GLGaeaServer.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"
#include "./LobbyManagerBase.h"
#include "./MatchingDefine.h"
#include "MatchingManager.h"

#include "MatchingCallbackTriggerDefine.h"
#include "./Tournament/GLTournamentBetting.h"








namespace MatchingSystem
{

	class GLTournamentBetting;
	class LobbyFieldManagerBase;

	extern GLAgentServer* gpAgentServer;
	extern GLGaeaServer* gpGaeaServer;

	//! ��Ī�� �����ϴ� �ھ� ���� (���� �ϳ��ۿ� ���� ���ϹǷ� �̱���)
	class MatchingCore
	{
	public:

		MatchingCore(GLAgentServer* const _pAgentServer);

		MatchingCore(GLGaeaServer* const _pGaeaServer);

		~MatchingCore();

	public: 
		static MatchingCore* getinstance() { return MatchingCore::m_instance; }

		static InstanceSystem::ManagerAgent* getInstanceManager(void) 
		{ return gpAgentServer->getInstanceManager(); }

	private:
		static MatchingCore* m_instance;

		// ���� ���
	public:

		void ForcCancel(DWORD dwDBNum );
		
		bool OnLoad(); // ��ũ��Ʈ�� �о� ���� Matching Land �� ����� ����.

		bool OnReload(); // ��ũ��Ʈ �缳��

		bool OnReload(const DWORD MatchNum); // Ư�� ��Ī ��ũ��Ʈ �缳��;

		void OnFrameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime);

		void SetFileName(std::string path) { m_FileName = path; } // ��ũ��Ʈ �̸� ����

		//��-����
		bool SetcopyScript(CopyTrigger* rvalue); // ��ũ��Ʈ �����Ͽ� ���ο� ��ʸ�Ʈ �������Ҷ� ���
		//����
		sc::LuaInstance* SetcopyScript(DWORD id);

		ScriptManager* GetScriptManager(){ return pmananger; }

		bool MatchingInfoamtionSend(DWORD ClietDBID);
		void UserMatchingStatusUpdate(DWORD ClientDBID, DWORD ScriptID,EMCUSTOMER_STATUS_TYPE GameStatus);
		void SendClientUserMatchingInfo(DWORD ClientDBID,DWORD ScriptID);

		void DropOutChar(DWORD ClietDBID);

	private:
		std::string GetFileName() { return m_FileName; }


	public: // ���� �������� ȣ�� �� ���ɼ��ִ� ���

		// ��Ī �ý��ۿ� Ŭ���̾�Ʈ ����
		bool JoinMatchingSystem(DWORD MatchNum,DWORD ClientDbid);
		bool OutMatchingSystem(DWORD MatchNum,DWORD ClientDbid);
		bool OutMatchingSystem(DWORD MatchNum);
		bool IsAreadyJoin(DWORD ClientDbid);
        EMCUSTOMER_STATUS_TYPE GetJoinUserState(DWORD MatchNum, DWORD ClientDbid);

		// �޽� �ý��ۿ� ���� ���
		bool RegistMatchingSystem(const std::string InstanceName);

		// GM������� ���� ���� ����
		bool GM_ForceStart(DWORD MatchNum);


		MatchingGameBase* getmatchingLand(DWORD matchingNumber);

		const bool messageProcedureAgent(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool messageProcedureField(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
        const bool messageProcedureMsgPack(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);

		const bool _messageResaultStart(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageResaultEnd(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageResaultScore(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageResaultFactionResualt(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
        const bool _messageResultCustomMsg(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageResaultError(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageJoinMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);		
		const bool _messageObserveMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageMatchedReqInfomation( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageGMCommand( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageAnswerRUOK(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageGroupTableInfpReq(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMoneyFeeCheckANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageTournamentMoneyFeeSpendANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageTournamentMoneyFeeCheckANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMoneyFeeSpendANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingGroupEployeeInfoANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMyBettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingGroupInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingBettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingApply(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
        const bool _messageCompetitionRatingGetInfoDF(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageCanselMatchedGame(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageFieldMatchingInfoUpdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageCanselBetting(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
        const bool _messageRoomCustomMessage(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageObserveOut( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID) ;


		/////////////////////////////////////////////////////////////////////////////
		///// �ʵ忡�� ó���Ǵ� ���ν����� �ؿ� ǥ���Ͽ� �򰥸��� �ʰ� �����Ѵ�./////
		/////////////////////////////////////////////////////////////////////////////
		// agent�� field�� ������ �ϴ� ���� �ٸ��� ������ �ִ� ������ ���� ���� �ٸ���,
		// �̷������� ���� ���� �� �ִ� ������ ����;
		// �������� ��������, ���� ���� ������ �޶� data�� �ŷڵ��� ��������,
		// agent/field �� ��ʿ��� � data�� ��� �Ǹ�, �������� ��� �Ǵ� data�� ��������,
		// code�� ������ ���������� ���� �� �� ����;
		// ��� ���� �ʴ� data�� ������ �����ν� �ӵ� ���� ��������;
		const bool _messageJoinFieldMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMoneyFeeCheck(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageTournamentMoneyFeeSpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageTournamentMoneyFeeCheck(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMoneyFeeSpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);		

		const bool _messageBettingMoneyINCOMESpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		const bool _messageBettingMoneyRestore( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID );
		const bool _messageSendPostWinMoneyFC(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);
		///////////////////////////////////////////////////////////////////////////////

		void sendMessageFieldToAgent( NET_MSG_GENERIC* _MessageBox );

		void _sendMessageToClient( const DWORD dbnum, NET_MSG_GENERIC* _MessageBox );
		void _sendMessageToALLClient( NET_MSG_GENERIC* _MessageBox );

		void _sendMessageAgentToALLField( NET_MSG_GENERIC* _MessageBox );

        CompetitionRatingManager* getRatingManager() const {return m_pRatingManager;};

        void UpdateCompetitionSeason(const std::string& _strContentType, const int _nSeason, const int _nBaseRating);

		void SetPastBettingUser( DWORD outChaDbNum, LONGLONG outBettingCount );
		void GetPastBettingUser( DWORD ChaDbNum , DWORD BettingStatus );

		const bool _messageRestoreBetting( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID );

        const bool _messageCaptureTheFlagResult(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID);


		void OnForceStart( DWORD Param1 );

		void PartyMemberChaged(DWORD dwGaeaID,DWORD ParyID );
		void PartyMemberRemoved(DWORD dwGaeaID,DWORD ParyID);
		void PartyMasterChaged(DWORD olddwGaeaID,DWORD NewdwGaeaID,DWORD ParyID);
		void PartyDestroyed(DWORD ParyID);
		

	private:
		VEC_P_COPYCOMMAND_LIST m_MatchingCopyCommand;

		VEC_P_MATCHING_LAND_LIST m_matchingLandlist;

		//ClientCoreUserInfoMAP m_MatchingUserInfo;

		//CDM ���鶧 �ʿ��ϴ� �ϼż� ����� ����
		//��Ȱ�� ��ü ���� ���¸� Ȯ�� ��
		//�׷쿡�� ������ �Ǿ�����, ���忡�� ������ �Ǵ� ���
		//�ǵ������� ��ũ��Ʈ���� ó���ϴ°��� �ٶ�����
		ClientCoreMAP m_ClientParticipate; ///< ��Ī ���� ��.

		// ��ũ��Ʈ ������
		ScriptManager* pmananger;

		std::string m_FileName;


        CompetitionRatingManager* m_pRatingManager; // ���� ���� �ý��� �Ŵ���;

		InstanceSystem::PacketCounter packetCounter;
	};

	class JoinData 
	{
	public:
		DWORD InstanceMapID;
		DWORD MatchingInstacneScriptID;	// MatchingInstanceID;
		DWORD dwMatchingBaseID;
		std::vector<DWORD> m_VecGroupID;

		std::vector<DWORD> m_VecClientID;

	};

	//MatchingInstancCommon

	class LobbyFieldManagerBase
	{
	public:
		LobbyFieldManagerBase(MatchingGameBase* parent,sc::LuaInstance* script);
		~LobbyFieldManagerBase();

		void TournamentRequestJoin( DWORD ClietDBID );

		void FieldMatchingStatusChaged(DWORD status);

		DWORD GetMatchingStatus();

		void SendPostWinMoneyFC( DWORD dwDbNum, DWORD dwKang, LONGLONG winMoney, EM_MATCHING_POST_TYPE emPostType ) ;

		void Serverlog(const char* message);

		MATCHING_CONTENTS_STATUS m_Status;

		float m_fElpaseTime;

		MatchingGameBase* m_parent;
		sc::LuaInstance* m_script;
	};

	class RoomManagerBase
	{
	public:

		RoomManagerBase(MatchingGameBase* parent,sc::LuaInstance* script);
		~RoomManagerBase();

		void Initialize();

		//void OnFrameMove(float fElpaseTime);

		void CreateNewField(RoomTrigger* _ptrigger); ///<  RoomTrigger ���� ����

		void CreateNewField(RoomMultyTrigger* _ptrigger); ///<  RoomMultyTrigger ���� ����

		int  CreateComplete(RoomReceipt* _RoomReceipt);

		int  DestroyComplete(DestroyReceipt* _RoomReceipt,bool NotOut = true);

		int	 JoinComplete(DWORD GroupID, DWORD _instanceMapID);

		int  FinishAllGroup(DWORD _instanceMapID);

		int	 ObserveJoinComplete(JoinReceipt* _RoomReceipt);


		bool RequestObserve(DWORD GroupID, DWORD ReQuester,bool UseCashItem);

		bool NoticeTournamentinfo(DWORD ClietDBID);

		void MatchingInfo(DWORD ClietDBID);

		void Refresh(DWORD _status);
		const bool SetStatus(DWORD _status);
		MATCHING_CONTENTS_STATUS GetStatus() { return m_GLMatching.m_ContentsStatus; }

        const GLMatchingInfo& GetMatchingInfo()const { return m_GLMatching; };

		void OnFrameMove(float fElpaseTime);

        void EventCustomMessage(DWORD _dwClientDBNum, DWORD _dwParam1, DWORD _dwParam2);

		void OutObserve( const DWORD RequestID, const InstanceSystem::InstanceMapID TornanmentMapID );

		bool IsMyMap( const InstanceSystem::InstanceMapID TornanmentMapID , DWORD MatchingInstanceID );

		DWORD GetChaNumOnGroup( const InstanceSystem::InstanceMapID nTornanmentMapID, DWORD nMatchingInstanceID, DWORD nFactionID );

		bool HasMyMap( DWORD MatchingInstanceID );

	public:
		const DWORD GetInstanceMapID_JoinGame ( const DWORD _dwChaDBNum );
		const DWORD GetMatchingBaseID_JoinGame ( const DWORD _dwChaDBNum );
		const DWORD GetMatchingInstanceID_JoinGame ( const DWORD _dwChaDBNum );

		const bool CanRejoin ( const DWORD _dwChaDBNum );

	public:
		GLMatchingInfo *GetGLMatchingInfo() { return &m_GLMatching; }
		const DWORD GetJoinMaxCount () const { return m_GLMatching.m_dwMaxJoinCount; }

	protected:
	private:

		float m_fElpaseTime;

		MatchingGameBase* m_parent;
		sc::LuaInstance* m_script;

		GLMatchingInfo m_GLMatching;

    public:
		std::vector<JoinData*> m_vecInstanceMapids;
    private:
		std::vector<RoomMultyTrigger*> m_FaildTrigger;
		float m_RetryTime ;

		int MatchingOpenIndex;
/*
		std::string m_MapInfo;
		std::string m_MapInfoCount;
		std::string m_MapReward;
		std::string m_MapRewardCount;
		std::string m_MapName;

		TOURNAMENT_TYPE		m_MapType;
		MATCHING_CONTENTS_STATUS	m_MapStatus;
*/

		
	};


	struct KeyPair 
	{
		int GroupID;
		int Index;
	};

    class BettingManagerbase
    {
    public:
        BettingManagerbase( MatchingGameBase* parent );

    };

    struct CompetitionRatingInfo
    {
        int Rating;
        int WinningStreak;
        int PlayCount;
        __time64_t LastPlayDate;
    };

    typedef std::pair<DWORD, std::string> RATING_MAP_KEY_PAIR;
    typedef std::map<RATING_MAP_KEY_PAIR, CompetitionRatingInfo> RATING_MAP;
    typedef RATING_MAP::iterator RATING_MAP_ITER;

	class CompetitionRatingManager
	{
        // ���� ������;
        RATING_MAP m_ratingMap;

	public:
		CompetitionRatingManager();
        virtual ~CompetitionRatingManager();

        CompetitionRatingInfo* GetRatingInfo(const DWORD dwDbNum, const std::string& strContentType);

        int LoadRatingInfo(const DWORD dwCharDbNum, const std::string& strContentType, const DWORD dwScriptID);
        int LoadRatingInfoFB(const NET_MSG_GENERIC* netMsg);
        int SaveRatingInfo(); // DB�� ���� Rating������ �����Ѵ�;
	};
}

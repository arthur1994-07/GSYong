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

	//! 메칭을 관리하는 코어 관자 (핵은 하나밖에 존재 안하므로 싱글톤)
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

		// 내부 기능
	public:

		void ForcCancel(DWORD dwDBNum );
		
		bool OnLoad(); // 스크립트를 읽어 내어 Matching Land 를 만들어 낸다.

		bool OnReload(); // 스크립트 재설정

		bool OnReload(const DWORD MatchNum); // 특정 매칭 스크립트 재설정;

		void OnFrameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime);

		void SetFileName(std::string path) { m_FileName = path; } // 스크립트 이름 설정

		//비-동기
		bool SetcopyScript(CopyTrigger* rvalue); // 스크립트 복사하여 새로운 토너먼트 만들어야할때 사용
		//동기
		sc::LuaInstance* SetcopyScript(DWORD id);

		ScriptManager* GetScriptManager(){ return pmananger; }

		bool MatchingInfoamtionSend(DWORD ClietDBID);
		void UserMatchingStatusUpdate(DWORD ClientDBID, DWORD ScriptID,EMCUSTOMER_STATUS_TYPE GameStatus);
		void SendClientUserMatchingInfo(DWORD ClientDBID,DWORD ScriptID);

		void DropOutChar(DWORD ClietDBID);

	private:
		std::string GetFileName() { return m_FileName; }


	public: // 서버 로직에서 호출 할 가능성있는 명령

		// 메칭 시스템에 클라이언트 입장
		bool JoinMatchingSystem(DWORD MatchNum,DWORD ClientDbid);
		bool OutMatchingSystem(DWORD MatchNum,DWORD ClientDbid);
		bool OutMatchingSystem(DWORD MatchNum);
		bool IsAreadyJoin(DWORD ClientDbid);
        EMCUSTOMER_STATUS_TYPE GetJoinUserState(DWORD MatchNum, DWORD ClientDbid);

		// 메싱 시스템에 던전 등록
		bool RegistMatchingSystem(const std::string InstanceName);

		// GM명령으로 인한 강제 시작
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
		///// 필드에서 처리되는 프로시져는 밑에 표기하여 헷갈리지 않게 주의한다./////
		/////////////////////////////////////////////////////////////////////////////
		// agent와 field가 엄연히 하는 일이 다르고 가지고 있는 정보의 종류 역시 다른데,
		// 이런식으로 만들어서 얻을 수 있는 이점은 없다;
		// 가독성도 떨어지고, 서로 가진 정보가 달라 data의 신뢰도도 떨어지며,
		// agent/field 중 어떤쪽에서 어떤 data가 사용 되며, 공용으로 사용 되는 data는 무엇인지,
		// code만 봐서는 직관적으로 구분 할 수 없다;
		// 사용 하지 않는 data를 가지게 됨으로써 속도 역시 느려진다;
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

		//CDM 만들때 필요하다 하셔서 만들어 놓음
		//역활은 전체 입장 상태를 확인 및
		//그룹에서 삭제는 되었지만, 입장에서 막혀야 되는 경우
		//되도록으면 스크립트에서 처리하는것이 바람직함
		ClientCoreMAP m_ClientParticipate; ///< 메칭 유저 맵.

		// 스크립트 관리자
		ScriptManager* pmananger;

		std::string m_FileName;


        CompetitionRatingManager* m_pRatingManager; // 전장 점수 시스템 매니져;

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

		void CreateNewField(RoomTrigger* _ptrigger); ///<  RoomTrigger 권장 지원

		void CreateNewField(RoomMultyTrigger* _ptrigger); ///<  RoomMultyTrigger 권장 지원

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
        // 전장 레이팅;
        RATING_MAP m_ratingMap;

	public:
		CompetitionRatingManager();
        virtual ~CompetitionRatingManager();

        CompetitionRatingInfo* GetRatingInfo(const DWORD dwDbNum, const std::string& strContentType);

        int LoadRatingInfo(const DWORD dwCharDbNum, const std::string& strContentType, const DWORD dwScriptID);
        int LoadRatingInfoFB(const NET_MSG_GENERIC* netMsg);
        int SaveRatingInfo(); // DB에 현재 Rating정보를 저장한다;
	};
}

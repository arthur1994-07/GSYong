#pragma once


#include <vector>
class GLGaeaClient;

struct NET_MSG_GENERIC;

#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Tournament/GLTournamentInfoDef.h"
#include "../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"


/**
	PVE 관련 패킷 및 클라이언트 로직을 처리 한다.
    
 */
namespace MatchingSystem
{
	


	class GLTouranmentClient
	{
	public:
		typedef std::vector<GLMatchingInfo*> VEC_MATCHINFO;
		typedef VEC_MATCHINFO::iterator VEC_MATCHINFO_ITOR;


		friend class GLGaeaClient;
	public:
		GLTouranmentClient(GLGaeaClient* pGaeaClent);
		virtual ~GLTouranmentClient();

		void Init();
		void FrameMove(float fElapsedTime);
		
		//토너먼트 정보 신청 기능.
		void GetTournamentInfomation();
		//토너먼트 정보 신청 기능.
		void JoinTournament(DWORD ScriptID);
		void CanselJoinTournament();

		void CanselBetting(DWORD LeageId);

		void ObserveTournament(DWORD ScriptID, const CString& _nameCharacterTargetObserve);
//		void ObserveTournament(DWORD ScriptID,DWORD GroupID);
		void OutObserveTournament();

		void AskJoinAnswer(bool _isok);

		void JoinBetting(DWORD bettingGroup,LONGLONG BettingCount,DWORD LeageId);
		void SetBettingMoney(LONGLONG BettingCount);
		
		void GetMyBettingInformation();
		void GetBettingGroupInfo();
		void GetBettingInfoEployee(DWORD GroupNum);

		void GetBettingInfo(DWORD LeageId);

		bool GetPageupdate(){ return m_Pageupdate; }
		void SetPageupdate(bool _update){ m_Pageupdate = _update; }

		bool GetDashboardupdate(){ return m_Dashboardupdate; }
		void SetDashboardupdate(bool _update){ m_Dashboardupdate = _update; }

		bool GetTOTOupdate(){ return m_TOTOupdate; }
		void SetTOTOupdate(bool _update){ m_TOTOupdate = _update; }

		bool GetMoreInfoupdate(){ return m_MoreInfoupdate; }
		void SetMoreInfoupdate(bool _update){ m_MoreInfoupdate = _update; }


		bool GetGroupInfoOutofDate(){ return m_GroupInfoOutofDate; }
		void SetGroupInfoOutofDate(bool _update){ m_GroupInfoOutofDate = _update; }

		TournamentObserveType GetTournamentType() {return (TournamentObserveType)m_dwObserveType ; }

		TOURNAMENT_MY_ROLE_TYPE GetTournamentViewType() {return (TOURNAMENT_MY_ROLE_TYPE)m_ClientType ; }
		bool GetIsTournamentView() {return m_ClientType!=MatchingSystem::TOURNAMENT_OUT ; }

		DWORD GetAskedMatchingInstance() { return m_AskedMatchingInstance; }

        void SendCustomMessage(DWORD _dwScriptID, DWORD _dwParam1, DWORD _dwParam2);



		VEC_MATCHINFO			m_VecMatchingInfo;

		MatchingSystem::GLMatChingGroupInfo* GetBettingNormalInfo(int _Number,DWORD ScriptID);
		MatchingSystem::BettingRatio* GetBettingTable(DWORD GroupID,DWORD ScriptID);
		MatchingSystem::GLMatChingGroupInfo* GetGroupInfo(DWORD GroupID,DWORD ScriptID);
		MatchingSystem::GLMatChingGroupInfo* GetGroupInfobyIndex(DWORD index,DWORD ScriptID);

		std::vector<MatchingSystem::Bettinginfo*> m_MyBettingNormalInfo;


		std::vector<MatchingSystem::GroupInforEployee*> m_MyBettingEployeeInfo;

		std::vector<MatchingSystem::TournamentGameInfo*> m_TournamentInfo;

		std::vector<MatchingSystem::MatchingClientInfo*> m_ClientMatchingStatus;

		MatchingSystem::MatchingClientInfo m_ClientMatchingStatus_Select;

		MatchingSystem::TournamentGameInfo* GetNowTeamInfo();

		void SetInstancePopupEvent(InstanceSystem::InstancePopUpTrigger* eventHandler);
		void InvokeInstancePopupCloseEvent();
		void HideLeftTOPControlGroup();
		void UpdateDashboard(DWORD matchingID);

		const bool CALLBACK OnMsgProcedure ( const GLMSG::NET_MATCHING_MESSAGE_HEADER* _pMsg );

		const bool CALLBACK _messageProcedure(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messagePackProcedure(const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool CALLBACK _MsgUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg );
		
		const bool CALLBACK _messageProcedure_error(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_admission(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_cancel(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_messagebox(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Notify_at_first(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedure_info_update(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_join_ask(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Matching_Group_Start(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedure_Matching_Group_Add(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedure_Matching_Group_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_Groupinfo(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_Group_More_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_Answer(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_BettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Betting_Group_Eployee_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_ObserveStart(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageSetPVEStageState(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);

		const bool CALLBACK _messageProcedure_Tournament_in_Tournament(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Out_Tournament(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_GameInformation_Role(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_GameInformation_Team_Info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Win(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Lose(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_timelimit(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_faction_info(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_game_popup(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_ClientINFO(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Refrash(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Outofdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_Tournament_Counting(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
		const bool CALLBACK _messageProcedure_InfoUpdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage);
			 
	    const bool CALLBACK _messageProcedure_Tournament_Indicator_UI_Info(const NET_MSG_GENERIC* const _pMessage);
		
	public:
		InstanceSystem::InstanceMapID NowTornanmentMapID;

		InstanceSystem::InstanceMapID TornanmentUIMapID;
		DWORD m_dwObserveType;
		DWORD m_dw1FactionID;
		DWORD m_dw2FactionID;
		DWORD m_dw1FactionScroe;
		DWORD m_dw2FactionScroe;
		bool  m_bTournamentUIview;

		DWORD m_SelectScriptID;

        // Indicator UI 가 FactionID만으로 UI정보를 표시하는 형태로 되어있어;
        // 기능이 한정적이고 확장성이 부족해서 다음과 같은 비 효율적인 변수들을 추가하였다.;
        // 언젠가 UI를 정리할 때가 온다면 이놈도 같이 죽여주기를...;
        float m_fIndicatorTime;
        std::string m_strLeftTeamIconXmlKeyword;
        std::string m_strRightTeamIconXmlKeyword;

        std::string m_strLeftTeamName;
        DWORD m_dwLeftTeamNameColor;
        std::string m_strRightTeamName;
        DWORD m_dwRightTeamNameColor;

        //std::string m_strLeftTeamScore;
        float m_fLeftTeamScore;                     // 왼쪽 팀 내용 텍스트;
        DWORD m_dwLeftTeamScoreColor;                // 왼쪽 팀 내용 텍스트 컬러;

        //std::string m_strRightTeamScore;
        float m_fRightTeamScore;                    // 오른쪽 팀 내용 텍스트;
        DWORD m_dwRightTeamScoreColor;               // 오른쪽 팀 내용 텍스트 컬러;

        std::string m_strLeftTeamImageXmlKeyword;   // 왼쪽 팀 이미지;
        bool m_bLeftTeamImageBlink;                 // 왼쪽 팀 이미지 점멸효과;
        std::string m_strRightTeamImageXmlKeyword;  // 오른쪽 팀 이미지;
        bool m_bRightTeamImageBlink;                // 오른쪽 팀 이미지 점멸효과;

        std::string m_strTeamObjectText ;           // 목표 설명 텍스트;
        DWORD m_dwTeamObjectTextColor;               // 목표 설명 텍스트 컬러;


		DWORD					m_OutofDateID;

		DWORD m_bettingGroup;
		DWORD m_LeageId;



	protected:

		GLGaeaClient*	        m_pGaeaClient;

		bool					m_Pageupdate;	
		bool					m_Dashboardupdate;	
		bool					m_TOTOupdate;	
		bool					m_MoreInfoupdate;	
		bool                    m_portal;
		bool					m_GroupInfoOutofDate;

		int						m_nThisStage;
		DWORD					m_AskedMatchingInstance;
		MatchingSystem::TOURNAMENT_MY_ROLE_TYPE m_ClientType;
		InstanceSystem::InstancePopUpTrigger* m_peventHandler;

		//Timer;
		float m_fElapsedTime;
		float m_ObserveJoinTime;
		

		
	};
}

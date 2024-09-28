#pragma once

#include "MatchingCallbackTriggerDefine.h"
#include "MatchingDefine.h"

namespace MatchingSystem
{
	class GLMatchingGroupManager;
}

namespace MatchingSystem
{
	class RoomManagerBase;
	class LobbyManagerbase;
	class GLTournamentBetting;
	class MatchingInstanceBase;
	class LobbyFieldManagerBase;

	//��Ī �ν��Ͻ��� �κ� �޴����� �Բ� ���� ������ �޴���
	class MatchingGameBase
	{
	public:
		enum EMMATCHING_MODE
		{
			EMMATCHING_MODE_SCRIPT,				// ��Ī�� ��ũ��Ʈ���� �����Ѵ�;
			EMMATCHING_MODE_SUSTAINED,
			EMMATCHING_MODE_IMMEDIATELY,
		};

	public:
		MatchingGameBase(sc::LuaInstance* script, DWORD ScriptID);
		~MatchingGameBase();



		bool OnLoad();

		void OnFrameMove(float fElpaseTime);

		// ��Ī �غ��ϱ�.
		DWORD PrePareInstance(DWORD ScriptID);

		// ��Ī �����
		void DeleteInstance(DWORD _InstanceID);

		DWORD GetScriptID (){ return m_ScriptID; }

		RoomManagerBase* getRoomManager() { return m_Roominstance; }

		LobbyManagerbase* getLobbyManager() { return m_Lobbyinstance; }

		MatchingSystem::GLMatchingGroupManager* getGroupManager() { return m_groupmanager; }

		GLTournamentBetting* getBettingManager() { return m_BettingManagerbase; }

		MatchingInstanceBase* getMatchingManager();

		void NoticeTournamentinfo(DWORD ClietDBID);

		void DropOutChar(DWORD ClietDBID);

		void JoinTournament(DWORD ClietDBID);

		void CancelTournament(DWORD ClietDBID);

		void BettingGroupEployeeInfo(DWORD ClietDBID, DWORD dwGroupID);

		void ClientMoneySpendCallBack(DWORD ClientDbid, BOOL Ok);

		void ClientMoneyCheckCallBack(DWORD ClientDbid, BOOL Ok);

		void RequireMatchingData(DWORD ClientDBid,DWORD TornanmentID);

		bool NoticeMatchedGameFinish(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _FactionID,const BOOL _IsWinner);
		bool NoticeMatchedGameCustomMSG(const InstanceSystem::InstanceMapID _instanceMapID, const double _param1, const double _param2, const double _param3, const double _param4);

		bool NoticeMatchedGameSendStart(const InstanceSystem::InstanceMapID _instanceMapID);
		bool NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const bool _data);
		bool NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const char* _data);
		bool NoticeMatchedGameSenddata(const InstanceSystem::InstanceMapID _instanceMapID,const char* varName,const double _data);
		bool NoticeMatchedGameSendEnd(const InstanceSystem::InstanceMapID _instanceMapID);

		bool NoticeMatchedGameSendERROR(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD errorCode);

		bool CaptureTheFlag_Result(const GLMSG::NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA& _msg);

		bool AnswerRUOK(DWORD ClietDBID,DWORD GamescriptID, bool isOk);

		const bool isUseCompetitionRating() {return bUseCompetitionRating;};
		const std::string& getCompetitionContentType() {return m_strContentType;}

		void RegistFunctionGameBase(sc::LuaInstance* script);

		int GetRating(DWORD CharDbNum);
		void SetRating(DWORD CharDbNum, int Rating);
		void AddRating(DWORD CharDbNum, int Rating);
		int GetWinningStreak(DWORD CharDbNum);
		void SetWinningStreak(DWORD CharDbNum, int WinningStreak);
		int GetPlayCount(DWORD CharDbNum);
		void AddPlayCount(DWORD CharDbNum);
		int GetLastPlayDate(lua_State* pState);

		void FieldMatchingStatusChaged(DWORD status);

		void PlayerPartyChanged(DWORD ClientDbid);

		void EventCustomMessage(DWORD _dwClientDBNum, DWORD _dwParam1, DWORD _dwParam2);

		void SendPostWinMoneyFC(DWORD dwDbNum,DWORD dwKang,LONGLONG winMoney,EM_MATCHING_POST_TYPE emPostType ) ;

		void FieldMatchingObserveOut( const DWORD RequestID, const InstanceSystem::InstanceMapID TornanmentMapID );

		int GetUserState(DWORD ClientDbid);

	public:
		/// Invoke Function;
		// Ŭ���̾�Ʈ���� �˸���;
		void	SendToClientMessageBox ( const lua_WChar* message, DWORD dbnum );
		void	SendToClientMessageBoxALL ( const lua_WChar* message );
		void	SendToClientMessageBoxGROUP ( const lua_WChar* message, DWORD GroupID );

		int		SendToClientMessageXML ( lua_State* pState );
		int		SendToClientMessageXMLALL ( lua_State* pState );
		int		SendToClientMessageXMLGROUP ( lua_State* pState );

		// ���� Log;
		void	Serverlog ( const char* strMessage );

	public:
		DWORD m_ScriptID;
		DWORD m_MatchingInstanceID;
		DWORD m_dwMatchingModeID;

		sc::LuaInstance* m_script;

	public:
		RoomManagerBase* m_Roominstance;

	private:
		LobbyManagerbase* m_Lobbyinstance;
		GLTournamentBetting* m_BettingManagerbase;
		LobbyFieldManagerBase* m_LobbyFieldManagerBase;

		MatchingSystem::GLMatchingGroupManager* m_groupmanager;

		MATCHING_MAP m_matchingLandlist; ///< ��Ī ID ��
		MATCHINGID m_UnUseMatchingID; ///< ������ ���� �� ID

		DWORD m_MAX_MATCHING_SIZE; ///< ��Ī �ִ� ���ڸ� �����Ѵ�.

		bool bUseCompetitionRating; // �������� �ý��� ��뿩��;
		std::string m_strContentType;  // �������� ������ Ÿ��;
	};

}
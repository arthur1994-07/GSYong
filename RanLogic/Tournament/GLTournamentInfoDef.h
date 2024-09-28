#pragma once

#include <vector>
#include "../s_NetGlobal.h"

#include "../Network/NetLogicDefine.h"

namespace MatchingSystem
{

	enum TOURNAMENT_MY_ROLE_TYPE
	{
		TOURNAMENT_PLAYER = 0,
		TOURNAMENT_OBSERVER = 1,
		TOURNAMENT_OUT		= 2,
	};

	enum MATCHING_CONTENTS_STATUS
	{
		EMMATCHING_CONTENTS_OPENED,         // - ��û����: �ش� ��ʸ�Ʈ�� ��û�� ������ �ð��� ���;
		EMMATCHING_CONTENTS_CLOSED,         // - ��û�Ұ�: �ش� ��ʸ�Ʈ�� ��û�� �Ұ����� ������ ���;
		EMMATCHING_CONTENTS_STANDBY_JOIN,   // - ������: �ش� ��ʸ�Ʈ�� ��û ���� �� ������� ����;
		EMMATCHING_CONTENTS_IN_PROGRESS,    // - ���� ��: �ش� ��ʸ�Ʈ�� ���� �������� ���;
		EMMATCHING_CONTENTS_UNKWON,			// - �˼�����: �ʱ�ȭ�� �ȵ�.
	};


	enum EMCUSTOMER_STATUS_TYPE
	{
		EMSTATUS_GROUP_OUT,				// ����.
		EMSTATUS_GROUP_IN,				// ��ϵ�.
		EMSTATUS_GROUP_PLAYING,			// �÷��� ��
	};
	enum EM_MATCHING_POST_TYPE
	{
		EMPOSTTYPE_WINNER,
		EMPOSTTYPE_TOTO_WINNER,
		EMPOSTTYPE_WINNER_POINT,
		EMPOSTTYPE_TOTO_WINNER_POINT,
	};

	enum EM_GM_MATCHING
	{
		EM_GM_MATCHING_RELOAD,
		EM_GM_MATCHING_COUNT_PACKET,
		EM_GM_MATCHING_FORCE_START,
		EM_GM_MATCHING_FORCE_OPEN,
		EM_GM_MATCHING_FORCE_CLOSE,
		EM_GM_MATCHING_FORCE_WIN,
		EM_GM_MATCHING_FORCE_LOSE,
	};

	enum EMMESAGE_TYPE_SERVER
	{

	};

	enum MATCHING_CONTENTS_TYPE
	{
		EMMATCHING_CONTENTS_NOMAL,              // �Ϲ�;
		EMMATCHING_CONTENTS_TOURNAMENT,         // ��ʸ�Ʈ;
        EMMATCHING_CONTENTS_NOMAL_REJOINALBE,   // ������ ���� �Ϲ�;
	};



	enum MATCHING_TIME_TYPE
	{
		EMMATCHING_TIME_PERIODIC,               //���� ���;
		EMMATCHING_TIME_ALWAYS,	                //��� ���;
		EMMATCHING_TIME_EVENTUAL,		        //�̺�Ʈ ���;
	};


	// ��ʸ�Ʈ �ý��ۿ� ���� �� ��� ���� �ڵ�� �̰���;
	enum EMERROR_TYPE
	{
		EMERROR_UNKWON,							// ������ �˼�����.
		EMERROR_ONLY_CAN_PARTYMASTER,			// ��Ƽ�常 ��û/��� �����ϴ�.
		EMERROR_FULL_QUEUE,						// ť�� ��á�� 
		EMERROR_CANCEL_MATCHING_JOIN_NOT_ENOUGHT,	// ��ʸ�Ʈ�� ��� �Ǿ���. (�ο��� ����)
		EMERROR_FAILED_TO_CREATE_INSTANCE,		// �δ��� ���̻� ������� ���� ��ҵ�
		EMERROR_NSIZE,
	};


	/// ���� ��û�� ����
	enum EM_BETTING_APPLAY_ANSWER
	{
		EM_BETTING_OK,
		EM_BETTING_TWICE,
		EM_BETTING_NO_MONEY,
		EM_BETTING_MONEY_LOW,
		EM_BETTING_JOINED_GAME,
		EM_BETTING_MONEY_HIGH,
		EM_BETTING_OUT_OF_TIME,
		EM_BETTING_CANCEL,
		EM_BETTING_NO_DATA,
		EM_BETTING_BUSY,
	};


	enum EM_CUSTOM_MESSAGE_PARAM
	{
		EM_CUSTOM_MESSAGE_INFO_BUTTON = 1,
	};

	struct BettingTable
	{
		DWORD UserDbid;
		DWORD bettingGroup;
		LONGLONG BettingCount;
		DWORD LeageId;
		float bettingRatio;

		MSGPACK_DEFINE(UserDbid,bettingGroup,BettingCount,LeageId,bettingRatio);
	};

	struct BettingRatio
	{
		DWORD bettingGroup;
		float fBettingRatio;
		MSGPACK_DEFINE(bettingGroup,fBettingRatio);
	};


	struct GLMatching_TimeInfo
	{
		WORD    wWeekDay;       // �� �� ȭ �� �� �� �� (1 2 3 4 5 6 7) 0 �� ��� üũ ����;
		WORD    wStartHour;     // �� (0-23����);
		WORD    wStartMin;      // �� (0-59����);
		DWORD   dwDurationMin;  // ���� �ð� (�� ����);

		MSGPACK_DEFINE( wWeekDay, wStartHour, wStartMin, dwDurationMin );

		GLMatching_TimeInfo ()
			: wWeekDay(0)
			, wStartHour(0)
			, wStartMin(0)
			, dwDurationMin(0)
		{}
	};
	typedef std::vector<GLMatching_TimeInfo> VEC_MATCHING_TIME;


	struct GLMatChingGroupInfo
	{
		int dwGROUPID;							// ��ʸ�Ʈ ��Ī ���� 1����� �ڽ��� ��ġ
		DWORD Position;							// ��ʸ�Ʈ ��ü ���� �� �ڽ��� ���� ���� ��ġ( ex. 16�� : 1 ~ 5( 16 8 4 2 1 ) )
		DWORD Index;							// ��ʸ�Ʈ ���� ���� ���̵�( �迭�� ������ ���� ������ �޶� ��� �Ǵµ�? )
		DWORD MatchingInstanceID;
		DWORD ScriptID;
		std::string GroupName;					//ĳ���� �̸�
		std::string ClubName;
		DWORD GroupSize;
		DWORD GroupMaxSize;
		DWORD CharLevel;
		DWORD GroupClass;

		MSGPACK_DEFINE(dwGROUPID, Position,Index, MatchingInstanceID, ScriptID,GroupName,ClubName,GroupSize,GroupMaxSize,CharLevel,GroupClass);
	};


	

	class GLMatchingInfo 
	{
	public:

        //------------------------------------------------------------------------------------------------------------
        //DWORD               m_dwContentsID;         // ������ ID ( ��Ī��ũ��Ʈ���� �ο����� ID );
        
        MATCHING_CONTENTS_TYPE      m_ContentsType;     // ������ Ÿ�� ( �Ϲ�, ��ʸ�Ʈ, ������ ���� �Ϲ� �� );
        MATCHING_CONTENTS_STATUS    m_ContentsStatus;   // ������ ���� ( ��û ����,�Ұ���, ������ �� );

        std::string         m_strContentsMainType;  // ������ �� �з� ��Ī;
        std::string         m_strContentsSubType;   // ������ �� �з� ��Ī;
        std::string         m_strContentsName;      // ������ �̸�;
        std::string         m_strMissionInfo;       // ������ ��ǥ;
        std::string         m_strDetailInfo;        // ������ �� ����;
        std::string         m_strRewardInfo;        // ������ ����;

        DWORD               m_dwContentsMapID;      // �� �̹����� ���� ��ID;

        MATCHING_TIME_TYPE  m_ContentsDateType;     // ������ ����ð� Ÿ�� ( ��� ��, ���� ��, �̺�Ʈ �� �� );
        __time64_t          m_tContentsStartDate;   // Ư�� �ð� ���� (�̺�Ʈ ��);
        __time64_t          m_tContentsEndDate;     // Ư�� �ð� ���� (�̺�Ʈ ��);
        VEC_MATCHING_TIME   m_vecTimeInfo;          // �ð� ���� ���� (���� ��);
        bool                m_bEnableInfoButton;    // �ش� �������� �������� ��ư�� ����ϴ��� ����;
		bool				m_bJoinInstanceDungeon;
        //------------------------------------------------------------------------------------------------------------
		DWORD				m_ScriptID;
		DWORD				m_dwMaxJoinCount;

		DWORD m_dwMatchingSize;
		DWORD m_dwCurrentRound;
        
		bool m_bCanObserve;
		bool m_bCanTOTO;

		std::vector<MatchingSystem::GLMatChingGroupInfo> m_GroupListInfo;
	};


	enum TournamentObserveType
	{
		TOURNAMENT_UI_ONE_BY_ONE = 0,
		TOURNAMENT_UI_N_BY_N,
        TOURNAMENT_UI_TEAM_VS_TEAM,
		TOURNAMENT_UI_LEAGE_SCORE,
		TOURNAMENT_UI_NON,
	};

}
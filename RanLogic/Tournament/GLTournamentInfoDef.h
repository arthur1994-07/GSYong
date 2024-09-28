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
		EMMATCHING_CONTENTS_OPENED,         // - 신청가능: 해당 토너먼트에 신청이 가능한 시간인 경우;
		EMMATCHING_CONTENTS_CLOSED,         // - 신청불가: 해당 토너먼트가 신청이 불가능한 상태인 경우;
		EMMATCHING_CONTENTS_STANDBY_JOIN,   // - 입장대기: 해당 토너먼트가 신청 마감 후 입장대기로 변경;
		EMMATCHING_CONTENTS_IN_PROGRESS,    // - 진행 중: 해당 토너먼트가 현재 진행중인 경우;
		EMMATCHING_CONTENTS_UNKWON,			// - 알수없음: 초기화가 안됨.
	};


	enum EMCUSTOMER_STATUS_TYPE
	{
		EMSTATUS_GROUP_OUT,				// 나옴.
		EMSTATUS_GROUP_IN,				// 등록됨.
		EMSTATUS_GROUP_PLAYING,			// 플레이 중
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
		EMMATCHING_CONTENTS_NOMAL,              // 일반;
		EMMATCHING_CONTENTS_TOURNAMENT,         // 토너먼트;
        EMMATCHING_CONTENTS_NOMAL_REJOINALBE,   // 재입장 가능 일반;
	};



	enum MATCHING_TIME_TYPE
	{
		EMMATCHING_TIME_PERIODIC,               //정기 방식;
		EMMATCHING_TIME_ALWAYS,	                //상시 방식;
		EMMATCHING_TIME_EVENTUAL,		        //이벤트 방식;
	};


	// 토너먼트 시스템에 관련 된 모든 에러 코드는 이곳에;
	enum EMERROR_TYPE
	{
		EMERROR_UNKWON,							// 이유를 알수없다.
		EMERROR_ONLY_CAN_PARTYMASTER,			// 파티장만 신청/취소 가능하다.
		EMERROR_FULL_QUEUE,						// 큐가 꽉찼다 
		EMERROR_CANCEL_MATCHING_JOIN_NOT_ENOUGHT,	// 토너먼트가 취소 되었다. (인원수 부족)
		EMERROR_FAILED_TO_CREATE_INSTANCE,		// 인던을 더이상 만들수가 없어 취소됨
		EMERROR_NSIZE,
	};


	/// 배팅 신청후 정보
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
		WORD    wWeekDay;       // 일 월 화 수 목 금 토 (1 2 3 4 5 6 7) 0 일 경우 체크 안함;
		WORD    wStartHour;     // 시 (0-23까지);
		WORD    wStartMin;      // 분 (0-59까지);
		DWORD   dwDurationMin;  // 진행 시간 (분 단위);

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
		int dwGROUPID;							// 토너먼트 매칭 시작 1라운드상 자신의 위치
		DWORD Position;							// 토너먼트 전체 진행 중 자신의 라운드 진행 위치( ex. 16강 : 1 ~ 5( 16 8 4 2 1 ) )
		DWORD Index;							// 토너먼트 내부 고유 아이디( 배열의 순서가 정렬 순서와 달라서 사용 되는듯? )
		DWORD MatchingInstanceID;
		DWORD ScriptID;
		std::string GroupName;					//캐릭터 이름
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
        //DWORD               m_dwContentsID;         // 컨텐츠 ID ( 매칭스크립트에서 부여받은 ID );
        
        MATCHING_CONTENTS_TYPE      m_ContentsType;     // 컨텐츠 타입 ( 일반, 토너먼트, 재입장 가능 일반 등 );
        MATCHING_CONTENTS_STATUS    m_ContentsStatus;   // 컨텐츠 상태 ( 신청 가능,불가능, 진행중 등 );

        std::string         m_strContentsMainType;  // 컨텐츠 대 분류 명칭;
        std::string         m_strContentsSubType;   // 컨텐츠 소 분류 명칭;
        std::string         m_strContentsName;      // 컨텐츠 이름;
        std::string         m_strMissionInfo;       // 컨텐츠 목표;
        std::string         m_strDetailInfo;        // 컨텐츠 상세 설명;
        std::string         m_strRewardInfo;        // 컨텐츠 보상;

        DWORD               m_dwContentsMapID;      // 맵 이미지에 사용될 맵ID;

        MATCHING_TIME_TYPE  m_ContentsDateType;     // 컨텐츠 진행시간 타입 ( 상시 식, 정기 식, 이벤트 식 등 );
        __time64_t          m_tContentsStartDate;   // 특정 시간 진행 (이벤트 식);
        __time64_t          m_tContentsEndDate;     // 특정 시간 종료 (이벤트 식);
        VEC_MATCHING_TIME   m_vecTimeInfo;          // 시간 진행 정보 (정기 식);
        bool                m_bEnableInfoButton;    // 해당 컨텐츠가 정보보기 버튼을 사용하는지 여부;
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
#pragma once

#include "GLContrlBaseMsg.h"
#include "../InstanceSystem/GLInstanceSystemDefine.h"
#include "../Tournament/GLTournamentInfoDef.h"

namespace MatchingSystem
{

	//CA (Client to AgentServer)
	//AC (AgentServer to Client)
	//FA (FieldServer to AgentServer)

	enum EMMESSAGE_TYPE
	{
		//클라와 대화할것
		EMMESSAGE_ERROR_AC,								// error!
		EMMESSAGE_ADMISSION_CF,							// 토너먼트 신청
		EMMESSAGE_ADMISSION_FA,							// 토너먼트 신청

		EMMESSAGE_OBSERVE_ADMISSION_CA,					// 토너먼트 관전 신청
		EMMESSAGE_OBSERVE_ADMISSION_AC,					// 토너먼트 관전 신청 정보
		EMMESSAGE_OBSERVE_OUT_ADMISSION_AC,					// 토너먼트 관전 나가기
		EMMESSAGE_CANCEL_CA,							// 케릭저 입장 취소
		EMMESSAGE_MATCHING_NOTIFY_MESSAGEBOX_AC,		// 메시지 박스 알림.
		EMMESSAGE_MATCHING_NOTIFY_RUOK_AC,				// 진입 메시지 박스 알림.
		EMMESSAGE_MATCHING_STAGE_STATE_FC,				// 토너먼트 스테이지 설정

		EMMESSAGE_MATCHING_NOTIFY_RUOK_CA,				// 진입 응답 메시지

		EMMESSAGE_MATCHING_REQUIRE_NOTIFY_AT_FIRST_CA,	// 토너먼트 정보 요청
		EMMESSAGE_MATCHING_NOTIFY_AT_FIRST_AC,			// 토너먼트 정보
		EMMESSAGE_MATCHING_REFRASH_FIRST_CA,			// 토너먼트가 리셋이 되었다.

		EMMESSAGE_MATCHING_GROUPTABLEINFO_REQ_CA,		// 토너먼트 정보 요청

		EMMESSAGE_MATCHING_GROUPTABLEINFO_OUTOFDATE_AC,	// 클라이언트가 들고있는 토너먼트 정보가 오래됬다.

		EMMESSAGE_MATCHING_GROUPTABLEINFO_START_AC,		// 토너먼트 정보 시작
		EMMESSAGE_MATCHING_GROUPTABLEINFO_ADD_AC,		// 토너먼트 정보 시작
		
		EMMESSAGE_MATCHING_NOTIFY_AC,					// 토너먼트 리그 정보들
		EMMESSAGE_MATCHING_GROUPTABLEINFO_NODE_AC,		// 토너먼트 정보들.

		EMMESSAGE_MATCHING_INFO_UPDATE_AC,				// 업데이트된 토너먼트 정보
		EMMESSAGE_MATCHING_INFO_UPDATE,				// 업데이트된 토너먼트 정보
		EMMESSAGE_MATCHING_INFO_CLIENT_AC,				// 유저에 따른 토너먼트 정보.
		
		EMMESSAGE_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC,	// 매칭에 들어간것을 알림

		EMMESSAGE_MATCHING_MONEY_FEE_OK_AF,				// 매칭에 필요한 돈 확인해보자
		EMMESSAGE_MATCHING_MONEY_FEE_OK_FA,				// 매칭에 필요한 돈 확인해보고 답장
		EMMESSAGE_MATCHING_MONEY_FEE_SPEND_AF,			// 매칭 들어갔으니 날려라.
		EMMESSAGE_MATCHING_MONEY_FEE_SPEND_FA,			// 돈날린 결과 보자. 없을수도 있으므로 주의!

		//토너먼트만 하는것
		EMMESSAGE_BETTING_MY_BETTING_INFO_CA,			// 나의 베팅 정보 요청
		EMMESSAGE_BETTING_MY_BETTING_INFO_AC,			// 나의 베팅 정보
		EMMESSAGE_BETTING_GROUPINFO_CA,					// 베팅 할수있는 사람들 정보 알고싶다.
		EMMESSAGE_BETTING_GROUPINFO_AC,					// 베팅 참가 정보 알려줌.
		EMMESSAGE_BETTING_GROUPINFO_MORE_AC,			// 베팅 참가 정보 더 자세하게 알려줌.
		EMMESSAGE_BETTING_BETTINGINFO_CA,				// 베팅 자체 정보 요청
		EMMESSAGE_BETTING_BETTINGINFO_AC,				// 베팅 자체 정보
		EMMESSAGE_BETTING_APPLY_CA,						// 베팅 하고싶어요 라고 요청
		EMMESSAGE_BETTING_CANSEL_CA,					// 베팅 하기싫어요 라고 요청
		EMMESSAGE_BETTING_ANSWER_AC,					// 베팅요청에 대한 대답


		EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_CA,		// 그룹에 들어간 인원 정보 알고싶다.
		EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_AC,		// 그룹에 들어간 정보 응답

		EMMESSAGE_BETTING_MONEY_OK_AF,					// 베팅 필드서버에게 이친구 돈있냐고 물어봄
		EMMESSAGE_BETTING_MONEY_OK_FA,					// 베팅 필드서버가 돈 결과를 알려줌
		EMMESSAGE_BETTING_MONEY_OK_SPEND_AF,			// 베팅 필드서버에게 돈쓰라고 알려줌
		EMMESSAGE_BETTING_MONEY_OK_SPEND_FA,			// 베팅 필드서버가 돈을 날리고 결과를 알려줌
		EMMESSAGE_BETTING_MONEY_OK_RESTORE_AF,			// 베팅 필드서버에게 돈을 회복시켜라고 알려줌
		EMMESSAGE_BETTING_MONEY_OK_RESTORE_FA,			// 베팅 필드서버가 돈을 회복시키고 알려줌.

		EMMESSAGE_BETTING_MONEY_OK_INCOME_AF,			// 필드서버에게 돈주라고 알려줌
		EMMESSAGE_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF,			// 필드서버에게 메일 보내라고 알려줌

		EMMESSAGE_TOURNAMENT_IN_TOURNAMENT_FC,				// 토너먼트 게임중이다 라고알림
		EMMESSAGE_TOURNAMENT_OUT_TOURNAMENT_FC,				// 토너먼트 아니라고 알림
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_ROLE_FC,		// 토너먼트 정보
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC,	// 토너먼트 팀정보 (승 패)
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_WIN_FC,		// 토너먼트 승리
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_LOSE_FC,		// 토너먼트 패배
		EMMESSAGE_TOURNAMENT_FIGHT_COUNTING_FC,				// 토너먼트 카운팅

		EMMESSAGE_TOURNAMENT_GAME_TIMELIMIT_FC,				// 토너먼트 시간제한.
		EMMESSAGE_TOURNAMENT_GAME_FACTION_INFO_FC,			// 토너먼트 펙션 정보.
		EMMESSAGE_TOURNAMENT_GAME_POPUP_FC,					// 토너먼트 팝업 띄우기.
		EMMESSAGE_TOURNAMENT_GAME_POPUP_CF,					// 토너먼트 팝업 꺼버림.

        EMMESSAGE_TOURNAMENT_INDICATOR_UI_INFO_FC,			// 토너먼트 펙션 정보.

        EMMESSAGE_CAPTURE_THE_FLAG_RESULT_FA,           // 깃발전 결과 정보;
		
        // 전장 점수 DB와 통신
		EMMESSAGE_COMPETITION_RATING_GETINFO_DA,        // 전장 점수 정보 DB에서의 응답;

		//필드서버와 할것

		EMMESSAGE_MATCHING_RESUALT_SEND_START_FA,		// 메칭된 게임 정보 보내기 시작.
		EMMESSAGE_MATCHING_RESUALT_SEND_END_FA,			// 메칭된 게임 정보 보내기 끝.

		EMMESSAGE_MATHCING_RESUALT_SCORE_FA,			// 메칭된 게임 정보 점수(LUA 변수이름으로보냄)
		EMMESSAGE_MATCHING_RESUALT_FACTION_RESUALT,		// 메칭된 게임 이긴 FACTION 정보 보냄
        EMMESSAGE_MATCHING_RESULT_CUSTOM_MSG_FA,        // 인던에서 매칭으로 커스텀 결과 메시지 보내기 (Matching에서 처리);
		
		EMMESSAGE_MATCHING_RESUALT_ERROR_FA,			// 메칭된 게임 에러 정보 보냄

		EMMESSAGE_MATCHING_PARTY_CHANGE_MASTER,			// 파티 마스터 바꿈.
		EMMESSAGE_MATCHING_PARTY_ADD_MEMBER,			// 파티 맴버 바뀜.
		EMMESSAGE_MATCHING_PARTY_REMOVE_MEMBER,			// 파티 맴버 지워짐.
		EMMESSAGE_MATCHING_PARTY_DISTROY,			    // 파티 맴버 지워짐.

        // 커스텀 메시지;
        EMMESSAGE_MATCHING_CUSTOM_MSG_CA,               // 컨텐츠에 커스텀 메시지 보내기 (Room에서 처리);

		//GM 명령어
		EMMESSAGE_MATCHING_GM_COMMAND_CA,					// GM 명령 Reset

		EMMESSAGE_MATCHING_NSIZE,
	};

	


}


namespace MatchingSystem
{
	struct BettingTable;
	struct GLMatching_TimeInfo;
	struct GLMatChingGroupInfo;
	class GLMatchingInfo;
}

namespace GLMSG
{
	// 토너먼트 베이스 메세지
	struct NET_MATCHING_MESSAGE : public NET_MSG_GENERIC
	{
		const MatchingSystem::EMMESSAGE_TYPE emType;

		NET_MATCHING_MESSAGE(const DWORD nSize, const MatchingSystem::EMMESSAGE_TYPE _emType)
			: NET_MSG_GENERIC(NET_MSG_GCTRL_MATCHING_SYSTEM, nSize)
			, emType(_emType)
		{
		}
	};

	//! 주의 : NET_INSTANCE_MSGPACK_MESSAGE를 상속받은 모든 구조체의 경우
	// MSGPACK_DEFINE 선언시 emType을 맨 첫번째로 넣은 이후에 자신의 자료형을 넣어줘야한다.
	struct NET_MATCHING_MSGPACK_MESSAGE // : public NET_MSG_GENERIC
	{
		DWORD emType;

		MSGPACK_DEFINE(emType);

		NET_MATCHING_MSGPACK_MESSAGE()
			: emType(0)
		{
		}

		NET_MATCHING_MSGPACK_MESSAGE(const MatchingSystem::EMMESSAGE_TYPE _emType)
			: emType(_emType)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////////
	///////In This line is All about Matching Message Pack ///////////////////
	//////////////////////////////////////////////////////////////////////////

	// 토너먼트 클라 리그 정보
	struct NET_MATCHING_GROUPTABLEINFO_START: public NET_MATCHING_MSGPACK_MESSAGE
	{
		DWORD TourNumentSize;
		DWORD TourNumCurrentRound;
		DWORD ScriptID;
		std::vector<MatchingSystem::GLMatChingGroupInfo> matchingInfo;

		MSGPACK_DEFINE(emType, TourNumentSize,TourNumCurrentRound, ScriptID, matchingInfo);

		NET_MATCHING_GROUPTABLEINFO_START()
			:NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_START_AC)
		{
		}
	};

	// 토너먼트 클라 리그 정보
	struct NET_MATCHING_GROUPTABLEINFO_ADD: public NET_MATCHING_MSGPACK_MESSAGE
	{
		DWORD TourNumentSize;
		DWORD ScriptID;
		std::vector<MatchingSystem::GLMatChingGroupInfo> matchingInfo;

		MSGPACK_DEFINE(emType, TourNumentSize, ScriptID, matchingInfo);

		NET_MATCHING_GROUPTABLEINFO_ADD()
			:NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_ADD_AC)
		{
		}

	};

	


	// 베팅 그룹 정보 알려줌.
	struct NET_BETTING_GROUPINFO_AC : public NET_MATCHING_MSGPACK_MESSAGE
	{

		DWORD bettingGroup;
		LONGLONG BettingCount;
		float BettingRato;
		DWORD ScriptID;

		std::vector<MatchingSystem::BettingRatio> Bettinginfo;

		MSGPACK_DEFINE(emType, bettingGroup, BettingCount, BettingRato,ScriptID,Bettinginfo);

		NET_BETTING_GROUPINFO_AC()
			: NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_AC)
		{
		}
	};

    struct NET_TOURNAMENT_INDICATOR_UI_INFO_FC: public NET_MATCHING_MSGPACK_MESSAGE
    {
        enum
        {
            Show = 0,
            Time,
            LeftName,
            RightName,
            LeftIcon,
            RightIcon,
            LeftScore,
            RightScore,
            LeftImage,
            RightImage,
            Object,  
        };

        DWORD TornanmentMapID;
        DWORD emInfoType;
        std::string strText;
        DWORD dwFontColor;
        float fFloatData;
        bool bBlink;

        MSGPACK_DEFINE(emType, TornanmentMapID, emInfoType, strText, dwFontColor, fFloatData, bBlink);


        NET_TOURNAMENT_INDICATOR_UI_INFO_FC()
            : NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_TOURNAMENT_INDICATOR_UI_INFO_FC)
        {
        }

        NET_TOURNAMENT_INDICATOR_UI_INFO_FC( const InstanceSystem::InstanceMapID _TornanmentMapID )
            : NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_TOURNAMENT_INDICATOR_UI_INFO_FC)
            , TornanmentMapID(_TornanmentMapID)
        {
        }
    };

    struct NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA : public NET_MATCHING_MSGPACK_MESSAGE
    {
        DWORD InstanceMapID;
        std::vector<DWORD> RedExistPlayer;
        std::vector<DWORD> BlueExistPlayer;
        std::vector<DWORD> RedRunawayPlayer;
        std::vector<DWORD> BlueRunawayPlayer;
        int dwResult;

        MSGPACK_DEFINE(emType, InstanceMapID, RedExistPlayer, BlueExistPlayer, RedRunawayPlayer, BlueRunawayPlayer, dwResult);

        NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA()
            : NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_CAPTURE_THE_FLAG_RESULT_FA)
        {
        }
    };


	//EMMESSAGE_TOURNAMENT_GAME_POPUP_CF
	// Faction 정보
	struct NET_TOURNAMENT_GAME_POPUP_CF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		NET_TOURNAMENT_GAME_POPUP_CF(const DWORD _dwDbNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAME_POPUP_CF), MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_POPUP_CF)
			, dwDbNum(_dwDbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAME_POPUP_CF) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_TOURNAMENT_GAME_POPUP_FC: public NET_MATCHING_MESSAGE
	{
		bool	bShow;

		NET_TOURNAMENT_GAME_POPUP_FC(const bool _bShow)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAME_POPUP_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_POPUP_FC)
			, bShow(_bShow)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAME_POPUP_FC) <= NET_DATA_BUFSIZE);
		}
	};


	// Faction 정보
	struct NET_TOURNAMENT_GAME_FACTION_INFO_FC: public NET_MATCHING_MESSAGE
	{

		const InstanceSystem::InstanceMapID TornanmentMapID;
		const DWORD dwDbNum;
		const DWORD dw1Faction;
		const DWORD dw2Faction;
		const DWORD dwViewType;

		NET_TOURNAMENT_GAME_FACTION_INFO_FC(
			const DWORD _dwDbNum
			,const DWORD _dw1Faction
			,const DWORD _dw2Faction
			,const DWORD _dwViewType
			,const InstanceSystem::InstanceMapID _TornanmentMapID )
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAME_FACTION_INFO_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_FACTION_INFO_FC)
			, dwDbNum(_dwDbNum)
			,dw1Faction(_dw1Faction)
			,dw2Faction(_dw2Faction)
			,TornanmentMapID(_TornanmentMapID)
			,dwViewType(_dwViewType)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAME_FACTION_INFO_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 게임 시간제한
	struct NET_TOURNAMENT_GAME_TIMELIMIT_FC: public NET_MATCHING_MESSAGE
	{
		const bool bShow;
		const bool bBlank;
		const int nSec;
		NET_TOURNAMENT_GAME_TIMELIMIT_FC(
			const bool _bShow,const bool _bBlank,int _nSec)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAME_TIMELIMIT_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAME_TIMELIMIT_FC)
			, bShow(_bShow),bBlank(_bBlank),nSec(_nSec)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAME_TIMELIMIT_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 게임 졌다고 알림
	struct NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC(
			const DWORD _dwDbNum,const DWORD _dwClientType,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_LOSE_FC)
			, dwDbNum(_dwDbNum),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAMEINFORMATION_LOSE_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 게임 이겼다고 알림
	struct NET_TOURNAMENT_GAMEINFORMATION_WIN_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_GAMEINFORMATION_WIN_FC(
			const DWORD _dwDbNum,const DWORD _dwClientType,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAMEINFORMATION_WIN_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_WIN_FC)
			, dwDbNum(_dwDbNum),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAMEINFORMATION_WIN_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 게임중이다 라고알림
	struct NET_TOURNAMENT_IN_TOURNAMENT_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwClientType;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_IN_TOURNAMENT_FC(const DWORD _dwClientType,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_IN_TOURNAMENT_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_IN_TOURNAMENT_FC)
			,dwClientType(_dwClientType),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_IN_TOURNAMENT_FC) <= NET_DATA_BUFSIZE);
		}
	};
	// 토너먼트 아니라고 알림
	struct NET_TOURNAMENT_OUT_TOURNAMENT_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_OUT_TOURNAMENT_FC(
			const DWORD _dwDbNum,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_OUT_TOURNAMENT_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_OUT_TOURNAMENT_FC)
			, dwDbNum(_dwDbNum),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_OUT_TOURNAMENT_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 정보
	struct NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC: public NET_MATCHING_MESSAGE
	{
		DWORD GameWinConditionScore;
		DWORD leftTime;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC(
			const DWORD _GameWinConditionScore,const DWORD _leftTime,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_ROLE_FC)
			, GameWinConditionScore(_GameWinConditionScore),leftTime(_leftTime),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAMEINFORMATION_ROLE_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 팀정보 (승 패)
	struct NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD TeamScore;
		const DWORD TeamFactionID;
		const InstanceSystem::InstanceMapID TornanmentMapID;

		NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC(
			const DWORD _TeamScore,const DWORD _TeamFactionID,InstanceSystem::InstanceMapID _TornanmentMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC)
			, TeamScore(_TeamScore),TeamFactionID(_TeamFactionID),TornanmentMapID(_TornanmentMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 팀정보 (승 패)
	struct NET_TOURNAMENT_FIGHT_COUNTING_FC: public NET_MATCHING_MESSAGE
	{
		const DWORD Counting;
		NET_TOURNAMENT_FIGHT_COUNTING_FC(const DWORD _Counting)
			: NET_MATCHING_MESSAGE(sizeof(NET_TOURNAMENT_FIGHT_COUNTING_FC), MatchingSystem::EMMESSAGE_TOURNAMENT_FIGHT_COUNTING_FC)
			, Counting(_Counting)
		{
			MIN_STATIC_ASSERT(sizeof(NET_TOURNAMENT_FIGHT_COUNTING_FC) <= NET_DATA_BUFSIZE);
		}
	};


	/*
	EMMESSAGE_TOURNAMENT_IN_TOURNAMENT_FC,				// 토너먼트 게임중이다 라고알림
		EMMESSAGE_TOURNAMENT_OUT_TOURNAMENT_FC,				// 토너먼트 아니라고 알림
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_ROLE_FC,		// 토너먼트 정보
		EMMESSAGE_TOURNAMENT_GAMEINFORMATION_TEAMINFO_FC,	// 토너먼트 팀정보 (승 패)
		*/


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	// 구룹정보 요청
	struct NET_BETTING_GROUP_EPLOYEE_INFO_CA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwGroupID;

		NET_BETTING_GROUP_EPLOYEE_INFO_CA(
			const DWORD _dwDbNum,const DWORD _dwGroupID)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_CA), MatchingSystem::EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_CA)
			, dwDbNum(_dwDbNum),dwGroupID(_dwGroupID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 구룹정보 요청
	struct NET_BETTING_GROUP_EPLOYEE_INFO_AC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		DWORD dwLevel;
		char  szSetName[CHAR_SZNAME];
		DWORD wClass;
		DWORD GroupID;
		DWORD LeageId;

		NET_BETTING_GROUP_EPLOYEE_INFO_AC(
			const DWORD _dwDbNum,const DWORD _dwLevel,const char* _szSetName,
			const DWORD _wClass,const DWORD _GroupID,const DWORD _LeageId)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_AC), MatchingSystem::EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_AC)
			, dwDbNum(_dwDbNum),dwLevel(_dwLevel),wClass(_wClass),GroupID(_GroupID),LeageId(_LeageId)
		{
			StringCchCopy(szSetName, CHAR_SZNAME, _szSetName);
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_AC) <= NET_DATA_BUFSIZE);
		}

		NET_BETTING_GROUP_EPLOYEE_INFO_AC(
			const DWORD _dwDbNum,const DWORD _dwLevel, std::string& _szSetName,
			const DWORD _wClass,const DWORD _GroupID,const DWORD _LeageId)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_AC), MatchingSystem::EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_AC)
			, dwDbNum(_dwDbNum),dwLevel(_dwLevel),wClass(_wClass),GroupID(_GroupID),LeageId(_LeageId)
		{
			StringCchCopy(szSetName, CHAR_SZNAME, _szSetName.c_str());
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_GROUP_EPLOYEE_INFO_AC) <= NET_DATA_BUFSIZE);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	///////BETTING//SOCK//////LIST////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// 내 배팅 내역
	struct NET_BETTING_MY_BETTING_INFO_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;

		NET_BETTING_MY_BETTING_INFO_CA(
			const DWORD _dwDbNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MY_BETTING_INFO_CA), MatchingSystem::EMMESSAGE_BETTING_MY_BETTING_INFO_CA)
			, dwDbNum(_dwDbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MY_BETTING_INFO_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 내 배팅 내역
	struct NET_BETTING_MY_BETTING_INFO_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD bettingGroup;
		const LONGLONG BettingCount;
		const DWORD LeageId;
		
		NET_BETTING_MY_BETTING_INFO_AC(
			const DWORD _dwDbNum,
			const DWORD _bettingGroup,
			const LONGLONG _BettingCount,
			const DWORD _LeageId)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MY_BETTING_INFO_AC), MatchingSystem::EMMESSAGE_BETTING_MY_BETTING_INFO_AC)
			, dwDbNum(_dwDbNum),bettingGroup(_bettingGroup),BettingCount(BettingCount)
			, LeageId(_LeageId)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MY_BETTING_INFO_AC) <= NET_DATA_BUFSIZE);
		}
	};


	// 베팅 할수있는 사람들 정보 알고싶다.
	struct NET_BETTING_GROUPINFO_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;

		NET_BETTING_GROUPINFO_CA(const DWORD _dwDbNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_GROUPINFO_CA), MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_CA)
			, dwDbNum(_dwDbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_GROUPINFO_CA) <= NET_DATA_BUFSIZE);
		}
	};

	
	// 베팅 그룹 정보 알려줌.
	struct NET_BETTING_GROUPINFO_MORE_AC : public NET_MATCHING_MESSAGE
	{
		char ClubName[CHAR_SZNAME];
		DWORD LeageId;
		DWORD MatchingInstanceNum;
		DWORD Groupsize;
		DWORD GroupID;
		DWORD GroupMaxSize;
		//SYSTEMTIME BettingTime;

		NET_BETTING_GROUPINFO_MORE_AC(
			std::string& _ClubName,
			DWORD _LeageId,
			DWORD _MatchingInstanceNum,
			DWORD _GroupID,
			DWORD _Groupsize,
			DWORD _GroupMaxSize
			/*SYSTEMTIME _BettingTime,*/)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_GROUPINFO_MORE_AC), MatchingSystem::EMMESSAGE_BETTING_GROUPINFO_MORE_AC)
			,Groupsize(_Groupsize),GroupID(_GroupID),GroupMaxSize(_GroupMaxSize), LeageId(_LeageId),
			MatchingInstanceNum(_MatchingInstanceNum)
		{
			StringCchCopy(ClubName, CHAR_SZNAME, _ClubName.c_str());
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_GROUPINFO_MORE_AC) <= NET_DATA_BUFSIZE);
		}
	};



	// 베팅 참가 정보 알려달라
	struct NET_BETTING_BETTINGINFO_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD ClientDbid;
		const DWORD LeageId;


		NET_BETTING_BETTINGINFO_CA(DWORD _ClientDbid,DWORD _LeageId)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_BETTINGINFO_CA), MatchingSystem::EMMESSAGE_BETTING_BETTINGINFO_CA)
			, ClientDbid(_ClientDbid),
			LeageId(_LeageId)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_BETTINGINFO_CA) <= NET_DATA_BUFSIZE);
		}
	};


	// 베팅 참가 정보 알려줌.
	struct NET_BETTING_BETTINGINFO_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD ClientDbid;
		const LONGLONG MinimumBettingSize;
		const LONGLONG MaximumBettingSize;
		const DWORD ScriptID;
		

		NET_BETTING_BETTINGINFO_AC(DWORD _ClientDbid,
			LONGLONG _MinimumBettingSize, LONGLONG _MaximumBettingSize,DWORD _LeageId)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_BETTINGINFO_AC), MatchingSystem::EMMESSAGE_BETTING_BETTINGINFO_AC)
			, ClientDbid(_ClientDbid), MinimumBettingSize(_MinimumBettingSize), MaximumBettingSize(_MaximumBettingSize),
			ScriptID(_LeageId)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_BETTINGINFO_AC) <= NET_DATA_BUFSIZE);
		}
	};


	// 베팅 하고싶어요 라고 요청
	struct NET_BETTING_APPLY_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD ClientDbid;
		DWORD bettingGroup;
		LONGLONG BettingCount;
		DWORD LeageId;

		NET_BETTING_APPLY_CA(DWORD _ClientDbid,DWORD _GroupId, LONGLONG _BettingMoney, DWORD _LeageID)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_APPLY_CA), MatchingSystem::EMMESSAGE_BETTING_APPLY_CA)
			, ClientDbid(_ClientDbid), bettingGroup(_GroupId), BettingCount(_BettingMoney)
			, LeageId(_LeageID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_APPLY_CA) <= NET_DATA_BUFSIZE);
		}
	};



	// 베팅 하기싫어요 라고 요청
	struct NET_BETTING_CANSEL_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD ClientDbid;
		DWORD LeageId;

		NET_BETTING_CANSEL_CA(DWORD _ClientDbid, DWORD _LeageID)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_CANSEL_CA), MatchingSystem::EMMESSAGE_BETTING_CANSEL_CA)
			, ClientDbid(_ClientDbid)
			, LeageId(_LeageID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_CANSEL_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 베팅요청에 대한 대답
	struct NET_BETTING_ANSWER_AC: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const MatchingSystem::EM_BETTING_APPLAY_ANSWER emReseon;

		NET_BETTING_ANSWER_AC(const DWORD _dwDbNum,MatchingSystem::EM_BETTING_APPLAY_ANSWER _emReseon)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_ANSWER_AC), MatchingSystem::EMMESSAGE_BETTING_ANSWER_AC)
			, dwDbNum(_dwDbNum),emReseon(_emReseon)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_ANSWER_AC) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버에게 이친구 돈있냐고 물어봄
	struct NET_BETTING_MONEY_OK_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_BETTING_MONEY_OK_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_AF), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_AF) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버가 결과를 알려줌
	struct NET_BETTING_MONEY_OK_FA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const BOOL ISGOOD;

		NET_BETTING_MONEY_OK_FA(const DWORD _dwDbNum,const BOOL _ISGOOD)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_FA), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_FA)
			, dwDbNum(_dwDbNum),ISGOOD(_ISGOOD)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_FA) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버에게 돈쓰라고 알려줌
	struct NET_BETTING_MONEY_OK_SPEND_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_BETTING_MONEY_OK_SPEND_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_SPEND_AF), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_SPEND_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_SPEND_AF) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버에게 돈을 회복싴켜라고 알려줌
	struct NET_BETTING_MONEY_OK_RESTORE_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_BETTING_MONEY_OK_RESTORE_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_RESTORE_AF), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_RESTORE_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_RESTORE_AF) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버에게 돈을 회복싴켜라고 알려줌
	struct NET_BETTING_MONEY_OK_RESTORE_FA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_BETTING_MONEY_OK_RESTORE_FA(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_RESTORE_FA), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_RESTORE_FA)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_RESTORE_FA) <= NET_DATA_BUFSIZE);
		}
	};


	// 필드서버가 돈을 날리고 결과를 알려줌
	struct NET_BETTING_MONEY_OK_SPEND_FA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const BOOL ISGOOD;


		NET_BETTING_MONEY_OK_SPEND_FA(const DWORD _dwDbNum,const BOOL _ISGOOD)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_SPEND_FA), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_SPEND_FA)
			, dwDbNum(_dwDbNum),ISGOOD(_ISGOOD)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_SPEND_FA) <= NET_DATA_BUFSIZE);
		}
	};

	// 필드서버에게 돈주라고 알려줌
	struct NET_BETTING_MONEY_OK_INCOME_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_BETTING_MONEY_OK_INCOME_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_BETTING_MONEY_OK_INCOME_AF), MatchingSystem::EMMESSAGE_BETTING_MONEY_OK_INCOME_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_INCOME_AF) <= NET_DATA_BUFSIZE);
		}
	};


	/// Agent -> Field
	struct NET_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF : public NET_MATCHING_MESSAGE
	{
		DWORD m_dwDbNum;
		DWORD m_dwKang;
		LONGLONG m_winMoney;
		MatchingSystem::EM_MATCHING_POST_TYPE m_emPostType;
		DWORD ScriptID;

		NET_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF(
			DWORD _ScriptID
			, DWORD dwDbNum
			, DWORD dwKang
			, LONGLONG winMoney
			, MatchingSystem::EM_MATCHING_POST_TYPE emPostType)
			: NET_MATCHING_MESSAGE( sizeof(NET_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF), MatchingSystem::EMMESSAGE_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF)
			, m_dwDbNum( dwDbNum )
			, m_dwKang( dwKang )
			, m_winMoney( winMoney )
			, m_emPostType(emPostType)
			, ScriptID(_ScriptID)
		{

		}
	};

	//////////////////////////////////////////////////////////////////////////
	///////BETTING//SOCK//////LIST///////END END END END END//////////////////
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	////////TOURNAMENT/////N///////MATCHING/////MONEY!////////////////////////
	//////////////////////////////////////////////////////////////////////////

	

	// 매칭에 필요한 돈 확인해보자
	struct NET_MATCHING_MONEY_FEE_OK_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_MATCHING_MONEY_FEE_OK_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_MONEY_FEE_OK_AF), MatchingSystem::EMMESSAGE_MATCHING_MONEY_FEE_OK_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_MONEY_FEE_OK_AF) <= NET_DATA_BUFSIZE);
		}
	};

	// 매칭에 필요한 돈 확인해보고 답장
	struct NET_MATCHING_MONEY_FEE_OK_FA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const BOOL ISGOOD;

		NET_MATCHING_MONEY_FEE_OK_FA(const DWORD _dwDbNum,const BOOL _ISGOOD)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_MONEY_FEE_OK_FA), MatchingSystem::EMMESSAGE_MATCHING_MONEY_FEE_OK_FA)
			, dwDbNum(_dwDbNum),ISGOOD(_ISGOOD)
		{
			MIN_STATIC_ASSERT(sizeof(NET_BETTING_MONEY_OK_SPEND_FA) <= NET_DATA_BUFSIZE);
		}
	};


	// 매칭 들어갔으니 날려라.
	struct NET_MATCHING_MONEY_FEE_SPEND_AF: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const LONGLONG Money;

		NET_MATCHING_MONEY_FEE_SPEND_AF(const DWORD _dwDbNum,LONGLONG _Money)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_MONEY_FEE_SPEND_AF), MatchingSystem::EMMESSAGE_MATCHING_MONEY_FEE_SPEND_AF)
			, dwDbNum(_dwDbNum),Money(_Money)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_MONEY_FEE_SPEND_AF) <= NET_DATA_BUFSIZE);
		}
	};

	// 매칭에 필요한 돈 확인해보고 답장
	struct NET_MATCHING_MONEY_FEE_SPEND_FA: public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const BOOL ISGOOD;

		NET_MATCHING_MONEY_FEE_SPEND_FA(const DWORD _dwDbNum,const BOOL _ISGOOD)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_MONEY_FEE_SPEND_FA), MatchingSystem::EMMESSAGE_MATCHING_MONEY_FEE_SPEND_FA)
			, dwDbNum(_dwDbNum),ISGOOD(_ISGOOD)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_MONEY_FEE_SPEND_FA) <= NET_DATA_BUFSIZE);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	////////TOURNAMENT/////N///////MATCHING////END END END END END////////////
	//////////////////////////////////////////////////////////////////////////

	// 토너먼트 정보 신청
	struct NET_MATCHING_MESSAGE_INFO_REQ_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;

		NET_MATCHING_MESSAGE_INFO_REQ_CA(const DWORD _dwDbNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_MESSAGE_INFO_REQ_CA), MatchingSystem::EMMESSAGE_MATCHING_REQUIRE_NOTIFY_AT_FIRST_CA)
			, dwDbNum(_dwDbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_MESSAGE_INFO_REQ_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 정보 다 리셋
	struct NET_MATCHING_REFRASH_FIRST_CA : public NET_MATCHING_MESSAGE
	{

		NET_MATCHING_REFRASH_FIRST_CA()
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_REFRASH_FIRST_CA), MatchingSystem::EMMESSAGE_MATCHING_REFRASH_FIRST_CA)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_REFRASH_FIRST_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 신청
	struct NET_MATCHING_ADMISSION_FA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD TornanmentID;

		NET_MATCHING_ADMISSION_FA(const DWORD _dwDbNum,const DWORD _TornanmentID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_ADMISSION_FA), MatchingSystem::EMMESSAGE_ADMISSION_FA)
			, dwDbNum(_dwDbNum), TornanmentID(_TornanmentID)
		{

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_ADMISSION_FA) <= NET_DATA_BUFSIZE);
		}
	};



	// 토너먼트 신청 Client To Field
	struct NET_MATCHING_ADMISSION_CF : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD TornanmentID;

		NET_MATCHING_ADMISSION_CF(const DWORD _dwDbNum,const DWORD _TornanmentID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_ADMISSION_CF), MatchingSystem::EMMESSAGE_ADMISSION_CF)
			, dwDbNum(_dwDbNum), TornanmentID(_TornanmentID)
		{

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_ADMISSION_CF) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 신청
	struct NET_MATCHING_OBSERVE_ADMISSION_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD ScriptID;
		const DWORD GroupID;
		const DWORD RequestID;

		NET_MATCHING_OBSERVE_ADMISSION_CA(const DWORD _ScriptID,DWORD _GroupID, DWORD _RequestID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_OBSERVE_ADMISSION_CA), MatchingSystem::EMMESSAGE_OBSERVE_ADMISSION_CA)
			, ScriptID(_ScriptID), GroupID(_GroupID), RequestID(_RequestID)
		{

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_OBSERVE_ADMISSION_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 신청
	struct NET_MATCHING_OBSERVE_ADMISSION_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD ScriptID;
		const DWORD GroupID;
		const DWORD RequestID;

		NET_MATCHING_OBSERVE_ADMISSION_AC(const DWORD _ScriptID,DWORD _GroupID, DWORD _RequestID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_OBSERVE_ADMISSION_AC), MatchingSystem::EMMESSAGE_OBSERVE_ADMISSION_AC)
			, ScriptID(_ScriptID), GroupID(_GroupID), RequestID(_RequestID)
		{

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_OBSERVE_ADMISSION_AC) <= NET_DATA_BUFSIZE);
		}
	};



	// 토너먼트 신청
	struct NET_MATCHING_OBSERVE_OUT_ADMISSION_AC : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		const DWORD RequestID;

		NET_MATCHING_OBSERVE_OUT_ADMISSION_AC(const InstanceSystem::InstanceMapID _TornanmentMapID, DWORD _RequestID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_OBSERVE_OUT_ADMISSION_AC), MatchingSystem::EMMESSAGE_OBSERVE_OUT_ADMISSION_AC)
			, TornanmentMapID(_TornanmentMapID), RequestID(_RequestID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_OBSERVE_OUT_ADMISSION_AC) <= NET_DATA_BUFSIZE);
		}
	};





	// 토너먼트 신청 취소
	struct NET_MATCHING_CANCEL_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD TornanmentID;

		NET_MATCHING_CANCEL_CA(const DWORD _dwDbNum,const DWORD _TornanmentID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_CANCEL_CA), MatchingSystem::EMMESSAGE_CANCEL_CA)
			, dwDbNum(_dwDbNum), TornanmentID(_TornanmentID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_CANCEL_CA) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 진행중 에러 메시지
	struct NET_MATCHING_ERROR_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwError;
		const DWORD TornanmentID;

		NET_MATCHING_ERROR_AC(const DWORD _dwDbNum,const DWORD _ERRORID,const DWORD _TornanmentID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_ERROR_AC), MatchingSystem::EMMESSAGE_ERROR_AC)
			, dwDbNum(_dwDbNum), dwError(_ERRORID), TornanmentID(_TornanmentID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_ERROR_AC) <= NET_DATA_BUFSIZE);
		}
	};



	// 토너먼트 클라이언트에게 알릴사항
	// GLCustommerEmployee 에서 쓰일것들
	struct NET_MATCHING_NOTIFY_MESSAGEBOX_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		char szCharName[CHAR_SZNAME];
		bool isXml;

		NET_MATCHING_NOTIFY_MESSAGEBOX_AC(const DWORD _dwDbNum,std::string& _strName)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_MESSAGEBOX_AC), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_MESSAGEBOX_AC)
			, dwDbNum(_dwDbNum)
		{
			isXml = false;
			StringCchCopy(szCharName, CHAR_SZNAME, _strName.c_str());

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_MESSAGEBOX_AC) <= NET_DATA_BUFSIZE);
		}

		NET_MATCHING_NOTIFY_MESSAGEBOX_AC(const DWORD _dwDbNum,std::string& _strName,bool _isXml)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_MESSAGEBOX_AC), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_MESSAGEBOX_AC)
			, dwDbNum(_dwDbNum),isXml(_isXml)
		{
			StringCchCopy(szCharName, CHAR_SZNAME, _strName.c_str());

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_MESSAGEBOX_AC) <= NET_DATA_BUFSIZE);
		}


	};

	// 토너먼트 입장되었음을 알림
	struct NET_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;

		NET_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC(const DWORD _dwDbNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC)
			, dwDbNum(_dwDbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_APPLY_TOURNAMENT_AC) <= NET_DATA_BUFSIZE);
		}
	};

	// 매칭에 들어가기 전에 매칭에 들어올것인지 질문;
	// 다른 클라이언트가 이미 취소했을경우 강제 취소;
	struct NET_MATCHING_NOTIFY_RUOK_AC : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwGameScriptID;
		const float fTime;

		bool bComplete;

		NET_MATCHING_NOTIFY_RUOK_AC(const DWORD _dwDbNum,const DWORD _dwGameScriptID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_RUOK_AC), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_RUOK_AC)
			, dwDbNum(_dwDbNum), dwGameScriptID(_dwGameScriptID) , fTime(5), bComplete( false )
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_RUOK_AC) <= NET_DATA_BUFSIZE);
		}
		NET_MATCHING_NOTIFY_RUOK_AC(const DWORD _dwDbNum,const DWORD _dwGameScriptID,const float _fTime)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_RUOK_AC), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_RUOK_AC)
			, dwDbNum(_dwDbNum), dwGameScriptID(_dwGameScriptID), fTime(_fTime), bComplete( false )
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_RUOK_AC) <= NET_DATA_BUFSIZE);
		}
	};

	// SetStageState를 통한 UI 사망대기 CLEAR 띄우기
	struct NET_MATCHING_STAGE_STATE_FC : public NET_MATCHING_MESSAGE
	{
		const int istate;
		const bool Show;

		NET_MATCHING_STAGE_STATE_FC(const int _istate,const bool _Show)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_STAGE_STATE_FC), MatchingSystem::EMMESSAGE_MATCHING_STAGE_STATE_FC)
			, istate(_istate), Show(_Show)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_STAGE_STATE_FC) <= NET_DATA_BUFSIZE);
		}
	};

	// 들어올꺼냐는 것에 대한 응답
	struct NET_MATCHING_NOTIFY_RUOK_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwGameScriptID;
		const bool isImok;

		NET_MATCHING_NOTIFY_RUOK_CA(const DWORD _dwDbNum,const DWORD _dwGameScriptID, const DWORD _isImok)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_NOTIFY_RUOK_CA), MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_RUOK_CA)
			, dwDbNum(_dwDbNum), dwGameScriptID(_dwGameScriptID),isImok(_isImok)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_NOTIFY_RUOK_CA) <= NET_DATA_BUFSIZE);
		}
	};

	//EMMESSAGE_MATCHING_GROUPTABLEINFO_REQ_CA

	// 토너먼트 정보 요청
	struct NET_MATCHING_GROUPTABLEINFO_REQ_CA : public NET_MATCHING_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwTornanmentID;

		NET_MATCHING_GROUPTABLEINFO_REQ_CA(const DWORD _dwDbNum,const DWORD _TornanmentID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_GROUPTABLEINFO_REQ_CA), MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_REQ_CA)
			, dwDbNum(_dwDbNum),dwTornanmentID(_TornanmentID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_GROUPTABLEINFO_REQ_CA) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_MATCHING_NOTIFY_AT_FIRST_AC : public NET_MATCHING_MSGPACK_MESSAGE
    {
        DWORD   dwDbNum;
        DWORD   dwScriptID;
        WORD    wContentsType;
        WORD    wContentsStatus;
        DWORD   dwContentsMapID;
        std::string strContentsMainType;
        std::string strContentsSubType;
        std::string strContentsName;
        std::string strContentsDetailInfo;
        std::string strContentsMissionInfo;
        std::string strContentsRewardInfo;
        WORD    wContentsDateType;
        MatchingSystem::VEC_MATCHING_TIME vecDateTimeInfo;

        bool    bCanObserve;
        bool    bCanTOTO;
        bool    bEnableInfoButton;
		bool	bJoinInstanceDungeon;

		DWORD	dwMaxJoinCount;

        MSGPACK_DEFINE(emType, dwDbNum, dwScriptID, wContentsType, wContentsStatus, dwContentsMapID, 
                        strContentsMainType, strContentsSubType, strContentsName, strContentsDetailInfo,
                        strContentsMissionInfo, strContentsRewardInfo, wContentsDateType, vecDateTimeInfo,
                        bCanObserve, bCanTOTO, bEnableInfoButton, bJoinInstanceDungeon, dwMaxJoinCount);

        NET_MATCHING_NOTIFY_AT_FIRST_AC()
             : NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_AT_FIRST_AC)
        {
        }

        NET_MATCHING_NOTIFY_AT_FIRST_AC(
            DWORD   _dwDbNum,
            DWORD   _dwScriptID,
            WORD    _wContentsType,
            WORD    _wContentsStatus,
            DWORD   _dwContentsMapID,
            const std::string&  _strContentsMainType,
            const std::string&  _strContentsSubType,
            const std::string&  _strContentsName,
            const std::string&  _strContentsDetailInfo,
            const std::string&  _strContentsMissionInfo,
            const std::string&  _strContentsRewardInfo,
            WORD    _wContentsDateType,
            const MatchingSystem::VEC_MATCHING_TIME& _vecTimeInfo,
            bool    _bCanObserve,
            bool    _bCanTOTO,
            bool    _bEnableInfoButton,
			bool	_bJoinInstanceDungeon,
			DWORD	_dwMaxJoinCount
            ) : NET_MATCHING_MSGPACK_MESSAGE(MatchingSystem::EMMESSAGE_MATCHING_NOTIFY_AT_FIRST_AC)
            , dwDbNum(_dwDbNum)
            , dwScriptID(_dwScriptID)
            , wContentsType(_wContentsType)
            , wContentsStatus(_wContentsStatus)
            , dwContentsMapID(_dwContentsMapID)
            , strContentsMainType(_strContentsMainType)
            , strContentsSubType(_strContentsSubType)
            , strContentsName(_strContentsName)
            , strContentsDetailInfo(_strContentsDetailInfo)
            , strContentsMissionInfo(_strContentsMissionInfo)
            , strContentsRewardInfo(_strContentsRewardInfo)
            , wContentsDateType(_wContentsDateType)
            , vecDateTimeInfo(_vecTimeInfo)
            , bCanObserve(_bCanObserve)
            , bCanTOTO(_bCanTOTO)
            , bEnableInfoButton(_bEnableInfoButton)
			, bJoinInstanceDungeon(_bJoinInstanceDungeon)
			, dwMaxJoinCount(_dwMaxJoinCount)
        {
        };
    };

	// 토너먼트 클라 리그 정보
	struct NET_MATCHING_GROUPTABLEINFO_OUTOFDATE: public NET_MATCHING_MESSAGE
	{
		DWORD LegeNum;
		NET_MATCHING_GROUPTABLEINFO_OUTOFDATE(
			DWORD _LegeNum)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_GROUPTABLEINFO_OUTOFDATE),
			MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_OUTOFDATE_AC),
			LegeNum(_LegeNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_GROUPTABLEINFO_OUTOFDATE) <= NET_DATA_BUFSIZE);
		}
	};


	
	// 토너먼트 클라 리그 정보
	struct NET_MATCHING_GROUPTABLEINFO_NODE: public NET_MATCHING_MESSAGE
	{
		DWORD dwGROUPID;
		char  GroupName[CHAR_SZNAME];
		DWORD Position;
		DWORD LegeNum;
		DWORD MatchingInstanceNum;
		NET_MATCHING_GROUPTABLEINFO_NODE(
			DWORD _dwGROUPID,
			std::string& _GroupName,
			DWORD _Position,
			DWORD _MatchingInstanceNum,
			DWORD _LegeNum
			)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_GROUPTABLEINFO_NODE),
			MatchingSystem::EMMESSAGE_MATCHING_GROUPTABLEINFO_NODE_AC),
			dwGROUPID(_dwGROUPID),Position(_Position),LegeNum(_LegeNum)
			,MatchingInstanceNum(_MatchingInstanceNum)
		{
			StringCchCopy(GroupName, CHAR_SZNAME, _GroupName.c_str());

			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_GROUPTABLEINFO_NODE) <= NET_DATA_BUFSIZE);
		}
	};


	// 토너먼트 클라이언트에게 알릴사항
	struct NET_MATCHING_INFO_UPDATE_AC : public NET_MATCHING_MESSAGE
	{
		DWORD MapStatus;
		DWORD ScriptID;

		NET_MATCHING_INFO_UPDATE_AC(
			DWORD		 _ScriptID,
			DWORD		 _MapStatus)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_INFO_UPDATE_AC),
			MatchingSystem::EMMESSAGE_MATCHING_INFO_UPDATE_AC),
			ScriptID(_ScriptID),MapStatus(_MapStatus)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_INFO_UPDATE_AC) <= NET_DATA_BUFSIZE);
		}
	};

	// 토너먼트 클라이언트에게 알릴사항
	struct NET_MATCHING_INFO_UPDATE : public NET_MATCHING_MESSAGE
	{
		DWORD MapStatus;
		DWORD ScriptID;

		NET_MATCHING_INFO_UPDATE(
			DWORD		 _ScriptID,
			DWORD		 _MapStatus)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_INFO_UPDATE),
			MatchingSystem::EMMESSAGE_MATCHING_INFO_UPDATE),
			ScriptID(_ScriptID),MapStatus(_MapStatus)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_INFO_UPDATE) <= NET_DATA_BUFSIZE);
		}
	};

	//


	// 토너먼트 클라이언트에게 알릴사항
	struct NET_MATCHING_INFO_USERDATA_AC : public NET_MATCHING_MESSAGE
	{
		DWORD MapStatus;
		DWORD ScriptID;

		NET_MATCHING_INFO_USERDATA_AC(
			DWORD		 _ScriptID,
			DWORD		 _MapStatus)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_INFO_USERDATA_AC),
			MatchingSystem::EMMESSAGE_MATCHING_INFO_CLIENT_AC),
			ScriptID(_ScriptID),MapStatus(_MapStatus)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_INFO_USERDATA_AC) <= NET_DATA_BUFSIZE);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////FIELD INSTANCE SEND RESUALT FOR NEXT MATCHING///////////
	//////////////////////////////////////////////////////////////////////////

	struct NET_MATCHING_RESUALT_SEND_START_FA : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		NET_MATCHING_RESUALT_SEND_START_FA(const InstanceSystem::InstanceMapID _instanceMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_SEND_START_FA),
			MatchingSystem::EMMESSAGE_MATCHING_RESUALT_SEND_START_FA),
			TornanmentMapID(_instanceMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_SEND_START_FA) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCHING_RESUALT_SEND_END_FA : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		NET_MATCHING_RESUALT_SEND_END_FA(const InstanceSystem::InstanceMapID _instanceMapID)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_SEND_END_FA),
			MatchingSystem::EMMESSAGE_MATCHING_RESUALT_SEND_END_FA),
			TornanmentMapID(_instanceMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_SEND_END_FA) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCHING_RESUALT_SCORE_FA : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		char szluavarName[CHAR_SZNAME];

		char LuaType;
		//lua basic Type 참조 (lua.h)
		char LuavalueChar[CHAR_SZNAME];
		double luavaluefloat;
		bool luavaluebool;

		NET_MATCHING_RESUALT_SCORE_FA(
			const InstanceSystem::InstanceMapID _instanceMapID
			,const char* _szluavarName
			,const double _luavaluefloat)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_SCORE_FA),
			MatchingSystem::EMMESSAGE_MATHCING_RESUALT_SCORE_FA),
			TornanmentMapID(_instanceMapID),LuaType(LUA_TNUMBER),luavaluefloat(_luavaluefloat)
		{
			StringCchCopy(szluavarName, CHAR_SZNAME, _szluavarName);
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_SCORE_FA) <= NET_DATA_BUFSIZE);
		}

		NET_MATCHING_RESUALT_SCORE_FA(
			const InstanceSystem::InstanceMapID _instanceMapID
			,const char* _szluavarName
			,const char* _LuavalueChar)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_SCORE_FA),
			MatchingSystem::EMMESSAGE_MATHCING_RESUALT_SCORE_FA),
			TornanmentMapID(_instanceMapID),LuaType(LUA_TSTRING)
		{
			StringCchCopy(szluavarName, CHAR_SZNAME, _szluavarName);
			StringCchCopy(LuavalueChar, CHAR_SZNAME, _LuavalueChar);
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_SCORE_FA) <= NET_DATA_BUFSIZE);
		}

		NET_MATCHING_RESUALT_SCORE_FA(
			const InstanceSystem::InstanceMapID _instanceMapID
			,const char* _szluavarName
			,const bool _luavaluebool)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_SCORE_FA), MatchingSystem::EMMESSAGE_MATHCING_RESUALT_SCORE_FA)
			,TornanmentMapID(_instanceMapID),LuaType(LUA_TBOOLEAN),luavaluebool(_luavaluebool)
		{
			StringCchCopy(szluavarName, CHAR_SZNAME, _szluavarName);
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_SCORE_FA) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCHING_RESUALT_FACTION_RESUALT_FA : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		const DWORD m_FactionID;
		const BOOL IsWinner;

		NET_MATCHING_RESUALT_FACTION_RESUALT_FA(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _FactionID,const BOOL _IsWinner)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_FACTION_RESUALT_FA), MatchingSystem::EMMESSAGE_MATCHING_RESUALT_FACTION_RESUALT)
			, m_FactionID(_FactionID), TornanmentMapID(_instanceMapID),IsWinner(_IsWinner)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_FACTION_RESUALT_FA) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_MATCHING_RESULT_CUSTOM_MSG_FA : public NET_MATCHING_MESSAGE
    {
        const InstanceSystem::InstanceMapID TornanmentMapID;
        const double param1;
        const double param2;
        const double param3;
        const double param4;

        NET_MATCHING_RESULT_CUSTOM_MSG_FA(const InstanceSystem::InstanceMapID _instanceMapID, const double _param1, const double _param2, const double _param3, const double _param4)
            : NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESULT_CUSTOM_MSG_FA), MatchingSystem::EMMESSAGE_MATCHING_RESULT_CUSTOM_MSG_FA)
            , TornanmentMapID(_instanceMapID), param1(_param1), param2(_param2), param3(_param3), param4(_param4)
        {
            MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_FACTION_RESUALT_FA) <= NET_DATA_BUFSIZE);
        }
    };

	struct NET_MATCHING_RESUALT_ERROR_FA : public NET_MATCHING_MESSAGE
	{
		const InstanceSystem::InstanceMapID TornanmentMapID;
		const DWORD emERROR;
		NET_MATCHING_RESUALT_ERROR_FA(const InstanceSystem::InstanceMapID _instanceMapID,DWORD _emERROR)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_RESUALT_ERROR_FA), MatchingSystem::EMMESSAGE_MATCHING_RESUALT_ERROR_FA)
			, TornanmentMapID(_instanceMapID),emERROR(_emERROR)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_RESUALT_ERROR_FA) <= NET_DATA_BUFSIZE);
		}
	};


/*
	// 토너먼트 매칭된 게임의 승자 알려주는 메시지
	struct NET_MATCHING_FINISH_MATCHED_GAME_FA : public NET_MATCHING_MESSAGE
	{
		
		const DWORD m_FactionID;
		const int Rating;
		const BOOL IsWinner;

		NET_MATCHING_FINISH_MATCHED_GAME_FA(const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _FactionID,const int _Rating, const BOOL _IsWinner)
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_FINISH_MATCHED_GAME_FA), MatchingSystem::EMMESSAGE_FINISH_MATCHED_GAME_FA)
			, m_FactionID(_FactionID), TornanmentMapID(_instanceMapID), Rating(_Rating), IsWinner(_IsWinner)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_FINISH_MATCHED_GAME_FA) <= NET_DATA_BUFSIZE);
		}
	};
	*/


	//////////////////////////////////////////////////////////////////////////
	///////////////////GM/COMMAND/MESSAGE/PROTOCALL///////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// 토너먼트 클라이언트에게 알릴사항
	struct NET_MATCHING_GM_COMMAND_CA : public NET_MATCHING_MESSAGE
	{
		MatchingSystem::EM_GM_MATCHING mstype;
		DWORD Param1;
		DWORD Param2;
		NET_MATCHING_GM_COMMAND_CA()
			: NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_GM_COMMAND_CA), MatchingSystem::EMMESSAGE_MATCHING_GM_COMMAND_CA)
		{
			Param1 = 0;
			Param2 = 0;
			MIN_STATIC_ASSERT(sizeof(NET_MATCHING_GM_COMMAND_CA) <= NET_DATA_BUFSIZE);
		}
	};

	


    //  (ex. 통합전장UI 정보보기 버튼, 토너먼트 대진표 요청)
    struct NET_MATCHING_CUSTOM_MSG_CA : public NET_MATCHING_MESSAGE
    {
        const DWORD dwGameScriptID;
        const DWORD dwParam1;
        const DWORD dwParam2;

        NET_MATCHING_CUSTOM_MSG_CA(const DWORD _dwGameScriptID, const DWORD _dwParam1, const DWORD _dwParam2)
            : NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_CUSTOM_MSG_CA), MatchingSystem::EMMESSAGE_MATCHING_CUSTOM_MSG_CA)
            , dwGameScriptID(_dwGameScriptID)
            , dwParam1(_dwParam1)
            , dwParam2(_dwParam2)
        {
            MIN_STATIC_ASSERT(sizeof(NET_MATCHING_CUSTOM_MSG_CA) <= NET_DATA_BUFSIZE);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    ///////////////////     Competition Rating       /////////////////////////
    //////////////////////////////////////////////////////////////////////////
    struct NET_MATCHING_COMPETITION_RATING_GETINFO_DA : public NET_MATCHING_MESSAGE
    {
        char strContentType[50];
        DWORD dwCharNum;
        int nRating;
        int nWinningStreak;
        int nPlayCount;
        __time64_t tLastPlayDate;

        DWORD dwScriptID;

        NET_MATCHING_COMPETITION_RATING_GETINFO_DA()
            : NET_MATCHING_MESSAGE(sizeof(NET_MATCHING_COMPETITION_RATING_GETINFO_DA), MatchingSystem::EMMESSAGE_COMPETITION_RATING_GETINFO_DA)
        {
            MIN_STATIC_ASSERT(sizeof(NET_MATCHING_COMPETITION_RATING_GETINFO_DA) <= NET_DATA_BUFSIZE);
        }
    };

	/// 원래의 란 시스템에 맞춰서 패킷모니터링에 노출시키기 위해서;
	/// 새로운 헤더를 정의한다;

	// Instance System 의 모든 패킷의 헤더;
	// 사용할 수 있는 패킷이 아니다;
	struct NET_MATCHING_MESSAGE_HEADER : public NET_MSG_GENERIC
	{
		NET_MATCHING_MESSAGE_HEADER ( const DWORD _nSize
			, const EMNET_MSG _msg )
			: NET_MSG_GENERIC( _msg, _nSize )
		{

		}
	};

	struct NET_MATCHING_UPDATE_JOIN_COUNT_AC : public NET_MATCHING_MESSAGE_HEADER
	{
		DWORD dwMatchingBaseID;
		DWORD dwJoinCount;

		NET_MATCHING_UPDATE_JOIN_COUNT_AC ( )
			: NET_MATCHING_MESSAGE_HEADER ( sizeof( NET_MATCHING_UPDATE_JOIN_COUNT_AC )
			, NET_MSG_GCTRL_MATCHING_UPDATE_JOIN_COUNT_AC )
			, dwMatchingBaseID ( MAXDWORD )
			, dwJoinCount ( 0 )
		{

		}
	};

}

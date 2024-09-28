#pragma once

#include "../WorldBattle/WorldBattleDefine.h"
#include "MsgGuid.h"
#include "GLContrlBaseMsg.h"

enum EMCLUB_NEW_FB
{
	EMCLUB_NEW_FB_FAIL			= 0,	// 일반오류.
	EMCLUB_NEW_FB_OK			= 1,	// 성공.
	EMCLUB_NEW_FB_ALREADY		= 2,	// 이미 다른 클럽에 가입되어 있음.
	EMCLUB_NEW_FB_NOTMASTER		= 3,	// 파티 마스터만 신청 가능.
	EMCLUB_NEW_FB_NOTMEMBER		= 4,	// 파티원 부족.
	EMCLUB_NEW_FB_NOTEXIST_MEM	= 5,	// 파티원이 근처에 없음.
	EMCLUB_NEW_FB_MEM_ALREADY	= 6,	// 이미 다른 클럽에 가입되어 있음.
	EMCLUB_NEW_FB_MASTER_LVL	= 7,	// 마스터의 레벨 부족.
	EMCLUB_NEW_FB_AVER_LIVING	= 8,	// 평균 생활점수 부족.
	EMCLUB_NEW_FB_NOTMONEY		= 9,	// 신청비용 부족.
	EMCLUB_NEW_FB_NAME			= 10,	// 클럽 이름 오류.
	EMCLUB_NEW_FB_BADNAME		= 11,	// 클럽 이름 오류.
	EMCLUB_NEW_FB_OTHERSCHOOL	= 12,	// 다른 학원생과 클럽 결성 못함.
	EMCLUB_NEW_FB_THAICHAR_ERROR = 13,  // 태국어 문자 조합 에러.
	EMCLUB_NEW_FB_VNCHAR_ERROR = 14,	// 베트남 문자 조합 에러.
};

enum EMCLUB_DIS_FB
{
	EMCLUB_DIS_FB_FAIL			= 0,	// 일반오류.
	EMCLUB_DIS_FB_OK			= 1,	// 해산 예약.
	EMCLUB_DIS_FB_CANCEL		= 2,	// 해산 취소.
	EMCLUB_DIS_FB_CLUBBATTLE	= 3,	// 클럽배틀중이라 예약 못함
};

enum EMCLUB_MEMBER_REQ_FB
{
	EMCLUB_MEMBER_REQ_FB_FAIL			  = 0,	//! 일반오류.
	EMCLUB_MEMBER_REQ_FB_OK				  = 1,	//! 성공.
	EMCLUB_MEMBER_REQ_FB_REFUSE			  = 2,	//! 상대방이 거절.
	EMCLUB_MEMBER_REQ_FB_ALREADY		  = 3,	//! 이미 가입되어 있는 사람.
	EMCLUB_MEMBER_REQ_FB_MAXNUM			  = 4,	//! 최대 가입수.
	EMCLUB_MEMBER_REQ_FB_SECEDED		  = 5,	//! 자진탈퇴한 사람이여서 가입 제한 시간 설정됨.
	EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL	  = 6,	//! 다른 학원생은 클럽에 참여 못함.
	EMCLUB_MEMBER_REQ_FB_CLUBBATTLE		  = 7,	//! 배틀중에는 멤버를 받을수 없다.
    EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY = 8,  //! 국가가 다르면 클럽원이 될수 없다.
	EMCLUB_MEMBER_REQ_FB_NONEXISTENT,			//! 초대 신청을 보낸 클럽마스터가 접속 종료한 이후부터 가입될 수 없다 [1/3/2013 gbgim]
};

enum EMCLUB_MEMBER_DEL_FB
{
	EMCLUB_MEMBER_DEL_FB_FAIL	= 0,	// 일반오류
	EMCLUB_MEMBER_DEL_FB_OK		= 1,	// 성공
    EMCLUB_MEMBER_DEL_FB_GRADE	= 2,	// 실패, 자신보다 상위 계급은 제명 불가능
};

enum EMCLUB_AUTHORITY_REQ_FB
{
	EMCLUB_AUTHORITY_REQ_FB_FAIL		= 0,	//	일반오류
	EMCLUB_AUTHORITY_REQ_FB_OK			= 1,	//	성공
	EMCLUB_AUTHORITY_REQ_FB_REFUSE		= 2,	//	거절
	EMCLUB_AUTHORITY_REQ_FB_NOMASTER	= 3,	//	마스터가 아님
	EMCLUB_AUTHORITY_REQ_FB_NOONLINE	= 4,	//	상대 유저 접속 안했음
	EMCLUB_AUTHORITY_REQ_FB_ALLIANCE	= 5,	//	동맹 상태임
	EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE	= 6,	//	클럽배틀 중
	EMCLUB_AUTHORITY_REQ_FB_CONFING		= 7,	//	대련중
	EMCLUB_AUTHORITY_REQ_FB_TIME		= 8,	//	시간제한
	EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE	= 9,	//	선도전중
	EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE	= 10,	//	CDM진행중
	EMCLUB_AUTHORITY_REQ_FB_OFF			= 11,	//	클럽마스터 위임 OFF
};

enum EMCLUB_MARK_INFO_FB
{
	EMCLUB_MARK_INFO_FB_FAIL	= 0,	//	일반오류.
	EMCLUB_MARK_INFO_FB_OK		= 1,	//	성공.
};

enum EMCLUB_MARK_CHANGE_FB
{
	EMCLUB_MARK_CHANGE_FB_FAIL	= 0,	//	일반 오류.
	EMCLUB_MARK_CHANGE_FB_OK	= 1,	//	성공.
    EMCLUB_CHANGE_MARK_LIMIT_TIME = 2, // 클럽 마크 변경 제한 시간으로 인한 오류.
};

enum EMCLUB_RANK_FB
{
	EMCLUB_RANK_FB_FAIL			= 0,	// 일반 오류.
	EMCLUB_RANK_FB_OK			= 1,	// 성공.
	EMCLUB_RANK_FB_MAX			= 2,	// 최고 랭크 도달.
	EMCLUB_RANK_FB_MASTER_LVL	= 3,	// 마스터의 레벨 부족.
	EMCLUB_RANK_FB_AVER_LIVING	= 4,	// 평균 생활점수 부족.
	EMCLUB_RANK_FB_NOTMONEY		= 5,	// 신청비용 부족.
};

enum EMCLUB_MEMBER_NICK_FB
{
	EMCLUB_MEMBER_NICK_FB_FAIL			 = 0, //! 일반 오류
	EMCLUB_MEMBER_NICK_FB_OK			 = 1, //! 성공
	EMCLUB_MEMBER_NICK_FB_BADNAME		 = 2, //! 사용금지된 이름
	EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR = 3, //! 태국어문자 조합 에러
	EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR	 = 4,
    EMCLUB_MEMBER_NICK_FB_WB_ERROR       = 5,
    EMCLUB_MEMBER_NICK_FB_KR_ERROR       = 6,
};

enum EMCLUB_MEMBER_SECEDE_FB
{
	EMCLUB_MEMBER_SECEDE_FB_FAIL	= 0,	//	일반 오류.
	EMCLUB_MEMBER_SECEDE_FB_OK		= 1,	//	성공.
};

enum
{
	EMMAXCLUBMEMBER_NEW = 8
};

enum EMCLUB_NOTICE_FB
{
	EMCLUB_NOTICE_FB_FAIL			= 0,	//	일반오류.
	EMCLUB_NOTICE_FB_OK				= 1,	//	성공.
	EMCLUB_NOTICE_FB_NOTMATER		= 2,	//	클럽 마스터가 아님.
};

enum EMCDCERTIFY
{
	EMCDCERTIFY_FAIL			= 0,	//	일반오류.
	EMCDCERTIFY_OK				= 1,	//	인증 시작.
	EMCDCERTIFY_BADNPC			= 2,	//	npc 오류.
	EMCDCERTIFY_NOTMASTER		= 3,	//	클럽 마스터가 아님.
	EMCDCERTIFY_AREADYCERTIFY	= 4,	//	이미 인증중.
	EMCDCERTIFY_OTHERCERTIFING	= 5,	//	이미 다른 사람이 인증중.
	EMCDCERTIFY_AREADYGUID		= 6,	//	이미 다른 선도 지역 관리.
	EMCDCERTIFY_NOTTIME			= 7,	//	인증이 가능한 시간이 아님.
	EMCDCERTIFY_DISTANCE		= 8,	//	거리가 멀리 떨어져있음.
	EMCDCERTIFY_NOTITEM			= 9,	//	인증용 CD가 없음.
	EMCDCERTIFY_NOTCONDITION	= 10,	//	npc 오류.
	EMCDCERTIFY_AREADY			= 11,	//	이미 인증 되어 있습니다.
	EMCDCERTIFY_HAVEGUIDCLUB	= 12,	// 동맹클럽중에 선도클럽이 있음

	EMCDCERTIFY_ING_TIMEOUT		= 13,	//	인증중 허용거리 초과.
	EMCDCERTIFY_ING_DISTANCE	= 14,	//	인증중 허용거리 초과.
	EMCDCERTIFY_ING_DIE			= 15,	//	사망.

	EMCDCERTIFY_COMPLETE		= 16,	//	인증 완료.
};

enum EMGUIDCOMMISSION_FB
{
	EMGUIDCOMMISSION_FB_FAIL	= 0,	//	일반오류.
	EMGUIDCOMMISSION_FB_OK		= 1,	//	성공.
	EMGUIDCOMMISSION_FB_RANGE	= 2,	//	허용영역 초과.
	EMGUIDCOMMISSION_FB_NOTCLUB	= 3,	//	선도 클럽 마스터가 아님.
	EMGUIDCOMMISSION_FB_ALREADY	= 4,	//	이미 예약되어 있음.
	EMGUIDCOMMISSION_FB_BATTLE	= 5,	//	선도전 진행중에는 수수료 변경할 수 없습니다.
};


enum EMCLUBSUBMASTER_FB
{
	EMCLUBSUBMASTER_FB_FAIL		= 0,
	EMCLUBSUBMASTER_FB_OK		= 1,
	EMCLUBSUBMASTER_FB_NOTMASTER= 2,
	EMCLUBSUBMASTER_FB_RESET	= 3,
	EMCLUBSUBMASTER_FB_BATTLE	= 4,
	EMCLUBSUBMASTER_FB_CDM		= 5,
	EMCLUBSUBMASTER_FB_CDM_MEM	= 6,	//	CDM 멤버수 초과
};

// 동맹가입/결성
enum EMCLUB_ALLIANCE_REQ_FB
{
	EMCLUB_ALLIANCE_REQ_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_REQ_FB_OK			= 1,
	EMCLUB_ALLIANCE_REQ_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF		= 3,
	EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER	= 4,
	EMCLUB_ALLIANCE_REQ_FB_ALREADY		= 5,
	EMCLUB_ALLIANCE_REQ_FB_SAMECLUB		= 6,
	EMCLUB_ALLIANCE_REQ_FB_MAX			= 7,
	EMCLUB_ALLIANCE_REQ_FB_REFUSE		= 8,
	EMCLUB_ALLIANCE_REQ_FB_DISTIME		= 9,
	EMCLUB_ALLIANCE_REQ_FB_SECTIME		= 10,
	EMCLUB_ALLIANCE_REQ_FB_TOSECTIME	= 11,
	EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL	= 12,
	EMCLUB_ALLIANCE_REQ_FB_BOTHGUID		= 13,
	EMCLUB_ALLIANCE_REQ_FB_GUIDMAP		= 14,
	EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION	= 15,	// 신청자가 해체예정일때
	EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2	= 16,	// 상대방이 해체예정일때
	EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE		= 17,	// 클럽 배틀이 진행중일때 ( 나 )
	EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE	= 18,	// 클럽 배틀이 진행중일때 ( 상대 )
    EMCLUB_ALLIANCE_REQ_FB_NO_AUTHORITY = 19, // 클럽 동맹 신청 권한이 없음.
};

// 클럽배틀
enum EMCLUB_BATTLE_REQ_FB
{
	EMCLUB_BATTLE_REQ_FB_FAIL			= 0,	// 일반적인 오류
	EMCLUB_BATTLE_REQ_FB_OK				= 1,
	EMCLUB_BATTLE_REQ_FB_NOTMASTER		= 2,	// 마스터가 아님	
	EMCLUB_BATTLE_REQ_FB_TARNOTMASTER	= 3,	// 상대방이 마스터가 아님
	EMCLUB_BATTLE_REQ_FB_ALREADY		= 4,	// 이미 클럽배틀 중	
	EMCLUB_BATTLE_REQ_FB_ALREADY2		= 5,	// 이미 클럽배틀 중	
	EMCLUB_BATTLE_REQ_FB_MAX			= 6,	// 동시진행 갯수 초과
	EMCLUB_BATTLE_REQ_FB_TARMAX			= 7,	// 상대편 동시진행 갯수 초과
	EMCLUB_BATTLE_REQ_FB_REFUSE			= 8,	// 상대편 거절
	EMCLUB_BATTLE_REQ_FB_GUIDMAP		= 9,	// 선도전 맵에서 안됨
	EMCLUB_BATTLE_REQ_FB_DISSOLUTION	= 10,	// 신청자가 해체예정일때
	EMCLUB_BATTLE_REQ_FB_DISSOLUTION2	= 11,	// 상대방이 해체예정일때
	EMCLUB_BATTLE_REQ_FB_ALLIANCE		= 12,	// 같은 동맹 소속일때
	EMCLUB_BATTLE_REQ_FB_RANK			= 13,	// 클럽랭크 미달
	EMCLUB_BATTLE_REQ_FB_TARRANK		= 14,	// 상대 클럽랭크 미달
	EMCLUB_BATTLE_REQ_FB_CONFT			= 15,	// 대련중
	EMCLUB_BATTLE_REQ_FB_TARCONFT		= 16,	// 상대 대련중
	EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE	= 17,	// 선도전 시간중에는 시작할수 없다.
	EMCLUB_BATTLE_REQ_FB_TIMEMIN		= 18,	// 진행시간이 작음
	EMCLUB_BATTLE_REQ_FB_TIMEMAX		= 19,	// 진행시간이 김
	EMCLUB_BATTLE_REQ_FB_CDMBATTLE		= 20,	// CDM시간중에는 안됨
	EMCLUB_BATTLE_REQ_FB_CDMMAP			= 21,	// 클럽데스매치존에서는 안됌
    EMCLUB_BATTLE_REQ_FB_NO_AUTHORITY	= 22,	// 클럽배틀 신청 권한이 없음.
};

// 클럽배틀
enum EMALLIANCE_BATTLE_REQ_FB
{
	EMALLIANCE_BATTLE_REQ_FB_FAIL			= 0,	// 일반적인 오류
	EMALLIANCE_BATTLE_REQ_FB_OK				= 1,
	EMALLIANCE_BATTLE_REQ_FB_NOTMASTER		= 2,	// 마스터가 아님	
	EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER	= 3,	// 상대방이 마스터가 아님
	EMALLIANCE_BATTLE_REQ_FB_ALREADY		= 4,	// 이미 클럽배틀 중	
	EMALLIANCE_BATTLE_REQ_FB_ALREADY2		= 5,	// 이미 동맹배틀 중	
	EMALLIANCE_BATTLE_REQ_FB_MAX			= 6,	// 동시진행 갯수 초과
	EMALLIANCE_BATTLE_REQ_FB_TARMAX			= 7,	// 상대편 동시진행 갯수 초과
	EMALLIANCE_BATTLE_REQ_FB_REFUSE			= 8,	// 상대편 거절
	EMALLIANCE_BATTLE_REQ_FB_GUIDMAP		= 9,	// 선도전 맵에서 안됨
	EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION	= 10,	// 신청자가 해체예정일때
	EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2	= 11,	// 상대방이 해체예정일때
	EMALLIANCE_BATTLE_REQ_FB_ALLIANCE		= 12,	// 같은 동맹 소속일때
	EMALLIANCE_BATTLE_REQ_FB_RANK			= 13,	// 클럽랭크 미달
	EMALLIANCE_BATTLE_REQ_FB_TARRANK		= 14,	// 상대 클럽랭크 미달
	EMALLIANCE_BATTLE_REQ_FB_CONFT			= 15,	// 대련중
	EMALLIANCE_BATTLE_REQ_FB_TARCONFT		= 16,	// 상대 대련중
	EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE	= 17,	// 선도전 시간중에는 시작할수 없다.
	EMALLIANCE_BATTLE_REQ_FB_TIMEMIN		= 18,	// 진행시간이 작음
	EMALLIANCE_BATTLE_REQ_FB_TIMEMAX		= 19,	// 진행시간이 김
	EMALLIANCE_BATTLE_REQ_FB_CDMBATTLE		= 20,	// CDM진행중에는 안됌
	EMALLIANCE_BATTLE_REQ_FB_CDMMAP			= 21,	// CDM 맵에서 안됨
    EMALLIANCE_BATTLE_REQ_FB_NO_AUTHORITY   = 22,   // 클럽 배틀 신청 권한 없음.
};

// 클럽 배틀 휴전
enum EMCLUB_BATTLE_ARMISTICE_REQ_FB
{
	EMCLUB_BATTLE_ARMISTICE_FB_FAIL			= 0,	//! 일반적인 오류
	EMCLUB_BATTLE_ARMISTICE_FB_OK			= 1,	//! 성공
	EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER	= 2,	//! 마스터가 아님
	EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER	= 3,	//! 상대 마스터가 비 접속중
	EMCLUB_BATTLE_ARMISTICE_FB_REFUSE		= 4,	//! 거절
	EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP		= 5,	//! 선도전 맵
	EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE		= 6,	//! 배틀중이 아님
	EMCLUB_BATTLE_ARMISTICE_FB_DISTIME		= 7,	//! 배틀 중지 불과
	EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP		= 8,	//! 클럽데스매치 존
    EMCLUB_BATTLE_NO_AUTHORITY              = 9,    //! 권한없음
};

//! 동맹 배틀 휴전
enum EMALLIANCE_BATTLE_ARMISTICE_REQ_FB
{
	EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL			= 0,	//! 일반적인 오류
	EMALLIANCE_BATTLE_ARMISTICE_FB_OK			= 1,	//! 성공
	EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER	= 2,	//! 마스터가 아님
	EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER	= 3,	//! 상대 마스터가 비 접속중
	EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE		= 4,	//! 거절
	EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP		= 5,	//! 선도전 맵
	EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE		= 6,	//! 배틀중이 아님
	EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME		= 7,	//! 배틀 중지 불과
	EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP		= 8,	//! 클럽데스매치 존
    EMALLIANCE_BATTLE_NO_AUTHORITY              = 9,    //! 권한 없음
};



//! 클럽 배틀 항복
enum EMCLUB_BATTLE_SUBMISSION_REQ_FB
{
	EMCLUB_BATTLE_SUBMISSION_FB_FAIL		 = 0, //! 일반적인 오류
	EMCLUB_BATTLE_SUBMISSION_FB_OK			 = 1, //! 성공
	EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER	 = 2, //! 마스터가 아님
	EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP		 = 3, //! 선도전 맵
	EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE	 = 4, //! 배틀중이 아님
	EMCLUB_BATTLE_SUBMISSION_FB_DISTIME		 = 5, //! 배틀 중지 불가
	EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP		 = 6, //! CDM맵
    EMCLUB_BATTLE_SUBMISSION_FB_NO_AUTHORITY = 7, //! 권한없음
};

//! 동맹 배틀 항복
enum EMALLIANCE_BATTLE_SUBMISSION_REQ_FB
{
	EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL		 = 0, //! 일반적인 오류
	EMALLIANCE_BATTLE_SUBMISSION_FB_OK			 = 1, //! 성공
	EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER	 = 2, //! 마스터가 아님
	EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP		 = 3, //! 선도전 맵
	EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE	 = 4, //! 배틀중이 아님
	EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME		 = 5, //! 배틀 중지 불과
	EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP		 = 6, //! CDM맵
    EMALLIANCE_BATTLE_SUBMISSION_FB_NO_AUTHORITY = 7, //! 권한없음
};

// 클럽 배틀 종료
enum EMCLUB_BATTLE_OVER_FB
{
	EMCLUB_BATTLE_OVER_DRAW					= 0,	// 비김
	EMCLUB_BATTLE_OVER_WIN					= 1,	// 승리
	EMCLUB_BATTLE_OVER_LOSE					= 2,	// 패배
	EMCLUB_BATTLE_OVER_ARMISTICE			= 3,	// 휴전
	EMCLUB_BATTLE_OVER_SUBMISSION			= 4,	// 항복
	EMCLUB_BATTLE_OVER_TARSUBMISSION		= 5,	// 항복 받는 대상
};


// 동맹클럽 제명
enum EMCLUB_ALLIANCE_DEL_FB
{
	EMCLUB_ALLIANCE_DEL_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_DEL_FB_OK			= 1,
	EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF_OK	= 2,
	EMCLUB_ALLIANCE_DEL_FB_NOTMASTER	= 3,
	EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF		= 4,
	EMCLUB_ALLIANCE_DEL_FB_GUIDMAP		= 5,
	EMCLUB_ALLIANCE_DEL_FB_BATTLE		= 6, //! 배틀중일때
    EMCLUB_ALLIANCE_DEL_FB_NO_AUTH      = 7, //! 권한없음
    EMCLUB_ALLIANCE_DEL_FB_NO_MYCLUB    = 8, //! 자기 클럽을 선택함.
};

// 동맹 탈퇴
enum EMCLUB_ALLIANCE_SEC_FB
{
	EMCLUB_ALLIANCE_SEC_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_SEC_FB_OK			= 1,
	EMCLUB_ALLIANCE_SEC_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_SEC_FB_ALLIANCE		= 3,
	EMCLUB_ALLIANCE_SEC_FB_GUIDMAP		= 4,
	EMCLUB_ALLIANCE_SEC_FB_BATTLE		= 5,	//	배틀중일때.
};

// 동맹 해체
enum EMCLUB_ALLIANCE_DIS_FB
{
	EMCLUB_ALLIANCE_DIS_FB_FAIL			= 0,
	EMCLUB_ALLIANCE_DIS_FB_OK			= 1,
	EMCLUB_ALLIANCE_DIS_FB_NOTMASTER	= 2,
	EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF		= 3,
	EMCLUB_ALLIANCE_DIS_FB_GUIDMAP		= 4,
	EMCLUB_ALLIANCE_DIS_FB_BATTLE		= 5,	//	배틀중일때.
};

// 클럽 이름 변경
enum EMCLUB_RENAME_FB
{
	CLUB_RENAME_FB_OK = 0, 
	CLUB_RENAME_FB_WRONG_ITEM = 1, 
	CLUB_RENAME_FB_AUTH = 2, 
	CLUB_RENAME_FB_NONAME = 3, 
	CLUB_RENAME_FB_TOO_SHORT = 4, 
	CLUB_RENAME_FB_TOO_LONG = 5, 
	CLUB_RENAME_FB_BLANK = 6, 
	CLUB_RENAME_FB_BAD_NAME = 7, 
	CLUB_RENAME_FB_NAME_DUP = 8, 
	CLUB_RENAME_FB_CLUB_BATTLE = 9, 
};

namespace GLMSG
{
    //! 클럽 생성
	struct SNET_CLUB_NEW : public NET_MSG_GENERIC
	{
		DWORD dwNpcGlobID;
		char  szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW()
			: dwNpcGlobID(0)
		{
			dwSize = sizeof(SNET_CLUB_NEW);
			nType = NET_MSG_GCTRL_CLUB_NEW;
			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NEW)<=NET_DATA_BUFSIZE);
		}

        void SetClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }
	};

    //! 클럽 생성 Lobby
    struct NET_CLUB_NEW_LOBBY_CA : public NET_MSG_GENERIC
    {
        DWORD m_ChaDbNum;
        char m_ClubName[CHAR_SZNAME];
        char m_ChaName[CHAR_SZNAME];
        sc::MGUID m_Guid;

        NET_CLUB_NEW_LOBBY_CA(DWORD ChaDbNum, const std::string& ClubName, const std::string& ChaName)
            : m_ChaDbNum(ChaDbNum)
        {
            dwSize = sizeof(NET_CLUB_NEW_LOBBY_CA);
            nType = NET_MSG_CLUB_NEW_LOBBY_CA;
            m_Guid = msg_guid::GUID2;
            
            StringCchCopy(m_ClubName, CHAR_SZNAME, ClubName.c_str());
            StringCchCopy(m_ChaName,  CHAR_SZNAME, ChaName.c_str());

            MIN_STATIC_ASSERT(sizeof(NET_CLUB_NEW_LOBBY_CA)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_CLUB_NEW_2AGT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW_2AGT()
		{
			dwSize = sizeof(SNET_CLUB_NEW_2AGT);
			nType = NET_MSG_GCTRL_CLUB_NEW_2AGT;
			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NEW_2AGT)<=NET_DATA_BUFSIZE);
		}

        void SetClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }
	};

	struct SNET_CLUB_NEW_DB2AGT : public NET_MSG_GENERIC
	{
        //enum { BUF_SIZE = NET_DATA_BUFSIZE-(sizeof(NET_MSG_GENERIC)+sizeof(EMCLUB_NEW_FB)+sizeof(DWORD)+sizeof(DWORD)+sizeof(char)*CHAR_SZNAME+sizeof(wb::EM_COUNTRY)), };       
		EMCLUB_NEW_FB emFB;
        DWORD m_ClientSlot;
		DWORD m_MasterDbNum;
		DWORD m_ClubDbNum;
        wb::EM_COUNTRY m_Country;
		char m_ClubName[CHAR_SZNAME];
        char m_ChaName[CHAR_SZNAME];
        bool m_bLobby; //! Lobby 에서 생성할때
		//BYTE		  m_buffer[BUF_SIZE];

        SNET_CLUB_NEW_DB2AGT(
            DWORD ClientSlot,
            DWORD MasterDbNum,
            DWORD ClubDbNum,
            wb::EM_COUNTRY Country,
            const char* ChaName,
            bool bLobby)
			: m_ClientSlot(ClientSlot)
            , emFB(EMCLUB_NEW_FB_FAIL)
			, m_MasterDbNum(MasterDbNum)
			, m_ClubDbNum(ClubDbNum)
            , m_Country(Country/*wb::UNKNOWN_COUNTRY*/)
            , m_bLobby(bLobby)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_NEW_DB2AGT);
			nType = NET_MSG_GCTRL_CLUB_NEW_DB2AGT;
			memset (m_ClubName, 0, sizeof(char) * CHAR_SZNAME);			
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NEW_DB2AGT)<=NET_DATA_BUFSIZE);

            StringCchCopy(m_ChaName, CHAR_SZNAME, ChaName);
		}

        void SetClubName(const std::string& _ClubName)
        {            
            StringCchCopy(m_ClubName, CHAR_SZNAME, _ClubName.c_str());
        }
	};

	struct SNET_CLUB_NEW_2FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCLUB_NEW_FB		emFB;

		DWORD				dwCLUBID;
		char				szClubName[CHAR_SZNAME];

		SNET_CLUB_NEW_2FLD () 
			: emFB(EMCLUB_NEW_FB_FAIL)
			, dwCLUBID(0)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_NEW_2FLD);
			nType = NET_MSG_GCTRL_CLUB_NEW_2FLD;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NEW_2FLD)<=NET_DATA_BUFSIZE);
		}

        void SetName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }
	};

	struct NET_CLUB_NEW_FB_FC : public NET_MSG_GENERIC
	{		
		EMCLUB_NEW_FB emFB;
		char szClubName[CHAR_SZNAME];
        bool m_bLobby;

		NET_CLUB_NEW_FB_FC(bool bLobby)
            : emFB(EMCLUB_NEW_FB_FAIL)
            , m_bLobby(bLobby)
		{
			dwSize = (DWORD) sizeof(NET_CLUB_NEW_FB_FC);
			nType = NET_MSG_GCTRL_CLUB_NEW_FB;
			memset (szClubName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_NEW_FB_FC)<=NET_DATA_BUFSIZE);
		}

        void SetClubName(const char* ClubName)
        {
            if (ClubName)
                StringCchCopy(szClubName, CHAR_SZNAME, ClubName);
        }
	};

	struct NET_NEW_CLUB_INFO_AF : public NET_MSG_GENERIC
	{
		DWORD		dwClubID;
        wb::EM_COUNTRY m_Country;
		char		szClubName[CHAR_SZNAME];

		DWORD		dwMasterID;
		char		szMasterName[CHAR_SZNAME];
        WORD        wMasterLevel;
		DWORD		dwCDCertifior;

		DWORD		dwRank;
		DWORD		dwMarkVER;

		__time64_t	tOrganize;
		__time64_t	tDissolution;

		DWORD		dwMemberNum;
		DWORD		aMembers[EMMAXCLUBMEMBER_NEW];
		char		szMemberName[EMMAXCLUBMEMBER_NEW][CHAR_SZNAME];

        NET_NEW_CLUB_INFO_AF(wb::EM_COUNTRY Country)
			: m_Country(Country)
            , dwClubID(0)
			, dwMasterID(0)
            , wMasterLevel(0)
			, dwCDCertifior(0)
			, tOrganize(0)
			, tDissolution(0)
			, dwRank(0)
			, dwMarkVER(0)
			, dwMemberNum(0)
		{
			dwSize = (DWORD) sizeof(NET_NEW_CLUB_INFO_AF);			
			nType = NET_MSG_GCTRL_CLUB_INFO_2FLD;
			MIN_STATIC_ASSERT(sizeof(NET_NEW_CLUB_INFO_AF)<=NET_DATA_BUFSIZE);
			
			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szMasterName, 0, sizeof(char) * CHAR_SZNAME);
			memset(aMembers, 0, sizeof(DWORD) * EMMAXCLUBMEMBER_NEW );			
			
			for (int i=0; i<EMMAXCLUBMEMBER_NEW; ++i)
			{
				memset(szMemberName[i], 0, sizeof(char) * CHAR_SZNAME);
			}
		}

        void SetClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }

        void SetMasterName(const char* szName)
        {
            if (szName)
                StringCchCopy(szMasterName, CHAR_SZNAME, szName);
        }

        void SetMemberName(size_t Index, const char* Name)
        {
            if (Index >= EMMAXCLUBMEMBER_NEW || !Name)
                return;
            else
                StringCchCopy(szMemberName[Index], CHAR_SZNAME, Name);
        }
	};

	struct SNET_CLUB_INFO_2CLT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwClubID;
		char				szClubName[CHAR_SZNAME];
		char				szNickName[CHAR_SZNAME];

		DWORD				dwMasterID;
		char				szMasterName[CHAR_SZNAME];
		DWORD				dwCDCertifior;

		DWORD				dwRank;
		DWORD				dwMarkVER;

		__time64_t			tOrganize;
		__time64_t			tDissolution;

		char				szNotice[EMCLUB_NOTICE_LEN];

		SNET_CLUB_INFO_2CLT () 
			: dwClubID(0)
			, dwMasterID(0)
			, dwCDCertifior(0)
			, dwRank(0)
			, dwMarkVER(0)
			, tOrganize(0)
			, tDissolution(0)
		{
			dwSize = sizeof(SNET_CLUB_INFO_2CLT);
			nType = NET_MSG_GCTRL_CLUB_INFO_2CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INFO_2CLT)<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szNickName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szMasterName, 0, sizeof(char) * CHAR_SZNAME);
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN));
		}

        void SetClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }
        
        void SetNickName(const char* szName)
        {
            if (szName)
                StringCchCopy(szNickName, CHAR_SZNAME, szName);
        }

        void SetMasterName(const char* szName)
        {
            if (szName)
                StringCchCopy(szMasterName, CHAR_SZNAME, szName);
        }

        void SetNotice(const char* szDesc)
        {
            if (szDesc)
                StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, szDesc);
        }
	};

	struct SNET_CLUB_INFO_DISSOLUTION : public NET_MSG_GENERIC
	{
		__time64_t			tDissolution;

		SNET_CLUB_INFO_DISSOLUTION () 
			: tDissolution( 0 ) 
		{
			dwSize = sizeof(SNET_CLUB_INFO_DISSOLUTION);
			nType = NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INFO_DISSOLUTION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SCLUBMEMBER
	{
		DWORD m_ChaDbNum;
		DWORD m_Flag;
        WORD  m_ChaLevel;
		char  m_Name[CHAR_SZNAME];
        __time64_t m_LastPlayTime;
        bool m_Online;

        SCLUBMEMBER();
		SCLUBMEMBER(
            DWORD ChaDbNum,
            DWORD Flags,
            WORD ChaLevel,
            const char* ChaName,
            const __time64_t& LastPlayTime,
            bool Online);

        void SetName(const char* ChaName)
        {
            if (ChaName)
                StringCchCopy(m_Name, CHAR_SZNAME, ChaName);
        }
	};

	struct SNET_CLUB_MEMBER_2CLT //: public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		enum { EMMAXMEMBER = 10 };		
		
        std::vector<GLCLUBMEMBER> m_Member; //[EMMAXMEMBER];

        MSGPACK_DEFINE(m_Member);

		SNET_CLUB_MEMBER_2CLT()
		{
			//dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
			//nType = NET_MSG_GCTRL_CLUB_MEMBER_2CLT;
			//MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_2CLT)<=NET_DATA_BUFSIZE);
		}

		bool ADDMEMBER(const GLCLUBMEMBER& Member)
		{
			if (m_Member.size() >= EMMAXMEMBER)
                return false;

            m_Member.push_back(Member);
			return true;
		}

		void RESET()
		{
			m_Member.clear();
		}
	};

	struct SNET_CLUB_INFO_BRD : public SNETPC_BROAD
	{
		DWORD				dwClubID;
		DWORD				dwMarkVer;
		char				szNickName[CHAR_SZNAME];
	
		SNET_CLUB_INFO_BRD () 
			: dwClubID(0)
			, dwMarkVer(0)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_INFO_BRD);
			nType = NET_MSG_GCTRL_CLUB_INFO_BRD;
			memset (szNickName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INFO_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INFO_MARK_BRD : public SNETPC_BROAD
	{
		DWORD				dwMarkVer;
	
		SNET_CLUB_INFO_MARK_BRD () 
			: dwMarkVer(0)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_INFO_MARK_BRD);
			nType = NET_MSG_GCTRL_CLUB_INFO_MARK_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INFO_MARK_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_INFO_NICK_BRD : public SNETPC_BROAD
	{
		char				szNickName[CHAR_SZNAME];
	
		SNET_CLUB_INFO_NICK_BRD () 
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_INFO_NICK_BRD);
			nType = NET_MSG_GCTRL_CLUB_INFO_NICK_BRD;
			memset (szNickName, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INFO_NICK_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION : public NET_MSG_GENERIC
	{
		bool				bCANCEL;

		SNET_CLUB_DISSOLUTION () 
			: bCANCEL(false)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_DISSOLUTION);
			nType = NET_MSG_GCTRL_CLUB_DISSOLUTION;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DISSOLUTION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION_2FLD : public NET_MSG_GENERIC
	{
		DWORD				dwClubID;
		__time64_t			tDissolution;

		SNET_CLUB_DISSOLUTION_2FLD () 
			: dwClubID(0)
			, tDissolution(0)
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_DISSOLUTION_2FLD);
			nType = NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DISSOLUTION_2FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DISSOLUTION_FB : public NET_MSG_GENERIC
	{
		EMCLUB_DIS_FB		emFB;
		__time64_t			tDissolution;

		SNET_CLUB_DISSOLUTION_FB () 
			: emFB(EMCLUB_DIS_FB_FAIL)
			, tDissolution(0)
		{
			dwSize = sizeof(SNET_CLUB_DISSOLUTION_FB);
			nType = NET_MSG_GCTRL_CLUB_DISSOLUTION_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DISSOLUTION_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_2FLD : public NET_MSG_GENERIC
	{
		DWORD				dwClubID;

		SNET_CLUB_DEL_2FLD () :
			dwClubID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_DEL_2FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEL_2FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_2CLT  : public NET_MSG_GENERIC
	{
		SNET_CLUB_DEL_2CLT ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_DEL_2CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEL_2CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEL_BRD : public SNETPC_BROAD
	{
		SNET_CLUB_DEL_BRD ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_DEL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_CLUB_INFO //: public NET_MSG_CHARACTER
	{
        enum { VERSION = 2, };
		DWORD				dwClubID;		
		DWORD				dwRank;
		DWORD				dwMarkVER;
		DWORD				dwMasterID;
        DWORD               dwMasterLevel;
		DWORD				dwCDCertifior;
		
        DWORD				dwAlliance;
		
        DWORD				dwMEMBER_NUM;
		DWORD				dwALLIANCE_NUM;
		DWORD				dwBATTLE_NUM;
		
        DWORD				dwBattleWin;
		DWORD				dwBattleLose;
		DWORD				dwBattleDraw;

		DWORD				dwAllianceBattleWin;
		DWORD				dwAllianceBattleLose;
		DWORD				dwAllianceBattleDraw;

        __time64_t			tOrganize;
        __time64_t			tDissolution;        
        
        std::string m_ClubName; // CLUB_NAME_LENGTH;
        std::string m_MasterName; // CHAR_SZNAME;

        DWORD m_PublicType; //! 공개/비공개 설정
        club::AUTO_KICK_OUT m_AutoKickOut; //! 자동 추방

        MSGPACK_DEFINE(
            dwClubID, dwRank, dwMarkVER, dwMasterID, dwMasterLevel, dwCDCertifior, dwAlliance,
            dwMEMBER_NUM, dwALLIANCE_NUM, dwBATTLE_NUM,
            dwBattleWin, dwBattleLose, dwBattleDraw,
            dwAllianceBattleWin, dwAllianceBattleLose, dwAllianceBattleDraw,
            tOrganize, tDissolution,
            m_ClubName, m_MasterName,
            m_PublicType, m_AutoKickOut);

		SNETLOBBY_CLUB_INFO() 
			: dwClubID(0)
			, dwMasterID(0)
            , dwMasterLevel(0)
			, dwRank(0)
			
			, dwMarkVER(0)
			
			, dwCDCertifior(0)
			
			, tOrganize(0)
			, tDissolution(0)
			
			, dwAlliance(0)
			
			, dwMEMBER_NUM(0)
			, dwALLIANCE_NUM(0)
			, dwBATTLE_NUM(0)
			, dwBattleWin(0)
			, dwBattleLose(0)
			, dwBattleDraw(0)
			, dwAllianceBattleWin(0)
			, dwAllianceBattleLose(0)
			, dwAllianceBattleDraw(0)
		{
			//dwSize = sizeof(SNETLOBBY_CLUB_INFO);
			//nType = NET_MSG_LOBBY_CLUB_INFO;
			//MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CLUB_INFO)<=NET_DATA_BUFSIZE);
			//memset(m_ClubName, 0, sizeof(char) * CLUB_NAME_LENGTH);
			//memset(m_MasterName, 0, sizeof(char) * CHAR_SZNAME);			
		}

        void SetClubName(const char* _ClubName)
        {
            if (_ClubName)
                m_ClubName = _ClubName;
        }

        void SetMasterName(const char* _MasterName)
        {
            if (_MasterName)
                m_MasterName = _MasterName;
        }        
	};

    struct SNETLOBBY_CLUB_INFO_EX //: public NET_MSG_CHARACTER
    {
        enum { VERSION = 2, };

        __time64_t  m_tNoticeDate;
        std::string m_Notice; //[EMCLUB_NOTICE_LEN];
        std::string m_NoticeChaName; //[CHAR_SZNAME];
        std::string m_NoticeNewbie; //[EMCLUB_NOTICE_LEN];

        MSGPACK_DEFINE(m_tNoticeDate, m_Notice, m_NoticeChaName, m_NoticeNewbie);

        SNETLOBBY_CLUB_INFO_EX()
            : m_tNoticeDate(0)
        {
            //MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CLUB_INFO_EX)<=NET_DATA_BUFSIZE);
            //dwSize = sizeof(SNETLOBBY_CLUB_INFO_EX);
            //nType = NET_MSG_LOBBY_CLUB_INFO_EX;
            //memset(m_Notice, 0, sizeof(char) * EMCLUB_NOTICE_LEN);
            //memset(m_NoticeNewbie, 0, sizeof(char) * EMCLUB_NOTICE_LEN);
            //memset(m_NoticeChaName, 0, sizeof(char) * CHAR_SZNAME);
        }

        void SetNotice(const char* Notice)
        {
            if (Notice)
                m_Notice = Notice;
        }

        void SetNoticeNewbie(const char* Notice)
        {
            if (Notice)
                m_NoticeNewbie = Notice;
        }

        void SetNoticeDate(__time64_t Date)
        {
            m_tNoticeDate = Date;
        }

        void SetNoticeCharName(const char* szName)
        {
            if (szName)
                m_NoticeChaName = szName;
        }
    };
/*
	struct SNETLOBBY_CLUB_MEMBER : public NET_MSG_CHARACTER
	{
		enum { EMMAXMEMBER = 10 };
		
		DWORD		m_MemberNum;
		SCLUBMEMBER	m_Members[EMMAXMEMBER];

		SNETLOBBY_CLUB_MEMBER()
            : m_MemberNum(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD);
			nType = NET_MSG_LOBBY_CLUB_MEMBER;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CLUB_MEMBER)<=NET_DATA_BUFSIZE);
		}

		bool ADDMEMBER(DWORD dwCharID, DWORD dwFlags, const char* szMember, WORD ChaLevel)
		{
			if (EMMAXMEMBER==m_MemberNum)
                return false;

			m_Members[m_MemberNum].m_ChaDbNum = dwCharID;
			m_Members[m_MemberNum].m_Flag = dwFlags;
			m_Members[m_MemberNum].SetName(szMember);
            m_Members[m_MemberNum].m_ChaLevel = ChaLevel;

			++m_MemberNum;

			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SCLUBMEMBER)*m_MemberNum;			
			return true;
		}

		void RESET ()
		{
			m_MemberNum = 0;
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD);
		}
	};
*/
    struct NET_LOBBY_CLUB_INFO_END : public NET_MSG_GENERIC
    {
        NET_LOBBY_CLUB_INFO_END()
        {
            dwSize = sizeof(NET_LOBBY_CLUB_INFO_END);
            nType = NET_MSG_LOBBY_CLUB_INFO_END;
            MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CLUB_INFO_END)<=NET_DATA_BUFSIZE);
        }
    };
    

	struct GLALLIANCE // : public NET_MSG_GENERIC
	{
		DWORD		m_dwID;
        DWORD       m_MarkVersion;
		char		m_szName[CHAR_SZNAME];
        char        m_szMasterName[CHAR_SZNAME];

		GLALLIANCE(DWORD MarkVersion=0)
            : m_dwID(CLUB_NULL)
            , m_MarkVersion(MarkVersion)
		{
			memset(m_szName, 0, sizeof(char) * CHAR_SZNAME);
            memset(m_szMasterName, 0, sizeof(char) * CHAR_SZNAME);
		}

		GLALLIANCE(DWORD dwID, const char* szName, const char* szMasterName, DWORD MarkVersion)
		{
			m_dwID = dwID;
            m_MarkVersion = MarkVersion;
            if (szName)
			    StringCchCopy(m_szName, CHAR_SZNAME, szName);
            if (szMasterName)
                StringCchCopy(m_szMasterName, CHAR_SZNAME, szMasterName);
		}
	};

	struct SNETLOBY_CLUB_ANCE_2CLT : public NET_MSG_CHARACTER
	{
		enum { EMMAX = 10 };
		
		DWORD		dwAllianceNum;
		GLALLIANCE	sALLIANCE[EMMAX];

		SNETLOBY_CLUB_ANCE_2CLT()
            : dwAllianceNum(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
			nType = NET_MSG_LOBBY_CLUB_ALLIANCE;
			memset(sALLIANCE, 0, sizeof(GLALLIANCE) * SNETLOBY_CLUB_ANCE_2CLT::EMMAX);
			MIN_STATIC_ASSERT(sizeof(SNETLOBY_CLUB_ANCE_2CLT)<=NET_DATA_BUFSIZE);
		}

		bool ADDALLIANCE(DWORD dwClubID, const char* szName, const char* szMasterName, DWORD MarkVersion)
		{
			if (dwAllianceNum >= EMMAX || szName == NULL)
			{
				return false;
			}
			else
			{
				sALLIANCE[dwAllianceNum] = GLALLIANCE(dwClubID, szName, szMasterName, MarkVersion);
				++dwAllianceNum;
				dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
				//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
				return true;
			}
		}

		void RESET ()
		{
			dwAllianceNum = 0;
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(GLALLIANCE)*dwAllianceNum;
			memset(sALLIANCE, 0, sizeof(GLALLIANCE) * SNETLOBY_CLUB_ANCE_2CLT::EMMAX);
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
		}
	};

    //! Lobby 에서 Club 가입
    struct NET_CLUB_MENBER_LOBBY_CA : public NET_MSG_GENERIC
    {
        DWORD m_ClubDbNum;
        DWORD m_ChaDbNum;
        char m_ChaName[CHAR_SZNAME];
        EMCHARCLASS m_ChaClass;
        sc::MGUID m_Guid;

        NET_CLUB_MENBER_LOBBY_CA(DWORD ClubDbNum, DWORD ChaDbNum, const char* szName, EMCHARCLASS ChaClass)
            : m_ClubDbNum(ClubDbNum)
            , m_ChaDbNum(ChaDbNum)
            , m_ChaClass(ChaClass)
        {
            dwSize = sizeof(NET_CLUB_MENBER_LOBBY_CA);
            nType = NET_MSG_CLUB_MENBER_LOBBY_CA;
            m_Guid = msg_guid::GUID1;
            if (szName)
                StringCchCopy(m_ChaName, CHAR_SZNAME, szName);
        }
    };

	struct NET_CLUB_MEMBER_REQ_CA : public NET_MSG_GENERIC
	{	
		DWORD m_TarGaeaID;
		CHAR				m_szChaName[CHR_ID_LENGTH+1];

		NET_CLUB_MEMBER_REQ_CA(DWORD TarGaeaID)
            : m_TarGaeaID(TarGaeaID)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_REQ_CA);
			nType = NET_MSG_CLUB_MEMBER_REQ_CA;
			ZeroMemory(m_szChaName,sizeof(m_szChaName));

			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_REQ_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_MEMBER_REQ_ASK_AC : public NET_MSG_GENERIC
	{
		DWORD				dwMaster;
		char				szClubName[CHAR_SZNAME];
		char				szMasterName[CHAR_SZNAME];

		NET_CLUB_MEMBER_REQ_ASK_AC () 
			: dwMaster(GAEAID_NULL)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_REQ_ASK_AC);
			nType = NET_MSG_CLUB_MEMBER_REQ_ASK_AC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_REQ_ASK_AC)<=NET_DATA_BUFSIZE);

			memset(	szClubName,   0, sizeof(char) * CHAR_SZNAME );
			memset( szMasterName, 0, sizeof(char) * CHAR_SZNAME );
		}

        void SetClubName(const char* _ClubName)
        {
            if (_ClubName)
                StringCchCopy(szClubName, CHAR_SZNAME, _ClubName);
        }

        void SetMasterName(const char* _Name)
        {
            if (_Name)
                StringCchCopy(szMasterName, CHAR_SZNAME, _Name);
        }
	};

	struct NET_CLUB_MEMBER_REQ_ANS_CA : public NET_MSG_GENERIC
	{
		DWORD				dwMaster;
		bool				bOK;

		NET_CLUB_MEMBER_REQ_ANS_CA () 
			: dwMaster(GAEAID_NULL)
			, bOK(false)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_REQ_ANS_CA);
			nType = NET_MSG_CLUB_MEMBER_REQ_ANS_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_REQ_ANS_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_REQ_2AGT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwReqGID;

		SNET_CLUB_MEMBER_REQ_2AGT () 
			: dwReqGID(GAEAID_NULL)
		{
			dwSize = sizeof(SNET_CLUB_MEMBER_REQ_2AGT);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_REQ_2AGT)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_MEMBER_REQ_FB_AC : public NET_MSG_GENERIC
	{
		EMCLUB_MEMBER_REQ_FB emFB;
        char szReqName[CHAR_SZNAME];
        bool m_bLobby;
				
		NET_CLUB_MEMBER_REQ_FB_AC(bool bLobby)
			: emFB(EMCLUB_MEMBER_REQ_FB_FAIL)
            , m_bLobby(bLobby)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_REQ_FB_AC);
			nType = NET_MSG_CLUB_MEMBER_REQ_FB_AC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_REQ_FB_AC)<=NET_DATA_BUFSIZE);

			memset(szReqName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetTarChaName(const char* szName)
        {
            if (szName)
                StringCchCopy(szReqName, CHAR_SZNAME, szName);
        }
	};

	struct SNET_CLUB_MEMBER_ADD_2FLD : public NET_MSG_GENERIC
	{		
		DWORD m_ClubDbNum;
		DWORD m_ChaDbNum;
        WORD  m_ChaLevel;
		char  m_ChaName[CHAR_SZNAME];
        EMCHARCLASS m_ChaClass; // EMCHARCLASS

        SNET_CLUB_MEMBER_ADD_2FLD(DWORD ClubDbNum, DWORD MemberDbNum, WORD ChaLevel, const char* szName, EMCHARCLASS ChaClass)
			: m_ClubDbNum(ClubDbNum)
			, m_ChaDbNum(MemberDbNum)
            , m_ChaLevel(ChaLevel)
            , m_ChaClass(ChaClass)
		{
			dwSize = sizeof(SNET_CLUB_MEMBER_ADD_2FLD);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_ADD_2FLD)<=NET_DATA_BUFSIZE);

			StringCchCopy(m_ChaName, CHAR_SZNAME, szName);
		}
	};

	struct SNET_CLUB_MEMBER_DB2DEL : public NET_MSG_GENERIC
	{
		DWORD				dwClub;
		DWORD				dwMember;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(DWORD), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_CLUB_MEMBER_DB2DEL () :
			dwClub(0),
			dwMember(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_DB2DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_MEMBER_DEL_CA : public NET_MSG_GENERIC
	{		
		DWORD m_ChaDbNum;

		NET_CLUB_MEMBER_DEL_CA()
			: m_ChaDbNum(0)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_DEL_CA);
			nType = NET_MSG_CLUB_MEMBER_DEL_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_DEL_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_MEMBER_DEL_AF : public NET_MSG_GENERIC
	{
		DWORD	   dwClub;
		DWORD	   dwMember;
		__time64_t tSECEDE;
		bool	   bIsKick;
		char	   cDelMemberName[CHAR_SZNAME];

        NET_CLUB_MEMBER_DEL_AF(DWORD ClubDbNum, DWORD DelMember, const __time64_t& DelTime, const char* DelMemberName, bool bKick = false)
			: dwClub(ClubDbNum)
			, dwMember(DelMember)
			, tSECEDE(DelTime)
			, bIsKick(bKick)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_DEL_AF);
			nType = NET_MSG_CLUB_MEMBER_DEL_AF;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_DEL_AF)<=NET_DATA_BUFSIZE);

			StringCchCopy(cDelMemberName, CHAR_SZNAME, DelMemberName);
		}
	};

	struct NET_CLUB_MEMBER_DEL_FC : public NET_MSG_GENERIC
	{
		DWORD				dwClub;
		DWORD				dwMember;

		NET_CLUB_MEMBER_DEL_FC () 
			: dwClub(0)
			, dwMember(0)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_DEL_FC);
			nType = NET_MSG_CLUB_MEMBER_DEL_FC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_DEL_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_MEMBER_DEL_AC : public NET_MSG_GENERIC
	{
		EMCLUB_MEMBER_DEL_FB emFB;

		NET_CLUB_MEMBER_DEL_AC () 
			: emFB(EMCLUB_MEMBER_DEL_FB_FAIL)
		{
			dwSize = sizeof(NET_CLUB_MEMBER_DEL_AC);
			nType = NET_MSG_CLUB_MEMBER_DEL_AC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MEMBER_DEL_AC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ : public NET_MSG_GENERIC
	{
		DWORD				dwMember;

		SNET_CLUB_AUTHORITY_REQ () 
			: dwMember(0)
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_FB : public NET_MSG_GENERIC
	{
		EMCLUB_AUTHORITY_REQ_FB	emFB;
		DWORD					dwCharID;
		__time64_t				tAuthority;

		SNET_CLUB_AUTHORITY_REQ_FB () 
			: emFB(EMCLUB_AUTHORITY_REQ_FB_FAIL)
			, dwCharID( GAEAID_NULL ) 
			, tAuthority( 0 )
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_FB);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_ASK : public NET_MSG_GENERIC
	{
		SNET_CLUB_AUTHORITY_REQ_ASK () 		
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_ASK);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_REQ_ASK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_AUTHORITY_REQ_ANS : public NET_MSG_GENERIC
	{
		bool				bOK;

		SNET_CLUB_AUTHORITY_REQ_ANS () 
			: bOK(false)
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_REQ_ANS);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_REQ_ANS)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNET_CLUB_AUTHORITY_CLT : public NET_MSG_GENERIC
	{
		DWORD				dwCharID;

		SNET_CLUB_AUTHORITY_CLT () 
			: dwCharID( GAEAID_NULL ) 
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_CLT);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_CLT)<=NET_DATA_BUFSIZE);
		}
	};

	struct 	SNET_CLUB_AUTHORITY_BRD : public SNETPC_BROAD
	{
		DWORD				dwCharID;

		SNET_CLUB_AUTHORITY_BRD () 
			: dwCharID( GAEAID_NULL ) 
		{
			dwSize = sizeof(SNET_CLUB_AUTHORITY_BRD);
			nType = NET_MSG_GCTRL_CLUB_AUTHORITY_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_AUTHORITY_BRD)<=NET_DATA_BUFSIZE);
		}
	};



	struct NET_CLUB_MASTER_CHANGE_AF : public NET_MSG_GENERIC
	{
		DWORD m_ClubDbNum;
		DWORD m_ChaDbNum;
        WORD  m_ChaLevel;

        NET_CLUB_MASTER_CHANGE_AF(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel)
			: m_ClubDbNum(ClubDbNum) // CLUB_NULL
			, m_ChaDbNum(ChaDbNum) // GAEAID_NULL
            , m_ChaLevel(ChaLevel)
		{
			dwSize = sizeof(NET_CLUB_MASTER_CHANGE_AF);
			nType = NET_MSG_CLUB_MASTER_CHANGE_AF;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_MASTER_CHANGE_AF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MARK_INFO : public NET_MSG_GENERIC
	{
		DWORD					dwClubID;

		SNET_CLUB_MARK_INFO ()
			: dwClubID (0)
		{
			dwSize = sizeof(SNET_CLUB_MARK_INFO);
			nType = NET_MSG_GCTRL_CLUB_MARK_INFO;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MARK_INFO_FB : public NET_MSG_GENERIC
	{
		EMCLUB_MARK_INFO_FB		emFB;
		DWORD					dwClubID;
		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_INFO_FB () 
			: emFB(EMCLUB_MARK_INFO_FB_FAIL)
			, dwClubID(0)
			, dwMarkVER(0)
		{
			dwSize = sizeof(SNET_CLUB_MARK_INFO_FB);
			nType = NET_MSG_GCTRL_CLUB_MARK_INFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_INFO_FB)<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE : public NET_MSG_GENERIC
	{
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE ()
		{
			dwSize = sizeof(SNET_CLUB_MARK_CHANGE);
			nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_CHANGE)<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_2FLD : public NET_MSG_GENERIC
	{
		DWORD					dwClub;
		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE_2FLD () 
			: dwClub(0)
			, dwMarkVER(0)
		{
			dwSize = sizeof(SNET_CLUB_MARK_CHANGE_2FLD);
			nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_CHANGE_2FLD)<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_2CLT : public NET_MSG_GENERIC
	{
		DWORD					dwMarkVER;
		DWORD					aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

		SNET_CLUB_MARK_CHANGE_2CLT () 
			: dwMarkVER(0)
		{
			dwSize = sizeof(SNET_CLUB_MARK_CHANGE_2CLT);
			nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_CHANGE_2CLT)<=NET_DATA_BUFSIZE);

			memset(aryMark, 0, sizeof(DWORD) * (EMCLUB_MARK_SX*EMCLUB_MARK_SY));
		}
	};

	struct SNET_CLUB_MARK_CHANGE_FB : public NET_MSG_GENERIC
	{
		EMCLUB_MARK_CHANGE_FB	emFB;

		SNET_CLUB_MARK_CHANGE_FB () 
			: emFB(EMCLUB_MARK_CHANGE_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_MARK_CHANGE_FB);
			nType = NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MARK_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //! 클럽 랭크 변경 요청/club rank change
	struct NET_CLUB_RANK_UP_CF : public NET_MSG_GENERIC
	{
		DWORD dwNpcGlobID;

		NET_CLUB_RANK_UP_CF(DWORD NpcId)
			: dwNpcGlobID(NpcId)
		{
			dwSize = sizeof(NET_CLUB_RANK_UP_CF);
			nType = NET_MSG_CLUB_RANK_UP_CF;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_RANK_UP_CF)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_RANK_CHANGE_FA : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		NET_CLUB_RANK_CHANGE_FA ()
		{
			dwSize = sizeof(NET_CLUB_RANK_CHANGE_FA);
			nType = NET_MSG_CLUB_RANK_CHANGE_FA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_RANK_CHANGE_FA)<=NET_DATA_BUFSIZE);
		}
	};

	//! 클럽 랭크 업/club rank up
    struct NET_CLUB_RANK_CHANGE_AF : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
		DWORD dwRank;
        DWORD m_ReqChaDbNum; //! 요청한 캐릭터 번호

        NET_CLUB_RANK_CHANGE_AF(
            DWORD ClubDbNum,
            DWORD Rank,
            DWORD ReqChaDbNum)
			: dwClubID(ClubDbNum)
			, dwRank(Rank)
            , m_ReqChaDbNum(ReqChaDbNum)
		{
			dwSize = sizeof(NET_CLUB_RANK_CHANGE_AF);
			nType = NET_MSG_CLUB_RANK_CHANGE_AF;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_RANK_CHANGE_AF)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_RANK_CHANGE_FC : public NET_MSG_GENERIC
	{
		DWORD dwRank;

		NET_CLUB_RANK_CHANGE_FC() 
			: dwRank(0)
		{
			dwSize = sizeof(NET_CLUB_RANK_CHANGE_FC);
			nType = NET_MSG_CLUB_RANK_CHANGE_FC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_RANK_CHANGE_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_RANK_FB : public NET_MSG_GENERIC
	{
		EMCLUB_RANK_FB			emFB;
		DWORD					dwValue;

		SNET_CLUB_RANK_FB () 
			: emFB(EMCLUB_RANK_FB_FAIL)
			, dwValue(0)
		{
			dwSize = sizeof(SNET_CLUB_RANK_FB);
			nType = NET_MSG_GCTRL_CLUB_RANK_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_RANK_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //! 선도클럽 맵 정보 요청.
    struct NET_GUIDANCE_MAP_INFO_CA : public NET_MSG_CHARACTER
    {
        DWORD ClubDbNum;

        NET_GUIDANCE_MAP_INFO_CA( DWORD _ClubDbNum )
            : ClubDbNum(_ClubDbNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_GUIDANCE_MAP_INFO_CA)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_GUIDANCE_MAP_INFO_CA);
            nType  = NET_MSG_GUIDANCE_MAP_INFO_CA; // EMNET_MSG
        }
    };

    struct NET__GUIDANCE_MAP_INFO_AC
    { 
        std::vector<DWORD> m_GuidMap; //! 수수료를 징수하는 맵

        MSGPACK_DEFINE(m_GuidMap);

        NET__GUIDANCE_MAP_INFO_AC()
        {
        }
    };

    //! 클럽 멤버 별명 변경
	struct SNET_CLUB_MEMBER_NICK : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		char szNick[CHAR_SZNAME];

		SNET_CLUB_MEMBER_NICK(const char* _szNick)
		{
			dwSize = sizeof(SNET_CLUB_MEMBER_NICK);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_NICK;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_NICK)<=NET_DATA_BUFSIZE);
			
            if (_szNick)
                StringCchCopy(szNick, CHAR_SZNAME, _szNick);
		}
	};

	struct SNET_CLUB_MEMBER_NICK_FB : public NET_MSG_GENERIC
	{
		EMCLUB_MEMBER_NICK_FB	emFB;
		char					szNick[CHAR_SZNAME];

		SNET_CLUB_MEMBER_NICK_FB () 
			: emFB(EMCLUB_MEMBER_NICK_FB_FAIL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_NICK_FB)<=NET_DATA_BUFSIZE);

			memset(szNick, 0, sizeof(char) * CHAR_SZNAME);
		}
	};
/*
	struct SMEMSTATE
	{
		DWORD		dwCHARID;		
		SNATIVEID	nidMAP;
        int			nCHANNEL;
        bool		bONLINE;

		SMEMSTATE () 
			: dwCHARID(0)
			, bONLINE(0)
			, nCHANNEL(0)
			, nidMAP(false)
		{
		}
	};
*/
/*
	struct SNET_CLUB_MEMBER_STATE : public NET_MSG_GENERIC
	{
		enum { EMMAX = 50, };
		
		DWORD					dwNumber;
		SMEMSTATE				sSTATE[EMMAX];

		SNET_CLUB_MEMBER_STATE () 
			: dwNumber(0)
		{
			dwSize = GETSIZE();
			nType = NET_MSG_GCTRL_CLUB_MEMBER_STATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_STATE)<=NET_DATA_BUFSIZE);

			memset(sSTATE, 0, sizeof(SMEMSTATE) * EMMAX);
		}

		int GETSIZE ()	{ return sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SMEMSTATE)*dwNumber; }

		bool ADD ( DWORD dwCHARID, bool bONLINE, int nCHANNEL, SNATIVEID nidMAP )
		{
			if ( dwNumber>=EMMAX )	return false;

			sSTATE[dwNumber].dwCHARID = dwCHARID;
			sSTATE[dwNumber].bONLINE = bONLINE;
			sSTATE[dwNumber].nCHANNEL = nCHANNEL;
			sSTATE[dwNumber].nidMAP = nidMAP;
			++dwNumber;

			dwSize = GETSIZE();
			//MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_STATE)<=NET_DATA_BUFSIZE);

			return true;
		}

		void RESET ()
		{
			dwNumber = 0;
			dwSize = GETSIZE();
			memset(sSTATE, 0, sizeof(SMEMSTATE) * EMMAX);
		}
	};
*/
/*
	struct SMEMPOS //: public NET_MSG_GENERIC
	{
		DWORD		dwCHARID;
		D3DXVECTOR2	vecPOS;

		SMEMPOS () 
			: dwCHARID(0)
			, vecPOS(0,0)
		{
		}
	};
*/
/*
	struct SNET_CLUB_MEMBER_POS : public NET_MSG_GENERIC
	{
		enum { EMMAX = 40, };
		
		DWORD	dwNumber;
		SMEMPOS sPOS[EMMAX];

		SNET_CLUB_MEMBER_POS()
			: dwNumber(0)
		{
			dwSize = GETSIZE();
			nType = NET_MSG_GCTRL_CLUB_MEMBER_POS;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_POS)<=NET_DATA_BUFSIZE);

			memset(sPOS, 0, sizeof(SMEMPOS) * EMMAX);
		}

		int GETSIZE ()	{ return sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(SMEMPOS)*dwNumber; }

		bool ADD ( DWORD dwCHARID, D3DXVECTOR2 vecPOS )
		{
			if ( dwNumber>=EMMAX )
				return false;

			sPOS[dwNumber].dwCHARID = dwCHARID;
			sPOS[dwNumber].vecPOS = vecPOS;
			++dwNumber;

			dwSize = GETSIZE();
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_POS)<=NET_DATA_BUFSIZE);

			return true;
		}

		void RESET ()
		{
			dwNumber = 0;
			dwSize = GETSIZE();
			memset(sPOS, 0, sizeof(SMEMPOS) * EMMAX);
		}
	};
*/
	struct SNET_CLUB_MEMBER_SECEDE : public NET_MSG_GENERIC
	{
		SNET_CLUB_MEMBER_SECEDE()
		{
			dwSize = sizeof(SNET_CLUB_MEMBER_SECEDE);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_SECEDE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_SECEDE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_MEMBER_SECEDE_FB : public NET_MSG_GENERIC
	{		
		EMCLUB_MEMBER_SECEDE_FB	emFB;
		SNET_CLUB_MEMBER_SECEDE_FB() 
			: emFB(EMCLUB_MEMBER_SECEDE_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_MEMBER_SECEDE_FB);
			nType = NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_SECEDE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_INCOME_MONEY_FA : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM = 29834, };
		DWORD    m_ClubDbNum;
		LONGLONG m_ProfitMoney;
        int      m_MagicNum;

		NET_CLUB_INCOME_MONEY_FA () 
			: m_ClubDbNum(0)
			, m_ProfitMoney(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(NET_CLUB_INCOME_MONEY_FA);
			nType = NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_INCOME_MONEY_FA)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_GETSTORAGE_CA : public NET_MSG_GENERIC
	{
		NET_CLUB_GETSTORAGE_CA()
		{
			dwSize = sizeof(NET_CLUB_GETSTORAGE_CA);
			nType = NET_MSG_CLUB_GETSTORAGE_CA;
		}
	};

	//! 
	struct SNET_CLUB_GETSTORAGE_ITEM //: public NET_MSG_GENERIC
	{
		DWORD	   m_Channel;
		SINVENITEM m_ItemData;

        MSGPACK_DEFINE(m_Channel, m_ItemData);

		SNET_CLUB_GETSTORAGE_ITEM()
			: m_Channel(0)
		{
			//dwSize = sizeof(SNET_CLUB_GETSTORAGE_ITEM);
			//nType = NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM;
		}
	};

	//! 클럽창고에 있는 아이템을 손에 들때
	struct NET_CLUB_STORAGE_TO_INVEN_CA //: public NET_MSG_GENERIC
	{
		WORD m_Channel;
		WORD m_PosX;
		WORD m_PosY;
        WORD m_HoldPosX;
        WORD m_HoldPosY;

        MSGPACK_DEFINE(m_Channel, m_PosX, m_PosY, m_HoldPosX, m_HoldPosY);

		NET_CLUB_STORAGE_TO_INVEN_CA()
			: m_Channel(0)
			, m_PosX(0)
			, m_PosY(0)
            , m_HoldPosX(0)
            , m_HoldPosY(0)
		{
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_TO_INVEN_CA)<=NET_DATA_BUFSIZE);
			//dwSize = sizeof(NET_CLUB_STORAGE_TO_HOLD_CA);
			//nType = NET_MSG_CLUB_STORAGE_TO_HOLD_CA;
		}
	};

    //! 클럽창고에 있는 아이템을 손에 들때
    struct NET_CLUB_STORAGE_TO_INVEN_AF //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 8282, };
        int m_MagicNum;
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        SITEMCUSTOM m_Item;
        WORD m_PosX;
        WORD m_PosY;

        MSGPACK_DEFINE(m_MagicNum, m_ChaDbNum, m_ClubDbNum, m_Item, m_PosX, m_PosY);

        NET_CLUB_STORAGE_TO_INVEN_AF()
            : m_MagicNum(MAGIC_NUM)
            , m_ChaDbNum(0)
            , m_ClubDbNum(0)
            , m_PosX(0)
            , m_PosY(0)
        {            
        }

        NET_CLUB_STORAGE_TO_INVEN_AF(DWORD ChaDbNum, DWORD ClubDbNum, const SITEMCUSTOM& ItemCustom, WORD PosX, WORD PosY)
            : m_MagicNum(MAGIC_NUM)
            , m_ChaDbNum(ChaDbNum)
            , m_ClubDbNum(ClubDbNum)
            , m_Item(ItemCustom)
            , m_PosX(PosX)
            , m_PosY(PosY)
        {
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_TO_INVEN_AF)<=NET_DATA_BUFSIZE);
            //nType = NET_MSG_CLUB_STORAGE_TO_HOLD_AF;            
        }
    };

    struct NET_CLUB_STORAGE_RESTORE_FA //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 29103, };
        int m_MagicNum;
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        SITEMCUSTOM m_Item;

        MSGPACK_DEFINE(m_MagicNum, m_ChaDbNum, m_ClubDbNum, m_Item);

        NET_CLUB_STORAGE_RESTORE_FA()
            : m_ChaDbNum(0)
            , m_ClubDbNum(0)            
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_RESTORE_FA(DWORD ChaDbNum, DWORD ClubDbNum, const SITEMCUSTOM& Item)
            : m_ChaDbNum(ChaDbNum)
            , m_ClubDbNum(ClubDbNum)
            , m_Item(Item)
            , m_MagicNum(MAGIC_NUM)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_RESTORE_FA);
            //nType = NET_MSG_CLUB_STORAGE_RESTORE_FA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_RESTORE_FA)<=NET_DATA_BUFSIZE);
        }
    };

	//! 들고 있는 아이템을 클럽창고에 있는 아이템과 교환
	struct NET_CLUB_STORAGE_EX_INVEN_CA //: public NET_MSG_GENERIC
	{
		WORD m_Channel;
		WORD m_ClubStoragePosX;
		WORD m_ClubStoragePosY;

        WORD m_InvenPosX;
        WORD m_InvenPosY;

        MSGPACK_DEFINE(m_Channel, m_ClubStoragePosX, m_ClubStoragePosY, m_InvenPosX, m_InvenPosY);

		NET_CLUB_STORAGE_EX_INVEN_CA() 
			: m_Channel(0)
			, m_ClubStoragePosX(0)
			, m_ClubStoragePosY(0)
            , m_InvenPosX(0)
            , m_InvenPosY(0)
		{
			//dwSize = sizeof(NET_CLUB_STORAGE_EX_HOLD_CA);
			//nType = NET_MSG_CLUB_STORAGE_EX_HOLD_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_EX_INVEN_CA)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_CLUB_STORAGE_EX_INVEN_AF //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM=41023, };

        int m_MagicNum;
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        SITEMCUSTOM m_Item;

        WORD m_Channel;

        WORD m_ClubStoragePosX;
        WORD m_ClubStoragePosY;

        WORD m_InvenPosX;
        WORD m_InvenPosY;

        MSGPACK_DEFINE(m_MagicNum, m_ChaDbNum, m_ClubDbNum, m_Item, m_Channel, m_ClubStoragePosX, m_ClubStoragePosY, m_InvenPosX, m_InvenPosY);

        NET_CLUB_STORAGE_EX_INVEN_AF()
            : m_ChaDbNum(0)
            , m_ClubDbNum(0)
            , m_Channel(0)
            , m_ClubStoragePosX(0)
            , m_ClubStoragePosY(0)
            , m_InvenPosX(0)
            , m_InvenPosY(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_EX_INVEN_AF(DWORD ChaDbNum, DWORD ClubDbNum, const SITEMCUSTOM& Item)
            : m_ChaDbNum(ChaDbNum)
            , m_ClubDbNum(ClubDbNum)
            , m_Item(Item)
            , m_InvenPosX(0)
            , m_InvenPosY(0)
            , m_MagicNum(MAGIC_NUM)
        {
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_EX_INVEN_AF)<=NET_DATA_BUFSIZE);
            //dwSize = sizeof(NET_CLUB_STORAGE_EX_HOLD_AF);
            //nType = NET_MSG_CLUB_STORAGE_EX_HOLD_AF;
        }
    };

    struct NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA //: public NET_MSG_GENERIC
    {
        WORD m_Channel;
        WORD m_PosX;
        WORD m_PosY;

        WORD m_HoldChannel;
        WORD m_HoldPosX;
        WORD m_HoldPosY;

        MSGPACK_DEFINE(m_Channel, m_PosX, m_PosY, m_HoldChannel, m_HoldPosX, m_HoldPosY);

        NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA() 
            : m_Channel(0)
            , m_PosX(0)
            , m_PosY(0)
            , m_HoldChannel(0)
            , m_HoldPosX(0)
            , m_HoldPosY(0)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA);
            //nType = NET_MSG_CLUB_ITEM_EXCHANGE_TO_CLUB_STORAGE_CA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_STORAGE_TO_CLUB_STORAGE_CA //: public NET_MSG_GENERIC
    {
        WORD m_Channel;
        WORD m_PosX;
        WORD m_PosY;

        WORD m_HoldChannel;
        WORD m_HoldPosX;
        WORD m_HoldPosY;

        MSGPACK_DEFINE(m_Channel, m_PosX, m_PosY, m_HoldChannel, m_HoldPosX, m_HoldPosY);

        NET_CLUB_STORAGE_TO_CLUB_STORAGE_CA() 
            : m_Channel(0)
            , m_PosX(0)
            , m_PosY(0)
            , m_HoldChannel(0)
            , m_HoldPosX(0)
            , m_HoldPosY(0)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_EX_CLUB_STORAGE_CA);
            //nType = NET_MSG_CLUB_ITEM_EXCHANGE_TO_CLUB_STORAGE_CA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_TO_CLUB_STORAGE_CA)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_STORAGE_EX_INVEN_FA //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 9872, };
        int m_MagicNum;
        DWORD m_ClubDbNum;
        DWORD m_ChaDbNum;
        SITEMCUSTOM m_Item;

        WORD m_Channel;

        WORD m_ClubStoragePosX;
        WORD m_ClubStoragePosY;

        MSGPACK_DEFINE(m_ClubDbNum, m_MagicNum, m_ChaDbNum, m_Item, m_Channel, m_ClubStoragePosX, m_ClubStoragePosY);

        NET_CLUB_STORAGE_EX_INVEN_FA()
            : m_ClubDbNum(0)
            , m_ChaDbNum(0)
            , m_MagicNum(MAGIC_NUM)
            , m_Channel(0)
            , m_ClubStoragePosX(0)
            , m_ClubStoragePosY(0)
        {
        }

        NET_CLUB_STORAGE_EX_INVEN_FA(DWORD ClubDbNum, DWORD ChaDbNum, const SITEMCUSTOM& Item, WORD Channel, WORD ClubStoragePosX, WORD ClubStoragePosY)
            : m_ClubDbNum(ClubDbNum)
            , m_ChaDbNum(ChaDbNum)
            , m_Item(Item)
            , m_MagicNum(MAGIC_NUM)
            , m_Channel(Channel)
            , m_ClubStoragePosX(ClubStoragePosX)
            , m_ClubStoragePosY(ClubStoragePosY)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_EX_HOLD_FA);
            //nType = NET_MSG_CLUB_STORAGE_EX_HOLD_FA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_EX_INVEN_FA)<=NET_DATA_BUFSIZE);
        }
    };

	//! 손에 들고있던 아이템 클럽창고에 넣음
	struct NET_CLUB_INVEN_TO_STORAGE_CF //: public NET_MSG_GENERIC
	{
        BYTE m_Channel;
		WORD m_PosX;
		WORD m_PosY;

        WORD m_HoldPosX;
        WORD m_HoldPosY;

        MSGPACK_DEFINE(m_Channel, m_PosX, m_PosY, m_HoldPosX, m_HoldPosY);

		NET_CLUB_INVEN_TO_STORAGE_CF()
			: m_Channel(0)
			, m_PosX(0)
			, m_PosY(0)
            , m_HoldPosX(0)
            , m_HoldPosY(0)
		{
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_INVEN_TO_STORAGE_CF)<=NET_DATA_BUFSIZE);			
			// NET_MSG_CLUB_HOLD_TO_STORAGE_CF;			
		}
	};

    //! 손에 들고있던 아이템 클럽창고에 넣음
    struct NET_CLUB_INVEN_TO_STORAGE_FA //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 19823, };
        int m_MagicNum;
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        DWORD m_Channel;
        WORD m_PosX;
        WORD m_PosY;
        SITEMCUSTOM m_Item;

        MSGPACK_DEFINE(m_MagicNum, m_ChaDbNum, m_ClubDbNum, m_Channel, m_PosX, m_PosY, m_Item);

        NET_CLUB_INVEN_TO_STORAGE_FA()
            : m_ChaDbNum(0), m_ClubDbNum(0), m_Channel(0), m_PosX(0), m_PosY(0), m_MagicNum(MAGIC_NUM)
        {
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_INVEN_TO_STORAGE_FA)<=NET_DATA_BUFSIZE);
        }

        NET_CLUB_INVEN_TO_STORAGE_FA(
            DWORD ClubDbNum,
            DWORD ChaDbNum,
            DWORD Channel,
            WORD PosX,
            WORD PosY,
            const SITEMCUSTOM& Item)
            : m_ClubDbNum(ClubDbNum)
            , m_ChaDbNum(ChaDbNum)
            , m_Channel(Channel)
            , m_PosX(PosX)
            , m_PosY(PosY)
            , m_Item(Item)
            , m_MagicNum(MAGIC_NUM)
        {
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_INVEN_TO_STORAGE_FA)<=NET_DATA_BUFSIZE);
            // NET_MSG_CLUB_HOLD_TO_STORAGE_FA;            
        }
    };

    //! 손에 들고있던 아이템 클럽창고에 넣음 실패/복구
    struct NET_HOLD_ITEM_RESTORE_AF //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 19824, };
        int m_MagicNum;
        DWORD m_ChaDbNum;
        SITEMCUSTOM m_Item;

        MSGPACK_DEFINE(m_MagicNum, m_ChaDbNum, m_Item);
        
        NET_HOLD_ITEM_RESTORE_AF()
            : m_ChaDbNum(0), m_MagicNum(MAGIC_NUM)
        {
            MIN_STATIC_ASSERT(sizeof(NET_HOLD_ITEM_RESTORE_AF)<=NET_DATA_BUFSIZE);
            //dwSize = sizeof(NET_HOLD_ITEM_RESTORE_AF);
            //nType = NET_MSG_CLUB_HOLD_TO_STORAGE_AF;            
        }

        NET_HOLD_ITEM_RESTORE_AF(
            DWORD ChaDbNum,
            const SITEMCUSTOM& Item)
            : m_ChaDbNum(ChaDbNum)
            , m_Item(Item)
            , m_MagicNum(MAGIC_NUM)
        {
            MIN_STATIC_ASSERT(sizeof(NET_HOLD_ITEM_RESTORE_AF)<=NET_DATA_BUFSIZE);
            //dwSize = sizeof(NET_HOLD_ITEM_RESTORE_AF);
            //nType = NET_MSG_CLUB_HOLD_TO_STORAGE_AF;            
        }
    };

	//! 클럽 차고 아이템 분리불가
    /*
	struct SNET_CLUB_STORAGE_SPLIT : public NET_MSG_GENERIC
	{
		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wSplit;

		SNET_CLUB_STORAGE_SPLIT () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
			, wSplit(0)
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_SPLIT);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_SPLIT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_SPLIT)<=NET_DATA_BUFSIZE);
		}
	};
    */

	//! 
	struct NET_CLUB_STORAGE_SAVE_MONEY_CF //: public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=90567, };
        int m_MagicNum;		
		LONGLONG m_DepositMoney;

        MSGPACK_DEFINE(m_MagicNum, m_DepositMoney);

		NET_CLUB_STORAGE_SAVE_MONEY_CF() 
			: m_DepositMoney(0)
            , m_MagicNum(MAGIC_NUM)
		{
			//dwSize = sizeof(NET_CLUB_STORAGE_SAVE_MONEY_CF);
			//nType = NET_MSG_CLUB_STORAGE_SAVE_MONEY_CF;
			//MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_SAVE_MONEY_CF)<=NET_DATA_BUFSIZE);
		}

        void SetMoney(LONGLONG Money)
        {
            if (Money > 0)
                m_DepositMoney = Money;
        }
	};

    struct NET_CLUB_STORAGE_SAVE_MONEY_FA //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 67578, };
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        LONGLONG m_Money;
        int m_MagicNum;

        MSGPACK_DEFINE(m_ChaDbNum, m_ClubDbNum, m_Money, m_MagicNum);

        NET_CLUB_STORAGE_SAVE_MONEY_FA()
            : m_ChaDbNum(0)
            , m_ClubDbNum(0)
            , m_Money(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_SAVE_MONEY_FA(DWORD ChaDbNum, DWORD ClubDbNum, LONGLONG Money) 
            : m_ChaDbNum(ChaDbNum)
            , m_ClubDbNum(ClubDbNum)
            , m_Money(Money)
            , m_MagicNum(MAGIC_NUM)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_SAVE_MONEY_FA);
            //nType = NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA;
            //MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_SAVE_MONEY_FA)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_STORAGE_SAVE_MONEY_AF //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 45367, };
        DWORD m_ChaDbNum;
        LONGLONG m_Money;
        int m_MagicNum;

        MSGPACK_DEFINE(m_ChaDbNum, m_Money, m_MagicNum);

        NET_CLUB_STORAGE_SAVE_MONEY_AF() 
            : m_ChaDbNum(0)
            , m_Money(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_SAVE_MONEY_AF(DWORD ChaDbNum, LONGLONG Money) 
            : m_ChaDbNum(ChaDbNum)            
            , m_Money(Money)
            , m_MagicNum(MAGIC_NUM)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_SAVE_MONEY_AF);
            //nType = NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF;
            //MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_SAVE_MONEY_AF)<=NET_DATA_BUFSIZE);
        }
    };

	//! 클럽 보관금 출금
	struct NET_CLUB_STORAGE_DRAW_MONEY_CA //: public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM = 43234, };
        int m_MagicNum;
		LONGLONG m_WithdrawMoney;

        MSGPACK_DEFINE(m_MagicNum, m_WithdrawMoney);

        NET_CLUB_STORAGE_DRAW_MONEY_CA()
            : m_WithdrawMoney(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

		NET_CLUB_STORAGE_DRAW_MONEY_CA(LONGLONG WithdrawMoney)
			: m_WithdrawMoney(WithdrawMoney)
            , m_MagicNum(MAGIC_NUM)
		{
			//dwSize = sizeof(NET_CLUB_STORAGE_DRAW_MONEY_CA);
			//nType = NET_MSG_CLUB_STORAGE_DRAW_MONEY_CA;
			//MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_DRAW_MONEY_CA)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_CLUB_STORAGE_DRAW_MONEY_AF //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 23838, };
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        LONGLONG m_Money;
        int m_MagicNum;
        //unsigned int Crc32;

        MSGPACK_DEFINE(m_ChaDbNum, m_ClubDbNum, m_Money, m_MagicNum);

        NET_CLUB_STORAGE_DRAW_MONEY_AF()
            : m_ChaDbNum(0)
            , m_ClubDbNum(0)
            , m_Money(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_DRAW_MONEY_AF(DWORD ChaDbNum, DWORD ClubDbNum, LONGLONG Money)
            : m_ChaDbNum(ChaDbNum)
            , m_ClubDbNum(ClubDbNum)
            , m_Money(Money)
            , m_MagicNum(MAGIC_NUM)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_DRAW_MONEY_AF);
            //nType = NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF;
            //MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_DRAW_MONEY_AF)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_STORAGE_DRAW_MONEY_FA //: public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 83848, };
		DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;
        LONGLONG m_Money;
        int m_MagicNum;
        //unsigned int Crc32;

        MSGPACK_DEFINE(m_ChaDbNum, m_ClubDbNum, m_Money, m_MagicNum);

        NET_CLUB_STORAGE_DRAW_MONEY_FA()
            : m_ChaDbNum(0)
			, m_ClubDbNum(0)
            , m_Money(0)
            , m_MagicNum(MAGIC_NUM)
        {
        }

        NET_CLUB_STORAGE_DRAW_MONEY_FA(DWORD ChaDbNum, DWORD ClubDbNum, LONGLONG Money)
            : m_ChaDbNum(ChaDbNum)
			, m_ClubDbNum(ClubDbNum)
            , m_Money(Money)
            , m_MagicNum(MAGIC_NUM)
        {
            //dwSize = sizeof(NET_CLUB_STORAGE_DRAW_MONEY_FA);
            //nType = NET_MSG_CLUB_STORAGE_DRAW_MONEY_FA;
            //MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_DRAW_MONEY_FA)<=NET_DATA_BUFSIZE);
        }
    };
	
	struct SNET_CLUB_STORAGE_INSERT //: public NET_MSG_GENERIC
	{
		DWORD		dwChannel;
		SINVENITEM	Data;

        MSGPACK_DEFINE(dwChannel, Data);

		SNET_CLUB_STORAGE_INSERT()
			: dwChannel(0)
		{
			//dwSize = sizeof(SNET_CLUB_STORAGE_INSERT);
			//nType = NET_MSG_GCTRL_CLUB_STORAGE_INSERT;
			//MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_INSERT)<=NET_DATA_BUFSIZE);
		}
	};

	//! 
	struct SNET_CLUB_STORAGE_DELETE : public NET_MSG_GENERIC
	{
		DWORD	dwChannel;
		WORD	wPosX;
		WORD	wPosY;

		SNET_CLUB_STORAGE_DELETE()
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_DELETE);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_DELETE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	//! 
	struct SNET_CLUB_STORAGE_DEL_AND_INS : public NET_MSG_GENERIC
	{		
		DWORD		dwChannel;
		WORD		wDelX;
		WORD		wDelY;        
		SINVENITEM	sInsert;

		SNET_CLUB_STORAGE_DEL_AND_INS()
			: dwChannel(0)
			, wDelX(0)
			, wDelY(0)
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_DEL_AND_INS);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_DEL_AND_INS)<=NET_DATA_BUFSIZE);
		}
	};

	//! 
	struct SNET_CLUB_STORAGE_UPDATE_ITEM : public NET_MSG_GENERIC
	{
		
		DWORD				dwChannel;

		WORD				wPosX;
		WORD				wPosY;

		WORD				wTurnNum;
		
		SNET_CLUB_STORAGE_UPDATE_ITEM () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
			, wTurnNum(0)
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_UPDATE_ITEM);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_UPDATE_ITEM)<=NET_DATA_BUFSIZE);
		}
	};
	
    //! Club Storage money changed
    struct SNET_CLUB_STORAGE_UPDATE_MONEY : public NET_MSG_GENERIC
	{		
		LONGLONG lnMoney;
        LONGLONG IncomeMoney;

        SNET_CLUB_STORAGE_UPDATE_MONEY(LONGLONG _Money, LONGLONG _IncomeMoney)
			: lnMoney(_Money)
            , IncomeMoney(_IncomeMoney)
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_UPDATE_MONEY);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_UPDATE_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_STORAGE_RESET : public NET_MSG_GENERIC
	{
		SNET_CLUB_STORAGE_RESET()
		{
			dwSize = sizeof(SNET_CLUB_STORAGE_RESET);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_RESET;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_RESET)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_CLUB_STORAGE_SEND_END : public NET_MSG_GENERIC
    {
        NET_CLUB_STORAGE_SEND_END()
        {
            dwSize = sizeof(NET_CLUB_STORAGE_SEND_END);
            nType = NET_MSG_CLUB_STORAGE_SEND_END;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_STORAGE_SEND_END)<=NET_DATA_BUFSIZE);
        }
    };

/*	//	내부 메시지용. 실제로 socket 을 거치지 않는다.
	struct SNET_CLUB_STORAGE_GET_DB : public NET_MSG_GENERIC
	{
		

		DWORD				m_dwCharID;
		DWORD				m_dwClubID;

		LONGLONG			m_lnStorageMoney;
		DWORD				m_State;
		se::ByteStream*		m_pStream;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(DWORD)+sizeof(LONGLONG)+sizeof(DWORD)+sizeof(se::ByteStream*), };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_CLUB_STORAGE_GET_DB () 
			: m_dwCharID(0)
			, m_dwClubID(0)
			, m_lnStorageMoney(0)
			, m_State(1)
			, m_pStream(NULL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CLUB_STORAGE_GET_DB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_STORAGE_GET_DB)<=NET_DATA_BUFSIZE);

			memset(m_cBUFFER, 0, sizeof(char) * (NET_DATA_BUFSIZE-EMMSG_SIZE));
		}
	};*/

	struct SNET_CLUB_STORAGE_GET_DB_END
	{
		DWORD m_dwCharID;
		DWORD m_dwClubID;
		LONGLONG m_lnStorageMoney;

		MSGPACK_DEFINE( m_dwCharID, m_dwClubID, m_lnStorageMoney );

		SNET_CLUB_STORAGE_GET_DB_END() 
			: m_dwCharID( 0 )
			, m_dwClubID( 0 )
			, m_lnStorageMoney( 0 )
		{
		}
	};

	struct SNET_CLUB_STORAGE_GET_DB
	{
		DWORD m_dwClubID;
		SINVENITEM_SAVE sItem;

		MSGPACK_DEFINE( m_dwClubID, sItem );

		SNET_CLUB_STORAGE_GET_DB() 
			: m_dwClubID( 0 )
		{
		}
	};

	struct NET_CLUB_INCOME_RENEW_CA : public NET_MSG_GENERIC
	{
		NET_CLUB_INCOME_RENEW_CA()
		{
			dwSize = sizeof(NET_CLUB_INCOME_RENEW_CA);
			nType = NET_MSG_CLUB_INCOME_RENEW_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_INCOME_RENEW_CA)<=NET_DATA_BUFSIZE);
		}
	};
    /*
	struct SNET_CLUB_INCOME_UP : public NET_MSG_GENERIC
	{
		DWORD				dwClubID;

		SNET_CLUB_INCOME_UP () 
			: dwClubID(CLUB_NULL)
		{
			dwSize = sizeof(SNET_CLUB_INCOME_UP);
			nType = NET_MSG_GCTRL_CLUB_INCOME_UP;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INCOME_UP)<=NET_DATA_BUFSIZE);
		}
	};
    */
    /*
	struct SNET_CLUB_INCOME_DN : public NET_MSG_GENERIC
	{
		
		DWORD				dwClubID;
		LONGLONG			lnInComeMoney;

		SNET_CLUB_INCOME_DN () 
			: dwClubID(CLUB_NULL)
			, lnInComeMoney(0)
		{
			dwSize = sizeof(SNET_CLUB_INCOME_DN);
			nType = NET_MSG_GCTRL_CLUB_INCOME_DN;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INCOME_DN)<=NET_DATA_BUFSIZE);
		}
	};
    */
	struct SNET_CLUB_INCOME_FB : public NET_MSG_GENERIC
	{
		
		DWORD				dwClubID;
		LONGLONG			lnInComeMoney;

		SNET_CLUB_INCOME_FB () 
			: dwClubID(CLUB_NULL)
			, lnInComeMoney(0)
		{
			dwSize = sizeof(SNET_CLUB_INCOME_FB);
			nType = NET_MSG_GCTRL_CLUB_INCOME_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_INCOME_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_NOTICE_REQ : public NET_MSG_GENERIC
	{		
		char szNotice[EMCLUB_NOTICE_LEN];

        SNET_CLUB_NOTICE_REQ(const std::string& Notice)
		{
			dwSize = sizeof(SNET_CLUB_NOTICE_REQ);
			nType = NET_MSG_GCTRL_CLUB_NOTICE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NOTICE_REQ)<=NET_DATA_BUFSIZE);
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN));

            StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice.c_str());
		}
        /*
        void SetNotice(const char* Notice)
        {
            if (Notice)
                StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice);
        }
        void SetNotice(const std::string& Notice)
        {   
            StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice.c_str());
        }
        */
	};

	struct SNET_CLUB_NOTICE_FB : public NET_MSG_GENERIC
	{
		EMCLUB_NOTICE_FB emFB;
		char szNotice[EMCLUB_NOTICE_LEN];

		SNET_CLUB_NOTICE_FB()
			: emFB(EMCLUB_NOTICE_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_NOTICE_FB);
			nType = NET_MSG_GCTRL_CLUB_NOTICE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NOTICE_FB)<=NET_DATA_BUFSIZE);
			memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN));
		}

        void SetNotice(const char* Notice)
        {
            if (Notice)
                StringCchCopyA(szNotice, EMCLUB_NOTICE_LEN, Notice);
        }
	};

	//! Agent->Field 클럽 공지 변경
    struct SNET_CLUB_NOTICE_FLD : public NET_MSG_GENERIC
	{
		DWORD	   dwCLUB_ID;
        __time64_t m_NoticeDate;
		char	   szNotice[EMCLUB_NOTICE_LEN];
        char       m_NoticeChaName[CHAR_SZNAME];

		SNET_CLUB_NOTICE_FLD(
            DWORD ClubDbNum,
            __time64_t NoticeDate,
            const char* Notice,
            const char* NoticeChaName)
			: dwCLUB_ID(ClubDbNum)
            , m_NoticeDate(NoticeDate)            
		{
			dwSize = sizeof(SNET_CLUB_NOTICE_FLD);
			nType = NET_MSG_GCTRL_CLUB_NOTICE_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NOTICE_FLD)<=NET_DATA_BUFSIZE);

            //memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN));
            StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice);
            StringCchCopy(m_NoticeChaName, CHAR_SZNAME, NoticeChaName);
		}
        /*
        void SetNotice(const std::string& Notice)
        {
            StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice.c_str());
        }
        */
	};

	struct SNET_CLUB_NOTICE_CLT : public NET_MSG_GENERIC
	{
        __time64_t m_NoticeDate;
		char szNotice[EMCLUB_NOTICE_LEN];
        char m_NoticeChaName[CHAR_SZNAME];

		SNET_CLUB_NOTICE_CLT(
            __time64_t NoticeDate,
            const char* Notice,
            const char* NoticeChaName)
            : m_NoticeDate(NoticeDate)
		{
			dwSize = sizeof(SNET_CLUB_NOTICE_CLT);
			nType = NET_MSG_GCTRL_CLUB_NOTICE_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_NOTICE_CLT)<=NET_DATA_BUFSIZE);

			//memset(szNotice, 0, sizeof(char) * (EMCLUB_NOTICE_LEN));
            StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice);
            StringCchCopy(m_NoticeChaName, CHAR_SZNAME, NoticeChaName);
		}
        /*
        void SetNotice(const char* Notice)
        {
            if (Notice)
                StringCchCopy(szNotice, EMCLUB_NOTICE_LEN, Notice);
        }
        */
	};

	struct SNET_CLUB_MEMBER_RENAME_FLD : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
		DWORD dwCharID;
		char  szName[CHAR_SZNAME];

		SNET_CLUB_MEMBER_RENAME_FLD() 
			: dwClubID(0)
            , dwCharID(0)
		{
			memset(szName, 0, CHAR_SZNAME);
			dwSize = sizeof(SNET_CLUB_MEMBER_RENAME_FLD);
			nType = NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_RENAME_FLD)<=NET_DATA_BUFSIZE);
		}

        void SetName(const char* Name)
        {
            if (Name)
                StringCchCopy(szName, CHAR_SZNAME, Name);
        }
	};

	struct SNET_CLUB_MEMBER_RENAME_CLT : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
		DWORD dwCharID;
		char  szName[CHAR_SZNAME];

		SNET_CLUB_MEMBER_RENAME_CLT () 
			: dwClubID(0)
            , dwCharID(0)
		{
			memset ( szName, 0, CHAR_SZNAME );
			dwSize = sizeof(SNET_CLUB_MEMBER_RENAME_CLT);
			nType = NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MEMBER_RENAME_CLT)<=NET_DATA_BUFSIZE);
		}

        void SetName(const char* Name)
        {
            if (Name)
                StringCchCopy(szName, CHAR_SZNAME, Name);
        }
	};
    /*
	struct SNET_CLUB_SUBMASTER : public NET_MSG_GENERIC
	{
		DWORD dwCharID;
		DWORD dwFlags;

		SNET_CLUB_SUBMASTER () 
			: dwCharID(0)
			, dwFlags(0)
		{
			dwSize = sizeof(SNET_CLUB_SUBMASTER);
			nType = NET_MSG_GCTRL_CLUB_SUBMASTER;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_SUBMASTER)<=NET_DATA_BUFSIZE);
		}
	};
    */

	struct SNET_CLUB_SUBMASTER_FB : public NET_MSG_GENERIC
	{
		EMCLUBSUBMASTER_FB	emFB;
		DWORD				dwCharID;
		DWORD				dwFlags;

		SNET_CLUB_SUBMASTER_FB () 
			: emFB(EMCLUBSUBMASTER_FB_FAIL)
			, dwCharID(0)
			, dwFlags(0)
		{
			dwSize = sizeof(SNET_CLUB_SUBMASTER_FB);
			nType = NET_MSG_GCTRL_CLUB_SUBMASTER_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_SUBMASTER_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_CLUB_CD_CERTIFIER_CHANGE_AC : public NET_MSG_GENERIC
    {
        DWORD m_NewCdCertifier;
        
        NET_CLUB_CD_CERTIFIER_CHANGE_AC(DWORD CdCertifier)
            : m_NewCdCertifier(CdCertifier)
        {
            dwSize = sizeof(NET_CLUB_CD_CERTIFIER_CHANGE_AC);
            nType = NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_CD_CERTIFIER_CHANGE_AC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_CD_CERTIFIER_CHANGE_AF : public NET_CLUB_CD_CERTIFIER_CHANGE_AC
    {
        enum { MAGIC_NUM=2391, };
        
        DWORD m_ClubDbNum;
        //DWORD m_NewCdCertifier; 겹쳐짐
        DWORD m_OldCdCertifier;
        int   m_MagicNum;

        NET_CLUB_CD_CERTIFIER_CHANGE_AF(DWORD ClubDbNum, DWORD NewCdCertifier, DWORD OldCdCertifier)
            : NET_CLUB_CD_CERTIFIER_CHANGE_AC(NewCdCertifier)
            , m_ClubDbNum(ClubDbNum)
            , m_OldCdCertifier(OldCdCertifier)
            , m_MagicNum(MAGIC_NUM)
        {
            dwSize = sizeof(NET_CLUB_CD_CERTIFIER_CHANGE_AF);
            nType = NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AF;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_CD_CERTIFIER_CHANGE_AF)<=NET_DATA_BUFSIZE);
        }
    };
    /*
	struct SNET_CLUB_SUBMASTER_BRD : public NET_MSG_GENERIC
	{
		DWORD dwCharID;
		DWORD dwFlags;
		DWORD dwCDCertifior;

		SNET_CLUB_SUBMASTER_BRD () 
			: dwCharID(0)
			, dwFlags(0)
			, dwCDCertifior(0)
		{
			dwSize = sizeof(SNET_CLUB_SUBMASTER_BRD);
			nType = NET_MSG_GCTRL_CLUB_SUBMASTER_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_SUBMASTER_BRD)<=NET_DATA_BUFSIZE);
		}
	};
    */
    /*
	struct SNET_CLUB_SUBMASTER_FLD : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
		DWORD dwCharID;
		DWORD dwFlags;
		DWORD dwCDCertifior;

		SNET_CLUB_SUBMASTER_FLD() 
			: dwClubID(0)
			, dwCharID(0)
			, dwFlags(0)
			, dwCDCertifior(0)
		{
			dwSize = sizeof(SNET_CLUB_SUBMASTER_FLD);
			nType = NET_MSG_GCTRL_CLUB_SUBMASTER_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_SUBMASTER_FLD)<=NET_DATA_BUFSIZE);
		}
	};
    */
    
    //! Club master 변경되었음
    struct NET_CLUB_MASTER_CHNAGE_AC : public NET_MSG_GENERIC
    {
        DWORD m_NewMasterDbNum;
        WORD m_NewMasterLevel;
        char m_ChaName[CHAR_SZNAME];

        NET_CLUB_MASTER_CHNAGE_AC(DWORD NewMasterDbNum, WORD NewMasterLevel, const char* szChaName)
            : m_NewMasterDbNum(NewMasterDbNum)
            , m_NewMasterLevel(NewMasterLevel)
        {
            dwSize = sizeof(NET_CLUB_MASTER_CHNAGE_AC);
            nType = NET_MSG_CLUB_MASTER_CHNAGE_AC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_MASTER_CHNAGE_AC)<=NET_DATA_BUFSIZE);

            if (szChaName)
                StringCchCopy(m_ChaName, CHAR_SZNAME, szChaName);
        }
    };

	struct SNET_CLUB_BATTLE_REQ : public NET_MSG_GENERIC
	{
		DWORD dwCharID;	// 배틀할 클럽마스터 ID.
		DWORD dwBattleTime;

		SNET_CLUB_BATTLE_REQ() 
			: dwCharID(0)
			, dwBattleTime(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_REQ);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ : public NET_MSG_GENERIC
	{
		DWORD dwCharID;	// 배틀할 클럽마스터 ID.
		DWORD dwBattleTime;

		SNET_ALLIANCE_BATTLE_REQ()
			: dwCharID(0)
			, dwBattleTime(0)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_BATTLE_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_REQ_FB () 
			: emFB(EMCLUB_BATTLE_REQ_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_REQ_FB);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMALLIANCE_BATTLE_REQ_FB	emFB;
		
		SNET_ALLIANCE_BATTLE_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_REQ_FB_FAIL)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_FB);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_REQ_ASK : public NET_MSG_GENERIC
	{
		DWORD dwClubCharID;					//	상대편 클럽장
        DWORD dwBattleTime;
		char  szClubName[CHAR_SZNAME];		//	상대편 클럽이름

		SNET_CLUB_BATTLE_REQ_ASK () 
			: dwClubCharID(0)
			, dwBattleTime(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_REQ_ASK);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_REQ_ASK)<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetClubName(const char* ClubName)
        {
            if (ClubName)
                StringCchCopy(szClubName, CHAR_SZNAME, ClubName);
        }
	};

	struct SNET_ALLIANCE_BATTLE_REQ_ASK : public NET_MSG_GENERIC
	{
		DWORD dwClubCharID;					//	상대편 클럽장		
		DWORD dwBattleTime;
        char  szClubName[CHAR_SZNAME];		//	상대편 클럽이름.

		SNET_ALLIANCE_BATTLE_REQ_ASK () 
			: dwClubCharID(0)
			, dwBattleTime(0)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_ASK);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_REQ_ASK)<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetClubName(const char* ClubName)
        {
            if (ClubName)
                StringCchCopy(szClubName, CHAR_SZNAME, ClubName);
        }
	};

	struct SNET_CLUB_BATTLE_REQ_ANS : public NET_MSG_GENERIC
	{
		

		DWORD				dwClubCharID;
		bool				bOK;

		SNET_CLUB_BATTLE_REQ_ANS () 
			: dwClubCharID(0)
			, bOK(false)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_REQ_ANS);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_REQ_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_REQ_ANS : public NET_MSG_GENERIC
	{
		

		DWORD				dwClubCharID;
		bool				bOK;

		SNET_ALLIANCE_BATTLE_REQ_ANS () 
			: dwClubCharID(0)
			, bOK(false)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_REQ_ANS);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_REQ_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_BEGIN_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		__time64_t				tStartTime;
		__time64_t				tEndTime;

		bool					bAlliance;

		SNET_CLUB_BATTLE_BEGIN_FLD () 
			: dwChiefClubID(0)
			, dwIndianClubID(0)
			, tStartTime(0)
			, tEndTime(0)
			, bAlliance(false)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_FLD);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_BEGIN_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_BEGIN_CLT : public NET_MSG_GENERIC
	{
		char szClubName[CHAR_SZNAME];
		bool bAlliance;

		SNET_CLUB_BATTLE_BEGIN_CLT () 
			: bAlliance(false)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_CLT);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_BEGIN_CLT)<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetName(const char* ClubName)
        {
            if (ClubName)
                StringCchCopy(szClubName, CHAR_SZNAME, ClubName);
        }
	};

	struct SNET_CLUB_BATTLE_BEGIN_CLT2 : public NET_MSG_GENERIC
	{
		

		DWORD					dwClubID;
		char					szClubName[CHAR_SZNAME];
		
		__time64_t				tStartTime;
		__time64_t				tEndTime;

		bool					bAlliance;

		SNET_CLUB_BATTLE_BEGIN_CLT2 () 
			: dwClubID(0)			
			, tStartTime(0)
			, tEndTime(0)
			, bAlliance(false)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_BEGIN_CLT2);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_BEGIN_CLT2)<=NET_DATA_BUFSIZE);

			memset(szClubName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetName(const char* szName)
        {
            if (szName)
                StringCchCopy(szClubName, CHAR_SZNAME, szName);
        }
	};

    //! 클럽 배틀 휴전 요청
	struct SNET_CLUB_BATTLE_ARMISTICE_REQ : public NET_MSG_GENERIC
	{
		DWORD dwClubID;

		SNET_CLUB_BATTLE_ARMISTICE_REQ(DWORD ClubDbNum)
			: dwClubID(ClubDbNum)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ)<=NET_DATA_BUFSIZE);
		}
	};

    //! 클럽 동맹 배틀 휴전 요청
	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ : public NET_MSG_GENERIC
	{
		DWORD dwClubID;

		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ(DWORD ClubDbNum) 
			: dwClubID(ClubDbNum)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_ARMISTICE_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_BATTLE_ARMISTICE_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_ARMISTICE_REQ_FB () 
			: emFB(EMCLUB_BATTLE_ARMISTICE_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_FB);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMALLIANCE_BATTLE_ARMISTICE_REQ_FB	emFB;
		
		SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //! 클럽 배틀 휴전 요청 Agent->Client
    struct SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK : public NET_MSG_GENERIC
	{
		DWORD dwClubID;		 //! 상대편 클럽ID
        DWORD m_ReqChaDbNum; //! 요청한 캐릭터

        SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK(DWORD ReqClubDbNum, DWORD ReqChaDbNum)
			: dwClubID(ReqClubDbNum)
            , m_ReqChaDbNum(ReqChaDbNum)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK)<=NET_DATA_BUFSIZE);			
		}
	};

    //! 클럽 동맹 배틀 휴전 요청 Agent->Client
    struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK : public NET_MSG_GENERIC
	{
		DWORD dwClubID;      //! 상대편 클럽ID
        DWORD m_ReqChaDbNum; //! 요청한 캐릭터 번호

        SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK(DWORD ClubDbNum, DWORD ReqChaDbNum)
			: dwClubID(ClubDbNum)
            , m_ReqChaDbNum(ReqChaDbNum)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK)<=NET_DATA_BUFSIZE);			
		}
	};

    //! 클럽 배틀 휴전 요청 답변 Client->Agent
	struct SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
        DWORD m_ReqChaDbNum;
		bool bOK;

		SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS(DWORD ClubDbNum, DWORD ReqChaDbNum, bool OkNo)
			: dwClubID(ClubDbNum)
            , m_ReqChaDbNum(ReqChaDbNum)
			, bOK(OkNo)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_ARMISTICE_REQ_ANS)<=NET_DATA_BUFSIZE);
		}
	};

    //! 클럽 동맹 배틀 휴전 답변 Client->Agent
    struct SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS : public NET_MSG_GENERIC
	{
		DWORD dwClubID;
        DWORD m_ReqChaDbNum;
		bool  bOK;

        SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS(DWORD ClubDbNum, DWORD ReqChaDbNum, bool OkNo)
			: dwClubID(ClubDbNum)
            , m_ReqChaDbNum(ReqChaDbNum)
			, bOK(OkNo)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	//! 클럽 배틀 항복 요청 Client->Agent
    struct SNET_CLUB_BATTLE_SUBMISSION_REQ : public NET_MSG_GENERIC
	{
		DWORD dwClubID;

		SNET_CLUB_BATTLE_SUBMISSION_REQ(DWORD ClubDbNum)
			: dwClubID(ClubDbNum)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ)<=NET_DATA_BUFSIZE);
		}
	};

    //! 동맹 배틀 항복 요청 Client->Agent
    struct SNET_ALLIANCE_BATTLE_SUBMISSION_REQ : public NET_MSG_GENERIC
	{
		DWORD dwClubID;

		SNET_ALLIANCE_BATTLE_SUBMISSION_REQ(DWORD ClubDbNum) 
			: dwClubID(ClubDbNum)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_SUBMISSION_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_BATTLE_SUBMISSION_REQ_FB	emFB;
		
		SNET_CLUB_BATTLE_SUBMISSION_REQ_FB () 
			: emFB(EMCLUB_BATTLE_SUBMISSION_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ_FB);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_SUBMISSION_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMALLIANCE_BATTLE_SUBMISSION_REQ_FB		emFB;
		
		SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB () 
			: emFB(EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL)
		{
			dwSize = sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB);
			nType = NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwClubID_A;
		DWORD					dwClubID_B;

		SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD () 
			: dwClubID_A(0)
			, dwClubID_B(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT : public NET_MSG_GENERIC
	{
		DWORD					dwClubID_A;
		DWORD					dwClubID_B;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT () 
			: dwClubID_A(0)
			, dwClubID_B(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_LAST_KILL_UPDATE_AGT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_OVER_FLD : public NET_MSG_GENERIC
	{
		

		EMCLUB_BATTLE_OVER_FB	emFB;

		DWORD					dwWinClubID;
		DWORD					dwLoseClubID;

		SNET_CLUB_BATTLE_OVER_FLD () 
			: emFB(EMCLUB_BATTLE_OVER_DRAW)
			, dwWinClubID(0)
			, dwLoseClubID(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_OVER_FLD);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_OVER_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_BATTLE_OVER_CLT : public NET_MSG_GENERIC
	{
		

		EMCLUB_BATTLE_OVER_FB	emFB;

		DWORD					dwClubID;

		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_OVER_CLT () 
			: emFB(EMCLUB_BATTLE_OVER_DRAW)
			, dwClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_OVER_CLT);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_OVER_CLT)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE_AGT : public NET_MSG_GENERIC
	{
		DWORD					dwClubID;
		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE_AGT () 
			: dwClubID(0)
			, dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_AGT);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_AGT)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwClubID;
		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE_FLD () 
			: dwClubID(0)
			, dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_FLD);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_KILL_UPDATE_FLD)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_KILL_UPDATE : public NET_MSG_GENERIC
	{
		

		DWORD					dwBattleClubID;
		WORD					wKillPoint;
		WORD					wDeathPoint;

		SNET_CLUB_BATTLE_KILL_UPDATE () 
			: dwBattleClubID(0)
			, wKillPoint(0)
			, wDeathPoint(0)
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_KILL_UPDATE);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_KILL_UPDATE)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_BATTLE_POINT_UPDATE : public NET_MSG_GENERIC
	{
		
		
		DWORD					dwBattleClubID;
		BOOL					bKillPoint;

		SNET_CLUB_BATTLE_POINT_UPDATE () 
			: dwBattleClubID(0)
			, bKillPoint(false)		
		{
			dwSize = sizeof(SNET_CLUB_BATTLE_POINT_UPDATE);
			nType = NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_BATTLE_POINT_UPDATE)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNETLOBBY_CLUB_BATTLE : public NET_MSG_CHARACTER
	{
		enum { EMMAXBATTLE = 10 };

		
		DWORD				dwBattleNum;

		GLCLUBBATTLE_LOBY	sBATTLE[EMMAXBATTLE];

		SNETLOBBY_CLUB_BATTLE () :
			dwBattleNum(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD);
			nType = NET_MSG_LOBBY_CLUB_BATTLE;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CLUB_BATTLE)<=NET_DATA_BUFSIZE);
		}

		bool ADDBATTLE ( const GLCLUBBATTLE_LOBY& sBattle )
		{
			if ( EMMAXBATTLE==dwBattleNum )		return false;

			sBATTLE[dwBattleNum] = sBattle;
			++dwBattleNum;

			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(GLCLUBBATTLE_LOBY)*dwBattleNum;
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			dwBattleNum = 0;
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD);
		}
	};

	//! 클럽 동맹(연합) 요청
    struct NET_CLUB_ALLIANCE_REQ_CA : public NET_MSG_GENERIC
	{
		DWORD m_TarCharDbNum; //! 동맹에 가입될 클럽마스터 ID.

		NET_CLUB_ALLIANCE_REQ_CA () 
			: m_TarCharDbNum(0)
		{
			dwSize = sizeof(NET_CLUB_ALLIANCE_REQ_CA);
			nType = NET_MSG_CLUB_ALLIANCE_REQ_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_ALLIANCE_REQ_CA)<=NET_DATA_BUFSIZE);
		}
	};

    //! 클럽 동맹(연합) 요청
    struct NET_CLUB_ALLIANCE_REQ_ASK_AC : public NET_MSG_GENERIC
	{
		DWORD m_ChiefCharID;			  //! 동맹 우두머리
        char  m_szChiefName[CHAR_SZNAME]; //! 동맹 우두머리

		NET_CLUB_ALLIANCE_REQ_ASK_AC () 
			: m_ChiefCharID(0)
		{
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_ALLIANCE_REQ_ASK_AC)<=NET_DATA_BUFSIZE);

			dwSize = sizeof(NET_CLUB_ALLIANCE_REQ_ASK_AC);
			nType = NET_MSG_CLUB_ALLIANCE_REQ_ASK_AC;

			memset(m_szChiefName, 0, sizeof(char) * CHAR_SZNAME);
		}

        void SetName(const char* szName)
        {
            if (szName)
                StringCchCopy(m_szChiefName, CHAR_SZNAME, szName);
        }
	};

    //! 클럽 연합(동맹) 응답
	struct NET_CLUB_ALLIANCE_REQ_ANS_CA : public NET_MSG_GENERIC
	{
		DWORD m_ReqChaDbNum;
		bool  m_bOk;

		NET_CLUB_ALLIANCE_REQ_ANS_CA() 
			: m_ReqChaDbNum(0)
			, m_bOk(false)
		{
			dwSize = sizeof(NET_CLUB_ALLIANCE_REQ_ANS_CA);
			nType = NET_MSG_CLUB_ALLIANCE_REQ_ANS_CA;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_ALLIANCE_REQ_ANS_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_REQ_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_ALLIANCE_REQ_FB	emFB;
		__time64_t				tBlock;

		SNET_CLUB_ALLIANCE_REQ_FB () 
			: tBlock(0)
			, emFB(EMCLUB_ALLIANCE_REQ_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_REQ_FB);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_ADD_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		SNET_CLUB_ALLIANCE_ADD_FLD () 
			: dwChiefClubID(0)
			, dwIndianClubID(0)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_ADD_FLD);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_ADD_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_CLUB_ALLIANCE_ADD_AC : public NET_MSG_GENERIC
	{
		DWORD					dwChiefClubID;
		DWORD					dwIndianClubID;

		DWORD					dwAllianceBattleWin;
		DWORD					dwAllianceBattleLose;
		DWORD					dwAllianceBattleDraw;

        DWORD                   m_ChiefMarkVersion;
        DWORD                   m_IndianMarkVersion;

        char					szChiefClub[CHAR_SZNAME];
        char					szIndianClub[CHAR_SZNAME];

        char					m_szChiefClubMasterName[CHAR_SZNAME];
        char					m_szIndianClubMasterName[CHAR_SZNAME];

		NET_CLUB_ALLIANCE_ADD_AC(
            const char* szChiefClubMasterName,
            const char* szIndianClubMasterName,
            DWORD ChiefMarkVersion,
            DWORD IndianMarkVersion)
			: dwChiefClubID(0)
			, dwIndianClubID(0)
			, dwAllianceBattleWin(0)
			, dwAllianceBattleLose(0)
			, dwAllianceBattleDraw(0)
            , m_ChiefMarkVersion(ChiefMarkVersion)
            , m_IndianMarkVersion(IndianMarkVersion)
		{
			dwSize = sizeof(NET_CLUB_ALLIANCE_ADD_AC);
			nType = NET_MSG_CLUB_ALLIANCE_ADD_AC;
			MIN_STATIC_ASSERT(sizeof(NET_CLUB_ALLIANCE_ADD_AC)<=NET_DATA_BUFSIZE);

			memset(szChiefClub, 0, sizeof(char) * CHAR_SZNAME);
			memset(szIndianClub, 0, sizeof(char) * CHAR_SZNAME);

            if (szChiefClubMasterName)
                StringCchCopy(m_szChiefClubMasterName, CHAR_SZNAME, szChiefClubMasterName);
            if (szIndianClubMasterName)
                StringCchCopy(m_szIndianClubMasterName, CHAR_SZNAME, szIndianClubMasterName);
		}

        void SetChiefClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szChiefClub, CHAR_SZNAME, szName);
        }

        void SetIndianClubName(const char* szName)
        {
            if (szName)
                StringCchCopy(szIndianClub, CHAR_SZNAME, szName);
        }

        void SetChiefMasterName(const char* szName)
        {
            if (szName)
                StringCchCopy(m_szChiefClubMasterName, CHAR_SZNAME, szName);
        }

        void SetIndianMasterName(const char* szName)
        {
            if (szName)
                StringCchCopy(m_szIndianClubMasterName, CHAR_SZNAME, szName);
        }
	};

    //! 클럽 동맹 해지 요청
	struct SNET_CLUB_ALLIANCE_DEL_REQ : public NET_MSG_GENERIC
	{
		DWORD dwDelClubID;

		SNET_CLUB_ALLIANCE_DEL_REQ(DWORD DelClubDbNum) 
			: dwDelClubID(DelClubDbNum)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_REQ);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DEL_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_ALLIANCE_DEL_FB	emFB;

		SNET_CLUB_ALLIANCE_DEL_FB () 
			: emFB(EMCLUB_ALLIANCE_DEL_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_FB);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DEL_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_SEC_REQ : public NET_MSG_GENERIC
	{
		

		SNET_CLUB_ALLIANCE_SEC_REQ ()
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_SEC_REQ);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_SEC_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_SEC_FB : public NET_MSG_GENERIC
	{
		

		EMCLUB_ALLIANCE_SEC_FB	emFB;

		SNET_CLUB_ALLIANCE_SEC_FB () 
			: emFB(EMCLUB_ALLIANCE_SEC_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_SEC_FB);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_SEC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwAlliance;
		DWORD					dwDelClubID;

		SNET_CLUB_ALLIANCE_DEL_FLD () 
			: dwAlliance(0)
			, dwDelClubID(0)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_FLD);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DEL_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DEL_CLT : public NET_MSG_GENERIC
	{
		

		DWORD					dwDelClubID;
		char					szDelClub[CHAR_SZNAME];		

		SNET_CLUB_ALLIANCE_DEL_CLT () 
			: dwDelClubID(0)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DEL_CLT);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DEL_CLT)<=NET_DATA_BUFSIZE);

			memset(szDelClub, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_REQ : public NET_MSG_GENERIC
	{
		

		SNET_CLUB_ALLIANCE_DIS_REQ ()
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_REQ);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DIS_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_FB : public NET_MSG_GENERIC
	{
		
		EMCLUB_ALLIANCE_DIS_FB	emFB;

		SNET_CLUB_ALLIANCE_DIS_FB () 
			: emFB(EMCLUB_ALLIANCE_DIS_FB_FAIL)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_FB);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DIS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_FLD : public NET_MSG_GENERIC
	{
		

		DWORD					dwChiefClubID;

		SNET_CLUB_ALLIANCE_DIS_FLD () 
			: dwChiefClubID(CLUB_NULL)
		{
			dwSize = sizeof(SNET_CLUB_ALLIANCE_DIS_FLD);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DIS_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_ALLIANCE_DIS_CLT : public NET_MSG_GENERIC
	{
        char szChiefClub[CHAR_SZNAME];

		SNET_CLUB_ALLIANCE_DIS_CLT ()
		{
			dwSize = (DWORD) sizeof(SNET_CLUB_ALLIANCE_DIS_CLT);
			nType = NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT;
			memset (szChiefClub, 0, sizeof(char) * CHAR_SZNAME);
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ALLIANCE_DIS_CLT)<=NET_DATA_BUFSIZE);
		}

        void SetName(const char* szName)
        {
            if (szName)
                StringCchCopy(szChiefClub, CHAR_SZNAME, szName);
        }
	};

    struct NET_CLUB_AUTH_FC : public NET_MSG_GENERIC
    {
        DWORD m_AuthFlag;
        BYTE m_Index;
        char m_AuthName[CHAR_SZNAME];

        NET_CLUB_AUTH_FC(BYTE Index, const club::AUTH_GRADE& Grade)
            : m_Index(Index)
        {
            dwSize = sizeof(NET_CLUB_AUTH_FC);
            nType = NET_MSG_CLUB_AUTH_FC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_AUTH_FC)<=NET_DATA_BUFSIZE);

            m_AuthFlag = Grade.m_Flag;
            StringCchCopy(m_AuthName, CHAR_SZNAME, Grade.m_Desc.c_str());
        }
    };

    struct NET_CLUB_AUTH_AF : public NET_CLUB_AUTH_FC
    {
        DWORD m_ClubDbNum;
        NET_CLUB_AUTH_AF(DWORD ClubDbNum, BYTE Index, const club::AUTH_GRADE& Grade)
            : NET_CLUB_AUTH_FC(Index, Grade)
            , m_ClubDbNum(ClubDbNum)
        {
            dwSize = sizeof(NET_CLUB_AUTH_AF);
            nType = NET_MSG_CLUB_AUTH_AF;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_AUTH_AF)<=NET_DATA_BUFSIZE);
        }
    };
//============================================================================================
//                                  데스매치 관련 패킷    [3/13/2013 hsshin]
//============================================================================================
    struct SNET_CLUB_DEATHMATCH_START_BRD : public NET_MSG_GENERIC  //데스매치 시작 알림  agent -> client
	{
		enum { TEXT_LEN = 33, };		
		
		//	0:즉시시작, 10, 10분후시작, 30분후시작,60분후에시작.
		int nTIME;
		char szName[TEXT_LEN];

		SNET_CLUB_DEATHMATCH_START_BRD () 
			: nTIME(0)
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_START_BRD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_START_BRD)<=NET_DATA_BUFSIZE);
			memset(szName, 0, sizeof(char) * TEXT_LEN);
		}

        void SetText(const std::string& Text)
        {
            SetText(Text.c_str());
        }

        void SetText(const char* szText)
        {
            if (szText)
                StringCchCopy(szName, TEXT_LEN, szText);
        }
	};

	struct SNET_CLUB_DEATHMATCH_READY_FLD : public NET_MSG_GENERIC  //데스매치 준비 알림  agent -> field
	{
		DWORD dwID;

		SNET_CLUB_DEATHMATCH_READY_FLD()
			: dwID(0)
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_READY_FLD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_READY_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_START_FLD : public NET_MSG_GENERIC  //데스매치 시작 알림  agent -> field
	{
		DWORD dwID;

		SNET_CLUB_DEATHMATCH_START_FLD()
			: dwID(0)
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_START_FLD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_START_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_END_FLD : public NET_MSG_GENERIC    //데스매치 종료 알림  agent -> field
	{		
		DWORD dwID;

		SNET_CLUB_DEATHMATCH_END_FLD() 
			: dwID(0)
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_END_FLD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_END_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CLUB_DEATHMATCH_END_AGT : public NET_MSG_GENERIC    //데스매치 종료결과 알림  field -> agent
	{
		DWORD					dwCDMID;
		DWORD					dwClubID[RANKING_NUM];
		WORD					wClubRanking[RANKING_NUM];
		WORD					wKillNum[RANKING_NUM];
		WORD					wDeathNum[RANKING_NUM];

		SNET_CLUB_DEATHMATCH_END_AGT () 
			: dwCDMID(0)
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_END_AGT);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_END_AGT)<=NET_DATA_BUFSIZE);

			memset( dwClubID, 0, sizeof( DWORD ) * RANKING_NUM );
			memset( wClubRanking, 0, sizeof( WORD ) * RANKING_NUM );
			memset( wKillNum, 0, sizeof( WORD ) * RANKING_NUM );
			memset( wDeathNum, 0, sizeof( WORD ) * RANKING_NUM );
		}
	};

	struct SNET_CLUB_DEATHMATCH_END_BRD : public NET_MSG_GENERIC    //데스매치 종료 알림 agent -> client
	{
		enum { TEXT_LEN = 33, };

		char szName[TEXT_LEN];

		SNET_CLUB_DEATHMATCH_END_BRD()
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_END_BRD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_END_BRD)<=NET_DATA_BUFSIZE);
			memset(szName, 0, sizeof(char) * TEXT_LEN);			
		}

        void SetText(const std::string& Text)
        {
            SetText(Text.c_str());
        }

        void SetText(const char* szText)
        {
            if (szText)
                StringCchCopy(szName, TEXT_LEN, szText);
        }
	};

	struct NET_CLUB_DEATHMATCH_REMAIN_BRD                       //데스매치 종료까지 남은시간 알림 agent -> client
	{
        /*데스메치 종료까지 남은 시간에 관한 패킷입니다.
          현재 데스매치맵의 경우 1개만 존재하고 기존의 코드 또한 1개 기준으로 작성되어있었기 때문에
          여러 데스매치맵이 다른 시간대에 진행가능하도록 로직만 변경하고 패킷은 변경하지않습니다.
          (지금 기획에서는 필요하지 않는것으로 보임)
          하지만 데스매치가 '동시'진행되는 경우에 대해서는 장담할 수 없음
          추후 데스메치의 기획변경으로 인해 데스매치맵이가 1개 이상 존재하게 되어야할 경우
          이 패킷을 변경하세요. (선도전 참조)*/
        DWORD dwTime;

        MSGPACK_DEFINE( dwTime );
        //NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD
		NET_CLUB_DEATHMATCH_REMAIN_BRD(){}
	};

	struct SNET_CLUB_DEATHMATCH_POINT_UPDATE : public NET_MSG_GENERIC
	{	
		bool bKillPoint;

		SNET_CLUB_DEATHMATCH_POINT_UPDATE() 
			: bKillPoint(false)		
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_POINT_UPDATE);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_POINT_UPDATE)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_DEATHMATCH_MYRANK_UPDATE : public NET_MSG_GENERIC
	{		
		SCDM_RANK_EX sMyCdmRank;

		SNET_CLUB_DEATHMATCH_MYRANK_UPDATE()
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_MYRANK_UPDATE);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_MYRANK_UPDATE)<=NET_DATA_BUFSIZE);			
		}        
	};

	struct SNET_CLUB_DEATHMATCH_RANKING_UPDATE : public NET_MSG_GENERIC
	{
        WORD				wRankNum;		
		SCDM_RANK			sCdmRank[RANKING_NUM];

		SNET_CLUB_DEATHMATCH_RANKING_UPDATE () 
			: wRankNum(0)
		{
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_RANKING_UPDATE)<=NET_DATA_BUFSIZE);
		}

		bool ADDCLUB ( const SCDM_RANK& sRank )
		{
			if ( RANKING_NUM==wRankNum )
                return false;

			sCdmRank[wRankNum] = sRank;
			++wRankNum;

			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD) + sizeof(SCDM_RANK)*wRankNum;
			//MIN_STATIC_ASSERT(sizeof()<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET ()
		{
			wRankNum = 0;
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
		}
	};

	struct SNET_CLUB_DEATHMATCH_RANKING_REQ : public NET_MSG_GENERIC
	{	
		DWORD				dwMapID;		
		
		SNET_CLUB_DEATHMATCH_RANKING_REQ () 
			: dwMapID(UINT_MAX)		
		{
			dwSize = sizeof(SNET_CLUB_DEATHMATCH_RANKING_REQ);
			nType = NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_DEATHMATCH_RANKING_REQ)<=NET_DATA_BUFSIZE);			
		}       

	};

	struct SNET_GM_DEATHMATCH_ON_OFF_CA : public NET_MSG_GENERIC
	{
		enum
		{
			ONE = 0,
			ALL = 1,
		};

		DWORD m_dwCDMID;
		int m_nFlag;

		SNET_GM_DEATHMATCH_ON_OFF_CA(DWORD dwCDMID, int nFlag)
			: m_dwCDMID(dwCDMID)
			, m_nFlag(nFlag)
		{
			MIN_STATIC_ASSERT(sizeof(SNET_GM_DEATHMATCH_ON_OFF_CA)<=NET_DATA_BUFSIZE);

			dwSize = sizeof(SNET_GM_DEATHMATCH_ON_OFF_CA);
			nType  = NET_MSG_GCTRL_CLUB_DEATHMATCH_ON_OFF_CA; // EMNET_MSG
		}
	};
//============================================================================================


//============================================================================================
//                                  선도전 관련 패킷    [2/18/2013 hsshin]
//============================================================================================
    struct SNET_CLUB_GUID_BATTLE_START_BRD : public NET_MSG_GENERIC
    {
        enum { TEXT_LEN = 33, };		

        //	0:즉시시작, 10, 10분후시작, 30분후시작,60분후에시작.
        int nTIME;
        char szName[TEXT_LEN];
		DWORD dwGuidanceID;

        SNET_CLUB_GUID_BATTLE_START_BRD() 
            : nTIME(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_BATTLE_START_BRD);
            nType = NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_BRD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_BATTLE_START_BRD)<=NET_DATA_BUFSIZE);
            memset(szName, 0, sizeof(char) * TEXT_LEN);
			dwGuidanceID = 0;
        }

        void SetText(const std::string& Text)
        {
            SetText(Text.c_str());
        }

        void SetText(const char* szText)
        {
            if (szText)
                StringCchCopy(szName, TEXT_LEN, szText);
        }
    };

    struct SNET_CLUB_GUID_BATTLE_START_FLD : public NET_MSG_GENERIC
    {
        DWORD dwID;

        SNET_CLUB_GUID_BATTLE_START_FLD()
            : dwID(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_BATTLE_START_FLD);
            nType = NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_FLD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_BATTLE_START_FLD)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_GUID_BATTLE_END_FLD : public NET_MSG_GENERIC
    {		
        DWORD dwID;

        SNET_CLUB_GUID_BATTLE_END_FLD()
            : dwID(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_BATTLE_END_FLD);
            nType = NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_FLD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_BATTLE_END_FLD)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_GUID_BATTLE_END_BRD : public NET_MSG_GENERIC
    {
        enum { TEXT_LEN = 33, };		
        char szName[TEXT_LEN];
        char szClubName[CLUB_NAME_LENGTH];
		DWORD dwGuidanceID;

        SNET_CLUB_GUID_BATTLE_END_BRD ()
        {
            dwSize = sizeof(SNET_CLUB_GUID_BATTLE_END_BRD);
            nType = NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_BRD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_BATTLE_END_BRD)<=NET_DATA_BUFSIZE);

            memset(szName, 0, sizeof(char) * TEXT_LEN);
            memset(szClubName, 0, sizeof(char) * CLUB_NAME_LENGTH);
			dwGuidanceID = 0;
        }

        void SetName(const char* _Name)
        {
            if (_Name)
                StringCchCopy(szName, TEXT_LEN, _Name);
        }

        void SetClubName(const char* _ClubName)
        {
            if (_ClubName)
                StringCchCopy(szClubName, CLUB_NAME_LENGTH, _ClubName);
        }
    };
    
    struct NET_CLUB_GUID_BATTLE_REMAIN_BRD
    {
        struct SGUIDANCE_DATA
        {
            std::string strName;
            DWORD       dwTime;
            MSGPACK_DEFINE( strName, dwTime );
        };
        std::vector<SGUIDANCE_DATA> vecGuid;    //선도전 정보
        MSGPACK_DEFINE( vecGuid );
        //NET_MSG_GCTRL_CLUB_BATTLE_REMAIN_BRD  //선도전 종료까지 남은시간 정보 패킷
        NET_CLUB_GUID_BATTLE_REMAIN_BRD(){}
    };
    struct SNET_CLUB_CD_CERTIFY : public NET_MSG_GENERIC
    {		
        DWORD dwNpcGlobID;

        SNET_CLUB_CD_CERTIFY()
            : dwNpcGlobID(0)
        {
            dwSize = sizeof(SNET_CLUB_CD_CERTIFY);
            nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_CD_CERTIFY)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_CD_CERTIFY_FB : public NET_MSG_GENERIC
    {				
        EMCDCERTIFY	emFB;

        SNET_CLUB_CD_CERTIFY_FB()
            : emFB(EMCDCERTIFY_FAIL)
        {
            dwSize = sizeof(SNET_CLUB_CD_CERTIFY_FB);
            nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_CD_CERTIFY_FB)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_CD_CERTIFY_ING_BRD //: public NET_MSG_GENERIC
    {
        //enum { TEXT_LEN = 33, };		
        int         emFB; // EMCDCERTIFY
        std::string m_ZoneName; //[TEXT_LEN];
        std::string m_ChaName; //[CHAR_SZNAME];
        std::string m_ClubName; //[CLUB_NAME_LENGTH];
        std::string m_AllianceName; //[CLUB_NAME_LENGTH];

        MSGPACK_DEFINE(emFB, m_ZoneName, m_ChaName, m_ClubName, m_AllianceName);

        NET_CLUB_CD_CERTIFY_ING_BRD () 
            : emFB(EMCDCERTIFY_FAIL)
        {
            //MIN_STATIC_ASSERT(sizeof(NET_CLUB_CD_CERTIFY_ING_BRD)<=NET_DATA_BUFSIZE);
            //dwSize = sizeof(NET_CLUB_CD_CERTIFY_ING_BRD);
            //nType = NET_MSG_CLUB_CD_CERTIFY_ING_BRD;
        }

        void SetZone(const char* _szZone)
        {
            if (_szZone)
                m_ZoneName = _szZone;
        }

        void SetName(const char* _szName)
        {
            if (_szName)
                m_ChaName = _szName;
        }

        void SetClubName(const char* ClubName)
        {
            if (ClubName)
                m_ClubName = ClubName;
        }

        void SetAlliance(const char* Alliance)
        {
            if (Alliance)
                m_AllianceName = Alliance;
        }
    };

    struct SNET_CLUB_CD_CERTIFY_BRD : public NET_MSG_GENERIC
    {
        enum { TEXT_LEN = 33, };


        char					szZone[TEXT_LEN];
        char					szName[CHAR_SZNAME];
        char					szClub[CHAR_SZNAME];
        char					szAlliance[CHAR_SZNAME];


        SNET_CLUB_CD_CERTIFY_BRD ()
        {
            dwSize = sizeof(SNET_CLUB_CD_CERTIFY_BRD);
            nType = NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_CD_CERTIFY_BRD)<=NET_DATA_BUFSIZE);

            memset(szZone, 0, sizeof(char) * TEXT_LEN);
            memset(szName, 0, sizeof(char) * CHAR_SZNAME);
            memset(szClub, 0, sizeof(char) * CHAR_SZNAME);
            memset(szAlliance, 0, sizeof(char) * CHAR_SZNAME);
        }
    };

    struct NET_CLUB_CERTIFIED_FA : public NET_MSG_GENERIC
    {	
        DWORD m_GuidanceId;
        DWORD m_ClubDbNum;

        NET_CLUB_CERTIFIED_FA () 
            : m_GuidanceId(0)
            , m_ClubDbNum(CLUB_NULL)
        {
            dwSize = sizeof(NET_CLUB_CERTIFIED_FA);
            nType = NET_MSG_CLUB_CERTIFIED_FA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_CERTIFIED_FA)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_CERTIFIED_AF : public NET_MSG_GENERIC
    {		
        DWORD m_GuidanceId;
        DWORD m_ClubDbNum;

        NET_CLUB_CERTIFIED_AF () 
            : m_GuidanceId(0)
            , m_ClubDbNum(CLUB_NULL)
        {
            dwSize = sizeof(NET_CLUB_CERTIFIED_AF);
            nType = NET_MSG_CLUB_CERTIFIED_AF;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_CERTIFIED_AF)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_GUID_COMMISSION : public NET_MSG_GENERIC
    {

        float					fCommission;

        SNET_CLUB_GUID_COMMISSION () 
            : fCommission(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_COMMISSION);
            nType = NET_MSG_GCTRL_CLUB_COMMISSION;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_COMMISSION)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_GUID_COMMISSION_FB : public NET_MSG_GENERIC
    {


        EMGUIDCOMMISSION_FB		emFB;
        float					fCommission;

        SNET_CLUB_GUID_COMMISSION_FB () 
            : emFB(EMGUIDCOMMISSION_FB_FAIL)
            , fCommission(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_FB);
            nType = NET_MSG_GCTRL_CLUB_COMMISSION_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_COMMISSION_FB)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_GUID_COMMISSION_FLD : public NET_MSG_GENERIC
    {
        DWORD dwID;
        float fCommission;

        NET_CLUB_GUID_COMMISSION_FLD(DWORD _dwID, float _fCommission)
            : dwID(_dwID)
            , fCommission(_fCommission)
        {
            dwSize = sizeof(NET_CLUB_GUID_COMMISSION_FLD);
            nType = NET_MSG_GCTRL_CLUB_COMMISSION_FLD;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_GUID_COMMISSION_FLD)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CLUB_GUID_COMMISSION_BRD : public NET_MSG_GENERIC
    {
        float fCommission;
        NET_CLUB_GUID_COMMISSION_BRD(float _fCommission) 
            : fCommission(_fCommission)
        {
            dwSize = sizeof(NET_CLUB_GUID_COMMISSION_BRD);
            nType = NET_MSG_GCTRL_CLUB_COMMISSION_BRD;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_GUID_COMMISSION_BRD)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_CLUB_GUID_COMMISSION_RESERVE_BRD : public NET_MSG_GENERIC
    {
        float					fCommission;

        SNET_CLUB_GUID_COMMISSION_RESERVE_BRD () 
            : fCommission(0)
        {
            dwSize = sizeof(SNET_CLUB_GUID_COMMISSION_RESERVE_BRD);
            nType = NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD;
            MIN_STATIC_ASSERT(sizeof(SNET_CLUB_GUID_COMMISSION_RESERVE_BRD)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_CLUB_ID_2MASTERNAME_REQ_CA //: public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		enum { EMMAXID = 10 };		

		std::vector<DWORD> m_vecClubId; //[EMMAXMEMBER];

		MSGPACK_DEFINE(m_vecClubId);

		SNET_CLUB_ID_2MASTERNAME_REQ_CA()
		{
			//dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
			//nType = NET_MSG_CLUB_ID_2MASTERNAME_REQ_CA;
			//MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ID_2MASTERNAME_REQ_CA)<=NET_DATA_BUFSIZE);
		}

		bool ADDID(const DWORD& Member)
		{
			if (m_vecClubId.size() >= EMMAXID)
				return false;

			m_vecClubId.push_back(Member);
			return true;
		}

		void RESET()
		{
			m_vecClubId.clear();
		}
	};

	struct SNET_CLUB_ID_2MASTERNAME_ANS_AC //: public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		enum { EMMAXID = 10 };		

		std::vector<std::string> m_vecMasterName; //[EMMAXID];
		std::vector<std::string> m_vecClubName; //[EMMAXID];

		MSGPACK_DEFINE(m_vecMasterName,m_vecClubName);

		SNET_CLUB_ID_2MASTERNAME_ANS_AC()
		{
			//dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD);
			//nType = SNET_CLUB_ID_2MASTERNAME_ANS_AC;
			//MIN_STATIC_ASSERT(sizeof(SNET_CLUB_ID_2MASTERNAME_ANS_AC)<=NET_DATA_BUFSIZE);
		}

		bool ADDMASTERNAME(const std::string& MasterName)
		{
			if (m_vecMasterName.size() >= EMMAXID)
				return false;

			m_vecMasterName.push_back(MasterName);
			return true;
		}

		bool ADDCLUBNAME(const std::string& clubName)
		{
			if (m_vecClubName.size() >= EMMAXID)
				return false;

			m_vecClubName.push_back(clubName);
			return true;
		}

		void RESET()
		{
			m_vecMasterName.clear();
			m_vecClubName.clear();
		}
	};

	struct SNET_CLUB_STORAGE_USER_RESET : public NET_MSG_GENERIC
	{
		DWORD ClubNum;
		SNET_CLUB_STORAGE_USER_RESET()
			: ClubNum( 0 )
		{
			dwSize = sizeof( SNET_CLUB_STORAGE_USER_RESET );
			nType = NET_MSG_GCTRL_CLUB_STORAGE_USER_RESET;
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_STORAGE_USER_RESET ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_CLUB_STORAGE_MONEY_USER_RESET : public NET_MSG_GENERIC
	{
		DWORD ClubNum;
		SNET_CLUB_STORAGE_MONEY_USER_RESET()
			: ClubNum( 0 )
		{
			dwSize = sizeof( SNET_CLUB_STORAGE_MONEY_USER_RESET );
			nType = NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_RESET;
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_STORAGE_MONEY_USER_RESET ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_CLUB_STORAGE_USER_FB
	{
		std::string ChaName;
		MSGPACK_DEFINE( ChaName );
		SNET_CLUB_STORAGE_USER_FB()
		{
		}
		SNET_CLUB_STORAGE_USER_FB( const std::string& Name )
		{
			ChaName = Name;
		}
	};

	//! 클럽 이름 변경 요청 Client->Field
	struct SNET_CLUB_RENAME_CF : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_CF()
			: wPosX( 0 )
			, wPosY( 0 )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_CF );
			nType = NET_MSG_GCTRL_CLUB_RENAME_CF;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_CF ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	//! 클럽 이름 변경 결과 DB->Field
	struct SNET_CLUB_RENAME_DF : public NET_MSG_GENERIC
	{
		DWORD ClubNum;
		DWORD ChaDbNum;
		int nResult;	// 0 성공, 0이 아닌 값은 실패
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_DF()
			: ClubNum( CLUB_NULL )
			, ChaDbNum( 0 )
			, nResult( -1 )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_DF );
			nType = NET_MSG_GCTRL_CLUB_RENAME_DF;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_DF ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	// 클럽 이름 변경 알림 Field->Agent
	struct SNET_CLUB_RENAME_FA : public NET_MSG_GENERIC
	{
		enum { MAGIC_NUM = 719907, };
		int MagicNum;
		DWORD ClubNum;
		DWORD ChaDbNum;
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_FA()
			: MagicNum( MAGIC_NUM )
			, ClubNum( CLUB_NULL )
			, ChaDbNum( 0 )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_FA );
			nType = NET_MSG_GCTRL_CLUB_RENAME_FA;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_FA ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	// 클럽 이름 변경 알림 Agent->Field
	struct SNET_CLUB_RENAME_AF : public NET_MSG_GENERIC
	{
		DWORD ClubNum;
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_AF()
			: ClubNum( CLUB_NULL )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_AF );
			nType = NET_MSG_GCTRL_CLUB_RENAME_AF;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_AF ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	struct SNET_CLUB_RENAME_2CLT : public NET_MSG_GENERIC
	{
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_2CLT ()
		{
			dwSize = sizeof( SNET_CLUB_RENAME_2CLT );
			nType = NET_MSG_GCTRL_CLUB_RENAME_CLT;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_2CLT ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	struct SNET_CLUB_RENAME_BRD : public SNETPC_BROAD
	{
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_BRD ()
		{
			dwSize = sizeof( SNET_CLUB_RENAME_BRD );
			nType = NET_MSG_GCTRL_CLUB_RENAME_BRD;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_BRD ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	struct SNET_CLUB_RENAME_2ALLIANCE : public NET_MSG_GENERIC
	{
		DWORD ClubDbNum;
		char szClubName[CHAR_SZNAME];

		SNET_CLUB_RENAME_2ALLIANCE()
			: ClubDbNum( CLUB_NULL )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_2ALLIANCE );
			nType = NET_MSG_GCTRL_CLUB_RENAME_2ALLIANCE;
			memset( szClubName, 0, sizeof( char ) * CHAR_SZNAME );
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_2ALLIANCE ) <= NET_DATA_BUFSIZE );
		}

		void SetClubName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szClubName, CHAR_SZNAME, szName );
		}
	};

	// 클럽 이름 변경 FB Field->Client
	struct SNET_CLUB_RENAME_FB : public NET_MSG_GENERIC
	{
		int nResult;

		SNET_CLUB_RENAME_FB()
			: nResult( CLUB_RENAME_FB_OK )
		{
			dwSize = sizeof( SNET_CLUB_RENAME_FB );
			nType = NET_MSG_GCTRL_CLUB_RENAME_FB;
			MIN_STATIC_ASSERT( sizeof( SNET_CLUB_RENAME_FB ) <= NET_DATA_BUFSIZE );
		}
	};

//============================================================================================
};

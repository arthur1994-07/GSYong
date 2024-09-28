#pragma once

#include "../s_NetGlobal.h"
#include "../../RanLogic/PVE/MapTable.h"
#include "../../RanLogic/PVE/PVEInDunID.h"
#include "../../SigmaCore/String/MinGuid.h"

class DxMsgServer;

/**
	PVE 관련 메시지들을 정의 한다.
	PVE 는 현재 무한의 계단 1개만 있는 상황이다.
	
    큰 틀의 이해는 아래 패킷 플로우 위키를 보며 선행적으로 흐름을 생각해보면 이해할 수 있을꺼라 생각한다.
    그 후 세부적인 부분은 실제 코드를 보며 이해하면 된다.
	패킷 플로우는 http://minwiki.mincoms.co.kr/index.php/PVE_%EB%AC%B4%ED%95%9C%EC%9D%98_%EA%B3%84%EB%8B%A8 참조 한다.

 */
namespace GLMSG
{
    /**
        1. 인던 정보 처리

        NET_PVE_MAPTABLE_REQ_CA : PVE 맵 테이블 요청
        NET_PVE_MAPTABLE_ACK_AC : PVE 맵 테이블 응답
     */

    struct NET_PVE_MAPTABLE_REQ_CA
    {
        DWORD dwGaeaID;

        MSGPACK_DEFINE(dwGaeaID);
        NET_PVE_MAPTABLE_REQ_CA() : dwGaeaID(0) {}
    };

    struct NET_PVE_MAPTABLE_ACK_AC
    {
        MapTableContainer tables;			//@ 자신이 만들거나 했던 인던 정보들
		MAP_TABLE table;					//@ 현재 자신이 속한 인던 정보

		MSGPACK_DEFINE(tables, table);
    };

    /**
        2. 인던 입장 처리

        NET_PVE_ENTRANCE_REQ_CA : 요청
        NET_PVE_ENTRANCE_ACK_AC : 응답
     */

	enum ENTRANCE_ACK
	{
		ENTRANCE_ACK_NONE,					//@ 입장 OK
		ENTRANCE_ACK_NOT_ENTRANCE,   		//@ 파티 입장 조건에 맞지 않습니다.
	};

    struct NET_PVE_ENTRANCE_REQ_CA
    {
        SPVEInDunID sInDunID;				//@ 입장 인던 ID
        DWORD dwGateID;						//@ 입장 게이트

		MSGPACK_DEFINE(sInDunID, dwGateID);
        NET_PVE_ENTRANCE_REQ_CA() : dwGateID(0) {}
    };

    struct NET_PVE_ENTRANCE_ACK_AC
    {
        SPVEInDunID sInDunID;
		BYTE byResult;		

		NET_PVE_ENTRANCE_ACK_AC() : byResult(ENTRANCE_ACK_NONE) {}
        MSGPACK_DEFINE(sInDunID, byResult);
    };

    /**
        3. 인던 입장 확인 처리

        NET_PVE_ENTRANCE_CONFIRM_REQ_CA : 요청
        NET_PVE_ENTRANCE_CONFIRM_ACK_AC : 응답
     */

    enum ENTRANCE_CONFIRM_ACK
    {
        ENTRANCE_ACK_CONFIRM_NONE,        
        ENTRANCE_ACK_CONFIRM_TIMEOVER,      //@ 입장 대기 시간 오버로 인한 실패
        ENTRANCE_ACK_CONFIRM_CANCEL,        //@ 파티원 입장 취소로 인한 실패        
        ENTRANCE_ACK_CONFIRM_OK,            //@ 파티원 수락
        ENTRANCE_ACK_CONFIRM_IMPOSSIBLE,    //! 입장 불가능 지역
    };

    enum ENTRANCE_CONFIRM_REQ
    {
        ENTRANCE_CONFIRM_REQ_FALSE = 0,     //! 입장 취소
        ENTRANCE_CONFIRM_REQ_TRUE,          //! 입장
        ENTRANCE_CONFIRM_REQ_IMPOSSIBLE     //! 입장 불가
    };
    struct NET_PVE_ENTRANCE_CONFIRM_REQ
    {
        DWORD       dwChaDbNum;
        BYTE        byOk;                    //@ 입장 확인 결과 (true = 입장, false = 입장 취소)
        SPVEInDunID sInDunID;

        MSGPACK_DEFINE(dwChaDbNum, byOk, sInDunID);
        NET_PVE_ENTRANCE_CONFIRM_REQ() : byOk(false) {}
    };

    struct NET_PVE_ENTRANCE_CONFIRM_ACK_AC
    {
        DWORD dwChaDbNum;
        BYTE byResult;                      //@ 입장 확인 결과에 대한 처리(ENTRANCE_CONFIRM_ACK 값을 가진다)

        NET_PVE_ENTRANCE_CONFIRM_ACK_AC() : byResult(ENTRANCE_ACK_CONFIRM_NONE) {}
        MSGPACK_DEFINE(byResult, dwChaDbNum);
    };

    /**
        4. 인던 귀속 처리

        NET_PVE_CLEAROFBELONGING_REQ_CA : 해제 요청
		NET_PVE_CLEAROFBELONGING_ACK_AC : 해제 결과

     */

    struct NET_PVE_CLEAROFBELONGING_REQ_CA
    {
        SPVEInDunID sInDunID;

		MSGPACK_DEFINE(sInDunID);
    };

	struct NET_PVE_CLEAROFBELONGING_ACK_AC
	{
		SPVEInDunID sInDunID;

		MSGPACK_DEFINE(sInDunID);
	};


    /**
        5. 인던 생성 및 입장 처리

        NET_PVE_CREATE_INSTANT_MAP_REQ_AF : 인던 생성 요청
        NET_PVE_CREATE_INSTANT_MAP_ACK_FA : 인던 생성 요청 결과 응답
        NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF : 인던 입장 요청
        NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA : 인던 입장 요청 결과 응답
     */

    struct NET_PVE_CREATE_INSTANT_MAP_REQ_AF
    {
        int         nStage;                 //@ 시작 스테이지
        SPVEInDunID sInDunID;               //@ 생성 인던 고유 구분자
        DWORD	    dwBaseMapID;            //@ 실제 생성 할 인스턴스 맵의 기본 ID
        DWORD	    dwInstanceMapID;        //@ 실제 생성 할 인스턴스 맵 ID
		GLPartyID	sPartyID;				//@ 인던 생성용 파티ID, 인던을 생성하려면 필요
        std::string strGuid;                //! 로그를 남기기위한 인던 고유ID

        MSGPACK_DEFINE(nStage, sInDunID, dwBaseMapID, dwInstanceMapID, sPartyID, strGuid);
        NET_PVE_CREATE_INSTANT_MAP_REQ_AF() : nStage(0), dwBaseMapID(0), dwInstanceMapID(0){}
    };

    struct NET_PVE_CREATE_INSTANT_MAP_ACK_FA
    {
        bool        bResult;                //@ 생성 요청 결과
		DWORD	    dwInstanceMapID;        //@ 생성 한 인스턴스 맵 ID
        SPVEInDunID sInDunID;               //@ 생성 인던 고유 구분자

        MSGPACK_DEFINE(bResult, dwInstanceMapID, sInDunID);
        NET_PVE_CREATE_INSTANT_MAP_ACK_FA() : bResult(false), dwInstanceMapID(0) {}
    };

    struct NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF
    {
        DWORD	    dwCharDbNum;            //@ 입장할 캐릭터의 Db ID
        DWORD	    dwMapID;                //@ 입장할 인던 맵 ID(인던 고유 ID 의 맵ID랑은 다름에 주의)
        DWORD	    dwGateID;               //@ 입장할 게이트 ID
        SPVEInDunID sInDunID;               //@ 인던 고유 ID, 응답용으로 사용 한다.

        MSGPACK_DEFINE(dwCharDbNum, dwMapID, dwGateID, sInDunID);
        NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF() : dwCharDbNum(0), dwMapID(0), dwGateID(0) {}
    };

    struct NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA
    {
        bool        bResult;                //@ 입장 요청 결과
        DWORD	    dwCharDbNum;            //@ 입장 한 캐릭터의 Db ID
        SPVEInDunID sInDunID;               //@ 입장 인던 고유 구분자

        MSGPACK_DEFINE(bResult, dwCharDbNum, sInDunID);
        NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA() : bResult(false), dwCharDbNum(0) {}
    };

    /**
        6. 인던 입장 상태 처리

        NET_PVE_ENTRANCE_STATE_FC : 입장 상태 전송

     */

    struct NET_PVE_ENTRANCE_STATE_FC
    {
        int nCur;                           //@ 현재 입장 인원
        int nAll;                           //@ 총 입장 인원
        int nTime;                          //@ 입장 대기 남은 시간(sec)
		int nStage;							//@ 현재 스테이지

        MSGPACK_DEFINE(nCur, nAll, nTime, nStage);
        NET_PVE_ENTRANCE_STATE_FC() : nCur(0), nAll(0), nTime(0), nStage(0) {}
    };

    /**
        7. 인던 스테이지 상태 처리

        NET_PVE_START_STAGE_FC : 스테이지 시작 전송
        NET_PVE_MSG_START_STAGE_FC : 스테이지 시작 메시지 전송
        NET_PVE_MSG_FC : 스테이지내 출력 메시지 전송
        NET_PVE_MONSTERCLEAR_STATE_FC : 몬스터 상태 전송
        NET_PVE_POSITIONINGCLEAR_STATE_FC : 위치 잡기 상태 전송
        NET_PVE_STAGE_STATE_FC : 스테이지 상태 전송

     */

    enum
    {
        STAGE_STATE_ONGOING = 0,            //@ 진행중
        STAGE_STATE_TIMEOVER,               //@ 스테이지 타임 오버
        STAGE_STATE_STAGECLEAR,             //@ 스테이지 클리어
        STAGE_STATE_RETRY,                  //@ 재도전 상태
        STAGE_STATE_ALLDIED,                //@ 전부 죽음
        STAGE_STATE_DIED,                   //@ 자신 죽음

        STAGE_STATE_GAMEROVER_FIREPARTY,    //@ 게임 종료 : 파티 종료
        STAGE_STATE_GAMEROVER_TIMEOVER,     //@ 게임 종료 : 인던 시간 종료
        STAGE_STATE_GAMEROVER_NOTENOUGH,    //@ 게임 종료 : 재도전 실패 - 돈 부족
        STAGE_STATE_GAMEROVER_WAITTIME,     //@ 게임 종료 : 재도전 실패 - 대기 시간 오버
        STAGE_STATE_GAMEROVER_NOTTRY,       //@ 게임 종료 : 재도전 실패 - 파티원 중 포기
        STAGE_STATE_GAMEROVER_COMPLETED,    //@ 게임 종료 : 모든 스테이지 완료
		STAGE_STATE_GAMEROVER_FAILED,		//@ 게임 종료 : 기타
        STAGE_STATE_GAMEROVER_RETRY,		//@ 게임 종료 : 기타
    };

    struct NET_PVE_START_STAGE_FC
    {
        int         nStage;                 //@ 시작 스테이지
        SPVEInDunID sInDunID;               //@ 입장 인던 고유 구분자
        DWORD       dwCharDbNum;            //@ 스테이지 접속 요청

        MSGPACK_DEFINE(nStage, sInDunID, dwCharDbNum);
        NET_PVE_START_STAGE_FC() : nStage(0), dwCharDbNum(0) {}
    };

    struct NET_PVE_MSG_START_STAGE_FC
    {
        int nStage;                         //@ 현재 시작 스테이지
        int	nTime;                          //@ 메시지 출력 시간

        MSGPACK_DEFINE(nStage, nTime);
        NET_PVE_MSG_START_STAGE_FC() : nStage(0), nTime(0) {}
    };

    struct NET_PVE_MSG_FC
    {
        std::string strMsg;                 //@ 메시지 내용(XML 키워드)
        int	nTime;                          //@ 메시지 출력 시간

        MSGPACK_DEFINE(strMsg, nTime);
        NET_PVE_MSG_FC() : nTime(0) {}
    };

    struct NET_PVE_MONSTERCLEAR_STATE_FC
    {
        int nCur;                           //@ 현재 잡은 몬스터 수
        int nAll;                           //@ 총 잡아야 하는 몬스터 수

        MSGPACK_DEFINE(nCur, nAll);
        NET_PVE_MONSTERCLEAR_STATE_FC() : nCur(0), nAll(0) {}
    };

    struct NET_PVE_POSITIONINGCLEAR_STATE_FC
    {
        int nX;                             //@ 이동 해야 할 위치 X
        int nY;                             //@ 이동 해야 할 위치 Y
        int nNow;                           //@ 현재 완료한 사람
        int nNeed;                          //@ 미션 완료에 필요한 사람
		bool bCompleted;					//@ 미션 완료 유무

        MSGPACK_DEFINE(nX, nY, nNow, nNeed, bCompleted);
        NET_PVE_POSITIONINGCLEAR_STATE_FC() : nX(0), nY(0), nNow(0), nNeed(0), bCompleted(false) {}
    };

    struct NET_PVE_STAGE_STATE_FC
    {
        int nStage;                         //@ 현재 스테이지
        int nTime;                          //@ 남은 시간
        int nState;                         //@ 스테이지 진행 상태

        MSGPACK_DEFINE(nStage, nTime, nState);
        NET_PVE_STAGE_STATE_FC() : nStage(0), nTime(0), nState(STAGE_STATE_ONGOING) {}
    };

    /**
        8. 인던 재도전 요청
         - 모두 사망하거나 시간이 오버되었을시 재도전 할 수 있다.

         NET_PVE_RETRY_REQ_CF: 재도전 요청
		 NET_PVE_RETRY_ACK_FC: 재도전 결과 응답
         NET_PVE_RETRY_REQ_FA: 재도전 요청
     */

	enum PVE_RETRY_RESULT_CODE
	{
		RESULT_CODE_CANCEL,					//@ 파티원의 취소에 의한 실패
		RESULT_CODE_NOT_ENOUGH,				//@ 자신의 머니 부족에 의한 실패
		RESULT_CODE_ANOTHER_NOT_ENOUGH,		//@ 타 파티원 머니 부족에 의한 실패
		RESULT_CODE_TIMEOVER,				//@ 재도전 대기 시간 종료에 의한 실패
        RESULT_CODE_OK,				        //! 수락
	};

    struct NET_PVE_RETRY_REQ_CF
    {
        bool bOk;							//@ true : 재도전 함, No : 재도전 안함

        MSGPACK_DEFINE(bOk);
        NET_PVE_RETRY_REQ_CF() : bOk(false) {}
    };

	struct NET_PVE_RETRY_ACK_FC
	{
        DWORD dwChaDbNum;
		int nResultCode;

		MSGPACK_DEFINE(dwChaDbNum, nResultCode);
		NET_PVE_RETRY_ACK_FC() : nResultCode(RESULT_CODE_CANCEL) {}
	};

    struct NET_PVE_RETRY_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ 인던 고유 구분자

        MSGPACK_DEFINE(sInDunID);
    };

    /**
        9. 인던 포탈 및 이동 처리
         - 기존 포탈로는 원하는 제어를 하기 힘들어 새로 포탈 패킷을 만듬
         - 보안을 위해 현재 던전 상태와 맞아야 포탈을 시킨다.

         NET_PVE_PORTAL_STATE_FC : 포탈 상태(활성, 비활성) 설정 요청
         NET_PVE_PORTAL_REQ_CF : 포탈 이동 요청
         NET_PVE_MOVE_REQ_FA : 다음 스테이지 인던으로 이동 요청
         NET_PVE_RETURN_REQ_AF : 귀환 요청
         NET_PVE_RETURN_REQ_FA : 귀환 요청
     */

    struct NET_PVE_PORTAL_STATE_FC
    {
        bool  bOpen;                        //@ 게이트 활성, 비활성 유무
        DWORD dwGateID;                     //@ 게이트 ID
        DWORD dwToGateID;                   //@ 이동할 게이트 ID
        DWORD dwToMapID;                    //@ 이동할 맵 ID

        MSGPACK_DEFINE(bOpen, dwGateID, dwToGateID, dwToMapID);
        NET_PVE_PORTAL_STATE_FC() : bOpen(false), dwGateID(0), dwToGateID(0), dwToMapID(0) {}
    };

    struct NET_PVE_PORTAL_REQ_CF
    {
        DWORD dwFromMapID;                  //@ 요청 맵 ID
        DWORD dwFromGateID;                 //@ 요청 게이트 ID
        DWORD dwToGateID;                   //@ 이동할 게이트 ID
        DWORD dwToMapID;                    //@ 이동할 맵 ID

        MSGPACK_DEFINE(dwFromMapID, dwFromGateID, dwToGateID, dwToMapID);
        NET_PVE_PORTAL_REQ_CF() : dwFromMapID(0), dwFromGateID(0), dwToGateID(0), dwToMapID(0) {}
    };

    struct NET_PVE_MOVE_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ 생성 인던 고유 구분자        
        DWORD       dwToGateID;             //@ 이동할 게이트 ID
        DWORD       dwToMapID;              //@ 이동할 맵 ID
        int         nStage;                 //@ 이동 후 재생할 인던 스테이지
		bool		bIsItem;				//@ 아이템 사용인지
		WORD		wPosX;					//@ 인벤토리 X좌표
		WORD		wPosY;					//@ 인벤토리 y좌표
		DWORD		dwCharDbNum;			//Q 케릭터 DB NUM

        MSGPACK_DEFINE(sInDunID, dwToGateID, dwToMapID, nStage, bIsItem, wPosX, wPosY, dwCharDbNum);
        NET_PVE_MOVE_REQ_FA() : dwToGateID(0), dwToMapID(0), nStage(0), bIsItem(false), wPosX(0), wPosY(0), dwCharDbNum(0) {}
    };

    struct NET_PVE_RETURN_REQ_AF
    {
        DWORD dwCharDbNum;                  //@ 귀환 시킬 캐릭터 DB Num
        int nWhy;                           //@ 리턴 사유

        MSGPACK_DEFINE(dwCharDbNum, nWhy);
        NET_PVE_RETURN_REQ_AF() : dwCharDbNum(0), nWhy(0) {}
    };

    struct NET_PVE_RETURN_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ 인던 고유 구분자
        int nWhy;                           //@ 리턴 사유

        MSGPACK_DEFINE(sInDunID, nWhy);
        NET_PVE_RETURN_REQ_FA() : nWhy(0) {}
    };

    /**
        10. 인던 스테이지 결과(보상) 처리

        NET_PVE_STAGE_RESULT_FC : 스테이지 완전 종료(모두 클리어 or 포기)시 클라에 인던 스테이지 결과(보상) 정보 전송
        NET_PVE_STAGE_RESULT_FA_OR_AF : 계속 연속해서 스테이지를 처리해야 하므로 릴레이를 위해 필요 이전 스테이지 결과 전송

     */

    struct CharResult
    {
        DWORD       charDbNum;              //@ 캐릭터 DB ID
        std::string chaName;                //@ 캐릭명
        DWORD       dwClass;                //@ 캐릭터 클래스
        DWORD       dwDamage;               //@ 피해량
        DWORD       dwHeal;                 //@ 치유량
        bool        bExp;                   //@ 경험치 보상 유무
        bool        bMoney;                 //@ 금전 보상 유무
        bool        bMVP;                   //@ MVP 유무

        MSGPACK_DEFINE(charDbNum, chaName, dwClass, dwDamage, dwHeal, bExp, bMoney, bMVP);
    };

    typedef std::vector<CharResult> StageResult;
    typedef StageResult::iterator StageResultIt;
    typedef StageResult::const_iterator StageResultCIt;

    struct NET_PVE_STAGE_RESULT_FC
    {
        StageResult results;                //@ 스테이지 결과
        int         nTimer;                 //@ 스테이지 결과를 보여줄 시간
        int         nStage;                 //@ 현재 스테이지
        bool        bLast;                  //@ 마지막 스테이지인지 여부

        MSGPACK_DEFINE(results, nTimer, nStage, bLast);
        NET_PVE_STAGE_RESULT_FC() : nTimer(0), nStage(0), bLast(false) {}
    };

    struct NET_PVE_STAGE_RESULT_FA_OR_AF
    {
        StageResult results;                //@ 스테이지 결과
        SPVEInDunID sInDunID;               //@ 인던 고유 구분자
        SNATIVEID   sNativeInDunID;         //@ 실제 생성된 인던 ID

        MSGPACK_DEFINE(results, sInDunID, sNativeInDunID);
        NET_PVE_STAGE_RESULT_FA_OR_AF() {}
    };

    /**
        11. 인던 나가기 처리
		 - 스테이지가 종료(실패 or 성공) 되면 나가기 버튼을 통해 나갈 수 있다.
        NET_PVE_EXIT_REQ_CF : 인던 나가기 요청
        
     */

    struct NET_PVE_EXIT_REQ_CF
    {
        DWORD dwCharDbNum;                  //@ 나가기 요청한 Char		
        SPVEInDunID sInDunID;               //@ 인던 고유 구분자

        MSGPACK_DEFINE(dwCharDbNum, sInDunID);
        NET_PVE_EXIT_REQ_CF() : dwCharDbNum(0) {}
    };

	/**
        12. 기타.
        NET_PVE_SAVEPOS_REQ_AF : 현재 위치 저장 요청, 입장 전 현재 위치 저장용
        NET_PVE_REMOVE_INDUN_AC : 인던 소멸 알림, 인던이 소멸되었을 경우 알려준다.
		NET_PVE_REBIRTH_FC : 제자리에서 로딩 없이 부활을 요청 한다.
		NET_PVE_GM_RELOAD_TRIGGER : 서버에 트리거를 재로딩 하도록 요청 한다.(기획팀 설정 편의용)
     */

    struct NET_PVE_SAVEPOS_REQ_AF
    {
        DWORD dwCharDbNum;                  //@ 위치 저장 요청한 Char

        MSGPACK_DEFINE(dwCharDbNum);
        NET_PVE_SAVEPOS_REQ_AF() : dwCharDbNum(0) {}
    };

	struct NET_PVE_REMOVE_INDUN_AC
	{
		SPVEInDunID sInDunID;               //@ 인던 고유 구분자

		MSGPACK_DEFINE(sInDunID);
	};

	struct NET_PVE_REBIRTH_FC
	{
		DWORD dwGaeaID;						//@ 부활할 캐릭터

		MSGPACK_DEFINE(dwGaeaID);
		NET_PVE_REBIRTH_FC() : dwGaeaID(0) {}
	};

	/**
		13. GM 명령어
		
		// 리로딩 GM 명령어
		NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF
		NET_PVE_GM_RELOAD_TRIGGER_ACK_AC
		NET_PVE_GM_RELOAD_TRIGGER_ACK_FA

		// 특정 스테이지 실행 명령어
		NET_PVE_GM_GOSTAGE_CA_OR_AF
	 */

	struct NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF
	{
		DWORD dwCharDbNum;					//@ 리로딩을 요청한 캐릭터 DB

		MSGPACK_DEFINE(dwCharDbNum);
		NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF() : dwCharDbNum(0) {}
	};

	struct NET_PVE_GM_RELOAD_TRIGGER_ACK_AC
	{
		bool bResult;						//@ 리로딩 성공 유무
		bool bAgent;						//@ 리로딩 한 서버(true : 에이전트, false : 필드)
		DWORD dwFieldSvr;					//@ 리로딩 한 서버가 필드시 몇번 필드 인지.

		MSGPACK_DEFINE(bResult, bAgent, dwFieldSvr);
		NET_PVE_GM_RELOAD_TRIGGER_ACK_AC() : bResult(false), bAgent(true), dwFieldSvr(0) {}
	};

	struct NET_PVE_GM_RELOAD_TRIGGER_ACK_FA
	{
		bool bResult;						//@ 리로딩 성공 유무
		DWORD dwFieldSvr;					//@ 리로딩한 필드 서버 넘버
		DWORD dwCharDbNum;					//@ 리로딩을 요청한 캐릭터 DB

		MSGPACK_DEFINE(bResult, dwFieldSvr, dwCharDbNum);
		NET_PVE_GM_RELOAD_TRIGGER_ACK_FA() : bResult(false), dwFieldSvr(0), dwCharDbNum(0) {}
	};

	struct NET_PVE_GM_GOSTAGE_CF
	{
		DWORD dwCharDbNum;					//@ 리로딩을 요청한 캐릭터 DB
		int nStage;							//@ 이동할 특정 스테이지
		bool bIsItem;

		MSGPACK_DEFINE(dwCharDbNum, nStage, bIsItem);
		NET_PVE_GM_GOSTAGE_CF() : dwCharDbNum(0), nStage(0), bIsItem(false) {}
	};

} // end namespace
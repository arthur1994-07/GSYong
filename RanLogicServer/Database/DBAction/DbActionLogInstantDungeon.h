// #ifndef _DB_ACTION_LOG_H_
// #define _DB_ACTION_LOG_H_

#pragma once

#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../DbDefineLog.h"
#include "DbAction.h"

#include "../../../RanLogic/GLogicData.h"

namespace db
{
    // 값 변경시 DB테이블도 같이 변경되어야 한다.
    enum EM_INSTANT_LOG_TYPE
    {
        // 넘버링 가이드;
        // 10000번대 : PVE시스템 or 구 인던;
        // 20000번대 : 신규 인던 시스템 공용 로그;
        // 30000번대 : 신규 인던 전장 컨텐츠 전용 로그 ( PVP );
		// 50000번대 : 신규 인던 전장 컨텐츠 전용 로그 ( PVE );

        // 무한의 재단 로그;
		//LOGTYPE_INFINITE_STAIRCASE              = 10000,	
        LOGTYPE_INFINITE_STAIRCASE              = 10001,	// 던전의 MAP NAME( GAMEINTEXT.XML )를 찍도록 변경되었다.	

        // 신규 인던 로그;
		LOGTYPE_NEW_INSTANCE                    = 20000,

		// 신규 인던 전장 컨텐츠 전용 로그 ( PVP );
        //LOGTYPE_COMPETITION                     = 30000,
        LOGTYPE_COMPETITION_CDM                 = 30100,    // CDM;
        LOGTYPE_COMPETITION_CAPTURE_THE_FLAG    = 30200,    // 깃발전;
        LOGTYPE_COMPETITION_GUIDANCE            = 30300,    // 선도전;
		LOGTYPE_COMPETITION_CTI					= 30400,    // 난사군도;

		LOGTYPE_TOURNAMENT                      = 40000,    // 토너먼트 로그;


		/// 기록되어야 할 정보별로 로그를 정리한다;

		// 진행 로그;
		LOGTYPE_PROGRESS_STAGE_INFO				= 50000,	// 스테이지 진행 및 재도전 정보;

		// 결과 로그;
		LOGTYPE_RESULT_ACCUMULATE_INFO			= 60000,	// 누적대미지, 누적힐량, 누적스킬부활 정보;

		// 보상 로그;
		LOGTYPE_REWARD_INFO						= 70000,	// 보상;
    };




	// INFINITE_STAIR
    static const char* INFINITE_STAIRCASE_START = "START";
    static const char* INFINITE_STAIRCASE_PLAY = "PLAY";
    static const char* INFINITE_STAIRCASE_END = "END";
    class LogInstantInfiniteStaircaseStart : public db::DbAction
    {
    public:
        LogInstantInfiniteStaircaseStart(
            sc::MGUID DungeonGuid,
            int nPersonnel,       
            DWORD dwChaNum, 
            int nChaLevel,
            int nEntryCount,      
            int nRemnantCout,
			SNATIVEID sMapID )
            : m_DungeonGuid( DungeonGuid )
            , m_nPersonnel( nPersonnel )
            , m_dwChaNum( dwChaNum )
            , m_nChaLevel( nChaLevel )
            , m_nEntryCount( nEntryCount )
            , m_nRemnantCount( nRemnantCout )
			, m_sMapID(sMapID)
        {}
        virtual ~LogInstantInfiniteStaircaseStart() {}
        virtual int Execute(NetServer *pServer);

    private:
        sc::MGUID m_DungeonGuid;    // 인던 GUID

        int m_nPersonnel;           // 플레이중인 인원
        DWORD m_dwChaNum;           // 기록 남기는 ChaNum
        int m_nChaLevel;            // 캐릭터 레벨
        int m_nEntryCount;          // 오늘 플레이한 횟수
        int m_nRemnantCount;         // 남은 플레이 횟수
		SNATIVEID m_sMapID;
    };

    class LogInstantInfiniteStaircasePlay : public db::DbAction
    {
    public:
        LogInstantInfiniteStaircasePlay(
            const std::string& strGuid,
            const int& nPersonnel,       
            const DWORD& dwChaNum, 
            const int& nChaLevel,
            const int& nStageNum,
			SNATIVEID sMapID )
            : m_strGuid( strGuid )
            , m_nPersonnel( nPersonnel )
            , m_dwChaNum( dwChaNum )
            , m_nChaLevel( nChaLevel )
            , m_nStageNum( nStageNum )
			, m_sMapID(sMapID)
        {}
        virtual ~LogInstantInfiniteStaircasePlay() {}
        virtual int Execute(NetServer *pServer);

    private:
        std::string m_strGuid;      // 인던 GUID

        int m_nPersonnel;           // 플레이중인 인원
        DWORD m_dwChaNum;           // 기록 남기는 ChaNum
        int m_nChaLevel;            // 캐릭터 레벨

        int m_nStageNum;            // 현재 스테이지
		SNATIVEID m_sMapID;
    };

    class LogInstantInfiniteStaircaseEnd : public db::DbAction
    {
    public:
        LogInstantInfiniteStaircaseEnd(
            const std::string& strGuid,
            const int& nPersonnel,       
            const DWORD& dwChaNum, 
            const int& nChaLevel,
            const int& nStageNum,
            const bool& bMVP,
            const LONGLONG& llMoney,
            const int& nExp,
			SNATIVEID sMapID )
            : m_strGuid( strGuid )
            , m_nPersonnel( nPersonnel )
            , m_dwChaNum( dwChaNum )
            , m_nChaLevel( nChaLevel )
            , m_nStageNum( nStageNum )
            , m_bMVP( bMVP )
            , m_llMoney( llMoney )
            , m_nExp( nExp )
			, m_sMapID(sMapID)
        {}
        virtual ~LogInstantInfiniteStaircaseEnd() {}
        virtual int Execute(NetServer *pServer);

    private:
        std::string m_strGuid;      // 인던 GUID
 
        int m_nPersonnel;           // 플레이중인 인원
        DWORD m_dwChaNum;           // 기록 남기는 ChaNum
        int m_nChaLevel;            // 캐릭터 레벨
        int m_nStageNum;            // 현재 스테이지

        bool m_bMVP;                // MVP 여부
        LONGLONG m_llMoney;         // 받은 골드
        int m_nExp;                 // 경험치
		SNATIVEID m_sMapID;

    };



	// New Instance Dungeon System
    static const char* LOG_SUBTYPE_NEW_INSTANCE_CREATE = "CREATE";
    static const char* LOG_SUBTYPE_NEW_INSTANCE_DESTROY = "DESTROY";

    class LogInstantNewInstance : public db::DbAction
	{
    public:
        enum
        {
            CREATE = 0,
            DESTROY,
        };

        LogInstantNewInstance(
            const int _nLogSubType,
            const sc::MGUID& _Guid
            )
            : nLogSubType( _nLogSubType )
            , dungeonGuid( _Guid )
            , wKeyMapMID(0)
            , wKeyMapSID(0)
            , dwBasMapID(0)
            , dwInstanceMapID(0)
            , tCreateDateTime(0)
            , tDestroyDateTime(0)
        {}
        virtual ~LogInstantNewInstance() {}
        virtual int Execute(NetServer *pServer);

	private:
        const int           nLogSubType;     // 인던 로그의 세부 타입;
        const sc::MGUID     dungeonGuid;        // 인던 GUID (LandManager 생성당시 부여받는 고유키);
    public:
        std::string         strInstanceName;    // 인던의 이름;
        WORD                wKeyMapMID;         // 인던의 KeyMapID 중 MainID;
        WORD                wKeyMapSID;         // 인던의 KeyMapID 중 SubID;
        DWORD               dwBasMapID;         // 인던의 BaseMapID;
        DWORD               dwInstanceMapID;    // 인던의 InstanceMapID;

        __time64_t          tCreateDateTime;    // 인던 생성 시점;
        __time64_t          tDestroyDateTime;   // 인던 파괴 시점;
	};


    ///////////////////////////////////////////////////////////////////////////////////////
    //                       Competition(BattleGround System)
    ///////////////////////////////////////////////////////////////////////////////////////
   
    // Club Death Match
    static const char* LOG_SUBTYPE_CDM_USER_IN = "IN";
    static const char* LOG_SUBTYPE_CDM_USER_OUT = "OUT";
    static const char* LOG_SUBTYPE_CDM_CLUB_RESULT = "RESULT";

    // Club Death Match 로그;
    class LogInstantCDM : public db::DbAction
    {
    public:
        enum
        {
            EM_IN = 0,      // CDM에 유저 입장시 로그;
            EM_OUT,         // CDM에서 유저 퇴장시 로그;
            EM_RESULT,      // CDM 결과 보상시 로그;
        };

    private:
        const int           nLogSubType;        // CDM 로그의 세부 타입 (입장,퇴장,결과 등);
        const sc::MGUID     dungeonGuid;        // CDM이 열린 인던의 GUID;
    public:
        DWORD               dwClubDbNum;        // 클럽 DBNum;
        DWORD               dwCharDbNum;        // 케릭터 DBNum;
        int                 nClubRank;          // 클럽 순위;
        int                 nClubPoint;         // 클럽 포인트;
        int                 nClubKill;          // 클럽 총 킬 수;
        int                 nClubDeath;         // 클럽 총 데스 수;

    public:
        LogInstantCDM(
            const int  _nLogSubType,            // CDM 로그의 세부 타입 (입장,퇴장,결과 등);
            const sc::MGUID&    _dungeonGuid    // CDM 열린 인던의 GUID;
            )
            : nLogSubType( _nLogSubType)
            , dungeonGuid(_dungeonGuid)
            , dwClubDbNum(0)
            , dwCharDbNum(0)
            , nClubRank(0)
            , nClubPoint(0)
            , nClubKill(0)
            , nClubDeath(0)
        {
        }
        virtual ~LogInstantCDM() {}
        virtual int Execute(NetServer *pServer);
    };
    

    // capture the flag
    static const char* LOG_SUBTYPE_CAPTURE_THE_FLAG_IN = "IN";
    static const char* LOG_SUBTYPE_CAPTURE_THE_FLAG_OUT = "OUT";
    static const char* LOG_SUBTYPE_CAPTURE_THE_FLAG_RESULT = "RESULT";
    static const char* LOG_SUBTYPE_CAPTURE_THE_FLAG_REWARD = "REWARD";
    static const char* LOG_SUBTYPE_CAPTURE_THE_FLAG_RATING_CHANGE = "RATING CHANGE";

    // 팀 결과 로그;
    class LogInstantCaptureTheFlag : public db::DbAction
    {
    public:
        enum
        {
            EM_IN = 0,
            EM_OUT,
            EM_RESULT,
            EM_REWARD,
            EM_RATING_CHANGE,
        };
    
        LogInstantCaptureTheFlag(
                const int  _nLogSubType,     // 깃발전 로그의 세부 타입 (팀결과, 유저결과 등);
                const sc::MGUID&    _dungeonGuid        // 깃발전이 열린 인던의 GUID;
            )
            : nLogSubType(_nLogSubType)
            , dungeonGuid(_dungeonGuid)
            , dwCharDbNum(0)
            , nTeam(0)
            , bResult(false)
            , nFlagScore(0)
            , nKill(0)
            , nDeath(0)
            , nReward(0)
            , nRating(0)
            , nChangedRating(0)
            , nWinningStreak(0)
        {}
        virtual ~LogInstantCaptureTheFlag() {}
        virtual int Execute(NetServer *pServer);        

    private:
        const int           nLogSubType;        // 깃발전 로그의 세부 타입 (팀결과, 유저결과 등);
        const sc::MGUID     dungeonGuid;        // 깃발전이 열린 인던의 GUID;
    public:
        DWORD               dwCharDbNum;        // 케릭터 DBNum;
        int                 nTeam;              // 팀;
        bool                bResult;            // 결과 승리 여부;
        int                 nFlagScore;         // 깃발 탈취수;
        int                 nKill;              // 킬 수;
        int                 nDeath;             // 사망 수;
        int                 nReward;            // 보상 경험치;
        int                 nRating;            // 전장 점수;
        int                 nChangedRating;     // 전장 점수 변동 폭;
        int                 nWinningStreak;     // 연승 정보;  
    };


    // Guidance Club Battle
    static const char* LOG_SUBTYPE_GUIDANCE_USER_IN = "IN";
    static const char* LOG_SUBTYPE_GUIDANCE_USER_OUT = "OUT";
    static const char* LOG_SUBTYPE_GUIDANCE_RESULT = "RESULT";
    static const char* LOG_SUBTYPE_GUIDANCE_REWARD = "REWARD";

    // Guidance Club Battle 로그;
    class LogInstantGuidance : public db::DbAction
    {
    public:
        enum
        {
            EM_IN = 0,      // Guidance에 유저 입장시 로그;
            EM_OUT,         // Guidance에서 유저 퇴장시 로그;
            EM_RESULT,      // Guidance 결과시 로그;
            EM_REWARD,      // Guidance 선도클럽 결정 보상시 로그;
        };

    private:
        const int           nLogSubType;        // Guidance 로그의 세부 타입 (입장,퇴장,결과 등);
        const sc::MGUID     dungeonGuid;        // Guidance이 열린 인던의 GUID;
    public:
        std::string         strGuidanceName;    // 선도전 전장 이름;
        DWORD               dwCharDbNum;        // 입,퇴장 시 해당 케릭터 CharDbNum;
        DWORD               dwClubDbNum;        // 1등 선도 클럽 DBNum;
        int                 nClubPoint;         // 1등 클럽 포인트;
        int                 nClubHaveAuthNum;   // 1등 클럽 인증 처리기 보유 수;
        int                 nClubAuthCount;     // 1등 클럽 인증 횟수;

        DWORD               dwSecondClubDbNum;        // 2등 선도 클럽 DBNum;
        int                 nSecondClubPoint;         // 2등 클럽 포인트;
        int                 nSecondClubHaveAuthNum;   // 2등 클럽 인증 처리기 보유 수;
        int                 nSecondClubAuthCount;     // 2등 클럽 인증 횟수;

        DWORD               dwThirdClubDbNum;        // 3등 선도 클럽 DBNum;
        int                 nThirdClubPoint;         // 3등 클럽 포인트;
        int                 nThirdClubHaveAuthNum;   // 3등 클럽 인증 처리기 보유 수;
        int                 nThirdClubAuthCount;     // 3등 클럽 인증 횟수;

    public:
        LogInstantGuidance(
            const int  _nLogSubType,            // 선도전 로그의 세부 타입 (입장,퇴장,결과 등);
            const sc::MGUID&    _dungeonGuid    // 선도전이 열린 인던의 GUID;
            )
            : nLogSubType( _nLogSubType)
            , dungeonGuid(_dungeonGuid)
            , dwCharDbNum(0)
            , dwClubDbNum(0)
            , nClubPoint(0)
            , nClubHaveAuthNum(0)
            , nClubAuthCount(0)
            , dwSecondClubDbNum(0)
            , nSecondClubPoint(0)
            , nSecondClubHaveAuthNum(0)
            , nSecondClubAuthCount(0)
            , dwThirdClubDbNum(0)
            , nThirdClubPoint(0)
            , nThirdClubHaveAuthNum(0)
            , nThirdClubAuthCount(0)
        {
        }
        virtual ~LogInstantGuidance() {}
        virtual int Execute(NetServer *pServer);
    };

	// Capture The Islands 로그;
	class LogInstantCTI : public db::DbAction
	{
	public:
		enum
		{
			EM_IN = 0,      // CTI 에 유저 입장시 로그;
			EM_OUT,         // CTI 에서 유저 퇴장시 로그;
			EM_RESULT,      // CTI 결과 시 로그;
			EM_REWARD,      // CTI 국가 결정 보상시 로그;
		};

	private:
		const int           nLogSubType;				// CTI 로그의 세부 타입 ( 입장,퇴장,결과 등 );
		const sc::MGUID     dungeonGuid;				// CTI 이 열린 인던의 GUID;

	public:
		std::string         strCTIName;					// CTI 전장 이름;
		DWORD               dwCharDbNum;				// 입, 퇴장 시 해당 캐릭터 CharDbNum;
		DWORD               dwCountryID;				// 1등 국가 CountryID;
		int                 nCountryPoint;				// 1등 국가 포인트;
		int                 nCountryHaveAuthNum;		// 1등 국가 인증 처리기 보유 수;
		int                 nCountryAuthCount;			// 1등 국가 인증 횟수;

		DWORD               dwSecondCountryID;			// 2등 국가 CountryID;
		int                 nSecondCountryPoint;		// 2등 국가 포인트;
		int                 nSecondCountryHaveAuthNum;	// 2등 국가 인증 처리기 보유 수;
		int                 nSecondCountryAuthCount;	// 2등 국가 인증 횟수;

		DWORD               dwThirdCountryID;			// 3등 국가 CountryID;
		int                 nThirdCountryPoint;			// 3등 국가 포인트;
		int                 nThirdCountryHaveAuthNum;	// 3등 국가 인증 처리기 보유 수;
		int                 nThirdCountryAuthCount;		// 3등 국가 인증 횟수;

	public:
		LogInstantCTI (
			const int  _nLogSubType,					// CTI 로그의 세부 타입 (입장,퇴장,결과 등);
			const sc::MGUID& _dungeonGuid )				// CTI 이 열린 인던의 GUID;
			: nLogSubType ( _nLogSubType )
			, dungeonGuid ( _dungeonGuid )
			, dwCharDbNum ( 0 )
			, dwCountryID ( 0 )
			, nCountryPoint ( 0 )
			, nCountryHaveAuthNum ( 0 )
			, nCountryAuthCount ( 0 )
			, dwSecondCountryID ( 0 )
			, nSecondCountryPoint ( 0 )
			, nSecondCountryHaveAuthNum ( 0 )
			, nSecondCountryAuthCount ( 0 )
			, dwThirdCountryID ( 0 )
			, nThirdCountryPoint ( 0 )
			, nThirdCountryHaveAuthNum ( 0 )
			, nThirdCountryAuthCount ( 0 )
		{
		}

		virtual ~LogInstantCTI ( ) { }
		virtual int Execute ( NetServer *pServer );
	};

	// 토너먼트 관련 로그
    class LogTournament : public db::DbAction
    {
    public:
     
        LogTournament(const int  _nLogSubType)
			: nLogSubType( _nLogSubType)
            , dwCharDbNum(0)
			, dwClass(0)
            , dwSchool(0)
            , dwPartyID(0)
            , nMoney(0)
            , dwItemMainID(0)
            , dwItemSubID(0)
            , dwItemCount(0)
			, dwTotoTargetChaNum(0)
			, dwCanObserve(0)
			, dwCanToto(0)
			, dwFactionID(0)
			, dwMapID(0)
			, dwForceWin(0)
			, dwTotalMemberCount(0)
			, dwTournamentSize(0)
			, dwTournamentNodeCount(0)
			{}
        virtual ~LogTournament() {}
        virtual int Execute(NetServer *pServer);    

    private:
        const int           nLogSubType;        
    public:
        std::string         strChaName;    
        DWORD               dwCharDbNum;   
		DWORD				dwClass;
        DWORD               dwSchool;      
        DWORD               dwPartyID;     
        DWORD               nMoney;   
        DWORD               dwItemMainID;    
		DWORD				dwItemSubID;
		DWORD				dwItemCount;

		DWORD				dwTotoTargetChaNum;
		std::string         strContentName;    
		DWORD				dwCanObserve;
		DWORD				dwCanToto;	

		DWORD				dwFactionID;
		DWORD				dwMapID;
		DWORD				dwForceWin;

		DWORD				dwTotalMemberCount;
		DWORD				dwTournamentSize;
		DWORD				dwTournamentNodeCount;
    };

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 기록되어야 할 정보별로 로그를 정리한다;

	// Parameter 4 개는 다음과 같이 공통적으로 설정해줘서 보기 편하게 한다;
	// Parameter 에서 독립시키는게 올바른 데이터지만 이미 잘 사용중이므로 굳이 독립시키지 않는다;
	// Parameter 1 : Log Sub Type;
	// Parameter 2 : GUID;
	// Parameter 3 : Instance Dungeon Contents Name;
	// Parameter 4 : Instance Dungeon Contents ID;

	// LOG SUBTYPE 문자열 정보;
	static const char* LOG_SUBTYPE_ENTER_USER		= "ENTER";
	static const char* LOG_SUBTYPE_REENTER_USER		= "REENTER";
	static const char* LOG_SUBTYPE_OUT_USER			= "OUT";
	static const char* LOG_SUBTYPE_PROGRESS			= "PROGRESS";
	static const char* LOG_SUBTYPE_RETRY			= "RETRY";
	static const char* LOG_SUBTYPE_RESULT			= "RESULT";
	static const char* LOG_SUBTYPE_REWARD			= "REWARD";


	// DBAction 진행정보 : 스테이지 정보;
	class DBAction_ProgressStage : public db::DbAction
	{
	public:
		enum EMSUBTYPE_LOG
		{
			EMTYPE_DEFAULT = 0,
			EMTYPE_ENTER,		// 유저 입장 ( 각 유저의 입장 정보를 기록한다 );
			EMTYPE_REENTER,		// 유저 재입장 ( 각 유저의 재입장 정보를 기록한다 );
			EMTYPE_OUT,			// 유저 퇴장 ( 각 유저의 퇴장 정보를 기록한다 );
			EMTYPE_PROGRESS,	// 스테이즈 진행 정보 ( 던전 스테이지 진행 정보를 기록한다 );
			EMTYPE_RETRY,		// 재도전 정보 ( 던전 재도전 정보를 기록한다 );
		};


	public:
		virtual int				Execute ( NetServer* _pServer );


	private:
		const std::string		m_strChaName;								// 유저 이름;

		const LONGLONG			m_llRetryMoney;								// 재도전에 사용한 금액;

		const DWORD				m_dwChaDBNum;								// 유저 DB Num;
		const DWORD				m_dwChaLevel;								// 유저 레벨;
		const DWORD				m_dwChaClass;								// 유저 클래스;
		const DWORD				m_dwStageNum;								// 현재 스테이지 번호;
		const DWORD				m_dwAllUserCount;							// 전체 유저 수;
		const DWORD				m_dwProgressUserCount;						// 진행 유저 수;
		const DWORD				m_dwRetryCount;								// 재도전 수;


	private:
		const sc::MGUID			m_sGUID;									// 던전 GUID;
		const std::string		m_strContentsName;							// 던전 이름;
		const DWORD				m_dwContentsID;								// 던전 Contents ID;
		const EMSUBTYPE_LOG		m_emLogType;								// 로그 타입 ( EMTYPE_IN, EMTYPE_OUT ... );


	public:
		// Enter, Reenter, Out;
		DBAction_ProgressStage ( const sc::MGUID& _sGUID
			, const std::string& _strContentsName
			, const DWORD _dwContentsID
			, const EMSUBTYPE_LOG _emLogType
			, const std::string& _strChaName
			, const DWORD _dwChaDBNum
			, const DWORD _dwChaLevel
			, const DWORD _dwChaClass
			, const DWORD _dwStageNum
			, const DWORD _dwAllUserCount
			, const DWORD _dwProgressUserCount
			, const DWORD _dwRetryCount)
			: m_sGUID ( _sGUID )
			, m_strContentsName ( _strContentsName )
			, m_dwContentsID ( _dwContentsID )
			, m_emLogType ( _emLogType )
			, m_strChaName ( _strChaName )
			, m_llRetryMoney ( 0 )
			, m_dwChaDBNum ( _dwChaDBNum )
			, m_dwChaLevel ( _dwChaLevel )
			, m_dwChaClass ( _dwChaClass )
			, m_dwStageNum ( _dwStageNum )
			, m_dwAllUserCount ( _dwAllUserCount )
			, m_dwProgressUserCount ( _dwProgressUserCount )
			, m_dwRetryCount ( _dwRetryCount )
		{
		}

		// Retry;
		DBAction_ProgressStage ( const sc::MGUID& _sGUID
			, const std::string& _strContentsName
			, const DWORD _dwContentsID
			, const LONGLONG _llRetryMoney
			, const DWORD _dwStageNum
			, const DWORD _dwAllUserCount
			, const DWORD _dwProgressUserCount
			, const DWORD _dwRetryCount )
			: m_sGUID ( _sGUID )
			, m_strContentsName ( _strContentsName )
			, m_dwContentsID ( _dwContentsID )
			, m_emLogType ( EMTYPE_RETRY )
			, m_llRetryMoney ( _llRetryMoney )
			, m_dwChaDBNum ( 0 )
			, m_dwChaLevel ( 0 )
			, m_dwChaClass ( 0 )
			, m_dwStageNum ( _dwStageNum )
			, m_dwAllUserCount ( _dwAllUserCount )
			, m_dwProgressUserCount ( _dwProgressUserCount )
			, m_dwRetryCount ( _dwRetryCount )
		{
		}

		// Progress;
		DBAction_ProgressStage ( const sc::MGUID& _sGUID
			, const std::string& _strContentsName
			, const DWORD _dwContentsID
			, const DWORD _dwStageNum
			, const DWORD _dwAllUserCount
			, const DWORD _dwProgressUserCount
			, const DWORD _dwRetryCount)
			: m_sGUID ( _sGUID )
			, m_strContentsName ( _strContentsName )
			, m_dwContentsID ( _dwContentsID )
			, m_emLogType ( EMTYPE_PROGRESS )
			, m_llRetryMoney ( 0 )
			, m_dwChaDBNum ( 0 )
			, m_dwChaLevel ( 0 )
			, m_dwChaClass ( 0 )
			, m_dwStageNum ( _dwStageNum )
			, m_dwAllUserCount ( _dwAllUserCount )
			, m_dwProgressUserCount ( _dwProgressUserCount )
			, m_dwRetryCount ( _dwRetryCount )
		{
		}

		virtual ~DBAction_ProgressStage ( ) { }

	};


	// DBAction 결과정보 : 누적 현황 정보;
	class DBAction_ResultAccumulate : public db::DbAction
	{
	public:
		virtual int				Execute ( NetServer* _pServer );


	private:
		const std::string		m_strChaName;								// 유저 이름;

		const DWORD				m_dwChaDBNum;								// 유저 DB Num;
		const DWORD				m_dwChaLevel;								// 유저 레벨;
		const DWORD				m_dwChaClass;								// 유저 클래스;
		const DWORD				m_dwStageNum;								// 진행한 스테이지 번호;
		const DWORD				m_dwReviveCount;							// 스킬 부활 사용 횟수;
		const DWORD				m_dwDamage;									// 유저 누적대미지;
		const DWORD				m_dwHeal;									// 유저 누적회복량;

		const bool				m_bMVP;										// 유저 MVP 여부;


	private:
		const sc::MGUID			m_sGUID;									// 던전 GUID;
		const std::string		m_strContentsName;							// 던전 이름;
		const DWORD				m_dwContentsID;								// 던전 Contents ID;


	public:
		DBAction_ResultAccumulate ( const sc::MGUID& _sGUID
			, const std::string& _strContentsName
			, const DWORD _dwContentsID
			, const std::string& _strChaName
			, const DWORD _dwChaDBNum
			, const DWORD _dwChaLevel
			, const DWORD _dwChaClass
			, const DWORD _dwStageNum
			, const DWORD _dwReviveCount
			, const DWORD _dwDamage
			, const DWORD _dwHeal
			, const bool _bMVP )
			: m_sGUID ( _sGUID )
			, m_strContentsName ( _strContentsName )
			, m_dwContentsID ( _dwContentsID )
			, m_strChaName ( _strChaName )
			, m_dwChaDBNum ( _dwChaDBNum )
			, m_dwChaLevel ( _dwChaLevel )
			, m_dwChaClass ( _dwChaClass )
			, m_dwStageNum ( _dwStageNum )
			, m_dwReviveCount ( _dwReviveCount )
			, m_dwDamage ( _dwDamage )
			, m_dwHeal ( _dwHeal )
			, m_bMVP ( _bMVP )
		{
		}

		virtual ~DBAction_ResultAccumulate ( ) { }

	};


	static const int MAX_REWARD_ITEM_COUNT = 5;

	// DBAction 보상정보 : 모든 보상 정보;
	class DBAction_Reward : public db::DbAction
	{
	public:
		virtual int				Execute ( NetServer* _pServer );


	public:
		void					AddRewardItem ( int _nIndex, const std::string& _strItemID );


	private:
		std::string				m_arrRewardItem[ MAX_REWARD_ITEM_COUNT ];	// 보상 아이템 정보;


	private:
		const std::string		m_strChaName;								// 유저 이름;

		const LONGLONG			m_llRewardMoney;							// 보상 금액;

		const DWORD				m_dwChaDBNum;								// 유저 DB Num;
		const DWORD				m_dwChaLevel;								// 유저 레벨;
		const DWORD				m_dwChaClass;								// 유저 클래스;
		const DWORD				m_dwRewardEXP;								// 보상 경험치;
		const DWORD				m_dwRewardItemCount;						// 보상 아이템 개수;


	private:
		const sc::MGUID			m_sGUID;									// 던전 GUID;
		const std::string		m_strContentsName;							// 던전 이름;
		const DWORD				m_dwContentsID;								// 던전 Contents ID;


	public:
		DBAction_Reward ( const sc::MGUID& _sGUID
			, const std::string& _strContentsName
			, const DWORD _dwContentsID
			, const std::string& _strChaName
			, const LONGLONG _llRewardMoney
			, const DWORD _dwChaDBNum
			, const DWORD _dwChaLevel
			, const DWORD _dwChaClass
			, const DWORD _dwRewardEXP
			, const DWORD _dwRewardItemCount )
			: m_sGUID ( _sGUID )
			, m_strContentsName ( _strContentsName )
			, m_dwContentsID ( _dwContentsID )
			, m_strChaName ( _strChaName )
			, m_llRewardMoney ( _llRewardMoney )
			, m_dwChaDBNum ( _dwChaDBNum )
			, m_dwChaLevel ( _dwChaLevel )
			, m_dwChaClass ( _dwChaClass )
			, m_dwRewardEXP ( _dwRewardEXP )
			, m_dwRewardItemCount ( _dwRewardItemCount )
		{
		}

		virtual ~DBAction_Reward ( ) { }

	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace db
// #endif //! _DB_ACTION_LOG_H_
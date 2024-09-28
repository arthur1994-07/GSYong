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
    // �� ����� DB���̺� ���� ����Ǿ�� �Ѵ�.
    enum EM_INSTANT_LOG_TYPE
    {
        // �ѹ��� ���̵�;
        // 10000���� : PVE�ý��� or �� �δ�;
        // 20000���� : �ű� �δ� �ý��� ���� �α�;
        // 30000���� : �ű� �δ� ���� ������ ���� �α� ( PVP );
		// 50000���� : �ű� �δ� ���� ������ ���� �α� ( PVE );

        // ������ ��� �α�;
		//LOGTYPE_INFINITE_STAIRCASE              = 10000,	
        LOGTYPE_INFINITE_STAIRCASE              = 10001,	// ������ MAP NAME( GAMEINTEXT.XML )�� �ﵵ�� ����Ǿ���.	

        // �ű� �δ� �α�;
		LOGTYPE_NEW_INSTANCE                    = 20000,

		// �ű� �δ� ���� ������ ���� �α� ( PVP );
        //LOGTYPE_COMPETITION                     = 30000,
        LOGTYPE_COMPETITION_CDM                 = 30100,    // CDM;
        LOGTYPE_COMPETITION_CAPTURE_THE_FLAG    = 30200,    // �����;
        LOGTYPE_COMPETITION_GUIDANCE            = 30300,    // ������;
		LOGTYPE_COMPETITION_CTI					= 30400,    // ���籺��;

		LOGTYPE_TOURNAMENT                      = 40000,    // ��ʸ�Ʈ �α�;


		/// ��ϵǾ�� �� �������� �α׸� �����Ѵ�;

		// ���� �α�;
		LOGTYPE_PROGRESS_STAGE_INFO				= 50000,	// �������� ���� �� �絵�� ����;

		// ��� �α�;
		LOGTYPE_RESULT_ACCUMULATE_INFO			= 60000,	// ���������, ��������, ������ų��Ȱ ����;

		// ���� �α�;
		LOGTYPE_REWARD_INFO						= 70000,	// ����;
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
        sc::MGUID m_DungeonGuid;    // �δ� GUID

        int m_nPersonnel;           // �÷������� �ο�
        DWORD m_dwChaNum;           // ��� ����� ChaNum
        int m_nChaLevel;            // ĳ���� ����
        int m_nEntryCount;          // ���� �÷����� Ƚ��
        int m_nRemnantCount;         // ���� �÷��� Ƚ��
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
        std::string m_strGuid;      // �δ� GUID

        int m_nPersonnel;           // �÷������� �ο�
        DWORD m_dwChaNum;           // ��� ����� ChaNum
        int m_nChaLevel;            // ĳ���� ����

        int m_nStageNum;            // ���� ��������
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
        std::string m_strGuid;      // �δ� GUID
 
        int m_nPersonnel;           // �÷������� �ο�
        DWORD m_dwChaNum;           // ��� ����� ChaNum
        int m_nChaLevel;            // ĳ���� ����
        int m_nStageNum;            // ���� ��������

        bool m_bMVP;                // MVP ����
        LONGLONG m_llMoney;         // ���� ���
        int m_nExp;                 // ����ġ
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
        const int           nLogSubType;     // �δ� �α��� ���� Ÿ��;
        const sc::MGUID     dungeonGuid;        // �δ� GUID (LandManager ������� �ο��޴� ����Ű);
    public:
        std::string         strInstanceName;    // �δ��� �̸�;
        WORD                wKeyMapMID;         // �δ��� KeyMapID �� MainID;
        WORD                wKeyMapSID;         // �δ��� KeyMapID �� SubID;
        DWORD               dwBasMapID;         // �δ��� BaseMapID;
        DWORD               dwInstanceMapID;    // �δ��� InstanceMapID;

        __time64_t          tCreateDateTime;    // �δ� ���� ����;
        __time64_t          tDestroyDateTime;   // �δ� �ı� ����;
	};


    ///////////////////////////////////////////////////////////////////////////////////////
    //                       Competition(BattleGround System)
    ///////////////////////////////////////////////////////////////////////////////////////
   
    // Club Death Match
    static const char* LOG_SUBTYPE_CDM_USER_IN = "IN";
    static const char* LOG_SUBTYPE_CDM_USER_OUT = "OUT";
    static const char* LOG_SUBTYPE_CDM_CLUB_RESULT = "RESULT";

    // Club Death Match �α�;
    class LogInstantCDM : public db::DbAction
    {
    public:
        enum
        {
            EM_IN = 0,      // CDM�� ���� ����� �α�;
            EM_OUT,         // CDM���� ���� ����� �α�;
            EM_RESULT,      // CDM ��� ����� �α�;
        };

    private:
        const int           nLogSubType;        // CDM �α��� ���� Ÿ�� (����,����,��� ��);
        const sc::MGUID     dungeonGuid;        // CDM�� ���� �δ��� GUID;
    public:
        DWORD               dwClubDbNum;        // Ŭ�� DBNum;
        DWORD               dwCharDbNum;        // �ɸ��� DBNum;
        int                 nClubRank;          // Ŭ�� ����;
        int                 nClubPoint;         // Ŭ�� ����Ʈ;
        int                 nClubKill;          // Ŭ�� �� ų ��;
        int                 nClubDeath;         // Ŭ�� �� ���� ��;

    public:
        LogInstantCDM(
            const int  _nLogSubType,            // CDM �α��� ���� Ÿ�� (����,����,��� ��);
            const sc::MGUID&    _dungeonGuid    // CDM ���� �δ��� GUID;
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

    // �� ��� �α�;
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
                const int  _nLogSubType,     // ����� �α��� ���� Ÿ�� (�����, ������� ��);
                const sc::MGUID&    _dungeonGuid        // ������� ���� �δ��� GUID;
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
        const int           nLogSubType;        // ����� �α��� ���� Ÿ�� (�����, ������� ��);
        const sc::MGUID     dungeonGuid;        // ������� ���� �δ��� GUID;
    public:
        DWORD               dwCharDbNum;        // �ɸ��� DBNum;
        int                 nTeam;              // ��;
        bool                bResult;            // ��� �¸� ����;
        int                 nFlagScore;         // ��� Ż���;
        int                 nKill;              // ų ��;
        int                 nDeath;             // ��� ��;
        int                 nReward;            // ���� ����ġ;
        int                 nRating;            // ���� ����;
        int                 nChangedRating;     // ���� ���� ���� ��;
        int                 nWinningStreak;     // ���� ����;  
    };


    // Guidance Club Battle
    static const char* LOG_SUBTYPE_GUIDANCE_USER_IN = "IN";
    static const char* LOG_SUBTYPE_GUIDANCE_USER_OUT = "OUT";
    static const char* LOG_SUBTYPE_GUIDANCE_RESULT = "RESULT";
    static const char* LOG_SUBTYPE_GUIDANCE_REWARD = "REWARD";

    // Guidance Club Battle �α�;
    class LogInstantGuidance : public db::DbAction
    {
    public:
        enum
        {
            EM_IN = 0,      // Guidance�� ���� ����� �α�;
            EM_OUT,         // Guidance���� ���� ����� �α�;
            EM_RESULT,      // Guidance ����� �α�;
            EM_REWARD,      // Guidance ����Ŭ�� ���� ����� �α�;
        };

    private:
        const int           nLogSubType;        // Guidance �α��� ���� Ÿ�� (����,����,��� ��);
        const sc::MGUID     dungeonGuid;        // Guidance�� ���� �δ��� GUID;
    public:
        std::string         strGuidanceName;    // ������ ���� �̸�;
        DWORD               dwCharDbNum;        // ��,���� �� �ش� �ɸ��� CharDbNum;
        DWORD               dwClubDbNum;        // 1�� ���� Ŭ�� DBNum;
        int                 nClubPoint;         // 1�� Ŭ�� ����Ʈ;
        int                 nClubHaveAuthNum;   // 1�� Ŭ�� ���� ó���� ���� ��;
        int                 nClubAuthCount;     // 1�� Ŭ�� ���� Ƚ��;

        DWORD               dwSecondClubDbNum;        // 2�� ���� Ŭ�� DBNum;
        int                 nSecondClubPoint;         // 2�� Ŭ�� ����Ʈ;
        int                 nSecondClubHaveAuthNum;   // 2�� Ŭ�� ���� ó���� ���� ��;
        int                 nSecondClubAuthCount;     // 2�� Ŭ�� ���� Ƚ��;

        DWORD               dwThirdClubDbNum;        // 3�� ���� Ŭ�� DBNum;
        int                 nThirdClubPoint;         // 3�� Ŭ�� ����Ʈ;
        int                 nThirdClubHaveAuthNum;   // 3�� Ŭ�� ���� ó���� ���� ��;
        int                 nThirdClubAuthCount;     // 3�� Ŭ�� ���� Ƚ��;

    public:
        LogInstantGuidance(
            const int  _nLogSubType,            // ������ �α��� ���� Ÿ�� (����,����,��� ��);
            const sc::MGUID&    _dungeonGuid    // �������� ���� �δ��� GUID;
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

	// Capture The Islands �α�;
	class LogInstantCTI : public db::DbAction
	{
	public:
		enum
		{
			EM_IN = 0,      // CTI �� ���� ����� �α�;
			EM_OUT,         // CTI ���� ���� ����� �α�;
			EM_RESULT,      // CTI ��� �� �α�;
			EM_REWARD,      // CTI ���� ���� ����� �α�;
		};

	private:
		const int           nLogSubType;				// CTI �α��� ���� Ÿ�� ( ����,����,��� �� );
		const sc::MGUID     dungeonGuid;				// CTI �� ���� �δ��� GUID;

	public:
		std::string         strCTIName;					// CTI ���� �̸�;
		DWORD               dwCharDbNum;				// ��, ���� �� �ش� ĳ���� CharDbNum;
		DWORD               dwCountryID;				// 1�� ���� CountryID;
		int                 nCountryPoint;				// 1�� ���� ����Ʈ;
		int                 nCountryHaveAuthNum;		// 1�� ���� ���� ó���� ���� ��;
		int                 nCountryAuthCount;			// 1�� ���� ���� Ƚ��;

		DWORD               dwSecondCountryID;			// 2�� ���� CountryID;
		int                 nSecondCountryPoint;		// 2�� ���� ����Ʈ;
		int                 nSecondCountryHaveAuthNum;	// 2�� ���� ���� ó���� ���� ��;
		int                 nSecondCountryAuthCount;	// 2�� ���� ���� Ƚ��;

		DWORD               dwThirdCountryID;			// 3�� ���� CountryID;
		int                 nThirdCountryPoint;			// 3�� ���� ����Ʈ;
		int                 nThirdCountryHaveAuthNum;	// 3�� ���� ���� ó���� ���� ��;
		int                 nThirdCountryAuthCount;		// 3�� ���� ���� Ƚ��;

	public:
		LogInstantCTI (
			const int  _nLogSubType,					// CTI �α��� ���� Ÿ�� (����,����,��� ��);
			const sc::MGUID& _dungeonGuid )				// CTI �� ���� �δ��� GUID;
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

	// ��ʸ�Ʈ ���� �α�
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
	// ��ϵǾ�� �� �������� �α׸� �����Ѵ�;

	// Parameter 4 ���� ������ ���� ���������� �������༭ ���� ���ϰ� �Ѵ�;
	// Parameter ���� ������Ű�°� �ùٸ� ���������� �̹� �� ������̹Ƿ� ���� ������Ű�� �ʴ´�;
	// Parameter 1 : Log Sub Type;
	// Parameter 2 : GUID;
	// Parameter 3 : Instance Dungeon Contents Name;
	// Parameter 4 : Instance Dungeon Contents ID;

	// LOG SUBTYPE ���ڿ� ����;
	static const char* LOG_SUBTYPE_ENTER_USER		= "ENTER";
	static const char* LOG_SUBTYPE_REENTER_USER		= "REENTER";
	static const char* LOG_SUBTYPE_OUT_USER			= "OUT";
	static const char* LOG_SUBTYPE_PROGRESS			= "PROGRESS";
	static const char* LOG_SUBTYPE_RETRY			= "RETRY";
	static const char* LOG_SUBTYPE_RESULT			= "RESULT";
	static const char* LOG_SUBTYPE_REWARD			= "REWARD";


	// DBAction �������� : �������� ����;
	class DBAction_ProgressStage : public db::DbAction
	{
	public:
		enum EMSUBTYPE_LOG
		{
			EMTYPE_DEFAULT = 0,
			EMTYPE_ENTER,		// ���� ���� ( �� ������ ���� ������ ����Ѵ� );
			EMTYPE_REENTER,		// ���� ������ ( �� ������ ������ ������ ����Ѵ� );
			EMTYPE_OUT,			// ���� ���� ( �� ������ ���� ������ ����Ѵ� );
			EMTYPE_PROGRESS,	// �������� ���� ���� ( ���� �������� ���� ������ ����Ѵ� );
			EMTYPE_RETRY,		// �絵�� ���� ( ���� �絵�� ������ ����Ѵ� );
		};


	public:
		virtual int				Execute ( NetServer* _pServer );


	private:
		const std::string		m_strChaName;								// ���� �̸�;

		const LONGLONG			m_llRetryMoney;								// �絵���� ����� �ݾ�;

		const DWORD				m_dwChaDBNum;								// ���� DB Num;
		const DWORD				m_dwChaLevel;								// ���� ����;
		const DWORD				m_dwChaClass;								// ���� Ŭ����;
		const DWORD				m_dwStageNum;								// ���� �������� ��ȣ;
		const DWORD				m_dwAllUserCount;							// ��ü ���� ��;
		const DWORD				m_dwProgressUserCount;						// ���� ���� ��;
		const DWORD				m_dwRetryCount;								// �絵�� ��;


	private:
		const sc::MGUID			m_sGUID;									// ���� GUID;
		const std::string		m_strContentsName;							// ���� �̸�;
		const DWORD				m_dwContentsID;								// ���� Contents ID;
		const EMSUBTYPE_LOG		m_emLogType;								// �α� Ÿ�� ( EMTYPE_IN, EMTYPE_OUT ... );


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


	// DBAction ������� : ���� ��Ȳ ����;
	class DBAction_ResultAccumulate : public db::DbAction
	{
	public:
		virtual int				Execute ( NetServer* _pServer );


	private:
		const std::string		m_strChaName;								// ���� �̸�;

		const DWORD				m_dwChaDBNum;								// ���� DB Num;
		const DWORD				m_dwChaLevel;								// ���� ����;
		const DWORD				m_dwChaClass;								// ���� Ŭ����;
		const DWORD				m_dwStageNum;								// ������ �������� ��ȣ;
		const DWORD				m_dwReviveCount;							// ��ų ��Ȱ ��� Ƚ��;
		const DWORD				m_dwDamage;									// ���� ���������;
		const DWORD				m_dwHeal;									// ���� ����ȸ����;

		const bool				m_bMVP;										// ���� MVP ����;


	private:
		const sc::MGUID			m_sGUID;									// ���� GUID;
		const std::string		m_strContentsName;							// ���� �̸�;
		const DWORD				m_dwContentsID;								// ���� Contents ID;


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

	// DBAction �������� : ��� ���� ����;
	class DBAction_Reward : public db::DbAction
	{
	public:
		virtual int				Execute ( NetServer* _pServer );


	public:
		void					AddRewardItem ( int _nIndex, const std::string& _strItemID );


	private:
		std::string				m_arrRewardItem[ MAX_REWARD_ITEM_COUNT ];	// ���� ������ ����;


	private:
		const std::string		m_strChaName;								// ���� �̸�;

		const LONGLONG			m_llRewardMoney;							// ���� �ݾ�;

		const DWORD				m_dwChaDBNum;								// ���� DB Num;
		const DWORD				m_dwChaLevel;								// ���� ����;
		const DWORD				m_dwChaClass;								// ���� Ŭ����;
		const DWORD				m_dwRewardEXP;								// ���� ����ġ;
		const DWORD				m_dwRewardItemCount;						// ���� ������ ����;


	private:
		const sc::MGUID			m_sGUID;									// ���� GUID;
		const std::string		m_strContentsName;							// ���� �̸�;
		const DWORD				m_dwContentsID;								// ���� Contents ID;


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
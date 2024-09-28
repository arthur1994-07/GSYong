#include "pch.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../enginelib/guinterface/GameTextControl.h"

#include "DbActionLogInstantDungeon.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantInfiniteStaircaseStart::Execute( NetServer *pServer )
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("Infinite Staircase use ado function"));
            return sc::db::DB_ERROR;
        }
        
        std::string strGuid         = sc::string::uuidToString(m_DungeonGuid);
        sc::string::upperString(strGuid);
        std::string strPerson       = sc::string::format( "%1%", m_nPersonnel );
        std::string strChaNum       = sc::string::format( "%1%", m_dwChaNum );
        std::string strChaLevel     = sc::string::format( "%1%", m_nChaLevel );
        std::string strEntryCount   = sc::string::format( "%1%", m_nEntryCount );
        std::string strRemnantCout  = sc::string::format( "%1%", m_nRemnantCount );
		std::string strMapName		= ID2GAMEINTEXT( sc::string::format( "MAP_%d_%d", m_sMapID.wMainID, 0 ).c_str() );

        return m_pSubDbManager->InstantDungeonLogInsert(
            LOGTYPE_INFINITE_STAIRCASE,    // 무한의 계단 로그( 변경시 DB 테이블도 수정되어야함 )
            INFINITE_STAIRCASE_START,   // 무한의 계단로그의 종류 설정
			strMapName.c_str(),			// 던전 이름
            strGuid.c_str(),            // GUID
            strPerson.c_str(),          // 플레이중인 인원
            strChaNum.c_str(),          // 캐릭터 번호
            strChaLevel.c_str(),        // 레벨
            strEntryCount.c_str(),      // 오늘 플레이한 횟수
            strRemnantCout.c_str() );	// 남은 플레이 횟수


        return sc::db::DB_OK;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantInfiniteStaircasePlay::Execute( NetServer *pServer )
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("Infinite Staircase use ado function"));
            return sc::db::DB_ERROR;
        }

        std::string strPerson       = sc::string::format( "%1%", m_nPersonnel );
        std::string strChaNum       = sc::string::format( "%1%", m_dwChaNum );
        std::string strChaLevel     = sc::string::format( "%1%", m_nChaLevel );

        std::string strStageNum     = sc::string::format( "%1%", m_nStageNum );
		std::string strMapName		= ID2GAMEINTEXT( sc::string::format( "MAP_%d_%d", m_sMapID.wMainID, 0 ).c_str() );

        return m_pSubDbManager->InstantDungeonLogInsert(
            LOGTYPE_INFINITE_STAIRCASE,    // 무한의 계단 로그( 변경시 DB 테이블도 수정되어야함 )
            INFINITE_STAIRCASE_PLAY,    // 무한의 계단로그의 종류 설정
			strMapName.c_str(),			// 던전 이름
            m_strGuid.c_str(),          // GUID
            strPerson.c_str(),          // 플레이중인 인원
            strChaNum.c_str(),          // 캐릭터 번호
            strChaLevel.c_str(),        // 레벨
            NULL,                       // 오늘 플레이한 횟수
            NULL,                       // 남은 플레이 횟수
            strStageNum.c_str());		// 현재 스테이지

        return sc::db::DB_OK;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantInfiniteStaircaseEnd::Execute( NetServer *pServer )
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("Infinite Staircase use ado function"));
            return sc::db::DB_ERROR;
        }

        std::string strPerson       = sc::string::format( "%1%", m_nPersonnel );
        std::string strChaNum       = sc::string::format( "%1%", m_dwChaNum );
        std::string strChaLevel     = sc::string::format( "%1%", m_nChaLevel );
        std::string strStageNum     = sc::string::format( "%1%", m_nStageNum );

        std::string strMVP          = sc::string::format( "%1%", m_bMVP );
        std::string strMoney        = sc::string::format( "%1%", m_llMoney );
        std::string strEXP          = sc::string::format( "%1%", m_nExp );

		std::string strMapName		= ID2GAMEINTEXT( sc::string::format( "MAP_%d_%d", m_sMapID.wMainID, 0 ).c_str() );

        return m_pSubDbManager->InstantDungeonLogInsert(
            LOGTYPE_INFINITE_STAIRCASE,    // 무한의 계단 로그( 변경시 DB 테이블도 수정되어야함 )
            INFINITE_STAIRCASE_END,     // 무한의 계단로그의 종류 설정
			strMapName.c_str(),			// 던전 이름
            m_strGuid.c_str(),          // GUID
            strPerson.c_str(),          // 플레이중인 인원
            strChaNum.c_str(),          // 캐릭터 번호
            strChaLevel.c_str(),        // 레벨
            NULL,                       // 오늘 플레이한 횟수
            NULL,                       // 남은 플레이 횟수
            strStageNum.c_str(),        // 현재 스테이지
            strMVP.c_str(),             // MVP 여부
            strMoney.c_str(),           // 받은 골드
            strEXP.c_str());			// 받은 경험치

        return sc::db::DB_OK;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantNewInstance::Execute( NetServer *pServer )
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("LogInstantNewInstance use ado function"));
            return sc::db::DB_ERROR;
        }

        switch ( nLogSubType )
        {
        case CREATE:
            {
                std::string strkeyMapMID        = sc::string::format( "%1%", wKeyMapMID );
                std::string strkeyMapSID        = sc::string::format( "%1%", wKeyMapSID );
                std::string strBasMapID         = sc::string::format( "%1%", dwBasMapID );
                std::string strInstanceMapID    = sc::string::format( "%1%", dwInstanceMapID );
                std::string strCreateDate       = sc::string::DateTimeFormat(tCreateDateTime,true,true);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_NEW_INSTANCE,                           // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_NEW_INSTANCE_CREATE,                // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),  // GUID;
                    strInstanceName.c_str(),                        // 인던 이름;
                    strkeyMapMID.c_str(),                           // 인던 키 맵 MID;
                    strkeyMapSID.c_str(),                           // 인던 키 맵 SID;
                    strBasMapID.c_str(),                            // 베이스 맵 ID; 
                    strInstanceMapID.c_str(),                       // 맵 ID;
                    strCreateDate.c_str(),                          // 인던 생성 시간;
                    NULL
                    );
            }
            break;

        case DESTROY:
            {
                std::string strBasMapID         = sc::string::format( "%1%", dwBasMapID );
                std::string strInstanceMapID    = sc::string::format( "%1%", dwInstanceMapID );
                std::string strDestroyDate      = sc::string::DateTimeFormat(tDestroyDateTime,true,true);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_NEW_INSTANCE,                           // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_NEW_INSTANCE_DESTROY,               // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),  // GUID;
                    strInstanceName.c_str(),                        // 인던 이름;
                    NULL,
                    NULL,
                    strBasMapID.c_str(),                            // 베이스 맵 ID; 
                    strInstanceMapID.c_str(),                       // 맵 ID;
                    NULL,
                    strDestroyDate.c_str()
                    );
            }
            break;

        default:
            sc::writeLogError(std::string("LogInstantNewInstance Unknown Log SubType"));
            return sc::db::DB_ERROR;
        }

        return sc::db::DB_OK;
    }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantCDM::Execute(NetServer *pServer)
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("LogInstantCDM use ado function"));
            return sc::db::DB_ERROR;
        }

        switch( nLogSubType)
        {
        case EM_IN:
            {
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
                std::string strPlayerDbNum      = sc::string::format("%1%", dwCharDbNum);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CDM,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CDM_USER_IN,                     // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strClubDbNum.c_str(),
                    strPlayerDbNum.c_str()
                    );
            }
            break;

        case EM_OUT:
            {
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
                std::string strPlayerDbNum      = sc::string::format("%1%", dwCharDbNum);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CDM,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CDM_USER_OUT,                     // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strClubDbNum.c_str(),
                    strPlayerDbNum.c_str()
                    );
            }
            break;

        case EM_RESULT:
            {
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
                //std::string strPlayerDbNum      = sc::string::format("%1%", dwCharDbNum);
                std::string strClubRank         = sc::string::format("%1%", nClubRank);
                std::string strClubPoint        = sc::string::format("%1%", nClubPoint);
                std::string strClubKill         = sc::string::format("%1%", nClubKill);
                std::string strClubDeath        = sc::string::format("%1%", nClubDeath);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CDM,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CDM_CLUB_RESULT,                       // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strClubDbNum.c_str(),
                    NULL,
                    strClubRank.c_str(),
                    strClubPoint.c_str(),
                    strClubKill.c_str(),
                    strClubDeath.c_str()
                    );
            }
            break;

        default:
            sc::writeLogError(std::string("LogInstantCDM Unknown Log SubType"));
            return sc::db::DB_ERROR;
        }

        return sc::db::DB_OK;        
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantCaptureTheFlag::Execute(NetServer *pServer)
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("LogInstantCaptureTheFlag use ado function"));
            return sc::db::DB_ERROR;
        }

        switch( nLogSubType)
        {
        case EM_IN:
            {
                std::string strCharDbNum        = sc::string::format("%1%", dwCharDbNum);
                std::string strTeam             = sc::string::format("%1%", nTeam);
                std::string strRating           = sc::string::format("%1%", nRating);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CAPTURE_THE_FLAG,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CAPTURE_THE_FLAG_IN,                        // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strCharDbNum.c_str(),
                    strTeam.c_str(),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    strRating.c_str()
                    );
            }
            break;

        case EM_OUT:
            {
                std::string strCharDbNum        = sc::string::format("%1%", dwCharDbNum);
                std::string strTeam             = sc::string::format("%1%", nTeam);
                std::string strRating           = sc::string::format("%1%", nRating);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CAPTURE_THE_FLAG,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CAPTURE_THE_FLAG_OUT,                       // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strCharDbNum.c_str(),
                    strTeam.c_str(),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    strRating.c_str()
                    );
            }
            break;

        case EM_RESULT:
            {
                std::string strTeam             = sc::string::format("%1%", nTeam);
                std::string strResult           = bResult ? "WIN" : "LOSE";
                std::string strFlagScore        = sc::string::format("%1%", nFlagScore);
                std::string strKill             = sc::string::format("%1%", nKill);
                std::string strDeath            = sc::string::format("%1%", nDeath);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CAPTURE_THE_FLAG,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CAPTURE_THE_FLAG_RESULT,                    // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    NULL,
                    strTeam.c_str(),
                    strResult.c_str(),
                    strFlagScore.c_str(),
                    strKill.c_str(),
                    strDeath.c_str()
                    );
            }
            break;

        case EM_REWARD:
            {
                std::string strCharDbNum        = sc::string::format("%1%", dwCharDbNum);
                std::string strReward           = sc::string::format("%1%", nReward);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CAPTURE_THE_FLAG,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CAPTURE_THE_FLAG_REWARD,                    // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strCharDbNum.c_str(),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    strReward.c_str()
                    );
            }
            break;

        case EM_RATING_CHANGE:
            {
                std::string strCharDbNum        = sc::string::format("%1%", dwCharDbNum);
                std::string strRating           = sc::string::format("%1%", nRating);
                std::string strChangedRating    = sc::string::format("%1%", nChangedRating);
                std::string strWinningStreak    = sc::string::format("%1%", nWinningStreak);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_CAPTURE_THE_FLAG,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_CAPTURE_THE_FLAG_RATING_CHANGE,             // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strCharDbNum.c_str(),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    strRating.c_str(),
                    strChangedRating.c_str(),
                    strWinningStreak.c_str()
                    );
            }
            break;

        default:
            sc::writeLogError(std::string("LogInstantCaptureTheFlag Unknown Log SubType"));
            return sc::db::DB_ERROR;
        }

        return sc::db::DB_OK;
    }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int LogInstantGuidance::Execute(NetServer *pServer)
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("LogInstantCDM use ado function"));
            return sc::db::DB_ERROR;
        }

        switch( nLogSubType)
        {
        case EM_IN:
            {
                std::string strPlayerDbNum      = sc::string::format("%1%", dwCharDbNum);
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
				std::string strClubPoint        = sc::string::format("%1%", nClubPoint);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_GUIDANCE,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_GUIDANCE_USER_IN,                     // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strGuidanceName.c_str(),
                    strPlayerDbNum.c_str(),
                    strClubDbNum.c_str(),
					strClubPoint.c_str()
                    );
            }
            break;

        case EM_OUT:
            {
                std::string strPlayerDbNum      = sc::string::format("%1%", dwCharDbNum);
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);       
				std::string strClubPoint        = sc::string::format("%1%", nClubPoint);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_GUIDANCE,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_GUIDANCE_USER_OUT,                     // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strGuidanceName.c_str(),
                    strPlayerDbNum.c_str(),
                    strClubDbNum.c_str(),
					strClubPoint.c_str()
                    );
            }
            break;

        case EM_RESULT:
            {
                std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
                std::string strClubPoint        = sc::string::format("%1%", nClubPoint);
                std::string strClubHaveAuthNum  = sc::string::format("%1%", nClubHaveAuthNum);
                std::string strClubAuthCount    = sc::string::format("%1%", nClubAuthCount);
    
                std::string strSecondClubDbNum        = sc::string::format("%1%", dwSecondClubDbNum);
                std::string strSecondClubPoint        = sc::string::format("%1%", nSecondClubPoint);
                std::string strSecondClubHaveAuthNum  = sc::string::format("%1%", nSecondClubHaveAuthNum);
                std::string strSecondClubAuthCount    = sc::string::format("%1%", nSecondClubAuthCount);

                std::string strThirdClubDbNum        = sc::string::format("%1%", dwThirdClubDbNum);
                std::string strThirdClubPoint        = sc::string::format("%1%", nThirdClubPoint);
                std::string strThirdClubHaveAuthNum  = sc::string::format("%1%", nThirdClubHaveAuthNum);
                std::string strThirdClubAuthCount    = sc::string::format("%1%", nThirdClubAuthCount);

                return m_pSubDbManager->InstantDungeonLogInsert(
                    LOGTYPE_COMPETITION_GUIDANCE,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                    LOG_SUBTYPE_GUIDANCE_RESULT,                       // 로그의 종류 설정 ( 생성, 파괴 );
                    sc::string::uuidToString(UUID(dungeonGuid)).c_str(),    // GUID;
                    strGuidanceName.c_str(),
                    NULL,
                    strClubDbNum.c_str(),
                    strClubPoint.c_str(),
                    strClubHaveAuthNum.c_str(),
                    strClubAuthCount.c_str(),
                    strSecondClubDbNum.c_str(),
                    strSecondClubPoint.c_str(),
                    strSecondClubHaveAuthNum.c_str(),
                    strSecondClubAuthCount.c_str(), 
                    strThirdClubDbNum.c_str(),
                    strThirdClubPoint.c_str(),
                    strThirdClubHaveAuthNum.c_str(),
                    strThirdClubAuthCount.c_str()
                    );
            }
            break;

        case EM_REWARD:
            {
				// 요청에 따라 보상 로그는 기록하지 않도록 변경

                //std::string strGuidanceID       = sc::string::format("%1%", dwCharDbNum);
                //std::string strClubDbNum        = sc::string::format("%1%", dwClubDbNum);
                //std::string strCommission       = sc::string::format("%1%", nClubPoint);

                //return m_pSubDbManager->InstantDungeonLogInsert(
                //    LOGTYPE_COMPETITION_GUIDANCE,                   // 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
                //    LOG_SUBTYPE_GUIDANCE_REWARD,                       // 로그의 종류 설정 ( 생성, 파괴 );
                //    NULL,                                           // GUID는 없음;
                //    strGuidanceName.c_str(),
                //    strGuidanceID.c_str(),
                //    strClubDbNum.c_str(),
                //    strCommission.c_str()
                //    );
            }
            break;

        default:
            sc::writeLogError(std::string("LogInstantCDM Unknown Log SubType"));
            return sc::db::DB_ERROR;
        }

        return sc::db::DB_OK;        
    }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int LogInstantCTI::Execute ( NetServer *pServer )
	{
		if ( NULL == m_pSubDbManager )
		{
			sc::writeLogError( std::string("LogInstantCTI use ado function") );

			return sc::db::DB_ERROR;
		}

		Country::GLCountryManServer* pCountryMan = Country::GLCountryManServer::Instance();
		if ( NULL == pCountryMan )
			return sc::db::DB_ERROR;

		switch ( nLogSubType )
		{
		case EM_IN:
			{
				std::string strPlayerDbNum      = sc::string::format( "%1%", dwCharDbNum );
				std::string strCountryID        = sc::string::format( "%1%", dwCountryID );
				std::string strCountryPoint     = sc::string::format( "%1%", nCountryPoint );

				Country::GLCountry* pCountry = pCountryMan->GetCountryUsingID( dwCountryID );
				if ( pCountry )
				{
					strCountryID = pCountry->GetCountryName();
				}

				return m_pSubDbManager->InstantDungeonLogInsert(
					LOGTYPE_COMPETITION_CTI,									// 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
					LOG_SUBTYPE_GUIDANCE_USER_IN,								// 로그의 종류 설정 ( 생성, 파괴 );
					sc::string::uuidToString( UUID( dungeonGuid ) ).c_str(),	// GUID;
					strCTIName.c_str(),
					strPlayerDbNum.c_str(),
					strCountryID.c_str(),
					strCountryPoint.c_str() );
			}
			break;

		case EM_OUT:
			{
				std::string strPlayerDbNum      = sc::string::format( "%1%", dwCharDbNum );
				std::string strCountryID        = sc::string::format( "%1%", dwCountryID );       
				std::string strCountryPoint        = sc::string::format( "%1%", nCountryPoint );

				Country::GLCountry* pCountry = pCountryMan->GetCountryUsingID( dwCountryID );
				if ( pCountry )
				{
					strCountryID = pCountry->GetCountryName();
				}

				return m_pSubDbManager->InstantDungeonLogInsert(
					LOGTYPE_COMPETITION_CTI,									// 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
					LOG_SUBTYPE_GUIDANCE_USER_OUT,								// 로그의 종류 설정 ( 생성, 파괴 );
					sc::string::uuidToString( UUID( dungeonGuid ) ).c_str(),    // GUID;
					strCTIName.c_str(),
					strPlayerDbNum.c_str(),
					strCountryID.c_str(),
					strCountryPoint.c_str()
					);
			}
			break;

		case EM_RESULT:
			{
				std::string strCountryID				= sc::string::format( "%1%", dwCountryID );
				std::string strCountryPoint				= sc::string::format( "%1%", nCountryPoint );
				std::string strCountryHaveAuthNum		= sc::string::format( "%1%", nCountryHaveAuthNum );
				std::string strCountryAuthCount			= sc::string::format( "%1%", nCountryAuthCount );

				std::string strSecondCountryID			= sc::string::format( "%1%", dwSecondCountryID );
				std::string strSecondCountryPoint       = sc::string::format( "%1%", nSecondCountryPoint );
				std::string strSecondCountryHaveAuthNum = sc::string::format( "%1%", nSecondCountryHaveAuthNum );
				std::string strSecondCountryAuthCount   = sc::string::format( "%1%", nSecondCountryAuthCount );

				std::string strThirdCountryID			= sc::string::format( "%1%", dwThirdCountryID );
				std::string strThirdCountryPoint        = sc::string::format( "%1%", nThirdCountryPoint );
				std::string strThirdCountryHaveAuthNum  = sc::string::format( "%1%", nThirdCountryHaveAuthNum );
				std::string strThirdCountryAuthCount    = sc::string::format( "%1%", nThirdCountryAuthCount );

				Country::GLCountry* pCountry = pCountryMan->GetCountryUsingID( dwCountryID );
				if ( pCountry )
				{
					strCountryID = pCountry->GetCountryName();
				}

				Country::GLCountry* pSecondCountry = pCountryMan->GetCountryUsingID( dwSecondCountryID );
				if ( pSecondCountry )
				{
					strSecondCountryID = pSecondCountry->GetCountryName();
				}

				Country::GLCountry* pThirdCountry = pCountryMan->GetCountryUsingID( dwThirdCountryID );
				if ( pThirdCountry )
				{
					strThirdCountryID = pThirdCountry->GetCountryName();
				}

				return m_pSubDbManager->InstantDungeonLogInsert(
					LOGTYPE_COMPETITION_CTI,									// 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
					LOG_SUBTYPE_GUIDANCE_RESULT,								// 로그의 종류 설정 ( 생성, 파괴 );
					sc::string::uuidToString( UUID( dungeonGuid ) ).c_str(),    // GUID;
					strCTIName.c_str(),
					NULL,
					strCountryID.c_str(),
					strCountryPoint.c_str(),
					strCountryHaveAuthNum.c_str(),
					strCountryAuthCount.c_str(),
					strSecondCountryID.c_str(),
					strSecondCountryPoint.c_str(),
					strSecondCountryHaveAuthNum.c_str(),
					strSecondCountryAuthCount.c_str(), 
					strThirdCountryID.c_str(),
					strThirdCountryPoint.c_str(),
					strThirdCountryHaveAuthNum.c_str(),
					strThirdCountryAuthCount.c_str()
					);
			}
			break;

		case EM_REWARD:
			{
				std::string strCTIID					= sc::string::format( "%1%", dwCharDbNum );
				std::string strCountryID				= sc::string::format( "%1%", dwCountryID );
				std::string strCommission				= sc::string::format( "%1%", nCountryPoint );

				return m_pSubDbManager->InstantDungeonLogInsert(
					LOGTYPE_COMPETITION_CTI,									// 신 인던 맵 로그 ( 변경시 DB 테이블도 수정되어야함 );
					LOG_SUBTYPE_GUIDANCE_REWARD,								// 로그의 종류 설정 ( 생성, 파괴 );
					NULL,														// GUID는 없음;
					strCTIName.c_str(),
					strCTIID.c_str(),
					strCountryID.c_str(),
					strCommission.c_str()
					);
			}
			break;

		default:
			sc::writeLogError(std::string("LogInstantCTI Unknown Log SubType"));
			return sc::db::DB_ERROR;
		}

		return sc::db::DB_OK;        
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int LogTournament::Execute(NetServer *pServer)
    {
        if ( m_pSubDbManager == NULL )
        {
            sc::writeLogError(std::string("LogTournament use ado function"));
            return sc::db::DB_ERROR;
        }

		std::string strLogType;
		switch(nLogSubType)
		{
		case 9001:
			strLogType = "토너먼트 시작";
			break;
		case 9002:
			strLogType = "토너먼트 라운드 시작";
			break;
		case 9003:
			strLogType = "토너먼트 라운드 부전승";
			break;
		case 9004:
			strLogType = "토너먼트 라운드 결과";
			break;
		case 9005:
			strLogType = "토너먼트 종료";
			break;
		case 9006:
			strLogType = "토너먼트 보상";
			break;
		case 9101:
			strLogType = "토너먼트 배팅";
			break;
		case 9102:
			strLogType = "토너먼트 배팅 취소";
			break;
		case 9103:
			strLogType = "토너먼트 배팅 보상";
			break;
		default:
			strLogType = "알수없음";
			break;
		}
		
        std::string strCharDbNum        = sc::string::format("%1%", dwCharDbNum);
        std::string strClass        = sc::string::format("%1%", dwClass);
        std::string strSchool  = sc::string::format("%1%", dwSchool);
        std::string strPartyID    = sc::string::format("%1%", dwPartyID);
        std::string strMoney        = sc::string::format("%1%", nMoney);
        std::string strwItemMainID        = sc::string::format("%1%", dwItemMainID);
        std::string strItemSubID  = sc::string::format("%1%", dwItemSubID);
        std::string	strItemCount    = sc::string::format("%1%", dwItemCount);

		std::string strTotoTargetChaNum    = sc::string::format("%1%", dwTotoTargetChaNum);
		std::string strCanObserve    = sc::string::format("%1%", dwCanObserve);
		std::string strCanToto    = sc::string::format("%1%", dwCanToto);

		std::string strFactionID    = sc::string::format("%1%", dwFactionID);
		std::string strMapID    = sc::string::format("%1%", dwMapID);
		std::string strForceWin    = sc::string::format("%1%", dwForceWin);

		std::string strTotalMemberCount    = sc::string::format("%1%", dwTotalMemberCount);
		std::string strTournamentSize    = sc::string::format("%1%", dwTournamentSize);
		std::string strTournamentNodeCount    = sc::string::format("%1%", dwTournamentNodeCount);

        return m_pSubDbManager->InstantDungeonLogInsert(
			LOGTYPE_TOURNAMENT,
            strLogType.c_str(), 
            strChaName.c_str(),
            strContentName.c_str(),

            strCharDbNum.c_str(),
            strClass.c_str(),
            strSchool.c_str(),
            strPartyID.c_str(),

            strMoney.c_str(),
            strwItemMainID.c_str(),
            strItemSubID.c_str(),
            strItemCount.c_str(),
			strTotoTargetChaNum.c_str(),
			strCanObserve.c_str(),
			strCanToto.c_str(),

			strFactionID.c_str(),
			strMapID.c_str(), 

			strForceWin.c_str(), 
			strTotalMemberCount.c_str(), 
			strTournamentSize.c_str(), 
			strTournamentNodeCount.c_str()
            );    
    }



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 기록되어야 할 정보별로 로그를 정리한다;

	int DBAction_ProgressStage::Execute ( NetServer* _pServer )
	{
		if ( NULL == m_pSubDbManager )
		{
			sc::writeLogError(
				std::string( "[ DB Log ] [ Failed Execute DBAction_ProgressStage, Ado Manager is NULL ]" ) );

			return sc::db::DB_ERROR;
		}

		std::string strSubType;

		switch ( m_emLogType )
		{
		case EMTYPE_ENTER:
			{
				strSubType = LOG_SUBTYPE_ENTER_USER;
			}
			break;

		case EMTYPE_REENTER:
			{
				strSubType = LOG_SUBTYPE_REENTER_USER;
			}
			break;

		case EMTYPE_OUT:
			{
				strSubType = LOG_SUBTYPE_OUT_USER;
			}
			break;

		case EMTYPE_PROGRESS:
			{
				strSubType = LOG_SUBTYPE_PROGRESS;
			}
			break;

		case EMTYPE_RETRY:
			{
				strSubType = LOG_SUBTYPE_RETRY;
			}
			break;

		default:
			{
				sc::writeLogError( "[ DB Log ] [ Failed Execute DBAction_ProgressStage, Invalid LogSubType ]" );
			}

			return sc::db::DB_ERROR;
		}

		return m_pSubDbManager->InstantDungeonLogInsert(
			LOGTYPE_PROGRESS_STAGE_INFO,
			strSubType.c_str(),
			sc::string::uuidToString( UUID( m_sGUID ) ).c_str(),
			m_strContentsName.c_str(),
			sc::string::format( "%1%", m_dwContentsID ).c_str(),
			sc::string::format( "%1%", m_dwChaDBNum ).c_str(),
			m_strChaName.c_str(),
			sc::string::format( "%1%", m_dwChaLevel ).c_str(),
			sc::string::format( "%1%", m_dwChaClass ).c_str(),
			sc::string::format( "%1%", m_dwStageNum ).c_str(),
			sc::string::format( "%1%", m_dwAllUserCount ).c_str(),
			sc::string::format( "%1%", m_dwProgressUserCount ).c_str(),
			sc::string::format( "%1%", m_dwRetryCount ).c_str(),
			sc::string::format( "%1%", m_llRetryMoney ).c_str() );
	}


	int DBAction_ResultAccumulate::Execute ( NetServer* _pServer )
	{
		if ( NULL == m_pSubDbManager )
		{
			sc::writeLogError(
				std::string( "[ DB Log ] [ Failed Execute DBAction_ResultAccumulate, Ado Manager is NULL ]" ) );

			return sc::db::DB_ERROR;
		}

		return m_pSubDbManager->InstantDungeonLogInsert(
			LOGTYPE_RESULT_ACCUMULATE_INFO,
			LOG_SUBTYPE_RESULT,
			sc::string::uuidToString( UUID( m_sGUID ) ).c_str(),
			m_strContentsName.c_str(),
			sc::string::format( "%1%", m_dwContentsID ).c_str(),
			sc::string::format( "%1%", m_dwChaDBNum ).c_str(),
			m_strChaName.c_str(),
			sc::string::format( "%1%", m_dwChaLevel ).c_str(),
			sc::string::format( "%1%", m_dwChaClass ).c_str(),
			sc::string::format( "%1%", m_dwStageNum ).c_str(),
			sc::string::format( "%1%", m_dwReviveCount ).c_str(),
			sc::string::format( "%1%", m_dwDamage ).c_str(),
			sc::string::format( "%1%", m_dwHeal ).c_str(),
			sc::string::format( "%1%", m_bMVP ).c_str() );
	}


	void DBAction_Reward::AddRewardItem ( int _nIndex, const std::string& _strItemID )
	{
		if ( _nIndex < 0 || MAX_REWARD_ITEM_COUNT <= _nIndex )
			return;

		m_arrRewardItem[ _nIndex ] = _strItemID;
	}

	int DBAction_Reward::Execute ( NetServer* _pServer )
	{
		if ( NULL == m_pSubDbManager )
		{
			sc::writeLogError(
				std::string( "[ DB Log ] [ Failed Execute DBAction_Reward, Ado Manager is NULL ]" ) );

			return sc::db::DB_ERROR;
		}

		if ( m_dwRewardItemCount >= MAX_REWARD_ITEM_COUNT )
			return sc::db::DB_ERROR;

		return m_pSubDbManager->InstantDungeonLogInsert(
			LOGTYPE_REWARD_INFO,
			LOG_SUBTYPE_REWARD,
			sc::string::uuidToString( UUID( m_sGUID ) ).c_str(),
			m_strContentsName.c_str(),
			sc::string::format( "%1%", m_dwContentsID ).c_str(),
			sc::string::format( "%1%", m_dwChaDBNum ).c_str(),
			m_strChaName.c_str(),
			sc::string::format( "%1%", m_dwChaLevel ).c_str(),
			sc::string::format( "%1%", m_dwChaClass ).c_str(),
			sc::string::format( "%1%", m_llRewardMoney ).c_str(),
			sc::string::format( "%1%", m_dwRewardEXP ).c_str(),
			sc::string::format( "%1%", m_dwRewardItemCount ).c_str(),
			m_arrRewardItem[0].c_str(),
			m_arrRewardItem[1].c_str(),
			m_arrRewardItem[2].c_str(),
			m_arrRewardItem[3].c_str(),
			m_arrRewardItem[4].c_str() );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace db

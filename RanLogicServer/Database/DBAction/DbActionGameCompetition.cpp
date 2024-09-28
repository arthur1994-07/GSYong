#include "pch.h"
#include "./DbActionGameCompetition.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"

#include "../../../RanLogic/Msg/GLContrlMatching.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
    //DbAction_CompetitionRatingGetInfo
    DbAction_CompetitionRatingGetInfo::DbAction_CompetitionRatingGetInfo(const std::string& _strContentType, const DWORD _dwCharDbNum, const DWORD _dwScriptID)
        : strContentType(_strContentType)
        , dwCharDbNum(_dwCharDbNum)
        , dwScriptID(_dwScriptID)
    {

    }

    DbAction_CompetitionRatingGetInfo::~DbAction_CompetitionRatingGetInfo()
    {

    }

    int DbAction_CompetitionRatingGetInfo::Execute( NetServer* pServer)
    {
        if( !pServer )
            return NET_ERROR;

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
        if( !pAgentServer )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionRatingGetInfo): pAgentServer is NULL.") );
            return sc::db::DB_ERROR;
        }

        if( m_pDbManager->GetConnectionType() != db::ADO )
        {
            sc::writeLogError( "DbAction_CompetitionRatingGetInfo::Execute. Use ADO" );
            return NET_ERROR;
        }

        int nReturn = NET_ERROR;

        int nRating =0;
        int nWinningStreak = 0;
        int nPlayCount = 0;
        __time64_t tLastPlayDate = 0;

        nReturn = m_pDbManager->CompetitionRatingGetInfo( strContentType, dwCharDbNum, nRating, nWinningStreak, nPlayCount, tLastPlayDate );
        if( nReturn == sc::db::DB_ERROR )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionRatingGetInfo) failed to call CompetitionRatingGetInfo().") );
            return nReturn;
        }

        GLMSG::NET_MATCHING_COMPETITION_RATING_GETINFO_DA netMsg;
        memset(netMsg.strContentType, 0, sizeof(netMsg.strContentType));
        strcpy_s(netMsg.strContentType, sizeof(netMsg.strContentType), strContentType.c_str());
        netMsg.dwCharNum = dwCharDbNum;
        netMsg.nRating = nRating;
        netMsg.nWinningStreak = nWinningStreak;
        netMsg.nPlayCount = nPlayCount;
        netMsg.tLastPlayDate = tLastPlayDate;

        netMsg.dwScriptID = dwScriptID;

        pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &netMsg );

        return nReturn;
    }



    //DbAction_CompetitionRatingUpdate
    DbAction_CompetitionRatingUpdate::DbAction_CompetitionRatingUpdate( const std::string& _strContentType,
                                                                        const DWORD _dwCharDbNum,
                                                                        const int _Rating,
                                                                        const int _WinningStreak,
                                                                        const int _PlayCount,
                                                                        const __time64_t _tLastPlayDate)
        : strContentType(_strContentType)
        , dwCharDbNum(_dwCharDbNum)
        , Rating(_Rating)
        , WinningStreak(_WinningStreak)
        , PlayCount(_PlayCount)
        , tLastPlayDate(_tLastPlayDate)
    {

    }

    DbAction_CompetitionRatingUpdate::~DbAction_CompetitionRatingUpdate()
    {

    }

    int DbAction_CompetitionRatingUpdate::Execute( NetServer* pServer)
    {
        if( !pServer )
            return NET_ERROR;

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
        if( !pAgentServer )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionRatingUpdate): pAgentServer is NULL.") );
            return sc::db::DB_ERROR;
        }

        if( m_pDbManager->GetConnectionType() != db::ADO )
        {
            sc::writeLogError( "DbAction_CompetitionRatingUpdate::Execute. Use ADO" );
            return NET_ERROR;
        }

        int nReturn = NET_ERROR;
        nReturn = m_pDbManager->CompetitionRatingUpdate( strContentType, dwCharDbNum, Rating, WinningStreak, PlayCount, tLastPlayDate );
        if( nReturn == sc::db::DB_ERROR )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionRatingUpdate) failed to call CompetitionRatingUpdate().") );
        }

        return nReturn;
    }



    // DbAction_CompetitionSeasonUpdate
    DbAction_CompetitionSeasonUpdate::DbAction_CompetitionSeasonUpdate( const std::string& _strContentType, const int _seasonNum, const int _baseRating )
        : strContentType(_strContentType)
        , seasonNum(_seasonNum)
        , baseRating(_baseRating)
    {

    }

    DbAction_CompetitionSeasonUpdate::~DbAction_CompetitionSeasonUpdate()
    {

    }

    int DbAction_CompetitionSeasonUpdate::Execute( NetServer* pServer)
    {
        if( !pServer )
            return NET_ERROR;

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
        if( !pAgentServer )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionSeasonUpdate): pAgentServer is NULL.") );
            return sc::db::DB_ERROR;
        }

        if( m_pDbManager->GetConnectionType() != db::ADO )
        {
            sc::writeLogError( "DbAction_CompetitionSeasonUpdate::Execute. Use ADO" );
            return NET_ERROR;
        }

        int nReturn = NET_ERROR;
        nReturn = m_pDbManager->CompetitionSeasonUpdate( strContentType, seasonNum, baseRating );
        if( nReturn == sc::db::DB_ERROR )
        {
            sc::writeLogError( std::string("DbAction(DbAction_CompetitionSeasonUpdate) failed to call CompetitionSeasonUpdate().") );
        }

        return nReturn;
    }
}
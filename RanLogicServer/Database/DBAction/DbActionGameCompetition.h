#pragma once

#include "DbAction.h"

namespace db
{
    // Load;
    class DbAction_CompetitionRatingGetInfo : public db::DbAction
    {
    public:
        DbAction_CompetitionRatingGetInfo( const std::string& _strContentType, const DWORD _dwCharDbNum, const DWORD _dwScriptID );
        virtual ~DbAction_CompetitionRatingGetInfo();
        virtual int Execute( NetServer* pServer ) override;

    private:
        const std::string& strContentType;
        const DWORD dwCharDbNum;
        const DWORD dwScriptID;
    };

    class DbAction_CompetitionRatingUpdate : public db::DbAction
    {
    public:
        DbAction_CompetitionRatingUpdate(   const std::string& _strContentType,
                                            const DWORD _dwCharDbNum,
                                            const int _Rating,
                                            const int _WinningStreak,
                                            const int _PlayCount,
                                            const __time64_t _tLastPlayDate);
        virtual ~DbAction_CompetitionRatingUpdate();
        virtual int Execute( NetServer* pServer ) override;

    private:
        const std::string strContentType;
        const DWORD dwCharDbNum;
        const int Rating;
        const int WinningStreak;
        const int PlayCount;
        const __time64_t tLastPlayDate;
    };

    class DbAction_CompetitionSeasonUpdate : public db::DbAction
    {
    public:
        DbAction_CompetitionSeasonUpdate( const std::string& _strContentType, const int _seasonNum, const int _baseRating );
        virtual ~DbAction_CompetitionSeasonUpdate();
        virtual int Execute( NetServer* pServer ) override;

    private:
        const std::string strContentType;
        const int seasonNum;
        const int baseRating;
    };
}
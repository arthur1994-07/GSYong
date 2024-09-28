#include "pch.h"
#include "MatchSystem/MatchingManager.h"
#include "MatchSystem/MatchingDefine.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../Database/DBAction/DbActionGameCompetition.h"

namespace MatchingSystem
{
    CompetitionRatingManager::CompetitionRatingManager()
    {
    }
    CompetitionRatingManager::~CompetitionRatingManager()
    {
        SaveRatingInfo();
    }

    CompetitionRatingInfo* CompetitionRatingManager::GetRatingInfo(const DWORD dwCharDbNum, const std::string& strContentType)
    {
        RATING_MAP_ITER iter = m_ratingMap.find(RATING_MAP_KEY_PAIR(dwCharDbNum, strContentType));
        if (iter != m_ratingMap.end())
            return &(iter->second);

        return NULL;
    }

    int CompetitionRatingManager::LoadRatingInfo(const DWORD dwCharDbNum, const std::string& strContentType, const DWORD dwScriptID)
    {
        if( gpAgentServer )
        {
            gpAgentServer->AddGameAdoJob( db::DbActionPtr(
                new db::DbAction_CompetitionRatingGetInfo( strContentType, dwCharDbNum, dwScriptID ) ) );
        }
        return 0;
    }

    int CompetitionRatingManager::LoadRatingInfoFB(const NET_MSG_GENERIC* netMsg)
    {
        const GLMSG::NET_MATCHING_COMPETITION_RATING_GETINFO_DA* pMsg = (const GLMSG::NET_MATCHING_COMPETITION_RATING_GETINFO_DA*)netMsg;
        CompetitionRatingInfo sInfo;
        sInfo.Rating = pMsg->nRating;
        sInfo.WinningStreak = pMsg->nWinningStreak;
        sInfo.PlayCount = pMsg->nPlayCount;
        sInfo.LastPlayDate = pMsg->tLastPlayDate;

        std::string _strContentType(pMsg->strContentType);
        m_ratingMap.insert(std::pair<RATING_MAP_KEY_PAIR, CompetitionRatingInfo> (RATING_MAP_KEY_PAIR(pMsg->dwCharNum, _strContentType), sInfo));

        return 0;
    }

    int CompetitionRatingManager::SaveRatingInfo() // DB에 현재 Rating정보를 전부 저장한다;
    {
        if( gpAgentServer )
        {
            RATING_MAP_ITER iter = m_ratingMap.begin();
            for ( ; iter != m_ratingMap.end(); iter++ )
            {
                const RATING_MAP_KEY_PAIR& sPair = iter->first;
                CompetitionRatingInfo& sInfo = iter->second;
                gpAgentServer->AddGameAdoJob( db::DbActionPtr(
                    new db::DbAction_CompetitionRatingUpdate( sPair.second, sPair.first, sInfo.Rating, sInfo.WinningStreak, sInfo.PlayCount, sInfo.LastPlayDate ) ) );
            }
            m_ratingMap.clear();
        }
        return 0;
    }
}
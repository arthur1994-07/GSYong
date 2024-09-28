#include "../../pch.h"
#include "../DbManager.h"
#include "./DbActionGamePointShop.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

CharacterPointSet::CharacterPointSet(
    DWORD ChaDbNum,
    __int64 PointRefundable,
    __int64 PointNotRefundable)
    : DbAction(GAME_DB)
    , m_ChaDbNum(ChaDbNum)
    , m_PointRefundable(PointRefundable)
    , m_PointNotRefundable(PointNotRefundable)
{   
}

CharacterPointSet::~CharacterPointSet()
{
}

int CharacterPointSet::Execute(NetServer* pServer)
{
    return m_pDbManager->SetChaPoint(m_ChaDbNum, m_PointRefundable, m_PointNotRefundable);
}


} // namespace db
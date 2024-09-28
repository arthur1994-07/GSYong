#include "../pch.h"
#include "./PointShopCache.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace pointshop
{

PointShopCache::PointShopCache()
{
}

PointShopCache::~PointShopCache()
{
}

void PointShopCache::AddStatistics(const SNATIVEID& Item, __int64 Price)
{
    SELL_STATISTICS_ITER iter = m_Statistics.find(Item);
    if (iter == m_Statistics.end())
    {
        SELL_ITEM_STATISTICS Statistics(Item, Price);
        m_Statistics.insert(SELL_STATISTICS_VALUE(Item, Statistics));
    }
    else
    {
        SELL_ITEM_STATISTICS& Statistics = iter->second;
        Statistics.m_TotalPrice += Price;
        Statistics.m_SellCount++;
    }
}

void PointShopCache::ClearStatistics()
{
    m_Statistics.clear();
}

void PointShopCache::GetStatistics(std::vector<SELL_ITEM_STATISTICS>& Statistics)
{
    for (SELL_STATISTICS_CITER iter=m_Statistics.begin(); iter!=m_Statistics.end(); ++iter)
    {
        const SELL_ITEM_STATISTICS& SellItem = iter->second;
        Statistics.push_back(SellItem);
    }
}

} // namespace pointshop
#ifndef _POINT_SHOP_CACHE_H_
#define _POINT_SHOP_CACHE_H_

#pragma once

#include <map>
#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/PointShop/PointShop.h"

namespace pointshop
{

class PointShopCache : public PointShop
{
public:
    PointShopCache();
    virtual ~PointShopCache();

private:
    typedef std::map<SNATIVEID, SELL_ITEM_STATISTICS>  SELL_STATISTICS;
    typedef SELL_STATISTICS::iterator                  SELL_STATISTICS_ITER;
    typedef SELL_STATISTICS::const_iterator            SELL_STATISTICS_CITER;    
    typedef SELL_STATISTICS::value_type                SELL_STATISTICS_VALUE;

    SELL_STATISTICS m_Statistics; //! 판매된 아이템 통계

public:
    void AddStatistics(const SNATIVEID& Item, __int64 Price);
    void ClearStatistics();
    void GetStatistics(std::vector<SELL_ITEM_STATISTICS>& Statistics);
};

} // namespace pointshop

#endif // _POINT_SHOP_CACHE_H_
#ifndef _POINT_SHOP_AGENT_H_
#define _POINT_SHOP_AGENT_H_

#pragma once

#include "../../RanLogic/PointShop/PointShop.h"

namespace pointshop
{

class PointShopAgent : public PointShop
{
public:
    PointShopAgent();
    virtual ~PointShopAgent();

public:
    const PS_DATA& GetSellData() const { return m_Data; }

};

} // namespace pointshop

#endif // _POINT_SHOP_FIELD_H_
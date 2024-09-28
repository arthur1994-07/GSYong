#include "pch.h"
#include "./PointShop.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace pointshop
{

PointShop::PointShop()
    : m_UpdateDate(0)
{
}

PointShop::~PointShop()
{
}

void PointShop::Clear()
{
    m_UpdateDate = 0;
    m_Data.clear();
}

void PointShop::Insert(const SellItem& Data)
{
    PS_DATA_ITER iter = m_Data.find(Data.ItemId().Id());
    if (iter != m_Data.end())
        m_Data.erase(iter);

    m_Data.insert(PS_DATA_VALUE(Data.ItemId().dwID, Data));
}

bool PointShop::Exist(SNATIVEID ItemId)
{
    PS_DATA_CITER iter = m_Data.find(ItemId.Id());
    if (iter != m_Data.end())
        return true;
    else
        return false;
}

std::tr1::tuple<bool, __int64> PointShop::Price(SNATIVEID ItemId)
{
    PS_DATA_CITER iter = m_Data.find(ItemId.Id());
    if (iter == m_Data.end())
        return std::tr1::make_tuple(false, 0);
    
    const SellItem& Item = iter->second;
    return std::tr1::make_tuple(true, Item.Price());
}

void PointShop::SellList(std::vector<SellItem>& List)
{
    for (PS_DATA_CITER iter=m_Data.begin(); iter!=m_Data.end(); ++iter)
    {
        const SellItem& Item = iter->second;
        List.push_back(Item);
    }
}

} // namespace pointshop
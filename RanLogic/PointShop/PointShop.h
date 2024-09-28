#ifndef _GL_POINT_SHOP_H_
#define _GL_POINT_SHOP_H_

#pragma once

#include <map>
#include <vector>
#include <boost/tr1/tuple.hpp> 
#include "./PointShopDefine.h"

namespace pointshop
{

class PointShop
{
public:
    typedef std::map<DWORD, SellItem> PS_DATA;
    typedef PS_DATA::iterator         PS_DATA_ITER;
    typedef PS_DATA::const_iterator   PS_DATA_CITER;
    typedef PS_DATA::value_type       PS_DATA_VALUE;

public:
    PointShop();
    virtual ~PointShop();

protected:
    __time64_t m_UpdateDate;
    PS_DATA m_Data;

public:
    inline __time64_t UpdateDate() const { return m_UpdateDate; }
    inline void SetUpdateDate(__time64_t Date) { m_UpdateDate=Date; }
    void Clear();

    void Insert(const SellItem& Data);
    bool Exist(SNATIVEID ItemId);
    std::tr1::tuple<bool, __int64> Price(SNATIVEID ItemId);
    void SellList(std::vector<SellItem>& List);
	inline size_t Size() const { return m_Data.size(); }
};

} // namespace pointshop

#endif // _GL_POINT_SHOP_H_
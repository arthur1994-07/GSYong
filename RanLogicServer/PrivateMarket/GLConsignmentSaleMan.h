#pragma once

#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>

#include "../../RanLogic/Market/GLConsignmentSaleDefine.h"

class CacheServer;
struct NET_MSG_GENERIC;

namespace db
{
    class IDbManager;
}

namespace private_market
{

class GLConsignmentSaleMan : private boost::noncopyable
{
public:
    GLConsignmentSaleMan::GLConsignmentSaleMan( CacheServer* pCacheServer );
    void                LoadSellItemDbJob();                      // ������DB���� �ε�

private:
    void                ReqItemList();

private:
    CacheServer*        m_pCacheServer;
    float               m_fCommissionRate;
};

}



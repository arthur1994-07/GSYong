#include "pch.h"

// #include <map>
// #include <boost/timer.hpp>
// 
// #include "../../EngineLib/Common/SUBPATH.h"
// #include "../../EngineLib/ServiceProviderDefine.h"
// 
// #include "../../SigmaCore/String/StringFormat.h"
// #include "../../SigmaCore/String/StringUtil.h"
// #include "../../SigmaCore/Util/MinMessageBox.h"
// #include "../../SigmaCore/Util/SystemInfo.h"
// #include "../../SigmaCore/Log/LogMan.h"
// 
// #include "../../RanLogic/Item/GLItemMan.h"
// #include "../../RanLogic/Market/MarketDefine.h"
// #include "../../RanLogic/Market/GLSaleItem.h"
// #include "../../RanLogic/GLStringTable.h"
// 
// #include "./GLPrivateMarketSearchSQLite.h"
// 
// #include "../../SigmaCore/DebugInclude.h"
// #include "../../SigmaCore/String/StringUtil.h"


#include "GLConsignmentSaleMan.h"


#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Server/CacheServer.h"
#include "../Database/DBAction/DbActionGameConsignmentSale.h"

namespace private_market
{

void GLConsignmentSaleMan::LoadSellItemDbJob()
{
    // 시스템이 닫혀 있다면 검색 시스템에서 아이템을 로드하지 않는다.
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        //sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return;
    }

    ReqItemList();
}


GLConsignmentSaleMan::GLConsignmentSaleMan( CacheServer* pCacheServer )
    : m_pCacheServer( pCacheServer )
{
}

void GLConsignmentSaleMan::ReqItemList()
{

    // 판매 가능 물품을 갖고 온다.
    m_pCacheServer->AddGameAdoJob( 
        db::DbActionPtr( new db::ConsignmentSaleGetListChche( private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee() ) ) );
}

}// namespace private_market
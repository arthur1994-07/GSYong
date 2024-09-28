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
    // �ý����� ���� �ִٸ� �˻� �ý��ۿ��� �������� �ε����� �ʴ´�.
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

    // �Ǹ� ���� ��ǰ�� ���� �´�.
    m_pCacheServer->AddGameAdoJob( 
        db::DbActionPtr( new db::ConsignmentSaleGetListChche( private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee() ) ) );
}

}// namespace private_market
#pragma once
#include "../../SigmaCore/String/MinGuid.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Market/MarketDefine.h"

namespace private_market
{
	enum LOG_STATE
	{
		STATE_NOT_USE,
		STATE_REGIST,
		STATE_RETURN,
		STATE_BUY,
	};

    // 전달용 구조체
    struct ConsignmentSaleItem
    {
        //sc::MGUID           Guid;           // 판매 아이템 고유번호
        std::string         strGuid;
        DWORD               dwChaDbNum;     // 소유자
        std::string         strChaName;     // 소유자 캐릭터명
        __time64_t          ExpirationDate; // 물품 만료시간
        SITEMCUSTOM         sItemcustom;    // 판매 아이템
        WORD                wTurnNum;       // 판매 개수
        WORD                wTotalTurnNum;  // 총 판매 개수
        LONGLONG            llPrice;        // 판매 가격
        int                 SellType;       // 판매형태 private_market::eSellType
		int					ServerGroup;

		ConsignmentSaleItem()
			: dwChaDbNum( 0 )
			, ExpirationDate( 0 )
			, wTurnNum( 0 )
			, wTotalTurnNum( 0 )
			, llPrice( 0 )
			, SellType( 0 )
			, ServerGroup( 0 )
		{
		}

        MSGPACK_DEFINE(strGuid, dwChaDbNum, strChaName, ExpirationDate, sItemcustom, wTurnNum, llPrice, SellType, wTotalTurnNum, ServerGroup );
        sc::MGUID GetGUID()
        {
            sc::MGUID Guid;

            if( strGuid.empty() )
                return Guid;
            
            char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
            StringCchCopy(szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.c_str());
            sc::string::stringToUUID(szGuid, Guid);

            return Guid;
        }
        std::string GetStrGUID()
        {
            //std::string strGuid = sc::string::uuidToString(Guid);
            //sc::string::upperString(strGuid);
            return strGuid;
        }
        void ResetGUID()
        {
            sc::MGUID Guid;
            sc::string::getUUID( Guid );

            strGuid = sc::string::uuidToString(Guid);
            sc::string::upperString(strGuid);
        }
        void SetGUID( sc::MGUID& Guid )
        {
            strGuid = sc::string::uuidToString(Guid);
            sc::string::upperString(strGuid);
        }
        void SetStrGUID( std::string& strGuid )
        {
            this->strGuid = strGuid;
        }
    };

    typedef std::vector<ConsignmentSaleItem> SALE_ITEMS_VECTOR;

}


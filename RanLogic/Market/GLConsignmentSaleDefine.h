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

    // ���޿� ����ü
    struct ConsignmentSaleItem
    {
        //sc::MGUID           Guid;           // �Ǹ� ������ ������ȣ
        std::string         strGuid;
        DWORD               dwChaDbNum;     // ������
        std::string         strChaName;     // ������ ĳ���͸�
        __time64_t          ExpirationDate; // ��ǰ ����ð�
        SITEMCUSTOM         sItemcustom;    // �Ǹ� ������
        WORD                wTurnNum;       // �Ǹ� ����
        WORD                wTotalTurnNum;  // �� �Ǹ� ����
        LONGLONG            llPrice;        // �Ǹ� ����
        int                 SellType;       // �Ǹ����� private_market::eSellType
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


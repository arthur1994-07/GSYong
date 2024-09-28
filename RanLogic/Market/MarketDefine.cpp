#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/gassert.h"

#include "./MarketDefine.h"
#include "../GLogicDataMan.h"
#include "../Item/GLItem.h"
#include "../GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace COMMENT
{
	std::string PRIVATE_MARKET_ITEM_TYPE[private_market::eItemTypeSize] =
	{
		"���",         // 0
		"����",
		"��",
		"��ų����",
		"��ȯ��",

		"����",         // 5
		"��",
		"Ż ��",
		"��ɼ�",
		"�ڽ�Ƭ ����", 

		"�ڽ�Ƭ ��Ÿ",  // 10
		"����",
		"������",
		"ī���",
		"��Ÿ �Ҹ�ǰ", 

        "���ӸӴ�"      // 15
	};

	std::string PrivateMarketItemTypeName( private_market::eItemType Type )
	{
		if ( Type < private_market::eItemTypeSize )
			return PRIVATE_MARKET_ITEM_TYPE[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_WEAPON_TYPE[private_market::eItemWeaponTypeSize] =
	{
		"���",
		"��",
		"��",
		"�ܰ�",
		"â",
		"������",
		"��",
		"Ȱ",
		"��ô",
		"����", 
		"���ϰ�", 
		"��Ż��",
		"���� ��",
		"����Ǿ�",
		"���̵�", 
		"�ذ��۷���", 
		"����������", 
		"��������", 
		"ä��"
	};

	std::string PrivateMarketItemWeaponTypeName( private_market::eItemWeaponType Type )
	{
		if ( Type < private_market::eItemWeaponTypeSize )
			return PRIVATE_MARKET_ITEM_WEAPON_TYPE[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_ARMOR_TYPE[private_market::eItemArmorTypeSize] =
	{
		"���",
		"�Ӹ�",
		"����",
		"����",
		"�尩",
		"�Ź�",
		"�����",
		"����",
		"����",
		"�㸮��",
		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"�Ͱ���", 
		"��ű�",
		*/
		// 20160622 �ٽ� ����
		"�Ͱ���", 
		"��ű�",
	};

	std::string PrivateMarketItemArmorTypeName( private_market::eItemArmorType Type )
	{
		if ( Type < private_market::eItemArmorTypeSize )
			return PRIVATE_MARKET_ITEM_ARMOR_TYPE[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_REQUIRE[private_market::eItemRequireSize] =
	{
		"����",
		"����",
		"��",
		"��ø",
		"����",
	};

	std::string PrivateMarketItemRequireName( private_market::eItemRequire Type )
	{
		if ( Type < private_market::eItemRequireSize )
			return PRIVATE_MARKET_ITEM_REQUIRE[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_RANK[private_market::eItemRankSize] =
	{
		"��� ���",
		"�Ϲ�",
		"������",
		"������",
		"����",
		"�鿭", 
	};

	std::string PrivateMarketItemRankName( private_market::eItemRank Type )
	{
		if ( Type < private_market::eItemRankSize )
			return PRIVATE_MARKET_ITEM_RANK[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_REQUIRE_CLASS[private_market::eItemRequireClassSize] =
	{
		"��� �μ�",
		"������",
		"�˵���",
		"��ú�",
		"�⿹��",
		"�ذ���", 
		"���к�", 
		"�μ���",
		"������",
	};

	std::string PrivateMarketItemRequireClassName( private_market::eItemRequireClass Type )
	{
		if ( Type < private_market::eItemRequireClassSize )
			return PRIVATE_MARKET_ITEM_REQUIRE_CLASS[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_REQUIRE_SEX[private_market::eItemRequireSexSize] =
	{
		"���",
		"����",
		"����",
	};

	std::string PrivateMarketItemRequireSexName( private_market::eItemRequireSex Type )
	{
		if ( Type < private_market::eItemRequireSexSize )
			return PRIVATE_MARKET_ITEM_REQUIRE_SEX[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_BASIC[private_market::eItemBasicSize] =
	{
		"����",
		"������",
		"�ʿ�SP",
		"����",
		"���߷�",
		"ȸ����",
		"���ݿ���",
		"����",
		"����ġ",
		"�������", 
		"����(��)", 
		"����(����)",
		"����(����)",
		"����(��)",
		"����(����)", 
	};

	std::string PrivateMarketItemBasicName( private_market::eItemBasic Type )
	{
		if ( Type < private_market::eItemBasicSize )
			return PRIVATE_MARKET_ITEM_BASIC[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM[private_market::eItemAddonVarRandomSize] =
	{
		"����",
		"���߷�",
		"ȸ����",
		"������",
		"����",
		"ü��(HP)",
		"����",
		"���׹̳�",
		"STATS ��",
		"STATS ü��", 
		"STATS ����", 
		"STATS ��ø",
		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"STATS ����",
		*/
		"STATS �ٷ�",
		"����ġ", 
		"���ġ",
		"���ġ",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������",
		"�̵��ӵ� ������",
		"���ݼӵ� ������",
		"ũ��Ƽ�� �߻�Ȯ��",
		"����Ÿ�� �߻�Ȯ��", 
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������",
		"�̵��ӵ� ������",
		"���ݼӵ� ������",
		"ũ��Ƽ�� �߻���",
		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"����Ÿ�� �߻���", 
		*/
		"�����̻�",
	};

	std::string PrivateMarketItemAddonVarRandomName( private_market::eItemAddonVarRandom Type )
	{
		if ( Type < private_market::eItemAddonVarRandomSize )
			return PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM[Type];
		else
			return std::string("");
	}

	std::string PRIVATE_MARKET_ITEM_STATE_BLOW[private_market::eItemStateBlowSize] =
	{
		"����",
		"����",
		"����",
		"��ȭ",
		"ȭ��",
		"�õ�", 
		"����", 
		"�ߵ�",
		"����", 
	};

	std::string PrivateMarketItemStateBlowName( private_market::eItemStateBlowType Type )
	{
		if ( Type < private_market::eItemStateBlowSize )
			return PRIVATE_MARKET_ITEM_STATE_BLOW[Type];
		else
			return std::string("");
	}

}

private_market::GLPrivateMarketPurchseQueue::GLPrivateMarketPurchseQueue()
: m_bPurchase( false )
{

}

private_market::GLPrivateMarketPurchseQueue::~GLPrivateMarketPurchseQueue()
{

}

bool private_market::GLPrivateMarketPurchseQueue::Push( sPurchaseQueue& sPurchase )
{
	m_theContainer.push( sPurchase );
	return true;
}

bool private_market::GLPrivateMarketPurchseQueue::Push( UUID sItemGUID, DWORD dwItemTurnNum, LONGLONG llPrice )
{
	sPurchaseQueue sPurchase;
	sPurchase.sItemGUID = sItemGUID;
	sPurchase.dwItemTurnNum = dwItemTurnNum;
	sPurchase.llPrice = llPrice;
	m_theContainer.push( sPurchase );
	return true;
}

bool private_market::GLPrivateMarketPurchseQueue::Push( std::vector< sPurchaseQueue >& vecPurchase )
{
	size_t nPurchaseSize = vecPurchase.size();
	for ( size_t loop = 0; loop < nPurchaseSize; ++loop )
	{
		m_theContainer.push( vecPurchase[loop] );
	}
	return true;
}

bool private_market::GLPrivateMarketPurchseQueue::Pop()
{
	if ( false == m_theContainer.empty() )
	{
		m_theContainer.pop();
	}

	if ( m_theContainer.empty() )
	{
		m_bPurchase = false;
		return true;
	}

	return false;
}

private_market::sPurchaseQueue& private_market::GLPrivateMarketPurchseQueue::Front()
{
	if ( m_theContainer.empty() )
	{
		m_bPurchase = false;
		static sPurchaseQueue dummy;
		return dummy;
	}
	return m_theContainer.front();
}

void private_market::GLPrivateMarketPurchseQueue::Reset()
{
	m_bPurchase = false;

	while ( !m_theContainer.empty() )
	{
		m_theContainer.pop();
	}
}

private_market::GLPrivateMarketDefine::GLPrivateMarketDefine()
    : m_dwSearchDelayMillisecond( 3000 )
    , m_dwLogDelayMillisecond( 5000 )
    , m_wSearchItemNumberPerPage( 8 )
    , m_wLogNumberPerPage( 10 )
    , m_wLogNumberMaxPerCharacter( 80 )
    , m_bCheckChannel( true )
    , m_bCheckSchool( false )
    , m_bSaleCommission( true )
    , m_bAllPage( false )
    // ��Ź�Ǹ� ( ����Ʈ �ŷ� )
    , m_wSellResgisterBasicNum( 3 )
    , m_wSellResgisterExpandAddNum( 0 )
    , m_wTradePossibleRegisterFeeAdd( 100 )
    , m_fSalesCompletionFee(1)
	, m_nSearchOption( 0 )
{
}

private_market::GLPrivateMarketDefine::~GLPrivateMarketDefine()
{

}

bool private_market::GLPrivateMarketDefine::Load( const std::string& strFileName, bool bServer )
{
	m_strFilePath = strFileName;

	try
	{   
		sc::lua_init();
		if ( !sc::lua_doFile( m_strFilePath ) )
		{
			std::string strErrMsg(
				sc::string::format(
				"%1% file missing.(error 1246)",
				m_strFilePath));
			sc::ErrorMessageBox( strErrMsg, false );
			return false;
		}

		loadCommon();
		loadUI();

		if ( bServer )
		{
			loadServer();
		}

        if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
            loadConsigmentSale();

		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg(
			sc::string::format(
			"%1%, %2%",
			m_strFilePath,
			e.GetErrorMessage()));
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	} 
}

bool private_market::GLPrivateMarketDefine::loadCommon()
{
	try
	{        
		LuaPlus::LuaObject common = sc::lua_getGlobalFromName( "common" );
		for (LuaPlus::LuaTableIterator Iter( common ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "SearchDelay" )
				m_dwSearchDelayMillisecond = static_cast< DWORD >( Iter.GetValue().GetFloat() * 1000 );
			else if ( strKey == "LogDelay" )
				m_dwLogDelayMillisecond = static_cast< DWORD >( Iter.GetValue().GetFloat() * 1000 );
			else if ( strKey == "SearchItemNumberPerPage" )
				m_wSearchItemNumberPerPage = Iter.GetValue().GetInteger();
			else if ( strKey == "LogNumberPerPage" )
				m_wLogNumberPerPage = Iter.GetValue().GetInteger();
			else if ( strKey == "Map" )
			{
				m_vecPurchaseMap.clear();

				for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
				{
					for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
					{
						SNATIVEID sMapID;
						sMapID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
						sMapID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() );

						m_vecPurchaseMap.push_back( sMapID.dwID );

						/*std::string strMessage( 
							sc::string::format( 
							"PrivateMarket purchase map mid: %1% sid: %2%", 
							sMapID.wMainID, sMapID.wSubID ) );
						sc::writeLogInfo( strMessage );*/
					}
				}
			}
		}
		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg(
			sc::string::format(
			"%1%, %2%",
			m_strFilePath,
			e.GetErrorMessage()));
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	}
}

bool private_market::GLPrivateMarketDefine::loadServer()
{
	try
	{        
		LuaPlus::LuaObject server = sc::lua_getGlobalFromName( "server" );
		for (LuaPlus::LuaTableIterator Iter( server ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "CheckChannel" )
				m_bCheckChannel = Iter.GetValue().GetBoolean();
			else if ( strKey == "CheckSchool" )
				m_bCheckSchool = Iter.GetValue().GetBoolean();
			else if ( strKey == "SaleCommission" )
				m_bSaleCommission = Iter.GetValue().GetBoolean();
			else if ( strKey == "AllPage" )
				m_bAllPage = Iter.GetValue().GetBoolean();
			else if ( strKey == "LogNumberMaxPerCharacter" )
				m_wLogNumberMaxPerCharacter = Iter.GetValue().GetInteger();
			else if ( strKey == "SearchOption" )
				m_nSearchOption = Iter.GetValue().GetInteger();
		}
		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg(
			sc::string::format(
			"%1%, %2%",
			m_strFilePath,
			e.GetErrorMessage()));
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	}
}

bool private_market::GLPrivateMarketDefine::loadUI()
{
	return true;
}

bool private_market::GLPrivateMarketDefine::loadConsigmentSale()
{
    try
    {        
        LuaPlus::LuaObject consignmentSale = sc::lua_getGlobalFromName( "ConsignmentSale" );
        for (LuaPlus::LuaTableIterator Iter( consignmentSale ); Iter; Iter.Next() )
        {
            std::string strKey( ( const char* ) Iter.GetKey().GetString() );

            if ( strKey == "SellResgisterBasicNum" )
            {
                m_wSellResgisterBasicNum = Iter.GetValue().GetInteger();
            }
            else if ( strKey == "SellResgisterExpandAddNum" )
            {
                m_wSellResgisterExpandAddNum = Iter.GetValue().GetInteger();
            }
            else if ( strKey == "TradePossibleRegisterFeeAdd" )
            {
                m_wTradePossibleRegisterFeeAdd = Iter.GetValue().GetInteger();
            }
            else if ( strKey == "SalesCompletionFee" )
            {
                m_fSalesCompletionFee = Iter.GetValue().GetFloat();
            }
            else if ( strKey == "SellGameMoneySetting" )
            {
                m_vecSellGameMoneyItemFee.clear();

                for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
                {
                    for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
                    {
                        GameMoneyItemFee MoneyFee;

                        MoneyFee.sNativeID.wMainID  = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
                        MoneyFee.sNativeID.wSubID   = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
                        MoneyFee.wFee               = static_cast< WORD >( Iter3.GetValue().GetInteger() );

                        m_vecSellGameMoneyItemFee.push_back( MoneyFee );
                    }
                }
            }
			else if ( strKey == "SellPeriodAndFee" )
			{
				m_vecSellPeriodAndFee.clear();

				for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
				{
					for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
					{
						PeriodFee periodFee;
						periodFee.wPeriod = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
						periodFee.wFee = static_cast< WORD >( Iter3.GetValue().GetInteger() );

						m_vecSellPeriodAndFee.push_back( periodFee );
					}
				}
			}
            else if ( strKey == "ItemGradeRegisterFeeAdd" )
            {
                m_vecItemGradeRegisterFeeAdd.clear();

                for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
                {
                    WORD wFee;
                    wFee = static_cast< WORD >( Iter2.GetValue().GetInteger() );
                    m_vecItemGradeRegisterFeeAdd.push_back( wFee );
                }
            }
            else if ( strKey == "RanPointSaleMinPrice" )
            {
                m_wRanPointSaleMinPrice = Iter.GetValue().GetInteger();
                
            }
			else if ( strKey == "RanGoldRegist" )
			{
				m_bGoldTrade = Iter.GetValue().GetBoolean();

			}
        }
        return true;
    }
    catch ( const LuaPlus::LuaException& e )
    {
        std::string strErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_strFilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox( strErrMsg, false );
        return false;
    }
}

bool private_market::GLPrivateMarketDefine::CanPurchase( SNATIVEID sMapID )
{
	return CanPurchase( sMapID.dwID );
}

bool private_market::GLPrivateMarketDefine::CanPurchase( DWORD dwMapID )
{
	std::vector< DWORD >::iterator it = std::find( m_vecPurchaseMap.begin(), m_vecPurchaseMap.end(), dwMapID );
	return it == m_vecPurchaseMap.end() ? false : true;
}

void private_market::GLPrivateMarketDefine::LoadXmlTextData()
{

	for ( int i = 0; i < private_market::eItemTypeSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_TYPE[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_TYPE", i );
	}
	for ( int i = 0; i < private_market::eItemWeaponTypeSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_WEAPON_TYPE[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_WEAPON_TYPE", i);
	}

	for ( int i = 0; i < private_market::eItemArmorTypeSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_ARMOR_TYPE[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_ARMOR_TYPE", i );
	}

	for ( int i = 0; i < private_market::eItemRequireSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_REQUIRE[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_REQUIRE", i);
	}

	for ( int i = 0; i < private_market::eItemRankSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_RANK[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_RANK", i);
	}

	for ( int i = 0; i < private_market::eItemRequireClassSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_REQUIRE_CLASS[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", i);
	}

	for ( int i = 0; i < private_market::eItemRequireSexSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_REQUIRE_SEX[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_REQUIRE_SEX", i);
	}

	for ( int i = 0; i < private_market::eItemBasicSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_BASIC[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_BASIC", i );
	}

	for ( int i = 0; i < private_market::eItemAddonVarRandomSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", i);
	}

	for ( int i = 0; i < private_market::eItemStateBlowSize; i++ )
	{
		COMMENT::PRIVATE_MARKET_ITEM_STATE_BLOW[i] = ID2GAMEWORD("PRIVATE_MARKET_ITEM_STATE_BLOW", i );
	}
}

WORD private_market::GLPrivateMarketDefine::GradeRegisterFeeAdd( EMITEMLEVEL ItemGrade )
{
    int nGrade = static_cast<int>( ItemGrade );

    GASSERT( EMITEM_LEVEL_NSIZE == m_vecItemGradeRegisterFeeAdd.size()
        && "PrivateMarket.lua������ ItemGradeRegisterFeeAdd �׸��� EMITEMLEVEL�� ��ġ���� �ʽ��ϴ�." );

    return m_vecItemGradeRegisterFeeAdd[nGrade];
}


DWORD private_market::GLPrivateMarketDefine::CalcFee( int nTimeTableIndex, const SITEMCUSTOM& sItemCustom )
{
    // ������ ���
    DWORD dwFee = 0;

    // ������ Ȯ��
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
    if ( !pItemData )
        return 0;

    // ������Ʈ �ŷ��� �Ҽ� �ִ� �����ΰ�?
    //if( pItemData->CanExchange() == false )
	if ( ( sItemCustom.IsExchange() != NULL ) == false )
        if( pItemData->sBasicOp.RanPointTradeCount() <= sItemCustom.wRanPointTradeCount )
		return 0;

    // �ŷ� �Ⱓ�� ���� �⺻ �����Ḧ �ΰ��Ѵ�.
    std::vector<private_market::PeriodFee> vecPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();
    if( 0 <= nTimeTableIndex && nTimeTableIndex < static_cast<int>(vecPeriodFee.size()) )
        dwFee += vecPeriodFee[nTimeTableIndex].wFee;
    else 
        return 0;


    std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
    vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();


    bool bIsGameMoney = false;
    DWORD dwGameMoneyFee = 0;
    for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

        if( value.sNativeID == sItemCustom.GetNativeID() )
        {
            bIsGameMoney = true;
            dwGameMoneyFee = value.wFee;
            break;
        }
    }

    // �⺻������ ������ ��޿� ���� �����Ḧ �ΰ��Ѵ�.
    // ���ӸӴ��ΰ�� �����Ḧ �ٸ��� �ΰ��Ѵ�.
    if( bIsGameMoney )
        dwFee += dwGameMoneyFee;
    else
        dwFee += private_market::GLPrivateMarketDefine::Instance()->GradeRegisterFeeAdd( pItemData->sBasicOp.emLevel );


    // �ŷ� �Ұ����� ��� �߰� �����Ḧ �ΰ��Ѵ�.
    //if( pItemData->CanExchange() == false )
    if ( ( sItemCustom.IsExchange() != NULL ) == false )
        dwFee += private_market::GLPrivateMarketDefine::Instance()->TradePossibleRegisterFeeAdd();


    return dwFee;
}

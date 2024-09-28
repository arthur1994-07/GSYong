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
		"모두",         // 0
		"무기",
		"방어구",
		"스킬문서",
		"귀환서",

		"연마",         // 5
		"펫",
		"탈 것",
		"기능성",
		"코스튬 복장", 

		"코스튬 기타",  // 10
		"상자",
		"문서류",
		"카드류",
		"기타 소모품", 

        "게임머니"      // 15
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
		"모두",
		"검",
		"도",
		"단검",
		"창",
		"몽둥이",
		"권",
		"활",
		"투척",
		"권총", 
		"레일건", 
		"포탈건",
		"갈고리 낫",
		"듀얼스피어",
		"사이드", 
		"극강글러브", 
		"마술지팡이", 
		"마술상자", 
		"채찍"
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
		"모두",
		"머리",
		"상의",
		"하의",
		"장갑",
		"신발",
		"목걸이",
		"팔찌",
		"반지",
		"허리띠",
		/*
		// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"귀걸이", 
		"장신구",
		*/
		// 20160622 다시 해제
		"귀걸이", 
		"장신구",
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
		"없음",
		"레벨",
		"힘",
		"민첩",
		"정신",
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
		"모든 등급",
		"일반",
		"진귀한",
		"유일한",
		"무한",
		"백열", 
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
		"모든 부서",
		"격투부",
		"검도부",
		"양궁부",
		"기예부",
		"극강부", 
		"과학부", 
		"인술부",
		"마술부",
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
		"모두",
		"남자",
		"여자",
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
		"없음",
		"데미지",
		"필요SP",
		"방어력",
		"명중률",
		"회피율",
		"공격연마",
		"방어연마",
		"경험치",
		"모든저항", 
		"저항(불)", 
		"저항(얼음)",
		"저항(전기)",
		"저항(독)",
		"저항(정기)", 
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
		"없음",
		"명중률",
		"회피율",
		"데미지",
		"방어력",
		"체력(HP)",
		"마나",
		"스테미나",
		"STATS 힘",
		"STATS 체력", 
		"STATS 정신", 
		"STATS 민첩",
		/*
		// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"STATS 지력",
		*/
		"STATS 근력",
		"격투치", 
		"사격치",
		"기력치",
		"HP 증가율",
		"MP 증가율",
		"SP 증가율",
		"HP+MP+SP 증가율",
		"이동속도 증가율",
		"공격속도 증가율",
		"크리티컬 발생확율",
		"강한타격 발생확율", 
		"HP 증가량",
		"MP 증가량",
		"SP 증가량",
		"HP+MP+SP 증가량",
		"이동속도 증가량",
		"공격속도 증가량",
		"크리티컬 발생량",
		/*
		// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
		// http://crm.mincoms.com/Ticket/TicketView/1056
		"강한타격 발생량", 
		*/
		"상태이상",
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
		"없음",
		"마비",
		"기절",
		"석화",
		"화염",
		"냉동", 
		"착란", 
		"중독",
		"저주", 
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
    // 위탁판매 ( 포인트 거래 )
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
        && "PrivateMarket.lua파일의 ItemGradeRegisterFeeAdd 항목이 EMITEMLEVEL와 일치하지 않습니다." );

    return m_vecItemGradeRegisterFeeAdd[nGrade];
}


DWORD private_market::GLPrivateMarketDefine::CalcFee( int nTimeTableIndex, const SITEMCUSTOM& sItemCustom )
{
    // 수수료 계산
    DWORD dwFee = 0;

    // 아이템 확인
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
    if ( !pItemData )
        return 0;

    // 란포인트 거래를 할수 있는 상태인가?
    //if( pItemData->CanExchange() == false )
	if ( ( sItemCustom.IsExchange() != NULL ) == false )
        if( pItemData->sBasicOp.RanPointTradeCount() <= sItemCustom.wRanPointTradeCount )
		return 0;

    // 거래 기간에 따라 기본 수수료를 부가한다.
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

    // 기본적으로 아이템 등급에 따라 수수료를 부가한다.
    // 게임머니인경우 수수료를 다르게 부가한다.
    if( bIsGameMoney )
        dwFee += dwGameMoneyFee;
    else
        dwFee += private_market::GLPrivateMarketDefine::Instance()->GradeRegisterFeeAdd( pItemData->sBasicOp.emLevel );


    // 거래 불가능인 경우 추가 수수료를 부가한다.
    //if( pItemData->CanExchange() == false )
    if ( ( sItemCustom.IsExchange() != NULL ) == false )
        dwFee += private_market::GLPrivateMarketDefine::Instance()->TradePossibleRegisterFeeAdd();


    return dwFee;
}

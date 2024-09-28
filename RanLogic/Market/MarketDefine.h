#pragma once

#include <vector>

#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../../SigmaCore/Util/GLSingletone.h"

#include "../Item/GLItemCustom.h"

struct SSALEITEM
{
	SNATIVEID	sSALEPOS;
	SNATIVEID	sINVENPOS;
	SITEMCUSTOM	sITEMCUSTOM;
	LONGLONG	llPRICE;
	DWORD		dwNUMBER;
	bool		bSOLD;
	UUID		sGUID;

	SSALEITEM () :
	sINVENPOS(false),
		llPRICE(0),
		dwNUMBER(1),
		bSOLD(false)
	{
	}
};

struct SFINDRESULT
{
	SNATIVEID	sSaleItemID;
	LONGLONG	llPRICE;

	SFINDRESULT () :
	sSaleItemID(false),
		llPRICE(0)
	{
	}
};


typedef std::map<DWORD, SSEARCHITEMDATA> MAPSEARCH;  //<�����۾��̵�,����������>
typedef MAPSEARCH::iterator				 MAPSEARCH_ITER;
typedef MAPSEARCH::const_iterator		 MAPSEARCH_CITER;
typedef MAPSEARCH::value_type            MAPSEARCH_VALUE;

const LONGLONG LL_LIMITED_A_UNIT_PRICE = 99999999999;
const WORD W_LIMITED_POINT_TRADE_COUNT = 9999;

namespace private_market
{
	enum eCommonError
	{
		eSuccess = 300, 
		eStringEmpty, 
		eWrongItem, 
		eNullTablePointer, 
		eNotExistSeller, 
		eNotExistSaleItem, 
		eItemLack, 
		eHoldItemInsertFail, 
		eIsHoldItem, 
		eNotExistBuyer, 
		eMoneyLack, 
		eHoldItemDeleteFail, 
		eNotExistHoldItem, 
		eSaleItemDeleteFail, 
		eSaleItemUpdateFail, 
		eSaleItemNULL, 
		eSaleLogInsertFail, 
		eRollbackMoney, 
		eAlreadyBuy, 
		eSearchDelay, 
		eNotExistSaleLog, 
		eNotExistItemCache, 
		eItemCacheInsertFail, 
		eItemReceiving, 
		eItemCached, 
		eItemNotCached,
		eItemNotCachedPage, 
		eCantBuyMyItem, 
		eItemCacheDeleteFail, 
		eNotPurchaseMap,
        eRanPointLack,
	};

	// �����۰��� define ���� ����Ǹ� ���⵵ ���� ������ ����ؾ� �Ѵ�.

	// ������ ����
	enum eItemType
	{
		eItemTypeAll, 
		eItemTypeWeapon, 
		eItemTypeArmor,
		eItemTypeSkillBook, 
		eItemTypeRecall, 

		eItemTypeGrinding, 
		eItemTypePet, 
		eItemTypeVehicle, 
		eItemTypeFunctional, 
		eItemTypeCostume, 

		eItemTypeCostumeExtra, 
		eItemTypeBox, 
		eItemTypeDocument, 
		eItemTypeCard, 
		eItemTypeConsumeExtra, 

        eItemTypeGameMoney,   // �� ���� �Ӵ�

		eItemTypeSize, 
	};

	// ������ ���� ����
	enum eItemWeaponType
	{
		eItemWeaponTypeAll,				// ���
		eItemWeaponTypeSword,			// ��
		eItemWeaponTypeSaber,			// ��
		eItemWeaponTypeDagger,			// �ܰ�
		eItemWeaponTypeSpear,			// â
		eItemWeaponTypeStick,			// ������
		eItemWeaponTypeGwon,			// ��
		eItemWeaponTypeBow,				// Ȱ
		eItemWeaponTypeThrow,			// ��ô
		eItemWeaponTypePistol,			// ����
		eItemWeaponTypeRailgun,			// ���ϰ�
		eItemWeaponTypePortalgun,		// ��Ż��
		eItemWeaponTypeSide,			// ���� ��
		eItemWeaponTypeDualspear,		// ��� ���Ǿ�
		eItemWeaponTypeThrowingKnife,	// ������
		eItemWeaponTypeExtremeGlove,	// �ذ��۷���
		eItemWeaponTypeTrickStick,		// ����������
		eItemWeaponTypeTrickBox,		// ��������
		eItemWeaponTypeTrickWhip,		// ä��
		eItemWeaponTypeActorShield,		// ���� ����
		eItemWeaponTypeActorHammer,		// ���� �ظ�
		eItemWeaponTypeActorUmbrella,	// ���� ���
		eItemWeaponTypeWeaponStr,		// �� ����
		eItemWeaponTypeWeaponDex,		// ��ø ����
		eItemWeaponTypeWeaponSpi,		// ���� ����
		eItemWeaponTypeSize,
	};

	// ������ ���� ��
	enum eItemArmorType
	{
		eItemArmorTypeAll,				// ���
		eItemArmorTypeHeadgear,			// �Ӹ�
		eItemArmorTypeUpper,			// ����
		eItemArmorTypeLower,			// ����
		eItemArmorTypeHand,				// �尩
		eItemArmorTypeFoot,				// �Ź�
		eItemArmorTypeNeck,				// �����
		eItemArmorTypeWrist,			// ����
		eItemArmorTypeFinger,			// ����
		eItemArmorTypeWaistband,		// �㸮��
		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		eItemArmorTypeEarring,			// �Ͱ���
		eItemArmorTypeAccessories,		// ��ű�
		*/

		// 20160622 �ٽ� ����
		eItemArmorTypeEarring,			// �Ͱ���
		eItemArmorTypeAccessories,		// ��ű�

		eItemArmorTypeSize, 
	};

	enum eItemRequire
	{
		eItemRequireNone, 
		eItemRequireLevel, 
		eItemRequirePow, 
		eItemRequireDex, 
		eItemRequireSpi, 

		eItemRequireSize, 
	};

	enum eItemRank
	{
		eItemRankAll, 
		eItemRankNomal, 
		eItemRankHigh, 
		eItemRankRare, 
		eItemRankUnique, 
		eItemRankEpic,
		eItemRankLegendary,

		eItemRankSize, 
	};

	enum eItemRequireClass
	{
		eItemRequireClassAll, 
		eItemRequireClassFighter, 
		eItemRequireClassArms, 
		eItemRequireClassArcher, 
		eItemRequireClassSpirit, 
		eItemRequireClassExtreme, 
		eItemRequireClassScientist, 
		eItemRequireClassAssassin, 
		eItemRequireClassTricker,
		eItemRequireClassActor,

		eItemRequireClassSize, 
	};

	enum eItemRequireSex
	{
		eItemRequireSexAll, 
		eItemRequireSexMan, 
		eItemRequireSexWoman, 

		eItemRequireSexSize, 
	};

	enum eItemBasic
	{
		eItemBasicNone, 
		eItemBasicDamage, 
		eItemBasicRequireSP, 
		eItemBasicDefence, 
		eItemBasicHitrate, 
		eItemBasicAvoidrate, 
		eItemBasicGrindingDamage, 
		eItemBasicGrindingDefence, 
		eItemBasicExpMultiple, 
		eItemBasicAllResist, 
		eItemBasicFireResist, 
		eItemBasicIceResist, 
		eItemBasicElecResist, 
		eItemBasicPoisonResist, 
		eItemBasicSpiritResist, 

		eItemBasicSize, 
	};

	enum eItemAddonVarRandom
	{
		eItemAddonVarRandomNone, 
		eItemAddonRandomHitrate, 
		eItemAddonRandomAvoidrate, 
		eItemAddonRandomDamage, 
		eItemAddonRandomDefence, 
		eItemAddonRandomHp, 
		eItemAddonRandomMp, 
		eItemAddonRandomSp, 
		eItemAddonRandomStatsPow, 
		eItemAddonRandomStatsStr, 
		eItemAddonRandomStatsSpi, 
		eItemAddonRandomStatsDex,
		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		eItemAddonRandomStatsInt, 
		*/
		eItemAddonRandomStatsSta, 
		eItemAddonRandomPA, 
		eItemAddonRandomSA, 
		eItemAddonRandomMA, 

		eItemVarRandomHprate, 
		eItemVarRandomMprate, 
		eItemVarRandomSprate, 
		eItemVarRandomHpMpSprate, 
		eItemVarRandomMoveSpeedrate, 

		eItemVarAttackSpeedrate, 
		eItemVarCriticalrate, 
		eItemVarCrushingrate, 

		eItemVarRandomHp, 
		eItemVarRandomMp, 
		eItemVarRandomSp, 
		eItemVarRandomHpMpSp, 
		eItemVarRandomMoveSpeed, 

		eItemVarAttackSpeed, 
		eItemVarCritical,

		/*
		// #CRM 1059 ���λ��� �ý��� ���� Ÿ�� ���� ��û
		// http://crm.mincoms.com/Ticket/TicketView/1056
		eItemVarCrushing, 
		*/

		eItemStateBlow, 

		eItemAddonVarRandomSize, 
	};

	enum eItemStateBlowType
	{
		eItemStateBlowNone,		// ����.
		eItemStateBlowNumb,		// ����.
		eItemStateBlowStun,		// ����.
		eItemStateBlowStone,	// ��ȭ.
		eItemStateBlowBurn,		// ȭ��.
		eItemStateBlowFrozen,	// �õ�.
		eItemStateBlowMad,		// ����.
		eItemStateBlowPoison,	// �ߵ�.
		eItemStateBlowCurse,	// ����.

		eItemStateBlowSize,
	};

	enum eItemSortOption
	{
		eItemSortOptionName, 
		eItemSortOptionPrice, 
	};

	enum eItemSortMethod
	{
		eItemSortMethodAsc,		// ���� �� ����
		eItemSortMethodDesc,	// ���� �� ����
	};

	enum eCommandOptionType
	{
		eCommandOptionTypeSearchDelay, 
		eCommandOptionTypeLogDelay, 
		eCommandOptionTypeCheckChannel, 
		eCommandOptionTypeCheckSchool, 
		eCommandOptionTypeCommission, 
		eCommandOptionTypeAllPage, 
		eCommandOptionTypeSearchOption, 
	};

    enum eSellType
    {
        eSellType_Error = 0,
        eSellType_PrivateMarket,            // ��Ź�Ǹ� ��� ����  
        eSellType_ConsignmentSale_RanPoint, // ������Ʈ �ŷ�
        eSellType_ConsignmentSale_Gold,     // ��� �ŷ�

        eSellType_ConsignmentSale_END, 
    };

    enum eConsignmentSaleAddError
    {
        eConsignmentSaleAdd_Error = 0,
        eConsignmentSaleAdd_Fail,           // ���� ���� �޽���
//         eConsignmentSaleAdd_
//         eConsignmentSaleAdd_
//         eConsignmentSaleAdd_

    };

	enum eSearchMoneyType
	{
		eSearchMoneyType_ALL= 0,		// ALL
		eSearchMoneyType_Gold,			// ���
		eSearchMoneyType_RanPoint,		// ������Ʈ
		eSearchMoneyTypeSize,			

	};

	struct sSaleItemSearchOptionNumeric
	{
		int nType;
		int nValue;

		MSGPACK_DEFINE( nType, nValue );

		sSaleItemSearchOptionNumeric()
			: nType( 0 )
			, nValue( 0 )
		{
		}

		bool operator == ( const sSaleItemSearchOptionNumeric& rvalue ) const
		{
			if ( 
				nType == rvalue.nType && 
				nValue == rvalue.nValue )
				return true;

			return false;
		}
	};

	struct sSaleItemSearchOptionFloat
	{
		int nType;
		float fValue;

		MSGPACK_DEFINE( nType, fValue );

		sSaleItemSearchOptionFloat()
			: nType( 0 )
			, fValue( 0.0f )
		{
		}

		bool operator == ( const sSaleItemSearchOptionFloat& rvalue ) const
		{
			if ( 
				nType == rvalue.nType && 
				fValue == rvalue.fValue )
				return true;

			return false;
		}
	};

	struct sSaleItemSearchandSortBasic
	{
		int nType;
		int nWeaponType;
		int nArmorType;

		int nRequire;
		int nRequireValue;

		int nRequireClass;
		int nRequireSex;

		int nRank;

		bool bTimelimit;

		std::vector< sSaleItemSearchOptionFloat > vecBasic;
		std::vector< sSaleItemSearchOptionFloat > vecAddonVarRandom;

		int nSortOption;
		int nSortMethod;

		DWORD dwItemTurnNum;

		WORD wPage;

		int nSearchSellType;	// eSearchMoneyType	// �Ǹ�Ÿ��

		std::string strItemName;


		MSGPACK_DEFINE( 
			nType, 
			nWeaponType, 
			nArmorType, 
			nRequire, 
			nRequireValue, 
			nRequireClass, 
			nRequireSex, 
			nRank, 
			bTimelimit, 
			vecBasic, 
			vecAddonVarRandom, 
			nSortOption, 
			nSortMethod, 
			dwItemTurnNum, 
			wPage,
			nSearchSellType,
			strItemName);

		sSaleItemSearchandSortBasic()
			: nType( eItemTypeAll )
			, nWeaponType( eItemWeaponTypeAll )
			, nArmorType( eItemArmorTypeAll )
			, nRequire( eItemRequireNone )
			, nRequireValue( 0 )
			, nRequireClass( eItemRequireClassAll )
			, nRequireSex( eItemRequireSexAll )
			, nRank( eItemRankAll )
			, bTimelimit( false )
			, nSortOption( eItemSortOptionName )
			, nSortMethod( eItemSortMethodAsc )
			, dwItemTurnNum( 0 )
			, wPage( 1 )
			, nSearchSellType(eSearchMoneyType_ALL)
		{
			strItemName.clear();
		}

		bool operator == ( const sSaleItemSearchandSortBasic& rvalue ) const
		{
			if ( 
				nType == rvalue.nType && 
				nWeaponType == rvalue.nWeaponType && 
				nArmorType == rvalue.nArmorType && 
				nRequire == rvalue.nRequire && 
				nRequireValue == rvalue.nRequireValue && 
				nRequireClass == rvalue.nRequireClass && 
				nRequireSex == rvalue.nRequireSex && 
				nRank == rvalue.nRank && 
				bTimelimit == rvalue.bTimelimit && 
				vecBasic == rvalue.vecBasic && 
				vecAddonVarRandom == rvalue.vecAddonVarRandom && 
				nSortOption == rvalue.nSortOption && 
				nSortMethod == rvalue.nSortMethod && 
				dwItemTurnNum == rvalue.dwItemTurnNum &&
				nSearchSellType == rvalue.nSearchSellType &&
				strItemName == rvalue.strItemName )
				return true;

			return false;
		}

		bool operator != ( const sSaleItemSearchandSortBasic& rvalue ) const
		{
			return !( *this == rvalue );
		}
	};

	struct sSaleItemSearchandSortName
	{
		std::string strItemName;
		int nSortOption;
		int nSortMethod;
		WORD wPage;

		sSaleItemSearchandSortName()
			: nSortOption( eItemSortOptionName )
			, nSortMethod( eItemSortMethodAsc )
			, wPage( 1 )
		{
		}

		bool operator == ( const sSaleItemSearchandSortName& rvalue ) const
		{
			if ( 
				strItemName == rvalue.strItemName && 
				nSortOption == rvalue.nSortOption && 
				nSortMethod == rvalue.nSortMethod  )
				return true;

			return false;
		}

		bool operator != ( const sSaleItemSearchandSortName& rvalue ) const
		{
			return !( *this == rvalue );
		}
	};

	struct sSaleItemSearchandSort
	{
		sSaleItemSearchandSortBasic sBasic;

		int nChannel;
		WORD wSchool;

		MSGPACK_DEFINE( sBasic, nChannel, wSchool );

		sSaleItemSearchandSort()
			: nChannel( 0 )
			, wSchool( 0 )
		{
		}
	};

	struct sSaleItem 
	{
		DWORD dwSellerChaDbNum;
		DWORD dwItemTurnNum;
		LONGLONG llItemPrice;
		UUID sItemGUID;
		SITEMCUSTOM sItemcustom;
		SNATIVEID sSellerMapID;
		char szSellerName[CHAR_SZNAME];
        int nSellType;
        __time64_t ExpirationDate;

		sSaleItem()
			: dwSellerChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, llItemPrice( 0 )
			, sSellerMapID( false )
            , nSellType( 0 )
            , ExpirationDate( 0 )
		{
		}

		sSaleItem( DWORD dwChaDbNum, DWORD dwTurnNum, LONGLONG llPrice, UUID& sGUID, SITEMCUSTOM& sItem, SNATIVEID sMapID, const char* szChaName, int nSellType_, __time64_t ExpirationDate_ )
		{
			dwSellerChaDbNum = dwChaDbNum;
			dwItemTurnNum = dwTurnNum;
			llItemPrice = llPrice;
			sItemGUID = sGUID;
			sItemcustom = sItem;
			sSellerMapID = sMapID;
            nSellType = nSellType_;
            ExpirationDate = ExpirationDate_;
			if ( szChaName )
				StringCchCopy( szSellerName, CHAR_SZNAME, szChaName );
		}

		sSaleItem( const sSaleItem& sItem )
		{
			dwSellerChaDbNum = sItem.dwSellerChaDbNum;
			dwItemTurnNum = sItem.dwItemTurnNum;
			llItemPrice = sItem.llItemPrice;
			sItemGUID = sItem.sItemGUID;
			sItemcustom = sItem.sItemcustom;
			sSellerMapID = sItem.sSellerMapID;
            nSellType = sItem.nSellType;
            ExpirationDate = sItem.ExpirationDate;
			if ( sItem.szSellerName )
				StringCchCopy( szSellerName, CHAR_SZNAME, sItem.szSellerName );
		}

		void SetName( const char* szName )
		{
			if ( szName )
				StringCchCopy( szSellerName, CHAR_SZNAME, szName );
		}
	};

	struct sHoldItem
	{
		DWORD dwSellerChaDbNum;
		DWORD dwItemTurnNum;
		LONGLONG llItemPrice;
		int nSellerChannel;
		DWORD dwSellerFieldSlot;
		int nSellerServerGroup;
		UUID sItemGUID;
		char szName[CHAR_SZNAME];
		DWORD dwBuyerChaDbNum;
		DWORD dwBuyerFieldSlot;
		int nBuyerServerGroup;
        int nSellType;

		sHoldItem()
			: dwSellerChaDbNum( 0 )
			, dwItemTurnNum( 0 )
			, llItemPrice( 0 )
			, nSellerChannel( 0 )
			, dwSellerFieldSlot( 0 )
			, dwBuyerChaDbNum( 0 )
			, dwBuyerFieldSlot( 0 )
            , nSellType( 0 )
			, nSellerServerGroup( 0 )
			, nBuyerServerGroup( 0 )
		{
			::memset( szName, 0x00, sizeof( char ) * CHAR_SZNAME );
		}

		void SetName( const char* _szName )
		{
			if ( _szName )
				StringCchCopy( szName, CHAR_SZNAME, _szName );
		}
	};

	struct sSaleLog
	{
		// �ӵ��� ������ ��Ʈ�� ����
		DWORD dwSellerChaDbNum;
		DWORD dwBuyerChaDbNum;
		std::string strSellerChaName;
		std::string strBuyerChaName;
		std::string strDate;
		std::string strHour;
		SNATIVEID sItemID;
		LONGLONG llMoney;
		DWORD dwItemTurnNum;
		float fCommission;
        int nSellType;

		MSGPACK_DEFINE( 
			dwSellerChaDbNum, 
			dwBuyerChaDbNum, 
			strSellerChaName, 
			strBuyerChaName, 
			strDate, 
			strHour, 
			sItemID, 
			llMoney, 
			dwItemTurnNum,
			fCommission,
            nSellType );

		sSaleLog()
			: dwSellerChaDbNum( 0 )
			, dwBuyerChaDbNum( 0 )
			, sItemID( false )
			, llMoney( 0 )
			, dwItemTurnNum( 0 )
			, fCommission ( 0 )
		{
		}

		sSaleLog( const sSaleLog& sLog )
		{
			dwSellerChaDbNum = sLog.dwSellerChaDbNum;
			dwBuyerChaDbNum = sLog.dwBuyerChaDbNum;
			strSellerChaName = sLog.strSellerChaName;
			strBuyerChaName = sLog.strBuyerChaName;
			strDate = sLog.strDate;
			strHour = sLog.strHour;
			sItemID = sLog.sItemID;
			llMoney = sLog.llMoney;
			dwItemTurnNum = sLog.dwItemTurnNum;
			fCommission = sLog.fCommission;
            nSellType = sLog.nSellType;
		}
	};

	struct sPurchaseQueue
	{
		UUID sItemGUID;
		DWORD dwItemTurnNum;
		LONGLONG llPrice;

		sPurchaseQueue()
			: dwItemTurnNum( 0 )
			, llPrice( 0 )
		{
		}
	};

    struct GameMoneyItemFee
    {
        SNATIVEID sNativeID;
        WORD wFee;
    };

    struct PeriodFee
    {
        WORD wPeriod;
        WORD wFee;
    };

	class GLPrivateMarketPurchseQueue
	{
	public:
		GLPrivateMarketPurchseQueue();
		~GLPrivateMarketPurchseQueue();

	public:
		bool Push( sPurchaseQueue& sPurchase );
		bool Push( UUID sItemGUID, DWORD dwItemTurnNum, LONGLONG llPrice );
		bool Push( std::vector< sPurchaseQueue >& vecPurchase );
	
		bool Pop();

		sPurchaseQueue& Front();

		bool Empty() { return m_theContainer.empty(); }

		void Reset();

	public:
		bool IsPurchasing() { return m_bPurchase; }

	public:
		typedef std::queue< sPurchaseQueue > PURCHASE_QUEUE;

	protected:
		PURCHASE_QUEUE m_theContainer;
		bool m_bPurchase;

	};

	class GLPrivateMarketDefine : public CSingletone< GLPrivateMarketDefine >
	{
	protected:
		friend CSingletone;

	public:
		GLPrivateMarketDefine();
		~GLPrivateMarketDefine();

	public:
		bool Load( const std::string& strFileName, bool bServer );

	public:
		DWORD GetSearchDelayMillisecond() { return m_dwSearchDelayMillisecond; }
		DWORD GetLogDelayMillisecond() { return m_dwLogDelayMillisecond; }

		void SetSearchDelayMillisecond( DWORD dwValue ) { m_dwSearchDelayMillisecond = dwValue; }
		void SetLogDelayMillisecond( DWORD dwValue ) { m_dwLogDelayMillisecond = dwValue; }

		WORD GetSearchItemNumberPerPage() { return m_wSearchItemNumberPerPage; }
		WORD GetLogNumberPerPage() { return m_wLogNumberPerPage; }
		WORD GetLogNumberMaxPerCharacter() { return m_wLogNumberMaxPerCharacter; }

		bool GetCheckChannel() { return m_bCheckChannel; }
		bool GetCheckSchool() { return m_bCheckSchool; }
		bool GetSaleCommission() { return m_bSaleCommission; }
		bool GetAllPage() { return m_bAllPage; }

		void SetCheckChannel( bool bValue ) { m_bCheckChannel = bValue; }
		void SetCheckSchool( bool bValue ) { m_bCheckSchool = bValue; }
		void SetSaleCommission( bool bValue ) { m_bSaleCommission = bValue; }
		void SetAllPage( bool bValue ) { m_bAllPage = bValue; }

		bool CanPurchase( SNATIVEID sMapID );
		bool CanPurchase( DWORD dwMapID );
		void LoadXmlTextData();

		int GetSearchOption() { return m_nSearchOption; }
		void SetSearchOption( int nValue ) { m_nSearchOption = nValue; }

	protected:
		bool loadCommon();
		bool loadServer();
		bool loadUI();
        bool loadConsigmentSale();

	protected:
		std::string m_strFilePath;

	protected:
		DWORD m_dwSearchDelayMillisecond;
		DWORD m_dwLogDelayMillisecond;

		WORD m_wSearchItemNumberPerPage;
		WORD m_wLogNumberPerPage;
		WORD m_wLogNumberMaxPerCharacter;

		bool m_bCheckChannel;
		bool m_bCheckSchool;
		bool m_bSaleCommission;
		bool m_bAllPage;

		std::vector< DWORD > m_vecPurchaseMap;

		int m_nSearchOption;

    // ��Ź�Ǹ� �ý���
    public:
        WORD SellResgisterBasicNum() const { return m_wSellResgisterBasicNum; }
        WORD SellResgisterExpandAddNum() const { return m_wSellResgisterExpandAddNum; }
        std::vector<GameMoneyItemFee > GetMoneySellPeriod(){ return m_vecSellGameMoneyItemFee; }
        std::vector<PeriodFee> GetSellPeriod(){ return m_vecSellPeriodAndFee; }
        WORD TradePossibleRegisterFeeAdd() const { return m_wTradePossibleRegisterFeeAdd; }
        WORD GradeRegisterFeeAdd( EMITEMLEVEL ItemGrade );
        float SalesCompletionFee() const { return m_fSalesCompletionFee; }
        WORD RanPointSaleMinPrice(){ return m_wRanPointSaleMinPrice; };
		bool GoldTrade(){ return m_bGoldTrade; };

        DWORD CalcFee( int nTimeTableIndex, const SITEMCUSTOM& sItemCustom );

    protected:
        WORD m_wSellResgisterBasicNum;
        WORD m_wSellResgisterExpandAddNum;
        std::vector<GameMoneyItemFee> m_vecSellGameMoneyItemFee;
        std::vector<PeriodFee> m_vecSellPeriodAndFee;
        WORD m_wTradePossibleRegisterFeeAdd;
        std::vector<WORD> m_vecItemGradeRegisterFeeAdd;
        float m_fSalesCompletionFee;
        WORD m_wRanPointSaleMinPrice;
		bool m_bGoldTrade;

    };

}

namespace COMMENT
{
	extern std::string PRIVATE_MARKET_ITEM_TYPE[private_market::eItemTypeSize];
	std::string PrivateMarketItemTypeName( private_market::eItemType Type );

	extern std::string PRIVATE_MARKET_ITEM_WEAPON_TYPE[private_market::eItemWeaponTypeSize];
	std::string PrivateMarketItemWeaponTypeName( private_market::eItemWeaponType Type );

	extern std::string PRIVATE_MARKET_ITEM_ARMOR_TYPE[private_market::eItemArmorTypeSize];
	std::string PrivateMarketItemArmorTypeName( private_market::eItemArmorType Type );

	extern std::string PRIVATE_MARKET_ITEM_REQUIRE[private_market::eItemRequireSize];
	std::string PrivateMarketItemRequireName( private_market::eItemRequire Type );

	extern std::string PRIVATE_MARKET_ITEM_RANK[private_market::eItemRankSize];
	std::string PrivateMarketItemRankName( private_market::eItemRank Type );

	extern std::string PRIVATE_MARKET_ITEM_REQUIRE_CLASS[private_market::eItemRequireClassSize];
	std::string PrivateMarketItemRequireClassName( private_market::eItemRequireClass Type );

	extern std::string PRIVATE_MARKET_ITEM_REQUIRE_SEX[private_market::eItemRequireSexSize];
	std::string PrivateMarketItemRequireSexName( private_market::eItemRequireSex Type );

	extern std::string PRIVATE_MARKET_ITEM_BASIC[private_market::eItemBasicSize];
	std::string PrivateMarketItemBasicName( private_market::eItemBasic Type );

	extern std::string PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM[private_market::eItemAddonVarRandomSize];
	std::string PrivateMarketItemAddonVarRandomName( private_market::eItemAddonVarRandom Type );

	extern std::string PRIVATE_MARKET_ITEM_STATE_BLOW[private_market::eItemStateBlowSize];
	std::string PrivateMarketItemStateBlowName( private_market::eItemStateBlowType Type );
}

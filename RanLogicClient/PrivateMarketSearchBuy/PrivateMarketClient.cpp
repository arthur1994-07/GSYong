#include "pch.h"

#include "../GLGaeaClient.h"
#include "PrivateMarketClient.h"

#include "../Char/GLCharacter.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../RanLogic/Market/MarketDefine.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Market/GLConsignmentSaleDefine.h"
#include "../../RanLogicClient/PrivateMarketSearchBuy/ConsignmentSaleClient.h"
#include "../../RanLogicClient/Char/GLCharClient.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../MfcExLib/RanFilter.h"

PrivateMarketClient::PrivateMarketClient(GLGaeaClient* pGaeaClient)
: m_pGaeaClient(pGaeaClient)
, m_nSearchPageCount(0)
, m_nWhishPageCount(0)
, m_nTotalItemNum(0)
, m_nTotalLogCount(0)
, m_nMaketLogPegeCount(0)
, m_bLogEnd(FALSE)
, m_strSearchItemName("")
, m_bOpener(false)
, m_dwOpenerID(NATIVEID_NULL().dwID)
{
	// 이시점에 패스가 세팅되어있지않다!;
	//LoadSearchFileFromITEM();
}

PrivateMarketClient::~PrivateMarketClient()
{

}

std::vector<SSEARCHPAGEDATA*> PrivateMarketClient::GetSearchPageData( int nPage )
{
	SetSearchPageCount();
	std::vector<SSEARCHPAGEDATA*> vecDataPointer;
	//vecDataPointer.clear();
	//unsigned int nRow = MARKET_ROW;

	//unsigned int nStart = nRow * nPage;
	//unsigned int nEnd = nStart+nRow;

	//for(unsigned int i = nStart ; i < nEnd ; i++ )
	//{
	//	if( i < m_vecSearchPageData.size() )
	//		vecDataPointer.push_back( &m_vecSearchPageData[i] );
	//}
	return vecDataPointer;
}

const private_market::sSaleItem* PrivateMarketClient::GetSearchPageData_sSaleItem( DWORD dwIndex )
{
	if ( dwIndex >= m_vecSearchPageData.size() )
		return NULL;

	return &m_vecSearchPageData[dwIndex];
}

std::vector<private_market::sSaleItem*> PrivateMarketClient::GetWhishPageData( int nPage )
{
	if ( nPage < 0 )
		nPage = 0;

	SetWhishPageCount();
	std::vector<private_market::sSaleItem*> vecDataPointer;
	vecDataPointer.clear();

	int nWhishItemCount = static_cast<int>(m_vecWhishPageData.size());

	for(int i = 0 ; i < nWhishItemCount ; i++ )
	{
		if( i < MARKET_ROW && nWhishItemCount > ( i+(nPage*MARKET_ROW) ) )
			vecDataPointer.push_back( &m_vecWhishPageData[i+(nPage*MARKET_ROW)] );
	}
	return vecDataPointer;
}

const private_market::sSaleItem* PrivateMarketClient::GetWishPageData_sSaleItem( DWORD dwIndex )
{
	if ( dwIndex >= m_vecWhishPageData.size() )
		return NULL;

	return &m_vecWhishPageData[ dwIndex ];
}

void PrivateMarketClient::SetSearchPageData()
{
	//int itemCount = 0;
	//GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	//if ( pCharacter )
	//{
	//	for (int i = 0; i < 9; i ++ )
	//	{
	//		for (int j = 0; j < 5; j ++ )
	//		{
	//			SINVENITEM* pInvenItem = pCharacter->m_cInventory.GetItem( i, j );
	//			if ( pInvenItem )
	//			{
	//				SITEMCUSTOM sItemCustom = pInvenItem->sItemCustom;
	//				SSEARCHPAGEDATA sTempData;
	//				sTempData.cstrSellerName="피자킹";
	//				sTempData.lnItemPrice = (LONGLONG)sc::Random::RandomNumber(0,9999)*12345L;
	//				sTempData.sItemcustom = sItemCustom;
	//				m_vecSearchPageData.push_back(sTempData);
	//			}
	//		}
	//	}
	//	//SetSearchPageCount();
	//}
}

bool PrivateMarketClient::SetWhishPageData( private_market::sSaleItem* psSaleItem )
{
    if ( psSaleItem->dwSellerChaDbNum == m_pGaeaClient->GetCharacter()->m_CharDbNum )
        return false;

	BOOST_FOREACH(private_market::sSaleItem vecpsalItem, m_vecWhishPageData)
	{
		if (vecpsalItem.sItemGUID == psSaleItem->sItemGUID)
			return false;
	}

	m_vecWhishPageData.push_back(*psSaleItem);

	int nSize = static_cast<int>(m_vecWhishPageData.size());
	SetTotalWhishItemNum(nSize);

	return true;
}

void PrivateMarketClient::SetSearchPageCount()
{
	int nSize = m_nTotalItemNum;
	int nTemp1;
	float fTemp2;
	nTemp1 = nSize / MARKET_ROW;
	fTemp2 = static_cast<float>(nSize % MARKET_ROW);

	if ( fTemp2 != 0 )
		m_nSearchPageCount = ( nTemp1 + 1 );
	else
		m_nSearchPageCount = nTemp1;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateSearchMaxPage,
		"-n", m_nSearchPageCount );
}

void PrivateMarketClient::SetWhishPageCount()
{
	int nSize = m_nTotalWhishItemNum;
	int nTemp1;
	float fTemp2;
	nTemp1 = nSize / MARKET_ROW;
	fTemp2 = static_cast<float>(nSize % MARKET_ROW);

	if ( fTemp2 != 0 )
		m_nWhishPageCount = ( nTemp1 + 1 );
	else
		m_nWhishPageCount = nTemp1;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateWishMaxPage,
		"-n", m_nWhishPageCount );
}

int PrivateMarketClient::GetSearchPageCount()
{
	return m_nSearchPageCount;
}

int PrivateMarketClient::GetWhishPageCount()
{
	return m_nWhishPageCount;
}

void PrivateMarketClient::DelWhishPageData( UUID uuidID )
{
	//// 예외 처리 필요
	//// 이작업이 성공 하였을시 해당 UI SetVisible(False)
	//BOOST_FOREACH(private_market::sSaleItem vecData, m_vecWhishPageData)
	//{
	//	if ( vecData.sItemGUID == uuidID )
	//		m_vecWhishPageData.erase(vecData);
	//}

	std::vector<private_market::sSaleItem> ::iterator iter = m_vecWhishPageData.begin();

	for ( unsigned i = 0; i < m_vecWhishPageData.size(); i++)
	{
		if ( m_vecWhishPageData[i].sItemGUID == uuidID )
			m_vecWhishPageData.erase(iter++);
		else
			++iter;
	}

	//for(int i = m_vecWhishPageData.size() - 1; i >= 0; i--)
	//{
	//	if ( m_vecWhishPageData[i].sItemGUID == uuidID )
	//		m_vecWhishPageData.erase(m_vecWhishPageData.begin() + i);
	//}

	SetTotalWhishItemNum(static_cast<int>(m_vecWhishPageData.size()));
}

private_market::sSaleItemSearchandSortBasic PrivateMarketClient::SetSearchData( CString strName,
																				 int nItemType,
																				 BOOL bBool,
																				 DWORD dwItemTurnNum,
																				 int nWeaponType,
																				 int nArmorType,
																				 int nStatsType,
																				 int nStatsValue,
																				 int nItemLRank,
																				 int nClassCategory,
																				 int nSexCategory,
																				 int nNomalStatsType1,
																				 float fNomalStatsValue1,
																				 int nNomalStatsType2,
																				 float fNomalStatsValue2,
																				 int nNomalStatsType3,
																				 float fNomalStatsValue3,
																				 int nNomalStatsType4,
																				 float fNomalStatsValue4,
																				 int nAddRandomType1,
																				 float fAddRandomValue1,
																				 int nAddRandomType2,
																				 float fAddRandomValue2,
																				 int nAddRandomType3,
																				 float fAddRandomValue3,
																				 int nAddRandomType4,
																				 float fAddRandomValue4,
																				 int nSearchSellType)
{
	m_sSaleItemSearchandSortBasic.vecBasic.clear();
	m_sSaleItemSearchandSortBasic.vecAddonVarRandom.clear();
	SetItemNameSearch( strName );
	SetItemType( nItemType);
	SetTimeLimit(  bBool );
	SetItemTurnNum(dwItemTurnNum);
	SetWeaponType( nWeaponType);
	SetArmorType( nArmorType);
	SetItemStatsType( nStatsType);// 힘, 민첩, 정신
	SetItemStatsValue( nStatsValue);
	SetItemLevelCategory( nItemLRank);
	SetClassCategory( nClassCategory);
	SetSexCategory( nSexCategory);
	//일반능력치
	SetNomalStatsCategory_1( nNomalStatsType1, fNomalStatsValue1-FLT_EPSILON);
	SetNomalStatsCategory_2( nNomalStatsType2, fNomalStatsValue2-FLT_EPSILON);
	SetNomalStatsCategory_3( nNomalStatsType3, fNomalStatsValue3-FLT_EPSILON);
	SetNomalStatsCategory_4( nNomalStatsType4, fNomalStatsValue4-FLT_EPSILON);
	//가산 및 랜덤 옵션
	SetAddRandomOptionCategory_1( nAddRandomType1, fAddRandomValue1-FLT_EPSILON);
	SetAddRandomOptionCategory_2( nAddRandomType2, fAddRandomValue2-FLT_EPSILON);
	SetAddRandomOptionCategory_3( nAddRandomType3, fAddRandomValue3-FLT_EPSILON);
	SetAddRandomOptionCategory_4( nAddRandomType4, fAddRandomValue4-FLT_EPSILON);

	m_sSaleItemSearchandSortBasic.nSearchSellType = nSearchSellType;
	return m_sSaleItemSearchandSortBasic;
}

void PrivateMarketClient::SetItemNameSearch( CString strName )
{
	m_strSearchItemName = strName;

    // Convert a TCHAR string to a LPCSTR
	CT2CA pszConvertedAnsiString(m_strSearchItemName);
	// Construct a std::string using the LPCSTR input
	std::string s(pszConvertedAnsiString);

	sc::string::trim( s );

	m_sSaleItemSearchandSortBasic.strItemName = s;
}

void PrivateMarketClient::SetItemType( int nType )
{
	m_sSaleItemSearchandSortBasic.nType = nType;
}


void PrivateMarketClient::SetItemLevelCategory(int nRank)
{
	m_sSaleItemSearchandSortBasic.nRank = nRank;
}

void PrivateMarketClient::SetClassCategory(int nClassCategory)
{
	m_sSaleItemSearchandSortBasic.nRequireClass = nClassCategory;
}

void PrivateMarketClient::SetSexCategory(int nSexCategory)
{
	m_sSaleItemSearchandSortBasic.nRequireSex = nSexCategory;
}

void PrivateMarketClient::SetNomalStatsCategory_1(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecBasic;
	vecBasic.nType = nType;
	vecBasic.fValue = fValue;
	m_sSaleItemSearchandSortBasic.vecBasic.push_back(vecBasic);
}

void PrivateMarketClient::SetNomalStatsCategory_2(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecBasic;
	vecBasic.nType = nType;
	vecBasic.fValue = fValue;
	m_sSaleItemSearchandSortBasic.vecBasic.push_back(vecBasic);
}

void PrivateMarketClient::SetNomalStatsCategory_3(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecBasic;
	vecBasic.nType = nType;
	vecBasic.fValue = fValue;
	m_sSaleItemSearchandSortBasic.vecBasic.push_back(vecBasic);
}

void PrivateMarketClient::SetNomalStatsCategory_4(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecBasic;
	vecBasic.nType = nType;
	vecBasic.fValue = fValue;
	m_sSaleItemSearchandSortBasic.vecBasic.push_back(vecBasic);
}

void PrivateMarketClient::SetAddRandomOptionCategory_1(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecAddonVarRandom;
	vecAddonVarRandom.fValue = fValue;
	vecAddonVarRandom.nType = nType;
	m_sSaleItemSearchandSortBasic.vecAddonVarRandom.push_back(vecAddonVarRandom);
}

void PrivateMarketClient::SetAddRandomOptionCategory_2(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecAddonVarRandom;
	vecAddonVarRandom.fValue = fValue;
	vecAddonVarRandom.nType = nType;
	m_sSaleItemSearchandSortBasic.vecAddonVarRandom.push_back(vecAddonVarRandom);
}

void PrivateMarketClient::SetAddRandomOptionCategory_3(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecAddonVarRandom;
	vecAddonVarRandom.fValue = fValue;
	vecAddonVarRandom.nType = nType;
	m_sSaleItemSearchandSortBasic.vecAddonVarRandom.push_back(vecAddonVarRandom);
}

void PrivateMarketClient::SetAddRandomOptionCategory_4(int nType, float fValue)
{
	private_market::sSaleItemSearchOptionFloat vecAddonVarRandom;
	vecAddonVarRandom.fValue = fValue;
	vecAddonVarRandom.nType = nType;
	m_sSaleItemSearchandSortBasic.vecAddonVarRandom.push_back(vecAddonVarRandom);
}

void PrivateMarketClient::SetItemStatsType(int nType)
{
	m_sSaleItemSearchandSortBasic.nRequire = nType;
}

void PrivateMarketClient::SetItemStatsValue( int nValue )
{
	m_sSaleItemSearchandSortBasic.nRequireValue = nValue;
}

void PrivateMarketClient::SetWeaponType( int nType )
{
	m_sSaleItemSearchandSortBasic.nWeaponType = nType;
}

void PrivateMarketClient::SetArmorType( int nType )
{
	m_sSaleItemSearchandSortBasic.nArmorType = nType;
}

void PrivateMarketClient::SetTimeLimit( BOOL bBool )
{
	m_sSaleItemSearchandSortBasic.bTimelimit = bBool;
}

void PrivateMarketClient::SetTotalItemNum( int nValue )
{
	if ( nValue < 0 )
		nValue = 0;
	m_nTotalItemNum = nValue;
	SetSearchPageCount();
}

void PrivateMarketClient::SetSearchResultData( private_market::sSaleItem sSaleItem )
{
	//sSaleItem NULL 체크 필요
	m_vecSearchPageData.push_back(sSaleItem);
}

std::vector<private_market::sSaleItem*> PrivateMarketClient::GetSearchResultData()
{

	std::vector<private_market::sSaleItem*> vecDataPointer;
	vecDataPointer.clear();

	for(unsigned int i = 0 ; i < m_vecSearchPageData.size() ; i++ )
	{
		if( i < 8 )
			vecDataPointer.push_back( &m_vecSearchPageData[i] );
	}
	return vecDataPointer;
}

void PrivateMarketClient::ClearSearchItemdate()
{
	m_vecSearchPageData.clear();
}

void PrivateMarketClient::SetLogPageData( private_market::sSaleLog sMaketLog )
{
	//sMaketLog NULL 체크 필요
	m_sMaketLog.push_back( sMaketLog );
}

void PrivateMarketClient::SetLogPageCount()
{
	int nSize = m_nTotalLogCount;
	int nTemp1;
	float fTemp2;
	nTemp1 = nSize / LOG_ROW;
	fTemp2 = static_cast<float>(nSize % LOG_ROW);

	if ( fTemp2 != 0 )
		m_nMaketLogPegeCount = ( nTemp1 + 1 );
	else
		m_nMaketLogPegeCount = nTemp1;
}

BOOL PrivateMarketClient::ISLogEnd()
{
	return m_bLogEnd;
}

void PrivateMarketClient::SetLogEnd( BOOL bLogEnd )
{
	m_bLogEnd = bLogEnd;
}

void PrivateMarketClient::SetTotalLogCount(const int nTotalLogCount)
{
	// 케릭터 선택 화면으로 갔다올때 리셋 필요
	if ( nTotalLogCount <= 0 )
		return;
	m_nTotalLogCount += nTotalLogCount;
	SetLogPageCount();
}

std::vector<private_market::sSaleLog*> PrivateMarketClient::GetLogtData()
{

	std::vector<private_market::sSaleLog*> vecDataPointer;
	vecDataPointer.clear();

	for(unsigned int i = 0 ; i < m_sMaketLog.size() ; i++ )
	{
		vecDataPointer.push_back( &m_sMaketLog[i] );
	}
	return vecDataPointer;
}

int PrivateMarketClient::GetLogPageCount()
{
	return m_nMaketLogPegeCount;
}

void PrivateMarketClient::SetTotalWhishItemNum( int nValue )
{
	if ( nValue < 0 )
		nValue = 0;
	m_nTotalWhishItemNum = nValue;
	SetWhishPageCount();
}

void PrivateMarketClient::SetItemTurnNum( DWORD dwNum )
{
	m_sSaleItemSearchandSortBasic.dwItemTurnNum = dwNum;
}

void PrivateMarketClient::ClearSearchPageData()
{
	m_vecSearchPageData.clear();
	m_nSearchPageCount = 0;
	m_nTotalItemNum = 0;
}

void PrivateMarketClient::ClearWhishPageData()
{
	m_vecWhishPageData.clear();
	m_nWhishPageCount = 0;
	m_nTotalWhishItemNum = 0;
}

void PrivateMarketClient::ClearLogPage()
{
	m_sMaketLog.clear();
	m_nTotalLogCount = 0;
	m_nMaketLogPegeCount = 0;
}

void PrivateMarketClient::ClearMarketSearchBuyData()
{
	ClearSearchPageData();
	ClearWhishPageData();
	ClearLogPage();
}

CString PrivateMarketClient::GetSearchItemName()
{
	return m_strSearchItemName;
}

void PrivateMarketClient::LoadSearchFileFromITEM()
{
	m_listItemName.clear();
	std::string FullPath(GLOGIC::GetPath());
	FullPath.append(GLogicData::GetInstance().GetItemFileName());

	std::auto_ptr<sc::BaseStream> pBStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(), 
		GLogicData::GetInstance().GetGlogicZipFile(),
		FullPath.c_str(),
		GLogicData::GetInstance().GetItemFileName(),
		false,
		GLOGIC::UseLogicPackFile()));

	if (!pBStream.get())
	{
		sc::writeLogError(
			sc::string::format(
			"GLItemMan Load file %1% %2% %3%",
			GLogicData::GetInstance().GetGlogicZipFile(),
			FullPath,
			GLogicData::GetInstance().GetItemFileName()));
		return;
	}

	sc::BaseStream& SFile = *pBStream;

	DWORD dwFILEVER;
	char szFILETYPE[_MAX_FNAME];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode ( (dwFILEVER>=GLItemMan::ENCODE_VER) );

	DWORD dwNum=0;

	SFile >> dwNum;

	for (DWORD i=0; i<dwNum; i++)
	{
		SITEM sItem;

		if ( !sItem.LoadFile(SFile) )
		{
			AfxMessageBox( "SITEM Loading 에 문제가 발생하였습니다." );
			return;	
		}

		if( sItem.sBasicOp.bSearch )
			m_listItemName.push_back( CString(sItem.GetName()) ); 
	}	
}

bool PrivateMarketClient::IsUseItem( const SITEMCUSTOM& sItemCustom )
{
	int nUseCount = 0;
	GLCharacter* pGLCharacter = m_pGaeaClient->GetCharacter();

	if ( !pGLCharacter )
		return false;

	DWORD	dwCharClass = CharIndexToClass(pGLCharacter->GetCharIndex());
	WORD	dwSchool = pGLCharacter->GetCharSchool();
	WORD	wLevel = pGLCharacter->GetLevel();
	const int nMelee(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_MELEE]);
	const int nRange(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_RANGE]);
	const int nMagic(pGLCharacter->m_powerAttribute[SKILL::EMAPPLY_MAGIC]);
	//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
	const SCHARSTATS& sstats = pGLCharacter->m_sSUMSTATS_SKILL;	

	SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if ( !pItemData )
		return false;

	if ( !(pItemData->sBasicOp.dwReqCharClass&dwCharClass) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.dwReqSchool & index2school(dwSchool) ) )
		nUseCount++;

	if ( !(wLevel >= pItemData->sBasicOp.wReqLevelDW) )
		nUseCount++;

	//상한 레벨이 0으로 셋팅되어 있어 비교가 안됨
	//if ( !(wLevel <= pItemData->sBasicOp.wReqLevelUP) )
	//	nUseCount++;

	if ( !(nMelee >= int(pItemData->sBasicOp.wReqPA)) )
		nUseCount++;

	if ( !(nRange >= int(pItemData->sBasicOp.wReqSA)) )
		nUseCount++;

	// 	if ( !(nMagic >= int(pItemData->sBasicOp.wReqMA)) )
	// 		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wDex <= sstats.wDex ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wInt <= sstats.wInt ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wPow <= sstats.wPow ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wSpi <= sstats.wSpi ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wSta <= sstats.wSta ) )
		nUseCount++;

	if ( !(pItemData->sBasicOp.sReqStats.wStr <= sstats.wStr ) )
		nUseCount++;

	if ( nUseCount>0 )
		return false;
	else
		return true;
}

void PrivateMarketClient::GenItemLuaTable( const std::vector< private_market::sSaleItem >& vecSaleItem, LuaTable& tb )
{
	int nTableCount = 1;
	int nIndexCount = 0;

	std::vector< private_market::sSaleItem >::const_iterator iter = vecSaleItem.begin();
	for( ; iter != vecSaleItem.end(); ++iter )
	{
		const private_market::sSaleItem& sSaleItem = *iter;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sSaleItem.sItemcustom.GetBoxWrappedItemID() );
		if( NULL == pItem )
			continue;

		// 시간
		__time64_t tSpanTime = sSaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sSaleItem.sItemcustom.Mid() );
		tbItem.set( 2, sSaleItem.sItemcustom.Sid() );
		
		LuaTable tbName( GLWidgetScript::GetInstance().GetLuaState() );
		tbName.set( 1, pItem->GetName() );
		tbName.set( 2, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );
		tbItem.set( 3, tbName );

		tbItem.set( 4, sSaleItem.sItemcustom.GETGRADE( EMGRINDING_NUNE ) );
		tbItem.set( 5, IsUseItem( sSaleItem.sItemcustom ) );
		tbItem.set( 6, sSaleItem.szSellerName );
		tbItem.set( 7, sSaleItem.llItemPrice );
		tbItem.set( 8, sSaleItem.dwItemTurnNum );
		tbItem.set( 9, nIndexCount++ );	
		tbItem.set( 10, static_cast< int >( tSpanTime / 3600 ) );
		tbItem.set( 11, private_market::eSellType_ConsignmentSale_RanPoint == sSaleItem.nSellType ? true : false );
		tbItem.set( 12, sSaleItem.sSellerMapID.dwID );

		tbItem.set( 13, sSaleItem.sItemGUID.Data1 );
		tbItem.set( 14, sSaleItem.sItemGUID.Data2 );
		tbItem.set( 15, sSaleItem.sItemGUID.Data3 );
		tbItem.set( 16, sSaleItem.sItemGUID.Data4 );

		tb.set( nTableCount++, tbItem );
	}
}

LuaTable PrivateMarketClient::GetSearchItemList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	GenItemLuaTable( m_vecSearchPageData, tbItemList );

	return tbItemList;
}

LuaTable PrivateMarketClient::GetSearchAbleItemNameList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	const int nSearchableItemSize(static_cast<int>(m_listItemName.size()));

	int nSearchableItemCnt(1);
	std::list<CString>::iterator itr_begin = m_listItemName.begin()
		, itr_end = m_listItemName.end();
	while ( itr_begin != itr_end )
	{
		tbItemList.set(nSearchableItemCnt, (*itr_begin).GetString());
		++itr_begin;
		++nSearchableItemCnt;
	}
	
	return tbItemList;
}

void PrivateMarketClient::ReqBuySearchItem( LuaTable tbPurchase )
{
	//구매목록
	std::vector< private_market::sPurchaseQueue > vecPurchase;

	for( int i = 1; i <= tbPurchase.TableLen(); ++i )
	{
		LuaTable tbItem = tbPurchase.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbItem ) )
			continue;

		if( 6 != tbItem.TableLen() )
			continue;

		const unsigned char* szData4 = tbItem.get< unsigned char* >( 4 );
		if( NULL == szData4 )
			continue;

		if( sizeof( unsigned char ) * 8 < sizeof( szData4 ) )
			continue;

		private_market::sPurchaseQueue sPurchaseQueue;
		sPurchaseQueue.sItemGUID.Data1 = tbItem.get< unsigned long >( 1 );
		sPurchaseQueue.sItemGUID.Data2 = tbItem.get< unsigned short >( 2 );
		sPurchaseQueue.sItemGUID.Data3 = tbItem.get< unsigned short >( 3 );
		memcpy( sPurchaseQueue.sItemGUID.Data4, szData4, sizeof( unsigned char ) * 8 );
		sPurchaseQueue.dwItemTurnNum = tbItem.get< DWORD >( 5 );
		sPurchaseQueue.llPrice = tbItem.get< LONGLONG >( 6 );
		vecPurchase.push_back( sPurchaseQueue );
	}
	
	//구입하고자 하는 아이템이 맞다면 구매 요청
	if( false == vecPurchase.empty() )
		m_pGaeaClient->ReqPrivateMarketItemBuy( vecPurchase );
}

bool PrivateMarketClient::SearchItem( WORD wPage, LuaTable tbSearchInfo )
{
	if( 31 != tbSearchInfo.TableLen() )
	{
		GLWidgetScript::PrintDebug( "PrivateMarketClient::SearchItem - Table Parameter Count is not match." );
		return false;
	}

	const char* szName = tbSearchInfo.get< const char* >( 1 );
	int nSortOption = tbSearchInfo.get< int >( 2 );
	int nSortMethod = tbSearchInfo.get< int >( 3 );

	std::string strName = NULL == szName ? "" : szName;

	if( false == strName.empty() )
	{
		private_market::sSaleItemSearchandSortName sSortName;
		sSortName.strItemName = strName;
		sSortName.nSortOption = nSortOption;
		sSortName.nSortMethod = nSortMethod;
		sSortName.wPage = wPage;

		if( FAILED( m_pGaeaClient->ReqPrivateMarketItemSearchByName( &sSortName ) ) )
			return false;
	}
	else
	{
		bool bMoney = tbSearchInfo.get< bool >( 7 );
		bool bPoint = tbSearchInfo.get< bool >( 8 );

		private_market::eSearchMoneyType emMoneyType = private_market::eSearchMoneyType_ALL;
		if( true == bMoney && false == bPoint )
			emMoneyType = private_market::eSearchMoneyType_Gold;
		else if( false == bMoney && true == bPoint )
			emMoneyType = private_market::eSearchMoneyType_RanPoint;

		private_market::sSaleItemSearchandSortBasic sBasic;
		sBasic = SetSearchData( NULL == szName ? "" : szName, 
			tbSearchInfo.get< int >( 4 ),
			true == tbSearchInfo.get< bool >( 9 ) ? TRUE : FALSE,
			tbSearchInfo.get< DWORD >( 10 ),
			tbSearchInfo.get< int >( 5 ),
			tbSearchInfo.get< int >( 6 ),
			tbSearchInfo.get< int >( 11 ),
			tbSearchInfo.get< int >( 12 ),
			tbSearchInfo.get< int >( 13 ),
			tbSearchInfo.get< int >( 14 ),
			tbSearchInfo.get< int >( 15 ),
			tbSearchInfo.get< int >( 16 ),
			tbSearchInfo.get< float >( 17 ),
			tbSearchInfo.get< int >( 18 ),
			tbSearchInfo.get< float >( 19 ),
			tbSearchInfo.get< int >( 20 ),
			tbSearchInfo.get< float >( 21 ),
			tbSearchInfo.get< int >( 22 ),
			tbSearchInfo.get< float >( 23 ),
			tbSearchInfo.get< int >( 24 ),
			tbSearchInfo.get< float >( 25 ),
			tbSearchInfo.get< int >( 26 ),
			tbSearchInfo.get< float >( 27 ),
			tbSearchInfo.get< int >( 28 ),
			tbSearchInfo.get< float >( 29 ),
			tbSearchInfo.get< int >( 30 ),
			tbSearchInfo.get< float >( 31 ),
			emMoneyType );

		sBasic.wPage = wPage;
		sBasic.nSortOption = nSortOption;
		sBasic.nSortMethod = nSortMethod;

		if( FAILED( m_pGaeaClient->ReqPrivateMarketItemSearch( &sBasic ) ) )
			return false;
	}

	return true;
}

LuaTable PrivateMarketClient::GetWishItemList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	GenItemLuaTable( m_vecWhishPageData, tbItemList );
	
	return tbItemList;
}

bool PrivateMarketClient::InsertWishItem( WORD wSearchIndex )
{
	if( 0 > wSearchIndex  || m_vecSearchPageData.size() < wSearchIndex )
		return false; 
	
	return SetWhishPageData( &m_vecSearchPageData[ wSearchIndex ] );
}

bool PrivateMarketClient::DeleteWishItem( WORD wWishIndex )
{
	if( 0 > wWishIndex || m_vecWhishPageData.size() < wWishIndex )
		return false; 
		
	DelWhishPageData( m_vecWhishPageData[ wWishIndex ].sItemGUID );

	return true;
}


bool PrivateMarketClient::RefreshLog()
{
	if( m_pGaeaClient )
	{
		SetLogEnd(FALSE);
	
		if (S_OK == m_pGaeaClient->ReqPrivateMarketLog()  )
			return true;
	}

	return false;
}

LuaTable PrivateMarketClient::GetLogList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	
	int nTableCount = 1;

	std::vector< private_market::sSaleLog >::iterator iter = m_sMaketLog.begin();
	for( ; iter != m_sMaketLog.end(); ++iter )
	{
		const private_market::sSaleLog& sLog = *iter;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sLog.sItemID );
		if( NULL == pItem )
			continue;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sLog.strHour );
		tbItem.set( 2, sLog.strDate );
		tbItem.set( 3, sLog.nSellType );

		LuaTable tbName( GLWidgetScript::GetInstance().GetLuaState() );
		tbName.set( 1, pItem->GetName() );
		tbName.set( 2, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );
		tbItem.set( 4, tbName );

		tbItem.set( 5, sLog.dwItemTurnNum );
		tbItem.set( 6, sLog.llMoney );
		tbItem.set( 7, sLog.fCommission );
		tbItem.set( 8, sLog.strBuyerChaName );
		tbItem.set( 9, sLog.strSellerChaName );

		tbItemList.set( nTableCount++, tbItem );
	}

	return tbItemList; 
}

LuaTable PrivateMarketClient::GetPointItemList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	
	int nTableCount = 1;
	int nIndexCount = 0;

	const private_market::SALE_ITEMS_VECTOR& vecItemList = m_pGaeaClient->GetConsignmentSale()->GetList();
	private_market::SALE_ITEMS_VECTOR::const_iterator iter = vecItemList.begin();

	for( ; iter != vecItemList.end(); ++iter )
	{
		const private_market::ConsignmentSaleItem& sSaleItem = *iter;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sSaleItem.sItemcustom.GetBoxWrappedItemID() );
		if( NULL == pItem )
			continue;

		// 시간
		__time64_t tSpanTime = sSaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sSaleItem.sItemcustom.Mid() );
		tbItem.set( 2, sSaleItem.sItemcustom.Sid() );
		tbItem.set( 3, pItem->GetName() );
		tbItem.set( 4, sSaleItem.sItemcustom.GETGRADE( EMGRINDING_NUNE ) );
		tbItem.set( 5, IsUseItem( sSaleItem.sItemcustom ) );
		tbItem.set( 6, sSaleItem.strChaName );
		tbItem.set( 7, sSaleItem.llPrice );
		tbItem.set( 8, sSaleItem.wTurnNum );
		tbItem.set( 9, sSaleItem.wTotalTurnNum );
		tbItem.set( 10, tSpanTime );
		tbItem.set( 11, private_market::eSellType_ConsignmentSale_RanPoint == sSaleItem.SellType ? true : false );
		tbItem.set( 12, nIndexCount++ );
		tbItem.set( 13, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );

		tbItemList.set( nTableCount++, tbItem );
	}

	return tbItemList;
}

LuaTable PrivateMarketClient::GetSellingMoneyList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableCount = 1;

	const std::vector< private_market::GameMoneyItemFee >& vecMoneyPeriodFee = 
		private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

	std::vector< private_market::GameMoneyItemFee >::const_iterator iter = vecMoneyPeriodFee.begin();
	for( ; iter != vecMoneyPeriodFee.end(); ++iter )
	{
		const private_market::GameMoneyItemFee& sGameMoney = *iter;

		const SITEM* pItem = GLItemMan::GetInstance().GetItem( sGameMoney.sNativeID );
		if( NULL == pItem )
		{
			GASSERT( 0 && "[PrivateMarket.lua] File SellGameMoneySetting Is Error!!!!" );
			continue;
		}

		if( ITEM_CONSIGMENT_SALE_GAME_MONEY != pItem->BasicType() )
		{
			GASSERT( 0 && "[PrivateMarket.lua] File SellGameMoneySetting Item Type Is Not [RanGameMoney]" );
			continue;
		}

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sGameMoney.sNativeID.Mid() );
		tbItem.set( 2, sGameMoney.sNativeID.Sid() );
		tbItem.set( 3, pItem->sBasicOp.dwSellPrice );

		tbItemList.set( nTableCount++, tbItem );
	}

	return tbItemList;
}

LuaTable PrivateMarketClient::GetSellPeriodList()
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableCount = 1;

	const std::vector< private_market::PeriodFee >& vecPeriodFee = 
		private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();

	std::vector< private_market::PeriodFee >::const_iterator iter = vecPeriodFee.begin();
	for( ; iter != vecPeriodFee.end(); ++iter )
		tbItemList.set( nTableCount++, iter->wPeriod );

	return tbItemList;
}

LuaTable PrivateMarketClient::GetExpansionSlotInfo()
{
	__time64_t tExpansionDate = m_pGaeaClient->GetCharacterData().GetConsignmentSaleSlotExpansionDate();    
	__time64_t tCurTime = CTime::GetCurrentTime().GetTime();
	
	WORD wExpandAddNum = 0;
	WORD wCurrentSlotNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();

	if( 0 < tExpansionDate - tCurTime )
	{
		wExpandAddNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();
		wCurrentSlotNum += wExpandAddNum;
	}

	CTime cTimm( tExpansionDate );

	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	tbItemList.set( 1, m_pGaeaClient->GetConsignmentSale()->GetList().size() );	// 현재 개인 상점 등록된 아이템 슬롯수
	tbItemList.set( 2, wExpandAddNum );											// 개인 상점 확장 슬롯
	tbItemList.set( 3, wCurrentSlotNum );										// 확장슬롯을 포함한 기본 최대 슬롯
	tbItemList.set( 4, cTimm.GetYear() );
	tbItemList.set( 5, cTimm.GetMonth() );
	tbItemList.set( 6, cTimm.GetDay() );

	return tbItemList;
}

float PrivateMarketClient::GetSalesCompletionFee()
{
	return (private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee() * 100.0f);
}

bool PrivateMarketClient::ReqRegistItem( int nPosX, int nPosY, WORD wCount,LONGLONG llItemPrice, 
										WORD wSellType, WORD wMoneyIndex, WORD wPeriodIndex )
{
	if ( 0 >= wCount || W_LIMITED_POINT_TRADE_COUNT < wCount || 0 >= llItemPrice || 0 >= wSellType )
		return false;

	if ( llItemPrice < GetPointTradeMinPrice() ||
		LL_LIMITED_A_UNIT_PRICE < llItemPrice )
		return false;

	SINVENITEM sInvenItem;

	// 인벤토리 아이템
	if( 0 <= nPosX && 0 <= nPosY )
	{
		GLCharacter* pChar = m_pGaeaClient->GetCharacter();
		if( NULL != pChar )
			sInvenItem = *pChar->m_cInventory.FindPosItem( nPosX, nPosY );
	}
	// 금화 아이템
	else
	{
		const std::vector< private_market::GameMoneyItemFee > vecMoneyPeriodFee =
			private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

		if( 0 <= wMoneyIndex && wMoneyIndex < vecMoneyPeriodFee.size() )
			sInvenItem.sItemCustom = SITEMCUSTOM( vecMoneyPeriodFee[ wMoneyIndex ].sNativeID );
	}

	// 아이템이 등록이 정상적으로 되지 않음
	if( false == sInvenItem.GetNativeID().IsValidNativeID() )
		return false;

	m_pGaeaClient->GetConsignmentSale()->ReqRegistItem( 
		sInvenItem, llItemPrice, wCount, wPeriodIndex, (private_market::eSellType) wSellType );

	return true;
}

bool PrivateMarketClient::ReqUnregistItem( int nSelectedIndex )
{
	private_market::SALE_ITEMS_VECTOR vecItemList = m_pGaeaClient->GetConsignmentSale()->GetList();

	if( 0 > nSelectedIndex || (size_t) nSelectedIndex >= vecItemList.size() )
		return false;

	m_pGaeaClient->GetConsignmentSale()->ReqUnregist( vecItemList[ nSelectedIndex ].GetGUID() );

	return true;
}

DWORD PrivateMarketClient::CalcInsertionFee( int nPeriodIndex, WORD wMID, WORD wSID )
{
	// 수수료 계산
	DWORD dwFee = 0;

	SITEMCUSTOM sItemCustom( SNATIVEID( wMID, wSID ) );

	// 아이템 확인
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if( NULL == pItem )
		return dwFee;

	// 란포인트 거래를 할수 있는 상태인가?
	if( false == sItemCustom.IsExchange() )
	{
		if( pItem->sBasicOp.RanPointTradeCount() <= sItemCustom.wRanPointTradeCount )
			return dwFee;
	}

	// 거래 기간에 따라 기본 수수료를 부가한다.
	const std::vector< private_market::PeriodFee >& vecPeriodFee = 
		private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();

	if( 0 <= nPeriodIndex && (size_t) nPeriodIndex < vecPeriodFee.size() )
		dwFee += vecPeriodFee[ nPeriodIndex ].wFee;
	else 
		return dwFee;

	const std::vector< private_market::GameMoneyItemFee >& vecMoneyPeriodFee =
		private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

	bool bIsGameMoney = false;
	DWORD dwGameMoneyFee = 0;

	for( unsigned int i = 0; i < vecMoneyPeriodFee.size(); ++i )
	{
		const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[ i ];

		if( value.sNativeID == sItemCustom.GetNativeID() )
		{
			bIsGameMoney = true;
			dwGameMoneyFee = value.wFee;
			break;
		}
	}

	// 기본적으로 아이템 등급에 따라 수수료를 부가한다.
	// 게임머니인경우 수수료를 다르게 부가한다.
	if( true == bIsGameMoney )
		dwFee += dwGameMoneyFee;
	else
		dwFee += private_market::GLPrivateMarketDefine::Instance()->GradeRegisterFeeAdd( pItem->sBasicOp.emLevel );


	// 거래 불가능인 경우 추가 수수료를 부가한다.
	if( sItemCustom.IsExchange() == false )
		dwFee += private_market::GLPrivateMarketDefine::Instance()->TradePossibleRegisterFeeAdd();

	return dwFee;
}

WORD PrivateMarketClient::GetPointTradeMinPrice()
{
	return private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice();
}

int PrivateMarketClient::PointTradeCheckRegItem( WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	// 인벤에 아이템이 있는가?
	const SINVENITEM* pInvenItem = pChar->m_cInventory.FindPosItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return 1;

	const SITEM* pItem = GLItemMan::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItem )
		return 1;

	// 코스튬이 붙어 있다면 거래 불가
	if( pInvenItem->sItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		return 2;
	}
	// 시효성 아이템 거래 불가
	else if( true == pInvenItem->sItemCustom.IsTimeLimit() )
	{
		return 3;
	}
	// 거래 불가템 거래 안됨
	else if( false == pInvenItem->sItemCustom.IsExchange() )
	{
		// 포인트 거래 횟수 초과
		if( pItem->sBasicOp.RanPointTradeCount() <= pInvenItem->sItemCustom.wRanPointTradeCount )
			return 4;
	}

	return 0;
}

void PrivateMarketClient::ReqPointTradeList()
{
	m_pGaeaClient->GetConsignmentSale()->ReqListUpdate();
}

void PrivateMarketClient::ReqPrivateMarketRegItemOpen( const char* cMarketName )
{
	if( NULL == cMarketName )
		return;

	const SMAPNODE *pMapNode(m_pGaeaClient->FindMapNode(m_pGaeaClient->GetActiveMapID().getBaseMapID()));
	if ( pMapNode && !pMapNode->IsPrivateMarketOpen() )	
	{
		m_pGaeaClient->PrintMsgTextDlg ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_NOT_OPENABLE") );
		return;
	}

	if( m_pGaeaClient->GetCharacter()->VehicleState() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_NOIDLE") );
		return;
	}

	CString strTITLE = cMarketName;
	CString strTrim = cMarketName;
	strTrim.Trim ( " " );
	if ( !strTITLE.GetLength() || !strTrim.GetLength() )
		return;

	// 금지어 필터링
	if ( CRanFilter::GetInstance().ChatFilter( strTITLE ) )
	{
		strTITLE = CRanFilter::GetInstance().GetProverb();
	}

	m_pGaeaClient->GetCharacter()->ReqPMarketTitle ( strTITLE );
}

void PrivateMarketClient::ReqPrivateMarketRegItemMsgBoxOpen( int nPosX, int nPosY )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	SINVENITEM* sInvenItem = pChar->m_cInventory.FindPosItem( nPosX, nPosY );
	if( sInvenItem == NULL )
		return;

	GLPrivateMarket& sPMarket = pChar->m_sPMarket;
	if( sPMarket.IsOpen() )
		return;

	const SITEM* pItemData = GLItemMan::GetInstance().GetItem( sInvenItem->Id() );
	if( !pItemData )
		return;

	bool bPileItem = false;
	if( pItemData->ISPILE() )
	{
		if( sInvenItem->sItemCustom.wTurnNum > 1 )
			bPileItem = true;
	}

	m_bOpener = true;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_RegItem,
		"-b, -n, -n", bPileItem, nPosX, nPosY );
}

void PrivateMarketClient::ReqPrivateMarketRegItem( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum )
{
	m_pGaeaClient->GetCharacter()->ReqPMarketRegItem( wPosX, wPosY, llMoney, dwNum );
}

void PrivateMarketClient::ReqPrivateMarketDelItem( WORD wPosX, WORD wPosY )
{
	m_pGaeaClient->GetCharacter()->ReqPMarketDisItem( wPosX, wPosY );
}

void PrivateMarketClient::ReqPrivateMarketOpen( bool bOnlyUI )
{
	m_pGaeaClient->GetCharacter()->ReqPMarketOpen( bOnlyUI );
}

void PrivateMarketClient::ReqPMarketInfo( DWORD dwGaeaID )
{
	if( m_pGaeaClient->GetCharacter()->ReqPMarketInfo( dwGaeaID ) == S_OK )
	{
		m_bOpener = false;
		m_dwOpenerID = dwGaeaID;
	}
}

void PrivateMarketClient::ReqPrivateMarketClose()
{
	if( true == m_bOpener )
	{
		m_pGaeaClient->GetCharacter()->ReqPMarketClose();
	}
	else
	{
		if( m_pGaeaClient->GetCharacter()->ReqPMarketInfoRelease( m_dwOpenerID ) == S_OK )
		{
			m_bOpener = false;
			m_dwOpenerID = NATIVEID_NULL().dwID;
		}
	}
}

void PrivateMarketClient::MsgPrivateMarketOpenFb()
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_MsgOpenFb );
}

void PrivateMarketClient::ReqPMarketBuy( WORD wPosX, WORD wPosY )
{
	std::tr1::shared_ptr<GLCharClient> pClient = m_pGaeaClient->GetChar( m_dwOpenerID );
	if( !pClient )
		return;

	SNATIVEID sSalePos( wPosX, wPosY );
	const SSALEITEM* pSaleItem = pClient->m_sPMarket.GetItem( sSalePos );
	if( !pSaleItem )
		return;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pSaleItem->sITEMCUSTOM.GetNativeID() );
	if ( !pITEM )
		return;

	//	Note : 겹침이 가능하고 한개씩 파는 아이템의 경우 구입 수량 입력. ( 화살묶음, 부적묶음. )
	if ( pITEM->ISPILE() && (pITEM->GETAPPLYNUM()==1) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_ReqBuyEx,
			"-dw, -dw", wPosX, wPosY );
		return;
	}
	else
	{
		// 물건 사기
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_ReqBuy, 
			"-dw, -dw, -s", wPosX, wPosY, pITEM->GetName() );
		return;
	}
}

void PrivateMarketClient::ReqPMarketBuyItem( WORD wPosX, WORD wPosY, DWORD dwNum )
{
	if ( wPosX < 0 || wPosY < 0 ) return ;

	if( dwNum != 1 )
	{
		LONGLONG lnMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();

		std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( m_dwOpenerID );
		if ( pCLIENT == NULL )
			return;

		GLPrivateMarket& sPMarket = pCLIENT->m_sPMarket;

		const SSALEITEM* pSALE = sPMarket.GetItem( SNATIVEID( wPosX, wPosY ) ); 
		if( pSALE == NULL )
			return;

		const SITEM* pItem = GLogicData::GetInstance().GetItem(pSALE->sITEMCUSTOM.GetNativeID());
		if (!pItem)
			return;

		DWORD dwPrice = static_cast<DWORD>(pSALE->llPRICE);

		DWORD dwMaxCount = (DWORD)(lnMoney / (LONGLONG)dwPrice);
		if ( dwMaxCount < 1 )
		{					
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::ORANGE, ID2GAMEINTEXT("BUYITEM_NOMONEY") );
			return ;
		}
		//	버튼 만들어지는대로 넣기
		//	최대 개수로 사기
		if ( dwMaxCount < dwNum )
		{				
			std::string strText = sc::string::format(ID2GAMEINTEXT("BUYITEM_MAXIMUM"), pItem->GetName(), dwMaxCount).c_str();
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::ORANGE, strText.c_str());
			dwNum = dwMaxCount;
		}
	}

	m_pGaeaClient->GetCharacter()->ReqPMarketBuy ( m_dwOpenerID, wPosX, wPosY, (WORD)dwNum );
}

void PrivateMarketClient::UpdatePrivateMarketItemList( GLPrivateMarket& sPMarket, const DWORD dwChaDBNum, const bool bMe )
{
	LuaTable tbItemList( GLWidgetScript::GetInstance().GetLuaState() );

	GLPrivateMarket::MAPITEM_ITER pos = sPMarket.GetItemList().begin();
	GLPrivateMarket::MAPITEM_ITER end = sPMarket.GetItemList().end();

	int nItemCount = 1;
	for ( ; pos != end; ++pos )
	{
		const SSALEITEM* pSaleItem = &( (*pos).second );
		if( !pSaleItem )
			return;

		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, pSaleItem->sSALEPOS.wMainID );
		tbItem.set( 2, pSaleItem->sSALEPOS.wSubID );
		tbItem.set( 3, pSaleItem->bSOLD );
		tbItemList.set( nItemCount, tbItem );
		nItemCount++;
	}
	
	GLWidgetCallFuncValue args[ 3 ];
	args[ 0 ].SetTable( &tbItemList );
	args[ 1 ].SetBool( bMe );
	args[ 2 ].SetInt( dwChaDBNum );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_Update, args, 3 );
}

bool PrivateMarketClient::IsRegInven( WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if( true == pChar->m_sPMarket.IsRegInvenPos( SNATIVEID( wPosX, wPosY ) ) )
		return true;

	SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return false;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItem )
		return false;

	// 겹침 가능일 경우 동일한 종류의 아이템이 이미 등록되어 있는지 검사.
	if( false == pItem->ISPILE() )
		return false;

	return pChar->m_sPMarket.IsRegItem( pInvenItem->GetNativeID() );
}

bool PrivateMarketClient::IsSold( WORD wPosX, WORD wPosY )
{
	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	return pChar->m_sPMarket.IsSoldPos( SNATIVEID( wPosX, wPosY ) );
}
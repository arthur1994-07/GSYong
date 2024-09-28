#include "pch.h"
#include <shlwapi.h>
#include <shlobj.h>

#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/DebugSet.h"
#include "../SigmaCore/Encrypt/CompByte.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/Common/gltexfile.h"

#include "./Input/GLInputManager.h"

#include "../RanLogic/GLogicFile.h"

#include "./RanParamClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace RANPARAM
{ 

void INIT_GAMEOPT()
{
	bExchangeCheck		= true;
	bDIS_CONFT			= FALSE;		//	'자동' 대련 거부.
	bDIS_TRADE			= FALSE;		//	'자동' 거래 거부.
	bDIS_PARTY			= FALSE;		//	'자동' 파티 거부.
	bDIS_FRIEND			= FALSE;		//	'자동' 친구 요청 거부.
	bDIS_VEHICLE		= FALSE;		//	'자동' 탑승 거부
	bSHOW_SIMPLEHP		= TRUE;
	b3D_SOUND			= FALSE;
	bSHOW_TIP			= TRUE;			//	팁 보여주기
	bMOVABLE_ON_CHAT	= FALSE;		//	채팅창 위에서 움직이기
	bFORCED_ATTACK		= FALSE;			//	강제 공격 - 중국
	bNAME_DISPLAY		= TRUE	;		//	이름 항상 표시
	bNON_Rebirth		= FALSE;		//  부활 스킬 금지
	bSIMPLE_SUM_NAME	= FALSE;		//  소환수 이름 간략하게 표시 여부
	bAUTO_CROW_HP	    = TRUE;		    //  자신을 타겟팅한 몬스터 HP바 자동표시
	bSKILL_RESERVATION	= TRUE;			//  스킬 예약 bjju.temp
	bCharInfoPrivate	= FALSE;		//	내 정보 비공개
	bCharInfoAddition	= TRUE;			//	캐릭터 추가 정보창 표시
	bGroupChatInvite	= FALSE;		//	'자동'그룹채팅 거부
	emAroundDetail		= EM_AROUND_ORIG;			//  주변 캐릭터 단순화 등급
	emSkillTypeUI		= EM_SKILLTYPE_PARTY;		//	스킬 타입 UI

	bCONFT_BOUND		= TRUE;			//	대련시 영역 제한 여부.
	wCONFT_RECOVER		= 0;			//	대련시 회복약 제한.
	fCONFT_HP_RATE		= 1.0f;			//	대련시 체력 확대 비율.
	fCONFT_TAR_HP_RATE	= 1.0f;			//	대련시 체력 확대 비율 [ 상대방(개인대련시만) ].

	dwPARTY_GET_ITEM	= 3;			//	파티옵션 아이템 분배 규칙.(디폴트는 순차습득)
	dwPARTY_GET_MONEY	= 3;			//	파티옵션 돈 분배 규칙.(디폴트는 균등분배)

	bCheckAutoRebirth = false;          // 부활시 부활하기로 부활 체크.

    bCheckUsableItemMarketPage = true;      // NPC 상점 ui에서 사용 가능한 아이템 체크.
    bCheckBuyableItemMarketPage =  true;    // NPC 상점 ui에서 구입 가능한 아이템 체크.
    bCheckRebuySuppliesRebuyPage = true; // NPC 재매입 ui에서 소모품 아이템 체크.
    bCheckAllDelItemsRebuyPage = true; // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
    bCheckSellItemRButton = false; // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
    bCheckConfirmSellItem = true; // NPC 상점 ui에서 아이템 판매 시 확인.

    dwCharacterTitleIndex = TITLE_INDEX_NONE; // 캐릭터 뱃지(타이틀) 설정값 등록.

	int i=0; 

	for( i = 0; i < QUICK_SLOT_NUM; ++i ) {
		QuickSlot[i]  =	DefaultShotcutKey[i];
	}

	for( i = 0; i < QUICK_SKILL_NUM; ++i ) {
		SkillSlot[i]  =	DefaultShotcutKey[i+QUICK_SLOT_NUM];
	}

	for( i = 0; i < QUICK_MENU_NUM; ++i ) {
		MenuShotcut[i] = DefaultShotcutKey[i+QUICK_SLOT_NUM+QUICK_SKILL_NUM];
	}

	for( i = 0; i < CHAT_MACRO_NUM; ++i ) {
		ChatMacro[i] = "";
	}

	for( i = 0; i < MAX_QUEST_DISPLAY; ++i ) {
		QuestDisplayID[i] = -1;
	}

	for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
		strPointShopSearchWord[i] = "";
	}

	for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
		dwPointShopWishList[ i ] = -1; // = dwPointShopWishList[ i ] = NATIVEID_NULL().dwID;
	}
}

BOOL LOAD_GAMEOPT(const SUBPATH* pPath,const std::string& charName)
{
	INIT_GAMEOPT();
	std::string strUSERID = RANPARAM::strENC_CHARID = charName;//GETCHARID(); // 파일명 가져오는 부분...
	if ( strUSERID.empty() )
	{
		MessageBox ( NULL, "USERID가 지정되어 있지 않아 LOAD_GAMEOPT() 수행불가.", "ERROR", MB_OK );
		return FALSE;
	}

	TCHAR szPROFILE[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	std::string strOPTFILE(szPROFILE);
	strOPTFILE += pPath->PlayInfoRoot(); //SUBPATH::PLAYINFO_ROOT;
	strOPTFILE += sc::string::format("%d_", dwServerNum);
	strOPTFILE += strUSERID;
	strOPTFILE += _T(".gameopt");

	gltexfile cFILE;
	cFILE.reg_sep("\t");
	//		cFILE.reg_sep ( ' ' );

	// 
	if (!cFILE.open(strOPTFILE, false))
	{
		// Note : 없다면 옛날 형식으로 찾아본다.
		std::string strOPTFILE_OLD(szPROFILE);
		strOPTFILE_OLD += pPath->PlayInfoRoot(); //SUBPATH::PLAYINFO_ROOT;
		strOPTFILE_OLD += strUSERID;
		strOPTFILE_OLD += _T(".gameopt");

		if ( !cFILE.open(strOPTFILE_OLD,false) )
		{
			return FALSE;
		}
	}

	DWORD dwVER;
	bool bok = cFILE.getflag( "EMGAMEOPT_VER", 1, 1, dwVER );
	if ( !bok )
        return FALSE;

	if ( dwVER == EMGAMEOPT_VER )	// UI 안티 앨리어싱, 이름/데미지 디스플레이 개수 제한 옵션 추가
	{
		cFILE.getflag( "bExchangeCheck", 1, 1, bExchangeCheck);
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bCharInfoAddition", 1, 1, bCharInfoAddition );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// 이름 보기 설정
		cFILE.getflag( "dwNameDisplay", 1, 1, dwNameDisplay );

		// 다른 유저 데미지 보기
		cFILE.getflag( "bShowOtherDamage", 1, 1, bShowOtherDamage );

		// UI 안티 앨리어싱
		cFILE.getflag( "nUIAntiAliasing", 1, 1, nUIAntiAliasing );
		// 이름/데미지 디스플레이 개수 제한
		cFILE.getflag( "bDisplayLimit", 1, 1, bDisplayLimit );

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
	else if ( dwVER == 0x0131 )	// 다른 유저 데미지 보기 설정 추가
	{
		cFILE.getflag( "bExchangeCheck", 1, 1, bExchangeCheck);
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bCharInfoAddition", 1, 1, bCharInfoAddition );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// 이름 보기 설정
		cFILE.getflag( "dwNameDisplay", 1, 1, dwNameDisplay );

		// 다른 유저 데미지 보기
		cFILE.getflag( "bShowOtherDamage", 1, 1, bShowOtherDamage );

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
	else if ( dwVER == 0x0130 ) // 이름 보기 설정 추가
	{
		cFILE.getflag( "bExchangeCheck", 1, 1, bExchangeCheck);
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bCharInfoAddition", 1, 1, bCharInfoAddition );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// 이름 보기 설정
		cFILE.getflag( "dwNameDisplay", 1, 1, dwNameDisplay );

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
	else if ( dwVER == 0x0129 ) // 캐릭터 추가정보창 보기 추가
	{
		cFILE.getflag( "bExchangeCheck", 1, 1, bExchangeCheck);
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bCharInfoAddition", 1, 1, bCharInfoAddition );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
	else if ( dwVER == 0x128 ) // 캐릭터 추가정보창 보기 추가
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bCharInfoAddition", 1, 1, bCharInfoAddition );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
	else if ( dwVER == 0x127 ) // 그룹채팅 거부 옵션 추가
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); 
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );
		cFILE.getflag( "bGroupChatInvite", 1, 1, bGroupChatInvite );

		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// NPC 상점 ui에서 사용 가능한 아이템 체크.
		cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
		// NPC 상점 ui에서 구입 가능한 아이템 체크.
		cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
		// NPC 재매입 ui에서 소모품 아이템 체크.
		cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
		// NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
		cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
		// NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
		cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
		// NPC 상점 ui에서 아이템 판매 시 확인.
		cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

		// 캐릭터 뱃지(타이틀) 설정값 등록.
		cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt(cFILE);
	}
    else if ( dwVER == 0x126  ) // 단축키의 세이브 로드가 되지 않는 문제 수정
    {
        cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
        cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
        cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
        cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
        cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
        cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
        cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
        cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
        cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
        cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
        cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
        cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
        cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
        cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); // 추가
        cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );

        int nTemp(0);
        cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
        emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
        if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

        cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
        if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
        emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

        cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
        cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
        cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
        cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

        cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
        cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

        CString strTemp;
        int i = 0;

        // SNS Setting Load
        // FACEBOOK -------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
        cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
        cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
        cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

        // TWITTER --------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
        cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
        cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
        cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

        // QUEST_DISPLAY
        for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
            strTemp.Format("QUEST_DISPLAY%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
        }

        // POINT_SHOP_SEARCH_WORD
        for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
            strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
        }

        // POINT_SHOP_WISH_LIST
        for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
            strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
        }

        // 부활시 부활하기로 부활 체크.
        cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

        // NPC 상점 ui에서 사용 가능한 아이템 체크.
        cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
        // NPC 상점 ui에서 구입 가능한 아이템 체크.
        cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
        // NPC 재매입 ui에서 소모품 아이템 체크.
        cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
        // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
        cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
        // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
        cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
        // NPC 상점 ui에서 아이템 판매 시 확인.
        cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

        // 캐릭터 뱃지(타이틀) 설정값 등록.
        cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

        // 채팅 매크로
        for (i = 0; i< CHAT_MACRO_NUM; ++i){
            strTemp.Format("CHATMACRO%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
        }

        // SHORTCUT KEY SETTING
        GLInputManager::GetInstance().LoadOpt(cFILE);
    }
	else if ( dwVER == 0x125 ) // 내 정보 비공개 추가
    {
        cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
        cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
        cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
        cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
        cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
        cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
        cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
        cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
        cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
        cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
        cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
        cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
        cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
        cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); // 추가
		cFILE.getflag( "bCharInfoPrivate", 1, 1, bCharInfoPrivate );

        int nTemp(0);
        cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
        emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
        if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

        cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
        if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
        emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

        cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
        cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
        cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
        cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

        cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
        cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

        CString strTemp;
        int i = 0;

        // SNS Setting Load
        // FACEBOOK -------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
        cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
        cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
        cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

        // TWITTER --------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
        cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
        cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
        cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

        // QUEST_DISPLAY
        for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
            strTemp.Format("QUEST_DISPLAY%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
        }

        // POINT_SHOP_SEARCH_WORD
        for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
            strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
        }

        // POINT_SHOP_WISH_LIST
        for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
            strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
        }

        // 부활시 부활하기로 부활 체크.
        cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

        // NPC 상점 ui에서 사용 가능한 아이템 체크.
        cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
        // NPC 상점 ui에서 구입 가능한 아이템 체크.
        cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
        // NPC 재매입 ui에서 소모품 아이템 체크.
        cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
        // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
        cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
        // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
        cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
        // NPC 상점 ui에서 아이템 판매 시 확인.
        cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

        // 캐릭터 뱃지(타이틀) 설정값 등록.
        cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

        // 채팅 매크로
        for (i = 0; i< CHAT_MACRO_NUM; ++i){
            strTemp.Format("CHATMACRO%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
        }

        // SHORTCUT KEY SETTING
        GLInputManager::GetInstance().LoadOpt120(cFILE);
    }
    else if ( dwVER == 0x0124 ) // 캐릭터 뱃지(타이틀) 설정값 추가 버전.
    {
        cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
        cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
        cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
        cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
        cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
        cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
        cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
        cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
        cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
        cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
        cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
        cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
        cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
        cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); // 추가

        int nTemp(0);
        cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
        emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
        if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

        cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
        if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
        emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

        cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
        cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
        cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
        cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

        cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
        cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

        CString strTemp;
        int i = 0;

        // SNS Setting Load
        // FACEBOOK -------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
        cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
        cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
        cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

        // TWITTER --------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
        cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
        cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
        cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

        // QUEST_DISPLAY
        for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
            strTemp.Format("QUEST_DISPLAY%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
        }

        // POINT_SHOP_SEARCH_WORD
        for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
            strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
        }

        // POINT_SHOP_WISH_LIST
        for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
            strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
        }

        // 부활시 부활하기로 부활 체크.
        cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

        // NPC 상점 ui에서 사용 가능한 아이템 체크.
        cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
        // NPC 상점 ui에서 구입 가능한 아이템 체크.
        cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
        // NPC 재매입 ui에서 소모품 아이템 체크.
        cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
        // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
        cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
        // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
        cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
        // NPC 상점 ui에서 아이템 판매 시 확인.
        cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

        // 캐릭터 뱃지(타이틀) 설정값 등록.
        cFILE.getflag( "dwCharacterTitleIndex", 1, 1, dwCharacterTitleIndex );

        // 채팅 매크로
        for (i = 0; i< CHAT_MACRO_NUM; ++i){
            strTemp.Format("CHATMACRO%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
        }

        // SHORTCUT KEY SETTING
        GLInputManager::GetInstance().LoadOpt120(cFILE);
    }
	else if ( dwVER == 0x0123 ) // NPC 거래 ui 옵션 추가 버전.
	{
        cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
        cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
        cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
        cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
        cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
        cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
        cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
        cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
        cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
        cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
        cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
        cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
        cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
        cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); // 추가

        int nTemp(0);
        cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
        emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
        if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

        cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
        if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
        emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

        cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
        cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
        cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
        cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

        cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
        cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

        CString strTemp;
        int i = 0;

        // SNS Setting Load
        // FACEBOOK -------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
        cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
        cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
        cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

        // TWITTER --------------------------------------------------------------------------------------
        cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
        cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
        cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
        cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

        // QUEST_DISPLAY
        for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
            strTemp.Format("QUEST_DISPLAY%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
        }

        // POINT_SHOP_SEARCH_WORD
        for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
            strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
        }

        // POINT_SHOP_WISH_LIST
        for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
            strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
        }

        // 부활시 부활하기로 부활 체크.
        cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

        // NPC 상점 ui에서 사용 가능한 아이템 체크.
        cFILE.getflag( "bCheckUsableItemMarketPage", 1, 1, bCheckUsableItemMarketPage );
        // NPC 상점 ui에서 구입 가능한 아이템 체크.
        cFILE.getflag( "bCheckBuyableItemMarketPage", 1, 1, bCheckBuyableItemMarketPage );
        // NPC 재매입 ui에서 소모품 아이템 체크.
        cFILE.getflag( "bCheckRebuySuppliesRebuyPage", 1, 1, bCheckRebuySuppliesRebuyPage );
        // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
        cFILE.getflag( "bCheckAllDelItemsRebuyPage", 1, 1, bCheckAllDelItemsRebuyPage );
        // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
        cFILE.getflag( "bCheckSellItemRButton", 1, 1, bCheckSellItemRButton );
        // NPC 상점 ui에서 아이템 판매 시 확인.
        cFILE.getflag( "bCheckConfirmSellItem", 1, 1, bCheckConfirmSellItem );

        // 채팅 매크로
        for (i = 0; i< CHAT_MACRO_NUM; ++i){
            strTemp.Format("CHATMACRO%d", i);
            cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
        }

        // SHORTCUT KEY SETTING
        GLInputManager::GetInstance().LoadOpt120(cFILE);
    }
    else if ( dwVER == 0x0122 ) // 컨트롤 타입 추가 버전
    {
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		cFILE.getflag( "bSKILL_RESERVATION", 1, 1, bSKILL_RESERVATION ); // 추가
		
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}

		// SHORTCUT KEY SETTING
		GLInputManager::GetInstance().LoadOpt120(cFILE);
	}
	else if ( dwVER == 0x0121 ) // 몬스터 HP바 자동표시 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		cFILE.getflag("SHOTCUT_POINT_SHOP", 1, 1, MenuShotcut[SHOTCUT_POINT_SHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 부활시 부활하기로 부활 체크.
		cFILE.getflag( "bCheckAutoRebirth", 1, 1, bCheckAutoRebirth );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	else if ( dwVER == 0x0120 ) // 몬스터 HP바 자동표시 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		cFILE.getflag( "bAUTO_CROW_HP", 1, 1, bAUTO_CROW_HP );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		cFILE.getflag("SHOTCUT_POINT_SHOP", 1, 1, MenuShotcut[SHOTCUT_POINT_SHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	else if ( dwVER == 0x0119 ) // 포인트샵 추가버전
	{

		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		cFILE.getflag("SHOTCUT_POINT_SHOP", 1, 1, MenuShotcut[SHOTCUT_POINT_SHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// POINT_SHOP_WISH_LIST
		for ( i = 0; i < MAX_POINT_SHOP_WISH_LIST; ++i ) {
			strTemp.Format("POINT_SHOP_WISH_LIST%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, dwPointShopWishList[ i ] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0118 ) // 포인트샵 추가버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		cFILE.getflag("SHOTCUT_POINT_SHOP", 1, 1, MenuShotcut[SHOTCUT_POINT_SHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// POINT_SHOP_SEARCH_WORD
		for ( i = 0; i < MAX_SEARCH_WORD; ++i ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d", i );
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, strPointShopSearchWord[i] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0117 ) // SNS 추가버전
	{           
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0116 ) // SNS 추가버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );
		cFILE.getflag("SHOTCUT_SNS", 1, 1, MenuShotcut[SHOTCUT_SNS] );

		// SNS Setting Load
		// FACEBOOK -------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_FACEBOOK_bAUTHEXPIRE",		1, 1, sSNSFaceBook.bAUTHEXPIRE );
		cFILE.getflag( "SNS_FACEBOOK_bLEVELUP",		1, 1, sSNSFaceBook.bLEVELUP );
		cFILE.getflag( "SNS_FACEBOOK_bGRINDING",		1, 1, sSNSFaceBook.bGRINDING );
		cFILE.getflag( "SNS_FACEBOOK_bSTART",			1, 1, sSNSFaceBook.bSTART );

		// TWITTER --------------------------------------------------------------------------------------
		cFILE.getflag( "SNS_TWITTER_bAUTHEXPIRE",		1, 1, sSNSTwitter.bAUTHEXPIRE );
		cFILE.getflag( "SNS_TWITTER_bLEVELUP",			1, 1, sSNSTwitter.bLEVELUP );
		cFILE.getflag( "SNS_TWITTER_bGRINDING",		1, 1, sSNSTwitter.bGRINDING );
		cFILE.getflag( "SNS_TWITTER_bSTART",			1, 1, sSNSTwitter.bSTART );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0115 )
	{
		// 퀘스트 디스플레이 추가버전
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );

		// QUEST_DISPLAY
		for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
			strTemp.Format("QUEST_DISPLAY%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuestDisplayID[i] );
		}

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0114 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );
		cFILE.getflag( "bDIS_VEHICLE", 1, 1, bDIS_VEHICLE );
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if ( dwVER == 0x0113 ) 
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;


		cFILE.getflag( "emSkillTypeUI", 1, 1, nTemp );
		if ( nTemp < 0  || nTemp >= EM_SKILLTYPE_SIZE ) nTemp = 0 ;
		emSkillTypeUI = (EM_SKILLTYPE_UI)nTemp;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	else if( dwVER == 0x0112 ) // 소환수 이름 단순화 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		cFILE.getflag( "bSIMPLE_SUM_NAME", 1, 1, bSIMPLE_SUM_NAME );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	else if( dwVER == 0x0111 ) // 채팅 매크로 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );
		cFILE.getflag("SHOTCUT_QBOX", 1, 1, MenuShotcut[SHOTCUT_QBOX] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}
	}
	if( dwVER == 0x0110 ) // 채팅 매크로 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );
		cFILE.getflag( "bNON_Rebirth", 1, 1, bNON_Rebirth );
		int nTemp(0);
		cFILE.getflag( "emAroundDetail", 1, 1, nTemp );
		emAroundDetail = (EM_AROUND_DETAILTYPE)nTemp;
		if( emAroundDetail >= EM_AROUND_SIZE ) emAroundDetail = EM_AROUND_ONLYNAME;

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}


	}
	if( dwVER == 0x0109 ) // 채팅 매크로 버전
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}


		// MENU_SHOTCUT
		cFILE.getflag("SHOTCUT_INVEN", 1, 1, MenuShotcut[SHOTCUT_INVEN] );
		cFILE.getflag("SHOTCUT_CHAR", 1, 1, MenuShotcut[SHOTCUT_CHAR] );
		cFILE.getflag("SHOTCUT_SKILL", 1, 1, MenuShotcut[SHOTCUT_SKILL] );
		cFILE.getflag("SHOTCUT_PARTY", 1, 1, MenuShotcut[SHOTCUT_PARTY] );
		cFILE.getflag("SHOTCUT_QUEST", 1, 1, MenuShotcut[SHOTCUT_QUEST] );
		cFILE.getflag("SHOTCUT_CLUB", 1, 1, MenuShotcut[SHOTCUT_CLUB] );
		cFILE.getflag("SHOTCUT_FRIEND", 1, 1, MenuShotcut[SHOTCUT_FRIEND] );
		cFILE.getflag("SHOTCUT_MAP", 1, 1, MenuShotcut[SHOTCUT_MAP] );
		cFILE.getflag("SHOTCUT_ITEMBANK", 1, 1, MenuShotcut[SHOTCUT_ITEMBANK] );
		cFILE.getflag("SHOTCUT_RUN", 1, 1, MenuShotcut[SHOTCUT_RUN] );
		cFILE.getflag("SHOTCUT_HELP", 1, 1, MenuShotcut[SHOTCUT_HELP] );
		cFILE.getflag("SHOTCUT_PET", 1, 1, MenuShotcut[SHOTCUT_PET] );
		cFILE.getflag("SHOTCUT_ATTACKMODE", 1, 1, MenuShotcut[SHOTCUT_ATTACKMODE] );
		cFILE.getflag("SHOTCUT_PKMODE", 1, 1, MenuShotcut[SHOTCUT_PKMODE] );
		//cFILE.getflag("SHOTCUT_ITEMSHOP", 1, 1, MenuShotcut[SHOTCUT_ITEMSHOP] );
		cFILE.getflag("SHOTCUT_CHATMACRO", 1, 1, MenuShotcut[SHOTCUT_CHATMACRO] );

		// 채팅 매크로
		for (i = 0; i< CHAT_MACRO_NUM; ++i){
			strTemp.Format("CHATMACRO%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 2, 2, ChatMacro[i] );
		}


	}
	else if ( dwVER==0x0108 ) // 단축키 저장 버전
	{ 
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );

		CString strTemp;
		int i = 0;

		// QUICK_POTION_SHOTCUT
		for (i = 0; i< QUICK_SLOT_NUM; ++i){
			strTemp.Format("QUICK_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, QuickSlot[i] );
		}

		// QUICK_SKILL_SHOTCUT
		for (i = 0; i< QUICK_SKILL_NUM; ++i){
			strTemp.Format("SKILL_SLOT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, SkillSlot[i] );
		}

		// QUICK_MENU_SHOTCUT
		for (i = 0; i< QUICK_MENU_NUM; ++i){
			strTemp.Format("MENU_SHOTCUT%d", i);
			cFILE.getflag( (LPCTSTR)strTemp, 1, 1, MenuShotcut[i] );
		}
	}
	else if ( dwVER==0x0107 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );
		cFILE.getflag( "bFORCED_ATTACK", 1, 1, bFORCED_ATTACK );
		cFILE.getflag( "bNAME_DISPLAY", 1, 1, bNAME_DISPLAY );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}
	else if ( dwVER==0x0106 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );
		cFILE.getflag( "bSHOW_TIP", 1, 1, bSHOW_TIP );
		cFILE.getflag( "bMOVABLE_ON_CHAT", 1, 1, bMOVABLE_ON_CHAT );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}
	else if ( dwVER==0x0105 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );		
		cFILE.getflag( "bSHOW_SIMPLEHP", 1, 1, bSHOW_SIMPLEHP );		

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}
	else if ( dwVER==0x0104 || dwVER==0x0103 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );
		cFILE.getflag( "bDIS_FRIEND", 1, 1, bDIS_FRIEND );			

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}
	else if ( dwVER==0x0102 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}
	else if ( dwVER==0x0101 )
	{
		cFILE.getflag( "bDIS_CONFT", 1, 1, bDIS_CONFT );
		cFILE.getflag( "bDIS_TRADE", 1, 1, bDIS_TRADE );
		cFILE.getflag( "bDIS_PARTY", 1, 1, bDIS_PARTY );

		cFILE.getflag( "bCONFT_BOUND", 1, 1, bCONFT_BOUND );
		cFILE.getflag( "wCONFT_RECOVER", 1, 1, wCONFT_RECOVER );
		cFILE.getflag( "fCONFT_HP_RATE", 1, 1, fCONFT_HP_RATE );
		cFILE.getflag( "fCONFT_TAR_HP_RATE", 1, 1, fCONFT_TAR_HP_RATE );

		cFILE.getflag( "dwPARTY_GET_ITEM", 1, 1, dwPARTY_GET_ITEM );
		cFILE.getflag( "dwPARTY_GET_MONEY", 1, 1, dwPARTY_GET_MONEY );
	}

	return TRUE;
}

BOOL SAVE_GAMEOPT(const SUBPATH* pPath)
{
	std::string strUSERID = strENC_CHARID;//GETCHARID(); // 파일명 가져오는 부분...
	if ( strUSERID.empty() )
	{
		//MessageBox ( NULL, "USERID가 지정되어 있지 않아 SAVE_GAMEOPT() 수행불가.", "ERROR", MB_OK );
		return FALSE;
	} 

	TCHAR szPROFILE[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	std::string strOPTFILE(szPROFILE);
	strOPTFILE += pPath->PlayInfoRoot(); //SUBPATH::PLAYINFO_ROOT;
	strOPTFILE += sc::string::format("%d_", dwServerNum);
	strOPTFILE += strUSERID;
	strOPTFILE += _T(".gameopt");

	FILE* file=NULL;
	file = _fsopen ( strOPTFILE.c_str(), "wt", _SH_DENYNO );
	if ( !file )	return FALSE;

	fprintf( file, "\n" );
	fprintf( file, "// GAME OPTION\n" );

	fprintf( file, "EMGAMEOPT_VER\t%d\n", EMGAMEOPT_VER );
	fprintf( file, "bExchangeCheck\t%d\n", bExchangeCheck );
	fprintf( file, "bDIS_CONFT\t%d\n", bDIS_CONFT );
	fprintf( file, "bDIS_TRADE\t%d\n", bDIS_TRADE );
	fprintf( file, "bDIS_PARTY\t%d\n", bDIS_PARTY );
	fprintf( file, "bDIS_FRIEND\t%d\n", bDIS_FRIEND );	
	fprintf( file, "bDIS_VEHICLE\t%d\n", bDIS_VEHICLE );		
	fprintf( file, "bSHOW_SIMPLEHP\t%d\n", bSHOW_SIMPLEHP );	
	fprintf( file, "bSHOW_TIP\t%d\n", bSHOW_TIP );
	fprintf( file, "bMOVABLE_ON_CHAT\t%d\n", bMOVABLE_ON_CHAT );
	fprintf( file, "bFORCED_ATTACK\t%d\n", bFORCED_ATTACK );
	fprintf( file, "bNAME_DISPLAY\t%d\n", bNAME_DISPLAY );
	fprintf( file, "bNON_Rebirth\t%d\n", bNON_Rebirth );
	fprintf( file, "bSIMPLE_SUM_NAME\t%d\n", bSIMPLE_SUM_NAME );
	fprintf( file, "bAUTO_CROW_HP\t%d\n", bAUTO_CROW_HP );
	fprintf( file, "bSKILL_RESERVATION\t%d\n", bSKILL_RESERVATION );
	fprintf( file, "bCharInfoPrivate\t%d\n", bCharInfoPrivate );
	fprintf( file, "bCharInfoAddition\t%d\n", bCharInfoAddition );
	fprintf( file, "bGroupChatInvite\t%d\n", bGroupChatInvite );
	
	int nTemp = (int)emAroundDetail;
	fprintf( file, "emAroundDetail\t%d\n", nTemp );

	nTemp = (int)emSkillTypeUI;
	fprintf( file, "emSkillTypeUI\t%d\n", nTemp );


	fprintf( file, "bCONFT_BOUND\t%d\n", bCONFT_BOUND );
	fprintf( file, "wCONFT_RECOVER\t%d\n", wCONFT_RECOVER );
	fprintf( file, "fCONFT_HP_RATE\t%1.1f\n", fCONFT_HP_RATE );
	fprintf( file, "fCONFT_TAR_HP_RATE\t%1.1f\n", fCONFT_TAR_HP_RATE );

	fprintf( file, "dwPARTY_GET_ITEM\t%d\n", dwPARTY_GET_ITEM );
	fprintf( file, "dwPARTY_GET_MONEY\t%d\n", dwPARTY_GET_MONEY );


	CString strTemp;
	int i=0;

	// MENU_SHOTCUT
	fprintf( file, "SHOTCUT_INVEN\t%d\n", MenuShotcut[SHOTCUT_INVEN] );
	fprintf( file, "SHOTCUT_CHAR\t%d\n", MenuShotcut[SHOTCUT_CHAR] );
	fprintf( file, "SHOTCUT_SKILL\t%d\n", MenuShotcut[SHOTCUT_SKILL] );
	fprintf( file, "SHOTCUT_PARTY\t%d\n", MenuShotcut[SHOTCUT_PARTY] );
	fprintf( file, "SHOTCUT_QUEST\t%d\n", MenuShotcut[SHOTCUT_QUEST] );
	fprintf( file, "SHOTCUT_CLUB\t%d\n", MenuShotcut[SHOTCUT_CLUB] );
	fprintf( file, "SHOTCUT_FRIEND\t%d\n", MenuShotcut[SHOTCUT_FRIEND] );
	fprintf( file, "SHOTCUT_MAP\t%d\n", MenuShotcut[SHOTCUT_MAP] );
	fprintf( file, "SHOTCUT_ITEMBANK\t%d\n", MenuShotcut[SHOTCUT_ITEMBANK] );
	fprintf( file, "SHOTCUT_RUN\t%d\n", MenuShotcut[SHOTCUT_RUN] );
	fprintf( file, "SHOTCUT_HELP\t%d\n", MenuShotcut[SHOTCUT_HELP] );
	fprintf( file, "SHOTCUT_PET\t%d\n", MenuShotcut[SHOTCUT_PET] );
	fprintf( file, "SHOTCUT_ATTACKMODE\t%d\n", MenuShotcut[SHOTCUT_ATTACKMODE] );
	fprintf( file, "SHOTCUT_PKMODE\t%d\n", MenuShotcut[SHOTCUT_PKMODE] );
	fprintf( file, "SHOTCUT_POINT_SHOP\t%d\n", MenuShotcut[SHOTCUT_POINT_SHOP] );
	fprintf( file, "SHOTCUT_CHATMACRO\t%d\n", MenuShotcut[SHOTCUT_CHATMACRO] );
	fprintf( file, "SHOTCUT_QBOX\t%d\n", MenuShotcut[SHOTCUT_QBOX] );
	fprintf( file, "SHOTCUT_SNS\t%d\n", MenuShotcut[SHOTCUT_SNS] );

	// SNS Setting Save

	// FACEBOOK -------------------------------------------------------------------------------------
	fprintf( file, "SNS_FACEBOOK_bAUTHEXPIRE\t%d\n",	sSNSFaceBook.bAUTHEXPIRE );
	fprintf( file, "SNS_FACEBOOK_bLEVELUP\t%d\n",		sSNSFaceBook.bLEVELUP );
	fprintf( file, "SNS_FACEBOOK_bGRINDING\t%d\n",	    sSNSFaceBook.bGRINDING );
	fprintf( file, "SNS_FACEBOOK_bSTART\t%d\n",		sSNSFaceBook.bSTART );

	// TWITTER --------------------------------------------------------------------------------------
	fprintf( file, "SNS_TWITTER_bAUTHEXPIRE\t%d\n",	sSNSTwitter.bAUTHEXPIRE );
	fprintf( file, "SNS_TWITTER_bLEVELUP\t%d\n",		sSNSTwitter.bLEVELUP );
	fprintf( file, "SNS_TWITTER_bGRINDING\t%d\n",		sSNSTwitter.bGRINDING );
	fprintf( file, "SNS_TWITTER_bSTART\t%d\n",		    sSNSTwitter.bSTART );

	// QUEST_DISPLAY
	for (i = 0; i< MAX_QUEST_DISPLAY; ++i){
		strTemp.Format("QUEST_DISPLAY%d\t%d\n", i, (int)QuestDisplayID[i]);
		fprintf( file, strTemp);
	}

	// POINT_SHOP_SEARCH_WORD
	for (i = 0; i< MAX_SEARCH_WORD; ++i){
		if ( strPointShopSearchWord[i].length() > 0 ) {
			strTemp.Format("POINT_SHOP_SEARCH_WORD%d\t=\t%s\n", i, strPointShopSearchWord[i].c_str() );
			fprintf( file, strTemp);
		}
	}

	// POINT_SHOP_WISH_LIST
	for (i = 0; i< MAX_POINT_SHOP_WISH_LIST; ++i){
		if ( dwPointShopWishList[ i ] != -1 )
		{
			strTemp.Format("POINT_SHOP_WISH_LIST%d\t%d\n", i, dwPointShopWishList[ i ] );
			fprintf( file, strTemp);
		}
	}

	// 부활시 부활하기로 부활 체크.
	fprintf( file, "bCheckAutoRebirth\t%d\n", bCheckAutoRebirth );

    // NPC 상점 ui에서 사용 가능한 아이템 체크.
    fprintf( file, "bCheckUsableItemMarketPage\t%d\n", bCheckUsableItemMarketPage );
    // NPC 상점 ui에서 구입 가능한 아이템 체크.
    fprintf( file, "bCheckBuyableItemMarketPage\t%d\n", bCheckBuyableItemMarketPage );
    // NPC 재매입 ui에서 소모품 아이템 체크.
    fprintf( file, "bCheckRebuySuppliesRebuyPage\t%d\n", bCheckRebuySuppliesRebuyPage );
    // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
    fprintf( file, "bCheckAllDelItemsRebuyPage\t%d\n", bCheckAllDelItemsRebuyPage );
    // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
    fprintf( file, "bCheckSellItemRButton\t%d\n", bCheckSellItemRButton );
    // NPC 상점 ui에서 아이템 판매 시 확인.
    fprintf( file, "bCheckConfirmSellItem\t%d\n", bCheckConfirmSellItem );

    // 캐릭터 뱃지(타이틀) 설정값 등록.
    fprintf( file, "dwCharacterTitleIndex\t%d\n", dwCharacterTitleIndex );

	// 채팅 매크로
	CString strMacro;
	for (i=0; i< CHAT_MACRO_NUM; ++i){
		if(ChatMacro[i].GetLength() > 0){
			strMacro = ChatMacro[i];
			strMacro.Replace("%", "%%");
			strTemp.Format("CHATMACRO%d\t=\t%s\n", i, strMacro);
			fprintf( file, strTemp);
		}
	}

	// 이름 보기 설정
	fprintf( file, "dwNameDisplay\t%d\n", dwNameDisplay );

	// 다른 유저 데미지 보기 설정
	fprintf( file, "bShowOtherDamage\t%d\n", bShowOtherDamage );

	// UI 안티 앨리어싱
	fprintf( file, "nUIAntiAliasing\t%d\n", nUIAntiAliasing );
	// 이름/데미지 디스플레이 개수 제한
	fprintf( file, "bDisplayLimit\t%d\n", bDisplayLimit );

	// SHORTCUT KEY SETTING
	GLInputManager::GetInstance().SaveOpt(file);

	fclose ( file );

	return TRUE;
}

} // namespace RANPARAM end
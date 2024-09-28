#ifndef _RAN_PARAM_H_
#define _RAN_PARAM_H_

#pragma once

#include <string>
#include "../EngineLib/DxCommon/d3dapp.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "./RanParamDefine.h"

#define STRING_NUM_128	128
#define	QUICK_SLOT_NUM	6
#define	QUICK_SKILL_NUM	10
#define	QUICK_MENU_NUM	18
#define	QUICK_ALL_NUM	(QUICK_SLOT_NUM + QUICK_SKILL_NUM + QUICK_MENU_NUM)
#define CHAT_MACRO_NUM	10
#define BASICMENU_NUM	13

class SUBPATH;
enum language::SERVICENATION;
enum language::EMPROVIDE_LANGFLAG;

namespace RANPARAM
{
	enum
	{ 
		MAX_FTP = 24, 
		EMGAMEOPT_VER = 0x0132,
		MAX_HTTP = 24,
		MAX_SERVER = 20,
		MAX_CHINA_REGION = 8,
		X_RESOLUTION = 1024,
        Y_RESOLUTION = 768,
        MAX_QUEST_DISPLAY = 5,
        MAX_SEARCH_WORD = 50,
        MAX_POINT_SHOP_WISH_LIST = 10,
	};

	enum ENameDisplay
	{
		None		= 0x00000000,
		CharAlly	= 0x00000001,	// 캐릭터 - 아군
		CharEnemy	= 0x00000002,	// 캐릭터 - 적군
		Mob			= 0x00000004,	// 몹
		NPC			= 0x00000008,	// NPC
		Summon		= 0x00000010,	// 소환수
		Item		= 0x00000020,	// 드랍 아이템

		All			= CharAlly | CharEnemy | Mob | NPC | Summon | Item
	};

	struct SERVER_SET
	{
		CString	strName;
		CString	LoginAddress;
		CString	WB_LoginAddress;
		CString	HttpAddressTable[MAX_HTTP];
		CString	strNewsURL;
		CString strImageURL;
		CString	ServerName[MAX_SERVER];
		CString strHomePage;
		CString strItemShop;
		CString strItemShopHP;
		CString strEventPage;
		CString strManualPath;
	};

    struct SNS_SET
    {
        BOOL bAUTHEXPIRE;
        BOOL bLEVELUP;
        BOOL bGRINDING;
        BOOL bSTART;

        SNS_SET ()
        {
            bAUTHEXPIRE  = TRUE;
            bLEVELUP	 = TRUE;
            bGRINDING	 = TRUE;
            bSTART		 = TRUE;
        }
    };

    struct SNSFACEBOOK_SET : public SNS_SET
    {
        //Blank
    };

    struct SNSTWITTER_SET : public SNS_SET
    {
        //Blank
    };

//	extern int				emSERVICE_TYPE;

	//	게임 설정.
	extern std::string		strENC_CHARID;
	extern BOOL				bSAVE_USERID;

	extern bool				bExchangeCheck;		//	'교환' 체크/비체크
	extern BOOL				bDIS_CONFT;			//	'자동' 대련 거부.
	extern BOOL				bDIS_TRADE;			//	'자동' 거래 거부.
	extern BOOL				bDIS_PARTY;			//	'자동' 파티 거부.
	extern BOOL				bDIS_FRIEND;
	extern BOOL				bDIS_VEHICLE;		//	'자동' 오토바이 탑승 거부.
	extern BOOL				bSHOW_SIMPLEHP;
	extern BOOL				b3D_SOUND;			//	3차원 사운드
	extern BOOL				bSHOW_TIP;			//	팁 보여주기
	extern BOOL				bMOVABLE_ON_CHAT;	//	채팅창 위에서 움직이기
	extern BOOL				bFORCED_ATTACK;		//	강제 공격
	extern BOOL				bNAME_DISPLAY;		//	이름 항상 표시, 선택 대상과 몹들만 출력한다;
	extern BOOL				bNON_Rebirth;		//	부활 스킬 금지
	extern BOOL				bSIMPLE_SUM_NAME;   //  소환수 이름 간략하게 표시 여부
	extern BOOL             bAUTO_CROW_HP;      //  자신을 타겟팅한 몬스터 HP바 자동표시
	extern BOOL				bCharInfoPrivate;	//	내 정보 비공개
	extern BOOL				bCharInfoAddition;	//	캐릭터 추가 정보창 표시
	extern BOOL				bSKILL_RESERVATION; //  스킬 예약
	extern BOOL				bGroupChatInvite; //  '자동' 그룹채팅 거부
	extern EM_AROUND_DETAILTYPE	emAroundDetail;		//  주변 캐릭터 단순화 등급
	extern EM_SKILLTYPE_UI	emSkillTypeUI;		//	스킬 타입 UI 표시


	extern BOOL				bCONFT_BOUND;		//	대련시 영역 제한 여부.
	extern WORD				wCONFT_RECOVER;		//	대련시 회복약 제한.
	extern float			fCONFT_HP_RATE;		//	대련시 체력 확대 비율.
	extern float			fCONFT_TAR_HP_RATE;	//	대련시 체력 확대 비율 [ 상대방(개인대련시만) ].

	extern DWORD			dwPARTY_GET_ITEM;	//	파티옵션 아이템 분배 규칙.
	extern DWORD			dwPARTY_GET_ITEM_Filter; // 파티 옵션 아이템 분배 규칙의 등급
	extern DWORD			dwPARTY_GET_MONEY;	//	파티옵션 돈 분배 규칙.
	extern DWORD			dwPARTY_SCHOOL; // 파티 옵션 구성 학원생;
	
	// (bjju) 컨트롤 개편으로 이제 사용하지 않습니다
	extern int				QuickSlot[QUICK_SLOT_NUM]; // 퀵슬롯
	extern int				SkillSlot[QUICK_SKILL_NUM]; // 퀵스킬
	extern int				MenuShotcut[QUICK_MENU_NUM]; // 메뉴단축키
	extern int				DefaultShotcutKey[QUICK_ALL_NUM]; // 디폴트 키 타입
	extern int				KeySettingToRanparam[QUICK_MENU_NUM];
	extern int				BasicMenuToRanparam[BASICMENU_NUM];

	extern CString			ChatMacro[CHAT_MACRO_NUM];

    extern DWORD			QuestDisplayID[MAX_QUEST_DISPLAY]; // 퀘스트 헬퍼 등록

    extern std::string      strPointShopSearchWord[MAX_SEARCH_WORD]; // 포인트 상점에서 찾아본 단어.
    extern DWORD            dwPointShopWishList[MAX_POINT_SHOP_WISH_LIST]; // 포인트 상점 쇼핑백 리스트.

    extern bool             bCheckAutoRebirth; // 부활시 부활하기로 부활 체크.

    extern bool             bCheckUsableItemMarketPage; // NPC 상점 ui에서 사용 가능한 아이템 체크.
    extern bool             bCheckBuyableItemMarketPage; // NPC 상점 ui에서 구입 가능한 아이템 체크.
    extern bool             bCheckRebuySuppliesRebuyPage; // NPC 재매입 ui에서 소모품 아이템 체크.
    extern bool             bCheckAllDelItemsRebuyPage; // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
    extern bool             bCheckSellItemRButton; // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
    extern bool             bCheckConfirmSellItem; // NPC 상점 ui에서 아이템 판매 시 확인.

    extern DWORD            dwCharacterTitleIndex; // 캐릭터 뱃지(타이틀) 설정값 등록.

	//	디스플레이 설정.
	extern BOOL				bCHECKWHQL;
	extern DWORD			dwScrWidth;
	extern DWORD			dwScrHeight;
	extern EMSCREEN_FORMAT	emScrFormat;
	extern UINT				uScrRefreshHz;
	extern BOOL				bScrWindowed;
	//extern BOOL				bScrWndHalfSize;
	extern BOOL				bScrWndFullSize;
	extern BOOL				bGameCursor;
	extern BOOL				bAllMinimum;

	extern bool				bDISP_CONSOLE;
	extern bool				bDISP_FPS;

	//	그래픽 설정.
	extern DWORD			dwVideoLevel;
	extern DWORD			dwShadowChar;	//	0~4
	extern BOOL				bBuff;
	//  [11/25/2016 gbgim];
	// 아래의 기능은 '월드 품질'로 편입됨, 매우낮음-false, esle true;
	extern BOOL				bShadowLand;
	extern BOOL				bRealReflect;
	extern BOOL				bRefract;
	extern BOOL				bGlow;
	extern BOOL				bPost;
	extern BOOL				bFrameLimit;
	extern BOOL				bTargetEffect;
	extern BOOL				bMineEffect;
	extern BOOL				bClickEffect;
	extern BOOL				bUseNormalMap;
	extern DWORD			dwSkinDetail;	// 0~2

	// 국가 선택;
	extern language::SERVICENATION	emNation;
	// SERVICENATION을 아래로 전환해야됨;
	extern language::EMPROVIDE_LANGFLAG emProvideLangFlag;

	// 노말맵 사용할수 있는지 여부
	extern BOOL				bPossibleNormalMap;

	extern BOOL				bBUFF_SFX;
	extern BOOL				bEMOTICON;

	//	시야
	extern DWORD			dwFogRange;

	extern LONG				nSndSfx;
	extern LONG				nSndMap;
	extern LONG				nSndMusic;
	extern DWORD			dw3DAlgorithm;
	extern DWORD			dwSndBufferSize;

	extern BOOL				bSndMuteSfx;
	extern BOOL				bSndMuteMap;
	extern BOOL				bSndMuteMusic;	

	//	서버 설정.
	extern TCHAR			LoginAddress[STRING_NUM_128];
    extern int              LoginServerPort;
	extern TCHAR			WB_LoginAddress[STRING_NUM_128];
    extern int              WB_LoginServerPort;
	extern TCHAR			FtpAddressTable[MAX_FTP][STRING_NUM_128];
	extern TCHAR			HttpAddressTable[MAX_HTTP][STRING_NUM_128];	
	extern CString			strNewsURL;
	extern CString			strImageURL;
	extern CString			strConnectionURL;
	extern CString			strForumURL;
	extern CString			strTWForumURL;
	extern BOOL				bUsePassiveDN;
	extern UINT				nChinaRegion; // 중국 - 패치서버 지역(MAX_CHINA_REGION:물어보기, 0~3:선택지역)
	extern TCHAR			ItemShopHPAddress[STRING_NUM_128];
	extern TCHAR			EventPageAddress[STRING_NUM_128];
	extern TCHAR			ManualPatchAddress[STRING_NUM_128];
	extern TCHAR			HomePageAddress[STRING_NUM_128];
    extern TCHAR			CustomerCenterAddress[STRING_NUM_128]; // 고객센터 주소.


	// 중국 지역에 따른 서버 설정
	extern SERVER_SET		China_Region[MAX_CHINA_REGION];

	//	언어 폰트 설정.
//	extern language::LANGFLAG emLangSet;

	extern CString			strGDIFont;
	extern CString			strFontType;

	//	기본 설정 파일. ( 언어별 txt )
	extern CString			strOriginalCountry;
	extern CString			strCountry;
	extern CString			strLauncherText;
	extern CString			strGameInText;
	extern CString			strGameExText;
	extern CString			strGameWord;
	extern CString			strServerText;
	extern CString			strSkillText;
	extern CString			strItemText;
	extern CString			strCrowText;
	extern CString			strTipText;
	extern CString			strCommentText;
	extern CString			strPetCommentText;
	extern CString			strQuestText;
	extern CString			strNPCTalkText;
	extern CString			strNPCAnswerText;
	extern CString			strAutoLevelSetText;

	extern CString			strUIOuterCfg;
	extern CString			strUIInnerCfg01;
	extern CString			strUIInnerCfg02;
	extern CString			strUIInnerCfg03;
	extern CString			strUIExtCfg;

    extern CString          strUITextureList;   // 새로운 UI 텍스처 관리
    extern CString          strUIXmlList;       // 새로운 UI 컨트롤 관리

    // 서버 인덱스
    extern DWORD		    dwServerNum;

    // SNS 설정
    extern SNSFACEBOOK_SET  sSNSFaceBook;
    extern SNSTWITTER_SET   sSNSTwitter;

	// 도움말 웹 페이지 주소
	extern TCHAR			HelpAddress[STRING_NUM_128];
	extern TCHAR			ItemShopAddress[STRING_NUM_128];
	extern TCHAR			RPFDataPath[STRING_NUM_128];

	//  [4/27/2015 gbgim];
	// 데이터 통합버전에서 도움말 UI 오픈시 사용하는 html;
	extern TCHAR			MultiHelpAddress[language::LANGFLAG_TOTAL_NUM][STRING_NUM_128];

	extern DWORD			dwNameDisplay;		// 이름 보기 설정
	extern bool				bShowOtherDamage;	// 다른 유저 데미지 보기
	extern int				nUIAntiAliasing;	// UI 안티 앨리어싱
	extern bool				bDisplayLimit;		// 이름, 데미지 디스플레이 개수 제한

	void SETIDPARAM(const std::string& strPARAM);
	void SETIDPARAM2(const std::string& strPARAM);
	void SETIDPARAM3(const std::string& strPARAM);
	void SETJAPANUUID(const std::string& strUUID);

	const TCHAR* GETIDPARAM();
	const TCHAR* GETIDPARAM2();
	const TCHAR* GETIDPARAM3();
	const TCHAR* GETJAPANUUID();

	bool VALIDIDPARAM();

	BOOL IsAutoOptionSame();
	void SetAutoOptionVer();

	BOOL SETUSERID(const TCHAR* szUSERID);
//	BOOL SETCHARID ( const TCHAR* szCHARID );
//	std::string GETCHARID();
	std::string GETUSERID_ENC ();
	std::string GETUSERID_DEC ();
	std::string GETNULLID ();

	BOOL LOAD(
        const TCHAR* szRootPath,
        EMSERVICE_PROVIDER ServiceProvider,
        bool bToolMode,
        bool bServer/*=false*/);

	BOOL LOAD_CFG(const TCHAR* szRootPath, EMSERVICE_PROVIDER ServiceProvider);
	
	BOOL LOAD_PARAM(const TCHAR* szRootPath, EMSERVICE_PROVIDER ServiceProvider, bool bToolMode);
	BOOL LOAD_OPT(const TCHAR *szRootPath, EMSERVICE_PROVIDER ServiceProvider, bool bToolMode, bool bServer/*=false*/);	

	BOOL SAVE(EMSERVICE_PROVIDER ServiceProvider);
	DWORD EncryptedStringLen( const char *pString );
	BOOL SAVE_PARAM(const EMSERVICE_PROVIDER ServiceProvider, const BOOL bLauncher = FALSE );
};

#endif // _RAN_PARAM_H_

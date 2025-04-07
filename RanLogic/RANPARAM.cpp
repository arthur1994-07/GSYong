#include "pch.h"
#include <shlwapi.h>
#include <shlobj.h>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/DebugSet.h"
#include "../SigmaCore/Encrypt/CompByte.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/Common/StlFunctions.h"
#include "../EngineLib/Common/gltexfile.h"
#include "../EngineLib/Common/IniLoader.h"
#include "../EngineLib/DxTools/DxFogMan.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"


#include "./Activity/TitleManager.h"
#include "./RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace RANPARAM
{
	std::string			strRootPath			= "";
	std::string			strParamFile		= "";
	std::string			strOptionFile		= "";

//	int					emSERVICE_TYPE		= RENDERPARAM::EMSERVICE_DEFAULT;

	BOOL				bSAVE_USERID		= FALSE;
	std::string			strENC_USERID		= "";
	std::string			strENC_CHARID		= "";

	const std::string	strIDNULL			= "(null)";

	DWORD				g_dwVer				= 0x101;		// 저장될 때 기록하는 Version

	bool				bExchangeCheck		= true;
	BOOL				bDIS_CONFT			= FALSE;		//	'자동' 대련 거부.
	BOOL				bDIS_TRADE			= FALSE;		//	'자동' 거래 거부.
	BOOL				bDIS_PARTY			= FALSE;		//	'자동' 파티 거부.
	BOOL				bDIS_FRIEND			= FALSE;		//	'자동' 친구 요청 거부.
	BOOL				bDIS_VEHICLE		= FALSE;		//	'자동' 탑승 거부
	BOOL				bSHOW_SIMPLEHP		= TRUE;			//	미니 HP 표시
	BOOL				b3D_SOUND			= FALSE;
	BOOL				bSHOW_TIP			= TRUE;			//	팁 보여주기
	BOOL				bMOVABLE_ON_CHAT	= FALSE;		//	채팅창 위에서 움직이기
	BOOL				bFORCED_ATTACK		= FALSE;			//	강제 공격
	BOOL				bNAME_DISPLAY		= TRUE;			//	이름 항상 표시
	BOOL				bNON_Rebirth		= FALSE;		//  부활 스킬 금지
    BOOL				bSIMPLE_SUM_NAME	= FALSE;		//  소환수 이름 간략하게 표시 여부
	BOOL				bAUTO_CROW_HP	    = TRUE;		    //  자신을 타겟팅한 몬스터 HP바 자동표시
	BOOL				bCharInfoPrivate	= FALSE;		//	내 정보 비공개
	BOOL				bCharInfoAddition	= TRUE;		//	캐릭터 추가 정보창 표시
	BOOL				bSKILL_RESERVATION	= FALSE;		//  스킬 예약
	BOOL				bGroupChatInvite	= FALSE;		//	'자동'그룹채팅 거부
	EM_AROUND_DETAILTYPE emAroundDetail		= EM_AROUND_ORIG;	// 주변 캐릭터 단순화 등급
	EM_SKILLTYPE_UI		emSkillTypeUI		= EM_SKILLTYPE_PARTY;

	BOOL				bCONFT_BOUND		= TRUE;			//	대련시 영역 제한 여부.
	WORD				wCONFT_RECOVER		= 0;			//	대련시 회복약 제한.
	float				fCONFT_HP_RATE		= 1.0f;			//	대련시 체력 확대 비율.
	float				fCONFT_TAR_HP_RATE	= 1.0f;			//	대련시 체력 확대 비율 [ 상대방(개인대련시만) ].

	DWORD				dwPARTY_GET_ITEM	= 0;			//	파티옵션 아이템 분배 규칙.(디폴트는 순차습득)
	DWORD				dwPARTY_GET_ITEM_Filter	= 0;		//	파티옵션 아이템 분배 규칙의 등급
	DWORD				dwPARTY_GET_MONEY	= 0;		//	파티옵션 돈 분배 규칙.(디폴트는 균등분배)
	DWORD				dwPARTY_SCHOOL = 0;				//	파티옵션 구성 학원.(디폴트는 모든 학원)
	
	int					QuickSlot[QUICK_SLOT_NUM]		= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D };
	
	int					SkillSlot[QUICK_SKILL_NUM]		= { DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
															DIK_7, DIK_8, DIK_9, DIK_0};
	
	int					MenuShotcut[QUICK_MENU_NUM]		= { DIK_I, DIK_C, DIK_K, DIK_P, DIK_T, DIK_G, 
															DIK_F, DIK_M, DIK_B, DIK_R, DIK_H, DIK_L,
															DIK_X, DIK_Z ,DIK_BACKSLASH, DIK_J, DIK_O, DIK_SLASH };
	
	int					DefaultShotcutKey[QUICK_ALL_NUM]	= { DIK_Q, DIK_W, DIK_E, DIK_A, DIK_S, DIK_D,
																DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6,
																DIK_7, DIK_8, DIK_9, DIK_0,	DIK_I, DIK_C,
																DIK_K, DIK_P, DIK_T, DIK_G, DIK_F, DIK_M, 
																DIK_B, DIK_R, DIK_H, DIK_L, DIK_X, DIK_Z,
																DIK_BACKSLASH, DIK_J, DIK_O, DIK_SLASH };

	// KeySettingWindow 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
	int					KeySettingToRanparam[QUICK_MENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR, SHOTCUT_SKILL, SHOTCUT_PARTY,
																 SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
																 SHOTCUT_CHATMACRO, SHOTCUT_ITEMBANK, SHOTCUT_POINT_SHOP, SHOTCUT_RUN,
																 SHOTCUT_HELP, SHOTCUT_PET, SHOTCUT_ATTACKMODE, SHOTCUT_PKMODE, SHOTCUT_QBOX};

	// BasicMenu 참조 테이블 단축키 순서가 다르게 설정되어 있어서 필요
	int					BasicMenuToRanparam[BASICMENU_NUM] = { SHOTCUT_INVEN, SHOTCUT_CHAR,	SHOTCUT_SKILL, SHOTCUT_PARTY,
															   SHOTCUT_QUEST, SHOTCUT_CLUB, SHOTCUT_FRIEND, SHOTCUT_MAP,
															   SHOTCUT_CHATMACRO, SHOTCUT_ITEMBANK, SHOTCUT_POINT_SHOP, SHOTCUT_SNS,
                                                               SHOTCUT_RUN
                                                             };
	
	CString				ChatMacro[CHAT_MACRO_NUM];

    DWORD			    QuestDisplayID[MAX_QUEST_DISPLAY];

    std::string         strPointShopSearchWord[MAX_SEARCH_WORD];
    DWORD            dwPointShopWishList[MAX_POINT_SHOP_WISH_LIST];

    bool             bCheckAutoRebirth = false; // 부활시 부활하기로 부활 체크.

    bool bCheckUsableItemMarketPage = true; // NPC 상점 ui에서 사용 가능한 아이템 체크.
    bool bCheckBuyableItemMarketPage =  true; // NPC 상점 ui에서 구입 가능한 아이템 체크.
    bool bCheckRebuySuppliesRebuyPage = true; // NPC 재매입 ui에서 소모품 아이템 체크.
    bool bCheckAllDelItemsRebuyPage = true; // NPC 재매입 ui에서 접속종료 시 재매입 아이템 확인.
    bool bCheckSellItemRButton = false; // NPC 상점 ui에서 마우스 우클릭으로 아이템 판매 체크.
    bool bCheckConfirmSellItem = true; // NPC 상점 ui에서 아이템 판매 시 확인.

    DWORD dwCharacterTitleIndex = TITLE_INDEX_NONE; // 캐릭터 뱃지(타이틀) 설정값 등록.

	BOOL				bCHECKWHQL			= TRUE;
	DWORD				dwScrWidth			= 800;
	DWORD				dwScrHeight			= 600;
	EMSCREEN_FORMAT		emScrFormat			= EMSCREEN_F16;
	UINT				uScrRefreshHz		= 60;
	BOOL				bScrWindowed		= FALSE;
	//BOOL				bScrWndHalfSize		= TRUE;
	BOOL				bScrWndFullSize		= FALSE;

	bool				bDISP_CONSOLE		= false;
	bool				bDISP_FPS			= false;

	BOOL				bGameCursor			= TRUE;

	BOOL				bAllMinimum			= TRUE;

	LONG				nSndSfx				= 0;
	LONG				nSndMap				= 0;
	LONG				nSndMusic			= 0;
	DWORD				dw3DAlgorithm		= 0;
	DWORD				dwSndBufferSize		= 10;

	BOOL				bSndMuteSfx			= FALSE;
	BOOL				bSndMuteMap			= FALSE;
	BOOL				bSndMuteMusic		= FALSE;
	BOOL				bUsePassiveDN		= FALSE;


	DWORD				dwVideoLevel		= 0;
	DWORD				dwShadowChar		= 4;
	BOOL				bBuff				= TRUE;
	BOOL				bRealReflect		= TRUE;
	BOOL				bRefract			= TRUE;
	BOOL				bGlow				= TRUE;
	BOOL				bPost				= TRUE;
	BOOL				bFrameLimit			= TRUE;
	BOOL				bTargetEffect		= TRUE;
	BOOL				bMineEffect			= TRUE;
	BOOL				bClickEffect		= TRUE;
	BOOL				bUseNormalMap		= TRUE;
	BOOL				bPossibleNormalMap  = TRUE;
	DWORD				dwSkinDetail		= 2;
	DWORD				dwAutoOptionVer		= 3;	// 그래픽 옵션의 변경으로 이전 유저가 불편할 수 있어 자동으로 Option을 변경해준다.
													// 숫자를 높히면 그 클라이언트의 사양을 보고 자동으로 Option을 셋팅한다. (한번)
	DWORD				dwAutoOptionLoad	= 0;	// 클라이언트의 AutoOption Version임.
	language::SERVICENATION		emNation	= language::NATION_KOREA;
	language::EMPROVIDE_LANGFLAG emProvideLangFlag = language::LANGFLAG_KOREA;

	BOOL				bBUFF_SFX			= TRUE;
	BOOL				bEMOTICON			= TRUE;

	DWORD				dwFogRange			= 0;

	TCHAR				LoginAddress[128]	= "211.172.252.50";
    int                 LoginServerPort = 5001;
	TCHAR				WB_LoginAddress[128]	= "211.172.252.50";
    int                 WB_LoginServerPort = 5001;

	TCHAR				FtpAddressTable[MAX_FTP][128] =
											{
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "",
											};
	TCHAR				HttpAddressTable[MAX_HTTP][128] =
											{
												"patch.ran-online.co.kr", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "", "",
												"", "", "", "",
											};

	CString				strNewsURL			= "about:blank";
	CString				strImageURL			= "about:blank";
	CString				strConnectionURL	= "2046.5u56.com";
	CString				strForumURL			= "about:blank";
	CString				strTWForumURL			= "https://forum.gamer.com.tw/A.php?bsn=11712";

	UINT				nChinaRegion		= MAX_CHINA_REGION;

	SERVER_SET			China_Region[MAX_CHINA_REGION];
	
//    language::LANGFLAG	emLangSet			= language::DEFAULT;
	CString				strGDIFont			= "돋움체";
	CString				strFontType			= "D3DFONT";
	
	CString				strOriginalCountry	= "kr";
	CString				strCountry			= "kr";
	CString				strLauncherText		= "Launcher.xml";
	CString				strGameExText		= "gameextext.xml";
	CString				strGameInText		= "gameintext.xml";
	CString				strGameWord			= "gameword.xml";
	CString				strServerText		= "servertext.xml";
	CString				strSkillText		= "skillstrtable.xml";
	CString				strItemText			= "itemstrtable.xml";
	CString				strCrowText			= "crowstrtable.xml";
	CString				strTipText			= "tipstrtable.xml";
	CString				strCommentText		= "commentstrtable.xml";
	CString				strPetCommentText	= "petcommentstrtable.xml";
	CString				strQuestText		= "queststrtable.xml";
	CString				strNPCTalkText		= "npctalkstrtable.xml";
	CString				strNPCAnswerText	= "npcanswerstrtable.xml";
	CString				strAutoLevelSetText	= "autolevelset.xml";

	CString				strUIOuterCfg		= "uioutercfg.xml";
	CString				strUIInnerCfg01		= "uiinnercfg01.xml";
	CString				strUIInnerCfg02		= "uiinnercfg02.xml";
	CString				strUIInnerCfg03		= "uiinnercfg03.xml";
	CString				strUIExtCfg			= "uiextcfg.xml";	
    
    CString             strUITextureList    = "uiinnerTextureList.xml"; 
    CString             strUIXmlList        = "UIXmlList.xml";


    DWORD		        dwServerNum	        = 0;

	TCHAR				HelpAddress[STRING_NUM_128] = _T("\\data\\help\\help.htm");
	// 중국 : http://2046.5u56.com/gameshop/gameshop.html
	TCHAR				ItemShopAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR				RPFDataPath[STRING_NUM_128] = _T("\\package\\data.rpf");
	TCHAR				ItemShopHPAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR				EventPageAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR				ManualPatchAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR				HomePageAddress[STRING_NUM_128] = _T("about:blank");
    TCHAR               CustomerCenterAddress[STRING_NUM_128] = _T("about:blank");
	TCHAR               MultiHelpAddress[language::LANGFLAG_TOTAL_NUM][STRING_NUM_128] =
	{
		/*
		_T("\\data\\help\\help.htm")
		, _T("\\data\\help\\help_gs.htm")
		, _T("\\data\\help\\help_gs.htm")
		, _T("\\data\\help\\help_gs.htm")
		, _T("\\data\\help\\help_tw.htm")
		, _T("\\data\\help\\help_id.htm")
		, _T("\\data\\help\\help_th.htm")
		, _T("\\data\\help\\help_me.htm")
		, _T("\\data\\help\\help_ml.htm")
		, _T("\\data\\help\\help_ph.htm")
		, _T("\\data\\help\\help_cn.htm")
		*/
		_T("\\data\\help\\help_kr.htm"),
		_T("\\data\\help\\help_tw.htm"),
		_T("\\data\\help\\help_cn.htm"),
		_T("\\data\\help\\help_jp.htm"),
		_T("\\data\\help\\help_ph.htm"),
		_T("\\data\\help\\help_th.htm"),
		_T("\\data\\help\\help_gs.htm"),
		_T("\\data\\help\\help_ml.htm"),
		_T("\\data\\help\\help_gs.htm"),
		_T("\\data\\help\\help_id.htm"),
		_T("\\data\\help\\help_me.htm"),
		_T("\\data\\help\\help_vn.htm"),
		_T("\\data\\help\\help_hk.htm"),
		_T("\\data\\help\\help_gs.htm"),
		_T("\\data\\help\\help_eu.htm"),
		_T("\\data\\help\\help_us.htm")
	};

	// 이름 보기 설정
	DWORD dwNameDisplay = ENameDisplay::All;

	// 다른 유저 데미지 보기
	bool bShowOtherDamage = false;

	// UI 안티 앨리어싱
	int	nUIAntiAliasing = 1;
	// 이름, 데미지 디스플레이 개수 제한
	bool bDisplayLimit = true;

    SNSFACEBOOK_SET     sSNSFaceBook;
    SNSTWITTER_SET      sSNSTwitter;

	BOOL LOAD_CFG(const TCHAR* szRootPath, EMSERVICE_PROVIDER ServiceProvider)
	{
		strRootPath = szRootPath;
		strParamFile = strRootPath + "\\param.cfg";

		gltexfile cFILE;
		cFILE.reg_sep("\t ");

		if (!cFILE.open(strParamFile, false))
        {
            std::string Message(
                sc::string::format(
                    "%1% file load failed",
                    strParamFile));
            AfxMessageBox(Message.c_str(), MB_OK);
            return FALSE;
        }

		int nTEMP(0);

		DWORD dwVer(0x100);
		cFILE.getflag ( "dwVer", 1, 1, dwVer, true );

		//#ifndef KR_PARAM
        if (ServiceProvider != SP_KOREA && ServiceProvider != SP_KOREA_TEST)
		{
			//#ifndef TW_PARAM
			//	cFILE.getflag ( "bSAVE_USERID", 1, 1, bSAVE_USERID, true );
			//	if ( bSAVE_USERID )
			//#endif
			{
				cFILE.getflag ( "strENC_USERID", 1, 1, strENC_USERID, true );
				if ( strIDNULL == strENC_USERID )	strENC_USERID = "";
			}
		}
		//#endif

		cFILE.getflag ( "bDISP_CONSOLE", 1, 1, bDISP_CONSOLE, true );
		cFILE.getflag ( "bDISP_FPS", 1, 1, bDISP_FPS, true );

		cFILE.getflag ( "bCHECKWHQL", 1, 1, bCHECKWHQL, true );
		cFILE.getflag ( "dwScrWidth", 1, 1, dwScrWidth, true );
		cFILE.getflag ( "dwScrHeight", 1, 1, dwScrHeight, true );
		cFILE.getflag ( "uScrRefreshHz", 1, 1, uScrRefreshHz, true );
		cFILE.getflag ( "emScrFormat", 1, 1, nTEMP, true );
		emScrFormat = (EMSCREEN_FORMAT) nTEMP;

		cFILE.getflag ( "bScrWindowed", 1, 1, bScrWindowed, true );
		cFILE.getflag ( "bScrWndFullSize", 1, 1, bScrWndFullSize, true );
		cFILE.getflag ( "bGameCursor", 1, 1, bGameCursor, true );
		cFILE.getflag ( "bAllMinimum", 1, 1, bAllMinimum, true );

		cFILE.getflag ( "b3D_SOUND", 1, 1, b3D_SOUND, true );
		cFILE.getflag ( "nSndSfx", 1, 1, nSndSfx, true );
		cFILE.getflag ( "nSndMap", 1, 1, nSndMap, true );
		cFILE.getflag ( "nSndMusic", 1, 1, nSndMusic, true );
		cFILE.getflag ( "dw3DAlgorithm", 1, 1, dw3DAlgorithm, true );
		cFILE.getflag ( "dwSndBufferSize", 1, 1, dwSndBufferSize, true );

		cFILE.getflag ( "bSndMuteSfx", 1, 1, bSndMuteSfx, true );
		cFILE.getflag ( "bSndMuteMap", 1, 1, bSndMuteMap, true );
		cFILE.getflag ( "bSndMuteMusic", 1, 1, bSndMuteMusic, true );
		cFILE.getflag ( "bUsePassiveDN", 1, 1, bUsePassiveDN, true );		

		cFILE.getflag ( "dwShadowChar", 1, 1, dwShadowChar, true );
		cFILE.getflag ( "bBuff", 1, 1, bBuff, true );
		cFILE.getflag ( "bRealReflect", 1, 1, bRealReflect, true );
		cFILE.getflag ( "bRefract", 1, 1, bRefract, true );
		cFILE.getflag ( "bGlow", 1, 1, bGlow, true );
		cFILE.getflag ( "bPost", 1, 1, bPost, true );
		cFILE.getflag ( "dwSkinDetail", 1, 1, dwSkinDetail, true );
		cFILE.getflag ( "dwAutoOptionLoad", 1, 1, dwAutoOptionLoad, true );
		cFILE.getflag ( "dwFogRange", 1, 1, dwFogRange, true );
		cFILE.getflag ( "bFrameLimit", 1, 1, bFrameLimit, true );
		cFILE.getflag ( "bTargetEffect", 1, 1, RENDERPARAM::bTargetEffect, true );
		cFILE.getflag ( "bMineEffect", 1, 1, RENDERPARAM::bMineEffect, true );
		cFILE.getflag ( "bClickEffect", 1, 1, RENDERPARAM::bClickEffect, true );		
		cFILE.getflag ( "bUseNormalMap", 1, 1, RENDERPARAM::bUseNormalMap, true );
        cFILE.getflag ( "bVolumeLight", 1, 1, RENDERPARAM::bVolumeLight, true );

		cFILE.getflag ( "dwTexFilter", 1, 1, RENDERPARAM::g_emTextureFiltering, true );
		cFILE.getflag ( "dwTexQuality", 1, 1, RENDERPARAM::g_emTextureQuality, true );
		RENDERPARAM::SetTextureQuality( RENDERPARAM::g_emTextureQuality );

		cFILE.getflag ( "dwPhysXCloth", 1, 1, RENDERPARAM::g_emPhysXCloth, true );

		cFILE.getflag ( "bBUFF_SFX", 1, 1, bBUFF_SFX );
		cFILE.getflag ( "bEMOTICON", 1, 1, bEMOTICON );

		cFILE.getflag_sz( "LoginAddress", 1, 1, LoginAddress, STRING_NUM_128, true );
        cFILE.getflag("LoginServerPort", 1, 1, LoginServerPort, false);

		// CFG 는 더이상 사용하지 않음
		cFILE.getflag_sz( "WB_LoginAddress", 1, 1, WB_LoginAddress, STRING_NUM_128, true );
        cFILE.getflag("WB_LoginServerPort", 1, 1, WB_LoginServerPort, false);

		for ( int i=0; i<MAX_FTP; ++i )
		{
			CString strFTPADD;
			strFTPADD.Format ( "FtpAddress%02d", i );



			memset ( FtpAddressTable[i], 0, sizeof(char)*128 );

			gltexfile::VECSTRING* pVECSTR = cFILE.findflag ( strFTPADD.GetString() );
			if ( pVECSTR )
			{
				cFILE.getflag_sz( strFTPADD.GetString(), 1, 1, FtpAddressTable[i], STRING_NUM_128, true );
			}
		}

		for ( int i=0; i<MAX_HTTP; ++i )
		{
			CString strHTTPADD;
			strHTTPADD.Format ( "HttpAddress%02d", i );

			memset ( HttpAddressTable[i], 0, sizeof(char)*128 );

			gltexfile::VECSTRING* pVECSTR = cFILE.findflag ( strHTTPADD.GetString() );
			if ( pVECSTR )
			{
				cFILE.getflag_sz( strHTTPADD.GetString(), 1, 1, HttpAddressTable[i], STRING_NUM_128, true );
			}
		}

		cFILE.getflag ( "strNewsURL", 1, 1, strNewsURL, true );
		cFILE.getflag ( "strImageURL", 1, 1, strImageURL, true );
		cFILE.getflag ( "strConnectionURL", 1, 1, strConnectionURL, true );
		cFILE.getflag ( "strForumURL", 1, 1, strForumURL, true );
		cFILE.getflag ( "nChinaRegion", 1, 1, nChinaRegion, true );

		cFILE.getflag ( "strFontType", 1, 1, strFontType, true );        

		DWORD dwLangSet = 0;
		RENDERPARAM::emLangSet = language::LANGFLAG(dwLangSet);
		emNation = language::StrNationToEMNation( strCountry );

		cFILE.getflag ( "dwLangSet", 1, 1, dwLangSet, true );
		cFILE.getflag ( "strGDIFont", 1, 1, strGDIFont, true );

		cFILE.getflag ( "strCountry", 1, 1, strCountry, true );
		
		emProvideLangFlag = language::StringToProvidelang(strCountry);
		if ( language::LANGFLAG_TOTAL_NUM == emProvideLangFlag )
		{
			emProvideLangFlag = language::StringToProvidelang(strOriginalCountry);
			if ( language::LANGFLAG_TOTAL_NUM == emProvideLangFlag )
			{
				// [shhan][2015.04.01] 언어 복원이 불가능 할 경우 GS 로 동작되도록 한다.
				emProvideLangFlag = language::LANGFLAG_GS;
			}
		}

		cFILE.getflag ( "strLauncherText", 1, 1, strLauncherText, true );
		cFILE.getflag ( "strGameExText", 1, 1, strGameExText, true );
		cFILE.getflag ( "strGameInText", 1, 1, strGameInText, true );
		cFILE.getflag ( "strGameWord", 1, 1, strGameWord, true );
		cFILE.getflag ( "strServerText", 1, 1, strServerText, true );
		cFILE.getflag ( "strSkillText", 1, 1,	strSkillText, true );
		cFILE.getflag ( "strItemText", 1, 1,	strItemText, true );
		cFILE.getflag ( "strCrowText", 1, 1,	strCrowText, true );
		cFILE.getflag ( "strTipText", 1, 1,		strTipText, true );
		cFILE.getflag ( "strCommentText", 1, 1,		strCommentText, true );		
		cFILE.getflag ( "strPetCommentText", 1, 1,	strPetCommentText, true );
		cFILE.getflag ( "strQuestText", 1, 1,		strQuestText, true );
		cFILE.getflag ( "strNPCTalkText", 1, 1,		strNPCTalkText, true );
		cFILE.getflag ( "strNPCAnswerText", 1, 1,	strNPCAnswerText, true );

		cFILE.getflag ( "strUIOuterCfg", 1, 1, strUIOuterCfg, true );
		cFILE.getflag ( "strUIInnerCfg01", 1, 1, strUIInnerCfg01, true );
		cFILE.getflag ( "strUIInnerCfg02", 1, 1, strUIInnerCfg02, true );
		cFILE.getflag ( "strUIInnerCfg03", 1, 1, strUIInnerCfg03, true );
		cFILE.getflag ( "strUIExtCfg", 1, 1, strUIExtCfg, true );

		//	Note : 유효성 체크.
		//
		// PARAM.INI LOAD시 XML이 로드 되어있지않다;
// 		if ( GLUseFeatures::GetInstance().IsUsingPrivateDataIntegrate() )
// 		{
// 			if ( CGameTextMan::GetInstance().GetCommentText("FONTSYS", 0).GetString()!=strFontType.GetString() 
// 				&& CGameTextMan::GetInstance().GetCommentText("FONTSYS", 1).GetString()!=strFontType.GetString() )
// 			{
// 				strFontType = COMMENT::FONTSYS[0].c_str();
// 			}
// 		}
// 		else
// 		{
			if ( COMMENT::FONTSYS[0]!=strFontType.GetString() && COMMENT::FONTSYS[1]!=strFontType.GetString() )
			{
				strFontType = COMMENT::FONTSYS[0].c_str();
			}
//		}

		//if ( !bScrWndHalfSize )		bScrWndFullSize = TRUE;	// 필요없는 데이터

		// 기존에는 3 개였지만.
		// 지금은 4개로 늘었고 앞부분이 늘어나서 1을 더해줌.
		if ( dwVer <= 0x100 )
		{
			if ( dwFogRange >= 0 && dwFogRange <= 2 )
			{
				dwFogRange += 1;
				if ( dwFogRange > EMFR_HIGH )
					dwFogRange = EMFR_HIGH;
			}
			else
			{
				// 이전 버젼 런쳐를 사용할 경우 dwFogRange 값에 문제가 발생할 수 있어서 보완함.
				dwFogRange = EMFR_HIGH;
			}
		}
		
		return TRUE;
	}
	
	BOOL LOAD(const TCHAR* szRootPath, EMSERVICE_PROVIDER ServiceProvider, bool bToolMode, bool bServer)
	{
        GLogicData::GetInstance().LoadCountryFile();

		strRootPath = szRootPath;

		if (!LOAD_PARAM(szRootPath, ServiceProvider, bToolMode))
		{
			return LOAD_CFG(szRootPath, ServiceProvider);
		}

		if (!LOAD_OPT(szRootPath, ServiceProvider, bToolMode, bServer))
		{
			return FALSE;
		}

        //	중국일 경우 지역 선택시 값을 갱신해준다.
//#ifdef CH_PARAM
		if (ServiceProvider == SP_CHINA)
		{		
			if ( nChinaRegion == MAX_CHINA_REGION )	return TRUE;

			StringCchCopy( LoginAddress, STRING_NUM_128, China_Region[nChinaRegion].LoginAddress.GetString() );

			for( int i=0 ; ; ++i )
			{
				if( China_Region[nChinaRegion].HttpAddressTable[i].IsEmpty() ) 
					break;

				StringCchCopy( HttpAddressTable[i], STRING_NUM_128, China_Region[nChinaRegion].HttpAddressTable[i].GetString() );
			}

			strNewsURL = China_Region[nChinaRegion].strNewsURL;
			strImageURL = China_Region[nChinaRegion].strImageURL;
			StringCchCopy( ItemShopAddress, STRING_NUM_128, China_Region[nChinaRegion].strItemShop.GetString() );
			StringCchCopy( ItemShopHPAddress, STRING_NUM_128, China_Region[nChinaRegion].strItemShopHP.GetString() );
			StringCchCopy( EventPageAddress, STRING_NUM_128, China_Region[nChinaRegion].strEventPage.GetString() );
			StringCchCopy( ManualPatchAddress, STRING_NUM_128, China_Region[nChinaRegion].strManualPath.GetString() );
			StringCchCopy( HomePageAddress, STRING_NUM_128, China_Region[nChinaRegion].strHomePage.GetString() );
		}
//#endif

		return TRUE;
	}

	BOOL SAVE(EMSERVICE_PROVIDER ServiceProvider)
	{
		if (strOptionFile.empty())
            return FALSE;

		CString strBuffer;

		strBuffer += _T("[GAME OPTION]\n");

		strBuffer.AppendFormat( "dwVer = %d\n", g_dwVer );

		//#ifndef KR_PARAM
        if (ServiceProvider != SP_KOREA && ServiceProvider != SP_KOREA_TEST)
		{
            strBuffer.AppendFormat( "bSAVE_USERID = %d\n", bSAVE_USERID );
			if (strENC_USERID.empty() || strENC_USERID == "" )
			{
				strBuffer.AppendFormat( "strENC_USERID = %s\n", strIDNULL.c_str() );
			}
			else
			{
				strBuffer.AppendFormat( "strENC_USERID = %s\n", strENC_USERID.c_str() );
			}
		}
		//#endif

		strBuffer.AppendFormat( "nChinaRegion = %d\n", nChinaRegion );

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[SCREEN OPTION]\n");
			strBuffer.AppendFormat( "bCHECKWHQL	= %d\n", bCHECKWHQL );
			strBuffer.AppendFormat( "dwScrWidth	= %d\n", dwScrWidth );
			strBuffer.AppendFormat( "dwScrHeight = %d\n", dwScrHeight );
			strBuffer.AppendFormat( "emScrFormat = %d\n", emScrFormat );
			strBuffer.AppendFormat( "uScrRefreshHz = %d\n", uScrRefreshHz );
			strBuffer.AppendFormat( "bScrWindowed = %d\n", bScrWindowed );
			strBuffer.AppendFormat( "bScrWndFullSize = %d\n", bScrWndFullSize );
			strBuffer.AppendFormat( "bGameCursor = %d\n", bGameCursor );
			strBuffer.AppendFormat( "bAllMinimum = %d\n", bAllMinimum );
		}

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[SOUND OPTION]\n");
			strBuffer.AppendFormat( "b3D_SOUND = %d\n", b3D_SOUND );
			strBuffer.AppendFormat( "nSndSfx = %d\n", nSndSfx );
			strBuffer.AppendFormat( "nSndMap = %d\n", nSndMap );
			strBuffer.AppendFormat( "nSndMusic = %d\n", nSndMusic );
			strBuffer.AppendFormat( "dw3DAlgorithm = %d\n", dw3DAlgorithm );
			strBuffer.AppendFormat( "dwSndBufferSize = %d\n", dwSndBufferSize );
			strBuffer.AppendFormat( "bSndMuteSfx = %d\n", bSndMuteSfx );
			strBuffer.AppendFormat( "bSndMuteMap = %d\n", bSndMuteMap );
			strBuffer.AppendFormat( "bSndMuteMusic = %d\n", bSndMuteMusic );
		}

		{
			strBuffer += _T(";\n");
			strBuffer += _T("[GRAPHIC OPTION]\n");
			strBuffer.AppendFormat( "dwVideoLevel = %d\n", dwVideoLevel );
			strBuffer.AppendFormat( "dwShadowChar = %d\n", dwShadowChar );
			strBuffer.AppendFormat( "bBuff = %d\n", bBuff );
			strBuffer.AppendFormat( "bRealReflect = %d\n", bRealReflect );
			strBuffer.AppendFormat( "bRefract = %d\n", bRefract );
			strBuffer.AppendFormat( "bGlow = %d\n", bGlow );
			strBuffer.AppendFormat( "bPost = %d\n", bPost );
			strBuffer.AppendFormat( "bFrameLimit = %d\n", bFrameLimit );
			strBuffer.AppendFormat( "bTargetEffect = %d\n", RENDERPARAM::bTargetEffect );
			strBuffer.AppendFormat( "bMineEffect = %d\n", RENDERPARAM::bMineEffect );
			strBuffer.AppendFormat( "bClickEffect = %d\n", RENDERPARAM::bClickEffect );
			strBuffer.AppendFormat( "bUseNormalMap = %d\n", RENDERPARAM::bUseNormalMap );
            strBuffer.AppendFormat( "bVolumeLight = %d\n", RENDERPARAM::bVolumeLight );
			strBuffer.AppendFormat( "dwSkinDetail = %d\n", dwSkinDetail );
			strBuffer.AppendFormat( "dwAutoOptionLoad = %d\n", dwAutoOptionLoad );
			strBuffer.AppendFormat( "bBUFF_SFX = %d\n", bBUFF_SFX );
			strBuffer.AppendFormat( "bEMOTICON = %d\n", bEMOTICON );
			strBuffer.AppendFormat( "dwFogRange = %d\n", dwFogRange );
			strBuffer.AppendFormat( "strFontType = %s\n", strFontType );
			strBuffer.AppendFormat( "dwTexFilter = %d\n", RENDERPARAM::g_emTextureFiltering );
			strBuffer.AppendFormat( "dwTexQuality = %d\n", RENDERPARAM::g_emTextureQuality );
			strBuffer.AppendFormat( "dwPhysXCloth = %d\n", RENDERPARAM::g_emPhysXCloth );
		}

        sc::CRijndael oRijndael;
        oRijndael.Initialize(sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

		int nLen = oRijndael.GetEncryptStringLength( strBuffer );
		char * szBuffer = new char[nLen];
		memset( szBuffer, 0, nLen );

		oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
        int nVersion = sc::CRijndael::VERSION;

		if ( nVersion <= 0 && strlen( szBuffer ) <= 0 )	return FALSE;

		CFile file;
		if ( ! file.Open( strOptionFile.c_str(), CFile::modeCreate|CFile::modeWrite ) )	// 파일 열기
		{
			delete [] szBuffer;
			return FALSE;
		}
		
		file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
		file.Write( szBuffer, nLen );		// 파일 쓰기
		file.Close();

		delete [] szBuffer;

		return TRUE;
	}

	std::string strIDPARAM;
	std::string strIDPARAM2;
	std::string strIDPARAM3;

	std::string strJPUUID;

	void SETIDPARAM(const std::string& strPARAM)  { strIDPARAM = strPARAM; }
	void SETIDPARAM2(const std::string& strPARAM) { strIDPARAM2 = strPARAM; }
	void SETIDPARAM3(const std::string& strPARAM) { strIDPARAM3 = strPARAM; }
	void SETJAPANUUID(const std::string& strUUID) { strJPUUID = strUUID; }

	const TCHAR* GETIDPARAM()	{ return strIDPARAM.c_str(); }
	const TCHAR* GETIDPARAM2()	{ return strIDPARAM2.c_str(); }
	const TCHAR* GETIDPARAM3()	{ return strIDPARAM3.c_str(); }
	const TCHAR* GETJAPANUUID()	{ return strJPUUID.c_str(); }

	bool VALIDIDPARAM ()
	{
//		switch ( emSERVICE_TYPE )
//		{
//		Japan 로그인 방식 변경
//		case EMSERVICE_JAPAN:
//			return !strIDPARAM.empty() && !strIDPARAM2.empty() && !strIDPARAM3.empty();

//		default:
//			return !strIDPARAM.empty();
//		};

		return !strIDPARAM.empty();
	}

	enum { emENCKEYLENG = MAX_COMPUTERNAME_LENGTH+32 };
	char szEncryptKEY[emENCKEYLENG] = "unnamed";

	void SETENCRYPT ()
	{
		//DWORD dwNAMELENG = emENCKEYLENG;
		//BOOL bOK = GetComputerName ( szEncryptKEY, &dwNAMELENG );
		//if ( !bOK )
		//{
		//	DWORD dwERR = GetLastError();
		//	sc::writeLogError ( "error GetComputerName(), code : %d", dwERR );
		//}

		//StringCchCat ( szEncryptKEY, emENCKEYLENG, "XWeRtSOn32QwRtQs" );
		StringCchCopy ( szEncryptKEY, emENCKEYLENG, "XWeRtSOn32QwRtQs" );
	}


// 	BOOL SETCHARID( const TCHAR* szCHARID )
// 	{
// 		strENC_CHARID = szCHARID;
// 		
// 		return TRUE;
// 	}
// 
// 	std::string GETCHARID()
// 	{
// 		return strENC_CHARID;
// 	}


	BOOL SETUSERID ( const TCHAR* szUSERID )
	{
		SETENCRYPT ();

		BYTE szENC_USERID[MAX_PATH] = "";

		size_t nKEYLENG = strlen ( szEncryptKEY );
		size_t nIDLENG = strlen ( szUSERID );
		compbyte::encrypt ( (UCHAR*) szEncryptKEY, (DWORD)nKEYLENG, (const UCHAR*) szUSERID, szENC_USERID, (DWORD)nIDLENG );
		compbyte::hex2string( szENC_USERID, (DWORD)nIDLENG, strENC_USERID );
	
		return TRUE;
	}

	std::string GETUSERID_ENC ()
	{
		return strENC_USERID;
	}

	std::string GETUSERID_DEC ()
	{
		SETENCRYPT ();

		if ( strENC_USERID.empty() )
		{
			return std::string("");
		}

		const char* szENC_USERID = strENC_USERID.c_str();

		size_t nKEYLENG = strlen ( szEncryptKEY );
		size_t nIDLENG = strlen ( szENC_USERID );
		if ( nIDLENG == 0 )		return "";

		BYTE szHEX_USERID[MAX_PATH] = "";
		char szDEC_USERID[MAX_PATH] = "";

		SecureZeroMemory ( szHEX_USERID, MAX_PATH );
		SecureZeroMemory ( szDEC_USERID, MAX_PATH );

		BOOL bOK = compbyte::string2hex ( (UCHAR*)szENC_USERID, (UCHAR*) szHEX_USERID, (DWORD) nIDLENG );
		DWORD nHEXLENG = EncryptedStringLen ( (char*) szHEX_USERID );

		compbyte::decrypt ( (UCHAR*) szEncryptKEY, (DWORD) nKEYLENG, (const UCHAR*) szHEX_USERID, (UCHAR*) szDEC_USERID, nHEXLENG );

		return szDEC_USERID;
	}

	// 암호화된 string 중에 0이 들어가는경우가 있으므로 strlen 사용 불가
	DWORD EncryptedStringLen( const char *pString )
	{
		int idx = 0;
		DWORD nCount = 0;

		while (idx < MAX_PATH-1)
		{
			if(pString[idx] == '\0' && pString[idx+1] == '\0') break;

			idx++;
			nCount++;
		}

		return nCount;
	}


	std::string GETNULLID ()
	{
		return strIDNULL;
	}

	BOOL IsAutoOptionSame()
	{
		if ( dwAutoOptionLoad == dwAutoOptionVer )
		{
			return TRUE;
		}

		return FALSE;
	}

	void SetAutoOptionVer()
	{
		dwAutoOptionLoad = dwAutoOptionVer;
	}

	BOOL LOAD_PARAM(const TCHAR* szRootPath, EMSERVICE_PROVIDER ServiceProvider, bool bToolMode)
	{
		strParamFile = strRootPath;
		strParamFile += "\\param.ini";

		CIniLoader cFILE(",", bToolMode);
		
		if (!cFILE.open(strParamFile, true))
			return FALSE;

		// param.ini use not error msg box
		cFILE.SetUseErrorMsgBox( FALSE );

		{
			cFILE.getflag_sz( "SERVER SET", "LoginAddress", 0, 1, LoginAddress, STRING_NUM_128 );
            cFILE.getflag( "SERVER SET", "LoginServerPort", 0, 1, LoginServerPort);
			cFILE.getflag_sz( "SERVER SET", "WB_LoginAddress", 0, 1, WB_LoginAddress, STRING_NUM_128 );
            cFILE.getflag( "SERVER SET", "WB_LoginServerPort", 0, 1, WB_LoginServerPort);

			UINT nNum = cFILE.GetKeyDataSize( "SERVER SET", "HttpAddress" );

			memset( HttpAddressTable, 0, sizeof(HttpAddressTable) );
			
			for ( UINT i=0; i<nNum; ++i )
			{
				CString strHttpAddress;
				cFILE.getflag( "SERVER SET", "HttpAddress", i, nNum, strHttpAddress );
				strHttpAddress.Trim( _T(" ") );

				StringCchCopy( HttpAddressTable[i], STRING_NUM_128, strHttpAddress );
			}

			cFILE.getflag ( "SERVER SET", "strNewsURL", 0, 1, strNewsURL );
			cFILE.getflag ( "SERVER SET", "strImageURL", 0, 1, strImageURL );
			cFILE.getflag ( "SERVER SET", "strConnectionURL", 0, 1, strConnectionURL );
			cFILE.getflag ( "SERVER SET", "strForumURL", 0, 1, strForumURL );
			cFILE.getflag ( "SERVER SET", "bUsePassiveDN", 0, 1, bUsePassiveDN );			
		}

		//#ifdef CH_PARAM
		if (ServiceProvider == SP_CHINA)
		{
			CString strSection;

			for( UINT i=0; i<MAX_CHINA_REGION; ++i )
			{
				strSection.Format( "CH_REGION_%d", i );
				cFILE.getflag( strSection.GetString(), "strName", 0, 1, China_Region[i].strName );
				cFILE.getflag( strSection.GetString(), "LoginAddress", 0, 1, China_Region[i].LoginAddress );

				UINT nNum = cFILE.GetKeyDataSize( strSection.GetString(), "HttpAddress" );
				if( nNum > MAX_HTTP ) nNum = MAX_HTTP;
				for( UINT j=0; j<nNum; ++j )
				{
					cFILE.getflag( strSection.GetString(), "HttpAddress", j, nNum, China_Region[i].HttpAddressTable[j] );
					China_Region[i].HttpAddressTable[j].Trim( _T(" ") );
				}

				cFILE.getflag( strSection.GetString(), "strNewsURL", 0, 1, China_Region[i].strNewsURL );
				cFILE.getflag( strSection.GetString(), "strImageURL", 0, 1, China_Region[i].strImageURL );

				nNum = cFILE.GetKeyDataSize( strSection.GetString(), "ServerName" );
				if( nNum > MAX_SERVER ) nNum = MAX_SERVER;
				for( UINT j=0; j<nNum; ++j )
				{
					cFILE.getflag( strSection.GetString(), "ServerName", j, nNum, China_Region[i].ServerName[j] );
					China_Region[i].ServerName[j].Trim( _T(" ") );
				}

				
				cFILE.getflag( strSection.GetString(), "HomePageAddress", 0, 1, China_Region[i].strHomePage );
				cFILE.getflag( strSection.GetString(), "ItemShopAddress", 0, 1, China_Region[i].strItemShop );
				cFILE.getflag( strSection.GetString(), "ItemShopHPAddress", 0, 1, China_Region[i].strItemShopHP );
				cFILE.getflag( strSection.GetString(), "EventPageAddress", 0, 1, China_Region[i].strEventPage );
				cFILE.getflag( strSection.GetString(), "ManualPatchAddress", 0, 1, China_Region[i].strManualPath );
			}
		}
		//#endif

		{
            DWORD dwLangSet = 0;
			cFILE.getflag ( "GUI OPTION", "dwLangSet", 0, 1, dwLangSet );
			RENDERPARAM::emLangSet = language::LANGFLAG(dwLangSet);

			cFILE.getflag ( "GUI OPTION", "strGDIFont", 0, 1, strGDIFont );
		}

		{
			cFILE.getflag ( "TEXT FILE", "strOriginalCountry", 0, 1, strOriginalCountry );
			cFILE.getflag ( "TEXT FILE", "strCountry", 0, 1, strCountry );
			cFILE.getflag ( "TEXT FILE", "strLauncherText", 0, 1, strLauncherText );
			cFILE.getflag ( "TEXT FILE", "strGameExText", 0, 1, strGameExText );
			cFILE.getflag ( "TEXT FILE", "strGameInText", 0, 1, strGameInText );
			cFILE.getflag ( "TEXT FILE", "strGameWord", 0, 1, strGameWord );
			cFILE.getflag ( "TEXT FILE", "strServerText", 0, 1, strServerText );
			cFILE.getflag ( "TEXT FILE", "strSkillText", 0, 1,	strSkillText );
			cFILE.getflag ( "TEXT FILE", "strItemText", 0, 1,	strItemText );
			cFILE.getflag ( "TEXT FILE", "strCrowText", 0, 1,	strCrowText );
			cFILE.getflag ( "TEXT FILE", "strTipText", 0, 1,	strTipText );
			cFILE.getflag ( "TEXT FILE", "strCommentText", 0, 1,strCommentText );
			cFILE.getflag ( "TEXT FILE", "strPetCommentText",0, 1, strPetCommentText );
			cFILE.getflag ( "TEXT FILE", "strQuestText", 0, 1,	strQuestText );
			cFILE.getflag ( "TEXT FILE", "strNPCTalkText", 0, 1,strNPCTalkText );
			cFILE.getflag ( "TEXT FILE", "strNPCAnswerText", 0, 1,strNPCAnswerText );

			cFILE.getflag ( "TEXT FILE", "strUIOuterCfg", 0, 1, strUIOuterCfg );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg01", 0, 1, strUIInnerCfg01 );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg02", 0, 1, strUIInnerCfg02 );
			cFILE.getflag ( "TEXT FILE", "strUIInnerCfg03", 0, 1, strUIInnerCfg03 );
			cFILE.getflag ( "TEXT FILE", "strUIExtCfg", 0, 1, strUIExtCfg );
		}

		emNation = language::StrNationToEMNation( strCountry );
		emProvideLangFlag = language::StringToProvidelang( strCountry );
		if ( language::LANGFLAG_TOTAL_NUM == emProvideLangFlag )
		{
			emProvideLangFlag = language::StringToProvidelang(strOriginalCountry);
			if ( language::LANGFLAG_TOTAL_NUM == emProvideLangFlag )
			{
				// [shhan][2015.04.01] 언어 복원이 불가능 할 경우 GS 로 동작되도록 한다.
				emProvideLangFlag = language::LANGFLAG_GS;
			}
		}

		// 도움말 웹 페이지 주소, 아직 사용 안함
		cFILE.getflag_sz( "ETC OPTION", "HomePageAddress", 0, 1, HomePageAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "HelpAddress", 0, 1, HelpAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "ItemShopAddress", 0, 1, ItemShopAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "ItemShopHPAddress", 0, 1, ItemShopHPAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "EventPageAddress", 0, 1, EventPageAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "ManualPatchAddress", 0, 1, ManualPatchAddress, STRING_NUM_128 );
		cFILE.getflag_sz( "ETC OPTION", "RPFDataPath", 0, 1, RPFDataPath, STRING_NUM_128 );
        cFILE.getflag_sz( "ETC OPTION", "CustomerCenter", 0, 1, CustomerCenterAddress, STRING_NUM_128 );

		//	Note : 유효성 체크.
		//
        // Note : 폰트 스타일을 무조건 D3DXFONT로 설정한다. 문제 없을 시에는 관련 설정 항목을 삭제하자.
// 		if ( GLUseFeatures::GetInstance().IsUsingPrivateDataIntegrate() )
// 			strFontType = CGameTextMan::GetInstance().GetCommentText("FONTSYS", 1).GetString();
// 		else
			strFontType = COMMENT::FONTSYS[1].c_str();

		/*if ( COMMENT::FONTSYS[0]!=strFontType.GetString() && COMMENT::FONTSYS[1]!=strFontType.GetString() )
		{
			strFontType = COMMENT::FONTSYS[0].c_str();
		}*/

		return TRUE;
	}

	
	BOOL LOAD_OPT(const TCHAR *szRootPath, EMSERVICE_PROVIDER ServiceProvider, bool bToolMode, bool bServer)
	{
		strOptionFile = strRootPath;
		strOptionFile += "\\option.ini";


		if ( !PathFileExists( strOptionFile.c_str() ) )
		{
			return FALSE;
		}

		CIniLoader cFILE(",", bToolMode);
		
		if( !cFILE.open( strOptionFile, true) )
		{
			return FALSE;
		}

		// param.ini use not error msg box
		cFILE.SetUseErrorMsgBox( FALSE );

		DWORD dwVer(0x100);
		cFILE.getflag("GAME OPTION", "dwVer", 0, 1, dwVer);

		//#ifndef KR_PARAM
        if (ServiceProvider != SP_KOREA && ServiceProvider != SP_KOREA_TEST)
		{
			if( !bServer )
			{
			//#ifndef TW_PARAM
			//	cFILE.getflag( "GAME OPTION", "bSAVE_USERID", 0, 1, bSAVE_USERID );
			//	if ( bSAVE_USERID )
			//#endif
				{
                    cFILE.getflag("GAME OPTION", "bSAVE_USERID", 0, 1, bSAVE_USERID);
					cFILE.getflag("GAME OPTION", "strENC_USERID", 0, 1, strENC_USERID);
					if (strIDNULL == strENC_USERID)
                        strENC_USERID = "";
				}
			}
			//cFILE.getflag ( "GAME OPTION", "bDISP_CONSOLE", 0, 1, bDISP_CONSOLE );
			//cFILE.getflag ( "GAME OPTION", "bDISP_FPS", 0, 1, bDISP_FPS );
		}
		//#endif

		cFILE.getflag( "GAME OPTION", "nChinaRegion", 0, 1, nChinaRegion );
		
		{
			cFILE.getflag ( "SCREEN OPTION", "bCHECKWHQL", 0, 1, bCHECKWHQL );
			cFILE.getflag ( "SCREEN OPTION", "dwScrWidth", 0, 1, dwScrWidth );
			cFILE.getflag ( "SCREEN OPTION", "dwScrHeight", 0, 1, dwScrHeight );
			int nTEMP(0);
			cFILE.getflag ( "SCREEN OPTION", "emScrFormat", 0, 1, nTEMP );
			emScrFormat = (EMSCREEN_FORMAT) nTEMP;
			cFILE.getflag ( "SCREEN OPTION", "uScrRefreshHz", 0, 1, uScrRefreshHz );
			cFILE.getflag ( "SCREEN OPTION", "bScrWindowed", 0, 1, bScrWindowed );
			cFILE.getflag ( "SCREEN OPTION", "bScrWndFullSize", 0, 1, bScrWndFullSize );
			cFILE.getflag ( "SCREEN OPTION", "bGameCursor", 0, 1, bGameCursor );
			cFILE.getflag ( "SCREEN OPTION", "bAllMinimum", 0, 1, bAllMinimum );
		}
		
		{
			cFILE.getflag ( "SOUND OPTION", "b3D_SOUND", 0, 1, b3D_SOUND );
			cFILE.getflag ( "SOUND OPTION", "nSndSfx", 0, 1, nSndSfx );
			cFILE.getflag ( "SOUND OPTION", "nSndMap", 0, 1, nSndMap );
			cFILE.getflag ( "SOUND OPTION", "nSndMusic", 0, 1, nSndMusic );
			cFILE.getflag ( "SOUND OPTION", "dw3DAlgorithm", 0, 1, dw3DAlgorithm );
			cFILE.getflag ( "SOUND OPTION", "dwSndBufferSize", 0, 1, dwSndBufferSize );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteSfx", 0, 1, bSndMuteSfx );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteMap", 0, 1, bSndMuteMap );
			cFILE.getflag ( "SOUND OPTION", "bSndMuteMusic", 0, 1, bSndMuteMusic );
		}

		{
			cFILE.getflag ( "GRAPHIC OPTION", "dwVideoLevel", 0, 1, dwVideoLevel );
			cFILE.getflag ( "GRAPHIC OPTION", "dwShadowChar", 0, 1, dwShadowChar );
			cFILE.getflag ( "GRAPHIC OPTION", "bBuff", 0, 1, bBuff );
			cFILE.getflag ( "GRAPHIC OPTION", "bRealReflect", 0, 1, bRealReflect );
			cFILE.getflag ( "GRAPHIC OPTION", "bRefract", 0, 1, bRefract );
			cFILE.getflag ( "GRAPHIC OPTION", "bGlow", 0, 1, bGlow );
			cFILE.getflag ( "GRAPHIC OPTION", "bPost", 0, 1, bPost );
			cFILE.getflag ( "GRAPHIC OPTION", "bFrameLimit", 0, 1, bFrameLimit );
			cFILE.getflag ( "GRAPHIC OPTION", "bTargetEffect", 0, 1, RENDERPARAM::bTargetEffect );
			cFILE.getflag ( "GRAPHIC OPTION", "bMineEffect", 0, 1, RENDERPARAM::bMineEffect );
			cFILE.getflag ( "GRAPHIC OPTION", "bClickEffect", 0, 1, RENDERPARAM::bClickEffect );
			cFILE.getflag ( "GRAPHIC OPTION", "bUseNormalMap", 0, 1, RENDERPARAM::bUseNormalMap );
            cFILE.getflag ( "GRAPHIC OPTION", "bVolumeLight", 0, 1, RENDERPARAM::bVolumeLight );
			cFILE.getflag ( "GRAPHIC OPTION", "dwSkinDetail", 0, 1, dwSkinDetail );
			cFILE.getflag ( "GRAPHIC OPTION", "dwAutoOptionLoad", 0, 1, dwAutoOptionLoad );
			cFILE.getflag ( "GRAPHIC OPTION", "dwFogRange", 0, 1, dwFogRange );
			cFILE.getflag ( "GRAPHIC OPTION", "bBUFF_SFX", 0, 1, bBUFF_SFX );
			cFILE.getflag ( "GRAPHIC OPTION", "bEMOTICON", 0, 1, bEMOTICON );
			cFILE.getflag ( "GRAPHIC OPTION", "dwTexFilter", 0, 1, RENDERPARAM::g_emTextureFiltering );
			cFILE.getflag ( "GRAPHIC OPTION", "dwTexQuality", 0, 1, RENDERPARAM::g_emTextureQuality );
			RENDERPARAM::SetTextureQuality( RENDERPARAM::g_emTextureQuality );
			cFILE.getflag ( "GRAPHIC OPTION", "dwPhysXCloth", 0, 1, RENDERPARAM::g_emPhysXCloth );

            // Note : 폰트 스타일을 무조건 D3DXFONT로 설정한다. 문제 없을 시에는 관련 설정 항목을 삭제하자.
// 			if ( GLUseFeatures::GetInstance().IsUsingPrivateDataIntegrate() )
// 				strFontType = CGameTextMan::GetInstance().GetCommentText("FONTSYS", 1).GetString();
// 			else
				strFontType = COMMENT::FONTSYS[1].c_str();
			//cFILE.getflag ( "GRAPHIC OPTION", "strFontType", 0, 1, strFontType, true );
		}

		// 기존에는 3 개였지만.
		// 지금은 4개로 늘었고 앞부분이 늘어나서 1을 더해줌.
		if ( dwVer <= 0x100 )
		{
			if ( dwFogRange >= 0 && dwFogRange <= 2 )
			{
				dwFogRange += 1;
				if ( dwFogRange > EMFR_HIGH )
					dwFogRange = EMFR_HIGH;
			}
			else
			{
				// 이전 버젼 런쳐를 사용할 경우 dwFogRange 값에 문제가 발생할 수 있어서 보완함.
				dwFogRange = EMFR_HIGH;
			}
		}

		//if ( !bScrWndHalfSize )		bScrWndFullSize = TRUE;	// 필요없는 데이터

		return TRUE;		
	}

	BOOL SAVE_PARAM( const EMSERVICE_PROVIDER ServiceProvider, const BOOL bLauncher )
	{
		if ( strParamFile.empty() )
			return FALSE;

		CString strBuffer;

		strBuffer += _T("[SERVER SET]\n");
		strBuffer.AppendFormat( "LoginAddress = %s\n",			LoginAddress );
		strBuffer.AppendFormat( "WB_LoginAddress = %s\n",		WB_LoginAddress );
		strBuffer.AppendFormat( "WB_LoginServerPort = %d\n",	WB_LoginServerPort );

		strBuffer.AppendFormat( "HttpAddress = " );
		for ( UINT i = 0; i < MAX_HTTP; ++i )
		{
			strBuffer.AppendFormat( "%s", HttpAddressTable[i] );
			
			if ( strlen( HttpAddressTable[i+1] ) == 0 )
				break;
			else
				strBuffer.AppendFormat( ", " );
		}
		strBuffer.AppendFormat("\n");
		strBuffer.AppendFormat( "strNewsURL = %s\n",		strNewsURL );
		strBuffer.AppendFormat( "strImageURL = %s\n",		strImageURL );
		strBuffer.AppendFormat( "strConnectionURL = %s\n",	strConnectionURL );
		strBuffer.AppendFormat( "strForumURL = %s\n",		strForumURL );
		strBuffer.AppendFormat( "bUsePassiveDN = %d\n",		bUsePassiveDN );

		if ( bLauncher == TRUE && emProvideLangFlag == language::LANGFLAG_CHINA )
		{
			strBuffer.AppendFormat( "nChinaRegion = %d\n", nChinaRegion );
		}

		CString strSection;
		for ( UINT i = 0; i < MAX_CHINA_REGION; ++i )
		{
			if ( China_Region[i].strName.IsEmpty() )
				break;

			strSection += _T(";\n");
			strSection.AppendFormat( "[CH_REGION_%d]\n", i );
			strSection.AppendFormat( "strName = %s\n",		China_Region[i].strName );
			strSection.AppendFormat( "LoginAddress = %s\n",	China_Region[i].LoginAddress );

			strSection.AppendFormat( "HttpAddress = " );
			for ( UINT j = 0; j < MAX_HTTP; ++j )
			{
				strSection.AppendFormat( "%s", China_Region[i].HttpAddressTable[j] );

				if ( strlen( China_Region[i].HttpAddressTable[j+1] ) == 0 )
					break;
				else
					strSection.AppendFormat( ", " );
			}
			strSection.AppendFormat("\n");
			strSection.AppendFormat( "strNewsURL = %s\n",		China_Region[i].strNewsURL );
			strSection.AppendFormat( "strImageURL = %s\n",	China_Region[i].strImageURL );
			strSection.AppendFormat( "ServerName = " );
			for ( UINT j = 0; j < MAX_HTTP; ++j )
			{
				strSection.AppendFormat( "%s", China_Region[i].ServerName[j] );

				if ( strlen( China_Region[i].ServerName[j+1] ) == 0 )
					break;
				else
					strSection.AppendFormat( ", " );
			}
			strSection.AppendFormat("\n");
			strSection.AppendFormat( "HomePageAddress = %s\n",	China_Region[i].strHomePage);
			strSection.AppendFormat( "ItemShopAddress = %s\n",	China_Region[i].strItemShop);
			strSection.AppendFormat( "ItemShopHPAddress = %s\n",	China_Region[i].strItemShopHP);
			strSection.AppendFormat( "EventPageAddress = %s\n",	China_Region[i].strEventPage);
			strSection.AppendFormat( "ManualPatchAddress = %s\n",	China_Region[i].strManualPath);
		}
		if ( strSection.IsEmpty() == false )
			strBuffer += strSection;

		strBuffer += _T(";\n");
		
		strBuffer += _T("[GUI OPTION]\n");
		strBuffer += _T("; FontType = [D3DFONT,D3DXFONT]\n; LangSet = [def=0, chn_tra=1, jp=2, kr=3, chn_sim=4, th=5, vn=6]\n; GDIFont = [돋움체,Fixedsys]\n"); // 빼고싶다...;
		// strBuffer.Append( "strFontType = %s\n", D3DXFONT); // D3DXFONT는 여기서 값이있어도 아무런 영향이없다;

		if ( bLauncher == TRUE )
		{
			const DWORD dwTempLang = language::ProvidelangToLangflag( emProvideLangFlag );
			strBuffer.AppendFormat( "dwLangSet = %d\n",		dwTempLang );
			strBuffer.AppendFormat( "strGDIFont = %s\n",	CD3DFontPar::szEnglishFontName[dwTempLang] );
		}
		else
		{
			strBuffer.AppendFormat( "dwLangSet = %d\n",		RENDERPARAM::emLangSet );
			strBuffer.AppendFormat( "strGDIFont = %s\n",	strGDIFont );
		}

		strBuffer += _T(";\n");
		
		strBuffer += _T("[TEXT FILE]\n");
		strBuffer.AppendFormat( "; 국가별 옵션은 XML파일의 태그 옵션과 일치해야 한다.\n");
		strBuffer.AppendFormat( "strOriginalCountry = %s\n",	strOriginalCountry );
		// strBuffer.AppendFormat( "strCountry = %s\n",			( bLauncher == TRUE ? language::strNationKeyWord[emNation] : strCountry) );
		strBuffer.AppendFormat( "strCountry = %s\n",			( bLauncher == TRUE ? language::strProvideLanguage[emProvideLangFlag] : strCountry) );
		strBuffer.AppendFormat( "strLauncherText = %s\n",		strLauncherText );
		strBuffer.AppendFormat( "strGameInText = %s\n",		strGameInText );
		strBuffer.AppendFormat( "strGameExText = %s\n",		strGameExText );
		strBuffer.AppendFormat( "strGameWord = %s\n",			strGameWord );
		strBuffer.AppendFormat( "strServerText = %s\n",		strServerText );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strBuffer.AppendFormat( "strSkillText = %s\n",		strSkillText );
			strBuffer.AppendFormat( "strItemText = %s\n",		strItemText );
			strBuffer.AppendFormat( "strCrowText = %s\n",		strCrowText );
			strBuffer.AppendFormat( "strTipText = %s\n",		strTipText );
			strBuffer.AppendFormat( "strCommentText = %s\n",	strCommentText );
			strBuffer.AppendFormat( "strPetCommentText = %s\n",	strPetCommentText );
			strBuffer.AppendFormat( "strQuestText = %s\n",		strQuestText );
			strBuffer.AppendFormat( "strNPCTalkText = %s\n",	strNPCTalkText );
			strBuffer.AppendFormat( "strNPCAnswerText = %s\n",	strNPCAnswerText );
		}

		strBuffer.AppendFormat( "strUIOuterCfg = %s\n",		strUIOuterCfg );
		strBuffer.AppendFormat( "strUIInnerCfg01 = %s\n",		strUIInnerCfg01 );
		strBuffer.AppendFormat( "strUIInnerCfg02 = %s\n",		strUIInnerCfg02 );
		strBuffer.AppendFormat( "strUIInnerCfg03 = %s\n",		strUIInnerCfg03 );
		strBuffer.AppendFormat( "strUIExtCfg = %s\n",			strUIExtCfg );
		strBuffer += _T(";\n");
		
		strBuffer += _T("[ETC OPTION]\n");
		strBuffer.AppendFormat( "; 웹 주소 예시 : http://ran.hicocoon.com/index.jsp \n; 폴더 주소 예시 : \Data\Help\Help.htm\n" );
		strBuffer.AppendFormat( "HelpAddress = %s\n",			HelpAddress );
		strBuffer.AppendFormat( "HomePageAddress = %s\n",		HomePageAddress );
		strBuffer.AppendFormat( "ItemShopAddress = %s\n",		ItemShopAddress );
		strBuffer.AppendFormat( "ItemShopHPAddress = %s\n",	ItemShopHPAddress );
		strBuffer.AppendFormat( "EventPageAddress = %s\n",	EventPageAddress );
		strBuffer.AppendFormat( "ManualPatchAddress = %s\n",	ManualPatchAddress );
		strBuffer.AppendFormat( "RPFDataPath = %s\n",			RPFDataPath );
		
		sc::CRijndael oRijndael;
		oRijndael.Initialize(sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

		int nLen = oRijndael.GetEncryptStringLength( strBuffer );
		char * szBuffer = new char[nLen];
		memset( szBuffer, 0, nLen );

		oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
		int nVersion = sc::CRijndael::VERSION;

		if ( nVersion <= 0 && strlen( szBuffer ) <= 0 )	return FALSE;

		CFile file;
		if ( ! file.Open( strParamFile.c_str(), CFile::modeCreate|CFile::modeWrite ) )	// 파일 열기
		{
			delete [] szBuffer;
			return FALSE;
		}

		file.Write( &nVersion, sizeof(int) );	// 버젼 쓰기;
		file.Write( szBuffer, nLen );			// 파일 쓰기;
		file.Close();

		delete [] szBuffer;

		return TRUE;
	}
};


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
		CharAlly	= 0x00000001,	// ĳ���� - �Ʊ� Character - Ally
		CharEnemy	= 0x00000002,	// ĳ���� - ���� Character - Enemy
		Mob			= 0x00000004,	// ��  Mob
		NPC			= 0x00000008,	// NPC
		Summon		= 0x00000010,	// ��ȯ�� Summoner
		Item		= 0x00000020,	// ��� ������ Drop Items

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

	//	���� ����.
	extern std::string		strENC_CHARID;
	extern BOOL				bSAVE_USERID;

	extern bool				bExchangeCheck;		//	'��ȯ' üũ/��üũ    Check/uncheck 'Exchange'
	extern BOOL				bDIS_CONFT;			//	'�ڵ�' ��� �ź�. 'Automatic' sparring rejection.
	extern BOOL				bDIS_TRADE;			//	'�ڵ�' �ŷ� �ź�. 'Automatic' transaction rejection.
	extern BOOL				bDIS_PARTY;			//	'�ڵ�' ��Ƽ �ź�.  'Automatic' party rejection.
	extern BOOL				bDIS_FRIEND;
	extern BOOL				bDIS_VEHICLE;		//	'�ڵ�' ������� ž�� �ź�.  Refusal to ride 'automatic' motorcycles.
	extern BOOL				bSHOW_SIMPLEHP;
	extern BOOL				b3D_SOUND;			//	3���� ����   3D Sound
	extern BOOL				bSHOW_TIP;			//	�� �����ֱ�    Show tips
	extern BOOL				bMOVABLE_ON_CHAT;	//	ä��â ������ �����̱�  Move around the chat window
	extern BOOL				bFORCED_ATTACK;		//	���� ����
	extern BOOL				bNAME_DISPLAY;		//	�̸� �׻� ǥ��, ���� ���� ���鸸 ����Ѵ�;
	extern BOOL				bNON_Rebirth;		//	��Ȱ ��ų ����
	extern BOOL				bSIMPLE_SUM_NAME;   //  ��ȯ�� �̸� �����ϰ� ǥ�� ����
	extern BOOL             bAUTO_CROW_HP;      //  �ڽ��� Ÿ������ ���� HP�� �ڵ�ǥ��
	extern BOOL				bCharInfoPrivate;	//	�� ���� �����
	extern BOOL				bCharInfoAddition;	//	ĳ���� �߰� ����â ǥ��
	extern BOOL				bSKILL_RESERVATION; //  ��ų ����
	extern BOOL				bGroupChatInvite; //  '�ڵ�' �׷�ä�� �ź�
	extern EM_AROUND_DETAILTYPE	emAroundDetail;		//  �ֺ� ĳ���� �ܼ�ȭ ���
	extern EM_SKILLTYPE_UI	emSkillTypeUI;		//	��ų Ÿ�� UI ǥ��


	extern BOOL				bCONFT_BOUND;		//	��ý� ���� ���� ����.
	extern WORD				wCONFT_RECOVER;		//	��ý� ȸ���� ����.
	extern float			fCONFT_HP_RATE;		//	��ý� ü�� Ȯ�� ����.
	extern float			fCONFT_TAR_HP_RATE;	//	��ý� ü�� Ȯ�� ���� [ ����(���δ�ýø�) ].

	extern DWORD			dwPARTY_GET_ITEM;	//	��Ƽ�ɼ� ������ �й� ��Ģ.
	extern DWORD			dwPARTY_GET_ITEM_Filter; // ��Ƽ �ɼ� ������ �й� ��Ģ�� ���
	extern DWORD			dwPARTY_GET_MONEY;	//	��Ƽ�ɼ� �� �й� ��Ģ.
	extern DWORD			dwPARTY_SCHOOL; // ��Ƽ �ɼ� ���� �п���;
	
	// (bjju) ��Ʈ�� �������� ���� ������� �ʽ��ϴ�
	extern int				QuickSlot[QUICK_SLOT_NUM]; // ������
	extern int				SkillSlot[QUICK_SKILL_NUM]; // ����ų
	extern int				MenuShotcut[QUICK_MENU_NUM]; // �޴�����Ű
	extern int				DefaultShotcutKey[QUICK_ALL_NUM]; // ����Ʈ Ű Ÿ��
	extern int				KeySettingToRanparam[QUICK_MENU_NUM];
	extern int				BasicMenuToRanparam[BASICMENU_NUM];

	extern CString			ChatMacro[CHAT_MACRO_NUM];

    extern DWORD			QuestDisplayID[MAX_QUEST_DISPLAY]; // ����Ʈ ���� ���

    extern std::string      strPointShopSearchWord[MAX_SEARCH_WORD]; // ����Ʈ �������� ã�ƺ� �ܾ�.
    extern DWORD            dwPointShopWishList[MAX_POINT_SHOP_WISH_LIST]; // ����Ʈ ���� ���ι� ����Ʈ.

    extern bool             bCheckAutoRebirth; // ��Ȱ�� ��Ȱ�ϱ�� ��Ȱ üũ.

    extern bool             bCheckUsableItemMarketPage; // NPC ���� ui���� ��� ������ ������ üũ.
    extern bool             bCheckBuyableItemMarketPage; // NPC ���� ui���� ���� ������ ������ üũ.
    extern bool             bCheckRebuySuppliesRebuyPage; // NPC ����� ui���� �Ҹ�ǰ ������ üũ.
    extern bool             bCheckAllDelItemsRebuyPage; // NPC ����� ui���� �������� �� ����� ������ Ȯ��.
    extern bool             bCheckSellItemRButton; // NPC ���� ui���� ���콺 ��Ŭ������ ������ �Ǹ� üũ.
    extern bool             bCheckConfirmSellItem; // NPC ���� ui���� ������ �Ǹ� �� Ȯ��.

    extern DWORD            dwCharacterTitleIndex; // ĳ���� ����(Ÿ��Ʋ) ������ ���.

	//	���÷��� ����.
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

	//	�׷��� ����.
	extern DWORD			dwVideoLevel;
	extern DWORD			dwShadowChar;	//	0~4
	extern BOOL				bBuff;
	//  [11/25/2016 gbgim];
	// �Ʒ��� ����� '���� ǰ��'�� ���Ե�, �ſ쳷��-    The following features are included in 'World Quality', Very Low false, esle true;
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

	// ���� ����;
	extern language::SERVICENATION	emNation;
	// SERVICENATION   need to be switched down;   �� �Ʒ��� ��ȯ�ؾߵ�;
	extern language::EMPROVIDE_LANGFLAG emProvideLangFlag;

	// �븻�� ����Ҽ� �ִ��� ����  Whether normal maps can be used
	extern BOOL				bPossibleNormalMap;

	extern BOOL				bBUFF_SFX;
	extern BOOL				bEMOTICON;

	//	�þ�
	extern DWORD			dwFogRange;

	extern LONG				nSndSfx;
	extern LONG				nSndMap;
	extern LONG				nSndMusic;
	extern DWORD			dw3DAlgorithm;
	extern DWORD			dwSndBufferSize;

	extern BOOL				bSndMuteSfx;
	extern BOOL				bSndMuteMap;
	extern BOOL				bSndMuteMusic;	

	//	���� ����.  Server settings.
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
	extern UINT				nChinaRegion; // �߱� - ��ġ���� ����(MAX_CHINA_REGION:�����, 0~3:��������)
	extern TCHAR			ItemShopHPAddress[STRING_NUM_128];
	extern TCHAR			EventPageAddress[STRING_NUM_128];
	extern TCHAR			ManualPatchAddress[STRING_NUM_128];
	extern TCHAR			HomePageAddress[STRING_NUM_128];
    extern TCHAR			CustomerCenterAddress[STRING_NUM_128]; // ������ �ּ�.


	// �߱� ������ ���� ���� ����  Server settings according to China region
	extern SERVER_SET		China_Region[MAX_CHINA_REGION];

	//	��� ��Ʈ ����.
//	extern language::LANGFLAG emLangSet;

	extern CString			strGDIFont;
	extern CString			strFontType;

	//	�⺻ ���� ����. ( �� txt ) Default settings file. (language specific txt)
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

    extern CString          strUITextureList;   // ���ο� UI �ؽ�ó ����  New UI texture management
    extern CString          strUIXmlList;       // ���ο� UI ��Ʈ�� ���� Managing new UI controls

    // ���� �ε���
    extern DWORD		    dwServerNum;

    // SNS ����
    extern SNSFACEBOOK_SET  sSNSFaceBook;
    extern SNSTWITTER_SET   sSNSTwitter;

	// ���� �� ������ �ּ�
	extern TCHAR			HelpAddress[STRING_NUM_128];
	extern TCHAR			ItemShopAddress[STRING_NUM_128];
	extern TCHAR			RPFDataPath[STRING_NUM_128];

	//  [4/27/2015 gbgim];
	// ������ ���չ������� ���� UI ���½� ����ϴ� html;  HTML used when opening the help UI in the data integration version;
	extern TCHAR			MultiHelpAddress[language::LANGFLAG_TOTAL_NUM][STRING_NUM_128];

	extern DWORD			dwNameDisplay;		// �̸� ���� ����
	extern bool				bShowOtherDamage;	// �ٸ� ���� ������ ����
	extern int				nUIAntiAliasing;	// UI ��Ƽ �ٸ����
	extern bool				bDisplayLimit;		// �̸�, ������ ���÷��� ���� ����

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

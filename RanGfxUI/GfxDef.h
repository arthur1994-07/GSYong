#pragma once

const int Disable_View_Level = -128;

// 로딩 화면 타입
enum ELoadingSceneType
{
	Intro = 0,
	MoveMap,
	LogOut,
};

// 아이콘 슬롯 타입 ( 수정시 WidgetDef.lua 파일 같이 수정 )
enum EMGFX_SLOT_TYPE
{
	EMGFX_SLOT_BASE							= 0,
	EMGFX_SLOT_TOOLTIP						= 1,	// 툴팁 슬롯
	EMGFX_SLOT_ITEMBASE						= 2,	// 아이템 슬롯
	EMGFX_SLOT_SKILLBASE					= 3,	// 스킬 슬롯

	EMGFX_SLOT_INVENTORY					= 4,	// 인벤토리 슬롯
	EMGFX_SLOT_CHARINFO						= 5,	// 캐릭터 정보 장비 슬롯
	EMGFX_SLOT_INVEN_BANK					= 6,	// 인벤토리 은행 슬롯
	EMGFX_SLOT_INVEN_WISHLIST				= 7,	// 인벤토리 위시리스트 슬롯
	EMGFX_SLOT_INVEN_TRASH					= 8,	// 인벤토리 휴지통 슬롯
	EMGFX_SLOT_SQS							= 9,	// 스킬 퀵슬롯
	EMGFX_SLOT_IQS							= 10,	// 아이템 퀵슬롯
	EMGFX_SLOT_SKILL						= 11,	// 스킬창 슬롯
	EMGFX_SLOT_VEHICLE						= 12,	// 탈것창 스롯
	EMGFX_SLOT_NPCMARKET					= 13,	// NPC 판매 아이템 슬롯
	EMGFX_SLOT_NPCCART						= 14,	// NPC 카트 아이템 슬롯
	EMGFX_SLOT_PRODUCT						= 15,	// 제작 슬롯
	EMGFX_SLOT_TRADE						= 16,	// 거래 슬롯
	EMGFX_SLOT_POINTSHOP					= 17,	// 포인트샾 슬롯
	EMGFX_SLOT_POINTSHOP_CART				= 18,	// 포인트샾 카트 슬롯
	EMGFX_SLOT_PMSPOINTTRADE				= 19,	// 개인 상점 검색 포인트 거래 아이템 등록 슬롯
	EMGFX_SLOT_POSTBOX						= 21,	// 우편 슬롯
	EMGFX_SLOT_POSTBOX_SUB					= 22,	// 우편 보조슬롯
	EMGFX_SLOT_REFERCHAR					= 23,	// 정보보기 슬롯
    EMGFX_SLOT_PET							= 24,   // 펫 장비 슬롯
	EMGFX_SLOT_ATTENDANCE_AWARD				= 25,	// 생활기록부 출석부 슬롯
	EMGFX_SLOT_ACTIVITY_SALES				= 26,	// 특별활동 판매아이템 슬롯
	EMGFX_SLOT_MINIGAME_DICE_REWARD			= 27,	// 미니게임 : 주사위, 보상 아이템 슬롯;
	EMGFX_SLOT_CLUBSTORAGE					= 28,   // 클럽창고 슬롯
	EMGFX_SLOT_REBUILD_CARD_ITEM			= 29,   // 개조카드 아이템 슬롯
	EMGFX_SLOT_REBUILD_CARD_STEMP			= 30,   // 개조카드 인장 슬롯
	EMGFX_SLOT_ITEM_PREVIEW					= 31,   // 아이템 미리보기 슬롯
	EMGFX_SLOT_PRIVATEMARKET				= 32,	// 개인상점 슬롯
	EMGFX_SLOT_LOCKER						= 33,	// 락커 슬롯
	EMGFX_SLOT_REFERCHAR_SKILL				= 34,	// 정보보기 스킬 슬롯
	EMGFX_SLOT_BUFF							= 35,	// 버프 슬롯
	EMGFX_SLOT_SETITEM						= 36,	// 세트 아이템 슬롯
	EMGFX_SLOT_SUMMONSTATE					= 37,	// 소환수 정보;
	EMGFX_SLOT_PARTYDISTRIBUTION			= 38,	// 파티 분배 슬롯;
	EMGFX_SLOT_PRIVATEMARKETSEARCH			= 39,	// 개인 상점 검색
	EMGFX_SLOT_ITEMLINKINFO					= 40,	// 아이템 링크 정보
};

// GfxTooltipSlot이 가지고있다;
// AS BaseTooltipUnit.as의 TooltipType과 일치해야한다;
enum EMGFX_TOOLTIP_TYPE
{
	EMGFX_TOOLTIPTYPE_BASIC		 = 0,
	EMGFX_TOOLTIPTYPE_SUB_SKILL	 = 1,
	EMGFX_TOOLTIPTYPE_SUB_ITEM	 = 2,
};

// 언어별 폰트
enum EMGFX_FONT_LANG
{
	EMGFX_FONT_NONE = -1,

	EMGFX_FONT_KOREAN = 0,		// 한국어
	EMGFX_FONT_ENGLISH,			// 영어
	EMGFX_FONT_CHINESE,			// 중국어
	EMGFX_FONT_THAI,			// 태국어
	EMGFX_FONT_UNKNOWN,
	
	EMGFX_FONT_SIZE
};

// AS 리턴 타입
enum EMGFX_OBJECT_TYPE
{
	EMGFX_OBJ_TYPE_NULL = 0,
	EMGFX_OBJ_TYPE_NUMBER,
	EMGFX_OBJ_TYPE_BOOL,
	EMGFX_OBJ_TYPE_STRING,
	EMGFX_OBJ_TYPE_INT,
	EMGFX_OBJ_TYPE_UINT
};

namespace NSGFXFILE
{
	// 확장명
#if defined( _RELEASED ) || defined( _DEBUG )
	const std::string g_strExtension = ".swf";
#else
	const std::string g_strExtension = ".gfx";
#endif

	// 데이터 경로
	extern std::string g_strGfxPath;
	// 텍스처 경로
	extern std::string g_strTexturePath;

	// Main
	extern std::string g_strMainFileName;
	// IME
	extern std::string g_strIMEFileName;
	// Loading Screen
	extern std::string g_strLoadingFileName;
	// Login Scene
	extern std::string g_strLoginSceneName;
	// Debug Scene
	extern std::string g_strDebugSceneName;

	// 폰트 설정 파일
	const std::string g_strFontConfigFileName = "fontconfig.txt";
	// 기본 폰트 라이브러리
	const std::string g_strFontLib = "fonts_kr";
}

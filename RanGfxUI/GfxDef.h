#pragma once

const int Disable_View_Level = -128;

// �ε� ȭ�� Ÿ��
enum ELoadingSceneType
{
	Intro = 0,
	MoveMap,
	LogOut,
};

// ������ ���� Ÿ�� ( ������ WidgetDef.lua ���� ���� ���� )
enum EMGFX_SLOT_TYPE
{
	EMGFX_SLOT_BASE							= 0,
	EMGFX_SLOT_TOOLTIP						= 1,	// ���� ����
	EMGFX_SLOT_ITEMBASE						= 2,	// ������ ����
	EMGFX_SLOT_SKILLBASE					= 3,	// ��ų ����

	EMGFX_SLOT_INVENTORY					= 4,	// �κ��丮 ����
	EMGFX_SLOT_CHARINFO						= 5,	// ĳ���� ���� ��� ����
	EMGFX_SLOT_INVEN_BANK					= 6,	// �κ��丮 ���� ����
	EMGFX_SLOT_INVEN_WISHLIST				= 7,	// �κ��丮 ���ø���Ʈ ����
	EMGFX_SLOT_INVEN_TRASH					= 8,	// �κ��丮 ������ ����
	EMGFX_SLOT_SQS							= 9,	// ��ų ������
	EMGFX_SLOT_IQS							= 10,	// ������ ������
	EMGFX_SLOT_SKILL						= 11,	// ��ųâ ����
	EMGFX_SLOT_VEHICLE						= 12,	// Ż��â ����
	EMGFX_SLOT_NPCMARKET					= 13,	// NPC �Ǹ� ������ ����
	EMGFX_SLOT_NPCCART						= 14,	// NPC īƮ ������ ����
	EMGFX_SLOT_PRODUCT						= 15,	// ���� ����
	EMGFX_SLOT_TRADE						= 16,	// �ŷ� ����
	EMGFX_SLOT_POINTSHOP					= 17,	// ����Ʈ�� ����
	EMGFX_SLOT_POINTSHOP_CART				= 18,	// ����Ʈ�� īƮ ����
	EMGFX_SLOT_PMSPOINTTRADE				= 19,	// ���� ���� �˻� ����Ʈ �ŷ� ������ ��� ����
	EMGFX_SLOT_POSTBOX						= 21,	// ���� ����
	EMGFX_SLOT_POSTBOX_SUB					= 22,	// ���� ��������
	EMGFX_SLOT_REFERCHAR					= 23,	// �������� ����
    EMGFX_SLOT_PET							= 24,   // �� ��� ����
	EMGFX_SLOT_ATTENDANCE_AWARD				= 25,	// ��Ȱ��Ϻ� �⼮�� ����
	EMGFX_SLOT_ACTIVITY_SALES				= 26,	// Ư��Ȱ�� �Ǹž����� ����
	EMGFX_SLOT_MINIGAME_DICE_REWARD			= 27,	// �̴ϰ��� : �ֻ���, ���� ������ ����;
	EMGFX_SLOT_CLUBSTORAGE					= 28,   // Ŭ��â�� ����
	EMGFX_SLOT_REBUILD_CARD_ITEM			= 29,   // ����ī�� ������ ����
	EMGFX_SLOT_REBUILD_CARD_STEMP			= 30,   // ����ī�� ���� ����
	EMGFX_SLOT_ITEM_PREVIEW					= 31,   // ������ �̸����� ����
	EMGFX_SLOT_PRIVATEMARKET				= 32,	// ���λ��� ����
	EMGFX_SLOT_LOCKER						= 33,	// ��Ŀ ����
	EMGFX_SLOT_REFERCHAR_SKILL				= 34,	// �������� ��ų ����
	EMGFX_SLOT_BUFF							= 35,	// ���� ����
	EMGFX_SLOT_SETITEM						= 36,	// ��Ʈ ������ ����
	EMGFX_SLOT_SUMMONSTATE					= 37,	// ��ȯ�� ����;
	EMGFX_SLOT_PARTYDISTRIBUTION			= 38,	// ��Ƽ �й� ����;
	EMGFX_SLOT_PRIVATEMARKETSEARCH			= 39,	// ���� ���� �˻�
	EMGFX_SLOT_ITEMLINKINFO					= 40,	// ������ ��ũ ����
};

// GfxTooltipSlot�� �������ִ�;
// AS BaseTooltipUnit.as�� TooltipType�� ��ġ�ؾ��Ѵ�;
enum EMGFX_TOOLTIP_TYPE
{
	EMGFX_TOOLTIPTYPE_BASIC		 = 0,
	EMGFX_TOOLTIPTYPE_SUB_SKILL	 = 1,
	EMGFX_TOOLTIPTYPE_SUB_ITEM	 = 2,
};

// �� ��Ʈ
enum EMGFX_FONT_LANG
{
	EMGFX_FONT_NONE = -1,

	EMGFX_FONT_KOREAN = 0,		// �ѱ���
	EMGFX_FONT_ENGLISH,			// ����
	EMGFX_FONT_CHINESE,			// �߱���
	EMGFX_FONT_THAI,			// �±���
	EMGFX_FONT_UNKNOWN,
	
	EMGFX_FONT_SIZE
};

// AS ���� Ÿ��
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
	// Ȯ���
#if defined( _RELEASED ) || defined( _DEBUG )
	const std::string g_strExtension = ".swf";
#else
	const std::string g_strExtension = ".gfx";
#endif

	// ������ ���
	extern std::string g_strGfxPath;
	// �ؽ�ó ���
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

	// ��Ʈ ���� ����
	const std::string g_strFontConfigFileName = "fontconfig.txt";
	// �⺻ ��Ʈ ���̺귯��
	const std::string g_strFontLib = "fonts_kr";
}

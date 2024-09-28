#include "pch.h"
#include "GLWidgetScriptDef.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanGfxUI/GfxDef.h"
#include "../Tooltip/GLItemTooltip.h"
#include "../Tooltip/GLSkillTooltip.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "./GLWidgetScript.h"
#include "../GfxInterfaceBase.h"

namespace NSWIDGET_SCRIPT
{
	// ����� ��?
	bool g_bWidgetDebug = false;

	// �̸� ���÷��� --------------------------------------- 
	float g_fDef_Display_ShowNameDistanceRatio = 0.5f;			// �̸� ����� �Ÿ� ����
	std::string g_strDef_Display_NameMC = "";					// �̸� ���÷���
	std::string g_strDef_Display_NameMC_tfName = "";			// �̸� �ؽ�Ʈ �ʵ� �ν��Ͻ�
	std::string g_strDef_Display_NameMC_mcNameBG = "";			// �̸� �ؽ�Ʈ ���
	std::string g_strDef_Display_NameMC_tfClub = "";			// Ŭ�� �ؽ�Ʈ �ʵ� �ν��Ͻ�
	std::string g_strDef_Display_NameMC_mcClubBG = "";			// Ŭ�� �ؽ�Ʈ ���
	std::string g_strDef_Display_NameMC_mcSchool = "";			// �п� ������ ����Ŭ�� �ν��Ͻ�
	std::string g_strDef_Display_NameMC_mcCountry = "";			// ���� �̹��� �ν��Ͻ�
	std::string g_strDef_Display_NameMC_mcParty = "";			// ��Ƽ ������ ����Ŭ�� �ν��Ͻ�
	std::string g_strDef_Display_NameMC_rtClubIcon = "";		// Ŭ�� ������ �����ؽ�ó �ν��Ͻ�
	std::string g_strDef_Display_NameMC_rtClubIconMC = "";		// Ŭ�� ������ �����ؽ�ó ����Ŭ�� ID
	std::string g_strDef_Display_NameMC_rtClubIconLink = "";	// Ŭ�� ������ ��Ʈ�� ID
	std::string g_strDef_Display_NameMC_barHP = "";				// HP ��
	int g_nDef_Display_NameMC_bgOffsetX = 0;					// �̸� ���÷��� ��� ������ X
	int g_nDef_Display_NameMC_bgOffsetY = 0;					// �̸� ���÷��� ��� ������ Y
	std::string g_strDef_Display_ItemNameMC = "";				// ������ �̸� ���÷��� Linkage

	// �п� ������ ������ ID
	std::string g_strDef_Display_NameMC_NoSchool = "";
	std::string g_strDef_Display_NameMC_SMSchool = "";
	std::string g_strDef_Display_NameMC_HASchool = "";
	std::string g_strDef_Display_NameMC_BHSchool = "";

	// ��Ƽ ������ ������ ID
	std::string g_strDef_Display_NameMC_NoParty = "";
	std::string g_strDef_Display_NameMC_PartyMaster = "";
	std::string g_strDef_Display_NameMC_PartyTarget = "";
	std::string g_strDef_Display_NameMC_PartyGeneral = "";

	// ����� ���÷��� ----------------------------------------
	std::string g_strDef_Display_HeadChatMC = "";		// ����� ���÷��� Linkage
	std::string g_strDef_Display_HeadChat_tf = "";		// �ؽ�Ʈ �ʵ� �ν��Ͻ�
	std::string g_strDef_Display_HeadChat_bg = "";		// ��� �ν��Ͻ�
	float g_fDef_Display_HeadChatLifeTime = 8.0f;		// ������ �ð�

	// ����� ���÷��� ----------------------------------------
	std::string g_strDef_Display_HpRedMC = "";
	std::string g_strDef_Display_HpGreenMC = "";
	std::string g_strDef_Display_HpBlueMC = "";
	std::string g_strDef_Display_HpGreenSimpleMC = "";

	std::string g_strDef_Display_Hp_tf = "";
	std::string g_strDef_Display_Hp_bar = "";

	// ������ ���÷��� -----------------------------------------
	std::string g_strDef_Display_DmgMc = "";			// ������ ����Ŭ��
	float g_fDef_Display_DmgLifeTime = 2.0f;			// ������ ���� �ð�
	float g_fDef_Display_DmgRandOffsetRange = 30.0f;	// ������ ��ġ ���� ������ ����
	float g_fDef_Display_DmgHeadOffsetX = 0.0f;			// ������ ��ġ �Ӹ��κ��� ������ X
	float g_fDef_Display_DmgHeadOffsetY = -50.0f;		// ������ ��ġ �Ӹ��κ��� ������ Y

	// ���λ��� ���÷��� ----------------------------------------
	std::string g_strDef_Display_PrivateMarket = "";
	std::string g_strDef_Display_PrivateMarket_Button = "";
	std::string g_strDef_Display_PrivateMarket_TextPM = "";
	std::string g_strDef_Display_PrivateMarket_TextName = "";
	std::string g_strDef_Display_PrivateMarket_TextPMName = "";

	int g_nDisplayLimitCount = 20;

	//----------------------------------------------------------------------------------

	bool g_bDef_WidgetCLoseToDeleteUseBatch = false;	// UI ���� ��� �ð� �ϰ� ���� ���?
	float g_fDef_WidgetCloseToDeleteTime = 300.0f;		// UI ���� �� ������ �ð�

	int g_nDef_SlotTooltipMaxWidth = 400;				// ���� ���� Width �ִ밪
	int g_nDef_SlotTooltipMinWidth = 200;				// ���� ���� Width �ּҰ�

	//----------------------------------------------------------------------------------

	DWORD g_dwDef_ItemSlotCountColor = 0xFFFFFFFF;		// ������ ���� ���� �÷�
	DWORD g_dwDef_ItemSlotMaxCountColor = 0xFFFFFFFF;	// ������ ���� �ִ� ���� �÷�
}

int NSWIDGET_SCRIPT::GetStringLength( const char* str )
{
	CStringW strTempW;
	if( EMGFX_FONT_KOREAN == GfxInterfaceBase::GetInstance()->GetFontLanguage() )
	{
		strTempW = AnsiToUnicode( str );
	}
	else
	{
		strTempW = sc::string::utf8ToUnicode( std::string( str ) ).c_str();
	}

	return strTempW.GetLength();
}

static int NSWIDGET_SCRIPT::StringFindFirst( const char* sz, const char* szFind )
{
	std::wstring strW = GfxInterfaceBase::GetInstance()->EncodeText( sz );
	std::wstring strFind = GfxInterfaceBase::GetInstance()->EncodeText( szFind );

	return strW.find( strFind );
}

std::string NSWIDGET_SCRIPT::SplitString( const char* str, int nBegin, int nEnd )
{
	std::wstring strTemp;
	if( EMGFX_FONT_KOREAN == GfxInterfaceBase::GetInstance()->GetFontLanguage() )
	{
		strTemp = AnsiToUnicode( str );
	}
	else
	{
		strTemp = sc::string::utf8ToUnicode( std::string( str ) ).c_str();
	}
	std::wstring strResult = L"";

	for( unsigned i = 0; i < strTemp.length(); ++i )
	{
		int nIndex = (int) i + 1;
		if( nIndex >= nBegin && nIndex <= nEnd )
		{
			wchar_t szChar = strTemp.at( i );

			CStringW strChar;
			strChar.Format( L"%c", szChar );

			strResult.append( strChar.GetBuffer() );
		}
	}

	return UnicodeToAnsi( strResult.c_str() );
}

NSWIDGET_SCRIPT::EMSKILL_BUFF_TYPE NSWIDGET_SCRIPT::SkillFactSlotToType( DWORD dwSlot )
{
	if( SKILLFACT_INDEX_NORMAL_START <= dwSlot && SKILLFACT_INDEX_NORMAL_END > dwSlot )
	{
		return EMBUFF_NORMAL;
	}
	else if( SKILLFACT_INDEX_ADDITIONAL_START <= dwSlot && SKILLFACT_INDEX_ADDITIONAL_END > dwSlot )
	{
		return EMBUFF_ADD;
	}
	else if( SKILLFACT_INDEX_LUNCHBOX_START <= dwSlot && SKILLFACT_INDEX_LUNCHBOX_END > dwSlot )
	{
		return EMBUFF_LUNCHBOX;
	}
	else if( SKILLFACT_INDEX_EXP_LUNCHBOX_START <= dwSlot && SKILLFACT_INDEX_EXP_LUNCHBOX_END > dwSlot )
	{
		return EMBUFF_EXP_LUNCHBOX;
	}
	else if( SKILLFACT_INDEX_SYSTEM_START <= dwSlot && SKILLFACT_INDEX_SYSTEM_END > dwSlot )
	{
		return EMBUFF_SYSTEM;
	}
	else if( SKILLFACT_INDEX_CTF_NORMAL_REWARD0 <= dwSlot && SKILLFACT_INDEX_CTF_NORMAL_REWARD2 >= dwSlot )
	{
		return EMBUFF_CTF_REWARD;
	}

	return EMBUFF_NONE;
}

std::string NSWIDGET_SCRIPT::GetItemInfoHeader( const SITEMCUSTOM& sItemCustom )
{
	LuaTable tbItemDataList( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLItemTooltip::GetInstance().GetItemDataHeader( sItemCustom, tbItemDataList ) )
		return "";

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbItemDataList );

	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetItemTooltipHeader, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GetItemInfo( const SITEMCUSTOM& sItemCustom, int nType )
{
	LuaTable tbItemDataList( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLItemTooltip::GetInstance().GetItemDataInfo( sItemCustom, tbItemDataList, nType, false ) )
		return "";

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbItemDataList );

	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetItemTooltip, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GetDurabilityInfoHeader(const SITEMCUSTOM& sItemCustom, int nType)
{
	LuaTable tbItemDataList( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLItemTooltip::GetInstance().GetItemDurabilityHeader(sItemCustom, tbItemDataList, nType, false) )
		return "";

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbItemDataList );

	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetItemDurabilityTooltipHeader, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GetDurabilityInfo(const SITEMCUSTOM& sItemCustom, int nType)
{
	LuaTable tbItemDataList( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLItemTooltip::GetInstance().GetItemDurability(sItemCustom, tbItemDataList, nType, false) )
		return "";

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbItemDataList );

	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetItemDurabilityTooltip, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GenerateItemInfoHeader( WORD wMID, WORD wSID )
{
	return GetItemInfoHeader( SITEMCUSTOM( SNATIVEID( wMID, wSID ) ) );
}

std::string NSWIDGET_SCRIPT::GenerateItemInfo( WORD wMID, WORD wSID )
{
	return GetItemInfo( SITEMCUSTOM( SNATIVEID( wMID, wSID ) ), 0 );
}

std::string NSWIDGET_SCRIPT::GetTypeText_SkillNextLevel()
{
	std::string strText(CGameTextMan::GetInstance().GetGameWord("SKILL_NEXTLEVEL").GetString());
	return strText;
}

std::string NSWIDGET_SCRIPT::GetSkillTypeText_MultiSkill()
{
	std::string strText(CGameTextMan::GetInstance().GetGameWord("MULTI_SKILL_TEXT").GetString());
	return strText;
}

std::string NSWIDGET_SCRIPT::GetSkillTypeText_LinkSkill()
{
	std::string strText(CGameTextMan::GetInstance().GetGameWord("LINK_SKILL_TEXT").GetString());
	return strText;
}

std::string NSWIDGET_SCRIPT::GetSkillTypeText_DelaySkill()
{
	std::string strText(CGameTextMan::GetInstance().GetGameWord("DEALY_ACTION_SKILL_TEXT").GetString());
	return strText;
}

std::string NSWIDGET_SCRIPT::GetSkillTypeText_ActiveSkill()
{
	std::string strText(CGameTextMan::GetInstance().GetGameWord("ACTIVE_ACTION_SKILL_TEXT").GetString());
	return strText;
}

std::string NSWIDGET_SCRIPT::GetSkillInfoHeader( const SNATIVEID& sID, WORD wSlotType, int nLevel )
{
	LuaTable tbHeader( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLSkillTooltip::GetInstance().GetHeader( sID, tbHeader, wSlotType, nLevel ) )
		return "";

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbHeader );
	
	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetSkillTooltipHeader, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GetSkillInfo( const SNATIVEID& sID, WORD wSlotType, int nLevel, bool bConditionCheck, int iDisguise )
{
	LuaTable tbTooltip( GLWidgetScript::GetInstance().GetLuaState() );
	if( false == GLSkillTooltip::GetInstance().GetTooltip( sID, tbTooltip, wSlotType, nLevel, iDisguise ) )
		return "";

	tbTooltip.set( "ConditionCheck", bConditionCheck );

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbTooltip );
	
	std::string strInfo = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		g_strFunc_GetSkillTooltip, &arg, 1 );

	return strInfo;
}

std::string NSWIDGET_SCRIPT::GenerateSkillInfoHeader( WORD wMID, WORD wSID, int nLevel )
{
	return GetSkillInfoHeader( SNATIVEID( wMID, wSID ), 0, nLevel );
}

std::string NSWIDGET_SCRIPT::GenerateSkillInfo( WORD wMID, WORD wSID, int nLevel )
{
	return GetSkillInfo( SNATIVEID( wMID, wSID ), 0, nLevel );
}
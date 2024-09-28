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
	// 디버깅 중?
	bool g_bWidgetDebug = false;

	// 이름 디스플레이 --------------------------------------- 
	float g_fDef_Display_ShowNameDistanceRatio = 0.5f;			// 이름 출력할 거리 비율
	std::string g_strDef_Display_NameMC = "";					// 이름 디스플레이
	std::string g_strDef_Display_NameMC_tfName = "";			// 이름 텍스트 필드 인스턴스
	std::string g_strDef_Display_NameMC_mcNameBG = "";			// 이름 텍스트 배경
	std::string g_strDef_Display_NameMC_tfClub = "";			// 클럽 텍스트 필드 인스턴스
	std::string g_strDef_Display_NameMC_mcClubBG = "";			// 클럽 텍스트 배경
	std::string g_strDef_Display_NameMC_mcSchool = "";			// 학원 아이콘 무비클립 인스턴스
	std::string g_strDef_Display_NameMC_mcCountry = "";			// 국가 이미지 인스턴스
	std::string g_strDef_Display_NameMC_mcParty = "";			// 파티 아이콘 무비클립 인스턴스
	std::string g_strDef_Display_NameMC_rtClubIcon = "";		// 클럽 아이콘 렌더텍스처 인스턴스
	std::string g_strDef_Display_NameMC_rtClubIconMC = "";		// 클럽 아이콘 렌더텍스처 무비클립 ID
	std::string g_strDef_Display_NameMC_rtClubIconLink = "";	// 클럽 아이콘 비트맵 ID
	std::string g_strDef_Display_NameMC_barHP = "";				// HP 바
	int g_nDef_Display_NameMC_bgOffsetX = 0;					// 이름 디스플레이 배경 오프셋 X
	int g_nDef_Display_NameMC_bgOffsetY = 0;					// 이름 디스플레이 배경 오프셋 Y
	std::string g_strDef_Display_ItemNameMC = "";				// 아이템 이름 디스플레이 Linkage

	// 학원 아이콘 프레임 ID
	std::string g_strDef_Display_NameMC_NoSchool = "";
	std::string g_strDef_Display_NameMC_SMSchool = "";
	std::string g_strDef_Display_NameMC_HASchool = "";
	std::string g_strDef_Display_NameMC_BHSchool = "";

	// 파티 아이콘 프레임 ID
	std::string g_strDef_Display_NameMC_NoParty = "";
	std::string g_strDef_Display_NameMC_PartyMaster = "";
	std::string g_strDef_Display_NameMC_PartyTarget = "";
	std::string g_strDef_Display_NameMC_PartyGeneral = "";

	// 헤드쳇 디스플레이 ----------------------------------------
	std::string g_strDef_Display_HeadChatMC = "";		// 헤드쳇 디스플레이 Linkage
	std::string g_strDef_Display_HeadChat_tf = "";		// 텍스트 필드 인스턴스
	std::string g_strDef_Display_HeadChat_bg = "";		// 배경 인스턴스
	float g_fDef_Display_HeadChatLifeTime = 8.0f;		// 보여질 시간

	// 헤드쳇 디스플레이 ----------------------------------------
	std::string g_strDef_Display_HpRedMC = "";
	std::string g_strDef_Display_HpGreenMC = "";
	std::string g_strDef_Display_HpBlueMC = "";
	std::string g_strDef_Display_HpGreenSimpleMC = "";

	std::string g_strDef_Display_Hp_tf = "";
	std::string g_strDef_Display_Hp_bar = "";

	// 데미지 디스플레이 -----------------------------------------
	std::string g_strDef_Display_DmgMc = "";			// 데미지 무비클립
	float g_fDef_Display_DmgLifeTime = 2.0f;			// 데미지 리셋 시간
	float g_fDef_Display_DmgRandOffsetRange = 30.0f;	// 데미지 위치 랜덤 오프셋 범위
	float g_fDef_Display_DmgHeadOffsetX = 0.0f;			// 데미지 위치 머리로부터 오프셋 X
	float g_fDef_Display_DmgHeadOffsetY = -50.0f;		// 데미지 위치 머리로부터 오프셋 Y

	// 개인상점 디스플레이 ----------------------------------------
	std::string g_strDef_Display_PrivateMarket = "";
	std::string g_strDef_Display_PrivateMarket_Button = "";
	std::string g_strDef_Display_PrivateMarket_TextPM = "";
	std::string g_strDef_Display_PrivateMarket_TextName = "";
	std::string g_strDef_Display_PrivateMarket_TextPMName = "";

	int g_nDisplayLimitCount = 20;

	//----------------------------------------------------------------------------------

	bool g_bDef_WidgetCLoseToDeleteUseBatch = false;	// UI 삭제 대기 시간 일괄 설정 사용?
	float g_fDef_WidgetCloseToDeleteTime = 300.0f;		// UI 닫은 후 삭제될 시간

	int g_nDef_SlotTooltipMaxWidth = 400;				// 슬롯 툴팁 Width 최대값
	int g_nDef_SlotTooltipMinWidth = 200;				// 슬롯 툴팁 Width 최소값

	//----------------------------------------------------------------------------------

	DWORD g_dwDef_ItemSlotCountColor = 0xFFFFFFFF;		// 아이템 슬롯 개수 컬러
	DWORD g_dwDef_ItemSlotMaxCountColor = 0xFFFFFFFF;	// 아이템 슬롯 최대 개수 컬러
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
#include "stdafx.h"
#include "GfxSlot.h"

#include "../../SigmaCore/String/StringUtils.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/Skill/GLSkillChangeList.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../../RanLogicClient/Crow/GLCrowClient.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"
#include "../../RanLogicClient/Input/GLInputManager.h"
#include "../../RanLogicClient/Chat/GLChatMan.h"
#include "../../RanLogicClient/NPCTalk/GLNpcTalkClient.h"
#include "../../RanLogicClient/StaticHoldItem.h"
#include "../../RanLogicClient/Land/GLLandManClient.h"
#include "../../RanLogicClient/ReferChar/ReferChar.h"
#include "../../RanLogicClient/Pet/GLPetClient.h"
#include "../../RanLogicClient/PointShop/PointShopClient.h"
#include "../../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "../../RanLogicClient/GroupChat/GroupChatClient.h"
#include "../../RanLogicClient/WishList/WishList.h"
#include "../../RanLogicClient/RebuildCard/RebuildCard.h"
#include "../../RanLogicClient/ItemPreview/GLItemPreviewClient.h"
#include "../../RanLogicClient/Club/GLClubClient.h"
#include "../../RanLogicClient/Tooltip/GLItemTooltip.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../GfxInterface.h"

namespace NSGFX_SLOT
{
	static const char* g_szSlotDir = "slot\\";
	static const char* g_szTempExt = ".png";

	static std::string AddCellNumber( const CString& strPath, const CString& strFile, WORD wMID, WORD wSID )
	{
		int nExtPos = strFile.ReverseFind( '.' );

		CString strFileName = strFile.Left( nExtPos );
		CString strPathFile = strPath + g_szSlotDir + strFileName + "\\";

		strPathFile.Format( "%s%s_%d_%d%s", strPathFile.GetBuffer(), strFileName.GetBuffer(), wSID, wMID, g_szTempExt );

		return sc::string::ansi_to_utf8( std::string( strPathFile.GetBuffer() ) );
	}

	void CreateSlot_Item( GFx::Movie* pMovie, GFx::Value* pValue, const SITEMCUSTOM& sItem, bool bNew, EMGFX_SLOT_TYPE emSlotType, const bool _bReferChar )
	{
		if( NULL == pMovie || NULL == pValue )
			return;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sItem.GetNativeID() );
		if( NULL == pItem )
			return;

		float fCoolDownTime = 0.0f;
		float fCoolDownRatio = 0.0f;

		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

		ReferChar::CClient* pReferChar = pClient->GetReferChar();
		
		GLCharacter* pChar = pClient->GetCharacter();
		if ( NULL == pChar )
			return;

		pChar->GetItemCoolDownTime( sItem.GetNativeID(), fCoolDownTime, fCoolDownRatio );

		EMCHARINDEX emClass = pChar->GETCHARINDEX();
		if ( _bReferChar && pReferChar )
			emClass = pReferChar->GetCharClass();
		std::string strIconFile = pItem->GetSubInventoryFile( emClass );
		if ( strIconFile.empty() )
			strIconFile = pItem->GetInventoryFile();

		std::string strIconPath = NSGFXFILE::g_strTexturePath;
		strIconPath.append( g_szSlotDir );
		strIconPath.append(	strIconFile );
		strIconPath = sc::string::ansi_to_utf8( strIconPath );

		std::string strWrapIconPath = "";
		WORD wWrapMID = 0;
		WORD wWrapSID = 0;

		std::string strLevelIconPath = "";

		// 포장 아이템
		if( true == sItem.IsBoxWrapped() )
		{
			const SITEM* pWrapItem = GLogicData::GetInstance().GetItem( sItem.GetRealNativeID() );
			if( NULL == pWrapItem )
				return;

			wWrapMID = pWrapItem->sBasicOp.sICONID.Mid();
			wWrapSID = pWrapItem->sBasicOp.sICONID.Sid();

			EMCHARINDEX emClass = pChar->GETCHARINDEX();
			std::string strIconFile = pWrapItem->GetSubInventoryFile( emClass );
			if ( strIconFile.empty() )
				strIconFile = pWrapItem->GetInventoryFile();

			strWrapIconPath = NSGFXFILE::g_strTexturePath;
			strWrapIconPath.append( g_szSlotDir );
			strWrapIconPath.append(	strIconFile );
			strWrapIconPath = sc::string::ansi_to_utf8( strWrapIconPath );
		}

		// OP 아이템?;
		std::string strOPIconPath = "";
		if ( GLUseFeatures::GetInstance().IsUsingOPItemCheck() && sItem.IsOpItem() )
		{
			strOPIconPath = NSGFXFILE::g_strTexturePath;
			strOPIconPath.append( g_szSlotDir );
			strOPIconPath.append( GLCONST_CHAR::strOPItemIcon );
			strOPIconPath = sc::string::ansi_to_utf8( strOPIconPath );
		}

		// 세트아이템;
		std::string strSetSlot_Indication("");
		if ( emSlotType == EMGFX_SLOT_CHARINFO )
		{
			DWORD dwMinTermCnt(0), dwEquipCnt(0);
			const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(pItem->BasicId());
			if ( sSetItem != NULL && pChar->GetSetitemEquipInfo(sSetItem->dwSetID, dwMinTermCnt, dwEquipCnt) == TRUE )
			{
				if ( dwMinTermCnt <= dwEquipCnt )
				{
					strSetSlot_Indication = NSGFXFILE::g_strTexturePath;
					strSetSlot_Indication.append(g_szSlotDir);
					strSetSlot_Indication.append(sSetItem->strSlotIndicationIcon);
					strSetSlot_Indication = sc::string::ansi_to_utf8( strSetSlot_Indication );
				}
			}
		}

		if ( pItem->IsSuitable() == true )
		{
			//// 아이테 레벨;
			if( sItem.GetItemLevel() != 0 && sItem.GetItemLevel() <= GLCONST_CHAR::EMITEMLEVEL_NUM)
			{
				std::string strIconFile  = GLCONST_CHAR::strItemLevelIcon[sItem.GetItemLevel()];

				strLevelIconPath = NSGFXFILE::g_strTexturePath;
				strLevelIconPath.append(g_szSlotDir);
				strLevelIconPath.append(strIconFile);
				strLevelIconPath.append(g_szTempExt);
				strLevelIconPath = sc::string::ansi_to_utf8( strLevelIconPath );
			}
		}

		bool bVisibleOneCount = false;
		if( 1 < pItem->sDrugOp.wPileNum )
			bVisibleOneCount = true;

		bool bShowGrade = false;	// 강화 수치 표기?
		WORD wCount = 0;			// 아이템 개수 / 강화 수치
		DWORD dwColor = 0;			// 텍스트 컬러
		if( false == pItem->ISPILE() && true == pItem->IsSuitable() )
		{
			bShowGrade = true;
			wCount = sItem.GETGRADE( EMGRINDING_NUNE );

			if( wCount == 0 )
				dwColor = NS_UITEXTUTIL::ENABLE;
			else if( wCount <= 4 )
				dwColor = D3DCOLOR_ARGB( 0xFF, 0xFF, 0xEA, 0x00 );
			else if( wCount <= 9 )
				dwColor = D3DCOLOR_ARGB( 0xFF, 0xFF, 0x00, 0x00 );
			else if( wCount <= 254 )
				dwColor = D3DCOLOR_ARGB( 0xFF, 0x00, 0xB4, 0xFF );
		}
		else
		{
			bShowGrade = false;
			wCount = sItem.TurnNum();

			dwColor = NSWIDGET_SCRIPT::g_dwDef_ItemSlotCountColor;
			if( pItem->sDrugOp.wPileNum <= sItem.wTurnNum )
				dwColor = NSWIDGET_SCRIPT::g_dwDef_ItemSlotMaxCountColor;
		}

		pMovie->CreateObject( pValue );
		pValue->SetMember( "id", (UInt32) sItem.GetNativeID().dwID );
		pValue->SetMember( "name", pItem->GetName() );
		pValue->SetMember( "iconPath", strIconPath.c_str() );
		pValue->SetMember( "count", (UInt32) wCount );
		pValue->SetMember( "coolDownTime", fCoolDownTime );
		pValue->SetMember( "coolDownRatio", fCoolDownRatio );
		pValue->SetMember( "colorMain", sItem.dwMainColor );
		pValue->SetMember( "colorSub", sItem.dwSubColor );
		pValue->SetMember( "wrapPath", strWrapIconPath.c_str() );
		pValue->SetMember( "opPath", strOPIconPath.c_str() );
		pValue->SetMember( "strSetSlot_IndicationPath", strSetSlot_Indication.c_str() );
		pValue->SetMember( "levelIconPath" , strLevelIconPath.c_str());
		pValue->SetMember( "dwCountColor", dwColor );
		pValue->SetMember( "bVisibleOneCount", bVisibleOneCount );
		pValue->SetMember( "bNew", bNew );
		pValue->SetMember( "bShowGrade", bShowGrade );
	}

	void CreateSlot_Skill( GFx::Movie* pMovie, GFx::Value* pValue, const SNATIVEID& sID )
	{
		if( NULL == pMovie || NULL == pValue )
			return;

		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( sID );
		if( NULL == pSkill )
			return;

		float fCoolDownTime = 0.0f;
		float fCoolDownRatio = 0.0f;

		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		GLCharacter* pChar = pClient->GetCharacter();
		if( NULL != pChar )
			pChar->GetSkillCoolDownTime( sID, fCoolDownTime, fCoolDownRatio );

		std::string strIconPath = NSGFXFILE::g_strTexturePath;
		strIconPath.append( g_szSlotDir );
		strIconPath.append( pSkill->m_sEXT_DATA.strICONFILE );
		strIconPath = sc::string::ansi_to_utf8( strIconPath );

		pMovie->CreateObject( pValue );
		pValue->SetMember( "id", (UInt32) sID.dwID );
		pValue->SetMember( "name", pSkill->GetName() );
		pValue->SetMember( "iconPath", strIconPath.c_str() );
		pValue->SetMember( "count", (UInt32) 1 );
		pValue->SetMember( "coolDownTime", fCoolDownTime );
		pValue->SetMember( "coolDownRatio", fCoolDownRatio );
		pValue->SetMember( "colorMain", (UInt32) 0 );
		pValue->SetMember( "colorSub", (UInt32) 0 );
		pValue->SetMember( "wrapPath", "" );
		pValue->SetMember( "opPath", "" );
		pValue->SetMember( "dwCountColor", (UInt32) 0xFFFFFFFF );
		pValue->SetMember( "bVisibleOneCount", false );
	}

	void CreateSlot_SummonState( GFx::Movie* pMovie, GFx::Value* pValue, std::string strIconPath, INT nIconIdxX, INT nIconIdxY)
	{
		if( NULL == pMovie || NULL == pValue )
			return;

		if( true == strIconPath.empty() )
			return;

		std::string strIconFullPath = NSGFXFILE::g_strTexturePath;
		strIconFullPath.append( g_szSlotDir );
		strIconFullPath.append( strIconPath );
		strIconFullPath = sc::string::ansi_to_utf8( strIconFullPath );

		pMovie->CreateObject( pValue );
		pValue->SetMember( "iconPath", strIconFullPath.c_str() );
		pValue->SetMember( "MID", (UInt32) nIconIdxX );
		pValue->SetMember( "SID", (UInt32) nIconIdxY );
	}
}

//-----------------------------------------------------------------------------------------
// 슬롯 베이스
void GfxSlotBase::OnAddHandler( const char* binding, const CLIKDataBindingHandlerDesc& desc )
{
	m_strBindingID = binding;
}

bool GfxSlotBase::GetAddSlotCount( int& nCount )
{
	nCount = GLWidgetScript::GetInstance().LuaCallFunc< int >( NSWIDGET_SCRIPT::g_strFunc_GetTootipAddSlotCount );

	return true;
}

bool GfxSlotBase::GetAddSlotBinding( std::string& strBinding )
{
	strBinding = GLWidgetScript::GetInstance().LuaCallFunc< char* >( NSWIDGET_SCRIPT::g_strFunc_GetTooltipAddSlotBinding );
	strBinding = sc::string::ansi_to_utf8( strBinding );

	return true;
}

bool GfxSlotBase::GetTradeSlotCount( int& nCount )
{
	nCount = GLWidgetScript::GetInstance().LuaCallFunc< int >( NSWIDGET_SCRIPT::g_strFunc_GetTooltipTradeSlotCount );

	return true;
}

bool GfxSlotBase::GetTradeSlotBinding( std::string& strBinding )
{
	strBinding = GLWidgetScript::GetInstance().LuaCallFunc< char* >( NSWIDGET_SCRIPT::g_strFunc_GetTooltipTradeSlotBinding );
	strBinding = sc::string::ansi_to_utf8( strBinding );

	return true;
}

//-----------------------------------------------------------------------------------------
// 툴팁 슬롯
GfxTooltipSlot::GfxTooltipSlot()
:	m_pSlot( NULL )
,	m_emParentTooltipType(EMGFX_TOOLTIPTYPE_BASIC)
{
}

GfxTooltipSlot::~GfxTooltipSlot()
{
}

void GfxTooltipSlot::GetValue( GFx::Movie* pMovie, GFx::Value* pValue )
{
	// 슬롯에서 출력된 툴팁이 아니라면 아이디를 참조한다;
	if( NULL == m_pSlot )
	{
		switch ( m_emParentTooltipType )
		{
		case EMGFX_TOOLTIPTYPE_SUB_SKILL:
			{
				NSGFX_SLOT::CreateSlot_Skill( pMovie, pValue, m_sNativeID );
			}
			break;
		case EMGFX_TOOLTIPTYPE_SUB_ITEM:
			{
				NSGFX_SLOT::CreateSlot_Item( pMovie, pValue, m_sNativeID, false, GetType(), GLItemTooltip::GetInstance().IsReferTooltip() );
			}
			break;
		}

		return;
	}

	m_pSlot->GetValue( pMovie, pValue );
	m_pSlot = NULL;
}

//-----------------------------------------------------------------------------------------
// 아이템 베이스 슬롯
GfxItemBaseSlot::GfxItemBaseSlot()
: m_bNew( false )
, m_bUseCompare( false )
{
}

GfxItemBaseSlot::~GfxItemBaseSlot()
{
}

void GfxItemBaseSlot::GetValue( GFx::Movie* pMovie, GFx::Value* pValue )
{
	NSGFX_SLOT::CreateSlot_Item( pMovie, pValue, m_sItemCustom, m_bNew, GetType() );
}

void GfxItemBaseSlot::Reset()
{
	m_sItemCustom = SITEMCUSTOM();
	m_bNew = false;
}

bool GfxItemBaseSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLItemTooltip::GetInstance().SetOtherCharData( NULL );

	std::string strHeader = NSWIDGET_SCRIPT::GetItemInfoHeader( m_sItemCustom );
	std::wstring strHeaderW = GfxInterface::GetInstance()->EncodeText( strHeader );

	std::string strToolTip = NSWIDGET_SCRIPT::GetItemInfo( m_sItemCustom, GetType() );
	std::wstring strToolTipW = GfxInterface::GetInstance()->EncodeText( strToolTip );

	std::string strTrade = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		NSWIDGET_SCRIPT::g_strFunc_GetItemTooltipTrade );
	std::wstring strTradeW = GfxInterface::GetInstance()->EncodeText( strTrade );

	pTooltip->SetMember( "type", (UInt32) GetType() );
	pTooltip->SetMember( "header", strHeaderW.c_str() );
	pTooltip->SetMember( "tooltip", strToolTipW.c_str() );
	pTooltip->SetMember( "trade", strTradeW.c_str() );
	pTooltip->SetMember( "itemid", (UInt32) m_sItemCustom.GetNativeID().Id() );

	// 툴팁 봤으면 신규 아이템 확인을 했다.
	m_bNew = false;
	
	return true;
}

bool GfxItemBaseSlot::GetSubTooltip(GFx::Value* pTooltipInfo)
{
	pTooltipInfo->SetMember("type", (UInt32)GetType());

	GFx::Value gfxSubInfo;
	pTooltipInfo->GetMovie()->CreateObject(&gfxSubInfo);
	GetItemTooltipSubInfo(&gfxSubInfo);
	pTooltipInfo->SetMember("sub_info", gfxSubInfo);

	return true;
}

void GfxItemBaseSlot::GetItemTooltipSubInfo(GFx::Value* pSubInfo)
{
	const SITEM* pItem = GLItemMan::GetInstance().GetItem(m_sItemCustom.GetNativeID().Id());
	//if ( pItem ) 이럴일없다;
	//const SNATIVEID& sLinkSkillNID = pItem->sItemSkill.sSkillID;
	const SNATIVEID& sLinkSkillNID = m_sItemCustom.GetLinkSkillID();
	const WORD wLinkSkillLevel = m_sItemCustom.GetLinkSkillLevel();
	// 링크 스킬 정보 세팅;
	if ( sLinkSkillNID != NATIVEID_NULL() )
	{
		std::wstring strType = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetSkillTypeText_MultiSkill() );
		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetSkillInfoHeader(
			sLinkSkillNID, 
			(WORD) EMGFX_SLOT_SKILLBASE,
			Disable_View_Level // 
			/*wLinkSkillLevel*/ /*pItem->sItemSkill.wSkillLv - 1*/ ) );
		// 아이템을 통한 스킬링크는 레벨을 표기하지 않기로함 
		// -1로 하려했으나 이미 처리 구문이있음
		// 시간이 많다면 이 상황을 위한 슬롯을 만들자.
		// http://192.168.100.87/redmine/issues/6003

		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfo(
			sLinkSkillNID,
			(WORD) EMGFX_SLOT_SKILLBASE,
			wLinkSkillLevel/*pItem->sItemSkill.wSkillLv - 1*/,
			false ) );

		GFx::Value gfxLinkValue;
		pSubInfo->GetMovie()->CreateObject(&gfxLinkValue);
		gfxLinkValue.SetMember( "typetext", strType.c_str() );
		gfxLinkValue.SetMember( "header", strHeader.c_str() );
		gfxLinkValue.SetMember( "tooltip", strTooltip.c_str() );
		gfxLinkValue.SetMember( "skillid", (UInt32) sLinkSkillNID.dwID );
		pSubInfo->SetMember( "multi_info", gfxLinkValue );
	}

	// 부스터 정보 세팅;
	if ( ItemDurability::GetInstance().IsUseWeapon(pItem) )
	{
		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetDurabilityInfoHeader( m_sItemCustom, GetType() ) );
		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetDurabilityInfo( m_sItemCustom, GetType() ) );

		GFx::Value gfxDurabilityValue;
		pSubInfo->GetMovie()->CreateObject(&gfxDurabilityValue);
		gfxDurabilityValue.SetMember("header", strHeader.c_str());
		gfxDurabilityValue.SetMember("tooltip", strTooltip.c_str());
		gfxDurabilityValue.SetMember("itemid", (UInt32)(m_sItemCustom.GetNativeID().Id()));
		pSubInfo->SetMember("durability_info", gfxDurabilityValue);
	}

	// 비교툴팁
	if( true == m_bUseCompare )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
		if( NULL != pItem && true == pItem->IsSuitable() )
		{
			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

			for( WORD i = SLOT_HEADGEAR; i < SLOT_TSIZE; ++i )
			{
				const SITEM* pPutOnItem = GLogicData::GetInstance().GetItem( pChar->m_PutOnItems[ i ].GetNativeID() );
				if( NULL == pPutOnItem )
					continue;

				if( pPutOnItem->sSuitOp.emSuit != pItem->sSuitOp.emSuit )
					continue;

				if( SUIT_HANDHELD == pItem->sSuitOp.emSuit &&
					pPutOnItem->sBasicOp.emItemType != pItem->sBasicOp.emItemType )
					continue;

				std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
					NSWIDGET_SCRIPT::GetItemInfoHeader( pChar->m_PutOnItems[ i ] ) );

				std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
					NSWIDGET_SCRIPT::GetItemInfo( pChar->m_PutOnItems[ i ] ) );

				GFx::Value gfxCompare;
				pSubInfo->GetMovie()->CreateObject( &gfxCompare );

				gfxCompare.SetMember( "header", strHeader.c_str() );
				gfxCompare.SetMember( "tooltip", strTooltip.c_str() );
				gfxCompare.SetMember( "itemid", (UInt32) pChar->m_PutOnItems[ i ].GetNativeID().Id() );
				pSubInfo->SetMember( "compare_info", gfxCompare );

				break;
			}
		}
	}
}

void GfxItemBaseSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	if ( GLUseFeatures::GetInstance().IsUsingOPItemCheck() && m_sItemCustom.IsOpItem() )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		pClient->PrintMessageBox( NSWIDGET_SCRIPT::SFMSG_BOX_TYPE_OK, ID2GAMEINTEXT("NOTIFY_OPITEM"));
		return;
	}

	// 아이템 미리보기
	if( false == bCtrl && true == bAlt && false == bShift )
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
		if( NULL == pItem )
			return;

		ItemPreview::GLItemPreviewClient* pItemPriview = 
			GfxInterface::GetInstance()->GetGaeaClient()->GetItemPreviewClient();
		if( NULL == pItemPriview )
			return;

		bool bIsPreview = false;

		// 미리보기 가능한 아이템
		if( true == pItem->IsPreview() )
		{
			bIsPreview = pItemPriview->IsEnableItemPreview( pItem );
		}
		else
		{
			// 박스 아이템이면 박스 안에 미리보기 가능한 아이템 있는지 확인
			if( ITEM_BOX == pItem->BasicType() )
			{
				for( size_t i = 0; i < pItem->sBox.vecItems.size(); ++i )
				{
					SITEM* pBoxItem = GLogicData::GetInstance().GetItem( pItem->sBox.vecItems[ i ].nidITEM );
					if( NULL == pBoxItem )
						continue;

					if( false == pBoxItem->IsPreview() )
						continue;

					bIsPreview = pItemPriview->IsEnableItemPreview( pBoxItem );
					break;
				}
			}
		}

		if( true == bIsPreview )
		{
			pItemPriview->OpenItemPreview( m_sItemCustom );
			pItemPriview->SetItem( m_sItemCustom );
		}
	}
}

void GfxItemBaseSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	if( false == bCtrl && false == bShift && true == bAlt )
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem ( m_sItemCustom.GetNativeID() );
		if( NULL == pItem )
			return;

		// 아이템 박스 미리보기
		if ( ITEM_RANDOMITEM == pItem->BasicType() ||
			ITEM_BOX == pItem->BasicType() ||
			ITEM_SELECTIVEFORM_BOX == pItem->BasicType() ||
			ITEM_LOCK_BOX == pItem->BasicType() )
		{
			if ( false == pItem->sBox.ShowContents )
				return;

			std::string strHeader = NSWIDGET_SCRIPT::GetItemInfoHeader( m_sItemCustom );
			std::string strInfo = NSWIDGET_SCRIPT::GetItemInfo( m_sItemCustom, (int) GetType() );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Inventory_WishItemShowTooltip,
				"-n, -n, -s, -s",
				m_sItemCustom.GetNativeID().Mid(),
				m_sItemCustom.GetNativeID().Sid(),
				strHeader.c_str(),
				strInfo.c_str() );
		}
		// 세트 아이템 미리보기
		else if( true == pItem->IsSuitable() )
		{
			GfxInterface::GetInstance()->GetGaeaClient()->SetItemGroupComposition( m_sItemCustom.GetNativeID() );
		}
	}
}

//-----------------------------------------------------------------------------------------
// 스킬 베이스 슬롯
GfxSkillBaseSlot::GfxSkillBaseSlot()
:	m_sNativeID( NATIVEID_NULL() )
,	m_nLevel( -1 )
{
}

GfxSkillBaseSlot::~GfxSkillBaseSlot()
{
}

void GfxSkillBaseSlot::GetValue( GFx::Movie* pMovie, GFx::Value* pValue )
{
	NSGFX_SLOT::CreateSlot_Skill( pMovie, pValue, m_sNativeID );
}

void GfxSkillBaseSlot::Reset()
{
	m_sNativeID = NATIVEID_NULL();
}

bool GfxSkillBaseSlot::GetTooltip( GFx::Value* pTooltip )
{
	std::string strHeader = NSWIDGET_SCRIPT::GetSkillInfoHeader( m_sNativeID, (WORD) GetType(), m_nLevel );
	std::wstring strHeaderW = GfxInterface::GetInstance()->EncodeText( strHeader );

	/*********************************************************************************************************/
	GLCharacter* pMainChar  = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	PGLSKILL	 pSkill     = GLSkillMan::GetInstance().GetData ( m_sNativeID );

	BOOL	  bChange      = FALSE;
	int		  dwTableID    = -1;
	int		  iDisuiseType = DISGUISE_NONE;

	if( pMainChar ) 
	{
		bChange   = RF_DISGUISE( pMainChar->m_EffSkillVarSet ).IsOn();
		dwTableID = RF_DISGUISE( pMainChar->m_EffSkillVarSet ).GetTableID() - 1;

		bChange = (dwTableID < 0) ? FALSE : bChange;

		if( bChange ) bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( bChange ) {

		PGLSKILL pOrgSkill = GSCL_INST.GetOrgChangeSkill( dwTableID, pSkill->m_sBASIC.sNATIVEID.dwID );

		if( pOrgSkill || pSkill->m_sEXT_DATA.idDisguiseSkill.IsValidNativeID() ) {
			iDisuiseType = DISGUISE_BEFORE_SKILL;
		}
	}
	else {
		iDisuiseType = pSkill->m_sEXT_DATA.idDisguiseSkill.IsValidNativeID() ? DISGUISE_BEFORE_SKILL : DISGUISE_NONE;
	}
	/*********************************************************************************************************/
	
	std::string strToolTip = NSWIDGET_SCRIPT::GetSkillInfo( m_sNativeID, (WORD) GetType(), m_nLevel, true, iDisuiseType );
	std::wstring strToolTipW = GfxInterface::GetInstance()->EncodeText( strToolTip );

	pTooltip->SetMember( "type", (UInt32) GetType() );
	pTooltip->SetMember( "header", strHeaderW.c_str() );
	pTooltip->SetMember( "tooltip", strToolTipW.c_str() );
	pTooltip->SetMember( "skillid", (UInt32)m_sNativeID.Id() );

	return true;
}

bool GfxSkillBaseSlot::GetSubTooltip( GFx::Value* pSubTooltip )
{
	pSubTooltip->SetMember( "type", (UInt32) GetType() );
	pSubTooltip->SetMember( "skillid", (UInt32) m_sNativeID.dwID );

	GFx::Value gfxSlotValue;
	NSGFX_SLOT::CreateSlot_Skill( pSubTooltip->GetMovie(), &gfxSlotValue, m_sNativeID );
	pSubTooltip->SetMember( "slotinfo", gfxSlotValue );

	GFx::Value gfxSubInfo;
	pSubTooltip->GetMovie()->CreateObject( &gfxSubInfo );
	GetSkillTooltipSubInfo( m_sNativeID, m_nLevel, &gfxSubInfo );
	pSubTooltip->SetMember( "sub_info", gfxSubInfo );

	return true;
}

// private
void GfxSkillBaseSlot::GetSkillTooltipSubInfo( const SNATIVEID& sID, WORD wLevel, GFx::Value* pSubInfo )
{
	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( m_sNativeID );

	if ( pSkill == NULL ) return;

	BOOL bDistinguishSkill = FALSE;//pSkill->m_sEXT_DATA.idDisguiseSkill != NATIVEID_NULL() ? TRUE : FALSE;
	BOOL bMultiSkill = pSkill->m_sEXT_DATA.idMultiTargetSkill != NATIVEID_NULL();
	BOOL bLinkSkill = FALSE;
	BOOL bDelayActionSkill = FALSE;
	BOOL bActiveSkill = FALSE;
	SNATIVEID sDelayActionSkillID;
	SNATIVEID sActiveSkillID;

	GLCharacter* pMainChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();

	int nLevel = m_nLevel;

	if( pMainChar && nLevel == -1 )
	{
		SCHARSKILL* pCharSkill = pMainChar->GETLEARNED_SKILL( m_sNativeID );

		if( pCharSkill ) {
			nLevel = (int)pCharSkill->wLevel;
		}
	}
	
	SKILL::SAPPLY sApply = pSkill->m_sAPPLY;
	for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
	{
		if( sApply.vecSPEC[i].emSPEC == EMSPECA_SKILL_LINK )
		{
			bLinkSkill = TRUE;
		}

		if( sApply.vecSPEC[i].emSPEC == EMSPECA_DELAY_ACTION_SKILL )
		{
			bDelayActionSkill = TRUE;
			WORD wMid;
			WORD wSid;
			wMid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR1);
			wSid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR2);
			sDelayActionSkillID.SetMid(wMid);
			sDelayActionSkillID.SetSid(wSid);
		}

		if( sApply.vecSPEC[i].emSPEC == EMSPECA_ACTIVATE_DAMAGE )
		{

			sActiveSkillID = sApply.vecSPEC[i].sSPEC[ nLevel ].dwNativeID;

			if( sActiveSkillID.IsValidNativeID() ) bActiveSkill = TRUE;
		}
	}

	if ( bMultiSkill )
	{
		SNATIVEID sMultiSkillNID = pSkill->m_sEXT_DATA.idMultiTargetSkill;

		std::wstring strType = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetSkillTypeText_MultiSkill() );
		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfoHeader( sMultiSkillNID, (WORD) EMGFX_SLOT_SKILLBASE, m_nLevel ) );
		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfo( sMultiSkillNID, (WORD) EMGFX_SLOT_SKILLBASE, m_nLevel, false ) );

		//pTooltipInfo->SetMember("type", (UINT)GetType());
		GFx::Value multValue;
		pSubInfo->GetMovie()->CreateObject(&multValue);
		multValue.SetMember("typetext", strType.c_str());
		multValue.SetMember("header", strHeader.c_str());
		multValue.SetMember("tooltip", strTooltip.c_str());
		multValue.SetMember("skillid", (UInt32)sMultiSkillNID.dwID);
		pSubInfo->SetMember("multi_info", multValue);
	}

	if ( bLinkSkill )
	{
		INT nCount(0);
		GFx::Value gfxLinkSkillValue;
		pSubInfo->GetMovie()->CreateArray(&gfxLinkSkillValue);
		std::vector<SNATIVEID> vecLinkSkill;
		for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
		{
			if( sApply.vecSPEC[i].emSPEC != EMSPECA_SKILL_LINK )
				continue;

			bool bFind(false);
			for ( DWORD j = 0; j < (int)vecLinkSkill.size(); ++j )
			{
				if ( vecLinkSkill[j] == sApply.vecSPEC[i].sSPEC[m_nLevel].dwNativeID )
				{
					bFind = true;
					break;
				}
			}
			if( bFind == true )
				continue;

			SNATIVEID sLinkSkillNid(SNATIVEID(sApply.vecSPEC[i].sSPEC[wLevel].dwNativeID));
			vecLinkSkill.push_back(sLinkSkillNid);
			PGLSKILL pLinkSkill = GLSkillMan::GetInstance().GetData( sLinkSkillNid );
			if( !pLinkSkill )
				continue;

			std::wstring strType = GfxInterface::GetInstance()->EncodeText( 
				NSWIDGET_SCRIPT::GetSkillTypeText_LinkSkill() );

			std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
				NSWIDGET_SCRIPT::GetSkillInfoHeader( sLinkSkillNid, (WORD) EMGFX_SLOT_SKILLBASE, wLevel ) );

			std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
				NSWIDGET_SCRIPT::GetSkillInfo( sLinkSkillNid, (WORD) EMGFX_SLOT_SKILLBASE, wLevel, false ) );

			GFx::Value gfxLinkUnit;
			pSubInfo->GetMovie()->CreateObject(&gfxLinkUnit);
			gfxLinkUnit.SetMember("typetext", strType.c_str());
			gfxLinkUnit.SetMember("header", strHeader.c_str());
			gfxLinkUnit.SetMember("tooltip", strTooltip.c_str());
			gfxLinkUnit.SetMember("skillid", (UInt32)sLinkSkillNid.dwID);
			gfxLinkSkillValue.SetElement(nCount++, gfxLinkUnit);
		}
		pSubInfo->SetMember("link_info", gfxLinkSkillValue);
	}

	if ( bDelayActionSkill )
	{
		std::wstring strType = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetSkillTypeText_DelaySkill() );

		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfoHeader( sDelayActionSkillID, (WORD) EMGFX_SLOT_SKILLBASE, wLevel ) );

		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfo( sDelayActionSkillID, (WORD) EMGFX_SLOT_SKILLBASE, wLevel, false ) );

		GFx::Value gfxDelaySkillValue;
		pSubInfo->GetMovie()->CreateObject(&gfxDelaySkillValue);
		gfxDelaySkillValue.SetMember("typetext", strType.c_str());
		gfxDelaySkillValue.SetMember("header", strHeader.c_str());
		gfxDelaySkillValue.SetMember("tooltip", strTooltip.c_str());
		gfxDelaySkillValue.SetMember("skillid", (UInt32)sDelayActionSkillID.dwID);
		pSubInfo->SetMember("delay_info", gfxDelaySkillValue);
	}
	
	BOOL	  bChange   = FALSE;
	int		  dwTableID = -1;
	SNATIVEID sDistinSkillNID;

	if( pMainChar ) 
	{
		bChange   = RF_DISGUISE( pMainChar->m_EffSkillVarSet ).IsOn();
		dwTableID = RF_DISGUISE( pMainChar->m_EffSkillVarSet ).GetTableID() - 1;

		bChange = (dwTableID < 0) ? FALSE : bChange;

		if( bChange ) bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( bChange ) {

		PGLSKILL pOrgSkill = GSCL_INST.GetOrgChangeSkill( dwTableID, pSkill->m_sBASIC.sNATIVEID.dwID );

		if( pOrgSkill ) {
			bDistinguishSkill = TRUE;
			sDistinSkillNID   = pOrgSkill->m_sBASIC.sNATIVEID;
		}
		else
		if( pSkill->m_sEXT_DATA.idDisguiseSkill.IsValidNativeID() ){
			bDistinguishSkill = TRUE;
			sDistinSkillNID   = pSkill->m_sEXT_DATA.idDisguiseSkill;
		}
	}
	else {
		bDistinguishSkill = pSkill->m_sEXT_DATA.idDisguiseSkill.IsValidNativeID() ? TRUE : FALSE;

		sDistinSkillNID = pSkill->m_sEXT_DATA.idDisguiseSkill;
	}
	
	if( bDistinguishSkill && sDistinSkillNID.IsValidNativeID() )
	{
		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfoHeader( sDistinSkillNID, (WORD) EMGFX_SLOT_SKILLBASE, nLevel ) );

		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfo( sDistinSkillNID, (WORD) EMGFX_SLOT_SKILLBASE, nLevel, true, DISGUISE_AFTER_SKILL ) );

		GFx::Value DistingValue;
		pSubInfo->GetMovie()->CreateObject(&DistingValue);
		//DistingValue.SetMember("typetext", "");
		DistingValue.SetMember("header", strHeader.c_str());
		DistingValue.SetMember("tooltip", strTooltip.c_str());
		DistingValue.SetMember("skillid", (UInt32)sDistinSkillNID.dwID);
		pSubInfo->SetMember("disguise_info", DistingValue);
	} 

	if ( bActiveSkill )
	{		
		PGLSKILL pActiveSkill = GLSkillMan::GetInstance().GetData( sActiveSkillID );

		std::wstring strType = GfxInterface::GetInstance()->EncodeText( 
			NSWIDGET_SCRIPT::GetSkillTypeText_ActiveSkill() );
		std::wstring strHeader = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfoHeader( sActiveSkillID, (WORD)EMGFX_SLOT_SKILLBASE, nLevel ) );
		std::wstring strTooltip = GfxInterface::GetInstance()->EncodeText(
			NSWIDGET_SCRIPT::GetSkillInfo( sActiveSkillID, (WORD)EMGFX_SLOT_SKILLBASE, nLevel, false ) );

		GFx::Value gfxActive;
		pSubInfo->GetMovie()->CreateObject(&gfxActive);
		gfxActive.SetMember("typetext", strType.c_str());
		gfxActive.SetMember("header", strHeader.c_str());
		gfxActive.SetMember("tooltip", strTooltip.c_str());
		gfxActive.SetMember("skillid", (UInt32)sActiveSkillID.dwID);
		pSubInfo->SetMember("active_info", gfxActive);
	}
}

//-----------------------------------------------------------------------------------------
// 인벤토리 슬롯
GfxInventorySlot::GfxInventorySlot()
:	m_nPosX( -1 )
,	m_nPosY( -1 )
{
}

GfxInventorySlot::~GfxInventorySlot()
{
}

bool GfxInventorySlot::GetTooltip( GFx::Value* pTooltip )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return __super::GetTooltip( pTooltip );

	SINVENITEM* pItem = pChar->m_cInventory.GetItem( m_nPosX, m_nPosY );
	if( NULL != pItem )
		pItem->bNew = false;

	return __super::GetTooltip( pTooltip );
}

void GfxInventorySlot::HoldStart( bool bLock )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqInvenTo( m_nPosX, m_nPosY );

	// 탈것 부품 슬롯 하이라이트
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_Vehicle_IsOpen ) )
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
		if( NULL == pItem )
			return;

		int nSuit = (int) pItem->sSuitOp.emSuit;
		if( SUIT_VEHICLE_PARTS_A <= nSuit && 
			SUIT_VEHICLE_PARTS_A + SUIT_VEHICLE_PARTS_SIZE > nSuit )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Vehicle_SlotHighlight,
				"-n, -n", (int) pItem->VehicleType(), nSuit - (int) SUIT_VEHICLE_PARTS_A );
		}
	}
}

bool GfxInventorySlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return false;

	// 개인 상점 개설했나?
	if( true == IsPrivateMarketOpener() )
		return false;

	// 휴지통 열렸나?
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_InvenTrash_IsOpen ) )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = (GfxInventorySlot*) pHoldSlot;
			if( NULL == pInvenSlot )
				return true;

			if( S_OK == pChar->ReqInvenTo( m_nPosX, m_nPosY ) )
			{
				int nPosX2, nPosY2;
				pInvenSlot->GetPosition( nPosX2, nPosY2 );

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_SwapInvenItemPos,
					"-n, -n, -n, -n",
					m_nPosX, m_nPosY, nPosX2, nPosY2 );

				SINVEN_POS sPos1;
				sPos1.wPosX = (WORD) m_nPosX;
				sPos1.wPosY = (WORD) m_nPosY;

				SINVEN_POS sPos2;
				sPos2.wPosX = (WORD) nPosX2;
				sPos2.wPosY = (WORD) nPosY2;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_BasicChat_SwapItemLinkPos,
					"-dw, -dw", sPos1.dwPos, sPos2.dwPos );
			}
			else
			{
				int nPosX = 0;
				int nPosY = 0;
				pInvenSlot->GetPosition( nPosX, nPosY );

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_SwapInvenItemPos,
					"-n, -n, -n, -n",
					nPosX, nPosY, nPosX, nPosY );
			}

			m_bNew = false;
		}
		break;

	case EMGFX_SLOT_CHARINFO :
	case EMGFX_SLOT_TRADE :
	case EMGFX_SLOT_CLUBSTORAGE :
	case EMGFX_SLOT_LOCKER :
		{
			pChar->ReqInvenTo( m_nPosX, m_nPosY );
			m_bNew = false;
		}
		break;	
	}

	return true;
}

void GfxInventorySlot::DragSlotProc( GfxSlotBase* pTargetSlot )
{
	// 아이템 분리 가능한지 체크
	if( 1 >= m_sItemCustom.wTurnNum )
		return;

	switch( pTargetSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			// 겹침 아이템 분리
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_SeparateOverlapItem,
				"-n, -n, -n", m_nPosX, m_nPosY, m_sItemCustom.wTurnNum );
		}
		break;
	}
}

void GfxInventorySlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		WishList::CClient* pWishList = GfxInterface::GetInstance()->GetGaeaClient()->GetWishList();
		if( NULL != pWishList )
			pWishList->NewInventoryItem( m_sItemCustom );
	}
	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLCharacter* pChar = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();

		// 개인 상점 아이템 등록
		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpener ) )
		{
			GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

			const SINVENITEM* pInvenItem = pChar->m_cInventory.FindItem( ITEM_PRIVATEMARKET );
			if( NULL == pInvenItem )
				return;

			if( 1 == pInvenItem->CustomItem().wTurnNum && 
				pInvenItem->wPosX == m_nPosX && pInvenItem->wPosY == m_nPosY )
			{
				pClient->PrintMsgTextDlg( 0xFFFF0000, ID2GAMEINTEXT( "IMPOSSIBLE_SALE_ITEM" ) );
				return;
			}

			PrivateMarketClient* pPrivateMarket = pClient->GetPrivateMarketClient();
			pPrivateMarket->ReqPrivateMarketRegItemMsgBoxOpen( m_nPosX, m_nPosY );

			return;
		}

		// 아이템 분해
		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_Inventory_IsItemDecompose ) )
		{
			if( true == pChar->IsInvenDecomposeItem( m_nPosX, m_nPosY ) )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Inventory_ConfirmItemDecompose,
					"-n, -n", m_nPosX, m_nPosY );
			}

			return;
		}
	}
}

void GfxInventorySlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{

	// 개인상점 개설중이면 인벤토리에서 마우스 우클릭을 막자 
	//if( GLWidgetScript::GetInstance().LuaCallFunc<bool>(NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpener ) == true )
	//	return;
	
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return;

	const SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( (WORD) m_nPosX, (WORD) m_nPosY );
	if( NULL == pInvenItem )
		return;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItemData )
		return;
	else
	{
		// 상점 개설자는 개인상점 검색카드를 제외하고 사용하지 못하도록 수정 .
		if( GLWidgetScript::GetInstance().LuaCallFunc<bool>(NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpener ) == true  &&
			pItemData->sBasicOp.emItemType != ITEM_PMARKET_SEARCH_CARD )
			 return ;
	}


	ESlotHold eHold = ESlotHold::Release;

	// 아이템 링크
	if( true == bCtrl && false == bAlt && false == bShift )
	{
		const SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( (WORD) m_nPosX, (WORD) m_nPosY );
		if( NULL == pInvenItem )
			return;

		bool bBasicChat_IsFocusIn = GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_BasicChat_IsFocusIn );

		// 그룹 채팅으로 링크 삽입되면 일반 채팅으로 링크할 필요 없음
		if( bBasicChat_IsFocusIn == false )
		{
			// 활성된 그룹 채팅 키
			DWORD dwGroupChatKey = GLWidgetScript::GetInstance().LuaCallFunc< DWORD >(
				NSWIDGET_SCRIPT::g_strFunc_GroupChat_GetEnableRoomKey );

			if( true == pClient->GetChatGroup()->AddLinkItem( dwGroupChatKey, pInvenItem->CustomItem() ) )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_GroupChat_UpdateLinkItem,
					"-dw", dwGroupChatKey );

				return;
			}
		}

		// 일반 채팅 아이템 링크
		SINVEN_POS sInvenPos;
		sInvenPos.wPosX = (WORD) m_nPosX;
		sInvenPos.wPosY = (WORD) m_nPosY;

		SLINK_DATA_BASIC sLinkDataBasic;
		sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
		sLinkDataBasic.sLinkType.wSType = ITEM_LINK_INVEN;
		sLinkDataBasic.dwData1 = sInvenPos.dwPos;

		GLChatMan::GetInstance().AddChatLink( sLinkDataBasic );
	}
	else if( false == bCtrl && false == bAlt && false == bShift )
	{
		bool bOpenMarketTab = GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_NpcTalk_IsMarketTabOpen );

		// NPC 대화창 마켓탭 활성되어있고 우클릭 판매 옵션이 켜져있으면 아이템 판매
		if( true == bOpenMarketTab && true == RANPARAM::bCheckSellItemRButton )
		{
			// 판매 가능 여부
			bool bSale = true == pInvenItem->sItemCustom.IsGM_GENITEM() ? false : true;

			// 팻카드일 경우
			switch( pItemData->sBasicOp.emItemType )
			{
			case ITEM_PET_CARD :
				{
					const SITEMCUSTOM& sHoldItem = pInvenItem->sItemCustom;
					GLPetClient* pMyPet = pClient->GetPetClient();
					if( true == pMyPet->IsVALID() && sHoldItem.PetDbNum() == pMyPet->DbNum() )
						bSale = false;
				}
				break;
			}

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_SellItem,
				"-b, -b, -s, -n, -n",
				bSale, RANPARAM::bCheckConfirmSellItem,
				pItemData->GetName(),
				m_nPosX, m_nPosY );

			return;
		}

		switch( pItemData->sBasicOp.emItemType )
		{
		case ITEM_SUIT :
			{
				if( true == pInvenItem->CustomItem().IsBoxWrapped() )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_UnwrapItem,
						"-s, -n, -n",
						m_sItemCustom.GETNAME().c_str(),
						m_nPosX, m_nPosY );

					return;
				}

				// 코스튬 씌울 때 아이템 기간이 코스튬 기간보다 짧다면 확인 메시지 띄운다.
				if( true == holditem::GetHoldItem().IsDisguise() )
				{
					SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
					SITEM* pCosutmeItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().GetNativeID() );

					if( NULL != pItem && NULL != pCosutmeItem && 0 != pItem->sDrugOp.tTIME_LMT )
					{
						bool bShowMsg = true;
						if(	0 != pCosutmeItem->sDrugOp.tTIME_LMT )
						{
							CTime cTime( m_sItemCustom.tBORNTIME );
							GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cTime );

							CTime cCostumeTime( holditem::GetHoldItem().tBORNTIME );
							GfxInterface::GetInstance()->GetGaeaClient()->GetConvertServerTime( cCostumeTime );

							CTimeSpan sLMT( pItem->sDrugOp.tTIME_LMT );
							cTime += sLMT + m_sItemCustom.GetPeriodExBasic();

							sLMT = CTimeSpan( pCosutmeItem->sDrugOp.tTIME_LMT );
							cCostumeTime += sLMT + holditem::GetHoldItem().GetPeriodExBasic();

							if( cTime > cCostumeTime )
								bShowMsg = false;
						}
						
						if( true == bShowMsg )
						{
							GLWidgetScript::GetInstance().LuaCallFunc< void >(
								NSWIDGET_SCRIPT::g_strFunc_CostumeDisguiseTimeCheck,
								"-n, -n, -n, -n",
								m_nPosX, m_nPosY, holditem::GetHoldPosX(), holditem::GetHoldPosY() );

							holditem::ReleaseHoldItem();
							return;
						}
					}
				}
			}
			break;
		}

		// 아이템 사용
		eHold = pChar->UseInvenItem( m_nPosX, m_nPosY );
	}

	if( ESlotHold::Release == eHold )
		holditem::ReleaseHoldItem();
}

bool GfxInventorySlot::IsPrivateMarketOpener()
{
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

		PrivateMarketClient* pPrivateMarket = pClient->GetPrivateMarketClient();
		if( true == pPrivateMarket->IsOpener() )
			return true;
	}

	return false;
}

void GfxInventorySlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxInventorySlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// 캐릭터 정보 슬롯
GfxCharInfoSlot::GfxCharInfoSlot()
:	m_nSlot( 0 )
{
}

GfxCharInfoSlot::~GfxCharInfoSlot()
{
}

void GfxCharInfoSlot::HoldStart( bool bLock )
{
	RegSlotTo();
}

bool GfxCharInfoSlot::PostDropSlotProc( GfxSlotBase* pSlot, bool bHoldLock, bool bTargetLock )
{
	RegSlotTo();

	return true;
}

void GfxCharInfoSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	if( false == bCtrl && false == bAlt && false == bShift )
		pChar->ReqSlotConsume( EMSLOT( m_nSlot ) );
}

void GfxCharInfoSlot::RegSlotTo()
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSlotTo( EMSLOT( m_nSlot ) );
}

bool GfxCharInfoSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return false;

	const SITEMCUSTOM& sItemCustom = pChar->GET_SLOT_ITEM( EMSLOT( m_nSlot ) );
	m_sItemCustom = sItemCustom;

	return __super::GetTooltip( pTooltip );
}

//-----------------------------------------------------------------------------------------
// 인벤토리 은행 슬롯
GfxInvenBankSlot::GfxInvenBankSlot()
:	m_nPosX( -1 )
,	m_nPosY( -1 )
{
}

GfxInvenBankSlot::~GfxInvenBankSlot()
{
}

void GfxInvenBankSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return;

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL != pWishList )
			pWishList->NewBankItem( m_sItemCustom );
	}
	// 인벤토리로 이동
	else if( true == bCtrl && false == bAlt && false == bShift )
	{
		pChar->ReqChargedItemTo( m_nPosX, m_nPosY );
	}
	// 인벤토리 이동 여부 물어보기
	else
	{
		const SINVENITEM* pItem = pChar->m_cInvenCharged.GetItem( (WORD) m_nPosX, (WORD) m_nPosY );
		if( NULL == pItem )
			return;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem( pItem->GetNativeID() );
		if( NULL == pItemData )
			return;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Inventory_BankItemClick,
			"-s, -n, -n", pItemData->GetName(), m_nPosX, m_nPosY );
	}
}

void GfxInvenBankSlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxInvenBankSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// 인벤토리 위시리스트 슬롯
GfxInvenWishListSlot::GfxInvenWishListSlot()
:	m_nPosX( -1 )
,	m_nPosY( -1 )
{
}

GfxInvenWishListSlot::~GfxInvenWishListSlot()
{
}

void GfxInvenWishListSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	std::string strHeader = NSWIDGET_SCRIPT::GetItemInfoHeader( m_sItemCustom );
	std::string strInfo = NSWIDGET_SCRIPT::GetItemInfo( m_sItemCustom, (int) GetType() );

	if( bAlt == false )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Inventory_WishItemShowTooltip,
			"-n, -n, -s, -s",
			m_sItemCustom.GetNativeID().Mid(),
			m_sItemCustom.GetNativeID().Sid(),
			strHeader.c_str(),
			strInfo.c_str() );
	}
}

void GfxInvenWishListSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		WishList::CClient* pWishList = GfxInterface::GetInstance()->GetGaeaClient()->GetWishList();
		if( NULL == pWishList )
			return;

		pWishList->RemoveItem( MAKELONG( (WORD) m_nPosX, (WORD) m_nPosY ) );
	}
}

void GfxInvenWishListSlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxInvenWishListSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// 인벤토리 휴지통 슬롯
GfxInvenTrashSlot::GfxInvenTrashSlot()
:	m_nPosX( -1 )
,	m_nPosY( -1 )
{
}

GfxInvenTrashSlot::~GfxInvenTrashSlot()
{
}

void GfxInvenTrashSlot::Reset()
{
	GfxItemBaseSlot::Reset();

	m_nPosX = -1;
	m_nPosY = -1;
}

bool GfxInvenTrashSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
			if( NULL == pChar )
				return false;

			pChar->m_sGarbageItem.RESET();

			int nPosX = 0;
			int nPosY = 0;
			pInvenSlot->GetPosition( nPosX, nPosY );

			pChar->m_sPreInventoryItem.SET( nPosX, nPosY );
			if( S_OK == pChar->ReqGarbageMoveItem() )
			{
				m_nPosX = nPosX;
				m_nPosY = nPosY;

				m_sItemCustom = pInvenSlot->GetItemCustom();
			}
		}
		break;
	}

	return false;
}

//-----------------------------------------------------------------------------------------
// 스킬 퀵슬롯
GfxSkillQuickSlot::GfxSkillQuickSlot()
:	GfxSkillBaseSlot()
,	m_nIndex( -1 )
,	m_nPrevIndex( -1 )
{
}

GfxSkillQuickSlot::~GfxSkillQuickSlot()
{
}

bool GfxSkillQuickSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	SCHARSKILL* pLearnSkill = pChar->GETLEARNED_SKILL( m_sNativeID );
	if( NULL == pLearnSkill )
		return false;

	m_nLevel = pLearnSkill->wLevel;

	return __super::GetTooltip( pTooltip );
}

void GfxSkillQuickSlot::HoldStart( bool bLock )
{
	// 페이지를 변경해서 드랍할 수 있기 때문에 
	// 홀드 시작되면 현재 값 미리 저장해둔다.
	m_sPrevNativeID = m_sNativeID;
	m_nPrevIndex = m_nIndex;
}

bool GfxSkillQuickSlot::PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock )
{
	// 드래그 된 슬롯이 없으면 퀵슬롯에서 뺀다.
	if( NULL == pTargetSlot )
	{
		if( 0 > m_nIndex )
			return false;

		GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
		if( NULL == pChar )
			return false;

		pChar->ReqSkillQuickReSet( (WORD) m_nIndex );

		return false;
	}

	return true;
}

bool GfxSkillQuickSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_SQS :
		{
			GfxSkillQuickSlot* pQuickSlot = dynamic_cast< GfxSkillQuickSlot* >( pHoldSlot );
			if( 0 > pQuickSlot->GetIndex() )
				return false;

			GLInputManager::GetInstance().ExternEvent_SkillTrayLeftClickUp( pQuickSlot->GetPrevNativeID(), m_nIndex );
			if( m_sNativeID != NATIVEID_NULL() )
			{
				GLInputManager::GetInstance().ExternEvent_SkillTrayLeftClickUp( m_sNativeID, pQuickSlot->GetPrevIndex() );
			}
			else
			{
				GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
				if( NULL == pChar )
					return false;

				pChar->ReqSkillQuickReSet( pQuickSlot->GetPrevIndex() );
			}
		}
		break;

	case EMGFX_SLOT_SKILL :
		{
			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
			if( NULL == pChar )
				return false;

			GfxSkillSlot* pSkillSlot = dynamic_cast< GfxSkillSlot* >( pHoldSlot );
			pChar->ReqSkillQuickSet( pSkillSlot->GetNativeID(), (WORD) m_nIndex );
		}
		break;
	}

	return false;
}

void GfxSkillQuickSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqSkillQuickReSet( m_nIndex );
}

//-----------------------------------------------------------------------------------------
// 아이템 퀵슬롯
GfxItemQuickSlot::GfxItemQuickSlot()
:	m_nIndex( -1 )
{
}

GfxItemQuickSlot::~GfxItemQuickSlot()
{
}

bool GfxItemQuickSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
			if( NULL == pChar )
				return false;

			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			// 퀵슬롯 등록
			if( pInvenSlot->GetItemCustom().GetRealNativeID() != NATIVEID_NULL() )
				pChar->ReqItemQuickSet( (WORD) m_nIndex );
		}
		break;
	}

	return false;
}

void GfxItemQuickSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqActionQ( (WORD) m_nIndex );
}

void GfxItemQuickSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( 0 > m_nIndex )
		return;

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
		if( NULL == pChar )
			return;

		pChar->ReqItemQuickReSet( (WORD) m_nIndex );
	}
}

//-----------------------------------------------------------------------------------------
// 미니게임 : 주사위 보상 아이템 슬롯;
GfxMiniGameDiceRewardSlot::GfxMiniGameDiceRewardSlot()
{
}
GfxMiniGameDiceRewardSlot::~GfxMiniGameDiceRewardSlot()
{
}

//-----------------------------------------------------------------------------------------
// 스킬창 슬롯
GfxSkillSlot::GfxSkillSlot()
{
}

GfxSkillSlot::~GfxSkillSlot()
{
}

void GfxSkillSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	GLSKILL* pSkill = GLogicData::GetInstance().GetSkillData( m_sNativeID );
	if( NULL == pSkill )
		return;

	// 패시브 스킬이면 등록되면 안된다.
	if( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
		return;

	pChar->ReqSkillQuickSet( GetNativeID(), pChar->GetEmptySkillQuick() );
}

//-----------------------------------------------------------------------------------------
// 탈것정보창 슬롯
GfxVehicleSlot::GfxVehicleSlot()
{
}

GfxVehicleSlot::~GfxVehicleSlot()
{
}

bool GfxVehicleSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	if( NULL == pHoldSlot )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
			if( NULL == pChar )
				return false;

			pChar->ReqVehicleChangeAccessory( Binding2Suit() );

		}
		break;
	}

	return true;
}

void GfxVehicleSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
		if( NULL == pChar )
			return;

		pChar->ReqVehicleRemoveSlotItem( Binding2Suit() );
	}
}

EMSUIT GfxVehicleSlot::Binding2Suit()
{
	int nParts = GLWidgetScript::GetInstance().LuaCallFunc< int >(
		NSWIDGET_SCRIPT::g_strFunc_Vehicle_Binding2Parts,
		"-s", m_strBindingID.c_str() );

	switch( nParts )
	{
	case 0 :
		return SUIT_VEHICLE_PARTS_A;

	case 1 :
		return SUIT_VEHICLE_PARTS_B;

	case 2 :
		return SUIT_VEHICLE_PARTS_C;

	case 3 :
		return SUIT_VEHICLE_PARTS_D;

	case 4 :
		return SUIT_VEHICLE_PARTS_E;

	case 5 :
		return SUIT_VEHICLE_PARTS_F;
	}

	return SUIT_NSIZE;
}

//-----------------------------------------------------------------------------------------
// NPC 판매 아이템 슬롯
GfxNpcMarketSlot::GfxNpcMarketSlot()
:	m_nPosX( -1 )
,	m_nPosY( -1 )
{
	m_bUseCompare = true;
}

GfxNpcMarketSlot::~GfxNpcMarketSlot()
{
}

bool GfxNpcMarketSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLItemTooltip::GetInstance().SetInvenPos( m_nPosX, m_nPosY );

	return __super::GetTooltip( pTooltip );
}

bool GfxNpcMarketSlot::GetSubTooltip(GFx::Value* pTooltipInfo)
{
	GLItemTooltip::GetInstance().SetInvenPos( m_nPosX, m_nPosY );

	return __super::GetSubTooltip( pTooltipInfo );
}

bool GfxNpcMarketSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			int nPosX = 0, nPosY = 0;
			pInvenSlot->GetPosition( nPosX, nPosY );

			const SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( nPosX, nPosY );
			if( NULL == pInvenItem )
				return false;

			// 들고있는 아이템이 있으면 팔기
			SNATIVEID sHoldItemId = holditem::GetHoldItem().GetNativeID();
			if( sHoldItemId == NATIVEID_NULL() )
				return false;

			const SITEM* pItemData = GLogicData::GetInstance().GetItem( sHoldItemId );
			if( NULL == pItemData )
				return false;

			// 판매 가능 여부
			bool bSale = true;
			if( false == holditem::GetHoldItem().IsSale() ||
				true == pInvenItem->sItemCustom.IsGM_GENITEM() )
			{
				bSale = false;
				holditem::ReleaseHoldItem();
			}

			// 팻카드일 경우
			if( ITEM_PET_CARD == pItemData->sBasicOp.emItemType )
			{
				const SITEMCUSTOM& sHoldItem = pInvenItem->sItemCustom;
				GLPetClient* pMyPet = pClient->GetPetClient();
				if( true == pMyPet->IsVALID() && sHoldItem.PetDbNum() == pMyPet->DbNum() )
				{
					bSale = false;
					holditem::ReleaseHoldItem();
				}
			}

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_SellItem,
				"-b, -b, -s, -n, -n",
				bSale, RANPARAM::bCheckConfirmSellItem,
				pItemData->GetName(),
				nPosX, nPosY );
		}
		break;
	}

	return false;
}

void GfxNpcMarketSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	SNATIVEID sNativeID = holditem::GetHoldItem().GetNativeID();
	if( sNativeID != NATIVEID_NULL() )
		return;

	// 아이템 바로 구매
	if( true == bCtrl && false == bAlt && false == bShift )
	{
		int nIndex = GLWidgetScript::GetInstance().LuaCallFunc< int >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_GetActiveMarket );

		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( GLNpcTalkClient::GetInstance().GetNpcID() );
		// 이럴 일이있나?;
		if ( !pCrowData )
			return;

		GLInventorySale *pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven ( nIndex ));
		if ( !pInven )
			return;

		// 교환;
		if ( pInven->GetSaleFuncType() == GLInventorySale::CROW_SALE_V_2 )
		{
			GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC sNetData;
			sNetData.sNID		 = GLNpcTalkClient::GetInstance().GetNpcID();
			sNetData.dwNpcGlobID = GLNpcTalkClient::GetInstance().GetNpcGlobalID();
			//sNetData.dwChannel   = nIndex;
			sNetData.emCrow      = GLNpcTalkClient::GetInstance().GetTalkCrow();
			sNetData.itemlist[0].dwChannel = nIndex;
			sNetData.itemlist[0].wPosX = m_nPosX;
			sNetData.itemlist[0].wPosY = m_nPosY;
			sNetData.itemlist[0].wBuyNum = 1;
			
			GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
			GLCharacter* pChar = pClient->GetCharacter();
			pChar->ReqExChangeNpcTo(&sNetData);
		}
		// 돈으로 구매;
		else if ( pInven->GetSaleFuncType() == GLInventorySale::CROW_SALE_V_1 )
		{
			GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter()->ReqNpcTo(
				GLNpcTalkClient::GetInstance().GetNpcID(),
				GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
				nIndex, m_nPosX, m_nPosY,
				GLNpcTalkClient::GetInstance().GetTalkCrow());
		}
	}
	// 위시리스트 등록
	else if( false == bCtrl && false == bAlt && true == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

		const SNATIVEID& sItemID = m_sItemCustom.GetNativeID();
		const SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemID );
		if( NULL == pItemData )
			return;

		GLLandManClient* pLandMan = pClient->GetActiveMap();
		if( NULL == pLandMan )
			return;

		std::tr1::shared_ptr< GLCrowClient > pCrow = pLandMan->GetCrow( 
			GLNpcTalkClient::GetInstance().GetNpcGlobalID() );
		if( NULL == pCrow )
			return;

		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL == pWishList )
			return;

		D3DXVECTOR3 vPos = pCrow->GetPosition();
		int nMapX = 0;
		int nMapY = 0;

		const GLMapAxisInfo& sMapAxis = pLandMan->GetMapAxisInfo();
		sMapAxis.Convert2MapPos( vPos.x, vPos.z, nMapX, nMapY );

		LONGLONG llPrice = pCrow->m_pCrowData->GetNpcSellPrice( sItemID.Id() );
		if( 0 == llPrice )
			llPrice = pItemData->sBasicOp.dwBuyPrice;

		SNATIVEID sNpcID = GLNpcTalkClient::GetInstance().GetNpcID();

		pWishList->NewNpcStoreItem( m_sItemCustom, sNpcID,
			pLandMan->GetGaeaMapID(), nMapX, nMapY, llPrice );
	}
	// 아이템 구매 확인
	else if( false == bCtrl && false == bAlt && false == bShift )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
		if( NULL == pItem )
			return;

		bool bPile = false;
		if( true == pItem->ISPILE() && 1 == pItem->GETAPPLYNUM() )
			bPile = true;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_BuyItem,
			"-s, -n, -n, -b", pItem->GetName(), m_nPosX, m_nPosY, bPile );
	}
}

void GfxNpcMarketSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	// 아이템 링크
	if( true == bCtrl && false == bAlt && false == bShift )
	{
		DWORD dwIndex = GLWidgetScript::GetInstance().LuaCallFunc< DWORD >( 
			NSWIDGET_SCRIPT::g_strFunc_NpcTalk_GetActiveMarket );

		SINVEN_POS sInvenPos;
		sInvenPos.wPosX = (WORD) m_nPosX;
		sInvenPos.wPosY = (WORD) m_nPosY;

		SLINK_DATA_BASIC sLinkDataBasic;
		sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
		sLinkDataBasic.sLinkType.wSType = ITEM_LINK_CROW_SALE;

		DWORD dwData = ( GLNpcTalkClient::GetInstance().GetNpcGlobalID() << 16 );
		dwData |= ( dwIndex & 0x0000FFFF );

		sLinkDataBasic.dwData1 = dwData;
		sLinkDataBasic.dwData2 = sInvenPos.dwPos;

		GLChatMan::GetInstance().AddChatLink( sLinkDataBasic );
	}
	// 카트에 넣기
	else if( false == bCtrl && false == bAlt && false == bShift )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sItemCustom.GetNativeID() );
		if( NULL == pItem )
			return;

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_NpcTalk_AddCart,
			"-s, -n, -n, -n, -n, -b, -n, -n", 
			pItem->GetName(), 
			m_sItemCustom.Mid(), m_sItemCustom.Sid(),
			m_nPosX, m_nPosY,
			pItem->ISPILE(), pItem->GETAPPLYNUM(), pItem->sDrugOp.wPileNum );
	}
}

void GfxNpcMarketSlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxNpcMarketSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// NPC 카트 아이템 슬롯
GfxNpcCartSlot::GfxNpcCartSlot()
:	m_wIndex( 0 )
{
}

GfxNpcCartSlot::~GfxNpcCartSlot()
{
}

void GfxNpcCartSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLNpcTalkClient::GetInstance().RemoveCart( m_wIndex );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_RemoveCart,
			"-n", m_wIndex );
	}
}
//-----------------------------------------------------------------------------------------
// 제작 슬롯
GfxProductSlot::GfxProductSlot()
{
	m_bUseCompare = true;
}

GfxProductSlot::~GfxProductSlot()
{
}

//-----------------------------------------------------------------------------------------
// 포인트샾 슬롯;
GfxPointShopSlot::GfxPointShopSlot()
{
}

GfxPointShopSlot::~GfxPointShopSlot()
{
}

void GfxPointShopSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL == pWishList )
			return;

		pWishList->NewPointStoreItem(
			m_sItemCustom,
			pClient->GetPointShopClient()->ItemPrice( m_sItemCustom.GetNativeID() ) );
	}
}

void GfxPointShopSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		if( pClient )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PointShop_UpdateCart, "-n", 
				pClient->GetPointShopClient()->AddCartList(m_sItemCustom) );
		}
	}
}

void GfxPointShopSlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxPointShopSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// 포인트샾 카트 슬롯;
GfxPointShopCartSlot::GfxPointShopCartSlot()
{
}

GfxPointShopCartSlot::~GfxPointShopCartSlot()
{
}

void GfxPointShopCartSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		if( pClient )
		{
			pClient->GetPointShopClient()->DelCartItem(m_sItemCustom, (m_nIndex - 1) );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PointShop_UpdateCart);
		}
	}
}

//-----------------------------------------------------------------------------------------
//거래 슬롯
GfxTradeSlot::GfxTradeSlot()
:	m_PosX( -1 )
,	m_PosY( -1 )
,	m_bMySlot( false )
{
}

GfxTradeSlot::~GfxTradeSlot()
{
}

bool GfxTradeSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	if( false == m_bMySlot )
		return false;

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();
	if( NULL == pChar )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{				
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			if( pInvenSlot->GetItemCustom().GetRealNativeID() != NATIVEID_NULL() )
			{
				if( FAILED( pChar->ReqTradeBoxTo( m_PosX, m_PosY ) ) )
					pClient->GetTradeClient()->ReSetPreItem();
			}
		}
		break;

	case EMGFX_SLOT_TRADE :
		{
			GfxTradeSlot* pTradeSlot = dynamic_cast< GfxTradeSlot* >( pHoldSlot );

			if( pTradeSlot->GetItemCustom().GetRealNativeID() != NATIVEID_NULL() )
				pChar->ReqTradeBoxTo( m_PosX, m_PosY );
		}
		break;
	}

	return true;
}

void GfxTradeSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// 위시리스트 등록 
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		PLANDMANCLIENT pLandMan = pClient->GetActiveMap();

		std::tr1::shared_ptr< GLCharClient > pCharClient = pLandMan->GetChar( 
			pClient->GetTradeClient()->GetTarTrade().GetTarget() );
		if( NULL == pCharClient )
			return;		

		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL != pWishList )
			pWishList->NewTradeItem( m_sItemCustom, std::string( pCharClient->GetName() ) );
	}
	// 거래할 아이템 넣기/제거/교환
	else if( false == bCtrl && false == bAlt && false == bShift )
	{
		if( false == m_bMySlot )
			return;

		GLCharacter* pChar = pClient->GetCharacter();
		if( NULL == pChar )
			return;

		pChar->ReqTradeBoxTo( m_PosX, m_PosY );
	}
}

void GfxTradeSlot::SetPosition( int nPosX, int nPosY )
{
	m_PosX = nPosX;
	m_PosY = nPosY;
}

void GfxTradeSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_PosX;
	nPosY = m_PosY;
}

//-----------------------------------------------------------------------------------------
//개인상점검색 슬롯
GfxPMSPointTradeSlot::GfxPMSPointTradeSlot()
{
}

GfxPMSPointTradeSlot::~GfxPMSPointTradeSlot()
{
}

bool GfxPMSPointTradeSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInventorySlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			int nPosX = -1, nPosY = -1;
			pInventorySlot->GetPosition( nPosX, nPosY );

			int nResult = pClient->GetPrivateMarketClient()->PointTradeCheckRegItem( (WORD) nPosX, (WORD) nPosY );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_RegisterItem,
				"-n, -n, -n, -n, -n, -s",
				nResult,
				pInventorySlot->GetItemCustom().GetNativeID().Mid(),
				pInventorySlot->GetItemCustom().GetNativeID().Sid(),
				nPosX,
				nPosY,
				pInventorySlot->GetItemCustom().GETNAME().c_str() );
		}
		break;
	}

	return true;
}

//-----------------------------------------------------------------------------------------
// 우편 슬롯
GfxPostBoxSlot::GfxPostBoxSlot()
{
}

GfxPostBoxSlot::~GfxPostBoxSlot()
{
}

bool GfxPostBoxSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	if( NULL == pHoldSlot )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
			if( NULL == pChar )
				return false;

			int nPosX = 0;
			int nPosY = 0;
			pInvenSlot->GetPosition( nPosX, nPosY );

			pChar->m_sPreInventoryItem.SET( nPosX, nPosY );
			if( S_OK == pChar->ReqPostBoxSendPageAttachItem() )
			{
				m_nPosX = nPosX;
				m_nPosY = nPosY;

				m_sItemCustom = pInvenSlot->GetItemCustom();

				GLWidgetScript::GetInstance().LuaCallFunc< bool >(
					NSWIDGET_SCRIPT::g_strFunc_PostBox_SendPageAttachSlot );
			}
		}
		break;
	}

	return true;
}

void GfxPostBoxSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_PostBox_SendPageResetSlot );

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->SetPostItemPos( SINVEN_POS() );
	pChar->SetPostItem( SITEMCUSTOM() );
}

//-----------------------------------------------------------------------------------------
// 우편 보조 슬롯
GfxPostBoxSubSlot::GfxPostBoxSubSlot()
{
}

GfxPostBoxSubSlot::~GfxPostBoxSubSlot()
{
}

//-----------------------------------------------------------------------------------------
//펫 장비 슬롯
GfxPetSlot::GfxPetSlot()
{
}

GfxPetSlot::~GfxPetSlot()
{
}

bool GfxPetSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLPetClient* pPetClient = pClient->GetPetClient();
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			if( pPetClient )
			{
				switch( m_nSlot )
				{
				case 1 :
					pPetClient->ReqChangeAccessory( SUIT_PET_A );
					break;

				case 2 :
					pPetClient->ReqChangeAccessory( SUIT_PET_B );
					break;
				}
			}
		}
		break;
	}

	return true;
}

void GfxPetSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		GLPetClient* pPetClient = pClient->GetPetClient();
		if( pPetClient )
		{
			switch( m_nSlot )
			{
			case 1 :
				pPetClient->ReqRemoveSlotItem( SUIT_PET_A );
				break;

			case 2 :
				pPetClient->ReqRemoveSlotItem( SUIT_PET_B );
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------
//정보보기 슬롯
GfxReferCharSlot::GfxReferCharSlot()
:	m_nSlot( 0 )
{
}

GfxReferCharSlot::~GfxReferCharSlot()
{
}

void GfxReferCharSlot::GetValue( GFx::Movie* pMovie, GFx::Value* pValue )
{
	NSGFX_SLOT::CreateSlot_Item( pMovie, pValue, m_sItemCustom, m_bNew, GetType(), true );
}

bool GfxReferCharSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	if ( pClient )
	{
		ReferChar::CClient* pReferChar = pClient->GetReferChar();
		if ( pReferChar )
		{
			GLCHARLOGIC* pTarget = pReferChar->GetCompleteData();
			if ( pTarget )
			{
				GLItemTooltip::GetInstance().SetOtherCharData( pTarget );
			}
		}
	}

	std::string strHeader = NSWIDGET_SCRIPT::GetItemInfoHeader( m_sItemCustom );
	std::wstring strHeaderW = GfxInterface::GetInstance()->EncodeText( strHeader );

	std::string strToolTip = NSWIDGET_SCRIPT::GetItemInfo( m_sItemCustom, GetType() );
	std::wstring strToolTipW = GfxInterface::GetInstance()->EncodeText( strToolTip );

	std::string strTrade = GLWidgetScript::GetInstance().LuaCallFunc< char* >( 
		NSWIDGET_SCRIPT::g_strFunc_GetItemTooltipTrade );
	std::wstring strTradeW = GfxInterface::GetInstance()->EncodeText( strTrade );

	pTooltip->SetMember( "type", (UInt32) GetType() );
	pTooltip->SetMember( "header", strHeaderW.c_str() );
	pTooltip->SetMember( "tooltip", strToolTipW.c_str() );
	pTooltip->SetMember( "trade", strTradeW.c_str() );
	pTooltip->SetMember( "itemid", (UInt32) m_sItemCustom.GetNativeID().Id() );

	// 툴팁 봤으면 신규 아이템 확인을 했다.
	m_bNew = false;

	return true;
}

void GfxReferCharSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

		ReferChar::CClient* pReferChar = pClient->GetReferChar();
		if( NULL == pReferChar )
			return;

		GLCHARLOGIC* pCharData = pReferChar->GetCompleteData();
		if( NULL == pCharData )
			return;

		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL != pWishList )
			pWishList->NewReferCharItem( m_sItemCustom, std::string( pCharData->GetName() ) );
	}
}

//-----------------------------------------------------------------------------------------
// 출석부 슬롯
GfxAttendanceAwardSlot::GfxAttendanceAwardSlot()
{
}

GfxAttendanceAwardSlot::~GfxAttendanceAwardSlot()
{
}

//-----------------------------------------------------------------------------------------
// 특별활동 판매 아이템 슬롯
GfxActivitySalesSlot::GfxActivitySalesSlot()
{
}

GfxActivitySalesSlot::~GfxActivitySalesSlot()
{
}



//-------------------------------------------------------------------------------------------
//클럽창고 슬롯
GfxClubStorageSlot::GfxClubStorageSlot()
:m_nSlot(0), m_PosX(0), m_PosY(0), m_Channel(0)
{
}

GfxClubStorageSlot::~GfxClubStorageSlot()
{
}

bool GfxClubStorageSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
	case EMGFX_SLOT_CLUBSTORAGE :
		{		
			GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

			if(pClient->GetMyClub()->FindPosItem(m_Channel, m_PosX, m_PosY))
			{						
				pClient->ReqClubItemExchangeToHoldItem(m_Channel, m_PosX, m_PosY);
				break;
			}

			pClient->ReqInvenItemToClub(m_Channel, m_PosX, m_PosY);
		}
		break;
	}

	return true;
}

void GfxClubStorageSlot::HoldStart( bool bLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	pClient->ReqClubItemToHoldItem(m_Channel, m_PosX, m_PosY);
}


//-----------------------------------------------------------------------------------------
// // 개조카드 아이템 슬롯
GfxRebuildCardItemSlot::GfxRebuildCardItemSlot()
{
}

GfxRebuildCardItemSlot::~GfxRebuildCardItemSlot()
{
}

bool GfxRebuildCardItemSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pHoldSlot );

			int nPosX = -1, nPosY = -1;
			pInvenSlot->GetPosition( nPosX, nPosY );

			RebuildCard::GetInstance().ReqRebuildMoveItem( false, nPosX, nPosY );
		}
		break;
	}

	return true;
}

void GfxRebuildCardItemSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
		RebuildCard::GetInstance().RemoveRebuildItem();
	}
}

//-----------------------------------------------------------------------------------------
// 개조카드 스템프 슬롯
GfxRebuildCardStempSlot::GfxRebuildCardStempSlot()
{
}

GfxRebuildCardStempSlot::~GfxRebuildCardStempSlot()
{
}

bool GfxRebuildCardStempSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_INVENTORY :
		{
			//GfxInventorySlot* pInvenSlot = dynamic_cast< GfxInventorySlot* >( pSlot );
			RebuildCard::GetInstance().ReqRebuildMoveStamp();
		}
		break;
	}

	return true;
}

void GfxRebuildCardStempSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	if( false == bCtrl && false == bAlt && false == bShift )
	{
		RebuildCard::GetInstance().RemoveRebuildStemp();
	}
}

//-----------------------------------------------------------------------------------------
// 아이템 미리보기 슬롯;
GfxItemPreviewSlot::GfxItemPreviewSlot()
{
}

GfxItemPreviewSlot::~GfxItemPreviewSlot()
{
}

//---------------------------------------------------------------------------------------
//개인상점 슬롯
GfxPrivateMarketSlot::GfxPrivateMarketSlot()
:	m_PosX( -1 )
,	m_PosY( -1 )
{
	m_bUseCompare = true;
}

GfxPrivateMarketSlot::~GfxPrivateMarketSlot()
{
}

bool GfxPrivateMarketSlot::GetTooltip( GFx::Value* pTooltip )
{
	GLItemTooltip::GetInstance().SetInvenPos( m_PosX, m_PosY );

	return __super::GetTooltip( pTooltip );
}

void GfxPrivateMarketSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::LeftMouseClick( bLock, bCtrl, bAlt, bShift );

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	PrivateMarketClient* pPrivateMarket = pClient->GetPrivateMarketClient();

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		std::tr1::shared_ptr< GLCharClient > pCharClient = pClient->GetChar( pPrivateMarket->GetOpenerID() );
		if( NULL == pCharClient )
			return;

		SNATIVEID sSalePos( m_PosX, m_PosY );
		const SSALEITEM* pSaleItem = pCharClient->m_sPMarket.GetItem( sSalePos );
		if( NULL == pSaleItem )
			return;

		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL == pWishList )
			return;

		pWishList->NewPrivatePersonItem( m_sItemCustom,	pSaleItem->llPRICE, std::string( pCharClient->GetName() ) );
	}
	else if( false == bCtrl && false == bAlt && false == bShift )
	{
		if( pPrivateMarket->IsOpener() )
			pPrivateMarket->ReqPrivateMarketDelItem( m_PosX, m_PosY );
		else
			pPrivateMarket->ReqPMarketBuy( m_PosX, m_PosY );
	}
}

void GfxPrivateMarketSlot::SetPosition( int nPosX, int nPosY )
{
	m_PosX = nPosX;
	m_PosY = nPosY;
}

void GfxPrivateMarketSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_PosX;
	nPosY = m_PosY;
}

//---------------------------------------------------------------------------------------
// 개인 상점 검색 슬롯
GfxPrivateMarketSearchSlot::GfxPrivateMarketSearchSlot()
{
}
GfxPrivateMarketSearchSlot::~GfxPrivateMarketSearchSlot()
{
}

void GfxPrivateMarketSearchSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL == pWishList )
			return;

		pWishList->NewInventoryItem( m_sItemCustom );
	}
}

//---------------------------------------------------------------------------------------
// 락커 슬롯
GfxLockerSlot::GfxLockerSlot()
:	m_nIndex( -1 )
,	m_nPosX( -1 )
,	m_nPosY( -1 )
{
}

GfxLockerSlot::~GfxLockerSlot()
{
}

void GfxLockerSlot::HoldStart( bool bLock )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	pChar->ReqStorageTo(
		GLNpcTalkClient::GetInstance().GetNpcID(),
		GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
		m_nIndex, m_nPosX, m_nPosY );
}

bool GfxLockerSlot::PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock )
{
	if( NULL == pHoldSlot )
		return false;

	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return false;

	switch( pHoldSlot->GetType() )
	{
	case EMGFX_SLOT_LOCKER :
	case EMGFX_SLOT_INVENTORY :
		{
			pChar->ReqStorageTo(
				GLNpcTalkClient::GetInstance().GetNpcID(),
				GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
				m_nIndex, m_nPosX, m_nPosY );
		}
		return true;
	}

	return false;
}

void GfxLockerSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	__super::RightMouseClick( bLock, bCtrl, bAlt, bShift );

	if( false == bCtrl && false == bAlt && false == bShift )
	{
		GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
		if( NULL == pChar )
			return;

		pChar->ReqStorageDrug( 
			GLNpcTalkClient::GetInstance().GetNpcID(),
			GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
			m_nIndex, m_nPosX, m_nPosY );
	}
}

void GfxLockerSlot::SetPosition( int nPosX, int nPosY )
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void GfxLockerSlot::GetPosition( int& nPosX, int& nPosY )
{
	nPosX = m_nPosX;
	nPosY = m_nPosY;
}

//-----------------------------------------------------------------------------------------
// 버프 슬롯
GfxBuffSlot::GfxBuffSlot()
{
}

GfxBuffSlot::~GfxBuffSlot()
{
}

void GfxBuffSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pChar )
		return;

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( m_sNativeID );
	if( NULL == pSkill )
		return;

	if( EMSKILL_LUNCHBOX == static_cast< EMSKILLCLASS >( pSkill->m_sBASIC.sNATIVEID.wMainID ) )
	{
		GLGaeaClient* pClient =  GfxInterface::GetInstance()->GetGaeaClient();

		GLLandManClient* pLand = pClient->GetActiveMap();
		if( NULL == pLand )
			return;

		// 버프 삭제 불가능 지역
		bool bForbid = pLand ? pLand->IsLunchBoxForbid() : true;
		if( true == bForbid )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Buff_ImpossibleDelete,
				"-s", pSkill->GetName() );

			return;
		}
	}

	// 버프만 삭제할 수 있다.
	if( SKILL::EMACTION_TYPE_BUFF == pSkill->m_sBASIC.emACTION_TYPE )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Buff_MsgDelete,
			"-dw, -s, -s", 
			pSkill->GetId().dwID, pSkill->GetName(), m_strBindingID.c_str() );

		return;
	}
}

//-----------------------------------------------------------------------------------------
// 세트 아이템 슬롯
GfxSetItemSlot::GfxSetItemSlot()
{
}

GfxSetItemSlot::~GfxSetItemSlot()
{
}

//-----------------------------------------------------------------------------------------
// 소환수 슬롯;
void GfxSetItemSlot::RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
}

GfxSummonStateSlot::GfxSummonStateSlot(void)
{
}

GfxSummonStateSlot::~GfxSummonStateSlot(void)
{
}

void GfxSummonStateSlot::GetValue( GFx::Movie* pMovie, GFx::Value* pValue )
{
	NSGFX_SLOT::CreateSlot_SummonState(pMovie, pValue, m_strIconPath, m_nIconIdxX, m_nIconIdxY);
}

void GfxSummonStateSlot::SetIconData(std::string strIconPath, INT nIconIdxX, INT nIconIdxY)
{
	m_strIconPath = strIconPath;
	m_nIconIdxX = nIconIdxX;
	m_nIconIdxY = nIconIdxY;
}

//-----------------------------------------------------------------------------------------
// 파티 분배
GfxPartyDistributionSlot::GfxPartyDistributionSlot()
: m_nSlotIndex(-1)
{
}

GfxPartyDistributionSlot::~GfxPartyDistributionSlot()
{
}

//-----------------------------------------------------------------------------------------
// 아이템 링크 정보
GfxItemLinkInfoSlot::GfxItemLinkInfoSlot()
{
}

GfxItemLinkInfoSlot::~GfxItemLinkInfoSlot()
{
}

void GfxItemLinkInfoSlot::LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// 위시리스트 등록
	if( false == bCtrl && false == bAlt && true == bShift )
	{
		WishList::CClient* pWishList = pClient->GetWishList();
		if( NULL == pWishList )
			return;

		pWishList->NewInventoryItem( m_sItemCustom );
	}
}
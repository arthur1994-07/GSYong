#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"

#include "../Item/ItemImage.h"

#include "ItemInfoSimpleToolTip.h"

ItemInfoSimpleToolTip::ItemInfoSimpleToolTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
{
}
ItemInfoSimpleToolTip::~ItemInfoSimpleToolTip(void)
{
}

void ItemInfoSimpleToolTip::CreateSubControl(void)
{
	CreateLineBox("ITEM_INFOR_SIMPLE_TOOLTIP", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	m_pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
	m_pItemImage->CreateSub( this, "ITEM_INFOR_TOOLTIP_ITEM_IMAGE", UI_FLAG_DEFAULT, ITEM_IMAGE );
	m_pItemImage->CreateSubControl();
	RegisterControl( m_pItemImage );	

	CD3DFontPar* pFont12 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );
	m_pItemName = new CBasicTextBox(m_pEngineDevice);
	m_pItemName->CreateSub(this, "ITEM_INFOR_TOOLTIP_ITEM_NAME");
	m_pItemName->SetFont( pFont12 );
	m_pItemName->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
	RegisterControl( m_pItemName );

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_EX_FLAG );
	m_pItemInfo = new CBasicTextBox(m_pEngineDevice);
	m_pItemInfo->CreateSub(this, "ITEM_INFOR_TOOLTIP_ITEM_LABEL1");
	m_pItemInfo->SetFont( pFont9 );
	m_pItemInfo->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
	RegisterControl(m_pItemInfo);	
}

void ItemInfoSimpleToolTip::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case ITEM_IMAGE:
		if ( CHECK_MOUSE_IN(dwMsg) && m_ItemCustom.GetNativeID() != SNATIVEID() )	
		{
			m_pInterface->SHOW_ITEM_INFO(
				m_ItemCustom, 
				FALSE, FALSE, FALSE, TRUE, FALSE, 
				WORD(m_vMousePosition.x), WORD(m_vMousePosition.y), SNATIVEID());			
		}
		break;
	}
//	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);
}

void ItemInfoSimpleToolTip::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	m_vMousePosition = D3DXVECTOR2(float(x), float(y));
}


const bool ItemInfoSimpleToolTip::SetItemInformation(const SITEMCUSTOM& ItemCustom)
{
	const SITEM* const pItemData = GLogicData::GetInstance().GetItem(ItemCustom.GetNativeID());
	if ( pItemData == NULL )
		return false;

	m_bNotAvailable = 0;
	m_pItemInfo->ClearText();
	SetItemImage(pItemData); // 아이콘;
	SetItemName(ItemCustom, pItemData); // 이름;

	// 아이템의 정보를 설정 하면서 m_bNotAvailable(착용 불가능 플래그)의 값을 정함;
	SetItemInfo(ItemCustom, pItemData); // 학원, 클래스, 성별, 레벨, 스탯, 거래 가/부;

	m_ItemCustom = ItemCustom;

	return m_bNotAvailable ? true : false;
}

void ItemInfoSimpleToolTip::SetItemImage(const SITEM* const pItemData)
{
	m_pItemImage->ResetItem();
	m_pItemImage->SetItem(pItemData->sBasicOp.sNativeID);
}

void ItemInfoSimpleToolTip::SetItemName(const SITEMCUSTOM& ItemCustom, const SITEM* const pItemData)
{
	m_pItemName->SetTextToLimit( ItemCustom.GETNAME().c_str(), FALSE, ITEM_INFOR_COLOR::dwItemRank[pItemData->sBasicOp.emLevel]);
}



void ItemInfoSimpleToolTip::SetItemInfo_School(const DWORD dwReqSchool, const int nIndex)
{
	if ( dwReqSchool == GLSCHOOL_ALL )
	{
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD( "ACADEMY_NAME", 3 ), ITEM_INFOR_COLOR::dwDefault );
		m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
	}
	else
	{
		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();		
		const DWORD dwColor = (dwReqSchool & index2school(pCharacter->m_wSchool)) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;	
		const EMSCHOOLFLAG emSchoolFlag[GLSCHOOL_NUM] = {GLSCHOOL_00, GLSCHOOL_01, GLSCHOOL_02};

		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);

		for ( DWORD _i = 0; _i < GLSCHOOL_NUM; ++_i )
		{
			if ( dwReqSchool & emSchoolFlag[_i])
			{
				m_pItemInfo->AddString( nIndex,
					GLCONST_CHAR::strSCHOOLNAME[school2index(emSchoolFlag[_i])].c_str(),
					dwColor );

				m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
			}
		}		
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Class(const SITEMCUSTOM& ItemCustom, const DWORD dwReqCharClass, const int nIndex)
{	
	if ( dwReqCharClass )
	{ // Info : 착용 클래스;
		if ( ItemCustom.nidDISGUISE != NATIVEID_NULL() )
			SetItemInfo_Class_Costume(dwReqCharClass, ItemCustom, nIndex);
		else
			SetItemInfo_Class_Normal(dwReqCharClass, ItemCustom, nIndex);
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Class_Costume(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex)
{
	const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem(ItemCustom.nidDISGUISE);
	if ( ! pItemDisguise )
		return;

	const DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;			

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwCharClass = DWORD(pCharacter->m_emClass);
	const DWORD dwColor = ((dwReqCharClass & dwCharClass) && (dwReqCharClass_Disguise & dwCharClass)) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;

	m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
	if ( ((dwReqCharClass == GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise == GLCC_ALL_TRICKER)) ||
		((dwReqCharClass == GLCC_MAN) && (dwReqCharClass_Disguise == GLCC_MAN)) ||
		((dwReqCharClass == GLCC_WOMAN) && (dwReqCharClass_Disguise == GLCC_WOMAN)) )
	{
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD("ITEM_ALL_CLASS"), ITEM_INFOR_COLOR::dwDefault );
		return;
	}
	
	const EMCHARCLASS emClassFlag[] =
	{
		GLCC_FIGHTER,
		GLCC_ARMS,
		GLCC_ARCHER,
		GLCC_SPIRIT,
		GLCC_EXTREME,
		GLCC_SCIENTIST,
		GLCC_ASSASSIN,
		GLCC_TRICKER,
		GLCC_ETC
	};
	
	bool bDistinction = false;
	const DWORD nClass(GLCI_NUM_ETC >> 1);
	for ( DWORD _i = 0; _i < nClass; ++_i )
	{
		if ( (dwReqCharClass & emClassFlag[_i]) & (dwReqCharClass_Disguise & emClassFlag[_i]) )
		{
			if ( bDistinction )
				m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
			bDistinction = true;

			m_pItemInfo->AddString( nIndex,
				ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex(emClassFlag[_i]) ),
				dwColor );
		}
	}
}
void ItemInfoSimpleToolTip::SetItemInfo_Class_Normal(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwCharClass = DWORD(pCharacter->m_emClass);
	const DWORD dwColor = (dwReqCharClass & pCharacter->m_emClass) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;

	if ( (dwReqCharClass == GLCC_ALL_TRICKER) ||
		(dwReqCharClass == GLCC_MAN) ||
		(dwReqCharClass == GLCC_WOMAN) )
	{
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD("ITEM_ALL_CLASS"), ITEM_INFOR_COLOR::dwDefault );
		return;
	}

	m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);	
	const EMCHARCLASS emClassFlag[] =
	{
		GLCC_FIGHTER,
		GLCC_ARMS,
		GLCC_ARCHER, 
		GLCC_SPIRIT,
		GLCC_EXTREME, 
		GLCC_SCIENTIST, 
		GLCC_ASSASSIN, 
		GLCC_TRICKER, 
		GLCC_ETC
	};

	bool bDistinction = false;
	const DWORD nClass(GLCI_NUM_ETC >> 1);
	for ( DWORD _i = 0; _i < nClass; ++_i )
	{
		if ( dwReqCharClass & emClassFlag[_i] )
		{
			if ( bDistinction )
				m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
			bDistinction = true;

			m_pItemInfo->AddString( nIndex,
				ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex(emClassFlag[_i]) ),
				dwColor );
		}
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Sex_Costume(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex)
{
	const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem(ItemCustom.nidDISGUISE);
	if ( pItemDisguise == NULL )
		return;

	const DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;

	if ( (dwReqCharClass == GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise == GLCC_ALL_TRICKER) )
		return;
	
	// 남여 둘다 참이면 리턴;
	const bool bReqMan = dwReqCharClass & GLCC_MAN;
	const bool bReqWoman = dwReqCharClass & GLCC_WOMAN;
	if ( bReqMan && bReqWoman )
		return;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwCharClass = DWORD(pCharacter->m_emClass);

	m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
	if ( bReqMan )
	{			
		const DWORD dwColor = (dwCharClass & GLCC_MAN) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
	}
	else
	{
		const DWORD dwColor = (dwCharClass & GLCC_WOMAN) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Sex_Normal(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex)
{
	if ( dwReqCharClass == GLCC_ALL_TRICKER )
		return;

	bool bMan = false;
	bool bWoman = false;

	const bool bReqMan = dwReqCharClass & GLCC_MAN;
	const bool bReqWoman = dwReqCharClass & GLCC_WOMAN;	
	if ( bReqMan && bReqWoman )
		return;

	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();	
	const DWORD dwCharClass = DWORD(pCharacter->m_emClass);

	m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );
	if ( bReqMan )
	{			
		const DWORD dwColor = (dwCharClass & GLCC_MAN) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
	}
	else
	{
		const DWORD dwColor = (dwCharClass & GLCC_WOMAN) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
		m_pItemInfo->AddString( nIndex, ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Sex(const SITEMCUSTOM& ItemCustom, const DWORD dwReqCharClass, const int nIndex)
{
	if ( ItemCustom.nidDISGUISE != NATIVEID_NULL() )
		SetItemInfo_Sex_Costume(dwReqCharClass, ItemCustom, nIndex);
	else
		SetItemInfo_Sex_Normal(dwReqCharClass, ItemCustom, nIndex);		
}

const bool ItemInfoSimpleToolTip::SetItemInfo_Condition_Level(const DWORD dwReqLevelDW, const DWORD dwReqLeveUP, const int nIndex)
{
	if ( (dwReqLevelDW == 0) &&  (dwReqLeveUP == 0) )
		return false;
	
	std::string strTemp = sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 0), 
		dwReqLevelDW );

	bool bReqLevel = true;

	const GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
	const WORD wLevel = pChar->m_wLevel;
	if ( dwReqLeveUP )
	{
		strTemp += sc::string::format( "~%d", dwReqLeveUP );

		bReqLevel = (dwReqLeveUP >= wLevel );
	}

	const DWORD dwColor =  ( (dwReqLevelDW <= wLevel) && bReqLevel ) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
	m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);
	m_pItemInfo->AddString( nIndex, strTemp.c_str(), dwColor );

	return true;
}
void ItemInfoSimpleToolTip::SetItemInfo_Condition_Status(const SCHARSTATS& rItemStats, bool bExistFrontValue, const int nIndex)
{
	const GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
	//2014.9.18 스킬효과의 스텟증가효과도 아이템 장착수치 계산에 포함시키기 위해 수정(CRM#3014)
	const SCHARSTATS& rCharStats = pChar->m_sSUMSTATS_SKILL;	

	const WORD wItemStats[EMSIZE][2] = {
		rItemStats.wPow, rCharStats.wPow, // 힘;
		rItemStats.wStr, rCharStats.wStr, // 체력;
		rItemStats.wSpi, rCharStats.wSpi, // 정신력;
		rItemStats.wDex, rCharStats.wDex, // 민첩성;
		rItemStats.wInt, rCharStats.wInt, // 지력;
		rItemStats.wSta, rCharStats.wSta // 근력;
	};

	for ( DWORD _i = 0; _i < EMSIZE; ++_i )
	{
		const WORD wItemStat = wItemStats[_i][0];
		if ( wItemStat == 0 )
			continue;		

		const WORD wCharStat = wItemStats[_i][1];
		const DWORD dwColor = (wItemStat <= wCharStat) ? ITEM_INFOR_COLOR::dwDefault : ITEM_INFOR_COLOR::dwReqWrong;
		m_bNotAvailable += ((dwColor == ITEM_INFOR_COLOR::dwDefault) ? 0 : 1);

		if ( bExistFrontValue )			
			m_pItemInfo->AddString( nIndex, "/", ITEM_INFOR_COLOR::dwDefault );		
		bExistFrontValue = true;

		m_pItemInfo->AddString( nIndex, 
			sc::string::format( "%s %d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 0 ), rItemStats.wPow ).c_str(), 
			dwColor );
	}
}

void ItemInfoSimpleToolTip::SetItemInfo_Condition(const SITEM* const pItemData, const int nIndex)
{	
	const bool bExistFrontValue = SetItemInfo_Condition_Level(pItemData->sBasicOp.wReqLevelDW, pItemData->sBasicOp.wReqLevelUP, nIndex);
	SetItemInfo_Condition_Status(pItemData->sBasicOp.sReqStats, bExistFrontValue, nIndex);
}

void ItemInfoSimpleToolTip::SetItemInfo(const SITEMCUSTOM& ItemCustom, const SITEM* const pItemData)
{	
	{ // 학원/직업/성별;
		const int nIndex = m_pItemInfo->AddText( "", ITEM_INFOR_COLOR::dwDefault );
		SetItemInfo_School(pItemData->sBasicOp.dwReqSchool, nIndex);
		SetItemInfo_Class(ItemCustom, pItemData->sBasicOp.dwReqCharClass, nIndex);
		SetItemInfo_Sex(ItemCustom, pItemData->sBasicOp.dwReqCharClass, nIndex);	
	}
	
	{ // 레벨/스텟;
		const int nIndex = m_pItemInfo->AddText( "", ITEM_INFOR_COLOR::dwDefault );
		SetItemInfo_Condition(pItemData, nIndex);
	}
}

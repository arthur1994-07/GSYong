#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Item/InventoryWindow.h"
#include "../Item/InventoryInterface.h"

#include "StatsResetWindow.h"
#include "StatsResetComboBoxRollOver.h"
#include "StatsInfo.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStatsResetWindow::CStatsResetWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice ) :
	CUIWindowEx	 ( pInterface, pEngineDevice ),
	m_pGaeaClient	 ( pGaeaClient ),

	m_pStats         ( NULL ),
	m_pStats_Preview ( NULL ),

	m_pPointDisable  ( NULL ),
	m_pPointEnable   ( NULL ),
	m_pPointText     ( NULL ),

	m_pPointDisable_Preview  ( NULL ),
	m_pPointEnable_Preview   ( NULL ),
	m_pPointText_Preview	 ( NULL ),

	m_pReturn_Stats_Text ( NULL ),

	m_pReturn_Item_ComboBox ( NULL ),

	m_pInfo_Head_Text ( NULL ),
	m_pInfo_Tail_Text ( NULL ),

	m_wReturnStatsMax	(0),
	m_wReturnStats		(0),
	m_wReturnSkillPoint (0),
	m_wReturnItem		(0),
	m_emItemType		(ITEM_SUIT),

	m_pReset_Button ( NULL ),

	m_emType            (UI::EMSTATSRESETWINDOW_TYPE_SKILL),
	m_emInfoTextHead	(EMINFOTEXTHEAD_NONE),
	m_emInfoTextTail	(EMINFOTEXTTAIL_NONE)
//  m_wRemainInven(0),
//  m_wReqInven(0)
{
	memset(m_wResetStats, 0, sizeof(WORD)*EMSIZE);
}

CStatsResetWindow::~CStatsResetWindow ()
{
}



CBasicLineBox* CStatsResetWindow::CreateBackgroundControl ( char* szConatrolKeyword )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szConatrolKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	
	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

CBasicTextBox* CStatsResetWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CStatsResetWindow::CreateSubControl ()
{
	CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	CD3DFontPar* pFont        = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nAlignLeft      = TEXT_ALIGN_LEFT;
	const int nAlignCenter    = TEXT_ALIGN_CENTER_X;		
	const DWORD& dwSilver     = NS_UITEXTCOLOR::SILVER;
	const DWORD& dwSubTitle   = NS_UITEXTCOLOR::PALEGOLDENROD;

    CBasicTextBox* pTextBox = NULL;

	//CreateControl ( "STATSRESET_SOCIAL_BACK_L" );
	//CreateControl ( "STATSRESET_SOCIAL_BACK_R" );
	
	// Note : 배경색
	{
		CreateBackgroundControl ( "STATSRESET_BACKGROUND_HEAD"   );
		CreateBackgroundControl ( "STATSRESET_BACKGROUND_ITEM"   );
		CreateBackgroundControl ( "STATSRESET_BACKGROUND_BUTTON" );
	}

	// Note : 항목 텍스트/이미지
	{
		// 현재스탯
		CreateControl ( "STATSRESET_TOPIC_STATS_CURRENT_BAR" );
		m_pTopic_Stats_Current = CreateStaticControl ( "STATSRESET_TOPIC_STATS_CURRENT", pFont, nAlignCenter );

		 // 반환설정/반환정보
		CreateControl ( "STATSRESET_TOPIC_STATS_PREVIEW_BAR" );
		m_pTopic_Stats_Preview = CreateStaticControl ( "STATSRESET_TOPIC_STATS_PREVIEW", pFont, nAlignCenter );

		// 반환아이템
		CreateControl ( "STATSRESET_TOPIC_ITEM_RETURN_BAR" );
		m_pTopic_Item_Return = CreateStaticControl ( "STATSRESET_TOPIC_ITEM_RETURN", pFont, nAlignCenter );
	}

	// Note : 현재 스탯
	{
		m_pStats = new CStatsInfo(m_pEngineDevice);
		m_pStats->CreateSub ( this, "STATSRESET_INFO_DEFAULT", UI_FLAG_YSIZE, STATSRESET_STATSINFO_DEFAULT );
		m_pStats->CreateSubControl ();
		RegisterControl ( m_pStats );

		{	//	Note: 가용포인트
			m_pPointDisable = CreateControl ( "STATSRESET_POINT_DISABLE_DEFAULT" );
			m_pPointEnable  = CreateControl ( "STATSRESET_POINT_ENABLE_DEFAULT" );

			CreateControl ( "STATSRESET_POINT_BACK_DEFAULT" );
			m_pPointText = CreateStaticControl ( "STATSRESET_POINT_TEXT_DEFAULT", pFont8Shadow, nAlignCenter );
		}
	}

	// Note : 반환 스탯
	{
		m_pStats_Preview = new CStatsInfo(m_pEngineDevice);
		m_pStats_Preview->CreateSub ( this, "STATSRESET_INFO_PREVIEW", UI_FLAG_YSIZE, STATSRESET_STATSINFO_PREVIEW );
		m_pStats_Preview->CreateSubControl ();
		RegisterControl ( m_pStats_Preview );

		{	//	Note: 가용포인트
			m_pPointDisable_Preview = CreateControl ( "STATSRESET_POINT_DISABLE_PREVIEW" );
			m_pPointEnable_Preview  = CreateControl ( "STATSRESET_POINT_ENABLE_PREVIEW" );

			CreateControl ( "STATSRESET_POINT_BACK_PREVIEW" );
			m_pPointText_Preview = CreateStaticControl ( "STATSRESET_POINT_TEXT_PREVIEW", pFont8Shadow, nAlignCenter );
		}
	}

	// Note : 반환 아이템
	{
		m_pReturn_Item_ComboBox = new CStatsResetComboBoxRollOver(m_pEngineDevice);
		m_pReturn_Item_ComboBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, STATSRESET_ITEM_COMBO_ROLLOVER );
		m_pReturn_Item_ComboBox->CreateBaseComboBoxRollOver ( "STATSRESET_ITEM_COMBO_ROLLOVER" );			
		m_pReturn_Item_ComboBox->SetVisibleSingle ( TRUE );
		RegisterControl ( m_pReturn_Item_ComboBox );
	}

	// Note : 반환 스텟/스킬/아이템 텍스트
	{
		m_pReturn_Stats_Text = CreateStaticControl ( "STATSRESET_STATIC_RETURN_STATS", pFont, nAlignLeft );
		m_pReturn_Skill_Text = CreateStaticControl ( "STATSRESET_STATIC_RETURN_SKILL", pFont, nAlignLeft );
		m_pReturn_Item_Text  = CreateStaticControl ( "STATSRESET_STATIC_RETURN_ITEM",  pFont, nAlignLeft );
        //m_pReq_Inven_Text = CreateStaticControl ( "STATSRESET_STATIC_REQ_INVEN",  pFont, nAlignLeft );
	}

	// Note : 가변 정보 텍스트 박스
	{
		m_pInfo_Head_Text = CreateStaticControl ( "STATSRESET_INFO_HEAD_TEXTBOX", pFont, nAlignLeft );
		m_pInfo_Tail_Text = CreateStaticControl ( "STATSRESET_INFO_TAIL_TEXTBOX", pFont, nAlignLeft );
	}

	// Note : 버튼
	{ 
		
        CBasicTextButton* pButton;
        pButton = new CBasicTextButton(m_pEngineDevice);
        pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, STATSRESET_BUTTON_OK );
        pButton->CreateBaseButton( "STATSRESET_BUTTON_OK", CBasicTextButton::SIZE14, 
            CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "STATSRESET_BUTTON", 0 ) );
        RegisterControl( pButton );

        pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, STATSRESET_BUTTON_CANCEL );
		pButton->CreateBaseButton( "STATSRESET_BUTTON_CANCEL", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "STATSRESET_BUTTON", 1 ) );
		RegisterControl( pButton );

		m_pReset_Button = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, STATSRESET_BUTTON_RESET );
		pButton->CreateBaseButton( "STATSRESET_BUTTON_RESET", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "STATSRESET_BUTTON", 2 ) );
		RegisterControl( pButton );
	}
}

void CStatsResetWindow::Close()
{
	IInventoryWindow* pInvenWindow = m_pInterface->GetInventoryWindow();
	if( pInvenWindow )
		pInvenWindow->UnLockTrade();

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory )
		pInventory->UnLockTrade();

	m_pInterface->UiHideGroup ( STATSRESET_WINDOW );
}

void CStatsResetWindow::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	CUIGroup::SetGlobalPos( vPos );

	const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();	
	
	//SetBright( sCharData );
	//SetCP( sCharData );
}

void CStatsResetWindow::ReqResetSkillStats()
{
	if ( !m_pStats_Preview )
	{
		return;
	}

	m_pStats_Preview->GetDecreaseStats(m_wResetStats);

	m_pGaeaClient->GetCharacter()->ReqResetSkillStats(m_wResetStats, m_wReturnStatsMax, m_emItemType);
}


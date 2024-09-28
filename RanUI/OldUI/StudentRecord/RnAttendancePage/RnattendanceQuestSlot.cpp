#include "StdAfx.h"

#include "../../../../SigmaCore/String/StringFormat.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../InnerInterface.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../Util/RnButton.h"
#include "RnattendanceQuestSlot.h"

const char* szButtonNormal = "RNBUTTON_DEFAULT_TEXTURE";
const char* szButtonOver = "RNBUTTON_OVER_TEXTURE";
const char* szButtonDown = "RNBUTTON_CLICK_TEXTURE";

const char* RNATTENDANCE_QUEST_SLOT_REWARD_BUTTON = "보상 받기";
const char* RNATTENDANCE_QUEST_SLOT_SELECT_BUTTON = "선  택";
const char* RNATTENDANCE_QUEST_SLOT_MONEY = "란머니";

RnAttendanceQuestSlot::RnAttendanceQuestSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pQuestNameTextBox(NULL)
, m_dwQuestId(UINT_MAX)
, m_pRewardButton(NULL)
, m_pSelectButton(NULL)
{
	RNATTENDANCE_QUEST_SLOT_REWARD_BUTTON = ID2GAMEINTEXT("RNATTENDANCE_QUEST_SLOT_REWARD_BUTTON");
	RNATTENDANCE_QUEST_SLOT_SELECT_BUTTON = ID2GAMEINTEXT("RNATTENDANCE_QUEST_SLOT_SELECT_BUTTON");
	RNATTENDANCE_QUEST_SLOT_MONEY = ID2GAMEINTEXT("RNATTENDANCE_QUEST_SLOT_MONEY");
}

RnAttendanceQuestSlot::~RnAttendanceQuestSlot()
{

}

void RnAttendanceQuestSlot::CreateSubControl()
{
	CUIGroup* pQuestSlotBG = new CBasicLineBoxSmart(m_pEngineDevice);
	pQuestSlotBG->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_BG_IMAGE" );
	RegisterControl( pQuestSlotBG );

	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	//과제 넘버
	CBasicTextBoxEx* pCourseTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pCourseTextBox->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_COURSE" );
	pCourseTextBox->SetFont( pFont9 );
	pCourseTextBox->SetTextAlign( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	pCourseTextBox->SetLineInterval ( 5.0f );
	pCourseTextBox->SetPartInterval ( 2.0f );
	RegisterControl( pCourseTextBox );
	m_pCourseTextBox = pCourseTextBox;

	//과제 이름
	CBasicTextBoxEx* pQuestNameTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pQuestNameTextBox->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_QUEST_NAME" );
	pQuestNameTextBox->SetFont( pFont9 );
	pQuestNameTextBox->SetTextAlign( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	pQuestNameTextBox->SetLineInterval ( 5.0f );
	pQuestNameTextBox->SetPartInterval ( 2.0f );
	RegisterControl( pQuestNameTextBox );
	m_pQuestNameTextBox = pQuestNameTextBox;

	//란머니
	CBasicTextBoxEx* pMoneyTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pMoneyTextBox->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_QUEST_MONEY" );
	pMoneyTextBox->SetFont( pFont9 );
	pMoneyTextBox->SetTextAlign( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	pMoneyTextBox->SetLineInterval ( 5.0f );
	pMoneyTextBox->SetPartInterval ( 2.0f );
	RegisterControl( pMoneyTextBox );
	m_pMoneyTextBox = pMoneyTextBox;

	// 경험치
	CBasicTextBoxEx* pExpTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pExpTextBox->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_QUEST_EXP" );
	pExpTextBox->SetFont( pFont9 );
	pExpTextBox->SetTextAlign( TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	pExpTextBox->SetLineInterval ( 5.0f );
	pExpTextBox->SetPartInterval ( 2.0f );
	RegisterControl( pExpTextBox );
	m_pExpTextBox = pExpTextBox;

	//보상아이템
	CBasicTextBoxEx* pRewardTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pRewardTextBox->CreateSub( this, "RNATTENDANCEPAGE_QUEST_SLOT_QUEST_ITEM" );
	pRewardTextBox->SetFont( pFont9 );
	pRewardTextBox->SetTextAlign( TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	pRewardTextBox->SetLineInterval ( 5.0f );
	pRewardTextBox->SetPartInterval ( 2.0f );
	RegisterControl( pRewardTextBox );
	m_pRewardTextBox = pRewardTextBox;

	//아이템 이미지
	CItemImageEx* pItemImage = new CItemImageEx( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub( this, "RNATTENDANCEPAGE_ITEM_IMAGE", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, ATTENDANCE_ITEM_IMAGE );
	pItemImage->CreateSubControl();
	RegisterControl( pItemImage );
	m_pItemImage = pItemImage;

	//보상받기 
	{
		RnButton* pRewardButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont9;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = RNATTENDANCE_QUEST_SLOT_REWARD_BUTTON;

		pRewardButton = new RnButton(m_pEngineDevice);
		pRewardButton->CreateSub(this, "RNATTENDANCEPAGE_REWARD_BUTTON", UI_FLAG_RIGHT, ATTENDANCE_REWARD_BUTTON );
		pRewardButton->CreateSubControl(arg);
		RegisterControl(pRewardButton);
		pRewardButton->SetVisibleSingle(FALSE);
		m_pRewardButton = pRewardButton;
	}

	//선택 
	{
		RnButton* pSelectButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont9;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = RNATTENDANCE_QUEST_SLOT_SELECT_BUTTON;

		pSelectButton = new RnButton(m_pEngineDevice);
		pSelectButton->CreateSub(this, "RNATTENDANCEPAGE_SELECT_BUTTON", UI_FLAG_RIGHT, ATTENDANCD_SELECT_QUEST_BUTON );
		pSelectButton->CreateSubControl(arg);
		RegisterControl(pSelectButton);
		pSelectButton->SetVisibleSingle(FALSE);
		m_pSelectButton = pSelectButton;
	}



}

void RnAttendanceQuestSlot::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}

void RnAttendanceQuestSlot::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch (ControlID)
	{
	case ATTENDANCE_ITEM_IMAGE:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				//마우스 커서를 업했을때 해당 라인의 아이템 정보 표시
				if( m_pItemImage )
				{
					SNATIVEID sItemImage = m_pItemImage->GetItemID();

					BOOL bChecked = TRUE;

					m_pInterface->SHOW_ITEM_INFO(
						SITEMCUSTOM( sItemImage ),
						FALSE, FALSE, FALSE, TRUE, bChecked,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;

	case ATTENDANCE_REWARD_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if (m_dwQuestId != UINT_MAX)
					m_pGaeaClient->GetCharacter()->ReqQuestComplete( m_dwQuestId );
			}
		}
		break;

	case  ATTENDANCD_SELECT_QUEST_BUTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if (m_dwQuestId != UINT_MAX)
				{
					m_pGaeaClient->GetCharacter()->ReqAttendanceQuestStart( m_dwQuestId );
					m_pGaeaClient->ClearAttendanceQuestIdServerToClient();
				}
			}
		}

	}
}

void RnAttendanceQuestSlot::SetInfoText( const CString strCourse, const D3DCOLOR& CourseTextColor /*= NS_UITEXTCOLOR::WHITE */,
										const CString strTitle, const D3DCOLOR& TitleTextColor /*= NS_UITEXTCOLOR::WHITE */,
										const DWORD dwMoney, const D3DCOLOR& MoneyTextColor  /*= NS_UITEXTCOLOR::WHITE */,
										const LONGLONG lnExp, const D3DCOLOR& ExpTextColor  /*= NS_UITEXTCOLOR::WHITE */,
										const CString strRewardItem, const D3DCOLOR& RewardItemTextColor /*= NS_UITEXTCOLOR::WHITE */,
										DWORD sItemID )
{
	//과제 넘버
	m_pCourseTextBox->ClearText();
	m_pCourseTextBox->AddText( strCourse, CourseTextColor );

	//과제 이름
	m_pQuestNameTextBox->ClearText();
	m_pQuestNameTextBox->AddText( strTitle, TitleTextColor );


	if (dwMoney <= 0)
	{
		m_pMoneyTextBox->SetVisibleSingle(FALSE);
	}
	else
	{
		//란머니
		CString strMoneyNumber = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( dwMoney, 3, "," );
		CString strMoney;
		strMoney.Format("%s : %s", RNATTENDANCE_QUEST_SLOT_MONEY,strMoneyNumber );
		m_pMoneyTextBox->SetVisibleSingle(TRUE);
		m_pMoneyTextBox->ClearText();
		m_pMoneyTextBox->AddText( strMoney, TitleTextColor );
	}


	//경험치
	if (lnExp <= 0)
	{
		m_pExpTextBox->SetVisibleSingle(FALSE);
	}
	else
	{
		m_pRewardTextBox->SetVisibleSingle(TRUE);
		m_pExpTextBox->SetVisibleSingle(TRUE);
		CString strExp;
		strExp.Format ( "%s : %I64d", ID2GAMEWORD("QUEST_GIFT",0), lnExp );
		m_pExpTextBox->ClearText();
		m_pExpTextBox->AddText( strExp, TitleTextColor );
	}


	

	SNATIVEID sId(sItemID);

	const SITEM* pItemData = GLogicData::GetInstance().GetItem( sId );
	if ( !pItemData )
	{
		m_pRewardTextBox->SetVisibleSingle(FALSE);
		m_pItemImage->SetVisibleSingle(FALSE);
		return;
	}
	else
	{
		//보상아이템
		m_pRewardTextBox->ClearText();
		m_pRewardTextBox->AddText( strRewardItem, RewardItemTextColor );
		m_pItemImage->ResetItem();
		m_pRewardTextBox->SetVisibleSingle(TRUE);
		m_pItemImage->SetVisibleSingle(TRUE);
		m_pItemImage->SetItem( pItemData->sBasicOp.sNativeID );
	}
}

void RnAttendanceQuestSlot::ClearSlot()
{
	m_pCourseTextBox->ClearText();
	m_pQuestNameTextBox->ClearText();
	m_pRewardTextBox->ClearText();
	m_pItemImage->ResetItem();
}

void RnAttendanceQuestSlot::SetVisibleSingleReWardButton( bool bVisible )
{
	m_pRewardButton->SetVisibleSingle( bVisible );
}

void RnAttendanceQuestSlot::SetVisibleSingleSelectButton( bool bVisible )
{
	m_pSelectButton->SetVisibleSingle( bVisible );
}

void RnAttendanceQuestSlot::SetQuestId( DWORD dwQuestId )
{
	m_dwQuestId = dwQuestId;
}


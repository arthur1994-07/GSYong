#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../InnerInterface.h"
#include "../../../Util/CheckBox.h"
#include "../../../Util/RnButton.h"
#include "./ExpeditionInfoPartySlot/ExpeditionInfoGroup.h"
#include "ExpeditionInfoTap.h"

#include "../../../../../RanLogic/Market/MarketDefine.h"
#include "../../../Util/SmartComboBoxEx.h"

void ExpeditionInfoTap::CreateSubControl_ExpeditionInfoGroup(void)
{ // 파티 그룹;
	CString strPartyInfoSlot[MAX_PARTYGROUP] = 
	{
		"EXPEDITION_INFO_GROUP_REGION0",
		"EXPEDITION_INFO_GROUP_REGION1",
		"EXPEDITION_INFO_GROUP_REGION2",
		"EXPEDITION_INFO_GROUP_REGION3",
		"EXPEDITION_INFO_GROUP_REGION4",
	};

	CreateLineBox( "EXPEDITION_INFO_GROUP_REGION5", "PARTY_INFO_LINEBOX_EXPEDITION_MARK" );

	for ( unsigned int _i = 0; _i < MAX_PARTYGROUP; _i++ )
	{
		m_pExpeditionInfoGroup[_i] = new ExpeditionInfoGroup(m_pEngineDevice);
		m_pExpeditionInfoGroup[_i]->CreateSub( this, strPartyInfoSlot[_i].GetString (), UI_FLAG_DEFAULT, EXPEDITION_INFO_SLOT0 + _i );
		m_pExpeditionInfoGroup[_i]->CreateSubControl();
		RegisterControl(m_pExpeditionInfoGroup[_i]);			
	}

	m_pExpeditionPopup = popupmenu::SP_POPUP_MENU(new popupmenu::PopupMenuComposite("", false));

    const int iWordIndexArray[POPUP_MENU_NSIZE] = { 12, 7, 8, 9, 11, 1, 2, 3, 4, 5, 6 };    //팝업메뉴 순서랑 텍스트인덱스가 맞지않아서 사용

    for ( int i = 0; i < POPUP_MENU_NSIZE; ++i )
    {
        m_pExpeditionPopupSub[i] = popupmenu::CreatePopupMenuItem(
            std::string(ID2GAMEWORD("PARTY_RENEWAL_POPUP_MENU_TEXT", iWordIndexArray[i] ) ), 
            boost::bind( &ExpeditionInfoTap::TranslateSlotPopupCallback, this, _1 ));

        m_pExpeditionPopup->addComponent( m_pExpeditionPopupSub[i] );
    }

	m_nDelegateID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("EXPEDITION_DELEGATE_TEXT")), boost::bind(&ExpeditionInfoTap::TranslateDelegateCallback, this, _1));
	m_nExpulsionID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("EXPEDITION_EXPULSION_TEXT")), boost::bind(&ExpeditionInfoTap::TranslateExpulsionCallback, this, _1));
	m_nReleasePartyID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("EXPEDITION_RELEASE_EXPEDITION_TEXT")), boost::bind(&ExpeditionInfoTap::TranslateReleaseExpeditionCallback, this, _1));
	m_nSecedeID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("EXPEDITION_SECEDE_TEXT")), boost::bind(&ExpeditionInfoTap::TranslateSecedeCallback, this, _1));	
	m_nNominateID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("EXPEDITION_NOMINATE_TEXT")), boost::bind(&ExpeditionInfoTap::TranslateNominateCallback, this, _1));		
}	

void ExpeditionInfoTap::CreateSubControl_Button(void)
{
	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
	
	{ // 원정대 해산 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_DISBAND_EXPEDITION);

		m_pReleaseExpeditionButton = new RnButton(m_pEngineDevice);
		m_pReleaseExpeditionButton->CreateSub(this,"EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON", UI_FLAG_DEFAULT, EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON);
		m_pReleaseExpeditionButton->CreateSubControl(Arg);
		RegisterControl(m_pReleaseExpeditionButton);
		m_pReleaseExpeditionButton->SetVisibleSingle(FALSE);
	}

	{ // 원정대 구성 버튼;				
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_CONSTRUCT_EXPEDITION);

		m_pConstructExpeditionButton = new RnButton(m_pEngineDevice);
		m_pConstructExpeditionButton->CreateSub(this,"EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON", UI_FLAG_DEFAULT, EXPEDITION_INFO_FORM_EXPEDITION_BUTTON);
		m_pConstructExpeditionButton->CreateSubControl(Arg);		
		RegisterControl(m_pConstructExpeditionButton);
		m_pConstructExpeditionButton->SetVisibleSingle(TRUE);
	}

	{ // 원정대 탈퇴 버튼;
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_SECEDE);

		m_pSecedeExpeditionButton = new RnButton(m_pEngineDevice);
		m_pSecedeExpeditionButton->CreateSub(this,"EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON", UI_FLAG_DEFAULT, EXPEDITION_INFO_SECEDE_EXPEDITION_BUTTON);
		m_pSecedeExpeditionButton->CreateSubControl(Arg);			
		RegisterControl(m_pSecedeExpeditionButton);
		m_pSecedeExpeditionButton->SetVisibleSingle(FALSE);
	}

	{ // 원정대 초대 버튼;		
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_INVITE);

		m_pInviteButton = new RnButton(m_pEngineDevice);
		m_pInviteButton->CreateSub(this,"EXPEDITION_INFO_INVITE_BUTTON", UI_FLAG_DEFAULT, EXPEDITION_INFO_INVITE_BUTTON);
		m_pInviteButton->CreateSubControl(Arg);
		RegisterControl(m_pInviteButton);
	}
}

void ExpeditionInfoTap::CreateSubControl_Text(void)
{ // 텍스트;
	CBasicTextBox* pTextBox = NULL;
	const int nTextAlign = TEXT_ALIGN_LEFT;
	D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	pTextBox = CreateStaticControl( "PARTY_INFO_ITEM_STATIC", pFont, dwFontColor, nTextAlign );
	pTextBox->AddText( (char*)ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_ITEM ) );

	m_pTextBoxItemFilter = CreateStaticControl( "PARTY_INFO_ITEM_FILTER_STATIC", pFont, dwFontColor, nTextAlign );
	m_pTextBoxItemFilter->AddText( (char*)ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_ITEM_FILTER ) );
	m_pTextBoxItemFilter->SetVisibleSingle( FALSE );

	pTextBox = CreateStaticControl( "PARTY_INFO_MONEY_STATIC", pFont, dwFontColor, nTextAlign );
	pTextBox->AddText( (char*)ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_MONEY ) );	
}

void ExpeditionInfoTap::CreateSubControl_ComboBox(void)
{	
    { // 아이템/게임머니 분배 방식, 구성학원생 콤보 박스;
		m_pComboBoxItem = new CBasicComboBox(m_pEngineDevice);
		m_pComboBoxItem->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, EXPEDITION_INFO_ITEM_COMBO_OPEN );
		m_pComboBoxItem->CreateBaseComboBox( "PARTY_INFO_ITEM_COMBO_OPEN" );	
		m_pComboBoxItem->SetVisibleSingle( TRUE );
		m_pComboBoxItem->SetText((char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", 1));
		RegisterControl(m_pComboBoxItem);

		m_pComboBoxItemFilter = new CBasicComboBox(m_pEngineDevice);
		m_pComboBoxItemFilter->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, EXPEDITION_INFO_ITEM_FILTER_COMBO_OPEN );
		m_pComboBoxItemFilter->CreateBaseComboBox( "PARTY_INFO_ITEM_FILTER_COMBO_OPEN" );	
		m_pComboBoxItemFilter->SetVisibleSingle( FALSE );
		m_pComboBoxItemFilter->SetText((char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", 0));
		RegisterControl(m_pComboBoxItemFilter);

		m_pComboBoxMoney = new CBasicComboBox(m_pEngineDevice);
		m_pComboBoxMoney->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, EXPEDITION_INFO_MONEY_COMBO_OPEN );
		m_pComboBoxMoney->CreateBaseComboBox( "PARTY_INFO_MONEY_COMBO_OPEN" );
		m_pComboBoxMoney->SetVisibleSingle( TRUE );
		m_pComboBoxMoney->SetText((char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", 1));
		RegisterControl(m_pComboBoxMoney);
	}

    // CAUTION!
    // 롤오버 메뉴들은 소스코드 맨아래 위치하게 해야 렌더링시 뒤쪽에위치하는 일이 없다.
    {   // 아이템 분배 방식 롤오버 메뉴
        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, EXPEDITION_INFO_ITEM_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "PARTY_INFO_ITEM_COMBO_ROLLOVER");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        pComboBoxRollOver->SetVisibleThumb( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollOverItem = pComboBoxRollOver;

        for(int nIndex=1; nIndex < EMPARTY_ITEM_NSIZE; nIndex++)
        {
            std::string strTemp = sc::string::format( " %s ", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", nIndex ) );
            pComboBoxRollOver->AddText( strTemp.c_str() );
        }
    }
    {   // 아이템 분배 필터 롤오버 메뉴
        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, EXPEDITION_INFO_ITEM_FILTER_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "PARTY_INFO_ITEM_FILTER_COMBO_ROLLOVER");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        pComboBoxRollOver->SetVisibleThumb( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollOverItemFilter = pComboBoxRollOver;

        for(int nIndex=1; nIndex < EMPARTY_ITEM_FILTER_NSIZE; nIndex++)
        {
            std::string strTemp = sc::string::format( " %s ", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", nIndex ) );
            pComboBoxRollOver->AddText( strTemp.c_str() );
        }
    }
    {   // 게임머니 분배 롤오버 메뉴
        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, EXPEDITION_INFO_MONEY_COMBO_ROLLOVER );
        pComboBoxRollOver->CreateBaseComboBoxRollOver( "PARTY_INFO_MONEY_COMBO_ROLLOVER");
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        pComboBoxRollOver->SetVisibleThumb( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pComboRollOverMoney = pComboBoxRollOver;

        for(int nIndex=1; nIndex < EMPARTY_MONEY_NSIZE; nIndex++)
        {
            std::string strTemp = sc::string::format( " %s ", ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", nIndex ) );
            pComboBoxRollOver->AddText( strTemp.c_str() );
        }
    }
}
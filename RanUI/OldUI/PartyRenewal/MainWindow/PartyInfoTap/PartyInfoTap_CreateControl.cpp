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
#include "./PartyInfoSlot/PartyInfoSlot.h"
#include "PartyInfoTap.h"

void PartyInfoTap::CreateSubControl_PartyInfoSlot(void)
{
	CString strPartyInfoSlot[MAXPARTY] = 
	{
		"PARTY_INFO_SLOT0",
		"PARTY_INFO_SLOT1",
		"PARTY_INFO_SLOT2",
		"PARTY_INFO_SLOT3",
		"PARTY_INFO_SLOT4",
		"PARTY_INFO_SLOT5",
		"PARTY_INFO_SLOT6",
		"PARTY_INFO_SLOT7",
	};

	for ( unsigned int _i = 0; _i < MAXPARTY; _i++ )
	{
		PartyInfoSlot* pPartySlot = new PartyInfoSlot(m_pEngineDevice, m_pInterface, m_pGaeaClient, _i);
		pPartySlot->CreateSub( this, strPartyInfoSlot[_i].GetString (), UI_FLAG_DEFAULT, PARTY_INFO_SLOT0 + _i );
		pPartySlot->CreateSubControl();		
		RegisterControl( pPartySlot );
		m_pMemberSlot[_i] = pPartySlot;
	}

	m_pSlotPopup = popupmenu::SP_POPUP_MENU(new popupmenu::PopupMenuComposite("", false));
	for ( DWORD _i = 0; _i < POPUP_MENU_NSIZE; _i++ )
    {
        m_pSlotPopupSub[_i] = popupmenu::CreatePopupMenuItem(
                                std::string(ID2GAMEWORD("PARTY_RENEWAL_POPUP_MENU_TEXT", _i) ), 
                                boost::bind( &PartyInfoTap::TranslateSlotPopupCallback, this, _1 ) );

		m_pSlotPopup->addComponent( m_pSlotPopupSub[_i] );
    }

	m_nDelegateID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("PARTY_DELEGATE_TEXT")), boost::bind(&PartyInfoTap::TranslateDelegateCallback, this, _1));
	m_nExpulsionID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("PARTY_EXPULSION_TEXT")), boost::bind(&PartyInfoTap::TranslateExpulsionCallback, this, _1));
	m_nReleasePartyID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("PARTY_RELEASE_PARTY_TEXT")), boost::bind(&PartyInfoTap::TranslateReleasePartyCallback, this, _1));
	m_nSecedeID = m_pInterface->AddPartyYesNoComponent(std::string(ID2GAMEINTEXT("PARTY_SECEDE_TEXT")), boost::bind(&PartyInfoTap::TranslateSecedeCallback, this, _1));	
}

void PartyInfoTap::CreateSubControl_Button(void){
	RnButton::CreateArg Arg;
	Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
	Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
	Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
	{ // 파티 해산 버튼;		
//		Arg.inactiveTextureKey =
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_DISBAND_PARTY);

		m_pReleasePartyButton = new RnButton(m_pEngineDevice);
		m_pReleasePartyButton->CreateSub(this,"PARTY_INFO_RELEASE_PARTY_BUTTON", UI_FLAG_DEFAULT, PARTY_INFO_RELEASE_PARTY_BUTTON);
		m_pReleasePartyButton->CreateSubControl(Arg);
		RegisterControl(m_pReleasePartyButton);
	}
	{ // 추방 버튼;		
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_EXPULSION);

		m_pExpulsionButton = new RnButton(m_pEngineDevice);
		m_pExpulsionButton->CreateSub(this,"PARTY_INFO_BAN_BUTTON", UI_FLAG_DEFAULT, PARTY_INFO_EXPULSION_BUTTON);
		m_pExpulsionButton->CreateSubControl(Arg);
		RegisterControl(m_pExpulsionButton);
	}
	{ // 위임 버튼;		
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_MANDATE);

		m_pDelegateButton = new RnButton(m_pEngineDevice);
		m_pDelegateButton->CreateSub(this,"PARTY_INFO_DELEGATE_BUTTON", UI_FLAG_DEFAULT, PARTY_INFO_DELEGATE_BUTTON);
		m_pDelegateButton->CreateSubControl(Arg);
		RegisterControl(m_pDelegateButton);
	}	
	{ // 탈퇴 버튼;		
		Arg.text = ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_SECEDE);

		m_pSecedeButton = new RnButton(m_pEngineDevice);
		m_pSecedeButton->CreateSub(this,"PARTY_INFO_WITHDRAW_BUTTON", UI_FLAG_DEFAULT, PARTY_INFO_SECEDE_BUTTON);
		m_pSecedeButton->CreateSubControl(Arg);		
		RegisterControl(m_pSecedeButton);
	}
}

void PartyInfoTap::CreateSubControl_CheckBox(void){
	{ // 버프 보기 체크 박스;
		CreateText((char*)ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_SHOWBUFF ),"PARTY_INFO_BUFF_CHECK_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

		m_pBuffCheckBox = new CCheckBox( m_pEngineDevice ); 
		m_pBuffCheckBox->CreateSub( this, "PARTY_INFO_BUFF_CHECK_BOX" , UI_FLAG_DEFAULT , PARTY_INFO_BUFF_CHECK );
		m_pBuffCheckBox->CreateSubControl ( "PARTY_INFO_BUFF_CHECK_BOX_ON", "PARTY_INFO_BUFF_CHECK_BOX_OFF" );
		m_pBuffCheckBox->SetCheck( FALSE );
		m_pBuffCheckBox->SetVisibleSingle(TRUE);
		RegisterControl( this->m_pBuffCheckBox );
	}

	{ // 파티원 정보 보기 체크 박스
		CreateText((char*)ID2GAMEWORD( "PARTY_RENEWAL_INFO_TEXT", EMPARTY_RENEWAL_WORD_SHOWINFO ),"PARTY_INFO_CHECK_TEXT", TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

		m_pInfoCheckBox = new CCheckBox( m_pEngineDevice ); 
		m_pInfoCheckBox->CreateSub( this, "PARTY_INFO_CHECK_BOX" , UI_FLAG_DEFAULT , PARTY_INFO_INFO_CHECK );
		m_pInfoCheckBox->CreateSubControl ( "PARTY_INFO_BUFF_CHECK_BOX_ON", "PARTY_INFO_BUFF_CHECK_BOX_OFF" );
		m_pInfoCheckBox->SetCheck( TRUE );
		m_pInfoCheckBox->SetVisibleSingle(TRUE);
		RegisterControl( this->m_pInfoCheckBox );
	}
}

void PartyInfoTap::CreateSubControl_ComboBox(void){
	{ // 아이템, 게임머니, 필터 텍스트;
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
    { // 아이템/게임머니/필터 분배 방식 콤보 박스;
        m_pComboBoxItem = new CBasicComboBox(m_pEngineDevice);
        m_pComboBoxItem->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_INFO_ITEM_COMBO_OPEN );
        m_pComboBoxItem->CreateBaseComboBox( "PARTY_INFO_ITEM_COMBO_OPEN" );	
        m_pComboBoxItem->SetVisibleSingle( TRUE );
        m_pComboBoxItem->SetText((char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", 1));
        RegisterControl( m_pComboBoxItem );

        m_pComboBoxItemFilter = new CBasicComboBox(m_pEngineDevice);
        m_pComboBoxItemFilter->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_INFO_ITEM_FILTER_COMBO_OPEN );
        m_pComboBoxItemFilter->CreateBaseComboBox( "PARTY_INFO_ITEM_FILTER_COMBO_OPEN" );	
        m_pComboBoxItemFilter->SetVisibleSingle( FALSE );
        m_pComboBoxItemFilter->SetText((char*)ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", 0));
        RegisterControl( m_pComboBoxItemFilter );

        m_pComboBoxMoney = new CBasicComboBox(m_pEngineDevice);
        m_pComboBoxMoney->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, PARTY_INFO_MONEY_COMBO_OPEN );
        m_pComboBoxMoney->CreateBaseComboBox( "PARTY_INFO_MONEY_COMBO_OPEN" );
        m_pComboBoxMoney->SetVisibleSingle( TRUE );
        m_pComboBoxMoney->SetText((char*)ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", 1));
        RegisterControl( m_pComboBoxMoney );
    }

    // CAUTION!
    // 롤오버 메뉴들은 소스코드 맨아래 위치하게 해야 렌더링시 뒤쪽에위치하는 일이 없다.
    {   // 아이템 분배 방식 롤오버 메뉴
        CBasicComboBoxRollOver* pComboBoxRollOver;
        pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_INFO_ITEM_COMBO_ROLLOVER );
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
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_INFO_ITEM_FILTER_COMBO_ROLLOVER );
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
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PARTY_INFO_MONEY_COMBO_ROLLOVER );
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

void PartyInfoTap::CreateSubControl_TextBox()
{
    { // 파티 속해있지 않을 때 중앙 텍스트박스
        const int nTextAlign = TEXT_ALIGN_CENTER_X;
        D3DCOLOR dwFontColor = NS_UITEXTCOLOR::DEFAULT;
        CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        m_pNonPartyText = CreateStaticControl( "PARTY_NONPARTY_TEXT", pFont, dwFontColor, nTextAlign );
        m_pNonPartyText->AddText( ID2GAMEINTEXT( "PARTY_RENEWAL_NONPARTY_TEXT" ) );
    }
}
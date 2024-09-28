#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../EngineLib/DeviceMan.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../InnerInterface.h"
#include "../../../Util/CheckBox.h"
#include "../../../Util/RnButton.h"
#include "./PartyInfoSlot/PartyInfoSlot.h"
#include "PartyInfoTap.h"

PartyInfoTap::PartyInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_dwSelectedSlot(DEFSLOT_NULL)
, m_bLockedClick(false)
{	
}

PartyInfoTap::~PartyInfoTap(void)
{	
}

void PartyInfoTap::CreateSubControl(void)
{
	CreateLineBox( "PARTY_INFO_OPTION_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "PARTY_INFO_MEMBER_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	CreateSubControl_PartyInfoSlot();   //파티원슬롯
	CreateSubControl_Button();  //버튼들
	CreateSubControl_CheckBox();    //체크박스
	CreateSubControl_ComboBox();	//콤보박스
    CreateSubControl_TextBox();    //텍스트박스
}

void PartyInfoTap::TranslateUIMessage_ViewSlotPopUp(const DWORD dwSlotIndex)
{
	m_dwSelectedSlot = dwSlotIndex;
	m_pInterface->OpenPopupMenu(m_pSlotPopup);	
}

void PartyInfoTap::TranslateUIMessage_Whisper(void)
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const CString& szName = m_pMemberSlot[m_dwSelectedSlot]->GetName();

	m_pInterface->GetBasicChatWindow()->BeginPrivateChat(UI::ToString(szName));
	m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX(szName);
}
void PartyInfoTap::TranslateUIMessage_ViewInfo(void)
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const CString& szName = m_pMemberSlot[m_dwSelectedSlot]->GetName();
	m_pInterface->ReferCharWindowOpen(szName.GetString());
}

void PartyInfoTap::TranslateUIMessage_AddFriend(void)
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const CString& szName = m_pMemberSlot[m_dwSelectedSlot]->GetName();
	m_pInterface->ADD_FRIEND(szName);
}

void PartyInfoTap::TranslateUIMessage_BlockFriend(const bool bBlock)
{
	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const CString& szName = m_pMemberSlot[m_dwSelectedSlot]->GetName();
	m_pInterface->ADD_FRIEND_BLOCK(szName, bBlock);
}

void PartyInfoTap::TranslateSlotPopupCallback(int nIndex)  // CALLBACK;
{
	switch ( EMPOPUP_MENU(nIndex) )
	{	
	case POPUP_MENU_DELEGATE:
		if ( m_pGaeaClient->GetPartyManager()->isPartyMaster() == false )
			break;
		m_pInterface->ToggleWindowYesNoParty(m_nDelegateID);
		break;
	case POPUP_MENU_WHISPER:
		TranslateUIMessage_Whisper();
		break;
	case POPUP_MENU_VIEW_INFO:
		TranslateUIMessage_ViewInfo();
		break;
	case POPUP_MENU_ADD_FRIEND:
		TranslateUIMessage_AddFriend();
		break;
	case POPUP_MENU_BLOCK:
		TranslateUIMessage_BlockFriend(true);
		break;
	case POPUP_MENU_BLOCK_DELETE:
		TranslateUIMessage_BlockFriend(false);
		break;
	case POPUP_MENU_EXPULSION:
		if ( m_pGaeaClient->GetPartyManager()->isPartyMaster() == false )
			break;
		m_pInterface->ToggleWindowYesNoParty(m_nExpulsionID);
		break;
	}
}

void PartyInfoTap::TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex)
{	
	if ( m_dwSelectedSlot == dwSlotIndex )
		m_dwSelectedSlot = DEFSLOT_NULL;
	else
		m_dwSelectedSlot = dwSlotIndex;
}

void PartyInfoTap::TranslateUIMessage_ItemCombo(void)
{	
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();	
	GLPartyClient* pParty = pPartyManager->GetMyParty();
	if ( pParty->GetPartyID().isValidExpedition() == true )
		return;

	if ( pPartyManager->isPartyMaster() == false )
		return; 

    if( m_pComboRollOverItem->IsVisible() == TRUE )
        m_pComboRollOverItem->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverItem->SetVisibleSingle( TRUE );
}
void PartyInfoTap::TranslateUIMessage_ItemFilterCombo(void)
{	
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();	
	GLPartyClient* pParty = pPartyManager->GetMyParty();
	if ( pParty->GetPartyID().isValidExpedition() == true )
		return;

	if ( pPartyManager->isPartyMaster() == false )
		return; 

    if( m_pComboRollOverItemFilter->IsVisible() == TRUE )
        m_pComboRollOverItemFilter->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverItemFilter->SetVisibleSingle( TRUE );
}
void PartyInfoTap::TranslateUIMessage_MoneyCombo(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	GLPartyClient* pParty = pPartyManager->GetMyParty();	
	if ( pParty->GetPartyID().isValidExpedition() == true )
		return;

	if ( pPartyManager->isPartyMaster() == false )
		return; 

    if( m_pComboRollOverMoney->IsVisible() == TRUE )
        m_pComboRollOverMoney->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverMoney->SetVisibleSingle( TRUE );
}

void PartyInfoTap::TranslateUIMessage_ItemComboRollOver(void)
{
    int nIndex = m_pComboRollOverItem->GetSelectIndex();
    if ( nIndex < 0 )
        return;
    //실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (기여자 우선 방식 없음);
    m_PartyOption.SetItemOption(EMPARTY_ITEM_OPTION(nIndex+1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void PartyInfoTap::TranslateUIMessage_ItemFilterComboRollOver(void)
{
    int nIndex = m_pComboRollOverItemFilter->GetSelectIndex();
    if ( nIndex < 0 )
        return;
	//실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (전체 없음);
    m_PartyOption.SetItemFilter(EMPARTY_ITEM_FILTER_OPTION(nIndex + 1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void PartyInfoTap::TranslateUIMessage_MoneyComboRollOver(void)
{
    int nIndex = m_pComboRollOverMoney->GetSelectIndex();
    if ( nIndex < 0 )
        return;
    //실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (기여자 우선 방식 없음);
    m_PartyOption.SetMoneyOption(EMPARTY_MONEY_OPTION(nIndex+1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void PartyInfoTap::TranslateUIMessage_BuffCheck(void)
{
	const BOOL bChecked = m_pBuffCheckBox->IsChecked();
	m_pInterface->SetVisibleSkillEffectPartyStateWindow(bChecked);
}

void PartyInfoTap::TranslateUIMessage_InfoCheck(void)
{
	const BOOL bChecked = m_pInfoCheckBox->IsChecked();
	if( bChecked )
	{
		m_pInterface->UiShowGroupBottom( PARTY_STATE_WINDOW );
	}
	else
	{
		m_pInterface->UiHideGroup( PARTY_STATE_WINDOW );
	}
}

void PartyInfoTap::SetPartyOption(const SPartyOption& PartyOption)
{	
	const DWORD dwItemOption = DWORD(PartyOption.GetItemOption());
	const DWORD dwMoneyOption = PartyOption.GetMoneyOption();
	
	CString strItemOption;
	switch ( dwItemOption )
	{
	case EMPARTY_ITEM_LEADER_SHARE:
	case EMPARTY_ITEM_DICE:
		{
			CString strItemFilter;
			const DWORD dwItemFilter = DWORD(PartyOption.GetItemFilter());			

			if( m_pComboBoxItem )
			{
				strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
				m_pComboBoxItem->SetText( strItemOption );			
			}

			if( m_pComboBoxItemFilter && m_pTextBoxItemFilter )
			{
				strItemFilter.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
				m_pComboBoxItemFilter->SetText( strItemFilter );
				m_pComboBoxItemFilter->SetVisibleSingle(TRUE);
				m_pTextBoxItemFilter->SetVisibleSingle(TRUE);
			}

			strItemOption.Format("%s(%s)", 
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption),
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
		}
		break;
	default:
		if( m_pComboBoxItem )
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
			m_pComboBoxItem->SetText(strItemOption);
		}

		if( m_pComboBoxItemFilter && m_pTextBoxItemFilter )
		{
			m_pComboBoxItemFilter->SetVisibleSingle(FALSE);
			m_pTextBoxItemFilter->SetVisibleSingle(FALSE);
		}
	}

	CString strMoneyOption;
	strMoneyOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", dwMoneyOption));

	m_pComboBoxMoney->SetText( strMoneyOption );
	m_PartyOption = PartyOption;

	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
		sc::string::format(ID2GAMEINTEXT("PARTY_OPTION_CHANGE"), strItemOption.GetString(), strMoneyOption.GetString()));	
}

void PartyInfoTap::TranslateSecedeCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	m_pGaeaClient->GetPartyManager()->Secede(dwMyGaeaID, static_cast<BYTE>(PARTY_ACTION_SECEDE_TYPE));
}

void PartyInfoTap::TranslateDelegateCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const DWORD dwNewMasterGaeaID = m_pMemberSlot[m_dwSelectedSlot]->GetGaeaID();
	m_pGaeaClient->GetPartyManager()->Authority(dwNewMasterGaeaID);
}
void PartyInfoTap::TranslateExpulsionCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	if ( m_dwSelectedSlot == DEFSLOT_NULL )
		return;

	const DWORD dwTargetGaeaID = m_pMemberSlot[m_dwSelectedSlot]->GetGaeaID();
	m_pGaeaClient->GetPartyManager()->Secede(dwTargetGaeaID, static_cast<BYTE>(PARTY_ACTION_BAN_TYPE));
}
void PartyInfoTap::TranslateReleasePartyCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	m_pGaeaClient->GetPartyManager()->Dissolve();
}


void PartyInfoTap::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{	
	const GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	switch ( ControlID )
	{
	case PARTY_INFO_SLOT0:
	case PARTY_INFO_SLOT1:
	case PARTY_INFO_SLOT2:
	case PARTY_INFO_SLOT3:
	case PARTY_INFO_SLOT4:
	case PARTY_INFO_SLOT5:
	case PARTY_INFO_SLOT6:
	case PARTY_INFO_SLOT7:
		{
            //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
            if ( m_bLockedClick )
                return;

			const DWORD dwSlotIndex = DWORD(ControlID - PARTY_INFO_SLOT0);
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				TranslateUIMessage_SelectPartySlot(dwSlotIndex);
			else if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
				TranslateUIMessage_ViewSlotPopUp(dwSlotIndex);
		}
		break;

	case PARTY_INFO_SECEDE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			m_pInterface->ToggleWindowYesNoParty(m_nSecedeID);
		}
		break;	

	case PARTY_INFO_DELEGATE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( pPartyManager->isPartyMaster() == false )
				break;
			m_pInterface->ToggleWindowYesNoParty(m_nDelegateID);
		}
		break;

	case PARTY_INFO_ITEM_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
        if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
			TranslateUIMessage_ItemCombo();
		break;

    case PARTY_INFO_ITEM_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {
                TranslateUIMessage_ItemComboRollOver();
            }
            m_pComboRollOverItem->SetVisibleSingle(FALSE);
        }
        break;

	case PARTY_INFO_ITEM_FILTER_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
        if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
			TranslateUIMessage_ItemFilterCombo();
		break;

    case PARTY_INFO_ITEM_FILTER_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {
                TranslateUIMessage_ItemFilterComboRollOver();
            }
            m_pComboRollOverItemFilter->SetVisibleSingle(FALSE);
        }
        break;

	case PARTY_INFO_MONEY_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
        if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
			TranslateUIMessage_MoneyCombo();
		break;

    case PARTY_INFO_MONEY_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {
                TranslateUIMessage_MoneyComboRollOver();
            }
            m_pComboRollOverMoney->SetVisibleSingle(FALSE);
        }
        break;

	case PARTY_INFO_EXPULSION_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( pPartyManager->isPartyMaster() == false )
				break;
			m_pInterface->ToggleWindowYesNoParty(m_nExpulsionID);
		}
		break;

	case PARTY_INFO_BUFF_CHECK:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_BuffCheck();
		break;

	case PARTY_INFO_INFO_CHECK:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_InfoCheck();
		break;

	case PARTY_INFO_RELEASE_PARTY_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( pPartyManager->isPartyMaster() == false )
				break;
			m_pInterface->ToggleWindowYesNoParty(m_nReleasePartyID);
		}
		break;
	}
}

void PartyInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pMemberSlot[_i - 1]->RESET();	

    GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
    if ( pPartyManager == NULL )
        return;

    const bool bParty = pPartyManager->isParty();   //파티인가?;
    const bool bPartyMaster = pPartyManager->isPartyMaster();   //파티 마스터인가?;
    const bool bExpedition = pPartyManager->isExpedition(); //원정대인가?;
    
    m_pNonPartyText->SetVisibleSingle(!bParty);  //파티가 아닐때 (속해있는 파티가 없습니다) 텍스트;
    m_pSecedeButton->SetVisibleSingle(bParty);  //탈퇴(파티나 원정대일때);

    const bool bOnlyPartyMaster = ( bPartyMaster && ( bExpedition ==  false ) );
    m_pReleasePartyButton->SetVisibleSingle(bOnlyPartyMaster); //해산(파티 마스터만 원정대X);
    m_pExpulsionButton->SetVisibleSingle(bOnlyPartyMaster);    //추방(파티 마스터만 원정대X);
    m_pDelegateButton->SetVisibleSingle(bOnlyPartyMaster); //위임(파티 마스터만 원정대X);

    m_pSlotPopupSub[POPUP_MENU_DELEGATE]->SetEnable( bOnlyPartyMaster );    //위임팝업메뉴(파티 마스터만 원정대X);
    m_pSlotPopupSub[POPUP_MENU_EXPULSION]->SetEnable( bOnlyPartyMaster );   //추방팝업메뉴(파티 마스터만 원정대X);

    if ( bParty == false )  //파티가 아니면 끝냄;
        return;

    GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
    if ( pMyParty == NULL )
        return;

    unsigned int dwSlotIndexUI = 0;		
    GLPartySlotClient* const pMaster = pMyParty->GetMember(MASTER_PARTY); //마스터;

    m_pMemberSlot[dwSlotIndexUI++]->SetPartyInfoMasterSlot(pMaster, pMyParty->GetPartyIndex() == MASTER_PARTY);
	for ( DWORD _i = 1; _i < MAXPARTY; ++_i )
	{		
		GLPartySlotClient* const pPartySlot = pMyParty->GetMember(_i);		
		if ( pPartySlot == NULL )
			continue;		

		m_pMemberSlot[dwSlotIndexUI++]->SetPartyInfoSlot(pPartySlot);
	}	

	if ( m_dwSelectedSlot != DEFSLOT_NULL )
		m_pMemberSlot[m_dwSelectedSlot]->SetSelect();

    if ( m_pComboRollOverItem->IsVisible() || m_pComboRollOverItemFilter->IsVisible() || m_pComboRollOverMoney->IsVisible() )
    {
        m_bLockedClick = true;
    }
    else
    {
        m_bLockedClick = false;
    }

}

void PartyInfoTap::SetVisibleSingle(BOOL bVisible)
{
    CUIGroupHelper::SetVisibleSingle(bVisible);	

    if ( bVisible == FALSE )
    {
        m_pInterface->UiHideGroup(POPUP_MENU);
    }
}
#include "StdAfx.h"

#include "../../../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../InnerInterface.h"
#include "../../../Util/CheckBox.h"
#include "../../../Util/RnButton.h"
#include "./ExpeditionInfoPartySlot/ExpeditionInfoGroup.h"
#include "ExpeditionInfoTap.h"

ExpeditionInfoTap::ExpeditionInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_dwSelectedPartySlot(DEFSLOT_NULL)
, m_bChangeMasterChatState( false )
, m_bLockedClick(false)
{
}

void ExpeditionInfoTap::CreateSubControl(void)
{
	CreateLineBox( "PARTY_INFO_OPTION_REGION", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	CreateSubControl_ExpeditionInfoGroup();
	CreateSubControl_Button();
	CreateSubControl_Text();
	CreateSubControl_ComboBox();	
}

void ExpeditionInfoTap::SetPartyOption(const SPartyOption& PartyOption)
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

	m_pComboBoxMoney->SetText(strMoneyOption);

	m_PartyOption = PartyOption;	
}

void ExpeditionInfoTap::TranslateUIMessage_SelectPartySlot(const DWORD dwSlotIndex)
{
	if ( m_dwSelectedPartySlot == dwSlotIndex )		
		return;

	if ( m_dwSelectedPartySlot != DEFSLOT_NULL )
		m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->ResetSelect();

	m_dwSelectedPartySlot = dwSlotIndex;
}
void ExpeditionInfoTap::TranslateUIMessage_ViewPopUp(const DWORD dwSlotIndex)
{
	m_SwapSlotState.Reset();
	m_dwSelectedPartySlot = dwSlotIndex;
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	m_dwSelectedMemberSlotByPopup = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetSelectedSlot();
	if ( m_dwSelectedMemberSlotByPopup == DEFSLOT_NULL )
		return;

	m_pInterface->OpenPopupMenu(m_pExpeditionPopup);
}

void ExpeditionInfoTap::TranslateUIMessage_SwapSlot(void) // 자리 이동;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	if ( m_dwSelectedMemberSlotByPopup == DEFSLOT_NULL )
		return;

	m_SwapSlotState.SetSwapSlotFrom(
		m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetSelectedGaeaID(m_dwSelectedMemberSlotByPopup),
		m_dwSelectedPartySlot);	
}

void ExpeditionInfoTap::TranslateUIMessage_Whisper(void) // 귓속말;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	const CString* szName = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetName(m_dwSelectedMemberSlotByPopup);
	if ( szName == NULL )
		return;

	m_pInterface->GetBasicChatWindow()->BeginPrivateChat(UI::ToString(*szName));
	m_pInterface->ADD_FRIEND_NAME_TO_EDITBOX(*szName);
}
void ExpeditionInfoTap::TranslateUIMessage_ViewInfo(void) // 정보 보기;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	const CString* szName = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetName(m_dwSelectedMemberSlotByPopup);
	if ( szName == NULL )
		return;

	m_pInterface->ReferCharWindowOpen(szName->GetString());
}
void ExpeditionInfoTap::TranslateUIMessage_AddFriend(void) // 친구 등록;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	const CString* szName = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetName(m_dwSelectedMemberSlotByPopup);
	if ( szName == NULL )
		return;

	m_pInterface->ADD_FRIEND(*szName);
}
void ExpeditionInfoTap::TranslateUIMessage_BlockFriend(const bool bBlock) // 차단/차단 해제;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	const CString* szName = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetName(m_dwSelectedMemberSlotByPopup);
	if ( szName == NULL )
		return;

	m_pInterface->ADD_FRIEND_BLOCK(*szName, bBlock);
}

void ExpeditionInfoTap::TranslateDelegateCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	if ( m_dwSelectedPartySlotByPopup == DEFSLOT_NULL )
		return;
	
	m_pGaeaClient->GetPartyManager()->Authority(m_dwSelectedGaeaIDByPopup, m_dwSelectedPartySlotByPopup, true);
}
void ExpeditionInfoTap::TranslateExpulsionCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	if ( m_dwSelectedGaeaIDByPopup == GAEAID_NULL )
		return;
	
	m_pGaeaClient->GetPartyManager()->Secede(m_dwSelectedGaeaIDByPopup);
}
void ExpeditionInfoTap::TranslateReleaseExpeditionCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	m_pGaeaClient->GetPartyManager()->Dissolve();
}
void ExpeditionInfoTap::TranslateSecedeCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	m_pGaeaClient->GetPartyManager()->Secede(dwMyGaeaID );
}
void ExpeditionInfoTap::TranslateNominateCallback(const bool bYes)
{
	if ( bYes == false )
		return;

	if ( m_dwSelectedPartySlotByPopup == DEFSLOT_NULL )
		return;

	GLSwapSlotInformation NewSubMasterSlotInfo;

	NewSubMasterSlotInfo.dwGaeaID = m_dwSelectedGaeaIDByPopup;
	NewSubMasterSlotInfo.nPartyIndex = m_dwSelectedPartySlotByPopup;

	m_pGaeaClient->GetPartyManager()->Nominate(NewSubMasterSlotInfo);	
}

void ExpeditionInfoTap::TranslateSlotPopupCallback(int nIndex)  // CALLBACK;
{
	if ( m_dwSelectedPartySlot == DEFSLOT_NULL )
		return;

	const DWORD dwGaeaID = m_pExpeditionInfoGroup[m_dwSelectedPartySlot]->GetSelectedGaeaID(m_dwSelectedMemberSlotByPopup);
	if ( dwGaeaID == GAEAID_NULL )
	{
		m_dwSelectedPartySlot = DEFSLOT_NULL;
		m_dwSelectedPartySlotByPopup = DEFSLOT_NULL;
		return;
	}

	m_dwSelectedPartySlotByPopup = m_dwSelectedPartySlot;
	m_dwSelectedGaeaIDByPopup = dwGaeaID;
	const GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	switch ( EMPOPUP_MENU(nIndex) )
	{
	case POPUP_MENU_SWAP_SLOT:
		TranslateUIMessage_SwapSlot();		
		break;	
	case POPUP_MENU_CHECK_READY:
		m_pGaeaClient->GetPartyManager()->CheckReadyRequest();
		break;
	case POPUP_MENU_MASTER_CHAT:
		m_pGaeaClient->GetPartyManager()->MasterChatting();
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
	case POPUP_MENU_DELEGATE:
		if ( pPartyManager->isExpeditionMaster() == false )
			break;
		m_pInterface->ToggleWindowYesNoParty(m_nDelegateID);		
		break;
	case POPUP_MENU_NOMINATE:
		if ( pPartyManager->isExpeditionMaster() == false )
			break;		
		m_pInterface->ToggleWindowYesNoParty(m_nNominateID);		
		break;
	case POPUP_MENU_EXPULSION:
		if ( pPartyManager->isExpeditionSubMaster() == false )
			break;
		m_pInterface->ToggleWindowYesNoParty(m_nExpulsionID);		
		break;
	}
}

void ExpeditionInfoTap::TranslateUIMessage_ItemCombo(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();	
	GLPartyClient* pParty = pPartyManager->GetMyParty();
	if ( pParty->GetPartyID().isValidExpedition() == false )
		return;

	if ( pPartyManager->isExpeditionMaster() == false )
		return; 

    if( m_pComboRollOverItem->IsVisible() == TRUE )
        m_pComboRollOverItem->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverItem->SetVisibleSingle( TRUE );
}

void ExpeditionInfoTap::TranslateUIMessage_ItemComboRollOver(void)
{
    int nIndex = m_pComboRollOverItem->GetSelectIndex();
    if ( nIndex < 0 )
        return;
    //실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (기여자 우선 방식 없음);
    m_PartyOption.SetItemOption(EMPARTY_ITEM_OPTION(nIndex+1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void ExpeditionInfoTap::TranslateUIMessage_ItemFilterCombo(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();	
	GLPartyClient* pParty = pPartyManager->GetMyParty();
	if ( pParty->GetPartyID().isValidExpedition() == false )
		return;

	if ( pPartyManager->isExpeditionMaster() == false )
		return; 

    if( m_pComboRollOverItemFilter->IsVisible() == TRUE )
        m_pComboRollOverItemFilter->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverItemFilter->SetVisibleSingle( TRUE );
}

void ExpeditionInfoTap::TranslateUIMessage_ItemFilterComboRollOver(void)
{
    int nIndex = m_pComboRollOverItemFilter->GetSelectIndex();
    if ( nIndex < 0 )
        return;
	//실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (전체 없음);
    m_PartyOption.SetItemFilter(EMPARTY_ITEM_FILTER_OPTION(nIndex + 1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void ExpeditionInfoTap::TranslateUIMessage_MoneyCombo(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	GLPartyClient* pParty = pPartyManager->GetMyParty();	
	if ( pParty->GetPartyID().isValidExpedition() == false )
		return;

	if ( pPartyManager->isExpeditionMaster() == false )
		return; 

    if( m_pComboRollOverMoney->IsVisible() == TRUE )
        m_pComboRollOverMoney->SetVisibleSingle( FALSE );
    else
        m_pComboRollOverMoney->SetVisibleSingle( TRUE );
}

void ExpeditionInfoTap::TranslateUIMessage_MoneyComboRollOver(void)
{
    int nIndex = m_pComboRollOverMoney->GetSelectIndex();
    if ( nIndex < 0 )
        return;
    //실제옵션인덱스와 롤오버 메뉴인덱스가 맞지않음 1 차이 남; (기여자 우선 방식 없음);
    m_PartyOption.SetMoneyOption(EMPARTY_MONEY_OPTION(nIndex+1));
    m_pGaeaClient->GetPartyManager()->ChangeOption(m_PartyOption);
}

void ExpeditionInfoTap::TranslateUIMessage_Invite(void)
{
	if ( m_pGaeaClient->GetPartyManager()->GetMyParty()->isMaster() == false )
		return;

	m_pInterface->ToggleWindowExpeditionInvite();
}


void ExpeditionInfoTap::TranslateUIMessage_FormExpedition(void)
{
	m_pGaeaClient->GetPartyManager()->Expedition();
}

void ExpeditionInfoTap::TranslateUIMessage_SwapSlotSelect(const DWORD dwSlotIndex)
{
	GLSwapSlotInformation SwapSlotTo;
	SwapSlotTo.dwGaeaID = m_pExpeditionInfoGroup[dwSlotIndex]->GetSelectedGaeaID();
	SwapSlotTo.nPartyIndex = dwSlotIndex;

	m_pGaeaClient->GetPartyManager()->SwapSlot(m_SwapSlotState.SwapSlotFrom, SwapSlotTo);

	m_SwapSlotState.Reset();
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		m_pExpeditionInfoGroup[_i - 1]->ResetSelect();
}

void ExpeditionInfoTap::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
    const GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	switch ( ControlID )
	{
	case EXPEDITION_INFO_SLOT0:
	case EXPEDITION_INFO_SLOT1:
	case EXPEDITION_INFO_SLOT2:
	case EXPEDITION_INFO_SLOT3:
	case EXPEDITION_INFO_SLOT4:
		{
            //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
            if ( m_bLockedClick )
                return;
			const DWORD dwSlotIndex = DWORD(ControlID - EXPEDITION_INFO_SLOT0);
			if ( m_SwapSlotState.isSwapSlotMode() )
			{
				if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
					TranslateUIMessage_SwapSlotSelect(dwSlotIndex);
			}
			else
			{
				if ( CHECK_MOUSE_IN(dwMsg) )
					TranslateUIMessage_SelectPartySlot(dwSlotIndex);
			}

			if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
				TranslateUIMessage_ViewPopUp(dwSlotIndex);
		}		
		break;

	case EXPEDITION_INFO_ITEM_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
        if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
            TranslateUIMessage_ItemCombo();
		break;

    case EXPEDITION_INFO_ITEM_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {
                TranslateUIMessage_ItemComboRollOver();
            }
            m_pComboRollOverItem->SetVisibleSingle(FALSE);
        }
        break;

	case EXPEDITION_INFO_ITEM_FILTER_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
            TranslateUIMessage_ItemFilterCombo();
		break;

    case EXPEDITION_INFO_ITEM_FILTER_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {
                m_bLockedClick = true;
                TranslateUIMessage_ItemFilterComboRollOver();
            }
            m_pComboRollOverItemFilter->SetVisibleSingle(FALSE);
        }
        break;

	case EXPEDITION_INFO_MONEY_COMBO_OPEN:
        //분배 방식 롤오버 메뉴 오픈으로 인한 중복클릭 방지 (모달과 유사하게)
        if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) && !m_bLockedClick )
            TranslateUIMessage_MoneyCombo();
		break;

    case EXPEDITION_INFO_MONEY_COMBO_ROLLOVER:
        if ( CHECK_LB_UP_LIKE(dwMsg) && m_bLockedClick )
        {
            if( CHECK_MOUSE_IN(dwMsg) )
            {	
                m_bLockedClick = true;
                TranslateUIMessage_MoneyComboRollOver();
            }
            m_pComboRollOverMoney->SetVisibleSingle(FALSE);
        }
        break;

	case EXPEDITION_INFO_INVITE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_Invite();
		break;
	case EXPEDITION_INFO_RELEASE_EXPEDITION_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( pPartyManager->isExpeditionMaster() == false )
				break;
			m_pInterface->ToggleWindowYesNoParty(m_nReleasePartyID);
		}
		break;
	case EXPEDITION_INFO_FORM_EXPEDITION_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			TranslateUIMessage_FormExpedition();
		break;
	case EXPEDITION_INFO_SECEDE_EXPEDITION_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
		{
			if ( pPartyManager->isExpeditionMaster() == true )
				break;
			m_pInterface->ToggleWindowYesNoParty(m_nSecedeID);
		}
		break;		
	}
}

void ExpeditionInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ){
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		m_pExpeditionInfoGroup[_i - 1]->RESET();

    GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
    
    if ( pPartyManager == NULL )
        return;

    const bool bPartyMaster = pPartyManager->isPartyMaster();
    const bool bExpedition = pPartyManager->isExpedition();
    const bool bExpeditionMaster = pPartyManager->isExpeditionMaster();
    const bool bExpeditionSubMaster = pPartyManager->isExpeditionSubMaster();

    if ( bExpedition == false )  //원정대가 아닐때
    {
        m_pInviteButton->SetVisibleSingle(FALSE);   //초대불가
        m_pSecedeExpeditionButton->SetVisibleSingle(FALSE); //탈퇴불가
        m_pReleaseExpeditionButton->SetVisibleSingle(FALSE);    //해산불가
        m_pConstructExpeditionButton->SetVisibleSingle(bPartyMaster);  //구성(파티장일때 가능)
        return;
    }

    m_pConstructExpeditionButton->SetVisibleSingle(FALSE);   //구성불가(원정대일 때)

    if ( bExpeditionMaster )
    {
        m_pSecedeExpeditionButton->SetVisibleSingle(FALSE); //탈퇴불가
        m_pReleaseExpeditionButton->SetVisibleSingle(TRUE);    //해산가능
        m_pInviteButton->SetVisibleSingle(TRUE);   //초대가능

        //마스터 기능 활성화
        m_pExpeditionPopupSub[POPUP_MENU_SWAP_SLOT]->SetEnable( TRUE );
        m_pExpeditionPopupSub[POPUP_MENU_DELEGATE]->SetEnable( TRUE );
        m_pExpeditionPopupSub[POPUP_MENU_NOMINATE]->SetEnable( TRUE );
        m_pExpeditionPopupSub[POPUP_MENU_CHECK_READY]->SetEnable( TRUE );
        m_pExpeditionPopupSub[POPUP_MENU_MASTER_CHAT]->SetEnable( TRUE );
        m_pExpeditionPopupSub[POPUP_MENU_EXPULSION]->SetEnable( TRUE );

    }
    else
    {
        m_pSecedeExpeditionButton->SetVisibleSingle(TRUE); //탈퇴가능
        m_pReleaseExpeditionButton->SetVisibleSingle(FALSE);    //해산불가

        m_pInviteButton->SetVisibleSingle(bExpeditionSubMaster);   //초대가능(부 마스터만)

        //마스터 단독기능 비활성화
        m_pExpeditionPopupSub[POPUP_MENU_SWAP_SLOT]->SetEnable( FALSE );
        m_pExpeditionPopupSub[POPUP_MENU_DELEGATE]->SetEnable( FALSE );
        m_pExpeditionPopupSub[POPUP_MENU_NOMINATE]->SetEnable( FALSE );
        m_pExpeditionPopupSub[POPUP_MENU_CHECK_READY]->SetEnable( FALSE );
        m_pExpeditionPopupSub[POPUP_MENU_EXPULSION]->SetEnable( FALSE );

        //부마스터 기능 (비)활성화
        m_pExpeditionPopupSub[POPUP_MENU_MASTER_CHAT]->SetEnable( bExpeditionSubMaster );
    }

    const bool bMasterChat = pPartyManager->isMasterChatting();
    if ( m_bChangeMasterChatState != bMasterChat )  //마스터 채팅 상태가 변경 되었을 때
    {
        if ( bMasterChat )
        {
            m_pExpeditionPopupSub[POPUP_MENU_MASTER_CHAT]->SetDescription( std::string(ID2GAMEWORD("PARTY_RENEWAL_POPUP_MENU_TEXT", 10 ) ) );
        }
        else
        {
            m_pExpeditionPopupSub[POPUP_MENU_MASTER_CHAT]->SetDescription( std::string(ID2GAMEWORD("PARTY_RENEWAL_POPUP_MENU_TEXT", 11 ) ) );
        }
        m_bChangeMasterChatState = bMasterChat;
    }

	m_SwapSlotState.fTimer += fElapsedTime;	
	if ( m_SwapSlotState.fTimer > GLCONST_CHAR::fSwapSlotBlankInterval )
	{
		m_SwapSlotState.bBlank = !m_SwapSlotState.bBlank;
		m_SwapSlotState.fTimer = 0.0f;
	}	

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		GLPartyClient* const pParty = pPartyManager->GetParty(_i - 1);
		m_pExpeditionInfoGroup[_i - 1]->SetExpeditionSlot(pParty, m_SwapSlotState.bSwapSlotMode, m_SwapSlotState.bBlank);
	}

    //롤오버 메뉴가 열러있을때는 중복 클릭문제를 막기위해 클릭 플래그를 on
    if ( m_pComboRollOverItem->IsVisible() || m_pComboRollOverItemFilter->IsVisible()  || m_pComboRollOverMoney->IsVisible() )
    {
        m_bLockedClick = true;
    }
    else
    {
        m_bLockedClick = false;
    }

}
void ExpeditionInfoTap::SetVisibleSingle(BOOL bVisible)
{
	CUIGroupHelper::SetVisibleSingle(bVisible);	
	m_SwapSlotState.Reset();

	if ( bVisible == FALSE )
    {
		m_pInterface->CloseExpeditionInviteWindow();
        m_pInterface->UiHideGroup(POPUP_MENU);
    }
}
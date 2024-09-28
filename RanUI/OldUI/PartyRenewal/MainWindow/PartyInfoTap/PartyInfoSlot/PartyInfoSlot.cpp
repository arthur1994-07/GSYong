#include "StdAfx.h"

#include "../../../../../../EngineLib/GUInterface/UIControl.h"
#include "../../../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../../../RanLogicClient/ClientActor.h"
#include "../../../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../InnerInterface.h"
#include "../../../../Util/RnButton.h"

#include "PartyInfoSlotInformation.h"
#include "PartyInfoSlotPortrait.h"
#include "PartyInfoSlot.h"

PartyInfoSlot::PartyInfoSlot(EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface, GLGaeaClient* pGaeaClient, const DWORD dwPartySlot) :
CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_dwPartySlot(dwPartySlot)
{
	m_pMasterCrown[0] = NULL;
	m_pMasterCrown[1] = NULL;
}

PartyInfoSlot::~PartyInfoSlot(void)
{
}

void PartyInfoSlot::CreateSubControl(void)
{
	CreateLineBox( "PARTY_INFO_SLOT_NUM", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

	if ( m_dwPartySlot == MASTER_PARTY )
	{
		m_pMasterCrown[0] = CreateControl( "PARTY_INFO_SLOT_MASTER", PARTY_INFO_SLOT_MASTER );
		m_pMasterCrown[1] = CreateControl( "PARTY_INFO_SLOT_SUB_MASTER", PARTY_INFO_SLOT_SUBMASTER );
	}
	else
	{
		CString strSlotNum;
		strSlotNum.Format("%d", m_dwPartySlot);
		CBasicTextBox* pSlotNum;
		pSlotNum = CreateText(strSlotNum.GetString(), "PARTY_INFO_SLOT_NUM_TEXT", TEXT_ALIGN_CENTER_X|TEXT_ALIGN_CENTER_Y);
	}

	{ // 직업 초상화;
		m_pPartySlotPortrait = new PartyInfoSlotPortrait(m_pEngineDevice);
		m_pPartySlotPortrait->CreateSub( this, "PARTY_INFO_SLOT_PORTRAIT", UI_FLAG_DEFAULT, PARTY_INFO_SLOT_PORTRAIT);
		m_pPartySlotPortrait->CreateSubControl();
		RegisterControl(m_pPartySlotPortrait);
	}

	{ // 케릭터 정보; ( 학원, 클럽, 레벨, 이름등 );
		m_pPartySlotInformation = new PartyInfoSlotInformation(m_pEngineDevice, m_pGaeaClient);
		m_pPartySlotInformation->CreateSub( this, "PARTY_INFO_SLOT_TEXT", UI_FLAG_DEFAULT, PARTY_INFO_SLOT_INFORMATION);
		m_pPartySlotInformation->CreateSubControl();
		RegisterControl(m_pPartySlotInformation);
	}
	m_pSelectBox = CreateLineBox("PARTY_INFO_SLOT_SELECT", "PARTY_INFO_SELECTBOX_TEXTUREINFO");

	{ // 친구 추가 / 정보 보기 버튼;
		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_SLOT_ADD_FRIEND_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_SLOT_ADD_FRIEND_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_SLOT_ADD_FRIEND_BUTTON_CLICK";
		{ // 친구 추가 버튼;
			m_pAddFriend = new RnButton(m_pEngineDevice);
			m_pAddFriend->CreateSub(this,"PARTY_INFO_SLOT_BTN0", UI_FLAG_DEFAULT, PARTY_INFO_SLOT_ADD_FRIEND);
			m_pAddFriend->CreateSubControl(Arg);
			RegisterControl(m_pAddFriend);
		}

		Arg.defaultTextureKey = "PARTY_SLOT_SHOW_INFO_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_SLOT_SHOW_INFO_OVER";
		Arg.mouseClickTextureKey = "PARTY_SLOT_SHOW_INFO_CLICK";
		{ // 정보 보기 버튼;
			m_pShowInfo = new RnButton(m_pEngineDevice);
			m_pShowInfo->CreateSub(this,"PARTY_INFO_SLOT_BTN1", UI_FLAG_DEFAULT, PARTY_INFO_SLOT_SHOW_INFO);
			m_pShowInfo->CreateSubControl(Arg);
			RegisterControl(m_pShowInfo);
		}
	}
	m_pBlockFriend = CreateControl( "PARTY_SLOT_BLOCK_FRIEND", PARTY_INFO_SLOT_BLOCK_FRIEND );
	m_pBlockFriend->SetVisibleSingle(FALSE);
}


void PartyInfoSlot::SetPartyInfoSlot(const GLPartySlotClient* const pPartyClient)
{	
	const GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter == NULL )
		return;

	const DWORD dwMyDbNum = pCharacter->GetCharID();
	if ( dwMyDbNum == pPartyClient->m_dwDbNum )
	{
		m_pBlockFriend->SetVisibleSingle(FALSE);
		m_pAddFriend->SetVisibleSingle(FALSE);
	}
	else
	{
		EMFRIEND_FLAGS emFriendState;
		m_pGaeaClient->GetFriendState(dwMyDbNum, pPartyClient->m_dwDbNum, emFriendState);

		switch ( emFriendState )
		{
		case EMFRIEND_BLOCK:
			m_pBlockFriend->SetVisibleSingle(TRUE);
			m_pAddFriend->SetVisibleSingle(FALSE);
			break;
		case EMFRIEND_VALID:
			m_pBlockFriend->SetVisibleSingle(FALSE);
			m_pAddFriend->SetVisibleSingle(FALSE);
			break;
		default:
			m_pBlockFriend->SetVisibleSingle(FALSE);
			m_pAddFriend->SetVisibleSingle(TRUE);
			break;
		}		
	}

	GLPartyManagerClient* const pPartyManager(m_pGaeaClient->GetPartyManager());
	if ( pPartyManager == NULL )
		return;

	GLPartyClient* const pMyParty(pPartyManager->GetMyParty());
	if ( pMyParty == NULL )
		return;

	GLPartySlotClient* const pMyPartySlot(pMyParty->GetMemberByGaeaID(pCharacter->GetGaeaID()));
	if ( pMyPartySlot == NULL )
		return;
	
	const bool bAvailable(pMyPartySlot->m_sMapID == pPartyClient->m_sMapID);

	m_pPartySlotPortrait->SetPortrait(pPartyClient, bAvailable); // 직업에 따른 초상화 설정;
	m_pPartySlotInformation->SetInformation(pPartyClient, bAvailable);  // 파티원 정보 설정;	

	SetVisibleSingle(TRUE);
}

void PartyInfoSlot::SetPartyInfoMasterSlot(const GLPartySlotClient* const pPartyClient, const bool bMaster)
{	
	m_pMasterCrown[0]->SetVisibleSingle(bMaster ? TRUE : FALSE);
	m_pMasterCrown[1]->SetVisibleSingle(bMaster ? FALSE : TRUE);

	SetPartyInfoSlot(pPartyClient);
}

void PartyInfoSlot::SetSelect(void)
{
	if ( IsVisible() )
	{
		m_pSelectBox->SetVisibleSingle(TRUE);
		m_pPartySlotInformation->SetColor(0xffffff00);
	}
}

const DWORD PartyInfoSlot::GetGaeaID(void) const
{
	return m_pPartySlotInformation->GetGaeaID();
}

const CString& PartyInfoSlot::GetName(void) const
{
	return m_pPartySlotInformation->GetName();
}

void PartyInfoSlot::RESET(void)
{	
	m_pSelectBox->SetVisibleSingle(FALSE);	
	m_pPartySlotInformation->RESET();
	SetVisibleSingle(FALSE);
}

void PartyInfoSlot::TranslateUIMessage_AddFriend(void)
{
	const CString& szName = m_pPartySlotInformation->GetName();
	m_pInterface->ADD_FRIEND(szName);
}
void PartyInfoSlot::TranslateUIMessage_ShowInfo(void)
{
	const CString& szName = m_pPartySlotInformation->GetName();
	m_pInterface->ReferCharWindowOpen(szName.GetString());
}

void PartyInfoSlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case PARTY_INFO_SLOT_ADD_FRIEND:
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                ID2GAMEWORD("PARTY_RENEWAL_ICON_TOOLTIP", 0 ),
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            if ( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                TranslateUIMessage_AddFriend();
            }
        }
		break;
    case PARTY_INFO_SLOT_BLOCK_FRIEND:
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                ID2GAMEWORD("PARTY_RENEWAL_ICON_TOOLTIP", 1 ),
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
        break;
	case PARTY_INFO_SLOT_SHOW_INFO:
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                ID2GAMEWORD("PARTY_RENEWAL_ICON_TOOLTIP", 2 ),
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            if ( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                TranslateUIMessage_ShowInfo();
            }
        }
		break;
    case PARTY_INFO_SLOT_MASTER:
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                ID2GAMEWORD("PARTY_RENEWAL_ICON_TOOLTIP", 3 ),
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
        break;
    case PARTY_INFO_SLOT_SUBMASTER:
        if ( CHECK_MOUSE_IN( dwMsg ) )
        {
            m_pInterface->SHOW_COMMON_LINEINFO(
                ID2GAMEWORD("PARTY_RENEWAL_ICON_TOOLTIP", 4 ),
                D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
                TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        }
        break;
	}
}
#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"

#include "./MiniExpeditionInfoGroup/MiniExpeditionInfoGroup.h"
#include "MiniExpeditionWindow.h"

MiniExpeditionWindow::MiniExpeditionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_dwTapPageIndex(0)
{
}

MiniExpeditionWindow::~MiniExpeditionWindow(void)
{
}

void MiniExpeditionWindow::EventSelectedTap(UIGUID controlID)
{
	switch ( controlID )
	{
	case EXPEDITION_TAP_1:
	case EXPEDITION_TAP_2:
	case EXPEDITION_TAP_3:
	case EXPEDITION_TAP_4:
	case EXPEDITION_TAP_5:
		m_dwTapPageIndex = controlID - EXPEDITION_TAP_1;
		break;
	}
}

void MiniExpeditionWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void MiniExpeditionWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	GLPartyManagerClient* pPartyManager = m_pGaeaClient->GetPartyManager();
	if ( pPartyManager->isParty() == false )
		return;
	
	GLPartyClient* const pParty = pPartyManager->GetParty(m_dwTapPageIndex);
	m_pExpeditionInfoGroup->SetExpeditionSlot(pParty, m_dwTapPageIndex);
}

void MiniExpeditionWindow::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);
    if ( bVisible )
    {
        //��Ƽ���� ���������� �˻�
        const GLPartyClient* pParty = m_pGaeaClient->GetMyPartyClient();
        if ( !pParty )
        {
            return;
        }
        if ( pParty->GetPartyID().isValidExpedition() )
        {
            SetMiniWindowForExpedition();
        }
        else
        {
            SetMiniWindowForParty();
        }
    }
    else
	{
		m_pExpeditionInfoGroup->SetExpeditionSlot(NULL);
	}
}

void MiniExpeditionWindow::SetMiniWindowForParty()
{
    //������ �� ������
    for ( DWORD _i = 0; _i < MAX_PARTYGROUP; ++_i )
    {
        m_pExpeditionTap[_i]->SetVisibleSingle( FALSE );
    }
    SetTitleName( (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 0) ); //������ Ÿ��Ʋ ��Ƽ��
    m_dwTapPageIndex = 0;   //��Ƽ�� 0�� ���� ���
}

void MiniExpeditionWindow::SetMiniWindowForExpedition()
{
    //������ �� ���
    for ( DWORD _i = 0; _i < MAX_PARTYGROUP; ++_i )
    {
        m_pExpeditionTap[_i]->SetVisibleSingle( TRUE );
    }
    SetTitleName( (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 1) );    //������ Ÿ��Ʋ �������
}

void MiniExpeditionWindow::CreateSubControl(void)
{
    CString strExpeditionInfoTap[MAX_PARTYGROUP] = 
	{
		"MINI_EXPEDITION_INFO_TAP_1",
		"MINI_EXPEDITION_INFO_TAP_2",
		"MINI_EXPEDITION_INFO_TAP_3",
		"MINI_EXPEDITION_INFO_TAP_4",
		"MINI_EXPEDITION_INFO_TAP_5",
	};

	for ( DWORD _i = 0; _i < MAX_PARTYGROUP; ++_i )
	{
		CTapSelectorEx::BUTTON* pExpeditionTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
		pExpeditionTap->CreateSub( this, strExpeditionInfoTap[_i].GetString (), UI_FLAG_DEFAULT, EXPEDITION_TAP_1 + _i );
		pExpeditionTap->CreateSubControl( (char*)ID2GAMEWORD("MINI_EXPEDITION_INFO_TAP", _i),
			"MINI_EXPEDITION_INFO_TAP_BASE","PARTY_TAP_ACTIVE",
			"MINI_EXPEDITION_INFO_TAP_BASE","PARTY_TAP_INACTIVE",
			"MINI_EXPEDITION_INFO_TAP_BASE");
		RegisterControl ( pExpeditionTap );
		RegisterTapButton( pExpeditionTap );
        m_pExpeditionTap[_i] = pExpeditionTap;
	}
	m_pExpeditionInfoGroup = new MiniExpeditionInfoGroup(m_pEngineDevice, m_pInterface, m_pGaeaClient);
	m_pExpeditionInfoGroup->CreateSub(this, "MINI_EXPEDITION_INFO_GROUP_REGION");
	m_pExpeditionInfoGroup->CreateSubControl();
	RegisterControl(m_pExpeditionInfoGroup);	
}

MyMiniExpeditionWindow::MyMiniExpeditionWindow(GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
MiniExpeditionWindow(pGaeaCleint, pInterface, pEngineDevice)
{	
}
void MyMiniExpeditionWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create( MINI_EXPEDITION_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateBaseWindowLightGray( "MINI_EXPEDITION_WINDOW", (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 1));
    CreateLineBox( "MINI_EXPEDITION_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	CreateSubControl();
	SetAlignFlag( UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
	m_pInterface->UiRegisterControl(this, true);
    m_pInterface->UiShowGroupFocus( MINI_EXPEDITION_WINDOW );
}

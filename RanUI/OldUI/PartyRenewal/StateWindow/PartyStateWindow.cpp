#include "StdAfx.h"

#include "../../../../EngineLib/GUInterface/TipBox.h"
#include "../../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "./MemberStateGroup/MemberStateGroup.h"
#include "./MemberStateGroup/MemberStateSlot/MemberStateSlot.h"
#include "PartyStateWindow.h"

PartyStateWindow::PartyStateWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_bDragging(false)
, m_bVisibleSkillEffect(false)
, m_fControlGapY(0.f)
{
}

void PartyStateWindow::CreateSubControl(void)
{
	m_pPartyStateGroup = new MemberStateGroup(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pPartyStateGroup->CreateSub(this, "PARTY_STATE_WINDOW_CONTROL", UI_FLAG_DEFAULT, ID_PARTY_STATE_GROUP);
	m_pPartyStateGroup->CreateSubControl();
	m_pPartyStateGroup->SetVisibleSingle(TRUE);
	RegisterControl(m_pPartyStateGroup);

	m_pControl = new CUIControl(m_pEngineDevice);
	m_pControl->CreateSub(this, "PARTY_STATE_WINDOW_CONTROL", UI_FLAG_DEFAULT, ID_PARTY_STATE_CONTROL);
	m_pControl->SetUseRender(FALSE);
	m_pControl->SetVisibleSingle(TRUE);
	RegisterControl(m_pControl);

	if( m_pControl )
		m_fControlGapY = m_pControl->GetLocalPos().top;
}

void PartyStateWindow::SetVisibleSkillEffect(const BOOL bShow)
{
	m_bVisibleSkillEffect = bShow;
	m_pPartyStateGroup->SetVisibleSkillEffect(bShow);
}

void PartyStateWindow::Resize(void)
{
	GLPartyManagerClient* const pPartyManager = m_pGaeaClient->GetPartyManager();
	const DWORD dwNumMember = pPartyManager->GetNMember();

	if ( dwNumMember > 1 ) //파티원이 2명이하일수 없다.
	{
		m_pPartyStateGroup->SetVisibleSkillEffect(m_bVisibleSkillEffect);

		//파티원상태창의 크기를 파이원 수, 버프보기 유무에 따라 변경시킨다.
		UIRECT Rect = m_pPartyStateGroup->GetGlobalPos();
		MemberStateSlot* pMember = m_pPartyStateGroup->m_pMemberSlot[0];

		Rect.sizeX = pMember->GetSlotPos().sizeX;
		if( m_bVisibleSkillEffect )
			Rect.sizeX += m_pPartyStateGroup->GetVisibleSkillEffectSizeX();

		float sizeY = ( m_UICfg.rcControlPos.sizeY * dwNumMember );
		Rect.sizeY = sizeY;
		Rect.right = Rect.left + Rect.sizeX;
		Rect.bottom = Rect.top + Rect.sizeY;
		m_pPartyStateGroup->SetGlobalPos( Rect );

		Rect = m_pControl->GetGlobalPos();
		Rect.sizeY = sizeY;
		Rect.bottom = Rect.top + Rect.sizeY;
		m_pControl->SetGlobalPos( Rect );
	}
}

void PartyStateWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ID_PARTY_STATE_CONTROL:
		if ( CHECK_MOUSE_IN(dwMsg) )
		{
			SetExclusiveControl();
			if ( dwMsg & UIMSG_LB_DOWN )
			{
				//SetExclusiveControl();
				m_bDragging = true;
			}
			else if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				//ResetExclusiveControl();
				m_bDragging = false;
			}
		}
		else if ( CHECK_LB_UP_LIKE( dwMsg) )
		{
			ResetExclusiveControl();			
			m_bDragging = false;
		}
	}	
}

void PartyStateWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	Resize();

	if( m_bDragging )
	{
		D3DXVECTOR2 vPos;
		vPos.x = m_vOldUIPosition.left + x - m_vOldMousePosition.x;
		vPos.y = m_vOldUIPosition.top + y - m_vOldMousePosition.y;
		SetGlobalPos( vPos );
	}
	else
	{
		bool bSetGlobalPos = false;
		m_vOldMousePosition.x = static_cast<float>(x);
		m_vOldMousePosition.y = static_cast<float>(y);
		m_vOldUIPosition = GetGlobalPos();

		const UIRECT& rect = m_pPartyStateGroup->m_pMemberSlot[0]->GetGlobalPos();
		if ( m_vOldUIPosition.left > rect.left )
		{
			m_vOldUIPosition.left = rect.left;
			bSetGlobalPos = true;
		}
		if ( m_vOldUIPosition.top > rect.top-m_fControlGapY )
		{
			m_vOldUIPosition.top = rect.top-m_fControlGapY;
			bSetGlobalPos = true;
		}

		if( bSetGlobalPos )
			SetGlobalPos( m_vOldUIPosition );	
    }

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

MyPartyStateWindow::MyPartyStateWindow(GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
PartyStateWindow(pGaeaCleint, pInterface, pEngineDevice)
{	
}

void MyPartyStateWindow::CreateUIWindowAndRegisterOwnership(void)
{
    // XML에서 파티원슬롯한명분의 창에대한 정보를 얻어온다.
    // 새로운 UIXML에서 찾지 못할경우 구형 UIXML에서 검색한다.
    if( !CUIConfigMgr::GetInstance().FindConfig( "PARTY_STATE_WINDOW", m_UICfg ) )
    {
        // 예전 UIXML에서 텍스처 검
        if( CInterfaceCfg::GetInstance().ArrangeInfo( "PARTY_STATE_WINDOW", m_UICfg ) == FALSE )
        {
            std::string strError = sc::string::format(" \"%s\" 컨트롤 Id를 찾지 못하였습니다.", "PARTY_STATE_WINDOW" );
            GASSERT_MSG( 0, strError );
            return;
        }
    }

 	Create( PARTY_STATE_WINDOW, "PARTY_STATE_WINDOW" );	
	CreateSubControl();
	m_pInterface->UiRegisterControl(this, true);
	m_pInterface->UiShowGroupBottom(PARTY_STATE_WINDOW);
}

void MyPartyStateWindow::SetVisibleSkillEffect(const BOOL bShow)
{
	PartyStateWindow::SetVisibleSkillEffect(bShow);
}
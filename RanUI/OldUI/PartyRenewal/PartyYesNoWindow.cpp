#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../Util/RnButton.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"

#include "PartyYesNoWindow.h"

PartyYesNoWindow::PartyYesNoWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pTextDescription(NULL)
, m_pButtonYes(NULL)
, m_pButtonNo(NULL)
{
}

PartyYesNoWindow::~PartyYesNoWindow(void)
{
	m_YesNoNodeVector.clear();
}

void PartyYesNoWindow::CreateSubControl(void)
{
	CreateLineBox("PARTY_YES_NO_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	//	CreateLineBox("EXPEDITION_CHECK_READY_WINDOW_CONTENT", "PARTY_INFO_LINEBOX_TEXTUREINFO");

	m_pTextDescription = CreateText(
		"(null)", 
		"PARTY_YES_NO_WINDOW_DISCRIPTION", 
		TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

	{ // 네/아니오 버튼;
		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
		{ // 네 버튼;
			Arg.text = ID2GAMEWORD( "PARTY_YES_NO_WINDOW_TEXT", 1);

			m_pButtonYes = new RnButton(m_pEngineDevice);
			m_pButtonYes->CreateSub(this,"PARTY_YES_NO_WINDOW_BUUTON_YES", UI_FLAG_DEFAULT, BUTTON_YES);
			m_pButtonYes->CreateSubControl(Arg);
			RegisterControl(m_pButtonYes);
		}

		{ // 아니오 버튼;		
			Arg.text = ID2GAMEWORD( "PARTY_YES_NO_WINDOW_TEXT", 2);

			m_pButtonNo = new RnButton(m_pEngineDevice);
			m_pButtonNo->CreateSub(this,"PARTY_YES_NO_WINDOW_BUTTON_NO", UI_FLAG_DEFAULT, BUTTON_NO);
			m_pButtonNo->CreateSubControl(Arg);
			RegisterControl(m_pButtonNo);
		}
	}	
}


void PartyYesNoWindow::Execute(const bool bYes)
{
	const DWORD nNode = m_YesNoNodeVector.size() - 1;
	if ( m_nID > nNode )
		return;

	const PartyYesNoNode& YesNoNode = m_YesNoNodeVector[m_nID];
	YesNoNode.m_Function(bYes);
	SetVisibleSingle(FALSE);
}

void PartyYesNoWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	// 토너먼트 사용중에는 파티를 승낙할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	if(IsVisible() == TRUE)
	{
		if( m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() == true)
		{
			Execute(false);
		}
	}


	SetFocusControl();
}

void PartyYesNoWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case BUTTON_YES:
	case BUTTON_NO:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			Execute(ControlID == BUTTON_YES ? true :false);
		break;
	}
}


/*
 * return nID;
*/
const DWORD PartyYesNoWindow::AddComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction)
{
	m_YesNoNodeVector.push_back(PartyYesNoNode(strDescription, PartyYesNoFunction));
	
	return m_YesNoNodeVector.size() - 1;
}

void PartyYesNoWindow::SetID(const DWORD nID)
{
	const DWORD nNode = m_YesNoNodeVector.size() - 1;
	if ( nID > nNode )
		return;

	const PartyYesNoNode& YesNoNode = m_YesNoNodeVector[nID];	

	m_pTextDescription->SetText(YesNoNode.m_strDescription.c_str());
	m_nID = nID;
}







MyPartyYesNoWindow::MyPartyYesNoWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: PartyYesNoWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyPartyYesNoWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create( PARTY_YES_NO_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CreateBaseWindowLightGray( "PARTY_YES_NO_WINDOW", (char*)ID2GAMEWORD( "PARTY_YES_NO_WINDOW_TEXT", 0) );		
	CreateSubControl();
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	m_pInterface->UiRegisterControl( this, true );
	m_pInterface->UiShowGroupFocus( PARTY_YES_NO_WINDOW );
}

/*
 * return nID;
*/
const DWORD MyPartyYesNoWindow::AddComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction)
{
	return PartyYesNoWindow::AddComponent(strDescription, PartyYesNoFunction);
}

void MyPartyYesNoWindow::SetID(const DWORD nID)
{
	PartyYesNoWindow::SetID(nID);
}


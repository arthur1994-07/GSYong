#include "StdAfx.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "./CompetitionWindow.h"
#include "./CompetitionObject.h"
#include "./CTFPage.h"
#include "./PVEPage.h"
#include "../Tournament/TournamentPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCompetitionWindow::CCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowObjectController(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pUIObject(NULL)
{
	memset(m_pPages, 0, sizeof(m_pPages));
}

CCompetitionWindow::~CCompetitionWindow()
{
	//Blank
}

void CCompetitionWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 윈도우 오브젝트
	{
		m_pUIObject = new CCompetitionObject    ( m_pInterface, m_pEngineDevice );
		m_pUIObject->Create					    ( COMPETITION_WINDOW_OBJECT, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pUIObject->CreateBaseWindowLightGray  ( "COMPETITION_OBJECT", (char*)ID2GAMEWORD("COMPETITION_WINDOW", 0) );
		m_pUIObject->CreateSubControl	        ( pd3dDevice );
		m_pUIObject->SetAlignFlag			    ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pUIObject->SetVisibleSingle		    ( FALSE );
		m_pUIObject->SetFlag                    ( CUIWindowObject::FLAG_TITLE_NAME_NONCHANGE );

		WindowRegister( m_pUIObject, COMPETITION_WINDOW_OBJECT );
	}

	int nIndex = 0;
	int nDefault = COMPETITION_PAGE_CTF;
	{
		// CaptureTheField
		if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
		{
			m_pPages[nIndex] = new CCTFPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			m_pPages[nIndex]->Create ( "COMPETITION_CTF_PAGE", NULL, COMPETITION_WINDOW_PAGE_CTF );
			PageRegister( m_pPages[nIndex] );
			++nIndex;
		}

        if ( GLUseFeatures::GetInstance().IsUsingInfinityStairs() )
        {
		    // PVE
		    m_pPages[nIndex] = new CPVEPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		    m_pPages[nIndex]->Create ( "COMPETITION_CTF_PAGE", NULL, COMPETITION_WINDOW_PAGE_PVE );
		    PageRegister( m_pPages[nIndex]);
		    ++nIndex;
        }
		
		if ( GLUseFeatures::GetInstance().IsUsingTournament() )
		{
			// Tournament
			m_pPages[nIndex] = new CTournamentPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			m_pPages[nIndex]->Create ( "COMPETITION_CTF_PAGE", NULL, COMPETITION_WINDOW_PAGE_TOURNAMENT );
			PageRegister( m_pPages[nIndex]);
			++nIndex;
		}


	}

	SetOpenWindow ( COMPETITION_WINDOW_OBJECT );
	SetOpenPage	  ( m_pPages[nDefault] );
}

void CCompetitionWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowObjectController::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CCompetitionWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	if (dwMsg & UIMSG_COMPETITION_OBJECT_TAP_CHANGE)
	{
		int nPage = m_pUIObject->GetSelectedTapID() - CCompetitionObject::TAP_CTF;

		if (nPage >= 0 && nPage < COMPETITION_PAGE_QUANTITY)
		{
			WindowPageOpen(CCompetitionWindow::COMPETITION_WINDOW_OBJECT, m_pPages[nPage]);
		}
	}

	CUIWindowObjectController::TranslateUIMessage(ControlID, dwMsg);
}

const bool CCompetitionWindow::IsOpen()
{
	return IsVisible() ? true : false;
}

void CCompetitionWindow::Open()
{
	if (IsVisible() == false)
	{		
		m_pInterface->UiShowGroupFocus( COMPETITION_WINDOW );
	}
}

void CCompetitionWindow::Close()
{
	if (IsVisible())
	{
		m_pInterface->UiHideGroup( COMPETITION_WINDOW );
	}
}

void CCompetitionWindow::Update(int nPage)
{
	if (nPage >= 0 && nPage < COMPETITION_PAGE_QUANTITY)
	{
		if(m_pPages[nPage])
			m_pPages[nPage]->Update();
	}
}


//----------------------------------------------------------------------------------------------------//

MyCompetitionWindow::MyCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CCompetitionWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyCompetitionWindow::CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CCompetitionWindow::Create( COMPETITION_WINDOW, "COMPETITION_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CCompetitionWindow::CreateSubControl( pd3dDevice );
	CCompetitionWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
	CCompetitionWindow::m_pInterface->UiRegisterControl( this, true );
	CCompetitionWindow::m_pInterface->UiShowGroupFocus( COMPETITION_WINDOW );
}

const bool MyCompetitionWindow::IsOpen()
{
	return CCompetitionWindow::IsOpen();
}

void MyCompetitionWindow::Update(int nPage)
{
	CCompetitionWindow::Update(nPage);
}

void MyCompetitionWindow::Open()
{
	CCompetitionWindow::Open();
}

void MyCompetitionWindow::Close()
{
	CCompetitionWindow::Close();
}

const UIRECT& MyCompetitionWindow::GetGlobalPos()
{
	return CCompetitionWindow::GetGlobalPos();
}


#include "StdAfx.h"
#include "BikeGetOffWindow.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBikeGetOffWindow::CBikeGetOffWindow(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pGetOffButton(NULL)
{
}

CBikeGetOffWindow::~CBikeGetOffWindow ()
{
}

void CBikeGetOffWindow::CreateSubControl ()
{
	m_pGetOffButton = CreateFlipButton( "BIKE_GET_OFF_BUTTON", "BIKE_GET_OFF_BUTTON_F", "PTOPMENU_BIKE_BUTTON_OVER", BIKE_GET_OFF_BUTTON );

	CBasicVarTextBox* pInfo = new CBasicVarTextBox(m_pEngineDevice);
	pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pInfo->CreateSubControl ();
	pInfo->SetVisibleSingle ( FALSE );
	RegisterControl ( pInfo );
	m_pInfo = pInfo;
	m_pInfo->SetTextNoSplit ( ID2GAMEINTEXT("BIKE_GET_OFF_MESSAGE"), NS_UITEXTCOLOR::SILVER );
}

CBasicButton* CBikeGetOffWindow::CreateFlipButton ( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( szButtonOver );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CBikeGetOffWindow::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}


void CBikeGetOffWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_pInfo->SetVisibleSingle( FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CBikeGetOffWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( cID, dwMsg );
	switch ( cID )
	{
	case BIKE_GET_OFF_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInfo->SetVisibleSingle ( TRUE );
				if (dwMsg & UIMSG_LB_UP)
				{
                    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
					if (pChar->VehicleState() ==  EMVEHICLE_DRIVER )
					{
						INT nNumber = pChar->VehicleePassengerNum();
						if( nNumber > 1 )
						{
							//운전자일때 유저 강퇴기능
							GLMSG::SNET_GET_OFF_WITH_VEHICLE NetMsg;
							NetMsg.dwGaeaID = pChar->GetGaeaID();
							NetMsg.bDriver = TRUE;
							m_pGaeaClient->NETSENDTOAGENT(&NetMsg);							
						}
					}
					else if (pChar->VehicleState() ==  EMVEHICLE_PASSENGER)
					{
						//탑승자일때 내리기 기능
						GLMSG::SNET_GET_OFF_WITH_VEHICLE NetMsg;
						NetMsg.dwGaeaID = pChar->GetGaeaID();
						NetMsg.bDriver = FALSE;
						m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
					}
				}
			}

		}
		break;
	}

}
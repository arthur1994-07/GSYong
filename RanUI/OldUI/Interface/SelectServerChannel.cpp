#include "StdAfx.h"
#include "SelectServerChannel.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../OuterInterface.h"
#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CSelectServerChannel::fLIST_PART_INTERVAL = 10.0f;
const D3DCOLOR	CSelectServerChannel::dwCLICKCOLOR = D3DCOLOR_ARGB(140,255,255,255);
const	int		CSelectServerChannel::nOUTOFRANGE = -1;
const	float	CSelectServerChannel::fNORMAL_PERCENT = 0.4f;
const	float	CSelectServerChannel::fMAX_PERCENT = 0.7f;

CSelectServerChannel::CSelectServerChannel ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice ) :
	CUIOuterWindow ( pInterface, pEngineDevice ),
	m_nIndex ( nOUTOFRANGE )
{
}

CSelectServerChannel::~CSelectServerChannel ()
{
}

void CSelectServerChannel::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SELECT_SERVER_CONNECT );
	pOKButton->CreateBaseButton("SELECT_CHANNEL_CONNECT", 
								CBasicTextButton::SIZE19, 
								CBasicButton::CLICK_FLIP, 
								(char*)ID2GAMEWORD ( "SELECT_SERVER_CONNECTQUIT", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	{
		m_pServerList = new CBasicTextBoxEx(m_pEngineDevice);
		m_pServerList->CreateSub ( this, "SELECT_SERVER_LIST_CHANNEL", UI_FLAG_DEFAULT, SELECT_SERVER_LIST );
		m_pServerList->SetFont ( pFont9 );
		m_pServerList->SetTextAlign ( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
		m_pServerList->SetPartInterval ( fLIST_PART_INTERVAL );
		m_pServerList->CreateMouseClick ( dwCLICKCOLOR );
		m_pServerList->SetSensitive ( true );
		m_pServerList->SetSensitiveView ( false );
		RegisterControl ( m_pServerList );
	}

	{
		m_pServerState = new CBasicTextBox(m_pEngineDevice);
		m_pServerState->CreateSub ( this, "SELECT_SERVER_LIST_STATE" );
		m_pServerState->SetFont ( pFont9 );
		m_pServerState->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		m_pServerState->SetPartInterval ( fLIST_PART_INTERVAL );
		RegisterControl ( m_pServerState );
	}

	CreateControl ( "SELECT_SERVER_BOTTOM" );
}

void CSelectServerChannel::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case SELECT_SERVER_CONNECT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_MOUSEIN_LOGIN );
			}
		}
		break;

	case SELECT_SERVER_LIST:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pServerList->GetSelectPos ();				
				if ( nIndex < 0 || m_pServerList->GetCount () <= nIndex ) return ;

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_nIndex = nIndex;
				}

				if ( dwMsg & UIMSG_LB_DUP )
				{
					AddMessageEx ( UIMSG_MOUSEIN_LOGIN );
				}
			}
		}
		break;
	}
}

void CSelectServerChannel::ResetAll ()
{
	if ( m_pServerList )	m_pServerList->ClearText ();
	if ( m_pServerState )	m_pServerState->ClearText ();

	m_nIndex = nOUTOFRANGE;
}

void CSelectServerChannel::AddChannel(	const CString& strChannelName, 
										const DWORD dwColor, 
										const CString& strState, 
										const DWORD dwStateColor, 
										const DWORD dwChannel )
{
	int nIndex = m_pServerList->AddText ( strChannelName, dwColor );
	m_pServerList->SetTextData ( nIndex, dwChannel );
	m_pServerState->AddText ( strState, dwStateColor );
}

DWORD CSelectServerChannel::GetSelectedChannel ( const int nIndex )
{
	return m_pServerList->GetTextData ( nIndex );
}
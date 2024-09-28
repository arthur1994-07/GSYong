
// bjju.sns 추가

#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/Common/CommonWeb.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"

#include "./FaceBookWindow.h"
#include "./SNSWindow.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"
#include "../SNSLib/SNS.h"

#include <WinInet.h>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CFaceBookWindow::CFaceBookWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CWebWindowBase( pInterface, pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
	, m_emType( PAGETYPE_NONE )
{
}

CFaceBookWindow::~CFaceBookWindow()
{
}

void CFaceBookWindow::SetVisibleSingle( BOOL bVisible )
{
	// Note : 사용안함
	if( !GLUseFeatures::GetInstance().IsUsingSNS() )
	{
		CWebWindowBase::SetVisibleSingle( FALSE );
		return;
	}

	switch ( m_emType )
	{
	// Note : 인증시
	case PAGETYPE_AUTH :
		{
			if ( bVisible )
				bVisible = BeginAuth();
			else
				EndAuth();
		}
		break;

	// Note : 인증 해제시
	case PAGETYPE_UNAUTH:
		{
			if ( bVisible )
				BeginUnAuth();
			else
				EndUnAuth();
		}
		break;

	// Note : 친구 요청시
	case PAGETYPE_FRIEND_ADD:
		{
			if ( bVisible )
				BeginFriendAdd();
			else
				EndFriendAdd();
		}
		break;
	};

	m_pInterface->GetBasicChatWindow()->SetNoUpdate( bVisible );

	CWebWindowBase::SetVisibleSingle( bVisible );
}

void CFaceBookWindow::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	UIKeyCheck::GetInstance()->Check ( DIK_RETURN, DXKEY_DOWN );

	CWebWindowBase::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CFaceBookWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CWebWindowBase::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( FACEBOOK_WINDOW );
			}
		}
		break;	
	}
}

BOOL CFaceBookWindow::BeginAuth	()
{
	//BOOL bBypassed = SNS::BypassCookie(std::string("http://www.facebook.com"));
	//BOOL bDeleted = SNS::DeleteCookie();

	std::string strURL = "";
	if( !m_pGaeaClient->GetSNSClient()->IsWait() &&
        m_pGaeaClient->GetSNSClient()->FBGetAuthURL( strURL ) )
	{
		// Some message handling here.

        SetAddress( strURL.c_str() );
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CFaceBookWindow::EndAuth ()
{
	if ( m_pGaeaClient->GetSNSClient()->FBGetAccessToken() )
	{
		// Some message handling here.

		m_pGaeaClient->GetSNSClient()->FBAuthedReq ();
	}
	else
	{
	    m_pInterface->SNSWindowDoModalError( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_FB_AUTH") );
		
		return FALSE;
	}

	return TRUE;
}

BOOL CFaceBookWindow::BeginUnAuth ()
{
	std::string strURL;
	if( !m_pGaeaClient->GetSNSClient()->FBGetLogOutURL ( strURL ) )
		return FALSE;

    SetAddress( strURL.c_str() );

	return TRUE;
}

BOOL CFaceBookWindow::EndUnAuth	()
{
    SetAddress( NULL );

	return TRUE;
}

BOOL CFaceBookWindow::BeginFriendAdd ()
{
    SetAddress( m_strAddFriendURL.c_str() );

	return TRUE;
}

BOOL CFaceBookWindow::EndFriendAdd ()
{
	//GLSNSClient::GetInstance().FBLogOut ();

    SetAddress( NULL );

	return TRUE;
}

void CFaceBookWindow::SetPageType ( const PAGETYPE emType )
{
	// Note : 켜져있는 상태라면 페이지만 바꿔준다.
	if ( IsVisible() )
	{
		if ( emType == m_emType )
			return;

		switch ( emType )
		{
		case PAGETYPE_AUTH:
			{
				EndAuth ();
			}
			break;

		case PAGETYPE_UNAUTH:
			{
				EndUnAuth ();
			}
			break;

		case PAGETYPE_FRIEND_ADD:
			{
				EndFriendAdd ();
			}
			break;
		}

		m_emType = emType;
		SetVisibleSingle( TRUE );
		return;
	}

	m_emType = emType;
}
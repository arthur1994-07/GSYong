#include "StdAfx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowMain.h"
#include "PostBoxWindowSub.h"
#include "PostBoxWindowNaked.h"

#include "PostBoxReceivePage.h"
#include "PostBoxReadPage.h"
#include "PostBoxSendPage.h"
#include "PostBoxFriendFindPage.h"
#include "PostBoxPreviewPage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CPostBoxWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	// Note : ������ ��� �Ұ����ε� Ȥ�ó� ���������� ���� ó��
	if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		if ( IsWindowCloseAble() )
		{
			Close();
		}

		return;
	}

	// Note : ��û ó��
	if ( m_pWindow_Naked )
	{
		while ( 1 )
		{
			DWORD dwRequest = m_pWindow_Naked->RequestPop();
			if ( dwRequest == POSTBOX_PAGE_REQ_NONE ) break;

			UpdatePageRequest ( POSTBOX_WINDOW_NAKED, dwRequest );
		}
	}

	if ( m_pWindow_Sub )
	{
		while ( 1 )
		{
			DWORD dwRequest = m_pWindow_Sub->RequestPop();
			if ( dwRequest == POSTBOX_PAGE_REQ_NONE ) break;
			
			UpdatePageRequest ( POSTBOX_WINDOW_SUB, dwRequest );
		}
	}

	if ( m_pWindow_Main)
	{
		while ( 1 )
		{
			DWORD dwRequest = m_pWindow_Main->RequestPop();
			if ( dwRequest == POSTBOX_PAGE_REQ_NONE ) break;

			UpdatePageRequest ( POSTBOX_WINDOW_MAIN, dwRequest );
		}
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxWindow::UpdatePageRequest ( UIGUID ControlID, const DWORD dwRequest )
{
	switch ( dwRequest )
	{
	case POSTBOX_PAGE_REQ_NONE: 
		break;

	case POSTBOX_PAGE_REQ_FRIENDFIND_OPEN:
		{
			WindowPageOpenSlot( m_pWindow_Naked, m_pPage_FriendFind, 1 );
		}
		break;

	case POSTBOX_PAGE_REQ_FRIENDFIND_CLOSE:
		{
			WindowPageClose ( m_pPage_FriendFind );
		}
		break;

	case POSTBOX_PAGE_REQ_PREVIEW_OPEN:
		{
			if ( !m_pPage_Preview )
			{
				break;
			}

			// Note : �̸������ ������ �����µ� ����
			SAPOSTCLIENT sPost;
			m_pPage_Send	-> MakePreviewPost ( sPost );
			m_pPage_Preview	-> SetPost ( &sPost );

			WindowPageOpenSlot( m_pWindow_Sub, m_pPage_Preview, 1 );
		}
		break;

	case POSTBOX_PAGE_REQ_PREVIEW_CLOSE:
		{
			WindowPageClose ( m_pPage_Preview );
		}
		break;

	case POSTBOX_PAGE_REQ_FRIENDFIND_SELECT_TO_SEND:
		{
			if ( m_pPage_FriendFind && m_pPage_Send )
			{
				m_pPage_Send->SetEditBoxTo ( m_pPage_FriendFind->GetSelectName() );
			}
		}
		break;

	case POSTBOX_PAGE_REQ_READ_OPEN:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			SAPOSTCLIENT* pPost = m_pPage_Receive->GetSelectedPost();

			if ( !pPost )
			{
				break;
			}

			// Note : ������ �����Ѵ�
			m_pPage_Read->SetPost ( pPost );
			m_pPage_Read->Reset	  ( );

			// Note : �ش� �������� Ȱ��ȭ �Ǿ����� �ʴٸ�
			if ( !m_pWindow_Sub->IsWindowOpen() || !m_pWindow_Sub->IsPageOpen( m_pPage_Read ) )
			{
				WindowPageOpen ( m_pWindow_Sub, m_pPage_Read );
			}
		}
		break;

	case POSTBOX_PAGE_REQ_READ_CLOSE:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			// ������ �����ϰ� �б�â�� �ݴ´�.
			m_pPage_Receive -> SelectReset();
			m_pPage_Read	-> SetPost( NULL );
			WindowPageClose ( m_pPage_Read );
		}
		break;

	case POSTBOX_PAGE_REQ_READ_RETURN:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			DWORD dwIndex = m_pPage_Receive->GetSelectedIndex ();
			
			if ( !m_pPage_Receive->ReturnIndex ( dwIndex ) )
			{
				break;
			}

			// ������ �����ϰ� �б�â�� �ݴ´�.
			m_pPage_Receive -> SelectReset();
			m_pPage_Read	-> SetPost( NULL );
			WindowPageClose ( m_pPage_Read );
		}
		break;

	case POSTBOX_PAGE_REQ_READ_DELETE:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			DWORD dwIndex = m_pPage_Receive->GetSelectedIndex ();

			if ( !m_pPage_Receive->DeleteIndex ( dwIndex ) )
			{
				break;
			}

			// ������ �����ϰ� �б�â�� �ݴ´�.
			m_pPage_Receive -> SelectReset();
			m_pPage_Read	-> SetPost( NULL );
			WindowPageClose ( m_pPage_Read );
		}
		break;

	case POSTBOX_PAGE_REQ_READ_REPLY:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			// ���� ����� �̸��� �����ͼ� Send�������� ����.
			SAPOSTCLIENT* pPost = m_pPage_Read->GetPost();

			if ( !pPost )
			{
				break;
			}

			m_pPage_Send->SetEditBoxTo ( pPost->m_SenderName );

			// ������ �����ϰ� �б�â�� �ݴ´�.
			m_pPage_Receive -> SelectReset();
			m_pPage_Read	-> SetPost( NULL );
			WindowPageClose ( m_pPage_Read );

			// ���� ������� �ٲ۴�.
			m_pWindow_Main->TapSelect ( CPostBoxWindowMain::TAP_SEND );
			RefreshMainPage ();
		}
		break;

	case POSTBOX_PAGE_REQ_RECEIVE_SELECT_RESET:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			m_pPage_Receive->SelectReset();
		}
		break;

	case POSTBOX_PAGE_REQ_RECEIVE_REFRESH:
		{
			if ( !m_pPage_Read || !m_pPage_Receive )
			{
				break;
			}

			// Note : �ޱ� ��Ÿ��
			if ( m_pGaeaClient->GetPostClient()->IsRecvCoolTime())
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_RECV_COOLTIME") );
				break;
			}

			// ������ �����ϰ� �б�â�� �ݴ´�.
			m_pPage_Receive -> SelectReset();
			m_pPage_Read	-> SetPost( NULL );
			WindowPageClose ( m_pPage_Read );

			if (   !m_pGaeaClient->GetPostClient()->IsRequirePost()
				&& !m_pGaeaClient->GetPostClient()->IsRequireSend() )
			{
				m_pGaeaClient->GetPostClient()->ReqReceivePostInfo	 ();
				m_pGaeaClient->GetPostClient()->ReqReceivePost		 ();
                m_pPage_History->ResetRequestState();
			}
		}
	}

}

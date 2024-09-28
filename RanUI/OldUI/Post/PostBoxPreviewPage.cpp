#include "StdAfx.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLogicEx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "PostBoxPreviewPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxPreviewPage::CPostBoxPreviewPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CPostBoxReadPage(pGaeaClient, pInterface, pEngineDevice)
{
	memset( &m_PreviewPost, 0, sizeof(SAPOSTCLIENT) );
}

CPostBoxPreviewPage::~CPostBoxPreviewPage()
{
	// Blank
}

void CPostBoxPreviewPage::DisableSubControl ()
{
	// Note : �����ð� ����
	m_pText_LeftTime->SetVisibleSingle( FALSE );

	// Note : ������ư ����
	m_pButton_Delete->SetVisibleSingle( FALSE );
	m_pButton_Delete->SetNoMessage( TRUE );
	m_pButton_Delete->SetNoMessage( TRUE );

	// Note : �ޱ��ư ����
	m_pButton_Take->SetVisibleSingle( FALSE );
	m_pButton_Take->SetNoMessage( TRUE );
	m_pButton_Take->SetNoMessage( TRUE );

	// Note : �ݼ۹�ư ����
	m_pButton_Return->SetVisibleSingle( FALSE );
	m_pButton_Return->SetNoMessage( TRUE );
	m_pButton_Return->SetNoMessage( TRUE );

	// Note : �����ư ����
	m_pButton_Reply->SetVisibleSingle( FALSE );
	m_pButton_Reply->SetNoMessage( TRUE );
	m_pButton_Reply->SetNoMessage( TRUE );
}

void CPostBoxPreviewPage::CreateSubControl ()
{
	CPostBoxReadPage::CreateSubControl();

	// Note : ��Ʈ�� ��Ȱ��ȭ
	DisableSubControl ();
}

void CPostBoxPreviewPage::OpenSubControl ()
{
	CPostBoxReadPage::OpenSubControl();

	// Note : �����ݾ� ���� �ؽ�Ʈ ������¡
	{
		CUIControl TempControl(m_pEngineDevice); 
		TempControl.Create ( 1, "POSTBOX_PAGE_PREVIEW_TEXT_STATIC_MYMONEY" );

		UIRECT rcOldPos = m_pText_Static_MyMoney->GetLocalPos ();
		UIRECT rcNewPos = TempControl.GetLocalPos ();

		m_pText_Static_MyMoney->AlignSubControl ( rcOldPos, rcNewPos );
		m_pText_Static_MyMoney->SetLocalPos     ( rcNewPos );

		m_pText_Static_MyMoney->ClearText();
		m_pText_Static_MyMoney->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_PREVIEW_TEXTBOX", 0 ) );
	}

	// Note : �����ݾ� �ؽ�Ʈ ������¡
	{
		CUIControl TempControl(m_pEngineDevice); 
		TempControl.Create ( 1, "POSTBOX_PAGE_PREVIEW_TEXT_MYMONEY" );

		UIRECT rcOldPos = m_pText_MyMoney->GetLocalPos ();
		UIRECT rcNewPos = TempControl.GetLocalPos ();

		m_pText_MyMoney->AlignSubControl ( rcOldPos, rcNewPos );
		m_pText_MyMoney->SetLocalPos     ( rcNewPos );
	}

	// Note : �����ݾ� ���ιڽ� ������¡
	{
		CUIControl TempControl(m_pEngineDevice); 
		TempControl.Create ( 1, "POSTBOX_PAGE_PREVIEW_LINEBOX_MYMONEY" );

		UIRECT rcOldPos = m_pLineBox_MyMoney->GetLocalPos ();
		UIRECT rcNewPos = TempControl.GetLocalPos ();

		m_pLineBox_MyMoney->AlignSubControl ( rcOldPos, rcNewPos );
		m_pLineBox_MyMoney->SetLocalPos     ( rcNewPos );
	}

	// Note : ��Ʈ�� ��Ȱ��ȭ
	DisableSubControl ();
}

void CPostBoxPreviewPage::CloseSubControl ()
{
	// Blank
}

void CPostBoxPreviewPage::UpdateVariableControl	()
{
	CPostBoxReadPage::UpdateVariableControl();

	// Note : ��Ʈ�� ��Ȱ��ȭ
	DisableSubControl ();
}

void CPostBoxPreviewPage::UpdateVariableText ()
{
	CPostBoxReadPage::UpdateVariableText();

	GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

	// Note : ���� ������ ���� �ݾ�
	if ( m_pPost )
	{
		BOOL bAttachDesign	= FALSE;
		BOOL bAttachItem	= FALSE;
		
		if ( m_pPost->m_iDesign )
		{
			bAttachDesign = TRUE;
		}

		if ( m_pPost->m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			bAttachItem = TRUE;
		}

		LONGLONG llCommision = GLPostClient::GetCommision ( bAttachDesign, bAttachItem, m_pPost->m_llAttachMoney );
		LONGLONG llMoney	 = sCharData.GetInvenMoney() - llCommision - m_pPost->m_llAttachMoney;

		CString strText;
		strText = NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( llMoney, nMONEYUNIT, "," );
		m_pText_MyMoney->SetOneLineText ( strText );
	}
	else
	{
		LONGLONG llCommision = GLPostClient::GetCommision ( FALSE, FALSE, 0 );
		LONGLONG llMoney	 = sCharData.GetInvenMoney() - llCommision;

		CString strText;
		strText = NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( llMoney, nMONEYUNIT, "," );
		m_pText_MyMoney->SetOneLineText ( strText );
	}
}

void CPostBoxPreviewPage::SetPost ( SAPOSTCLIENT* pPost )
{
	if ( !pPost )
	{
		m_pPost = NULL;
		return;
	}

	m_PreviewPost = (*pPost);
	m_pPost		  = &m_PreviewPost;
}

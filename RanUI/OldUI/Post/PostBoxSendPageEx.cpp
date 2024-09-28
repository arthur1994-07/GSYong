#include "StdAfx.h"

#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "PostBoxWindow.h"
#include "PostBoxSendPage.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Item/ItemImage.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../MfcExLib/RanFilter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CPostBoxSendPage::SetMoneyReqType ( const EMREQMONEYTYPE emType )
{
	if ( m_emMoneyReq == emType )
	{
		return;
	}

	m_emMoneyReq = emType;

	switch ( m_emMoneyReq )
	{
	case EMREQMONEYTYPE_SEND:
		{
			m_pLineBox_SendMoney		->SetVisibleSingle( TRUE  );
			m_pLineBox_SendMoney_Disable->SetVisibleSingle( FALSE );

			m_pLineBox_Billing			->SetVisibleSingle( FALSE );
			m_pLineBox_Billing_Disable  ->SetVisibleSingle( TRUE  );

			m_pText_Static_SendMoney	->SetUseTextColor( 0, TRUE );
			m_pText_Static_Billing		->SetUseTextColor( 0, TRUE );

			m_pText_Static_SendMoney	->SetTextColor( 0, cCOLOR_TEXT_ENABLE  );
			m_pText_Static_Billing		->SetTextColor( 0, cCOLOR_TEXT_DISABLE );

			m_pEditBox_SendMoney->SetEditString ("0");
			m_pEditBox_Billing  ->SetEditString	("" );

			m_pEditBox_SendMoney->EndEdit();
			m_pEditBox_Billing	->EndEdit();

			m_pImage_SendMoney->SetImage( "POSTBOX_PAGE_SEND_IMAGE_GOLD_ENABLE"  );
			m_pImage_Billing  ->SetImage( "POSTBOX_PAGE_SEND_IMAGE_GOLD_DISABLE" );
		}
		break;

	case EMREQMONEYTYPE_BILLING:
		{
			m_pLineBox_SendMoney		->SetVisibleSingle( FALSE );
			m_pLineBox_SendMoney_Disable->SetVisibleSingle( TRUE  );

			m_pLineBox_Billing			->SetVisibleSingle( TRUE  );
			m_pLineBox_Billing_Disable  ->SetVisibleSingle( FALSE );

			m_pText_Static_SendMoney	->SetUseTextColor( 0, TRUE );
			m_pText_Static_Billing		->SetUseTextColor( 0, TRUE );

			m_pText_Static_SendMoney	->SetTextColor( 0, cCOLOR_TEXT_DISABLE );
			m_pText_Static_Billing		->SetTextColor( 0, cCOLOR_TEXT_ENABLE  );

			m_pEditBox_SendMoney->SetEditString ("" );
			m_pEditBox_Billing  ->SetEditString	("0");

			m_pEditBox_SendMoney->EndEdit();
			m_pEditBox_Billing	->EndEdit();

			m_pImage_SendMoney->SetImage( "POSTBOX_PAGE_SEND_IMAGE_GOLD_DISABLE" );
			m_pImage_Billing  ->SetImage( "POSTBOX_PAGE_SEND_IMAGE_GOLD_ENABLE"	 );
		}
		break;
	};
}

void CPostBoxSendPage::SetEditBoxTo ( const CString& strName )
{
	if ( m_pEditBox_To )
	{
		m_pEditBox_To->SetEditString( strName );
		m_pEditBox_To->BeginEdit();
	}
}

const BOOL CPostBoxSendPage::ReqSendPost ()
{
    //////////////////////////////////////////////////////////////////////////
    // ���� ( Post )
    //  ������ �������� ���ؼ� ���� ������ ����� ���� �ִ�.
    if( GLUseFeatures::GetInstance().IsUsingPostSendBlock() )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, "Not available.");
        return FALSE;
    }

	GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if ( !pChar || !pChar->IsValidBody() )
	{
		return FALSE;
	}

	// Note : ���� üũ
	if ( (DWORD)pChar->GETLEVEL() < GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL )
	{
		CString strText;
		strText.Format ( ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_LEVEL"), GLCONST_POST::dwPOST_SEND_REQUIRE_LEVEL );
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText.GetString() );

		return FALSE;
	}

	// Note : ������ ���϶�
	if (m_pGaeaClient->GetPostClient()->IsRequireSend())
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_PROCESSING") );
		return FALSE;
	}

	// Note : ���� ��Ÿ��
	if (m_pGaeaClient->GetPostClient()->IsSendCoolTime())
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_COOLTIME") );
		return FALSE;
	}

	SAPOST sPost;
	MakeSendPost ( sPost );

	if ( sPost.m_RecverName[0] == NULL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_RECVERNAME") );
		return FALSE;
	}

	if ( sPost.m_Title[0] == NULL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_TITLE") );
		return FALSE;
	}

	if ( sPost.m_Content[0] == NULL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_CONTENTS") );
		return FALSE;
	}

	// Note : ���û���Ͻ� û�� �ݾ��� �־���Ѵ�.
	if ( m_emMoneyReq == EMREQMONEYTYPE_BILLING )
	{
		if ( !sPost.m_llCharge )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_BILLING_ZERO") );
			return FALSE;
		}
	}

	// Note : ������ �˻�
	{
		BOOL		bAttachDesign = (sPost.m_iDesign) ? TRUE : FALSE;
		BOOL		bAttachItem   = (sPost.m_sAttach01.GetNativeID() != NATIVEID_NULL()) ? TRUE : FALSE;
		LONGLONG	llCommision   = GLPostClient::GetCommision ( bAttachDesign, bAttachItem, sPost.m_llAttachMoney );

		GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
		
		if ( sCharData.GetInvenMoney() < (sPost.m_llAttachMoney + llCommision) )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("POSTBOX_ERROR_MSG_SEND_MONEY_LACK") );
			return FALSE;
		}
	}

	// Note : �޼��� ä���
	{
		GLMSG::SNET_POST_SEND NetMsg;
		NetMsg.iSender = sPost.m_iSender;

		// Note : �޴� ����� ������ ���� ä�� �ʿ� ����.
		NetMsg.iRecver		  = 0;
		NetMsg.nUserNumRecver = 0;

		// Note : ���� Ÿ��
		NetMsg.byPostClass = sPost.m_byPostClass;
		NetMsg.byPostState = sPost.m_byPostState;
		NetMsg.byPostType  = sPost.m_byPostType;

		// Note : ��� û���� ���
		if ( sPost.m_llCharge )
		{
			NetMsg.llAttachMoney = 0;
			NetMsg.llCharge		 = sPost.m_llCharge;
			NetMsg.byPayment	 = 1;
		}
		// Note : �ݾ� ÷���� ���
		else
		{
			NetMsg.llAttachMoney = sPost.m_llAttachMoney;
			NetMsg.llCharge		 = 0;
			NetMsg.byPayment	 = 0;
		}

		// Note : ÷�� ������
		if ( sPost.m_sAttach01.GetNativeID() != NATIVEID_NULL() )
		{
			const SINVEN_POS& sPos = pChar->GetPostItemPos();
			NetMsg.xAttach01 = sPos.wPosX;
			NetMsg.yAttach01 = sPos.wPosY;
			NetMsg.nAttach01 = sPost.m_sAttach01.TurnNum();
			NetMsg.idAttach01 = sPost.m_sAttach01.GetNativeID();
		}
		else
		{
			NetMsg.xAttach01 = USHRT_MAX;
			NetMsg.yAttach01 = USHRT_MAX;
			NetMsg.nAttach01 = 0;
			NetMsg.idAttach01 = NATIVEID_NULL();
		}

		// Note : �ؽ�Ʈ
		StringCchCopy( NetMsg.RecverName,	CHR_ID_LENGTH,			sPost.m_RecverName );
		StringCchCopy( NetMsg.Title,		sizeof(NetMsg.Title)  ,	sPost.m_Title		 );
		StringCchCopy( NetMsg.Content,		sizeof(NetMsg.Content),	sPost.m_Content	 );

		// Note : ������ ī��
		NetMsg.iDesign = sPost.m_iDesign;

		// Note : ����
		if (!m_pGaeaClient->GetPostClient()->ReqSendPost(&NetMsg))
		{
			return FALSE;
		}

		m_pInterface->PostBoxWindowDoModalMsg(ID2GAMEINTEXT("MODAL_POSTBOX", 5));

		return TRUE;
	}

	return FALSE;
}

void CPostBoxSendPage::MakeSendPost	( SAPOST& OutPost )
{
	ZeroMemory ( &OutPost, sizeof(SAPOST) );

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
		return;

	// Note : �ؽ�Ʈ ����
	CString strSender   = pCharacter->GetName();
	CString strReceiver = m_pEditBox_To->GetEditString();
	CString strSubject  = m_pEditBox_Subject->GetEditString();
	CString strContents = ""; 

	//m_pMultiEditBox_Contents->GetText( strContents );
	strContents = m_pMultiEditBox_Contents->GetEditString();

	// Note : �ݾ� ���� ����
	LONGLONG llMoney  = 0;
	BOOL	 bBilling = FALSE;
	{
		switch ( m_emMoneyReq )
		{
			// �ݾ� ÷��
		case EMREQMONEYTYPE_SEND:
			{
				const CString& strMoney = m_pEditBox_SendMoney->GetEditString();
				llMoney = static_cast<LONGLONG>( _atoi64 ( strMoney ) );

				bBilling = FALSE;
			}
			break;

			// ��� û��
		case EMREQMONEYTYPE_BILLING:
			{
				const CString& strMoney = m_pEditBox_Billing->GetEditString();
				llMoney = static_cast<LONGLONG>( _atoi64 ( strMoney ) );

				bBilling = TRUE;
			}
			break;
		}

		if ( llMoney < 0 )
		{
			llMoney = 0;
		}
	}

	// Note : ä���
	{	
		SAPOST sPost;
		ZeroMemory ( &sPost, sizeof(SAPOST) );

		sPost.m_iSender = (int)pCharacter->GetCharID();
		sPost.m_iRecver = 0;

		// Note : ���� Ÿ��
		sPost.m_byPostClass = EMPOST_USER;	// �Ϲݿ���
		sPost.m_byPostState = EMPOST_UNREAD;   // �Ϲݻ���

		// Note : ���� ����
		sPost.m_byAttach01Accept = FALSE;
		sPost.m_byMoneyAccept	   = FALSE;
		sPost.m_byChargePayment  = FALSE;

		// Note : ��� û���� ���
		if ( bBilling )
		{
			sPost.m_llAttachMoney   = 0;
			sPost.m_llCharge		  = llMoney;
		}
		// Note : �ݾ� ÷���� ���
		else
		{
			sPost.m_llAttachMoney = llMoney;
			sPost.m_llCharge		= 0;
		}

		// Note : ÷�� ������
		sPost.m_sAttach01 = pCharacter->GetPostItem();

		// Note : �ؽ�Ʈ
		StringCchCopy( sPost.m_SenderName,	CHR_ID_LENGTH,			strSender   );
		StringCchCopy( sPost.m_RecverName,	CHR_ID_LENGTH,			strReceiver );

		// Note : �ؽ�Ʈ �� ���͸��� ����
		if ( CRanFilter::GetInstance().ChatFilter( strSubject ) )
		{
			strSubject = CRanFilter::GetInstance().GetProverb();
		}

		if ( CRanFilter::GetInstance().ChatFilter( strContents ) )
		{
			strContents = CRanFilter::GetInstance().GetProverb();
		}

		StringCchCopy( sPost.m_Title,			sizeof(sPost.m_Title)  ,  strSubject  );
		StringCchCopy( sPost.m_Content,		sizeof(sPost.m_Content),  strContents );

		// Note : ������ ī��
		sPost.m_iDesign = (int)m_dwDesign;

		// Note : ���� Ÿ��
		BOOL bAttachItem  = !( sPost.m_sAttach01.GetNativeID() == NATIVEID_NULL() );
		BOOL bAttachMoney = ( sPost.m_llAttachMoney > 0 );

		if ( bAttachMoney && bAttachItem )
		{
			sPost.m_byPostType = EMPOST_MONEYITEM;
		}
		else
		{	
			if (bAttachMoney)
			{
				sPost.m_byPostType = EMPOST_MONEY;
			}
			else if (bAttachItem)
			{
				sPost.m_byPostType = EMPOST_ITEM;
			}
			else
			{
				sPost.m_byPostType = EMPOST_TEXT;
			}
		}

		OutPost = sPost;
	}
}

void CPostBoxSendPage::MakePreviewPost ( SAPOSTCLIENT& OutPostClient )
{
	// Note : ä���
	{
		SAPOST sPost;
		MakeSendPost ( sPost );
		OutPostClient = sPost;

		// Note : ���� ��¥ ���
		CTime cCurrentTime;
		cCurrentTime = CTime::GetCurrentTime();
        OutPostClient.m_SendDate = cCurrentTime.GetTime();
	}
}

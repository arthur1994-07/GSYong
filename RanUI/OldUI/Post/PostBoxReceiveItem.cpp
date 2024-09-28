#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../Item/ItemImageEx.h"
#include "PostBoxReceiveItem.h"
#include "PostBoxReadPage.h"

#include "../Util/CheckBox.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxReceiveItem::CPostBoxReceiveItem(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pCheck			( NULL  )
	, m_pItem			( NULL  )
	, m_pItemException	( NULL  )
	, m_pText_Date		( NULL  )
	, m_pText_Subject	( NULL  )
	, m_pText_From		( NULL  )
	, m_pText_LeftTime	( NULL  )
	, m_pBoxFlag		( NULL  )
	, m_pBoxAlpha		( NULL  )
	, m_emBoxType		( EMRECEIVEITEMBOXTYPE_DEFAULT )
	, m_PostData		( NULL  )
{
	for ( DWORD i=0; i<BOX_MAX;		++i ) { m_pBox	  [i] = NULL; } 
	for ( DWORD i=0; i<BOXLINE_MAX; ++i ) { m_pBoxLine[i] = NULL; }
}

CPostBoxReceiveItem::~CPostBoxReceiveItem()
{
	// Blank
}

void CPostBoxReceiveItem::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// Note : �ڽ�
	{
		CBasicLineBoxSmart* pBox = NULL;

		pBox = m_pBox[BOX_DEFAULT] = new CBasicLineBoxSmart(m_pEngineDevice);
		pBox->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_ITEM_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_RECEIVE_ITEM_BOX_DEFAULT );
		pBox->CreateSubControl( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_DEFAULT_TEXINFO" );
		RegisterControl ( pBox );

		pBox = m_pBox[BOX_EMPTY] = new CBasicLineBoxSmart(m_pEngineDevice);
		pBox->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_ITEM_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_RECEIVE_ITEM_BOX_EMPTY );
		pBox->CreateSubControl( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_EMPTY_TEXINFO" );
		RegisterControl ( pBox );
	}

	// Note : üũ�ڽ�
	{
		m_pCheck = new CCheckBox(m_pEngineDevice);
		m_pCheck->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_ITEM_CHECKBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_RECEIVE_ITEM_CHECKBOX );
		m_pCheck->CreateSubControl( "POSTBOX_PAGE_RECEIVE_ITEM_CHECKBOX_ON", "POSTBOX_PAGE_RECEIVE_ITEM_CHECKBOX_OFF" );
		RegisterControl ( m_pCheck );
	}

	// Note : ����
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_RECEIVE_ITEM_LINE" );
	}

	// Note : ������
	{
		m_pItem          = CreateItemImage ( "POSTBOX_PAGE_RECEIVE_ITEM_ITEMIMAGE" );
		m_pItemException = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_ITEM_ITEMIMAGE_EXCEPTION" );

		SetItemImage ( NATIVEID_NULL() );
	}

	// Note : �ؽ�Ʈ
	{
		m_pText_Date	 = CreateTextBox ( "POSTBOX_PAGE_RECEIVE_ITEM_TEXT_DATE",	   pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
		m_pText_From	 = CreateTextBox ( "POSTBOX_PAGE_RECEIVE_ITEM_TEXT_FROM",	   pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
		m_pText_LeftTime = CreateTextBox ( "POSTBOX_PAGE_RECEIVE_ITEM_TEXT_LEFTTIME",  pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
		m_pText_Subject  = CreateTextBox ( "POSTBOX_PAGE_RECEIVE_ITEM_TEXT_SUBJECT",   pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	}

	// Note : ���� Ÿ��
	{
		m_pType = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_ITEM_TYPE" );
	}

	// Note :  �ڽ� ����
	{
		CBasicLineBoxSmart* pBoxLine = NULL;

		pBoxLine = m_pBoxLine[BOXLINE_SELECTED] = new CBasicLineBoxSmart(m_pEngineDevice);
		pBoxLine->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_ITEM_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_SELECTED );
		pBoxLine->CreateSubControl( "POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_SELECTED_TEXINFO" );
		RegisterControl ( pBoxLine );

		pBoxLine = m_pBoxLine[BOXLINE_NEW] = new CBasicLineBoxSmart(m_pEngineDevice);
		pBoxLine->CreateSub ( this, "POSTBOX_PAGE_RECEIVE_ITEM_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_NEW );
		pBoxLine->CreateSubControl( "POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_NEW_TEXINFO" );
		RegisterControl ( pBoxLine );
	}

	// Note : �ڽ� �÷���
	{
		m_pBoxFlag = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_FLAG" );
	}

	// Note : �ڽ� ����
	{
		m_pBoxAlpha = CreateSwapImage ( "POSTBOX_PAGE_RECEIVE_ITEM_BOX" );
		m_pBoxAlpha->SetImage( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_ALPHA" );
	}

	UpdateBox ();
}

void CPostBoxReceiveItem::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_EMPTY )
	{
		return;
	}

	// Note : ���� ��Ʈ�� ������Ʈ
	UpdateVariableControl();

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxReceiveItem::UpdateBox ()
{
	// Note : ���� �ʱ�ȭ.
	//
	for ( DWORD i=0; i<BOX_MAX; ++i )
	{
		m_pBox[i]->SetVisibleSingle( FALSE );
	}

	for ( DWORD i=0; i<BOXLINE_MAX; ++i )
	{
		m_pBoxLine[i]->SetVisibleSingle( FALSE );
	}

	m_pBoxFlag ->SetVisibleSingle( FALSE );
	m_pBoxAlpha->SetVisibleSingle( FALSE );

	// Note : Ÿ�Կ� ���� ���ش�.
	//
	switch ( m_emBoxType )
	{
	case EMRECEIVEITEMBOXTYPE_DEFAULT:
		{
			m_pBox[ BOX_DEFAULT ]->SetVisibleSingle( TRUE );

			m_pBoxAlpha->SetVisibleSingle( TRUE );
		}
		break;

	case EMRECEIVEITEMBOXTYPE_EMPTY:
		{
			m_pBox[ BOX_EMPTY ]->SetVisibleSingle( TRUE );
		}
		break;

	case EMRECEIVEITEMBOXTYPE_SELECTED:
		{
			m_pBox	   [ BOX_DEFAULT	  ]->SetVisibleSingle( TRUE );
			m_pBoxLine [ BOXLINE_SELECTED ]->SetVisibleSingle( TRUE );
		}
		break;

	case EMRECEIVEITEMBOXTYPE_NEW:
		{
			m_pBox	   [ BOX_DEFAULT	  ]->SetVisibleSingle( TRUE );
			m_pBoxLine [ BOXLINE_NEW	  ]->SetVisibleSingle( TRUE );
		}
		break;
	}

	// Note : ������ ī��
	if ( m_PostData )
	{
		if ( m_emBoxType != EMRECEIVEITEMBOXTYPE_EMPTY )
		{
			DWORD dwDesign = m_PostData->m_iDesign;

			if ( dwDesign != 0 )
			{
				DWORD dwBoxWidth  = nBOX_SIZE_WIDTH;
				DWORD dwBoxHeight = nBOX_SIZE_HEIGHT;

				DWORD dwPosX   = ((dwDesign-1)%CPostBoxReadPage::nDESIGN_CARD_NUM_WIDTH) * CPostBoxReadPage::nDESIGN_CARD_SIZE_WIDTH;
				DWORD dwPosY   = ((dwDesign-1)/CPostBoxReadPage::nDESIGN_CARD_NUM_WIDTH) * CPostBoxReadPage::nDESIGN_CARD_SIZE_HEIGHT;

				dwPosY += (CPostBoxReadPage::nDESIGN_CARD_SIZE_HEIGHT/2) - (dwBoxHeight/2);

				m_pBox [ BOX_DEFAULT ]->SetBackgroundImageEx	( "POSTBOX_DISIGN_CARD", dwPosX, dwPosY, dwBoxWidth, dwBoxHeight );
				m_pBox [ BOX_DEFAULT ]->SetBackgroundView		( TRUE );
			}
			else
			{
				m_pBox[ BOX_DEFAULT	]->SetBackgroundView		( FALSE );
			}
		}
	}

	// Note : �ڽ� �÷���
	if ( m_PostData )
	{
		if ( m_emBoxType != EMRECEIVEITEMBOXTYPE_EMPTY )
		{
			if ( m_PostData->m_byPostClass == EMPOST_GM )
			{
				m_pBoxFlag->SetVisibleSingle( TRUE );
				m_pBoxFlag->SetImageResizing( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_FLAG_GM" );
			}
			else if ( m_PostData->m_byPostClass == EMPOST_SYSTEM )
			{
				m_pBoxFlag->SetVisibleSingle( TRUE );
				m_pBoxFlag->SetImageResizing( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_FLAG_SYSTEM" );
			}
			else if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_NEW )
			{
				m_pBoxFlag->SetVisibleSingle( TRUE );
				m_pBoxFlag->SetImageResizing( "POSTBOX_PAGE_RECEIVE_ITEM_BOX_FLAG_NEW" );
			}
		}
	}
}

void CPostBoxReceiveItem::UpdateVariableControl ()
{
	//  Note : ������ ���� ������Ʈ
	//
	if ( m_pItem )
	{
		if ( m_PostData )
		{
			if ( m_PostData->m_byAttach01Accept || m_PostData->m_sAttach01.GetNativeID() == NATIVEID_NULL() )
			{
				SetItemImage ( NATIVEID_NULL() );
				m_pItem->SetTurnNum( 0 );
			}
			else
			{
				m_pItem->SetTurnNum( m_PostData->m_sAttach01.wTurnNum );
			}
		}
		else
		{
			SetItemImage ( NATIVEID_NULL() );
			m_pItem->SetTurnNum( 0 );
		}
	}

	// Note : ���� ���� ������Ʈ
	if ( m_PostData )
	{
		if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_SELECTED )
		{
			m_pText_Subject->SetOneLineText ( m_PostData->m_Title, NS_UITEXTCOLOR::WHITE );
		}
		else
		{
			m_pText_Subject->SetOneLineText ( m_PostData->m_Title, COLOR_SUBJECT_DEFAULT );
		}
	}
}

void CPostBoxReceiveItem::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case POSTBOX_PAGE_RECEIVE_ITEM_BOX_DEFAULT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg&UIMSG_LB_DOWN )
				{
					AddMessageEx ( UIMSG_POSTBOX_RECEIVE_ITEM_READ );
				}
			}
		}
		break;
	};
}

void CPostBoxReceiveItem::Reset ()
{
	m_emBoxType = EMRECEIVEITEMBOXTYPE_EMPTY;

	// Note : ������ �ʴ� �͵�
	{
		m_pItem			 ->SetVisibleSingle( FALSE );
		m_pItemException ->SetVisibleSingle( FALSE );
		m_pText_Date	 ->SetVisibleSingle( FALSE );
		m_pText_Subject  ->SetVisibleSingle( FALSE );
		m_pText_From     ->SetVisibleSingle( FALSE );
		m_pText_LeftTime ->SetVisibleSingle( FALSE );
		m_pType			 ->SetVisibleSingle( FALSE );
	}

	UpdateBox ();

	m_pCheck->SetCheckOff();
	SetDiffuse( COLOR_DISABLE );

	// Note : ���� ��Ʈ�� ������Ʈ
	UpdateVariableControl();
}

CItemImageEx* CPostBoxReceiveItem::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImageEx* pItemImage = new CItemImageEx ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

CSwapImage* CPostBoxReceiveItem::CreateSwapImage ( const char* szControl, UIGUID ControlID )
{
	CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	RegisterControl ( pSwapImage );

	return pSwapImage;
}

CBasicTextBox* CPostBoxReceiveItem::CreateTextBox ( const char* szControl, CD3DFontPar* pFont, int nAlign, const UIGUID& ControlID )
{
	CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
	pText->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pText->SetFont ( pFont );
	pText->SetTextAlign ( nAlign );	
	RegisterControl ( pText );

	return pText;
}

CBasicLineBox*  CPostBoxReceiveItem::CreateBackgroundControl ( const char* szControl )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

void CPostBoxReceiveItem::SetPost ( SAPOSTCLIENT* pPostData, const BOOL bForcing )
{
	// Note : ���� ���� ����
	if ( bForcing )
	{
		m_PostData = pPostData;
	}
	else
	{
		if ( !m_PostData || !pPostData )
		{
			m_PostData = pPostData;
		}
		else if ( m_PostData != pPostData )
		{
			m_PostData = pPostData;
		}
		else
		{
			if ( m_PostData->m_llPostID == pPostData->m_llPostID )
			{
				// Note : ������� ���ٸ� ���� �� �ʿ� ����.
				return;
			}

			m_PostData = pPostData;
		}
	}

	if ( !m_PostData )
	{
		Reset();
		return;
	}

	// Note : �ڽ� ����
	if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_SELECTED )
	{
		m_emBoxType = EMRECEIVEITEMBOXTYPE_SELECTED;
	}
	else
	{
		if ( m_PostData->IsUnRead() )
		{
			m_emBoxType = EMRECEIVEITEMBOXTYPE_NEW;
		}
		else
		{
			m_emBoxType = EMRECEIVEITEMBOXTYPE_DEFAULT;
		}
	}


	// Note : ��Ʈ�� Ȱ��ȭ
	{
		m_pItem			 ->SetVisibleSingle( TRUE );
		m_pItemException ->SetVisibleSingle( TRUE );
		m_pText_Date	 ->SetVisibleSingle( TRUE );
		m_pText_Subject  ->SetVisibleSingle( TRUE );
		m_pText_From     ->SetVisibleSingle( TRUE );
		m_pText_LeftTime ->SetVisibleSingle( TRUE );
		m_pType			 ->SetVisibleSingle( TRUE );

		m_pCheck->SetCheckOff();
		SetDiffuse( COLOR_ENABLE );

		UpdateBox ();
	}

	// Note : ������ ����
	{
		if ( m_PostData->m_byAttach01Accept || m_PostData->m_sAttach01.GetNativeID() == NATIVEID_NULL() )
		{
			SetItemImage ( NATIVEID_NULL() );
		}
		else
		{
			SetItemImage ( m_PostData->m_sAttach01.GetNativeID() );
		}
	}

	// Note : ���� Ÿ�� �̹��� ����
	{
		// �ݼ۵� �����̶��
		if ( m_PostData->IsReturnPost() )
		{
			m_pType->SetImage( "POSTBOX_PAGE_RECEIVE_ITEM_TYPE_RETURN" );
			m_pType->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pType->SetVisibleSingle( FALSE );

			// ��� û�� �϶�
			if ( !m_PostData->m_byChargePayment && m_PostData->m_llCharge )
			{
				m_pType->SetImage( "POSTBOX_PAGE_RECEIVE_ITEM_TYPE_BILLING" );
				m_pType->SetVisibleSingle( TRUE );
			}
			// �ݾ� ÷�� �϶�
			else if ( !m_PostData->m_byMoneyAccept && m_PostData->m_llAttachMoney )
			{
				m_pType->SetImage( "POSTBOX_PAGE_RECEIVE_ITEM_TYPE_MONEY" );
				m_pType->SetVisibleSingle( TRUE );
			}
		}
	}

	// Note : �ؽ�Ʈ ä���
	{ 
        CTime recentSendDate ( m_PostData->GetRecentSendDate() );
        m_pGaeaClient->GetConvertServerTime( recentSendDate );
        CString strTime;
        strTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", recentSendDate.GetYear(), recentSendDate.GetMonth(),
            recentSendDate.GetDay(), recentSendDate.GetHour(), recentSendDate.GetMinute(), recentSendDate.GetSecond());
		m_pText_Date		->SetOneLineText ( strTime,	NS_UITEXTCOLOR::DODGERBLUE );
		m_pText_From		->SetOneLineText ( m_PostData->m_SenderName,			NS_UITEXTCOLOR::DODGERBLUE );

		// Note : ���� ��¥ ���
		{
			DWORD dwDay = m_PostData->GetExpireDay(m_pGaeaClient);
			
			CString strLeftTime;
			strLeftTime.Format( "%d%s", dwDay, ID2GAMEWORD("POSTBOX_EXPIRE_DATE", 0) );
			m_pText_LeftTime->SetOneLineText ( strLeftTime,	NS_UITEXTCOLOR::ORANGE );
		}
	}

	// Note : ���� ��Ʈ�� ������Ʈ
	UpdateVariableControl();
}

void CPostBoxReceiveItem::SetItemImage ( const SNATIVEID& sNativeID )
{
	if ( !m_pItem )
	{
		return;
	}

	m_pItemException->SetVisibleSingle ( FALSE );
	m_pItem			->SetVisibleSingle ( FALSE );

	if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_EMPTY )
	{
		return;
	}

	if ( sNativeID == NATIVEID_NULL() )
	{
		m_pItemException->SetVisibleSingle ( TRUE );
		m_pItemException->SetImage ( "POSTBOX_PAGE_RECEIVE_ITEM_ITEMIMAGE_DEFAULT" );
	}
	else if ( sNativeID != m_pItem->GetItemImageSlotPos() )
	{
		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );

		if ( pItemData )
		{
			m_pItem->SetVisibleSingle ( TRUE );
			// rm #720
			m_pItem->SetItem(m_PostData->m_sAttach01);
			//m_pItem->SetItem( sNativeID );
		}
	}
}

void CPostBoxReceiveItem::SetSelected ( const BOOL bEnable )
{
	BOOL bUpdate = FALSE;

	if ( !m_PostData )
	{
		if ( m_emBoxType != EMRECEIVEITEMBOXTYPE_EMPTY )
		{
			m_emBoxType = EMRECEIVEITEMBOXTYPE_EMPTY;
			bUpdate		= TRUE;
		}

		if ( bUpdate )
		{
			UpdateBox ();
		}

		return;
	}

	// Note : ���� �ʾҴٸ� ���� ���·� ����� �ش� ������ Open �Ѵ�.
	if ( bEnable )
	{
		if ( m_PostData->IsUnRead() )
		{
			m_pGaeaClient->GetPostClient()->ReqOpenPost(m_PostData);
			bUpdate = TRUE;
		}

		if ( m_emBoxType != EMRECEIVEITEMBOXTYPE_SELECTED )
		{
			m_emBoxType = EMRECEIVEITEMBOXTYPE_SELECTED;
			bUpdate		= TRUE;
		}
	}
	else
	{
		if ( m_emBoxType == EMRECEIVEITEMBOXTYPE_SELECTED )
		{
			if ( m_PostData->IsUnRead() )
			{
				m_emBoxType = EMRECEIVEITEMBOXTYPE_NEW;
			}
			else
			{
				m_emBoxType = EMRECEIVEITEMBOXTYPE_DEFAULT;
			}

			bUpdate	= TRUE;
		}
	}

	if ( bUpdate )
	{
		UpdateBox ();
	}
}

const BOOL CPostBoxReceiveItem::IsSelected ()
{
	return (m_emBoxType == EMRECEIVEITEMBOXTYPE_SELECTED);
}

const BOOL CPostBoxReceiveItem::IsChecked ()
{
	if ( !m_pCheck )
	{
		return FALSE;
	}

	return m_pCheck->IsChecked();
}


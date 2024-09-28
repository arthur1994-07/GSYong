#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImageEx.h"
#include "../Util/CheckBox.h"

#include "PostBoxHistoryItem.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxHistoryItem::CPostBoxHistoryItem(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pImage_Type		( NULL )
	, m_pText_Date		( NULL )
	, m_pText_Name		( NULL )
	, m_pText_Info		( NULL )
	, m_pHistory		( NULL )
{
	for ( DWORD i=0; i<STATE_MAX; ++i ) { m_pImage_State[i] = NULL; } 
}

CPostBoxHistoryItem::~CPostBoxHistoryItem()
{
	// Blank
}

void CPostBoxHistoryItem::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// Note : ����
	{
		CreateBackgroundControl ( "POSTBOX_PAGE_HISTORY_ITEM_LINE" );
	}

	// Note : ���� Ÿ��
	{
		m_pImage_Type = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_ITEM_TYPE" );
	}

	// Note : ���� ����
	{
		m_pImage_State[ STATE_CARD  ] = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_ITEM_STATE_CARD"  );
		m_pImage_State[ STATE_ITEM  ] = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_ITEM_STATE_ITEM"  );
		m_pImage_State[ STATE_MONEY ] = CreateSwapImage ( "POSTBOX_PAGE_HISTORY_ITEM_STATE_MONEY" );
	}

	// Note : �ؽ�Ʈ
	{
		m_pText_Date = CreateTextBox ( "POSTBOX_PAGE_HISTORY_ITEM_DATE",  pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
		m_pText_Name = CreateTextBox ( "POSTBOX_PAGE_HISTORY_ITEM_NAME",  pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
		m_pText_Info = CreateTextBox ( "POSTBOX_PAGE_HISTORY_ITEM_INFO",  pFont, TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	}

	Reset ();
}

void CPostBoxHistoryItem::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !m_pHistory )
	{
		return;
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxHistoryItem::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxHistoryItem::Reset ()
{
	m_pHistory = NULL;

	// Note : ������ �ʴ� �͵�
	{
		for ( DWORD i=0; i<STATE_MAX; ++i )
		{
			m_pImage_State[i]->SetVisibleSingle( FALSE );
		}

		m_pImage_Type ->SetVisibleSingle( FALSE );

		m_pText_Date  ->SetVisibleSingle( FALSE );
		m_pText_Name  ->SetVisibleSingle( FALSE );
		m_pText_Info  ->SetVisibleSingle( FALSE );
	}
}

CSwapImage* CPostBoxHistoryItem::CreateSwapImage ( const char* szControl, UIGUID ControlID )
{
	CSwapImage* pSwapImage = new CSwapImage(m_pEngineDevice);
	pSwapImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	RegisterControl ( pSwapImage );

	return pSwapImage;
}

CBasicTextBox* CPostBoxHistoryItem::CreateTextBox ( const char* szControl, CD3DFontPar* pFont, int nAlign, const UIGUID& ControlID )
{
	CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
	pText->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pText->SetFont ( pFont );
	pText->SetTextAlign ( nAlign );	
	RegisterControl ( pText );

	return pText;
}

CBasicLineBox*  CPostBoxHistoryItem::CreateBackgroundControl ( const char* szControl )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( pLineBoxCombo, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE );

	RegisterControl ( pBody );
	RegisterControl ( pLineBoxCombo );

	return pLineBoxCombo;
}

void CPostBoxHistoryItem::SetHistory ( SAPOSTHISTORY* pHistory, const BOOL bForcing )
{
	if ( bForcing )
	{
		m_pHistory = pHistory;
	}
	else
	{
		if ( !m_pHistory || !pHistory )
		{
			m_pHistory = pHistory;
		}
		else if ( m_pHistory != pHistory )
		{
			m_pHistory = pHistory;
		}
		else
		{
			if ( m_pHistory->llPostID == pHistory->llPostID )
			{
				// Note : ������� ���ٸ� ���� �� �ʿ� ����.
				return;
			}

			m_pHistory = pHistory;
		}
	}

	if ( !m_pHistory )
	{
		Reset();
		return;
	}

	// Note : �⺻ ��Ʈ�� Ȱ��ȭ
	{
		m_pImage_Type	->SetVisibleSingle( TRUE );
		m_pText_Date	->SetVisibleSingle( TRUE );
		m_pText_Name	->SetVisibleSingle( TRUE );
		m_pText_Info	->SetVisibleSingle( TRUE );
	}

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	if ( !pCharacter )
	{
		Reset();
		return;
	}

	// Note : ���� Ÿ�� �̹���
	{
		// �ݼ� �� ���� �̶��
		if ( m_pHistory->IsReturnPost() )
		{
			m_pImage_Type->SetImage("POSTBOX_PAGE_HISTORY_ITEM_TYPE_RETURN");
		}
		else
		{
			// ����
			if ( m_pHistory->iRecver == pCharacter->GetCharID() )
			{
				m_pImage_Type->SetImage("POSTBOX_PAGE_HISTORY_ITEM_TYPE_RECEIVE");
			}
			// �߽�
			else
			{
				m_pImage_Type->SetImage("POSTBOX_PAGE_HISTORY_ITEM_TYPE_SEND");
			}
		}
	}

	// Note : ���� ���� �̹���
	{
		for ( DWORD i=0; i<STATE_MAX; ++i )
		{
			m_pImage_State[i]->SetVisibleSingle( FALSE );
		}

		// ī������ �ؽ�Ʈ���� ����
		m_pImage_State[ STATE_CARD ]->SetVisibleSingle( TRUE );

		if ( m_pHistory->iDesign )
		{
			m_pImage_State[ STATE_CARD ]->SetImage( "POSTBOX_PAGE_HISTORY_ITEM_STATE_IMAGE_DESIGN" );
		}
		else
		{
			m_pImage_State[ STATE_CARD ]->SetImage( "POSTBOX_PAGE_HISTORY_ITEM_STATE_IMAGE_TEXT" );
		}

		// ������ ÷�� ����
		switch ( m_pHistory->byPostType )
		{
		case EMPOST_ITEM:
		case EMPOST_MONEYITEM:
			{
				m_pImage_State[ STATE_ITEM ]->SetImage( "POSTBOX_PAGE_HISTORY_ITEM_STATE_IMAGE_ITEM" );
				m_pImage_State[ STATE_ITEM ]->SetVisibleSingle( TRUE );
			}
			break;
		};

		// �ݾ�÷��/���û�� ����
		if ( m_pHistory->llCharge )
		{
			m_pImage_State[ STATE_MONEY ]->SetImage ( "POSTBOX_PAGE_HISTORY_ITEM_STATE_IMAGE_BILLING" );
			m_pImage_State[ STATE_MONEY ]->SetVisibleSingle( TRUE );
		}
		else
		{
			switch ( m_pHistory->byPostType )
			{
			case EMPOST_MONEY:
			case EMPOST_MONEYITEM:
				m_pImage_State[ STATE_MONEY ]->SetImage ( "POSTBOX_PAGE_HISTORY_ITEM_STATE_IMAGE_MONEY" );
				m_pImage_State[ STATE_MONEY ]->SetVisibleSingle( TRUE );
				break;
			};
		}
	}

	// Note : �ؽ�Ʈ ä���
	{
		D3DCOLOR cColorTitle = 0;

		// Note : ����/�߽� �� ����
		if ( m_pHistory->iRecver == pCharacter->GetCharID() )
		{
			cColorTitle = NS_UITEXTCOLOR::GREENYELLOW;
		}
		else
		{
			cColorTitle = NS_UITEXTCOLOR::DODGERBLUE;
		}

		// Note : ��¥
		{
			CTime cSendTime ( m_pHistory->GetRecentSendDate () );
			m_pGaeaClient->GetConvertServerTime( cSendTime );

			CString strTime;
			strTime.Format( "[%02d.%02d.%02d]", cSendTime.GetYear()%100, cSendTime.GetMonth(), cSendTime.GetDay() );
			m_pText_Date->SetOneLineText ( strTime, cColorTitle );
		}

		// Note : �̸�
		{
			// Note : �����̶�� �߽���
			if ( m_pHistory->iRecver == pCharacter->GetCharID() )
			{
				m_pText_Name->SetOneLineText ( m_pHistory->SenderName, cColorTitle );
			}
			else
			{
				m_pText_Name->SetOneLineText ( m_pHistory->RecverName, cColorTitle );
			}
		}

		// Note : ����
		{
			m_pText_Info->ClearText();

			// �ݼۿ����̶��
			if ( m_pHistory->IsReturnPost() )
			{
				if ( m_pHistory->IsDeletePost () )
				{
					// Note : ������ �ݼۿ���
					m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 4 ), NS_UITEXTUTIL::DISABLE );
				}
				else
				{
					// Note : �ݼ� ����
					if ( m_pHistory->iRecver == pCharacter->GetCharID() )
					{
						m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 0 ), NS_UITEXTUTIL::DISABLE );
					}
					// Note : �ݼ� ����
					else
					{
						m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 5 ), NS_UITEXTUTIL::DISABLE );
					}
				}
			}
			else
			{
				// Note : ������ ���¿� ���� ������ �����ش�.
				if ( m_pHistory->iRecver == pCharacter->GetCharID() )
				{
					if ( m_pHistory->IsDeletePost () )
					{
						m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 3 ), NS_UITEXTUTIL::DISABLE );
					}
				}
				else
				{
					if ( m_pHistory->IsUnRead() )
					{
						m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 1 ) );
					}
					else
					{
						m_pText_Info->SetOneLineText( (char*)ID2GAMEWORD( "POSTBOX_PAGE_HISTORY_TEXT", 2 ) );
					}
				}
			}
		}
	}
}

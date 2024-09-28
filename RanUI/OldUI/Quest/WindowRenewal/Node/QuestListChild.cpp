#include "StdAfx.h"

#include "./QuestListChild.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../EngineLib/DeviceMan.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../EngineLib/GUInterface/UITextControl.h"
#include "../QuestWindowRenewal.h"
#include "../List/QuestListRenewal.h"
#include "../../../../InnerInterface.h"
#include "../../QuestHelper.h"

CQuestListChild::CQuestListChild(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
    , m_pText(NULL)
    , m_pCheck(NULL)
    , m_pUnCheck(NULL)
    , m_pBack(NULL)
    , m_pCheckBack(NULL)
    , m_dwID( NATIVEID_NULL().dwID )
    , m_pDone(NULL)
    , m_pFail(NULL)
{
	m_strText = "";
}

CQuestListChild::~CQuestListChild ()
{
}

void CQuestListChild::CreateSubControl ()
{
	{
		m_pBack = new CUIControl(m_pEngineDevice);
		m_pBack->CreateSub( this, "QUEST_WINDOW_LIST_CHILD_BOX_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_BACK );
		RegisterControl ( m_pBack );
		m_pBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_TEXT_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_TEXT );
		pText->SetFont ( pFont9 );
		pText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pText );
		m_pText = pText;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_CHECK_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_CHECK );
		RegisterControl ( pControl );
		m_pCheck = pControl;
		m_pCheck->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_UNCHECK_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_UNCHECK );
		RegisterControl ( pControl );
		m_pUnCheck = pControl;
		m_pUnCheck->SetVisibleSingle( TRUE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_DONE_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_DONE );
		RegisterControl ( pControl );
		m_pDone = pControl;
		m_pDone->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_FAIL_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_FAIL );
		RegisterControl ( pControl );
		m_pFail = pControl;
		m_pFail->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_CHECK_BACK_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_CHILD_CHECK_BACK );
		RegisterControl ( pControl );
		m_pCheckBack = pControl;
	}
}

void CQuestListChild::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CQuestListChild::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_LIST_CHILD_CHECK_BACK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if (m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
				{
                    bool bIsCheck = IsCheck();
                    bool bResult = m_pInterface->GetQuestWindowRenewal()->CheckListAndReturnResult( m_dwID, bIsCheck );

                    if ( bResult )
                    {
                        if ( bIsCheck )
                            Uncheck();
                        else
                            Check();
                    }
				}
			}
		}
		break;

	case QUEST_LIST_CHILD_TEXT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->GetQuestWindowRenewal()->SetQuestListID( m_dwID );
			}
		}
		break;
	}
}

void CQuestListChild::SetID( DWORD dwID, bool bCheck /* = false */, bool bComplete /* = false */ )
{
	m_dwID = dwID;

	if (m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{
		if ( bCheck )
		{
			Check( FALSE );
			m_pDone->SetVisibleSingle( FALSE );
			m_pFail->SetVisibleSingle( FALSE );
		}
		else
		{
			Uncheck();
			m_pDone->SetVisibleSingle( FALSE );
			m_pFail->SetVisibleSingle( FALSE );
		}
	}
	else if (m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
	{
		Uncheck();

		if ( bComplete )
		{
			m_pDone->SetVisibleSingle( TRUE );
			m_pFail->SetVisibleSingle( FALSE );
		}
		else
		{
			m_pDone->SetVisibleSingle( FALSE );
			m_pFail->SetVisibleSingle( TRUE );
		}
	}
}

void CQuestListChild::SetText( CString strText )
{
	m_pText->ClearText();
	m_pText->SetText( strText, NS_UITEXTCOLOR::WHITE );

	m_strText = strText;
}

CString CQuestListChild::GetText()
{
	return m_strText;
}

void CQuestListChild::SetSelect()
{
	if ( m_strText.GetLength() != 0 )
	{
		m_pText->ClearText();
		m_pText->SetText( m_strText, NS_UITEXTCOLOR::ORANGE );
	}

}

void CQuestListChild::UnSelect()
{
	if ( m_strText.GetLength() != 0 )
	{
		m_pText->ClearText();
		m_pText->SetText( m_strText, NS_UITEXTCOLOR::WHITE );
	}
}

bool CQuestListChild::IsCheck()
{
	if ( m_pCheck->IsVisible() )
		return true;
	
	return false;
}

bool CQuestListChild::Check( bool bPrintMsg )
{
	if ( !IsCheck() )
	{
		bool bIsSame = m_pInterface->IS_SAME_QUEST_HELPER( m_dwID );

		if ( !bIsSame )
		{
			bool bSet = m_pInterface->SET_QUEST_HELPER( m_dwID, bPrintMsg );

			if ( bSet )
			{
				m_pCheck->SetVisibleSingle( TRUE );
				m_pUnCheck->SetVisibleSingle( FALSE );

				m_pInterface->GetQuestWindowRenewal()->ShowHelperCount();

				return true;
			}
            else
            {
                // TODO : 체크 못했다.
                m_pInterface->GetQuestWindowRenewal()->CheckListAndReturnResult( m_dwID, true );
            }
		}
		else
		{
			m_pCheck->SetVisibleSingle( TRUE );
			m_pUnCheck->SetVisibleSingle( FALSE );

			m_pInterface->GetQuestWindowRenewal()->ShowHelperCount();

			return true;
		}
	}

	return false;
}

bool CQuestListChild::Uncheck()
{
	if ( IsCheck() )
	{
		m_pCheck->SetVisibleSingle( FALSE );
		m_pUnCheck->SetVisibleSingle( TRUE );

		bool bIsSame = m_pInterface->IS_SAME_QUEST_HELPER( m_dwID );

		if ( bIsSame )
			m_pInterface->RESET_QUEST_HELPER( m_dwID );

		m_pInterface->GetQuestWindowRenewal()->ShowHelperCount();

		return true;
	}

	return false;
}
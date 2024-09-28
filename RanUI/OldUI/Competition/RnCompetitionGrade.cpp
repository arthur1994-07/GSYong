#include "StdAfx.h"

#include "./RnCompetitionGrade.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../InnerInterface.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

RnCompetitionInstanceTypeMain::RnCompetitionInstanceTypeMain(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pText(NULL)
    , m_pOpen(NULL)
    , m_pClose(NULL)
    , m_pOpenOver(NULL)
    , m_pCloseOver(NULL)
    , m_pBack(NULL)
    //, m_dwID( NATIVEID_NULL().dwID )
	, m_strType("")
{
}

RnCompetitionInstanceTypeMain::~RnCompetitionInstanceTypeMain ()
{
}

void RnCompetitionInstanceTypeMain::CreateSubControl ()
{
	{
		m_pBack = new CTipBox(m_pEngineDevice);;
		m_pBack->CreateSub( this, "QUEST_WINDOW_PARENT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, INSTANCE_TYPE_BACK );
		m_pBack->CreateQuestList( "RNCOMPETITION_TYPE_BACK" );
		RegisterControl ( m_pBack );
		m_pBack->SetVisibleSingle( TRUE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);
		pText->CreateSub ( this, "RNCOMPETITION_TYPE_TEXT", UI_FLAG_DEFAULT, INSTANCE_TYPE_TEXT );
		pText->SetFont ( pFont9 );
		pText->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl ( pText );
		m_pText = pText;
		//m_pText->SetUseRender( TRUE );
		//m_pText->SetDiffuse( D3DCOLOR_ARGB( 255, 128, 128, 128 ) );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "SPEC_ACT_GRADE_OPEN", UI_FLAG_DEFAULT, INSTANCE_TYPE_OPEN );
		RegisterControl ( pControl );
		m_pOpen = pControl;
		m_pOpen->SetVisibleSingle( FALSE );
	}

	{   
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "SPEC_ACT_GRADE_CLOSE", UI_FLAG_DEFAULT, INSTANCE_TYPE_CLOSE );
		RegisterControl ( pControl );
		m_pClose = pControl;
		m_pClose->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_PARENT_OPEN_OVER_LUXES", UI_FLAG_DEFAULT, INSTANCE_TYPE_OVER );
		RegisterControl ( pControl );
		m_pOpenOver = pControl;
		m_pOpenOver->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_PARENT_CLOSE_OVER_BOX_LUXES", UI_FLAG_DEFAULT, INSTANCE_TYPE_CLOSE_OVER );
		RegisterControl ( pControl );
		m_pCloseOver = pControl;
		m_pCloseOver->SetVisibleSingle( FALSE );
	}
}

void RnCompetitionInstanceTypeMain::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void RnCompetitionInstanceTypeMain::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( ControlID == INSTANCE_TYPE_BACK )
	{
		if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
		{
			if ( IsOpen() )
			{
				Close();					
			}
			else
			{
				Open();					
			}
		}

        if ( CHECK_MOUSE_IN( dwMsg ) )
        {

            if ( IsOpen() )
            {
                m_pOpen->SetVisibleSingle( FALSE );
                m_pClose->SetVisibleSingle( FALSE );

                m_pOpenOver->SetVisibleSingle( TRUE );
                m_pCloseOver->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pOpen->SetVisibleSingle( FALSE );
                m_pClose->SetVisibleSingle( FALSE );

                m_pOpenOver->SetVisibleSingle( FALSE );
                m_pCloseOver->SetVisibleSingle( TRUE );
            }
        }
        else
        {
            if ( IsOpen() )
            {
                m_pOpen->SetVisibleSingle( TRUE );
                m_pClose->SetVisibleSingle( FALSE );

                m_pOpenOver->SetVisibleSingle( FALSE );
                m_pCloseOver->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pOpen->SetVisibleSingle( FALSE );
                m_pClose->SetVisibleSingle( TRUE );

                m_pOpenOver->SetVisibleSingle( FALSE );
                m_pCloseOver->SetVisibleSingle( FALSE );
            }
        }
	}
}

/*
void ActivityGrade::SetID( DWORD dwID, bool bOpen  )
{
	m_dwID = dwID;

	if ( bOpen )
		Open();
	else
		Close();
}
*/

bool RnCompetitionInstanceTypeMain::IsOpen()
{
	if ( m_pOpen->IsVisible() || m_pOpenOver->IsVisible() )
		return true;

	else if ( m_pClose->IsVisible() || m_pCloseOver->IsVisible() )
		return false;

	return false;
}

void RnCompetitionInstanceTypeMain::Open()
{
	if ( m_pOpenOver->IsVisible() )
	{
		m_pOpen->SetVisibleSingle( FALSE );
		m_pClose->SetVisibleSingle( FALSE );

		m_pOpenOver->SetVisibleSingle( TRUE );
		m_pCloseOver->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pOpen->SetVisibleSingle( TRUE );
		m_pClose->SetVisibleSingle( FALSE );

		m_pOpenOver->SetVisibleSingle( FALSE );
		m_pCloseOver->SetVisibleSingle( FALSE );
	}
}

void RnCompetitionInstanceTypeMain::Close()
{
	if ( m_pCloseOver->IsVisible() )
	{
		m_pOpen->SetVisibleSingle( FALSE );
		m_pClose->SetVisibleSingle( FALSE );

		m_pOpenOver->SetVisibleSingle( FALSE );
		m_pCloseOver->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pOpen->SetVisibleSingle( FALSE );
		m_pClose->SetVisibleSingle( TRUE );

		m_pOpenOver->SetVisibleSingle( FALSE );
		m_pCloseOver->SetVisibleSingle( FALSE );
	}
}

void RnCompetitionInstanceTypeMain::SetText( CString strText )
{
	m_pText->ClearText();
	m_pText->SetText( strText, NS_UITEXTCOLOR::WHITE );
}

void RnCompetitionInstanceTypeMain::MouseOver( bool bOver )
{
	if ( bOver )
	{
		if ( m_pOpen->IsVisible() || m_pOpenOver->IsVisible() )
		{
			m_pOpen->SetVisibleSingle( FALSE );
			m_pClose->SetVisibleSingle( FALSE );

			m_pOpenOver->SetVisibleSingle( TRUE );
			m_pCloseOver->SetVisibleSingle( FALSE );
		}
		else if ( m_pClose->IsVisible() || m_pCloseOver->IsVisible() )
		{
			m_pOpen->SetVisibleSingle( FALSE );
			m_pClose->SetVisibleSingle( FALSE );

			m_pOpenOver->SetVisibleSingle( FALSE );
			m_pCloseOver->SetVisibleSingle( TRUE );
		}
	}
	else
	{
		if ( m_pOpen->IsVisible() || m_pOpenOver->IsVisible() )
		{
			m_pOpen->SetVisibleSingle( TRUE );
			m_pClose->SetVisibleSingle( FALSE );

			m_pOpenOver->SetVisibleSingle( FALSE );
			m_pCloseOver->SetVisibleSingle( FALSE );
		}
		else if ( m_pClose->IsVisible() || m_pCloseOver->IsVisible() )
		{
			m_pOpen->SetVisibleSingle( FALSE );
			m_pClose->SetVisibleSingle( TRUE );

			m_pOpenOver->SetVisibleSingle( FALSE );
			m_pCloseOver->SetVisibleSingle( FALSE );
		}
	}
}
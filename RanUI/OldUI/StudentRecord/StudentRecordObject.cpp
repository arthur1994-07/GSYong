#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../Util/UITextButton.h"

#include "./StudentRecordObject.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStudentRecordObject::CStudentRecordObject ( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowObject ( pInterface, pEngineDevice )
{
    memset(m_pTapButton, 0, sizeof(m_pTapButton));
}

CStudentRecordObject::~CStudentRecordObject ()
{
	// Blank
}

void CStudentRecordObject::CreateSubControlEx ()
{	
	// Note : 페이지 프레임
	{
        CreateLineBox  ( "STUDENTRECORD_OBJECT_PAGE", "STUDENTRECORD_OBJECT_PAGE_LINEBOX_TEXINFO" );
        CreatePageFrame( "STUDENTRECORD_OBJECT_PAGE" );
	}

    // Note : 탭 버튼
    {
        std::string strTapControlFormat("STUDENTRECORD_OBJECT_TAP_%1%");
        UINT        nTapCount = 0;

        CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        if ( GLUseFeatures::GetInstance().IsUsingActivity() )
        {
            std::string strTapControl = sc::string::format( strTapControlFormat, nTapCount++ );

            STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
            pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, STUDENTRECORD_OBJECT_TAP_ACTIVITY );
            pTapButton->CreateSubControl( (char*)ID2GAMEWORD("SPECIAL_ACTIVITY_WINDOW"),
				"STUDENTRECORD_OBJECT_TAP_BASE","STUDENTRECORD_OBJECT_TAP_TEXINFO_ACTIVE",
				"STUDENTRECORD_OBJECT_TAP_BASE","STUDENTRECORD_OBJECT_TAP_TEXINFO_INACTIVE",
				"STUDENTRECORD_OBJECT_TAP_BASE");
            RegisterControl ( pTapButton );
            m_pTapButton[0] = pTapButton;
            TAPSELECTOR::RegisterTapButton( m_pTapButton[0] );
        }

        if ( GLUseFeatures::GetInstance().IsUsingAttendance() )
        {
            std::string strTapControl = sc::string::format( strTapControlFormat, nTapCount++ );

            STAPBUTTON* pTapButton = new STAPBUTTON(m_pEngineDevice);
            pTapButton->CreateSub ( this, strTapControl.c_str(), UI_FLAG_DEFAULT, STUDENTRECORD_OBJECT_TAP_ATTENDANCE );
            pTapButton->CreateSubControl( (char*)ID2GAMEWORD("ATTENDANCE_WINDOW_TITLE") ,
				"STUDENTRECORD_OBJECT_TAP_BASE","STUDENTRECORD_OBJECT_TAP_TEXINFO_ACTIVE",
				"STUDENTRECORD_OBJECT_TAP_BASE","STUDENTRECORD_OBJECT_TAP_TEXINFO_INACTIVE",
				"STUDENTRECORD_OBJECT_TAP_BASE");
            RegisterControl ( pTapButton );
            m_pTapButton[1] = pTapButton;
            TAPSELECTOR::RegisterTapButton( m_pTapButton[1] );
        }
        
        TAPSELECTOR::TapSelect( TAP_DEFAULT );
    }
}

void CStudentRecordObject::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case STUDENTRECORD_OBJECT_TAP_ACTIVITY   :
	case STUDENTRECORD_OBJECT_TAP_ATTENDANCE :
		{
			TAPSELECTOR::EMTAPSELECTORRESULT emResult = TAPSELECTOR::EMTAPSELECTORRESULT_NONE;
			TAPSELECTOR::TapSelectorUIMessage( ControlID, dwMsg, emResult );

			if ( emResult == TAPSELECTOR::EMTAPSELECTORRESULT_SELECT )
			{
				AddMessageEx ( UIMSG_STUDENTRECORD_OBJECT_TAP_CHANGE );
			}
		}
		break;
	}

	CUIWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void CStudentRecordObject::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


// void CStudentRecordObject::TapEnable  ( TAPSELECTOR::BUTTON* pButton )
// {
//     if ( pButton )
//     {
//         pButton->SetEnable( true );
//     }
// }

// void CStudentRecordObject::TapDisable ( TAPSELECTOR::BUTTON* pButton )
// {
//     if ( pButton )
//     {
//         pButton->SetEnable( false );
//     }
// }

// CBasicLineBoxSmart* CStudentRecordObject::CreateLineBox( const std::string& strControl, const std::string& strTexInfo )
// {
//     CBasicLineBoxSmart* pLineBox;
//     pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
//     pLineBox->CreateSub( this, strControl.c_str() );
//     pLineBox->CreateSubControl( strTexInfo.c_str() );
//     RegisterControl( pLineBox );
// 
//     return pLineBox;
// }

//----------------------------------------------------------------------------------------------------//
// 
// void CStudentRecordObject::STAPBUTTON::CreateSubControl ( const std::string& strLabel )
// {
//     CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
// 
//     ACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
//     ACTIVE->CreateSub( this, "STUDENTRECORD_OBJECT_TAP_BASE" );
//     ACTIVE->CreateSubControl( "STUDENTRECORD_OBJECT_TAP_TEXINFO_ACTIVE" );
//     RegisterControl( ACTIVE );
// 
//     INACTIVE = new CBasicLineBoxSmart(m_pEngineDevice);
//     INACTIVE->CreateSub( this, "STUDENTRECORD_OBJECT_TAP_BASE" );
//     INACTIVE->CreateSubControl( "STUDENTRECORD_OBJECT_TAP_TEXINFO_INACTIVE" );
//     RegisterControl( INACTIVE );
// 
//     TEXTBOX = new CBasicTextBox(m_pEngineDevice);
//     TEXTBOX->CreateSub( this, "STUDENTRECORD_OBJECT_TAP_BASE" );
//     TEXTBOX->SetFont( pFont );
//     TEXTBOX->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
//     TEXTBOX->SetText( strLabel.c_str() );
//     TEXTBOX->SetUseTextColor( 0, TRUE );
//     RegisterControl( TEXTBOX );
// }
// 
// void CStudentRecordObject::STAPBUTTON::SetEnable ( const bool bEnable )
// {
//     ENABLE = bEnable;
// 
//     if ( bEnable )
//     {
//         ACTIVE->SetVisibleSingle( TRUE );
//         INACTIVE->SetVisibleSingle( FALSE );
//     }
//     else
//     {
//         ACTIVE->SetVisibleSingle( FALSE );
//         INACTIVE->SetVisibleSingle( TRUE );
//     }
// }

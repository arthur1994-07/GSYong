
// bjju.StudentRecord

#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIInfoCfg_Interface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "../Util/ModalWindow.h"
#include "../Util/UIPageFrame.h"
#include "../Util/UIPage.h"

#include "./StudentRecordWindow.h"
#include "./StudentRecordObject.h"

#include "./ActivityWindowPage.h"
#include "./AttendancePage.h"

#include "./ActivityHelpWindow.h"
#include "./ActivitySalesItemWindow.h"

// ----------------------------------------------------------------------------
#include "./RnAttendancePage/RnAttendancePage.h"
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CStudentRecordWindow::CStudentRecordWindow (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowObjectController (pInterface, pEngineDevice)
    , m_pGaeaClient             (pGaeaClient)
    , m_pUIObject		        ( NULL )
    , m_pOpenModal              ( NULL )
    , m_pPage_Activity          ( NULL )
    , m_pPage_Attendance        ( NULL )
    , m_pActivitySalesItem      ( NULL )
    , m_pActivityHelp           ( NULL )
    , m_bFirst                  (true)
{
}

CStudentRecordWindow::~CStudentRecordWindow ()
{
    // Blank
}

void CStudentRecordWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    // Note : 윈도우 오브젝트
    {
        m_pUIObject = new CStudentRecordObject  ( m_pInterface, m_pEngineDevice );
        m_pUIObject->Create					    ( STUDENTRECORD_WINDOW_OBJECT, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pUIObject->CreateBaseWindowLightGray  ( "STUDENTRECORD_WINDOW_OBJECT", (char*)ID2GAMEWORD("STUDENTRECORD_WINDOW_TITLE") );
        m_pUIObject->CreateSubControl	        ( pd3dDevice );
        m_pUIObject->SetAlignFlag			    ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pUIObject->SetVisibleSingle		    ( FALSE );
        m_pUIObject->SetFlag                    ( CUIWindowObject::FLAG_TITLE_NAME_NONCHANGE );

        WindowRegister( m_pUIObject, STUDENTRECORD_WINDOW_OBJECT );
    }

    CUIPage* pDefaultPage = NULL;
    {
        // Activity
        if ( GLUseFeatures::GetInstance().IsUsingActivity() )
        {
            m_pPage_Activity = new CActivityWindowPage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
            m_pPage_Activity->Create ( "STUDENTRECORD_WINDOW_PAGE_ACTIVITY", NULL, STUDENTRECORD_WINDOW_PAGE_ACTIVITY );
            m_pPage_Activity->AdapterPosition ( "STUDENTRECORD_WINDOW_PAGE_ACTIVITY_ADAPTER_POSITION", true );
            PageRegister( m_pPage_Activity );
            pDefaultPage = pDefaultPage ? pDefaultPage : m_pPage_Activity;

            // Note : 특별활동 도움말
            m_pActivityHelp = new CActivityHelpWindow( m_pGaeaClient, m_pInterface, m_pUIObject, m_pEngineDevice );
            m_pActivityHelp->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE | UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
            m_pActivityHelp->CreateBaseWindowLightGray( "ACTIVITY_HELP_WINDOW", NULL );
            m_pActivityHelp->CreateSubControl();
            m_pActivityHelp->SetTitleName( (char*)ID2GAMEWORD("ACTIVITY_HELP_TEXT_TITLE", 0) );
            m_pActivityHelp->SetVisibleSingle( FALSE );
            RegisterControl( m_pActivityHelp );

            // Note : 특별활동 판매 아이템
            m_pActivitySalesItem = new CActivitySalesItemWindow( m_pGaeaClient, m_pInterface, m_pUIObject, m_pEngineDevice );
            m_pActivitySalesItem->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE | UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
            m_pActivitySalesItem->CreateBaseWindowLightGray( "ACTIVITY_SALESITEM_WINDOW", NULL );
            m_pActivitySalesItem->CreateSubControl();
            m_pActivitySalesItem->SetTitleName( (char*)ID2GAMEWORD("ACTIVITY_SALESITEM_TITLE", 0) );
            m_pActivitySalesItem->SetVisibleSingle( FALSE );
            RegisterControl( m_pActivitySalesItem );
        }

        // Attendance
        if ( GLUseFeatures::GetInstance().IsUsingAttendance() || GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
        {
			if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
			{
				m_pRnattendancePage = new RnattendancePage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
				m_pRnattendancePage->Create ( "STUDENTRECORD_WINDOW_PAGE_ATTENDANCE", NULL, STUDENTRECORD_WINDOW_PAGE_ATTENDANCE );
				//m_pRnattendancePage->CreateSubControl();
				PageRegister( m_pRnattendancePage );
				pDefaultPage = pDefaultPage ? pDefaultPage : m_pRnattendancePage;
			}
			else
			{
				m_pPage_Attendance = new CAttendancePage( m_pGaeaClient, m_pInterface, m_pEngineDevice );
				m_pPage_Attendance->Create ( "STUDENTRECORD_WINDOW_PAGE_ATTENDANCE", NULL, STUDENTRECORD_WINDOW_PAGE_ATTENDANCE );
				PageRegister( m_pPage_Attendance );
				pDefaultPage = pDefaultPage ? pDefaultPage : m_pPage_Attendance;
             }
            
        }
    }

    SetOpenWindow ( STUDENTRECORD_WINDOW_OBJECT );
    SetOpenPage	  ( pDefaultPage );
}

void CStudentRecordWindow::SetVisibleSingle ( BOOL bVisible )
{
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		m_pRnattendancePage->SetSlotLScroolBarUpDate();
		m_pRnattendancePage->ResetCalender();
	}

    CUIWindowObjectController::SetVisibleSingle( bVisible );

    if ( bVisible )
    {

    }
}

void CStudentRecordWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowObjectController::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CStudentRecordWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowObjectController::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case STUDENTRECORD_WINDOW_OBJECT :
        {
            if ( dwMsg&UIMSG_STUDENTRECORD_OBJECT_TAP_CHANGE )
            {
                switch ( m_pUIObject->GetSelectedTapID() )
                {
                case CStudentRecordObject::TAP_ACTIVITY:
                    m_pUIObject->PageOpen( m_pPage_Activity );
                    SetOpenPage	( m_pPage_Activity );
                    break;

                case CStudentRecordObject::TAP_ATTENDANCE:
					{
						if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
						{
							m_pUIObject->PageOpen ( m_pRnattendancePage );
							SetOpenPage ( m_pRnattendancePage );
						}
						else
						{
							m_pUIObject->PageOpen ( m_pPage_Attendance );
							SetOpenPage ( m_pPage_Attendance );
						}
					}
                    break;
                };
            }
        }
        break;
    };
}

void CStudentRecordWindow::UpdateActivity ()
{
    if ( m_pPage_Activity )
    {
        m_pPage_Activity->UpdateActivity ();
    }
}

void CStudentRecordWindow::UpdateActivityPoint ()
{
    if ( m_pPage_Activity )
    {
        m_pPage_Activity->UpdateActivityPoint ();
    }
}

void CStudentRecordWindow::UpdateAttendance ()
{
    if ( m_pPage_Attendance )
    {
        m_pPage_Attendance->UpdateContents ();
        m_pPage_Attendance->UpdateDayList  ();
        m_pPage_Attendance->UpdateCalendar ();
    }
}

void CStudentRecordWindow::UpdateAttendanceTask ()
{
    if ( m_pPage_Attendance )
    {
        m_pPage_Attendance->UpdateContents ();
    }
}

void CStudentRecordWindow::UnlockAttendanceUI()
{
	if ( m_pPage_Attendance )
	{
		m_pPage_Attendance->UnlockUI();
	}
}

const bool CStudentRecordWindow::IsOpenable ()
{
    if ( IsOpenableActivityPage() || IsOpenableAttendancePage() )
    {
        return true;
    }

    return false;
}

const bool CStudentRecordWindow::IsOpenableActivityPage ()
{
    if ( m_pPage_Activity && GLUseFeatures::GetInstance().IsUsingActivity() )
    {
        return true;
    }

    return false;
}

const bool CStudentRecordWindow::IsOpenableAttendancePage ()
{
    if ( ( m_pPage_Attendance && GLUseFeatures::GetInstance().IsUsingAttendance() ) ||
		 ( m_pRnattendancePage && GLUseFeatures::GetInstance().IsUsingRnattendancePage() ) )
    {
        return true;
    }

    return false;
}

const bool CStudentRecordWindow::IsOpenActivityPage ()
{
    if ( m_pPage_Activity && IsPageOpen( m_pPage_Activity ) )
    {
        return true;
    }

    return false;
}

const bool CStudentRecordWindow::IsOpenAttendancePage ()
{
    if ( m_pPage_Attendance && IsPageOpen( m_pPage_Attendance ) )
    {
        return true;
    }

    return false;
}

void CStudentRecordWindow::SetActivityProgressDisplay( const bool bEnable )
{
    if ( m_pPage_Activity )
    {
        m_pPage_Activity->SetProgressDisplay( bEnable );
    }
}

const bool CStudentRecordWindow::IsActivityProgressDisplay ()
{
    if ( m_pPage_Activity )
    {
        return m_pPage_Activity->IsProgressDisplay();
    }

    return false;
}

const bool CStudentRecordWindow::IsOpenModalWindow ()
{
    if ( m_pOpenModal && m_pOpenModal->IsVisible() )
    {
        return true;
    }

    return false;
}

void CStudentRecordWindow::CloseModalWindow ()
{
    if ( m_pOpenModal && m_pOpenModal->IsVisible() )
    {
        m_pOpenModal->SetVisibleSingle( FALSE );
        m_pOpenModal = NULL;
    }
}

void CStudentRecordWindow::OpenAttendanceWindow ()
{
    if ( !IsOpenableAttendancePage() )
    {
        return;
    }

    m_pUIObject->SelectTapAttendance();

    if ( IsVisible() )
    {
		if (GLUseFeatures::GetInstance().IsUsingRnattendancePage())
			 WindowPageOpen( CStudentRecordWindow::STUDENTRECORD_WINDOW_OBJECT, GetRnattendancePage());
		else
			WindowPageOpen( CStudentRecordWindow::STUDENTRECORD_WINDOW_OBJECT, GetAttendancePage() );
    }
    else
    {
        if ( m_bFirst )
        {
			if (GLUseFeatures::GetInstance().IsUsingRnattendancePage())
				SetOpenPage( GetRnattendancePage() );
			else
				SetOpenPage( GetAttendancePage() );
            m_pInterface->UiShowGroupFocus( STUDENTRECORD_WINDOW );
        }
    }

    m_bFirst = false;
}

HRESULT CStudentRecordWindow::InfoFileLoad ()
{
    using namespace NSUIINFOCFG;
    NSUIINFOCFG::CONTROL sInfoControl = NSUIINFOCFG::LOAD_CONTROL( "STUDENTRECORD_WINDOW" );

    if ( !sInfoControl.Empty() )
    {
        bool bRet = false;

        //////////////////////////////////////////////////////////////////////
        BOOL bFlag = FALSE;
        bRet = sInfoControl.GetValue( "ACTIVITY_PROGRESS_DISPLAY", bFlag );
        SetActivityProgressDisplay( bFlag ? true : false );
    }

    return S_OK;
}

HRESULT CStudentRecordWindow::InfoFileSave ()
{
    using namespace NSUIINFOCFG;
    NSUIINFOCFG::CONTROL sInfoControl = NSUIINFOCFG::SAVE_CONTROL( "STUDENTRECORD_WINDOW" );

    if ( !sInfoControl.Empty() )
    {
        bool bRet = false;

        //////////////////////////////////////////////////////////////////////
        BOOL bFlag = IsActivityProgressDisplay() ? TRUE : FALSE;
        bRet = sInfoControl.SetValue( "ACTIVITY_PROGRESS_DISPLAY", bFlag );
    }

    return S_OK;
}

CUIControl* CStudentRecordWindow::CreateBackgroundControl ( char* szControlKeyword )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub ( this, szControlKeyword, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    RegisterControl ( pControl );

    return pControl;
}

void CStudentRecordWindow::RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE */ )
{
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() && m_pRnattendancePage )
	{
		m_pRnattendancePage->SetDayInfoText( strEventText, TextColor );
	}
}

void CStudentRecordWindow::RnAttendanceClerText()
{
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() && m_pRnattendancePage )
	{
		m_pRnattendancePage->ClearDayInfo();
	}
}

void CStudentRecordWindow::REFRESH_RnattendancePage()
{
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() && m_pRnattendancePage )
	{
				//m_pRnattendancePage->ResetCalender();
		m_pRnattendancePage->SetSlotLScroolBarUpDate();
		m_pRnattendancePage->ResetCalender();
	}
}

//----------------------------------------------------------------------------------------------------//

MyStudentRecordWindow::MyStudentRecordWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CStudentRecordWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyStudentRecordWindow::CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice )
{
    CStudentRecordWindow::Create( STUDENTRECORD_WINDOW, "STUDENTRECORD_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CStudentRecordWindow::CreateSubControl( pd3dDevice );
    CStudentRecordWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );
    CStudentRecordWindow::m_pInterface->UiRegisterControl( this, true );
    CStudentRecordWindow::m_pInterface->UiShowGroupFocus( STUDENTRECORD_WINDOW );
}

void MyStudentRecordWindow::UpdateActivity ()
{
    CStudentRecordWindow::UpdateActivity ();
}

void MyStudentRecordWindow::UpdateActivityPoint ()
{
    CStudentRecordWindow::UpdateActivityPoint ();
}

void MyStudentRecordWindow::UpdateAttendance ()
{
    CStudentRecordWindow::UpdateAttendance ();
}

void MyStudentRecordWindow::UpdateAttendanceTask ()
{
    CStudentRecordWindow::UpdateAttendanceTask ();
}

void MyStudentRecordWindow::UnlockAttendanceUI()
{
    CStudentRecordWindow::UnlockAttendanceUI();
}

const bool MyStudentRecordWindow::IsOpenable ()
{
    return CStudentRecordWindow::IsOpenable ();
}

const bool MyStudentRecordWindow::IsOpenableActivityPage ()
{
    return CStudentRecordWindow::IsOpenableActivityPage();
}

const bool MyStudentRecordWindow::IsOpenableAttendancePage ()
{
    return CStudentRecordWindow::IsOpenableAttendancePage();
}

const bool MyStudentRecordWindow::IsOpenActivityPage ()
{
    return CStudentRecordWindow::IsOpenActivityPage ();
}

const bool MyStudentRecordWindow::IsOpenAttendancePage ()
{
    return CStudentRecordWindow::IsOpenAttendancePage ();
}

const bool MyStudentRecordWindow::IsActivityProgressDisplay ()
{
    return CStudentRecordWindow::IsActivityProgressDisplay ();
}

const bool MyStudentRecordWindow::IsOpenModalWindow()
{
    return CStudentRecordWindow::IsOpenModalWindow();
}

void MyStudentRecordWindow::CloseModalWindow()
{
    CStudentRecordWindow::CloseModalWindow();
}

void MyStudentRecordWindow::OpenAttendanceWindow ()
{
    CStudentRecordWindow::OpenAttendanceWindow();
}

void MyStudentRecordWindow::RnAttendanceSetDayInfoText( const CString strEventText, const D3DCOLOR& TextColor /*= NS_UITEXTUTIL::ENABLE */ )
{
	CStudentRecordWindow::RnAttendanceSetDayInfoText( strEventText, TextColor );
}

void MyStudentRecordWindow::RnAttendanceClerText()
{
	CStudentRecordWindow::RnAttendanceClerText();
}

void MyStudentRecordWindow::REFRESH_RnattendancePage()
{
	CStudentRecordWindow::REFRESH_RnattendancePage();
}
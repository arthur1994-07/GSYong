#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogic/Attendance/TaskN.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "./AttendanceTask.h"
#include "./AttendanceAward.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CAttendanceTask::CAttendanceTask( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient    ( pGaeaClient )
    , m_pInterface     ( pInterface  )
    , m_pLine          ( NULL )
    , m_pButton        ( NULL )
    , m_pAward         ( NULL )
    , m_pTaskCaption   ( NULL )
    , m_pTaskProgress  ( NULL )
    , m_pAwardReceived ( NULL )
    , m_nIndex         ( 0 )
    , m_bReceived      ( false )
    , m_emState        ( EMSTATE_DISABLE )
{
    memset( m_pTitle, 0, sizeof(m_pTitle) );
}

CAttendanceTask::~CAttendanceTask ()
{
    //Blank
}

void CAttendanceTask::CreateSubControl()
{
    // 타이틀
    {
        CUIControl* pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TITLE_HEAD" );
        RegisterControl( pControl );
        m_pTitle[TITLE_HEAD] = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TITLE_BODY" );
        RegisterControl( pControl );
        m_pTitle[TITLE_BODY] = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TITLE_TAIL" );
        RegisterControl( pControl );
        m_pTitle[TITLE_TAIL] = pControl;
    }

    // 라인
    {
        CUIControl* pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_LINE" );
        pControl->SetUseRender( TRUE );
        pControl->SetDiffuse( COLOR_LINE );
        RegisterControl( pControl );
        m_pLine = pControl;
    }

    // Task 제목
    {
        CD3DFontPar* pFont        = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
        CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TEXT_CAPTION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont_Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOverColor( 0, NS_UITEXTCOLOR::DIMGREY );
        RegisterControl( pTextBox );
        m_pTaskCaption = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TEXT_CAPTION", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetUseOverColor( TRUE );
        pTextBox->SetOverColor( 0, D3DCOLOR_ARGB(255, 180, 180, 190) );
        pTextBox->SetOneLineText("");
        RegisterControl( pTextBox );
        m_pTaskCaption_Disable = pTextBox;
    }

    // Task 내용
    {
        CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_TEXT_PROGRESS", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOneLineText("");
        RegisterControl( pTextBox );
        m_pTaskProgress = pTextBox;
    }

    // 보상
    {
        CAttendanceAward* pAward = new CAttendanceAward( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pAward->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_AWARD" );
        pAward->CreateSubControl ();
        RegisterControl( pAward );
        m_pAward = pAward;
    }

    // 보상버튼
    {
        CD3DFontPar* pFontShadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
        CD3DFontPar* pFont       = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        SBUTTON* pControl;
        pControl = new SBUTTON(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON", UI_FLAG_DEFAULT, ATTENDANCE_TASK_BUTTON );
        pControl->CreateSubControl( pFontShadow, ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 0 ) );
        RegisterControl( pControl );
        m_pButton = pControl;

        // 보상받음 텍스트
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        pTextBox->SetOneLineText( ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 1 ), NS_UITEXTCOLOR::ATTENDANCE_TASK_AWARD_ReceiveD );
        RegisterControl( pTextBox );
        m_pAwardReceived = pTextBox;
    }
}

void CAttendanceTask::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CAttendanceTask::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case ATTENDANCE_TASK_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pGaeaClient->ReqAttendanceTakeReward( m_nIndex );
            }
        }
        break;
    }
    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CAttendanceTask::Update ( TaskN* pTask, const UINT nIndex, const bool bLock, bool bLockUI )
{
    if ( !pTask )
    {
        SetState ( EMSTATE_EMPTY );
        return;
    }

    m_nIndex = nIndex;

    std::string strCaption1 = m_pGaeaClient->AttendanceUIGetStringCaption_1( pTask );
    std::string strCaption2 = m_pGaeaClient->AttendanceUIGetStringCaption_2( pTask );
    std::string strProgress = m_pGaeaClient->AttendanceUIGetStringProgress ( pTask );

    // Task 캡션 스트링
    m_pTaskCaption->ClearText();
    m_pTaskCaption->SetOneLineText( sc::string::format( ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 2 ), m_nIndex+1).c_str(), NS_UITEXTCOLOR::ORANGE );
    m_pTaskCaption->AddString( 0, " " );
    m_pTaskCaption->AddString( 0, strCaption1.c_str(), NS_UITEXTCOLOR::WHITE );
    m_pTaskCaption->AddString( 0, strCaption2.c_str(), NS_UITEXTCOLOR::LIGHTSKYBLUE );

    m_pTaskCaption_Disable->ClearText();
    m_pTaskCaption_Disable->SetOneLineText( sc::string::format( ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 2 ), m_nIndex+1).c_str(), NS_UITEXTCOLOR::ORANGE );
    m_pTaskCaption_Disable->AddString( 0, " " );
    m_pTaskCaption_Disable->AddString( 0, strCaption1.c_str(), NS_UITEXTCOLOR::WHITE );
    m_pTaskCaption_Disable->AddString( 0, strCaption2.c_str(), NS_UITEXTCOLOR::LIGHTSKYBLUE );

    // Task 진행 스트링
    m_pTaskProgress->ClearText();
    m_pTaskProgress->SetOneLineText( strProgress.c_str() );

    // 보상 받음 여부
    m_bReceived = pTask->IsTookRewards() ? true : false;

    // Award 업데이트
    m_pAward->Update ( pTask );

    // 상태 업데이트
    if ( bLock )
    {
        SetState ( EMSTATE_DISABLE );
        return;
    }
	else if( bLockUI )
	{
		SetState ( EMSTATE_LOCKUI );
		return;
	}

    switch ( pTask->GetStatus() )
    {
    case ATTENDANCE_STATUS_INPROGRESS : SetState( EMSTATE_ENABLE );     break;
    case ATTENDANCE_STATUS_COMPLETED  : SetState( EMSTATE_COMPLETE );   break;
    };
}

void CAttendanceTask::SetState ( const EMSTATE emState )
{
    m_emState = emState;
    UpdateState ();
}

void CAttendanceTask::UpdateState ()
{
    SetVisibleSingle( TRUE );

    // 상태에 따른 업데이트
    switch ( m_emState )
    {
    case EMSTATE_ENABLE:
        {
            m_pLine->SetVisibleSingle( FALSE );
            m_pButton->SetVisibleSingle( FALSE );
            m_pAwardReceived->SetVisibleSingle( FALSE );

            m_pTaskCaption->SetVisibleSingle( TRUE  );
            m_pTaskCaption_Disable->SetVisibleSingle( FALSE );

            m_pTaskProgress->SetTextColor    ( 0, NS_UITEXTCOLOR::WHITE );
            m_pTaskProgress->SetUseTextColor ( 0, TRUE );

            m_pAward->SetState( CAttendanceAward::EMSTATE_ENABLE );
        }
        break;

    case EMSTATE_COMPLETE:
        {
            m_pLine->SetVisibleSingle( TRUE );
            m_pTaskCaption->SetVisibleSingle( TRUE  );
            m_pTaskCaption_Disable->SetVisibleSingle( FALSE );

            m_pTaskProgress->SetTextColor    ( 0, NS_UITEXTCOLOR::LIGHTSKYBLUE );
            m_pTaskProgress->SetUseTextColor ( 0, TRUE );

            if ( m_bReceived )
            {
                m_pButton->SetVisibleSingle( FALSE );
                m_pAwardReceived->SetVisibleSingle( TRUE );
                m_pAward->SetState( CAttendanceAward::EMSTATE_COMPLETE );
            }
            else
            {
                m_pButton->SetVisibleSingle( TRUE );
                m_pAwardReceived->SetVisibleSingle( FALSE );
                m_pAward->SetState( CAttendanceAward::EMSTATE_ENABLE );
            }
        }
        break;

    case EMSTATE_DISABLE:
        {
            m_pLine->SetVisibleSingle( FALSE );
            m_pButton->SetVisibleSingle( FALSE );
            m_pAwardReceived->SetVisibleSingle( FALSE );

            m_pTaskCaption->SetVisibleSingle( FALSE );
            m_pTaskCaption_Disable->SetVisibleSingle( TRUE );

            m_pTaskProgress->SetOneLineText( sc::string::format( ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 3 ), 1 ).c_str() , NS_UITEXTCOLOR::ATTENDANCE_TASK_PROGRESS_DISABLE );

            m_pAward->SetState( CAttendanceAward::EMSTATE_DISABLE );
        }
        break;

    case EMSTATE_EMPTY:
        {
            SetVisibleSingle( FALSE );
            m_pAward->SetState( CAttendanceAward::EMSTATE_EMPTY );
        }
        break;
	case EMSTATE_LOCKUI:
        {
            m_pLine->SetVisibleSingle( FALSE );
            m_pButton->SetVisibleSingle( FALSE );
            m_pAwardReceived->SetVisibleSingle( FALSE );

            m_pTaskCaption->SetVisibleSingle( FALSE );
            m_pTaskCaption_Disable->SetVisibleSingle( TRUE );

			m_pTaskProgress->SetOneLineText( ID2GAMEWORD( "ATTENDANCE_TASK_TEXT", 4 ), NS_UITEXTCOLOR::ATTENDANCE_TASK_PROGRESS_DISABLE );

            m_pAward->SetState( CAttendanceAward::EMSTATE_DISABLE );
        }
		break;
    };
}

CBasicLineBoxSmart* CAttendanceTask::CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID )
{
    CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
    pLineBox->CreateSub( this, strControlName.c_str(), UI_FLAG_DEFAULT, cID );
    pLineBox->CreateSubControl( strTexInfo.c_str() );
    RegisterControl( pLineBox );
    return pLineBox;
}

//-----------------------------------------------------------------------------------------------//

void CAttendanceTask::SBUTTON::CreateSubControl( CD3DFontPar* pFont, const std::string& strText )
{
    std::string strParent = GetControlNameEx();

    CUIControl ParentControl(m_pEngineDevice); 
    ParentControl.Create ( 1, strParent.c_str() );
    const UIRECT& rcParentLocal = ParentControl.GetLocalPos();

    {
        CUIControl* pControl;
        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON_HEAD" );
        RegisterControl( pControl );
        pHEAD = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON_BODY" );
        RegisterControl( pControl );
        pBODY = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON_TAIL" );
        RegisterControl( pControl );
        pTAIL = pControl;

        const UIRECT& rcHeadLocalPos = pHEAD->GetLocalPos();
        const UIRECT& rcBodyLocalPos = pBODY->GetLocalPos();
        const UIRECT& rcTailLocalPos = pTAIL->GetLocalPos();

        pBODY->SetLocalPos( UIRECT( rcHeadLocalPos.sizeX, rcBodyLocalPos.top, rcParentLocal.sizeX - (rcHeadLocalPos.sizeX + rcTailLocalPos.sizeX), rcBodyLocalPos.sizeY ) );
        pTAIL->SetLocalPos( UIRECT( rcParentLocal.sizeX-rcTailLocalPos.sizeX, rcTailLocalPos.top, rcTailLocalPos.sizeX, rcTailLocalPos.sizeY ) );
    }

    pTEXT = new CBasicTextBox(m_pEngineDevice);
    pTEXT->CreateSub( this, "ATTENDANCE_PAGE_CONTENTS_TASK_BUTTON_TEXT", UI_FLAG_DEFAULT, ID_TEXT_BUTTON );
    pTEXT->SetFont( pFont );
    pTEXT->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    pTEXT->SetText( strText.c_str() );
    pTEXT->SetUseTextColor( 0, TRUE );
    RegisterControl( pTEXT );

    Release ();
}

void CAttendanceTask::SBUTTON::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case ID_TEXT_BUTTON:
        {
            if ( dwMsg&UIMSG_LB_UP )
            {
                if ( bKEYDOWN && CHECK_MOUSE_IN ( dwMsg ) )
                {
                    AddMessageEx( UIMSG_TASKBUTTON_PUSHUP );
                }

                bKEYDOWN = FALSE;
                Release ();
            }
            else if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&UIMSG_LB_DOWN )
                {
                    bKEYDOWN = TRUE;
                    Push ();
                }
                else if ( bKEYDOWN && dwMsg&(UIMSG_LB_PRESEED|UIMSG_LB_DRAG) )
                {
                    Push ();
                }
                else
                {
                    bKEYDOWN = FALSE;
                    Release ();
                }
            }
            else
            {
                Release ();
            }
        }
        break;
    };
}

void CAttendanceTask::SBUTTON::Push ()
{
    pHEAD->SetDiffuse( COLOR_BUTTON_PUSH );
    pBODY->SetDiffuse( COLOR_BUTTON_PUSH );
    pTAIL->SetDiffuse( COLOR_BUTTON_PUSH );

    pTEXT->SetTextColor( 0, COLOR_TEXT_PUSH );
}

void CAttendanceTask::SBUTTON::Release ()
{
    pHEAD->SetDiffuse( COLOR_BUTTON_RELEASE );
    pBODY->SetDiffuse( COLOR_BUTTON_RELEASE );
    pTAIL->SetDiffuse( COLOR_BUTTON_RELEASE );

    pTEXT->SetTextColor( 0, COLOR_TEXT_RELEASE );
}

#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include ".\skilltraytab.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UIInfoCfg_Interface.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "BasicQuickSkillSlot.h"
#include "BasicSkillTray.h"
#include "BasicLevelDisplay.h"
#include "../Util/UITextButton.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

INT CSkillTrayTab::NEXT_INDEX[] = {1, 2, 3, 0};

const FLOAT CSkillTrayTab::fTURN_RECT_GAP_DISTANCE = 50.0f;
const FLOAT CSkillTrayTab::fTURN_RECT_SIZE_RATIO   = 0.3f;
const FLOAT CSkillTrayTab::fTURN_RECT_SIZE_BORDER  = 50.0f;

CSkillTrayTab::CSkillTrayTab(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pCurrentSkillSlot(NULL)
    , m_pTurnButton(NULL)
    , m_pOpenButton(NULL)
    , m_pCloseButton(NULL)
    , m_pLockButtonOpenUnlock(NULL)
    , m_pLockButtonCloseUnlock(NULL)
    , m_pLockButtonOpenUnlockH(NULL)
    , m_pLockButtonCloseUnlockH(NULL)
    , m_pLockButtonOpenLock(NULL)
    , m_pLockButtonCloseLock(NULL)
    , m_pLockButtonOpenLockH(NULL)
    , m_pLockButtonCloseLockH(NULL)
    , m_pGap0(NULL)
    , m_pGap1(NULL)
    , m_pGap2(NULL)
	, m_pQuickSkillTrayMoveLockButtonOpen(NULL)
	, m_pQuickSkillTrayMoveLockButtonOpenH(NULL)
	, m_pQuickSkillTrayMoveLockButtonClose(NULL)
	, m_pQuickSkillTrayMoveLockButtonCloseH(NULL)
	, m_nTabIndex(0)
	, m_nCount(0)
    , m_bClose(FALSE)
    , m_bDrag(TRUE)
    , m_bTurnHorizontal(FALSE)
    , m_bMoveLock(false)
{
	memset( m_pSkillTray, 0, sizeof( CBasicSkillTray* ) * MAX_TAB_INDEX );
	memset( m_pTextButton, 0, sizeof( CBasicTextButton* ) * MAX_TAB_INDEX );
}

CSkillTrayTab::~CSkillTrayTab(void)
{
}

void CSkillTrayTab::CreateSubControl()
{
	CD3DFontPar * m_pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

/* 
#ifdef CH_PARAM // 중국 인터페이스 변경
	CUIControl* pTrayLeft = new CUIControl(m_pRenderDevice);
	pTrayLeft->CreateSub( this, "BASIC_QUICK_SKILL_TRAY_LEFT" );
	RegisterControl( pTrayLeft );

	CBasicLevelDisplay* pLevelDisplay = new CBasicLevelDisplay;
	pLevelDisplay->CreateSub ( this, "BASIC_LEVEL_DISPLAY", UI_FLAG_DEFAULT, BASIC_LEVEL_DISPLAY );
	pLevelDisplay->CreateSubControl ();
	RegisterControl ( pLevelDisplay );
#endif
    */

    CBasicQuickSkillSlot * pSkillSlot = new CBasicQuickSkillSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillSlot->CreateSub( this, "BASIC_QUICK_SKILL_SLOT", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_SLOT );
    pSkillSlot->CreateSubControl();
    pSkillSlot->SetUseSkillImageTwinkle( false ); // 스킬 종료시 깜빡이는 액션을 사용하지 않습니다.
    RegisterControl( pSkillSlot );
    m_pCurrentSkillSlot = pSkillSlot;

    CUITextButton* pTextButton = new CUITextButton(m_pEngineDevice);
    pTextButton->CreateSub ( this, "QUICK_SKILL_TRAY_TAB_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F1 );
    pTextButton->CreateSubControl( "QUICK_SKILL_TRAY_TAB_BUTTON_DEFAULT", "QUICK_SKILL_TRAY_TAB_BUTTON_CLICK", m_pFont8 );
    pTextButton->SetOneLineText( ID2GAMEWORD( "SKILL_TAB_INFO", 0 ) );
    RegisterControl( pTextButton );
    SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );
    m_pTextButton[0] = pTextButton;

    pTextButton = new CUITextButton(m_pEngineDevice);
    pTextButton->CreateSub ( this, "QUICK_SKILL_TRAY_TAB_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F2 );
    pTextButton->CreateSubControl( "QUICK_SKILL_TRAY_TAB_BUTTON_DEFAULT", "QUICK_SKILL_TRAY_TAB_BUTTON_CLICK", m_pFont8 );
    pTextButton->SetOneLineText( ID2GAMEWORD( "SKILL_TAB_INFO", 1 ) );
    pTextButton->SetVisibleSingle( FALSE );
    RegisterControl( pTextButton );
    SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );
    m_pTextButton[1] = pTextButton;

    pTextButton = new CUITextButton(m_pEngineDevice);
    pTextButton->CreateSub ( this, "QUICK_SKILL_TRAY_TAB_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F3 );
    pTextButton->CreateSubControl( "QUICK_SKILL_TRAY_TAB_BUTTON_DEFAULT", "QUICK_SKILL_TRAY_TAB_BUTTON_CLICK", m_pFont8 );
    pTextButton->SetOneLineText( ID2GAMEWORD( "SKILL_TAB_INFO", 2 ) );
    pTextButton->SetVisibleSingle( FALSE );
    RegisterControl( pTextButton );
    SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );
    m_pTextButton[2] = pTextButton;

    pTextButton = new CUITextButton(m_pEngineDevice);
    pTextButton->CreateSub ( this, "QUICK_SKILL_TRAY_TAB_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F4 );
    pTextButton->CreateSubControl( "QUICK_SKILL_TRAY_TAB_BUTTON_DEFAULT", "QUICK_SKILL_TRAY_TAB_BUTTON_CLICK", m_pFont8 );
    pTextButton->SetOneLineText( ID2GAMEWORD( "SKILL_TAB_INFO", 3 ) );
    pTextButton->SetVisibleSingle( FALSE );
    RegisterControl( pTextButton );
    SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );
    m_pTextButton[3] = pTextButton;

    /*
	CBasicTextButton * pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F1 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP,  );
	RegisterControl( pTextButton );
	m_pTextButton[0] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F2 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 1 ) );
	pTextButton->SetVisibleSingle( FALSE );
	RegisterControl( pTextButton );
	m_pTextButton[1] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F3 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 2 ) );
	pTextButton->SetVisibleSingle( FALSE );
	RegisterControl( pTextButton );
	m_pTextButton[2] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );

	pTextButton = new CBasicTextButton(m_pRenderDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE | UI_FLAG_YSIZE, BASIC_TEXT_BUTTON_F4 );
	pTextButton->CreateBaseButton( "QUICK_SKILL_TRAY_TAB_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SKILL_TAB_INFO", 3 ) );
	pTextButton->SetVisibleSingle( FALSE );
	RegisterControl( pTextButton );
	m_pTextButton[3] = pTextButton;
	SetFoldGroup( pTextButton, TABBUTTON_FOLDGROUP_ID );
    */

    CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
    pButton->CreateSub( this, "QUICK_SKILL_TRAY_TURN_BUTTON", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_TURN_BUTTON );
    pButton->CreateFlip( "QUICK_SKILL_TRAY_TURN_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
    pButton->SetUseGlobalAction( TRUE );	
    RegisterControl( pButton );
    m_pTurnButton  = pButton;

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_GAP_0" );
        RegisterControl( pControl );
        m_pGap0 = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_GAP_1" );
        RegisterControl( pControl );
        m_pGap1 = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_GAP_2" );
        RegisterControl( pControl );
        m_pGap2 = pControl;
    }

    {
        CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_UNLOCK", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_UNLOCK_F", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonOpenUnlock = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_LOCK", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_LOCK_F", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonOpenLock = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_UNLOCK_H", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_UNLOCK_F_H", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonOpenUnlockH = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_LOCK_H", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_LOCK_F_H", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonOpenLockH = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_UNLOCK", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_UNLOCK_F", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonCloseUnlock = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_LOCK", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_LOCK_F", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonCloseLock = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_UNLOCK_H", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_UNLOCK_F_H", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonCloseUnlockH = pButton;

        pButton = new CBasicButton(m_pEngineDevice);
        pButton->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_LOCK_H", UI_FLAG_DEFAULT );
        pButton->CreateFlip( "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_LOCK_F_H", CBasicButton::MOUSEIN_FLIP );
        pButton->SetUseGlobalAction( TRUE );	
        RegisterControl( pButton );
        m_pLockButtonCloseLockH = pButton;

        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_BOX", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN );
        RegisterControl( pControl );
		m_pQuickSkillTrayMoveLockButtonOpen = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_BOX", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE );
        RegisterControl( pControl );
		pControl->SetVisibleSingle(FALSE);
		m_pQuickSkillTrayMoveLockButtonClose = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_OPEN_BOX_H", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN_H );
        RegisterControl( pControl );
		m_pQuickSkillTrayMoveLockButtonOpenH = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "QUICK_SKILL_TRAY_LOCK_BUTTON_CLOSE_BOX_H", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE_H );
        RegisterControl( pControl );
		pControl->SetVisibleSingle(FALSE);
		m_pQuickSkillTrayMoveLockButtonCloseH = pControl;
    }

// #ifndef CH_PARAM // 중국 인터페이스 변경
    pButton = new CBasicButton(m_pEngineDevice);
    pButton->CreateSub ( this, "QUICK_SKILL_TRAY_OPEN_BUTTON", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_OPEN_BUTTON );
    pButton->CreateFlip ( "QUICK_SKILL_TRAY_OPEN_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
    pButton->SetUseGlobalAction ( TRUE );
    pButton->SetVisibleSingle( FALSE );
    RegisterControl ( pButton );
    m_pOpenButton = pButton;

	pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, "QUICK_SKILL_TRAY_CLOSE_BUTTON", UI_FLAG_DEFAULT, QUICK_SKILL_TRAY_CLOSE_BUTTON );
	pButton->CreateFlip( "QUICK_SKILL_TRAY_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction( TRUE );	
    RegisterControl( pButton );
    m_pCloseButton  = pButton;

// #endif

    CBasicSkillTray * pSkillTray = new CBasicSkillTray(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F1 );
    pSkillTray->CreateSubControl( 0 );
    RegisterControl( pSkillTray );
    m_pSkillTray[0] = pSkillTray;

    pSkillTray = new CBasicSkillTray(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F2 );
    pSkillTray->CreateSubControl( 1 );
    pSkillTray->SetVisibleSingle( FALSE );
    RegisterControl( pSkillTray );
    m_pSkillTray[1] = pSkillTray;

    pSkillTray = new CBasicSkillTray(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F3 );
    pSkillTray->CreateSubControl( 2 );
    pSkillTray->SetVisibleSingle( FALSE );
    RegisterControl( pSkillTray );
    m_pSkillTray[2] = pSkillTray;

    pSkillTray = new CBasicSkillTray(m_pGaeaClient, m_pInterface, m_pEngineDevice);
    pSkillTray->CreateSub( this, "BASIC_QUICK_SKILL_TRAY", UI_FLAG_DEFAULT, BASIC_QUICK_SKILL_TRAY_F4 );
    pSkillTray->CreateSubControl( 3 );
    pSkillTray->SetVisibleSingle( FALSE );
    RegisterControl( pSkillTray );
    m_pSkillTray[3] = pSkillTray;

    m_rcBackBasic = GetGlobalPos();
}

void CSkillTrayTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( !IsMoveLock() )
        DragUpdate( x, y, m_vPos );

    if ( !IsExclusiveControl() )
    {
        m_bDrag = FALSE;

        INT nIndex = GetTabIndex();

        m_pSkillTray[nIndex]->SetNoMessage( FALSE );
        m_pSkillTray[nIndex]->SetNoUpdate ( FALSE );

        m_pTextButton[nIndex]->SetNoMessage( FALSE );
        m_pTextButton[nIndex]->SetNoUpdate( FALSE );

        m_pTurnButton->SetNoMessage( FALSE );
        m_pTurnButton->SetNoUpdate( FALSE );

        m_pLockButtonOpenUnlock->SetNoMessage( FALSE );
        m_pLockButtonOpenLock->SetNoUpdate( FALSE );
        m_pLockButtonOpenUnlockH->SetNoMessage( FALSE );
        m_pLockButtonOpenLockH->SetNoUpdate( FALSE );
        m_pLockButtonCloseUnlock->SetNoMessage( FALSE );
        m_pLockButtonCloseLock->SetNoUpdate( FALSE );
        m_pLockButtonCloseUnlockH->SetNoMessage( FALSE );
        m_pLockButtonCloseLockH->SetNoUpdate( FALSE );

        m_pOpenButton->SetNoMessage( FALSE );
        m_pOpenButton->SetNoUpdate( FALSE );

        m_pCloseButton->SetNoMessage( FALSE );
        m_pCloseButton->SetNoUpdate( FALSE );
    }
    else if ( !(m_rcBackPos == GetGlobalPos()) )
    {
        m_bDrag = TRUE;

        INT nIndex = GetTabIndex();

        m_pSkillTray[nIndex]->SetNoMessage( TRUE );
        m_pSkillTray[nIndex]->SetNoUpdate ( TRUE );
        m_pSkillTray[nIndex]->SetSkillInfoVisibleOff();

        m_pTextButton[nIndex]->SetNoMessage( TRUE );
        m_pTextButton[nIndex]->SetNoUpdate( TRUE );

        m_pTurnButton->SetNoMessage( TRUE );
        m_pTurnButton->SetNoUpdate( TRUE );

        m_pLockButtonOpenUnlock->SetNoMessage( TRUE );
        m_pLockButtonOpenLock->SetNoUpdate( TRUE );
        m_pLockButtonOpenUnlockH->SetNoMessage( TRUE );
        m_pLockButtonOpenLockH->SetNoUpdate( TRUE );
        m_pLockButtonCloseUnlock->SetNoMessage( TRUE );
        m_pLockButtonCloseLock->SetNoUpdate( TRUE );
        m_pLockButtonCloseUnlockH->SetNoMessage( TRUE );
        m_pLockButtonCloseLockH->SetNoUpdate( TRUE );

        m_pOpenButton->SetNoMessage( TRUE );
        m_pOpenButton->SetNoUpdate( TRUE );

        m_pCloseButton->SetNoMessage( TRUE );
        m_pCloseButton->SetNoUpdate( TRUE );
    }
}

void CSkillTrayTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    if ( !IsMoveLock() )
        DragMessage( dwMsg );

    if ( m_bDrag )
    {
        return;
    }

    if ( UIMSG_LB_DOWN&dwMsg )
    {
        m_rcBackPos = GetGlobalPos();
    }

	switch ( ControlID )
    {
    case BASIC_QUICK_SKILL_SLOT:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( UIMSG_LB_DUP & dwMsg )
                {
                    TurnVertical();
                    TrayOpen();
                    SetGlobalPos( m_rcBackBasic );
                    ResetExclusiveControl();
                }
            }
        }
        break;

    case QUICK_SKILL_TRAY_TURN_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    if ( m_bTurnHorizontal )
                    {
                        TurnVertical();
                    }
                    else
                    {
                        TurnHorizontal();
                    }
                }
            }
        }
        break;

    case QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN:
	case QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN_H:
    case QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE:
    case QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE_H:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SetMoveLock( !m_bMoveLock );
        }
        break;

    case QUICK_SKILL_TRAY_OPEN_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 사용금지.
                        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
                    else
                    {
                        TrayOpen();
                        
                        // 연 직후 클릭 방지용
                        INT nIndex = GetTabIndex();
                        m_pSkillTray[nIndex]->SetNoUpdate ( TRUE );
                    }
                }
            }
        }
        break;

	case QUICK_SKILL_TRAY_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					if ( m_pGaeaClient->GetTutorial()->IsTutorial() ) // 튜토리얼 중에는 사용금지.
						m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TUTORIAL_CANNOT_USE") );
					else
					{
                        TrayClose();

                        // 연 직후 클릭 방지용
                        INT nIndex = GetTabIndex();
                        m_pSkillTray[nIndex]->SetNoUpdate ( TRUE );
					}
				}
			}
		}
		break;

	case BASIC_TEXT_BUTTON_F1:
	case BASIC_TEXT_BUTTON_F2:
	case BASIC_TEXT_BUTTON_F3:
	case BASIC_TEXT_BUTTON_F4:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE( dwMsg ) )
				{
					INT nIndex = GetTabIndex();
					INT nNextIndex = NEXT_INDEX[nIndex];

					m_pSkillTray[nIndex]->SetVisibleSingle( FALSE );
					m_pSkillTray[nNextIndex]->SetVisibleSingle( TRUE );

					m_pTextButton[nIndex]->SetVisibleSingle( FALSE );
					m_pTextButton[nNextIndex]->SetVisibleSingle( TRUE );

					SetTabIndex( nNextIndex );
				}
			}
		}
		break;

	case BASIC_QUICK_SKILL_TRAY_F1:
	case BASIC_QUICK_SKILL_TRAY_F2:
	case BASIC_QUICK_SKILL_TRAY_F3:
	case BASIC_QUICK_SKILL_TRAY_F4:
		{
			if( dwMsg & UIMSG_MOUSEIN_SKILLSLOT )
			{
				AddMessageEx( UIMSG_MOUSEIN_SKILLSLOT );
			}
		}
		break;
	}
}

void CSkillTrayTab::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
	{
        TrayOpen ();
	}
}

void CSkillTrayTab::SetShotcutText( DWORD nID, CString& strTemp )
{
	m_pSkillTray[0]->SetShotcutText( nID, strTemp );
	m_pSkillTray[1]->SetShotcutText( nID, strTemp );
	m_pSkillTray[2]->SetShotcutText( nID, strTemp );
	m_pSkillTray[3]->SetShotcutText( nID, strTemp );
}

void CSkillTrayTab::SetTabIndex( INT nIndex )
{ 
	GASSERT( 0 <= nIndex && nIndex < MAX_TAB_INDEX ); 

	if ( nIndex == m_nTabIndex ) return;

    if ( IsTrayClose() )
    {
        TrayOpen();
    }

	m_pSkillTray[m_nTabIndex]->SetVisibleSingle( FALSE );
	m_pSkillTray[nIndex]->SetVisibleSingle( TRUE );
	m_pTextButton[m_nTabIndex]->SetVisibleSingle( FALSE );
	m_pTextButton[nIndex]->SetVisibleSingle( TRUE );

	m_nTabIndex = nIndex; 
}

const UIRECT& CSkillTrayTab::GetCurrentSkillSlotGlobalPos()
{
    return m_pCurrentSkillSlot->GetGlobalPos();
}

const WORD CSkillTrayTab::FindSkillSlot( const SNATIVEID& sSkillID )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	UINT nSkillIndex = static_cast<UINT>(GetTabIndex() * QUICK_SKILL_SLOT_MAX);

	for ( UINT i=0; i<QUICK_SKILL_SLOT_MAX; ++i )
	{
		if ( pCharacter->m_sSKILLQUICK[nSkillIndex+i] == sSkillID )
		{
			return nSkillIndex+i;
		}
	}

	return USHRT_MAX;
}

const UIRECT& CSkillTrayTab::FindSkillSlotGlobalPos( const SNATIVEID& sSkillID )
{
	static UIRECT rcNull;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	UINT nSkillIndex = static_cast<UINT>(GetTabIndex() * QUICK_SKILL_SLOT_MAX);

	for ( UINT i=0; i<QUICK_SKILL_SLOT_MAX; ++i )
	{
		if ( pCharacter->m_sSKILLQUICK[nSkillIndex+i] == sSkillID )
		{
			return m_pSkillTray[m_nTabIndex]->GetSlotGlobalPos(static_cast<WORD>(i));
		}
	}

	return rcNull;
}

void CSkillTrayTab::TrayOpen()
{
    if ( m_bClose )
    {
		m_pQuickSkillTrayMoveLockButtonOpen->SetVisibleSingle(TRUE);
		m_pQuickSkillTrayMoveLockButtonOpenH->SetVisibleSingle(TRUE);
		m_pQuickSkillTrayMoveLockButtonClose->SetVisibleSingle(FALSE);
		m_pQuickSkillTrayMoveLockButtonCloseH->SetVisibleSingle(FALSE);

        CUIControl ParentControl(m_pEngineDevice); 

        if ( m_bTurnHorizontal )
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_H" );
        }
        else
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW" );
        }

        UIRECT rcOldGlobal = GetGlobalPos();
        UIRECT rcNewGlobal = ParentControl.GetGlobalPos();
        rcNewGlobal.left   = rcOldGlobal.left;
        rcNewGlobal.right  = rcOldGlobal.left + rcNewGlobal.sizeX;
        rcNewGlobal.top    = rcOldGlobal.top;
        rcNewGlobal.bottom = rcOldGlobal.top  + rcNewGlobal.sizeY;

        CUIControl::SetLocalPos( D3DXVECTOR2(0.0f, 0.0f) );
        CUIControl::ReSizeControl( rcOldGlobal, rcNewGlobal );

        m_pSkillTray[0]->SetVisibleSingle( FALSE );
        m_pSkillTray[1]->SetVisibleSingle( FALSE );
        m_pSkillTray[2]->SetVisibleSingle( FALSE );
        m_pSkillTray[3]->SetVisibleSingle( FALSE );

        m_pTextButton[0]->SetVisibleSingle( FALSE );
        m_pTextButton[1]->SetVisibleSingle( FALSE );
        m_pTextButton[2]->SetVisibleSingle( FALSE );
        m_pTextButton[3]->SetVisibleSingle( FALSE );

        INT nIndex = GetTabIndex();
        m_pSkillTray[nIndex]->SetVisibleSingle( TRUE );
        m_pTextButton[nIndex]->SetVisibleSingle( TRUE );

        m_pGap0->SetVisibleSingle( TRUE );
        m_pGap1->SetVisibleSingle( TRUE );
        m_pGap2->SetVisibleSingle( FALSE );

        m_pTurnButton->SetVisibleSingle(TRUE);
        if ( m_bTurnHorizontal )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        m_pOpenButton->SetVisibleSingle(FALSE);
        m_pCloseButton->SetVisibleSingle(TRUE);

        m_bClose = FALSE;
    }
}

void CSkillTrayTab::TrayClose()
{
    if ( !m_bClose )
    {
		m_pQuickSkillTrayMoveLockButtonOpen->SetVisibleSingle(FALSE);
		m_pQuickSkillTrayMoveLockButtonOpenH->SetVisibleSingle(FALSE);
		m_pQuickSkillTrayMoveLockButtonClose->SetVisibleSingle(TRUE);
		m_pQuickSkillTrayMoveLockButtonCloseH->SetVisibleSingle(TRUE);
        CUIControl ParentControl(m_pEngineDevice); 

        if ( m_bTurnHorizontal )
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_CLOSE_H" );
        }
        else
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_CLOSE" );
        }

        UIRECT rcOldGlobal = GetGlobalPos();
        UIRECT rcNewGlobal = ParentControl.GetGlobalPos();
        rcNewGlobal.left   = rcOldGlobal.left;
        rcNewGlobal.right  = rcOldGlobal.left + rcNewGlobal.sizeX;
        rcNewGlobal.top    = rcOldGlobal.top;
        rcNewGlobal.bottom = rcOldGlobal.top  + rcNewGlobal.sizeY;

        CUIControl::SetLocalPos( D3DXVECTOR2(0.0f, 0.0f) );
        CUIControl::ReSizeControl( rcOldGlobal, rcNewGlobal );

        m_pSkillTray[0]->SetVisibleSingle( FALSE );
        m_pSkillTray[1]->SetVisibleSingle( FALSE );
        m_pSkillTray[2]->SetVisibleSingle( FALSE );
        m_pSkillTray[3]->SetVisibleSingle( FALSE );

        m_pTextButton[0]->SetVisibleSingle( FALSE );
        m_pTextButton[1]->SetVisibleSingle( FALSE );
        m_pTextButton[2]->SetVisibleSingle( FALSE );
        m_pTextButton[3]->SetVisibleSingle( FALSE );

        m_pGap0->SetVisibleSingle( FALSE );
        m_pGap1->SetVisibleSingle( FALSE );
        m_pGap2->SetVisibleSingle( TRUE );

        m_pTurnButton->SetVisibleSingle(FALSE);
        if ( m_bTurnHorizontal )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( TRUE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( TRUE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        m_pOpenButton->SetVisibleSingle(TRUE);
        m_pCloseButton->SetVisibleSingle(FALSE);

        m_bClose = TRUE;
    }
}

void CSkillTrayTab::TurnHorizontal ()
{
    if ( m_bTurnHorizontal )
    {
        return;
    }

    UIRECT rcOldGlobal = GetGlobalPos();

    // 최상단 컨트롤의 사이즈
    {
        CUIControl ParentControl(m_pEngineDevice); 

        if ( m_bClose )
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_CLOSE_H" );
        }
        else
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_H" );
        }

        UIRECT rcOldGlobal = GetGlobalPos();
        UIRECT rcNewGlobal = ParentControl.GetGlobalPos();
        rcNewGlobal.left   = rcOldGlobal.left;
        rcNewGlobal.right  = rcOldGlobal.left + rcNewGlobal.sizeX;
        rcNewGlobal.top    = rcOldGlobal.top;
        rcNewGlobal.bottom = rcOldGlobal.top  + rcNewGlobal.sizeY;

        CUIControl::SetLocalPos( D3DXVECTOR2(0.0f, 0.0f) );
        CUIControl::ReSizeControl( rcOldGlobal, rcNewGlobal );
    }

    // 스킬 바 돌리기
    {
        m_pSkillTray[0]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_H" );
        m_pSkillTray[0]->TurnHorizontal();
        m_pSkillTray[1]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_H" );
        m_pSkillTray[1]->TurnHorizontal();
        m_pSkillTray[2]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_H" );
        m_pSkillTray[2]->TurnHorizontal();
        m_pSkillTray[3]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_H" );
        m_pSkillTray[3]->TurnHorizontal();
    }

    {
        m_pGap0->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_0_H" );
        m_pGap1->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_1_H" );
        m_pGap2->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_2_H" );
    }

    {
        CUIControl ButtonControl(m_pEngineDevice);
        ButtonControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_BUTTON" );

        m_pTextButton[0]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON_H" );
        m_pTextButton[1]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON_H" );
        m_pTextButton[2]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON_H" );
        m_pTextButton[3]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON_H" );

        m_pTurnButton->ResetControl ( GetGlobalPos(), "QUICK_SKILL_TRAY_TURN_BUTTON_H" );
        m_pTurnButton->GetFlipControl()->ResetControl( m_pTurnButton->GetGlobalPos(), "QUICK_SKILL_TRAY_TURN_BUTTON_F_H" );

        if ( m_bClose )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( TRUE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( TRUE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }

        m_pOpenButton->ResetControl ( GetGlobalPos(), "QUICK_SKILL_TRAY_OPEN_BUTTON_H" );
        m_pOpenButton->GetFlipControl()->ResetControl( m_pOpenButton->GetGlobalPos(), "QUICK_SKILL_TRAY_OPEN_BUTTON_F_H" );

        m_pCloseButton->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_CLOSE_BUTTON_H" );
        m_pCloseButton->GetFlipControl()->ResetControl( m_pCloseButton->GetGlobalPos(), "QUICK_SKILL_TRAY_CLOSE_BUTTON_F_H" );
    }

    // 돌린후 이동
    {
        const UIRECT& rcNewGlobal = GetGlobalPos();

        SetGlobalPos( D3DXVECTOR2 (   
            rcOldGlobal.left + rcOldGlobal.sizeX*0.5f - rcNewGlobal.sizeX*0.5f,
            rcOldGlobal.top  + rcOldGlobal.sizeY*0.5f - rcNewGlobal.sizeY*0.5f 
            ));
    }

    m_bTurnHorizontal = TRUE;
}

void CSkillTrayTab::TurnVertical ()
{
    if ( !m_bTurnHorizontal )
    {
        return;
    }

    UIRECT rcOldGlobal = GetGlobalPos();

    // 최상단 컨트롤의 사이즈
    {
        CUIControl ParentControl(m_pEngineDevice); 

        if ( m_bClose )
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW_CLOSE" );
        }
        else
        {
            ParentControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_WINDOW" );
        }

        UIRECT rcOldGlobal = GetGlobalPos();
        UIRECT rcNewGlobal = ParentControl.GetGlobalPos();
        rcNewGlobal.left   = rcOldGlobal.left;
        rcNewGlobal.right  = rcOldGlobal.left + rcNewGlobal.sizeX;
        rcNewGlobal.top    = rcOldGlobal.top;
        rcNewGlobal.bottom = rcOldGlobal.top  + rcNewGlobal.sizeY;

        CUIControl::SetLocalPos( D3DXVECTOR2(0.0f, 0.0f) );
        CUIControl::ReSizeControl( rcOldGlobal, rcNewGlobal );
    }

    // 스킬 바 돌리기
    {
        m_pSkillTray[0]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY" );
        m_pSkillTray[0]->TurnVertical();
        m_pSkillTray[1]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY" );
        m_pSkillTray[1]->TurnVertical();
        m_pSkillTray[2]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY" );
        m_pSkillTray[2]->TurnVertical();
        m_pSkillTray[3]->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY" );
        m_pSkillTray[3]->TurnVertical();
    }

    {
        m_pGap0->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_0" );
        m_pGap1->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_1" );
        m_pGap2->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_GAP_2" );
    }

    {
        CUIControl ButtonControl(m_pEngineDevice);
        ButtonControl.Create ( 1, "QUICK_SKILL_TRAY_TAB_BUTTON" );

        m_pTextButton[0]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON" );
        m_pTextButton[1]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON" );
        m_pTextButton[2]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON" );
        m_pTextButton[3]->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_TAB_BUTTON" );

        m_pTurnButton->ResetControl ( GetGlobalPos(), "QUICK_SKILL_TRAY_TURN_BUTTON" );
        m_pOpenButton->GetFlipControl()->ResetControl( m_pOpenButton->GetGlobalPos(), "QUICK_SKILL_TRAY_TURN_BUTTON_F" );

        if ( m_bClose )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }

        m_pOpenButton->ResetControl ( GetGlobalPos(), "QUICK_SKILL_TRAY_OPEN_BUTTON" );
        m_pOpenButton->GetFlipControl()->ResetControl( m_pOpenButton->GetGlobalPos(), "QUICK_SKILL_TRAY_OPEN_BUTTON_F" );

        m_pCloseButton->ResetControl( GetGlobalPos(), "QUICK_SKILL_TRAY_CLOSE_BUTTON" );
        m_pCloseButton->GetFlipControl()->ResetControl( m_pCloseButton->GetGlobalPos(), "QUICK_SKILL_TRAY_CLOSE_BUTTON_F" );
    }

    // 돌린후 이동
    {
        const UIRECT& rcNewGlobal = GetGlobalPos();

        SetGlobalPos( D3DXVECTOR2 (   
                 rcOldGlobal.left + rcOldGlobal.sizeX*0.5f - rcNewGlobal.sizeX*0.5f,
                 rcOldGlobal.top  + rcOldGlobal.sizeY*0.5f - rcNewGlobal.sizeY*0.5f 
            ));
    }

    m_bTurnHorizontal = FALSE;
}

const BOOL CSkillTrayTab::CollisionCheck  ( const int x, const int y, const UIRECT& rcDest )
{
    if( x  < rcDest.left || x > rcDest.right  ) return FALSE;
    if( y  < rcDest.top  || y > rcDest.bottom ) return FALSE;

    return TRUE;
}

HRESULT CSkillTrayTab::InfoFileLoad ()
{
    using namespace NSUIINFOCFG;
    NSUIINFOCFG::CONTROL sInfoControl = NSUIINFOCFG::LOAD_CONTROL( "SKILL_TRAY_TAB" );

    if ( !sInfoControl.Empty() )
    {
        bool bRet = false;

        //////////////////////////////////////////////////////////////////////
        bool bHorizontal = false;

        bRet = sInfoControl.GetValue( "HORIZONTAL", bHorizontal );

        if ( bHorizontal )
        {
            TurnHorizontal();
        }

        //////////////////////////////////////////////////////////////////////
        bool bTrayClose = false;

        bRet = sInfoControl.GetValue( "TRAYCLOSE", bTrayClose );

        if ( bTrayClose )
        {
            TrayClose();
        }

        //////////////////////////////////////////////////////////////////////
        int nPosX = 0;
        int nPosY = 0;

        bRet = sInfoControl.GetValueAttr( "POSITION", 
            ATTRLIST (
                ATTR("X", (int)nPosX),
                ATTR("Y", (int)nPosY)
            ) );

        SetLocalPos ( D3DXVECTOR2((float)nPosX, (float)nPosY) );

        //////////////////////////////////////////////////////////////////////
        bRet = sInfoControl.GetValue( "TRAY_MOVE_LOCK", m_bMoveLock );
        SetMoveLock( m_bMoveLock );
    }

    return S_OK;
}

HRESULT CSkillTrayTab::InfoFileSave ()
{
    using namespace NSUIINFOCFG;
    NSUIINFOCFG::CONTROL sInfoControl = NSUIINFOCFG::SAVE_CONTROL( "SKILL_TRAY_TAB" );

    if ( !sInfoControl.Empty() )
    {
        bool bRet = false;

        //////////////////////////////////////////////////////////////////////
        bRet = sInfoControl.SetValue( "HORIZONTAL", m_bTurnHorizontal );

        //////////////////////////////////////////////////////////////////////
        bRet = sInfoControl.SetValue( "TRAYCLOSE", m_bClose );

        //////////////////////////////////////////////////////////////////////
        const UIRECT& rcPosition = GetGlobalPos();

        bRet = sInfoControl.SetValueAttr( "POSITION",  
            ATTRLIST (
                ATTR("X", int(rcPosition.left)),
                ATTR("Y", int(rcPosition.top))
            ) );

        //////////////////////////////////////////////////////////////////////
        bRet = sInfoControl.SetValue( "TRAY_MOVE_LOCK", m_bMoveLock );
    }

    return S_OK;
}

void CSkillTrayTab::SetMoveLock( bool bMoveLock )
{
    m_bMoveLock = bMoveLock;

    if ( m_bTurnHorizontal )
    {
        if ( m_bClose )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( TRUE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( TRUE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
    }
    else
    {
        if ( m_bClose )
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
        else
        {
            if ( IsMoveLock() )
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLock->SetVisibleSingle(TRUE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
            else
            {
                m_pLockButtonOpenUnlock->SetVisibleSingle(TRUE);
                m_pLockButtonOpenLock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlock->SetVisibleSingle(FALSE);
                m_pLockButtonCloseLock->SetVisibleSingle(FALSE);
                m_pLockButtonOpenUnlockH->SetVisibleSingle(FALSE);
                m_pLockButtonOpenLockH->SetVisibleSingle(FALSE);
                m_pLockButtonCloseUnlockH->SetVisibleSingle( FALSE );
                m_pLockButtonCloseLockH->SetVisibleSingle( FALSE );
            }
        }
    }
}

bool CSkillTrayTab::IsMoveLock()
{
    return m_bMoveLock;
}

void CSkillTrayTab::ResetSlotText()
{
	if ( m_pSkillTray[0] )
		m_pSkillTray[0]->ResetSlotText();
	if ( m_pSkillTray[1] )
		m_pSkillTray[1]->ResetSlotText();
	if ( m_pSkillTray[2] )
		m_pSkillTray[2]->ResetSlotText();
	if ( m_pSkillTray[3] )
		m_pSkillTray[3]->ResetSlotText();
}

MySkillTrayTab::MySkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CSkillTrayTab( pGaeaClient, pInterface, pEngineDevice )
{
}

void MySkillTrayTab::CreateUIWindowAndRegisterOwnership()
{
    CSkillTrayTab::CreateEx( QUICK_SKILL_TRAY_TAB_WINDOW, "QUICK_SKILL_TRAY_TAB_WINDOW" );
    CSkillTrayTab::CreateSubControl();
    CSkillTrayTab::m_pInterface->UiRegisterControl( this, true );
    CSkillTrayTab::m_pInterface->UiShowGroupFocus( QUICK_SKILL_TRAY_TAB_WINDOW );
}


const UIRECT& MySkillTrayTab::GetUIWindowGlobalPos()
{
    return CSkillTrayTab::GetGlobalPos();
}

const UIRECT& MySkillTrayTab::GetCurrentSkillSlotGlobalPos()
{
    return CSkillTrayTab::GetCurrentSkillSlotGlobalPos();
}

const WORD MySkillTrayTab::FindSkillSlot( const SNATIVEID& sSkillID )
{
	return CSkillTrayTab::FindSkillSlot( sSkillID );
}

const UIRECT& MySkillTrayTab::FindSkillSlotGlobalPos( const SNATIVEID& sSkillID )
{
	return CSkillTrayTab::FindSkillSlotGlobalPos( sSkillID );
}

void MySkillTrayTab::SetShotcutText( DWORD nID, UI::String& strTemp )
{
    CSkillTrayTab::SetShotcutText( nID, UI::ToString( strTemp ) );
}

INT MySkillTrayTab::GetTabIndex()
{
    return CSkillTrayTab::GetTabIndex();
}

void MySkillTrayTab::SetTabIndex( INT nIndex )
{
    CSkillTrayTab::SetTabIndex( nIndex );
}

void MySkillTrayTab::SelectSlot( INT nIndex )
{
	//
}

void MySkillTrayTab::TrayToggle ()
{
    if ( CSkillTrayTab::IsTrayClose() )
    {
        CSkillTrayTab::TrayOpen();
    }
    else
    {
        CSkillTrayTab::TrayClose();
    }
}

void MySkillTrayTab::TrayOpen ()
{
    if ( CSkillTrayTab::IsTrayClose() )
    {
        CSkillTrayTab::TrayOpen();
    }
}

BOOL MySkillTrayTab::IsMoveLock ()
{
	return CSkillTrayTab::IsMoveLock();
}

void MySkillTrayTab::ResetSlotText()
{
	CSkillTrayTab::ResetSlotText();
}

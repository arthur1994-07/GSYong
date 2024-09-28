#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../Item/ItemImage.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../Util/RnButton.h"
#include "./RebirthWindow.h"
#include "./RebirthWindowButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float fDEFAULT_TIME_LEFT = 5.0f;

CRebirthWindow::CRebirthWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx( pInterface, pEngineDevice )
, m_pGaeaClient(pGaeaClient)
, m_pInterface ( pInterface )
, m_pNotifyBack(NULL)
, m_pNotifyBack2(NULL)
, m_pRecommandBack(NULL)
, m_pNotifyMain(NULL)
, m_pNotifyMainImage(NULL)
, m_pRebirthQuestionText(NULL)
, m_pRebirthInfoText(NULL)
, m_pAutoRebirthCheck(NULL)
, m_pAutoRebirthUnCheck(NULL)
, m_pAutoRebirthText(NULL)
, m_pTempLeftButton(NULL)
, m_pTempRightButton(NULL)
, m_pCheckPointRebirthButton(NULL)
, m_pExpRebirthButton(NULL)
, m_pCurrentPosRebirthButton(NULL)
, m_pSkillRebirthButton(NULL)
, m_pHelpButton(NULL)
, m_pTooltipBack(NULL)
, m_pTooltipText(NULL)
, m_pRecommendItem(NULL)
, m_pRecommendItemBack(NULL)
, m_pRecommendItemText(NULL)
, m_pBuyButton(NULL)
, m_nDONE_ACTION(REBIRTH_NONE)
, m_bAutoRebirth(false)
, m_bReviveSkill(false)
, m_fTIME_LEFT(fDEFAULT_TIME_LEFT)
, m_nACTION(NO_ID)
, m_fAutoRebirthTime(GLCONST_CHAR::wAUTO_REBIRTH_TIMER)
{
}

CRebirthWindow::~CRebirthWindow ()
{
}

void CRebirthWindow::CreateSubControl ()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_NOTIFY_REBIRTH_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_NOTIFY_REBIRTH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pNotifyBack = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_NOTIFY_REBIRTH_BACK2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_NOTIFY_REBIRTH_BACK2_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pNotifyBack = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_RECOMMAND_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_RECOMMAND_ITEM_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pRecommandBack = pLinBoxSmart;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "REBIRTH_WINDOW_NOTIFY_MAIN_IMAGE", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pNotifyMainImage = pControl;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_NOTIFY_REBIRTH_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_NOTIFY_REBIRTH_MAIN_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pNotifyMain = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont12Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub ( this, "REBIRTH_WINDOW_QUESTION_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont12Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pRebirthQuestionText = pTextBox;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub ( this, "REBIRTH_WINDOW_INFORMATION_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pRebirthInfoText = pTextBox;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "REBIRTH_WINDOW_AUTO_REBIRTH_CHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pAutoRebirthCheck = pControl;
        m_pAutoRebirthCheck->SetVisibleSingle( FALSE );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "REBIRTH_WINDOW_AUTO_REBIRTH_UNCHECK", UI_FLAG_DEFAULT );
        RegisterControl( pControl );
        m_pAutoRebirthUnCheck = pControl;

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "REBIRTH_WINDOW_AUTO_REBIRTH_CHECK_BACK", UI_FLAG_DEFAULT, AUTO_REBIRTH_CHECK );
        RegisterControl( pControl );

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub ( this, "REBIRTH_WINDOW_AUTO_REBIRTH_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pAutoRebirthText = pTextBox;
        m_pAutoRebirthText->SetText( ID2GAMEINTEXT( "REBIRTH_WINDOW_AUTO_REBIRTH" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        m_pTempLeftButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pTempLeftButton->CreateSub( this, "REBIRTH_WINDOW_LEFT_BUTTON" );
        m_pTempLeftButton->CreateSubControl();
        RegisterControl( m_pTempLeftButton );
        m_pTempLeftButton->SetVisibleSingle( FALSE );

        m_pTempRightButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pTempRightButton->CreateSub( this, "REBIRTH_WINDOW_RIGHT_BUTTON" );
        m_pTempRightButton->CreateSubControl();
        RegisterControl( m_pTempRightButton );
        m_pTempRightButton->SetVisibleSingle( FALSE );
    }

    {
        std::string strButtonText = "";

        m_pCheckPointRebirthButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pCheckPointRebirthButton->CreateSub( this, "REBIRTH_WINDOW_LEFT_BUTTON", UI_FLAG_DEFAULT, CHECK_POINT_BUTTON );
        m_pCheckPointRebirthButton->CreateSubControl();
        RegisterControl( m_pCheckPointRebirthButton );
        m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );

        strButtonText = sc::string::format( "%s", ID2GAMEWORD( "REBIRTH_BUTTON", 0 ) );
        m_pCheckPointRebirthButton->SetText( strButtonText );

        m_pExpRebirthButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pExpRebirthButton->CreateSub( this, "REBIRTH_WINDOW_LEFT_BUTTON", UI_FLAG_DEFAULT, RECOVERY_EXP_BUTTON );
        m_pExpRebirthButton->CreateSubControl();
        RegisterControl( m_pExpRebirthButton );
        m_pExpRebirthButton->SetVisibleSingle( FALSE );

        strButtonText = sc::string::format( "%s", ID2GAMEWORD( "REBIRTH_BUTTON", 2 ) );
        m_pExpRebirthButton->SetText( strButtonText );

        m_pCurrentPosRebirthButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pCurrentPosRebirthButton->CreateSub( this, "REBIRTH_WINDOW_LEFT_BUTTON", UI_FLAG_DEFAULT, CURRENT_POS_BUTTON );
        m_pCurrentPosRebirthButton->CreateSubControl();
        RegisterControl( m_pCurrentPosRebirthButton );
        m_pCurrentPosRebirthButton->SetVisibleSingle( FALSE );

        strButtonText = sc::string::format( "%s", ID2GAMEWORD( "REBIRTH_BUTTON", 1 ) );
        m_pCurrentPosRebirthButton->SetText( strButtonText );

        m_pSkillRebirthButton = new CRebirthWindowButton( m_pInterface, m_pEngineDevice );
        m_pSkillRebirthButton->CreateSub( this, "REBIRTH_WINDOW_LEFT_BUTTON", UI_FLAG_DEFAULT, SKILL_REBIRTH_BUTTON );
        m_pSkillRebirthButton->CreateSubControl();
        RegisterControl( m_pSkillRebirthButton );
        m_pSkillRebirthButton->SetVisibleSingle( FALSE );

        strButtonText = sc::string::format( "%s", ID2GAMEWORD( "REBIRTH_BUTTON", 3 ) );
        m_pSkillRebirthButton->SetText( strButtonText );
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "REBIRTH_WINDOW_HELP_BUTTON_OVER" );
        RegisterControl( pControl );
        m_pHelpButton = pControl;
        m_pHelpButton->SetVisibleSingle( TRUE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_ITEM_EXPLAIN_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_ITEM_EXPLAIN_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pRecommendItemBack = pLinBoxSmart;

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub ( this, "REBIRTH_WINDOW_ITEM_EXPLAIN_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pRecommendItemText = pTextBox;
        m_pRecommendItemText->AddText( ID2GAMEINTEXT( "REBIRTH_WINDOW_RECOMMEND_ITEM_EXPLAIN" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CItemImage* pItemImage = new CItemImage( m_pGaeaClient, m_pEngineDevice );
        pItemImage->CreateSub( this, "REBIRTH_WINDOW_ITEM_IMAGE", UI_FLAG_DEFAULT );
        pItemImage->CreateSubControl();
        RegisterControl( pItemImage );
        m_pRecommendItem = pItemImage;
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        arg.text = sc::string::format( "%1%", ID2GAMEWORD( "REBIRTH_WINDOW_BUY_BUTTON_TEXT" ) );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"REBIRTH_WINDOW_ITEM_BUY_BUTTON",UI_FLAG_DEFAULT,BUY_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pBuyButton = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "REBIRTH_WINDOW_TOOLTIP_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "REBIRTH_WINDOW_TOOLTIP_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pTooltipBack = pLinBoxSmart;
        m_pTooltipBack->SetVisibleSingle( FALSE );

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );
        pTextBox->CreateSub ( this, "REBIRTH_WINDOW_TEXT_BOX", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTooltipText = pTextBox;
        m_pTooltipText->SetVisibleSingle( FALSE );
    }

    if ( GLogicData::GetInstance().GetServiceProvider() == SP_THAILAND )
    {
        m_pBuyButton->SetVisibleSingle(FALSE);
    }
}

void CRebirthWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible() ) return;

    CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    m_fAutoRebirthTime -= fElapsedTime;

	int nTempAction = m_nACTION;
	if ( RANPARAM::bCheckAutoRebirth )
		m_nACTION = REBIRTH_CHECKPOINT;

    if ( m_fAutoRebirthTime <= 0.0f )
    {
        m_fAutoRebirthTime = 0.0f;

        // TODO : 부활.
        switch ( m_nACTION )
        {
        case REBIRTH_CURRENT_POSITION:
            {
                m_nDONE_ACTION = REBIRTH_CURRENT_POSITION;
                //CurrentPosRebirth();
            }
            break;

        case REBIRTH_EXP:
            {
                m_nDONE_ACTION = REBIRTH_EXP;
                //RecoveryExpRebirth();
            }
            break;

        case REBIRTH_CHECKPOINT:
            {
                m_nDONE_ACTION = REBIRTH_CHECKPOINT;
                //CheckPointRebirth();
            }
            break;

        case REBIRTH_SKILL:
            {
                m_nDONE_ACTION = REBIRTH_SKILL;
            }
            break;

        default:
            {
                m_nDONE_ACTION = REBIRTH_CHECKPOINT;
                GASSERT( 0 && "부활에서 알 수 없는 액션입니다." );
                CheckPointRebirth();
            }
            break;
        }
    }

    std::string strRebirth = "";

    switch ( m_nACTION )
    {
    case REBIRTH_CURRENT_POSITION:
        {
            strRebirth = sc::string::format( "%s", ID2GAMEWORD( "AUTO_REBIRTH_ACTION", 1 ) );
        }
        break;

    case REBIRTH_EXP:
        {
            strRebirth = sc::string::format( "%s", ID2GAMEWORD( "AUTO_REBIRTH_ACTION", 2 ) );
        }
        break;

    case REBIRTH_CHECKPOINT:
        {
            strRebirth = sc::string::format( "%s", ID2GAMEWORD( "AUTO_REBIRTH_ACTION", 0 ) );
        }
        break;

    case REBIRTH_SKILL:
        {
            strRebirth = sc::string::format( "%s", ID2GAMEWORD( "AUTO_REBIRTH_ACTION", 3 ) );
        }
        break;

    default:
        {
            GASSERT( 0 && "부활에서 알 수 없는 액션입니다." );
        };
    }

    m_pRebirthQuestionText->ClearText();
    strRebirth = sc::string::format( ID2GAMEINTEXT( "REBIRTH_WINDOW_QUESTION" ), m_fAutoRebirthTime, strRebirth );
    m_pRebirthQuestionText->AddText( strRebirth.c_str(), NS_UITEXTCOLOR::WHITE );

    if ( m_nDONE_ACTION != REBIRTH_NONE )
    {
        std::string strLeftTime;
        strLeftTime = sc::string::format( "%1.0f%s", floor(m_fTIME_LEFT) + 1.0f, ID2GAMEWORD("WAITSERVER_TIMELEFT_UNIT"));

        m_pRebirthQuestionText->ClearText();
        m_pRebirthQuestionText->AddText( ID2GAMEINTEXT("WAITSERVER_MESSAGE") );
        m_pRebirthQuestionText->AddText( strLeftTime.c_str() );

        m_fTIME_LEFT -= fElapsedTime;

        bool bTimeEvent = false;
        if ( m_fTIME_LEFT <= -1.0f )
        {
            m_fTIME_LEFT = -1.0f;

            switch ( m_nDONE_ACTION )
            {
            case REBIRTH_CURRENT_POSITION:
                {
                    m_nDONE_ACTION = REBIRTH_NONE;
                    CurrentPosRebirth();
                }
                break;

            case REBIRTH_EXP:
                {
                    m_nDONE_ACTION = REBIRTH_NONE;
                    RecoveryExpRebirth();
                }
                break;

            case REBIRTH_CHECKPOINT:
                {
                    m_nDONE_ACTION = REBIRTH_NONE;
                    CheckPointRebirth();
                }
                break;

            case REBIRTH_SKILL:
                {
                    m_nDONE_ACTION = REBIRTH_NONE;
                    SKillRebirth();
                }
                break;

            default:
                {
                    m_nDONE_ACTION = REBIRTH_NONE;
                    GASSERT( 0 && "부활에서 알 수 없는 액션입니다." );
                    CheckPointRebirth();
                }
                break;
            }
        }
    }

	m_nACTION = nTempAction;
}

void CRebirthWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    // Note : 윈도우 이동을 막는다.

    switch ( ControlID )
    {
    case BUY_BUTTON:
        {
            if ( GLogicData::GetInstance().GetServiceProvider() == SP_THAILAND )
                break;

            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
                PointshopOpenRecommendItem();
        }
        break;

    case AUTO_REBIRTH_CHECK:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                PressCheckAutoRebirth();
            }
        }
        break;

    case CHECK_POINT_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                float fDecExpPercent = static_cast<float>( m_longDecExp / static_cast<float>(m_pGaeaClient->GetCharacter()->GET_LEVELUP_EXP()) * 100.0f );

                std::string strtext;
                strtext = sc::string::format( ID2GAMEINTEXT( "REBIRTH_WINDOW_INFORMATION_CHECK_POINT" ),
                    m_strCheckPoint.c_str(), m_longDecExp, fDecExpPercent );

                m_pTooltipText->ClearText();
                m_pTooltipText->AddText( strtext.c_str(), NS_UITEXTCOLOR::WHITE );

                // 툴팁 위치 설정.
                float fOffset = 10.0f;
                D3DXVECTOR2 vPos;

                const UIRECT& rcTarLP = m_pCheckPointRebirthButton->GetLocalPos();
                vPos.x = rcTarLP.right;
                vPos.y = rcTarLP.bottom;
                m_pTooltipBack->SetLocalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetLocalPos( vPos );

                const UIRECT& rcTarGP = m_pCheckPointRebirthButton->GetGlobalPos();
                vPos.x = rcTarGP.right;
                vPos.y = rcTarGP.bottom;
                m_pTooltipBack->SetGlobalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetGlobalPos( vPos );

                m_pTooltipText->SetVisibleSingle( TRUE );
                m_pTooltipBack->SetVisibleSingle( TRUE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_nDONE_ACTION = REBIRTH_CHECKPOINT;
                //CheckPointRebirth();

                UnShowAllButton();
            }
        }
        break;

    case RECOVERY_EXP_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                std::string strtext;
                strtext = sc::string::format( ID2GAMEINTEXT( "REBIRTH_WINDOW_INFORMATION_RECOVERY_EXP" ),
                    m_strCheckPoint.c_str(), m_longDecMoney, m_longReExp );

                m_pTooltipText->ClearText();
                m_pTooltipText->AddText( strtext.c_str(), NS_UITEXTCOLOR::WHITE );

                // 툴팁 위치 설정.
                float fOffset = 10.0f;
                D3DXVECTOR2 vPos;

                const UIRECT& rcTarLP = m_pExpRebirthButton->GetLocalPos();
                vPos.x = rcTarLP.right;
                vPos.y = rcTarLP.bottom;
                m_pTooltipBack->SetLocalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetLocalPos( vPos );

                const UIRECT& rcTarGP = m_pExpRebirthButton->GetGlobalPos();
                vPos.x = rcTarGP.right;
                vPos.y = rcTarGP.bottom;
                m_pTooltipBack->SetGlobalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetGlobalPos( vPos );

                m_pTooltipText->SetVisibleSingle( TRUE );
                m_pTooltipBack->SetVisibleSingle( TRUE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_nDONE_ACTION = REBIRTH_EXP;
                //RecoveryExpRebirth();

                UnShowAllButton();
            }
        }
        break;

    case CURRENT_POS_BUTTON:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                m_pTooltipText->ClearText();
                m_pTooltipText->AddText( ID2GAMEINTEXT( "REBIRTH_WINDOW_INFORMATION_CURRENT_POSITION" ),
                    NS_UITEXTCOLOR::WHITE );

                // 툴팁 위치 설정.
                float fOffset = 10.0f;
                D3DXVECTOR2 vPos;

                const UIRECT& rcTarLP = m_pCurrentPosRebirthButton->GetLocalPos();
                vPos.x = rcTarLP.right;
                vPos.y = rcTarLP.bottom;
                m_pTooltipBack->SetLocalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetLocalPos( vPos );

                const UIRECT& rcTarGP = m_pCurrentPosRebirthButton->GetGlobalPos();
                vPos.x = rcTarGP.right;
                vPos.y = rcTarGP.bottom;
                m_pTooltipBack->SetGlobalPos( vPos );
                vPos.x += fOffset;
                vPos.y += fOffset;
                m_pTooltipText->SetGlobalPos( vPos );

                m_pTooltipText->SetVisibleSingle( TRUE );
                m_pTooltipBack->SetVisibleSingle( TRUE );
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_nDONE_ACTION = REBIRTH_CURRENT_POSITION;
                //CurrentPosRebirth();

                UnShowAllButton();
            }
        }
        break;

    case SKILL_REBIRTH_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_nDONE_ACTION = REBIRTH_SKILL;

                UnShowAllButton();
            }
        }
        break;
    }

    if ( !m_pCheckPointRebirthButton->IsFlip() && !m_pExpRebirthButton->IsFlip()
        && !m_pCurrentPosRebirthButton->IsFlip() )
    {
        m_pTooltipText->ClearText();
        m_pTooltipText->SetVisibleSingle( FALSE );
        m_pTooltipBack->SetVisibleSingle( FALSE );
    }
}

HRESULT CRebirthWindow::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr = S_OK;
    hr = CUIWindowEx::RestoreDeviceObjects( pd3dDevice );
    if ( FAILED( hr ) ) return hr;

    const long lResolution = m_pInterface->UiGetResolution();
    WORD X_RES = HIWORD( lResolution );
    WORD Y_RES = LOWORD( lResolution );

    {
        const UIRECT& rebirthWindowLP = GetLocalPos();
        const UIRECT& rebirthWindowGP = GetGlobalPos();

        D3DXVECTOR2 vPos;

        vPos.x = ( X_RES - rebirthWindowLP.sizeX ) * 0.5f;
        vPos.y = ( Y_RES - rebirthWindowLP.sizeY ) * 0.5f;
        SetLocalPos( vPos );

        vPos.x = ( X_RES - rebirthWindowGP.sizeX ) * 0.5f;
        vPos.y = ( Y_RES - rebirthWindowGP.sizeY ) * 0.5f;
        SetGlobalPos( vPos );
    }

    return S_OK;
}

void CRebirthWindow::SetItemRebirth( bool bItemRebirth, bool bAutoRebirth, bool bCantUseRebirth )
{
    m_nDONE_ACTION = REBIRTH_NONE;
    m_fTIME_LEFT = fDEFAULT_TIME_LEFT;
    m_bAutoRebirth = false;
    m_fAutoRebirthTime = static_cast<float>( GLCONST_CHAR::wAUTO_REBIRTH_TIMER );

    SetRecommendItemImage();
    if ( RANPARAM::bCheckAutoRebirth )
    {
        m_pAutoRebirthCheck->SetVisibleSingle( TRUE );
        m_pAutoRebirthUnCheck->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pAutoRebirthCheck->SetVisibleSingle( FALSE );
        m_pAutoRebirthUnCheck->SetVisibleSingle( TRUE );
    }

    bool bGuidBattleMap(false);
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( pLand )
    {
        bGuidBattleMap = (pLand->m_bClubBattle || pLand->m_bClubDeathMatch ); 
    }

    ShowAllButton();

    EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();

    if ( bItemRebirth && !bGuidBattleMap )	// 귀혼주, 선도불가지역x, 귀혼불가지역x
    {
        {
            const UIRECT& rc2ButtonL_G = m_pTempLeftButton->GetGlobalPos();
            const UIRECT& rc2ButtonL_L = m_pTempLeftButton->GetLocalPos();

            m_pCheckPointRebirthButton->SetGlobalPos( rc2ButtonL_G );
            m_pCheckPointRebirthButton->SetLocalPos( rc2ButtonL_L );
        }
        {
            const UIRECT& rc2ButtonR_G = m_pTempRightButton->GetGlobalPos();
            const UIRECT& rc2ButtonR_L = m_pTempRightButton->GetLocalPos();

            m_pCurrentPosRebirthButton->SetGlobalPos( rc2ButtonR_G );
            m_pCurrentPosRebirthButton->SetLocalPos( rc2ButtonR_L );
        }

        m_pExpRebirthButton->SetVisibleSingle( FALSE );

        m_bAutoRebirth = bAutoRebirth;

        m_nACTION = REBIRTH_CURRENT_POSITION;

    }
    // 경험치회복_정의_Normal
    else if ( m_pGaeaClient->CanRecoveryExp() && !bGuidBattleMap )	// 귀혼주x, 귀혼불가지역x, 선도전지역x
    {
        {
            const UIRECT& rc2ButtonL_G = m_pTempLeftButton->GetGlobalPos();
            const UIRECT& rc2ButtonL_L = m_pTempLeftButton->GetLocalPos();

            m_pCheckPointRebirthButton->SetGlobalPos( rc2ButtonL_G );
            m_pCheckPointRebirthButton->SetLocalPos( rc2ButtonL_L );
        }

        {
            const UIRECT& rc2ButtonR_G = m_pTempRightButton->GetGlobalPos();
            const UIRECT& rc2ButtonR_L = m_pTempRightButton->GetLocalPos();

            m_pExpRebirthButton->SetGlobalPos( rc2ButtonR_G );
            m_pExpRebirthButton->SetLocalPos( rc2ButtonR_L );
        }

        {
            m_pCurrentPosRebirthButton->SetVisibleSingle( FALSE );
        }

        m_nACTION = REBIRTH_EXP;
    }
    else
    {
        {
            const UIRECT& rc1BUTTON = m_pTempRightButton->GetGlobalPos();
            D3DXVECTOR2 vPos ( rc1BUTTON.left, rc1BUTTON.top );
            m_pCheckPointRebirthButton->SetGlobalPos( vPos );
        }

        {
            m_pCurrentPosRebirthButton->SetVisibleSingle( FALSE );
            m_pExpRebirthButton->SetVisibleSingle( FALSE );
        }

        m_nACTION = REBIRTH_CHECKPOINT;
    }

    if ( m_bReviveSkill ) 
    {
        m_pSkillRebirthButton->SetVisibleSingle( TRUE );

        switch ( m_nACTION )
        {
        case REBIRTH_CURRENT_POSITION:
            {
                m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );
            }
            break;

        case REBIRTH_EXP:
            {
                m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );
            }
            break;

        case REBIRTH_CHECKPOINT:
            {
            }
            break;
        }
    }
    else
        m_pSkillRebirthButton->SetVisibleSingle( FALSE );


    PostSetItemRebirth();
    NotifyRebirthLossInformation();
}

void CRebirthWindow::PostSetItemRebirth()
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

    if ( pChar == NULL )
    {
        return;
    }

    // 귀환 지점 부활 상태;
    if (pChar->m_sResurrectAbleState.isState(EM_RESURRECTION_NOMAL) == false)
    {
        m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );
        m_pAutoRebirthCheck->SetVisibleSingle( FALSE );
        m_pAutoRebirthUnCheck->SetVisibleSingle( FALSE );
        m_pAutoRebirthText->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pAutoRebirthText->SetVisibleSingle( TRUE );
    }

    // 경험치 부활 상태;
    if (pChar->m_sResurrectAbleState.isState(EM_RESURRECTION_SPECIAL) == false)
    {
        m_pExpRebirthButton->SetVisibleSingle( FALSE );
        m_pRebirthInfoText->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pRebirthInfoText->SetVisibleSingle( TRUE );
    }

    // 아이템(귀혼주) 부활 상태;
    if (pChar->m_sResurrectAbleState.isState(EM_RESURRECTION_SPECIAL_ITEM) == false)
    {
        m_pCurrentPosRebirthButton->SetVisibleSingle( FALSE );
        m_pRecommendItem->SetVisibleSingle( FALSE );
        m_pRecommandBack->SetVisibleSingle( FALSE );
        m_pTooltipText->SetVisibleSingle( FALSE );
        m_pTooltipBack->SetVisibleSingle( FALSE );
        m_pBuyButton->SetVisibleSingle( FALSE );
    }
    else
    {
        m_pBuyButton->SetVisibleSingle( TRUE );
    }

    // 스킬 부활 상태;
    if (pChar->m_sResurrectAbleState.isState(EM_RESURRECTION_SKILL) == false)
    {
        m_pSkillRebirthButton->SetVisibleSingle( FALSE );
    }

    // 특수 상황
    // 현재 시간 문제상 아쉽게도 SetItemRebirth() 코드를
    // PostSetItemRebirth() 포팅을 하지 못했다.
    // 향후 코드를 상태값으로 변화시켜 코드를 처리 해야 한다.    
    if ( pChar->m_sResurrectAbleState == EM_RESURRECTION_SKILL )
    {
        m_nACTION = REBIRTH_SKILL;
    }
}

bool CRebirthWindow::CapableVisible()
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

    if ( pChar == NULL )
    {
        return false;
    }

    // 부활가능한 상태가 없다면 부활창을 보이지 않게 함;
    if ( pChar->m_sResurrectAbleState == NULL )
    {
        return false;
    }

    return true;
}

void CRebirthWindow::SetReviveSkill( bool bAct )
{
    m_bReviveSkill = bAct;

    if ( bAct )
    {
        if ( m_nDONE_ACTION == REBIRTH_NONE )
        {
            m_pSkillRebirthButton->SetVisibleSingle( TRUE );

            switch ( m_nACTION )
            {
            case REBIRTH_CURRENT_POSITION:
                {
                    m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );
                }
                break;

            case REBIRTH_EXP:
                {
                    m_pCheckPointRebirthButton->SetVisibleSingle( FALSE );
                }
                break;

            case REBIRTH_CHECKPOINT:
                {
                }
                break;
            }            
        }
    }
    else
    {
        m_pSkillRebirthButton->SetVisibleSingle( FALSE );
    }
}

void CRebirthWindow::ShowAllButton()
{
    m_pCheckPointRebirthButton->SetVisibleSingle ( TRUE );
    m_pCurrentPosRebirthButton->SetVisibleSingle ( TRUE );
    m_pExpRebirthButton->SetVisibleSingle( TRUE );
    m_pSkillRebirthButton->SetVisibleSingle( TRUE );
}

void CRebirthWindow::UnShowAllButton()
{
    m_pCheckPointRebirthButton->SetVisibleSingle ( FALSE );
    m_pCurrentPosRebirthButton->SetVisibleSingle ( FALSE );
    m_pExpRebirthButton->SetVisibleSingle( FALSE );
    m_pSkillRebirthButton->SetVisibleSingle( FALSE );
}

void CRebirthWindow::NotifyRebirthLossInformation()
{
    std::string strLossInfo = "";

    GetExpRecovery( m_longDecExp, m_longReExp, m_longDecMoney );

    m_longDecReExp = m_longDecExp - m_longReExp;

    switch ( m_nACTION )
    {
    case REBIRTH_CURRENT_POSITION:
        {
            float fDecExpPercent = static_cast<float>( m_longDecExp / static_cast<float>(m_pGaeaClient->GetCharacter()->GET_LEVELUP_EXP()) * 100.0f );

            strLossInfo = sc::string::format( ID2GAMEINTEXT( "REBIRTH_WINDOW_NOTIFY_CURRENT_POS_REBIRTH" ),
                m_longDecExp, fDecExpPercent );
        }
        break;

    case REBIRTH_EXP:
        {
            float fDecExpPercent = static_cast<float>( m_longDecExp / static_cast<float>(m_pGaeaClient->GetCharacter()->GET_LEVELUP_EXP()) * 100.0f );
            float fDecReExpPercent = static_cast<float>( m_longDecReExp / static_cast<float>(m_pGaeaClient->GetCharacter()->GET_LEVELUP_EXP()) * 100.0f );

            strLossInfo = sc::string::format( ID2GAMEINTEXT( "REBIRTH_WINDOW_NOTIFY_RECOVERY_EXP" ),
                m_longDecExp, fDecExpPercent,
                m_longDecReExp, fDecReExpPercent );
        }
        break;

    case REBIRTH_CHECKPOINT:
        {
            strLossInfo = sc::string::format( "%s", ID2GAMEINTEXT( "REBIRTH_WINDOW_NOTIFY_CHECK_POS_REBIRTH" ) );
        }
        break;
    }

    m_pRebirthInfoText->ClearText();
    m_pRebirthInfoText->AddText( strLossInfo.c_str(), NS_UITEXTCOLOR::ORANGE );

    SMAPNODE *pNODE = m_pGaeaClient->FindMapNode( m_pGaeaClient->GetCharacter()->m_sStartMapID );
    if ( pNODE )
    {
        m_strCheckPoint = m_pGaeaClient->GetMapName(pNODE->GetId());
    }
}

void CRebirthWindow::GetExpRecovery( LONGLONG& _longDecExp, LONGLONG& _longReExp, LONGLONG& _longDecMoney )
{
    LONGLONG nReExp = 0;
    LONGLONG nDecMoney = 0;
    LONGLONG nDecExp = 0;

    WORD wCharLev = m_pGaeaClient->GetCharacter()->GETLEVEL();
    LONGLONG longLevUpExp = m_pGaeaClient->GetCharacter()->GET_LEVELUP_EXP();

    nDecExp = __int64 ( GLOGICEX::GLDIE_DECEXP( wCharLev ) * 0.01f * longLevUpExp );

    LONGLONG longExpLow = m_pGaeaClient->GetCharacter()->m_sExperience.lnLow;
    if ( nDecExp > longExpLow ) nDecExp = longExpLow;

    _longDecExp = nDecExp;
    
    {
        // 회복되는 경험치.
        if ( nDecExp <= 0 )	nReExp = 0;


        float fRExpRecovery = GLOGICEX::GLDIE_RECOVERYEXP( wCharLev );
        nReExp = (LONGLONG)( nDecExp * fRExpRecovery );

        if ( nReExp < 0  ) nReExp = 0;	
        if ( nDecExp < nReExp ) nReExp = nDecExp;

        _longReExp = nReExp;
    }

    {
        // 소모되는 돈.
        if ( nReExp <= 0 ) nReExp = 0;

        float fRMoney = GLOGICEX::GLDIE_EXPMONEY( wCharLev );
        nDecMoney = (LONGLONG)( nReExp * fRMoney );

        _longDecMoney = nDecMoney;
    }
}

void CRebirthWindow::CheckPointRebirth()
{
    if ( m_pGaeaClient->GetCharacter()->IsDie() )
    {
        // 부활하기
        m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_NOMAL);
        SetReviveSkill( false );

        // 열려진 창들 닫기
        m_pInterface->CloseAllWindow ();				
    }
}

void CRebirthWindow::RecoveryExpRebirth()
{
    if ( m_pGaeaClient->GetCharacter()->IsDie() )  
    {	
        m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SPECIAL);
        SetReviveSkill( false );
    }
}

void CRebirthWindow::CurrentPosRebirth()
{
    if ( m_pGaeaClient->GetCharacter()->IsDie() )
    {
        // 귀혼주 사용
        m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SPECIAL_ITEM);
        SetReviveSkill( false );

        // 열려진 창들 닫기
        m_pInterface->CloseAllWindow();
    }
}

void CRebirthWindow::SKillRebirth()
{
    if ( m_pGaeaClient->GetCharacter()->IsDie() )
    {
        m_pGaeaClient->GetCharacter()->ReqResurrect(EM_RESURRECTION_SKILL);
        SetReviveSkill( false );

        // 열려진 창들 닫기
        m_pInterface->CloseAllWindow();
    }
}

void CRebirthWindow::PointshopOpenRecommendItem()
{
    if ( GLUseFeatures::GetInstance().IsUsingPointShop() )
    {
        if ( !m_pInterface->UiIsVisibleGroup( POINT_SHOP_WINDOW ) )
        {
            m_pInterface->UiShowGroupFocus( POINT_SHOP_WINDOW, true );
        }

        m_pInterface->GetPointShopWindow()->SearchItem( GLCONST_CHAR::strREBIRTH_RECOMMEND_ITEM );
    }
}

void CRebirthWindow::SetRecommendItemImage()
{
    SNATIVEID sItemID = GLCONST_CHAR::nidREBIRTH_RECOMMEND_ITEM;
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemID );
    if ( !pItem )
    {
        GASSERT ( 0 && "등록되지 않은 ID입니다." );
        return ;
    }

    m_pRecommendItem->SetItem( sItemID );
    m_pRecommendItem->SetVisibleSingle( TRUE );
}

void CRebirthWindow::PressCheckAutoRebirth()
{
    if ( RANPARAM::bCheckAutoRebirth )
    {
        RANPARAM::bCheckAutoRebirth = false;

        m_pAutoRebirthCheck->SetVisibleSingle( FALSE );
        m_pAutoRebirthUnCheck->SetVisibleSingle( TRUE );
    }
    else
    {
        RANPARAM::bCheckAutoRebirth = true;

        m_pAutoRebirthCheck->SetVisibleSingle( TRUE );
        m_pAutoRebirthUnCheck->SetVisibleSingle( FALSE );
    }
}

void CRebirthWindow::SetAutoRebirthTime( UINT iTime )
{
    if ( iTime <= 0 )
        m_fAutoRebirthTime = static_cast<float>( GLCONST_CHAR::wAUTO_REBIRTH_TIMER );
    else
        m_fAutoRebirthTime = static_cast<float>( iTime );
}

MyRebirthWindow::MyRebirthWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CRebirthWindow(pGaeaClient, pInterface, pEngineDevice)
{
}

void MyRebirthWindow::CreateUIWindowAndRegisterOwnership()
{
    CRebirthWindow::Create( REBIRTH_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    {
        CRebirthWindow::CreateTitle( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", NULL );
        
        EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
        if (ServiceProvider == SP_OFFICE_TEST ||
            ServiceProvider == SP_KOREA ||
            ServiceProvider == SP_KOREA_TEST ||            
            ServiceProvider == SP_TAIWAN ||    
            ServiceProvider == SP_HONGKONG ||
            ServiceProvider == SP_INDONESIA ||
            ServiceProvider == SP_VIETNAM ||
            ServiceProvider == SP_JAPAN ||
            ServiceProvider == SP_CHINA ||
            ServiceProvider == SP_WORLD_BATTLE)
        {
            CRebirthWindow::CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", NULL );
        }

        CRebirthWindow::CreateBody( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
        CRebirthWindow::ResizeControl( "REBIRTH_WINDOW" );

        WORD wFlag = GetAlignFlag();
        wFlag &= ~UI_FLAG_XSIZE;
        wFlag &= ~UI_FLAG_YSIZE;
        CRebirthWindow::SetAlignFlag( wFlag );
    }
    CRebirthWindow::CreateSubControl();
    CRebirthWindow::m_pInterface->UiRegisterControl( this );
    CRebirthWindow::m_pInterface->UiShowGroupFocus( REBIRTH_WINDOW );
}

void MyRebirthWindow::SetItemRebirth( bool bItemRebirth, bool bAutoRebirth, bool bCantUseRebirth )
{
    CRebirthWindow::SetItemRebirth( bItemRebirth, bAutoRebirth, bCantUseRebirth );
}

void MyRebirthWindow::SetReviveSkill( bool bAct )
{
    CRebirthWindow::SetReviveSkill( bAct );
}

void MyRebirthWindow::SetAutoRebirthTime( UINT iTime )
{
    CRebirthWindow::SetAutoRebirthTime( iTime );
}

bool MyRebirthWindow::CapableVisible()
{
    return CRebirthWindow::CapableVisible();
}

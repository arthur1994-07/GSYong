#include "StdAfx.h"
//
//#include "../../../SigmaCore/String/StringFormat.h"
//
//#include "../../../EngineLib/DxTools/DxFontMan.h"
//#include "../../../EngineLib/DxTools/d3dfont.h"
//#include "../../../EngineLib/GUInterface/UITextControl.h"
//#include "../../../EngineLib/GUInterface/GameTextControl.h"
//#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
//#include "../../../EngineLib/GUInterface/BasicTextBox.h"
//
//#include "../../InnerInterface.h"
//#include "./CompetitionAlarm.h"
//
//#include <boost/lambda/lambda.hpp>
//#include <boost/lambda/bind.hpp>
//
//// ----------------------------------------------------------------------------
//#include "../../../SigmaCore/DebugInclude.h"
//// ----------------------------------------------------------------------------
//
//CCompetitionAlarm::CCompetitionAlarm(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
//: CUIGroup(pEngineDevice)
//, m_pGaeaClient(pGaeaClient)
//, m_pInterface(pInterface)
//{
//}
//
//CCompetitionAlarm::~CCompetitionAlarm ()
//{
//}
//
//void CCompetitionAlarm::CreateSubControl ()
//{
//    {
//        CUIControl BaseButton(m_pEngineDevice); 
//        BaseButton.Create ( 1, "COMPETITION_ALARM_BASE_BUTTON" );
//
//        CUIControl BaseGap(m_pEngineDevice); 
//        BaseGap.Create ( 1, "COMPETITION_ALARM_BASE_BUTTON_GAP" );
//
//        CUIControl BaseShow(m_pEngineDevice); 
//        BaseShow.Create ( 1, "COMPETITION_ALARM_BUTTON_SHOW" );
//
//        CUIControl BaseHide(m_pEngineDevice); 
//        BaseHide.Create ( 1, "COMPETITION_ALARM_BUTTON_HIDE" );
//
//        m_rcBaseButton = BaseButton.GetLocalPos();
//        m_rcBaseGap    = BaseGap.GetLocalPos();
//        m_rcBaseShow   = BaseShow.GetLocalPos();
//        m_rcBaseHide   = BaseHide.GetLocalPos();
//    }
//
//    // 버튼
//    SBUTTON* pButton = NULL;
//
//    pButton = CreateButton( COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD );
//    pButton->SetText( "(s)발전소 점령" );
//
//    pButton = CreateButton( COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP2 );
//    pButton->SetText( "(s)발전소 임시 2" );
//
//    pButton = CreateButton( COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP3 );
//    pButton->SetText( "(s)발전소 임시 3" );
//
//    pButton = CreateButton( COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP4 );
//    pButton->SetText( "(s)발전소 임시 4" );
//}
//
//void CCompetitionAlarm::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
//{
//    m_fMousePosX = static_cast<float>(x);
//    m_fMousePosY = static_cast<float>(y);
//
//    BUTTONLIST_ITER iter = m_VisibleButtonList.begin();
//    UINT nIndex = 0;
//
//    for ( ; iter != m_VisibleButtonList.end(); ++nIndex )
//    {
//        SBUTTON* pButton = (*iter);
//
//        if ( !pButton )
//        {
//            ++iter;
//            continue;
//        }
//
//        SBUTTON::EMSTATE emState = pButton->GetState();
//
//        switch ( emState )
//        {
//        case SBUTTON::EMSTATE_DEFAULT:
//            {
//                const UIRECT& rcButtonLocal = pButton->GetLocalPos();
//                float fHeight = GetButtonPostionYByIndex(nIndex);
//
//                if ( fHeight < rcButtonLocal.top )
//                {
//                    pButton->SetMovingDestHeight( fHeight );
//                    pButton->SetState( SBUTTON::EMSTATE_MOVING );
//                }
//            }
//            break;
//
//        case SBUTTON::EMSTATE_END:
//            {
//                pButton->SetVisibleSingle( FALSE );
//                iter = m_VisibleButtonList.erase( iter );
//                continue;
//            }
//            break;
//        }
//
//        ++iter;
//    }
//
//    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
//}
//
//void CCompetitionAlarm::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
//{
//    switch ( ControlID )
//    {
//    case COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD:
//        {
//            if ( dwMsg&UIMSG_CLICK_COMPETITION_ALARM_BUTTON )
//            {
//                m_pInterface->PopupMenuClearItem();
//                m_pInterface->PopupMenuAddItem("(s)참여", boost::lambda::bind( &CCompetitionAlarm::JoinCaptureTheField, this ) );
//                m_pInterface->PopupMenuOpen(NULL, m_fMousePosX, m_fMousePosY);
//            }
//
//            if ( CHECK_MOUSE_IN(dwMsg) )
//            {
//                AddMessageEx( UIMSG_MOUSEIN_COMPETITION_ALARM_BUTTON );
//            }
//        }
//        break;
//    };
//}
//
//CCompetitionAlarm::SBUTTON* CCompetitionAlarm::CreateButton ( const UIGUID cID )
//{
//    std::string strControl = "COMPETITION_ALARM_BASE_BUTTON";
//
//    SBUTTON* pButton = new SBUTTON(m_pEngineDevice);
//    pButton->CreateSub( this, strControl.c_str(), UI_FLAG_DEFAULT, cID );
//    pButton->CreateSubControl();
//    pButton->SetVisibleSingle( FALSE );
//    RegisterControl( pButton );
//
//    // 추가한만큼 컨트롤의 사이즈를 늘린다.
//    //---------------------------------//
//    const UIRECT& rcLocal  = GetLocalPos();
//    const UIRECT& rcGlobal = GetGlobalPos();
//    SetLocalPos  ( UIRECT(rcLocal.left, rcLocal.top, rcLocal.sizeX, rcLocal.sizeY + m_rcBaseButton.sizeY + m_rcBaseGap.sizeY) );
//    SetGlobalPos ( UIRECT(rcGlobal.left, rcGlobal.top, rcGlobal.sizeX, rcGlobal.sizeY + m_rcBaseButton.sizeY + m_rcBaseGap.sizeY) );
//
//    return pButton;
//}
//
//void CCompetitionAlarm::ShowButton ( const UIGUID cID )
//{
//    SBUTTON* pButton = GetButton ( cID );
//
//    if ( !pButton )
//    {
//        return;
//    }
//
//    using namespace boost::lambda;
//    BUTTONLIST_ITER iter = std::find_if( m_VisibleButtonList.begin(), m_VisibleButtonList.end(),
//        boost::lambda::bind( &CUIControl::GetWndID, boost::lambda::_1 ) == cID );
//
//    if ( iter != m_VisibleButtonList.end() )
//    {
//        return;
//    }
//
//    const UIRECT& rcGlobal = pButton->GetGlobalPos();
//
//    float fPositionY = GetButtonPostionYByIndex( static_cast<UINT>(m_VisibleButtonList.size()) );
//
//    UIRECT rcShow = UIRECT( m_rcBaseShow.left, fPositionY, m_rcBaseShow.sizeX, m_rcBaseShow.sizeY );
//    UIRECT rcHide = UIRECT( m_rcBaseHide.left, fPositionY, m_rcBaseHide.sizeX, m_rcBaseHide.sizeY );
//
//    pButton->SetLocalPos( D3DXVECTOR2(rcHide.left, fPositionY) );
//    pButton->SetGlobalPos( D3DXVECTOR2(rcGlobal.left + rcHide.left, rcGlobal.top + fPositionY) );
//    pButton->SetShowRect( rcShow );
//    pButton->SetHideRect( rcHide );
//    pButton->SetAlarm( true );
//    pButton->SetVisibleSingle( TRUE );
//    pButton->SetState( SBUTTON::EMSTATE_SHOW );
//    m_VisibleButtonList.push_back( pButton );
//}
//
//void CCompetitionAlarm::HideButton ( const UIGUID cID )
//{
//    SBUTTON* pButton = GetButton ( cID );
//
//    if ( !pButton )
//    {
//        return;
//    }
//
//    using namespace boost::lambda;
//    BUTTONLIST_ITER iter = std::find_if( m_VisibleButtonList.begin(), m_VisibleButtonList.end(),
//        boost::lambda::bind( &CUIControl::GetWndID, boost::lambda::_1 ) == cID );
//
//    if ( iter == m_VisibleButtonList.end() )
//    {
//        return;
//    }
//
//    const UIRECT& rcButtonLocal = pButton->GetLocalPos();
//
//    UIRECT rcShow = UIRECT( m_rcBaseShow.left, rcButtonLocal.top, m_rcBaseShow.sizeX, m_rcBaseShow.sizeY );
//    UIRECT rcHide = UIRECT( m_rcBaseHide.left, rcButtonLocal.top, m_rcBaseHide.sizeX, m_rcBaseHide.sizeY );
//
//    pButton->SetLocalPos( D3DXVECTOR2(rcHide.left, rcButtonLocal.top) );
//    pButton->SetShowRect( rcShow );
//    pButton->SetHideRect( rcHide );
//    pButton->SetState( SBUTTON::EMSTATE_HIDE );
//}
//
//CCompetitionAlarm::SBUTTON* CCompetitionAlarm::GetButton ( const UIGUID cID )
//{
//    CUIControl* pControl = FindControl ( cID );
//    if ( !pControl )
//    {
//        GASSERT( !"CCompetitionAlarm::GetButton - pControl is null" );
//        return NULL;
//    }
//
//    SBUTTON* pButton = dynamic_cast<SBUTTON*>(pControl);
//    if ( !pButton )
//    {
//        GASSERT( !"CCompetitionAlarm::GetButton - dynamic_cast<SBUTTON*>(pControl) is failed" );
//        return NULL;
//    }
//
//    return pButton;
//}
//
//const float CCompetitionAlarm::GetButtonPostionYByIndex ( const UINT nIndex )
//{
//    return (m_rcBaseButton.sizeY*static_cast<float>(nIndex)) + (m_rcBaseGap.sizeY*static_cast<float>(nIndex));
//}
//
//void CCompetitionAlarm::JoinCaptureTheField()
//{
//
//}
//
////----------------------------------------------------------------------------------------------------//
//
//const float fBUTTON_MOVING_SPEED      = 100.0f;
//const float fBUTTON_ALARM_DIFFUSE_MIN = 60.0f;
//
//void CCompetitionAlarm::SBUTTON::CreateSubControl()
//{
//    CD3DFontPar* pFont  = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
//
//    pLINEBOX = new CBasicLineBoxSmart(m_pEngineDevice);
//    pLINEBOX->CreateSub( this, "COMPETITION_ALARM_BUTTON_LINEBOX", UI_FLAG_DEFAULT, ID_BUTTON );
//    pLINEBOX->CreateSubControl( "COMPETITION_ALARM_BUTTON_LINEBOX_TEXINFO" );
//    RegisterControl( pLINEBOX );
//
//    pTEXTBOX = new CBasicTextBox(m_pEngineDevice);
//    pTEXTBOX->CreateSub( this, "COMPETITION_ALARM_BUTTON_LINEBOX" );
//    pTEXTBOX->SetFont( pFont );
//    pTEXTBOX->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
//    RegisterControl( pTEXTBOX );
//
//    SetState ( EMSTATE_DEFAULT );
//}
//
//void CCompetitionAlarm::SBUTTON::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
//{
//    switch ( emSTATE )
//    {
//    case EMSTATE_DEFAULT:
//        {
//            UpdateAlarm ( fElapsedTime );
//
//            if ( bALARM )
//            {
//                pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
//            }
//            else
//            {
//                SetDiffuse( NS_UITEXTCOLOR::WHITE );
//                pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
//            }
//        }
//        break;
//
//    case EMSTATE_SHOW:
//        {
//            fVISIBLETIME += fElapsedTime*1.5f;
//
//            if ( fVISIBLETIME >= 1.0f )
//            {
//                fVISIBLETIME = 1.0f;
//                SetState( EMSTATE_DEFAULT );
//            }
//
//            SetDiffuseAlpha( (WORD)(255.0f*fVISIBLETIME) );
//
//            UIRECT rcAniLocal(
//                rcVISIBLEHIDE.left  *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.left  *fVISIBLETIME,
//                rcVISIBLEHIDE.top   *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.top   *fVISIBLETIME,
//                rcVISIBLEHIDE.sizeX *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.sizeX *fVISIBLETIME,
//                rcVISIBLEHIDE.sizeY *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.sizeY *fVISIBLETIME
//                );
//            
//            SetLocalPos( rcAniLocal );
//
//            if ( GetParent() )
//            {
//                const UIRECT& rcParentGlobalPos = GetParent()->GetGlobalPos();
//                SetGlobalPos( D3DXVECTOR2(rcParentGlobalPos.left + rcAniLocal.left, rcParentGlobalPos.top + rcAniLocal.top) );
//            }
//            else
//            {
//                SetGlobalPos( D3DXVECTOR2(rcAniLocal.left, rcAniLocal.top) );
//            }
//        }
//        break;
//        
//    case EMSTATE_HIDE:
//        {
//            fVISIBLETIME -= fElapsedTime*1.5f;
//
//            if ( fVISIBLETIME <= 0.0f )
//            {
//                fVISIBLETIME = 0.0f;
//                SetState( EMSTATE_END );
//            }
//
//            SetDiffuseAlpha( (WORD)(255.0f*fVISIBLETIME) );
//
//            UIRECT rcAniLocal(
//                rcVISIBLEHIDE.left  *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.left  *fVISIBLETIME,
//                rcVISIBLEHIDE.top   *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.top   *fVISIBLETIME,
//                rcVISIBLEHIDE.sizeX *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.sizeX *fVISIBLETIME,
//                rcVISIBLEHIDE.sizeY *(1.0f-fVISIBLETIME) + rcVISIBLESHOW.sizeY *fVISIBLETIME
//                );
//
//            SetLocalPos( rcAniLocal );
//
//            if ( GetParent() )
//            {
//                const UIRECT& rcParentGlobalPos = GetParent()->GetGlobalPos();
//                SetGlobalPos( D3DXVECTOR2(rcParentGlobalPos.left + rcAniLocal.left, rcParentGlobalPos.top + rcAniLocal.top) );
//            }
//            else
//            {
//                SetGlobalPos( D3DXVECTOR2(rcAniLocal.left, rcAniLocal.top) );
//            }
//        }
//        break;
//        
//    case EMSTATE_MOVING:
//        {
//            fMOVINGSPEEDTIME += fElapsedTime*2.0f;
//
//            if ( fMOVINGSPEEDTIME >= 1.0f )
//            {
//                fMOVINGSPEEDTIME = 1.0f;
//            }
//
//            UIRECT rcLocal = GetLocalPos();
//
//            if ( fMOVINGDEST <= rcLocal.top )
//            {
//                rcLocal.top -= fBUTTON_MOVING_SPEED * fMOVINGSPEEDTIME * fElapsedTime;
//
//                if ( rcLocal.top <= fMOVINGDEST )
//                {
//                    rcLocal.top = fMOVINGDEST;
//                    fMOVINGSPEEDTIME = 0.0f;
//                    SetState( EMSTATE_DEFAULT );
//                }
//            }
//            else
//            {
//                rcLocal.top += fBUTTON_MOVING_SPEED * fMOVINGSPEEDTIME * fElapsedTime;
//
//                if ( fMOVINGDEST <= rcLocal.top )
//                {
//                    rcLocal.top = fMOVINGDEST;
//                    fMOVINGSPEEDTIME = 0.0f;
//                    SetState( EMSTATE_DEFAULT );
//                }
//            }
//
//            SetLocalPos( D3DXVECTOR2(rcLocal.left, rcLocal.top) );
//            
//            if ( GetParent() )
//            {
//                const UIRECT& rcParentGlobalPos = GetParent()->GetGlobalPos();
//                SetGlobalPos( D3DXVECTOR2(rcParentGlobalPos.left + rcLocal.left, rcParentGlobalPos.top + rcLocal.top) );
//            }
//            else
//            {
//                SetGlobalPos( D3DXVECTOR2(rcLocal.left, rcLocal.top) );
//            }
//
//            UpdateAlarm ( fElapsedTime );
//        }
//        break;
//
//    case EMSTATE_END:
//        {
//            return;
//        }
//        break;
//    };
//
//    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
//}
//
//void CCompetitionAlarm::SBUTTON::UpdateAlarm ( float fElapsedTime )
//{
//    if ( !bALARM )
//    {
//        return;
//    }
//
//    fALARMTIME += fElapsedTime*1.2f;
//
//    while ( fALARMTIME >= 2.0f )
//    {
//        fALARMTIME -= 2.0f;
//    }
//
//    float fRate     = (1.0f + cosf(fALARMTIME*D3DX_PI))/2.0f;
//    DWORD dwDiffuse = (DWORD)((255.0f * fRate) + (fBUTTON_ALARM_DIFFUSE_MIN*(1.0f-fRate)));
//
//    SetDiffuse( D3DCOLOR_ARGB(0xFF, dwDiffuse, dwDiffuse, dwDiffuse) );
//}
//
//void CCompetitionAlarm::SBUTTON::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
//{
//    if ( emSTATE != EMSTATE_DEFAULT )
//    {
//        return;
//    }
//
//    switch ( cID )
//    {
//    case ID_BUTTON:
//        {
//            if( CHECK_MOUSE_IN(dwMsg) && (dwMsg&(UIMSG_LB_DOWN | UIMSG_LB_PRESEED | UIMSG_LB_DRAG)) )
//            {
//                fALARMTIME = 0.0f;
//                SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
//                pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::DIMGREY );
//            }
//            else if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
//            {
//                SetAlarm ( false );
//                AddMessageEx( UIMSG_CLICK_COMPETITION_ALARM_BUTTON );
//
//                SetDiffuse( NS_UITEXTCOLOR::WHITE );
//                pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::ORANGE );
//            }
//            else if ( CHECK_MOUSE_IN(dwMsg) )
//            {
//                fALARMTIME = 0.0f;
//                SetDiffuse( NS_UITEXTCOLOR::WHITE );
//                pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::ORANGE );
//            }
//        }
//        break;
//    };
//}
//
//void CCompetitionAlarm::SBUTTON::SetAlarm ( const bool bEnable )
//{
//    bALARM = bEnable;
//    fALARMTIME = 0.0f;
//}
//
//void CCompetitionAlarm::SBUTTON::SetText ( const std::string& strCaption )
//{
//    if ( !pTEXTBOX )
//    {
//        return;
//    }
//
//    pTEXTBOX->SetText( strCaption.c_str() );
//    pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
//    pTEXTBOX->SetUseTextColor( 0, TRUE );
//}
//
//void CCompetitionAlarm::SBUTTON::SetState( const EMSTATE emState )
//{
//    if ( emSTATE == emState )
//    {
//        return;
//    }
//
//    emSTATE = emState;
//
//    switch ( emSTATE )
//    {
//    case EMSTATE_DEFAULT: 
//        {
//            SetDiffuse( NS_UITEXTCOLOR::WHITE );
//            pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
//        }
//        break;
//
//    case EMSTATE_SHOW: 
//        {
//            SetDiffuseAlpha( 0 );
//        }
//        break;
//
//    case EMSTATE_HIDE: 
//    case EMSTATE_MOVING:
//        {
//            SetDiffuse( NS_UITEXTCOLOR::WHITE );
//            pTEXTBOX->SetTextColor( 0, NS_UITEXTCOLOR::WHITE );
//        }
//        break;
//    };
//}
//
////----------------------------------------------------------------------------------------------------//
//
//void CCompetitionAlarm::ShowTemp ()
//{
//    static int aa = 0;
//
//    int nIndex = 0;
//    int nMax = COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP4-COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD;
//
//    while ( nIndex <= nMax )
//    {
//        UIGUID cd = aa%(COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP4-COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD+1) + COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD;
//        
//        using namespace boost::lambda;
//        BUTTONLIST_ITER iter = std::find_if( m_VisibleButtonList.begin(), m_VisibleButtonList.end(),
//            boost::lambda::bind( &CUIControl::GetWndID, boost::lambda::_1 ) == cd );
//
//        if ( iter == m_VisibleButtonList.end() )
//        {
//            ShowButton( cd );
//            break;
//        }
//
//        ++aa;
//        ++nIndex;
//    }
//}
//
//void CCompetitionAlarm::HideTemp ()
//{
//    BUTTONLIST_ITER iter = m_VisibleButtonList.begin();
//    int nSize = m_VisibleButtonList.size();
//    int nIndex = 0;
//
//    for ( ; iter != m_VisibleButtonList.end(); ++iter, ++nIndex )
//    {
//        SBUTTON* pButton = (*iter);
//
//        if ( !pButton )
//        {
//            continue;
//        }
//
//        if ( SBUTTON::EMSTATE_HIDE != pButton->GetState() )
//        {
//            if ( nIndex+1 == nSize )
//            {
//                HideButton( pButton->GetWndID() );
//                break;
//            }
//            else if ( rand()%2 )
//            {
//                HideButton( pButton->GetWndID() );
//                break;
//            }
//        }
//    }
//}
//
//MyCompetitionAlarm::MyCompetitionAlarm( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
//: CCompetitionAlarm( pGaeaClient, pInterface, pEngineDevice )
//{
//}
//
//void MyCompetitionAlarm::CreateUIWindowAndRegisterOwnership()
//{
//    CCompetitionAlarm::Create( COMPETITION_ALARM, "COMPETITION_ALARM", UI_FLAG_RIGHT );
//    CCompetitionAlarm::CreateSubControl();
//    CCompetitionAlarm::m_pInterface->UiRegisterControl( this );
//    CCompetitionAlarm::m_pInterface->UiShowGroupBottom( COMPETITION_ALARM );
//}
//
//void MyCompetitionAlarm::ShowCaptureTheField()
//{
//    //CCompetitionAlarm::ShowCaptureTheField();
//    CCompetitionAlarm::ShowTemp();
//    
//}
//
//void MyCompetitionAlarm::HideCaptureTheField()
//{
//    CCompetitionAlarm::HideTemp();
//    //CCompetitionAlarm::HideCaptureTheField();
//
//}
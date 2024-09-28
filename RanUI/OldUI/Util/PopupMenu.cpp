#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"

#include "../../StaticUIManager.h"
#include "./PopupMenuComponent.h"
#include "./PopupMenu.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CPopupMenu::nMAX_POPUP = 5;
const float CPopupMenu::fPOSITION_OFFSET = 3.0f;
const float CPopupMenu::fTEXBOX_OFFSET = 5.0f;

CPopupMenu::CPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroupHelper(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pDummyControl(NULL)
    , m_pLineBoxDummyControl(NULL)
    , m_pTextBoxDummyControl(NULL)
    , m_bInitPostion(false)
    , CurPosX(0.0f)
    , CurPosY(0.0f)
{
    m_vecLineBox.clear();
    m_vecTextBox.clear();
    m_vecPos.clear();
}

CPopupMenu::~CPopupMenu ()
{
}

void CPopupMenu::CreateSubControl ()
{
    {
        CUIControl* pControl = new CUIControl(GetEngineDeviceMan());
        pControl->CreateSub( this, "POPUPMENU" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pDummyControl = pControl;

        pControl = new CUIControl(GetEngineDeviceMan());
        pControl->CreateSub( this, "POPUPMENU_BOX" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pLineBoxDummyControl = pControl;

        pControl = new CUIControl(GetEngineDeviceMan());
        pControl->CreateSub( this, "POPUPMENU_TEXT" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pTextBoxDummyControl = pControl;
    }

    CreatePopupList( TEXT_BOX_START );
}

void CPopupMenu::CreatePopupList( int TextBoxID )
{
    for ( int i = 0; i < nMAX_POPUP; i++)
    {
        CBasicLineBoxSmart* pBasicLineBoxSmart = new CBasicLineBoxSmart(GetEngineDeviceMan());
        pBasicLineBoxSmart->CreateSub( this, "POPUPMENU_BOX" );
        pBasicLineBoxSmart->CreateSubControl( "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
        RegisterControl( pBasicLineBoxSmart );
        pBasicLineBoxSmart->SetVisibleSingle( FALSE );
        m_vecLineBox.push_back( pBasicLineBoxSmart );

        m_vecPos.push_back( D3DXVECTOR2( 0.0f, 0.0f ) );
    }

    for ( int i = 0; i < nMAX_POPUP; i++)
    {
        CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

        CBasicTextBoxEx* pBasicTextBox = new CBasicTextBoxEx(GetEngineDeviceMan());
        pBasicTextBox->CreateSub( this, "POPUPMENU_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TextBoxID+i );
        pBasicTextBox->CreateMouseOver( NS_UITEXTCOLOR::DARKGRAY );
        pBasicTextBox->SetFont( pFont10 );
        pBasicTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        pBasicTextBox->SetPartInterval( 5.0f );
        RegisterControl( pBasicTextBox );
        pBasicTextBox->SetVisibleSingle( FALSE );
        m_vecTextBox.push_back( pBasicTextBox );
    }
}

void CPopupMenu::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible() )
        return;

    CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_bInitPostion )
    {
        m_bInitPostion = false;

        RePosition( static_cast<float>(x), static_cast<float>(y) );
        PopupMenuRePosition( 0, static_cast<float>(x), static_cast<float>(y) );
    }
}

void CPopupMenu::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    for ( size_t i = 0; i < m_vecTextBox.size(); i++ )
    {
        UIGUID CurID = TEXT_BOX_START + i;

        if ( CurID == ControlID)
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                SetVisiblePopupList( i, true );

                if ( ExpandMenu( m_vecTextBox[i], CurID ) )
                    SetVisiblePopupList( CurID-TEXT_BOX_START+1, true );
                else
                {
                    int TextBoxSize = static_cast<int>(m_vecTextBox.size());
                    for ( int nIndex = CurID-TEXT_BOX_START+1; nIndex < TextBoxSize; nIndex++ )
                        SetVisiblePopupList( nIndex, false );
                }
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                OnComponentClick( m_vecTextBox[i] );
            }
        }
    }

    if ( !IsFocusControl() )
        m_pInterface->UiHideGroup( GetWndID() );

    if ( CHECK_LB_UP_LIKE( dwMsg ) || CHECK_RB_UP_LIKE( dwMsg ) )
        m_pInterface->UiHideGroup( GetWndID() );
}

void CPopupMenu::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible == TRUE )
    {
        m_bInitPostion = true;
    }

    CUIGroupHelper::SetVisibleSingle( bVisible );
}

HRESULT CPopupMenu::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr( S_OK );
	V_RETURN( CUIGroup::RestoreDeviceObjects( pd3dDevice ) );

	RePosition( static_cast<float>(CurPosX), static_cast<float>(CurPosY) );
	PopupMenuRePosition( 0, static_cast<float>(CurPosX), static_cast<float>(CurPosY) );

	return S_OK;
}

void CPopupMenu::RePosition( float x, float y )
{
    CurPosX = x;
    CurPosY = y;

    const UIRECT& OriginRect = m_pDummyControl->GetLocalPos();
    const UIRECT& LineBoxOriginRect = m_pLineBoxDummyControl->GetLocalPos();
    const UIRECT& TextBoxOriginRect = m_pTextBoxDummyControl->GetLocalPos();

    const UIRECT& CurRect = GetLocalPos();

    AlignSubControl( CurRect, OriginRect );

    long lResolution = CUIMan::GetResolution ();
    WORD X_RES = HIWORD(lResolution);
    WORD Y_RES = LOWORD(lResolution);

    UIRECT rcNewRect = UIRECT( 0, 0, X_RES, Y_RES );

    AlignSubControl( OriginRect, rcNewRect );

    SetGlobalPos( rcNewRect );
}

void CPopupMenu::PopupMenuRePosition( int nTextBoxIndex, float x, float y )
{
    int LineBoxSize = static_cast<int>(m_vecLineBox.size());
    int TextBoxSize = static_cast<int>(m_vecTextBox.size());

    if ( LineBoxSize <= nTextBoxIndex || TextBoxSize <= nTextBoxIndex )
        return;

    m_vecPos[nTextBoxIndex] = D3DXVECTOR2( x, y );

    float NewPosX = x + fPOSITION_OFFSET;
    float NewPosY = y + fPOSITION_OFFSET;

    float SizeX = 0.0f;
    float SizeY = 0.0f;

    int nTotalLine = m_vecTextBox[nTextBoxIndex]->GetTotalLine();
    if ( nTotalLine > 0 )
    {
        SizeX = static_cast<float>(m_vecTextBox[nTextBoxIndex]->GetLongestLine()) + fTEXBOX_OFFSET*2;
        SizeX += (fTEXBOX_OFFSET*2);
    }

    SizeY = m_vecTextBox[nTextBoxIndex]->CalcMaxHEIGHT( 0, nTotalLine );
    SizeY += (fTEXBOX_OFFSET*2);

    UIRECT ParentRect = UIRECT( NewPosX, NewPosY, SizeX, SizeY );
    if ( nTextBoxIndex - 1 >= 0 )
        ParentRect = m_vecTextBox[nTextBoxIndex - 1]->GetGlobalPos();
    CheckUIBoundary( NewPosX, NewPosY, SizeX, SizeY, ParentRect );

    UIRECT rcNewRect = UIRECT( NewPosX, NewPosY, SizeX, SizeY );

    m_vecLineBox[nTextBoxIndex]->SetGlobalPos( rcNewRect );

    rcNewRect = UIRECT( NewPosX+fTEXBOX_OFFSET, NewPosY+fTEXBOX_OFFSET,
        SizeX - (fTEXBOX_OFFSET*2), SizeY - (fTEXBOX_OFFSET*2) );

    m_vecTextBox[nTextBoxIndex]->SetGlobalPos( rcNewRect );
}

void CPopupMenu::CheckUIBoundary( float& PosX, float& PosY, float SizeX, float SizeY, const UIRECT& ParentRect )
{
    long lResolution = CUIMan::GetResolution ();
    float X_RES = static_cast<float>( HIWORD(lResolution) );
    float Y_RES = static_cast<float>( LOWORD(lResolution) );

    if ( X_RES <= PosX + SizeX )
    {
        PosX = ParentRect.left - SizeX;
        if ( PosX < 0.0f )
            PosX = 0.0f;
    }

    if ( Y_RES <= PosY + SizeY )
    {
        PosY = Y_RES - SizeY;
    }
}

void CPopupMenu::OpenPopup( popupmenu::SP_POPUP_MENU PopupMenu )
{
    m_PopupMenu = PopupMenu;

    m_PopupMenu->SetKey( 0, m_PopupMenu );

    AllUnshow();

    bool bIsVisible = SetPopupText( m_PopupMenu, m_vecTextBox[0] );

    if ( bIsVisible )
    {
        SetVisiblePopupList( 0, true );
        m_pInterface->UiShowGroupFocus( POPUP_MENU );
    }
}

bool CPopupMenu::SetPopupText( popupmenu::SP_POPUP_MENU pPopupComponent, CBasicTextBoxEx* pTextBox )
{
    if ( !pTextBox )
        return false;

    bool bIsVisible = false;

    pTextBox->ClearText();

    popupmenu::PopupMenuComponent::MAP_MENU_COMPONENT MapMenuComponent;
    pPopupComponent->GetSubComponent( MapMenuComponent );

    std::vector<int> VecSubComponentIndex;

    for ( popupmenu::PopupMenuComponent::ITER_MENU_COMPONENT itr = MapMenuComponent.begin(); itr != MapMenuComponent.end(); itr++ )
    {
        unsigned int MenuKey = itr->first;
        popupmenu::SP_POPUP_MENU FindComponent = m_PopupMenu->FindInAllComponents( MenuKey );
        if ( !FindComponent )
            continue;

        popupmenu::SP_POPUP_MENU MenuComponent = itr->second;

        if ( MenuComponent )
        {
            bIsVisible = true;

            std::string strMarkSubComponent = "";

            /*if ( MenuComponent->IsSubComponent() )
                strMarkSubComponent = " ▶";
            std::string strDescription = stringformat( "%1%%2%", MenuComponent->GetDescription(), strMarkSubComponent );*/

            std::string strDescription = stringformat( "%1%", MenuComponent->GetDescription() );

            int nIndex = -1;
            if ( FindComponent->IsEnable() )
                nIndex = pTextBox->AddTextNoSplit( strDescription.c_str(), NS_UITEXTCOLOR::WHITE );
            else
                nIndex = pTextBox->AddTextNoSplit( strDescription.c_str(), NS_UITEXTCOLOR::DARKGRAY );

            pTextBox->SetTextData( nIndex, static_cast<DWORD>(MenuKey) );

            if ( MenuComponent->IsSubComponent() )
                VecSubComponentIndex.push_back( 1 );
			else
				VecSubComponentIndex.push_back( 0 );
        }
    }

    std::string LongestText = "";
    for ( int i = 0; i < pTextBox->GetTotalLine(); i++ )
    {
        std::string Temp = stringformat( "%1%", pTextBox->GetText( i ) );

        if ( LongestText.length() < Temp.length() )
            LongestText = Temp;
    }

    for ( size_t i = 0; i < VecSubComponentIndex.size(); i++ )
    {
        int Index = VecSubComponentIndex[i];

		if ( Index == 1 )
		{
			std::string Temp = stringformat( "%1%", pTextBox->GetText( i ) );

			std::string strMarkSubComponent = "";
			for ( size_t j = 0; j < (LongestText.length() - Temp.length() + 2); j++ )
			    strMarkSubComponent += " ";
			pTextBox->AddString( i, strMarkSubComponent.c_str(), pTextBox->GetTextColor( i ) );

			//"▶"이미지를 추가하는 코드 11은 이미지의 크기 2는 공백
			long  lnLongestKine = pTextBox->GetLongestLine();
			pTextBox->SetTextImage( Index, "SPEC_ACT_GRADE_OPEN_OVER", lnLongestKine + 11 );
		}
		else
			pTextBox->SetTextImage( Index, "NEW_SERVER_IMAGE_NONE");
    }

    return bIsVisible;
}

bool CPopupMenu::IsSelected( CBasicTextBoxEx* pTextBox, int nIndex )
{
    if ( !pTextBox )
        return false;

    if ( nIndex < 0 || pTextBox->GetTotalLine() <= nIndex )
        return false;

    return true;
}

void CPopupMenu::OnComponentClick( CBasicTextBoxEx* pTextBox )
{
    if ( !pTextBox )
        return;

    int nIndex = pTextBox->GetSelectPos();

    if ( !IsSelected( pTextBox, nIndex ) )
        return;

    unsigned int MenuKey = static_cast<unsigned int>( pTextBox->GetTextData( nIndex ) );
    if ( !IsEnableNode( MenuKey ) )
        return;

    popupmenu::SP_POPUP_MENU FindComponent = m_PopupMenu->FindInAllComponents( MenuKey );

    if ( FindComponent )
        FindComponent->execute( nIndex );
}

bool CPopupMenu::IsEnableNode( unsigned int MenuKey )
{
    popupmenu::SP_POPUP_MENU FindComponent = m_PopupMenu->FindInAllComponents( MenuKey );
    if ( FindComponent )
        return FindComponent->IsEnable();

    return false;
}

bool CPopupMenu::ExpandMenu( CBasicTextBoxEx* pTextBox, int nTextBoxID )
{
    if ( !pTextBox )
        return false;

    int NextTextBoxIndex = nTextBoxID + 1;
    int nTextBoxIndex = NextTextBoxIndex-TEXT_BOX_START;
    int TextBoxSize = static_cast<int>(m_vecTextBox.size());

    if ( TextBoxSize <= nTextBoxIndex )
    {
        return false;
        //CreatePopupList( NextTextBoxIndex );
    }

    int nIndex = pTextBox->GetSelectPos();

    unsigned int MenuKey = static_cast<unsigned int>( pTextBox->GetTextData( nIndex ) );
    if ( !IsEnableNode( MenuKey ) )
        return false;

    popupmenu::SP_POPUP_MENU FindComponent = m_PopupMenu->FindInAllComponents( MenuKey );

    if ( FindComponent )
    {
        if ( FindComponent->IsSubComponent() )
        {
            CBasicTextBoxEx* pNextTextBox = NULL;

            pNextTextBox = m_vecTextBox[ nTextBoxIndex ];

            SetPopupText( FindComponent, pNextTextBox );

            RePosition( CurPosX, CurPosY );

            for ( int i = 0; i < nTextBoxIndex; i++)
            {
                D3DXVECTOR2& vPos = m_vecPos[i];
                PopupMenuRePosition( i, vPos.x, vPos.y );
                SetVisiblePopupList( i, true );
            }

            const UIRECT& LineBoxRect = m_vecLineBox[nTextBoxID-TEXT_BOX_START]->GetGlobalPos();
            UIRECT PartRect;
            pTextBox->GetPartInfo( nIndex, PartRect );

            PopupMenuRePosition( nTextBoxIndex, LineBoxRect.right, PartRect.top-fTEXBOX_OFFSET );

            pTextBox->SetSelectPos( nIndex );
            pTextBox->UpdateMouseOverImage();

            return true;
        }
    }

    return false;
}

void CPopupMenu::SetVisiblePopupList( int nIndex, bool bVisible )
{
    int LineBoxSize = static_cast<int>(m_vecLineBox.size());
    int TextBoxSize = static_cast<int>(m_vecTextBox.size());

    if ( LineBoxSize <= nIndex || TextBoxSize <= nIndex )
        return;

    if ( bVisible )
    {
        m_vecLineBox[nIndex]->SetVisibleSingle( TRUE );
        m_vecTextBox[nIndex]->SetVisibleSingle( TRUE );
    }
    else
    {
        m_vecLineBox[nIndex]->SetVisibleSingle( FALSE );
        m_vecTextBox[nIndex]->SetVisibleSingle( FALSE );
    }
}

void CPopupMenu::AllUnshow()
{
    for ( size_t i = 0; i < m_vecLineBox.size(); i++ )
    {
        m_vecLineBox[i]->SetVisibleSingle( FALSE );
        m_vecTextBox[i]->SetVisibleSingle( FALSE );
    }
}

MyPopupMenu::MyPopupMenu( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CPopupMenu( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyPopupMenu::CreateUIWindowAndRegisterOwnership()
{
    CPopupMenu::Create( POPUP_MENU, "POPUPMENU", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CPopupMenu::CreateSubControl();
    CPopupMenu::m_pInterface->UiRegisterControl( this );
    CPopupMenu::m_pInterface->UiShowGroupFocus( POPUP_MENU );
}

void MyPopupMenu::OpenPopup( popupmenu::SP_POPUP_MENU PopupMenu )
{
    CPopupMenu::OpenPopup( PopupMenu );
}

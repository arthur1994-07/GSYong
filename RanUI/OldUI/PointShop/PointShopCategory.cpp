#include "StdAfx.h"

#include "./PointShopCategory.h"

#include "../../InnerInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../SigmaCore/String/StringFormat.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPointShopCategory::CPointShopCategory(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pText(NULL)
    , m_pCategoryCheck(NULL)
    , m_pCategoryUnCheck(NULL)
    , m_bCheck(false)
    , m_bEnableAll(false)
    , m_bForceChecked(false)
{
}

CPointShopCategory::~CPointShopCategory()
{
}

void CPointShopCategory::CreateSubControl()
{
    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_CATEGORY_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pText = pTextBox;

    }

    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CATEGORY_CHECK", UI_FLAG_DEFAULT, CATEGORY_CHECK );
        RegisterControl( pControl );
        m_pCategoryCheck = pControl;
        m_pCategoryCheck->SetVisibleSingle( FALSE );

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CATEGORY_UNCHECK", UI_FLAG_DEFAULT, CATEGORY_UNCHECK );
        RegisterControl( pControl );
        m_pCategoryUnCheck = pControl;

        pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub( this, "POINT_SHOP_CATEGORY_CHECK_BACK", UI_FLAG_DEFAULT, CATEGORY_CHECK_BACK );
        RegisterControl( pControl );
    }
}

void CPointShopCategory::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPointShopCategory::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case CATEGORY_CHECK_BACK:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                PressCheckBox();

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                m_pInterface->GetPointShopWindow()->SetAllCategory( false );

                Check();

                m_pInterface->GetPointShopWindow()->ReLoadItemList();
            }
        }
        break;
    }
}

void CPointShopCategory::SetCategoryData( std::string& strName, int nCount )
{
    m_pText->ClearText();
    std::string strText = sc::string::format( "%s(%d)", strName, nCount );
    m_pText->SetText(strText.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CPointShopCategory::PressCheckBox()
{
    if ( m_bCheck )
    {
        UnCheck();
    }
    else
    {
        Check();
    }

    if ( m_bEnableAll )
        m_pInterface->GetPointShopWindow()->SetAllCategory( m_bCheck );

    m_pInterface->GetPointShopWindow()->ReLoadItemList();
}

void CPointShopCategory::SetEnableCategoryAll( bool _bEnable )
{
    m_bEnableAll = _bEnable;

    if ( m_bEnableAll )
    {
        m_pText->ClearText();
        m_pText->SetText( ID2GAMEWORD("POINT_SHOP_ALL_CATEGORY"), NS_UITEXTCOLOR::WHITE );
    }
}

void CPointShopCategory::Check()
{
    if ( !m_bCheck )
    {
        m_bCheck = true;

        m_pCategoryUnCheck->SetVisibleSingle( FALSE );
        m_pCategoryCheck->SetVisibleSingle( TRUE );

        if ( !m_bForceChecked )
            m_pInterface->GetPointShopWindow()->UnCheckIncludeTextSearch();
    }
}

void CPointShopCategory::UnCheck()
{

    if ( m_bCheck )
    {
        m_bCheck = false;

        m_pCategoryUnCheck->SetVisibleSingle( TRUE );
        m_pCategoryCheck->SetVisibleSingle( FALSE );
    }
}
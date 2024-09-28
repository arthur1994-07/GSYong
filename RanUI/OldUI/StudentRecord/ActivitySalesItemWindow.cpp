#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Activity/ActivityBase.h"
#include "../../../RanLogic/Activity/ActivitySalesItem.h"

#include "../../InnerInterface.h"

#include "./ActivitySalesItemWindow.h"
#include "./ActivitySalesItemList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CActivitySalesItemWindow::CActivitySalesItemWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, CUIGroup* pCaller, EngineDeviceMan* pEngineDevice) 
    : CUIWindowEx( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pItemList(NULL)
    , m_pCloseButton(NULL)
    , m_pCaller(pCaller)
{
}

CActivitySalesItemWindow::~CActivitySalesItemWindow()
{
}

void CActivitySalesItemWindow::CreateSubControl()
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 설명
    {
        CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
        pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pBasicLineBox->CreateBaseBoxQuestList ( "ACTIVITY_SALESITEM_WINDOW_LINEBOX_DESC" );
        RegisterControl ( pBasicLineBox );

        CBasicTextBox* pBasicTextBox = new CBasicTextBox(m_pEngineDevice);
        pBasicTextBox->CreateSub ( this, "ACTIVITY_SALESITEM_WINDOW_TEXT_DESC" );
        pBasicTextBox->SetFont ( pFont );
        RegisterControl ( pBasicTextBox );
        m_pDesc = pBasicTextBox;
    }

    // 아이템 리스트
    {
        m_pItemList = new CActivitySalesItemList ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        m_pItemList->CreateSub ( this, "ACTIVITY_SALESITEM_WINDOW_ITEMLIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pItemList->CreateSubControl ();
        RegisterControl ( m_pItemList );
    }

    // 닫기 버튼
    {
        CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
        pButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ACTIVITY_SALESITEM_WINDOW_BUTTON_CLOSE );
        pButton->CreateBaseButton ( "ACTIVITY_SALESITEM_WINDOW_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
            CBasicButton::CLICK_FLIP, ID2GAMEWORD("ACTIVITY_SALESITEM_WINDOW_BUTTON", 0) );
        RegisterControl ( pButton );
        m_pCloseButton = pButton;
    }
}

void CActivitySalesItemWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    NSGUI::SetCharMoveBlock ();

    switch ( ControlID )
    {
    case ET_CONTROL_BUTTON:
    case ACTIVITY_SALESITEM_WINDOW_BUTTON_CLOSE:
        {
            if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };
}

void CActivitySalesItemWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );

    if ( bVisible )
    {
        m_pCaller->SetNoUpdate ( TRUE );
        m_pCaller->SetNoMessage( TRUE );
    }
    else
    {
        m_pCaller->SetNoUpdate ( FALSE );
        m_pCaller->SetNoMessage( FALSE );
    }
}

void CActivitySalesItemWindow::InitSalesList ( const EMSALESTYPE emType )
{
    // 텍스트를 설정
    std::string strDesc ( sc::string::format("ACTIVITY_SALESITEM_WINDOW_TEXT_DESC%1%", emType) );
    m_pDesc->SetText ( ID2GAMEINTEXT( strDesc.c_str() ), NS_UITEXTCOLOR::WHITE );

    // 아이템 리스트를 설정
    MMACTIVITYSALESITEM& mmActivitySalesItem = ActivityBaseData::GetInstance()->GetSalesItemData();
    m_pItemList->ClearList ();

    RANGE_MMACTIVITYSALESITEM Range = mmActivitySalesItem.equal_range( emType );

    for ( ITER_MMACTIVITYSALESITEM iter = Range.first; iter != Range.second; ++iter )
    {
        m_pItemList->AddList ( &iter->second );
    }

    m_pItemList->UpdateList ();
}

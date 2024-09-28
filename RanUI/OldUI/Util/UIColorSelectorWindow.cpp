
// bjju.sns

#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "../Util/ModalWindow.h"
#include "../Util/UIWindowEx.h"

#include "../Util/UIWindowObject.h"
#include "../Util/UIPageFrame.h"
#include "../Util/UIPage.h"

#include "./UIColorSelectorWindow.h"
#include "./UIColorPannelPage.h"
#include "./UIColorPickerPage.h"
//#include "./SNSFacebookPage.h"
//#include "./SNSTwitterPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorSelectorWindow::CUIColorSelectorWindow (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowObjectController (pInterface, pEngineDevice)
    , m_pGaeaClient             (pGaeaClient)
    , m_pUIObject		        ( NULL )
    , m_pPage_Pannel            ( NULL )
    , m_pPage_Picker            ( NULL )
    , m_dwColorBackup           ( 0 )
{
    // Blank
}

CUIColorSelectorWindow::~CUIColorSelectorWindow ()
{
    // Blank
}

void CUIColorSelectorWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& strTitle, FuncCreate _1f, FuncInit _2f, FuncUpdate _3f, FuncCommit _4f )
{
    m_FuncCreate = _1f;
    m_FuncInit   = _2f;
    m_FuncUpdate = _3f;
    m_FuncCommit = _4f;

    // Note : 윈도우 오브젝트
    {
        m_pUIObject = new CUIWindowObject(m_pInterface, m_pEngineDevice);
        m_pUIObject->Create					( COLORSELECTOR_WINDOW_OBJECT, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pUIObject->CreateBaseWindowBlack	( "COLORSELECTOR_WINDOW_OBJECT", "" );
        m_pUIObject->CreateSubControl		( pd3dDevice );
        m_pUIObject->SetAlignFlag			( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        m_pUIObject->SetVisibleSingle		( FALSE );
        m_pUIObject->SetFlag				( CUIWindowObject::FLAG_PAGE_CHANGE_RESIZE_WINDOW_ANIMATION | CUIWindowObject::FLAG_PAGE_CHANGE_ANIMATION );

        {
            m_pUIObject->SetWindowAniType		( CUIWindowObject::WINDOW_ANIMATION_LINEAR	);
            m_pUIObject->SetWindowAniSpeed		( 2.5f );
            m_pUIObject->SetPageAniType			( CUIPageFrame::PAGE_ANIMATION_ALPHA );
            m_pUIObject->SetPageAniSpeed		( 2.5f );
        }

        WindowRegister( m_pUIObject, COLORSELECTOR_WINDOW_OBJECT );
    }

    // Note : 페이지
    {
        m_FuncCreate ( m_InfoArray_Org );

        m_pPage_Pannel = new CUIColorPannelPage( m_pInterface, m_pEngineDevice );
        m_pPage_Pannel->Create ( "COLORSELECTOR_WINDOW_PAGE_PANNEL", strTitle.c_str(), COLORSELECTOR_WINDOW_PAGE_PANNEL );
        m_pPage_Pannel->CreateSubControl( m_InfoArray_Org );
        PageRegister( m_pPage_Pannel );

        m_pPage_Picker = new CUIColorPickerPage( m_pInterface, m_pEngineDevice );
        m_pPage_Picker->Create ( "COLORSELECTOR_WINDOW_PAGE_PICKER", strTitle.c_str(), COLORSELECTOR_WINDOW_PAGE_PICKER );
        PageRegister( m_pPage_Picker );
    }

    SetOpenWindow ( COLORSELECTOR_WINDOW_OBJECT );
    SetOpenPage	  ( m_pPage_Pannel );
}

void CUIColorSelectorWindow::SetVisibleSingle ( BOOL bVisible )
{
    CUIWindowObjectController::SetVisibleSingle( bVisible );

    if ( bVisible )
    {
        m_FuncInit(m_InfoArray_Org);
        m_InfoArray = m_InfoArray_Org;

        m_pPage_Pannel->SetAttribute(m_InfoArray);
        m_pPage_Pannel->SelectPannel(0,0);
    }
    else
    {
        m_FuncUpdate ( m_InfoArray_Org );
    }
}

void CUIColorSelectorWindow::Update	( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIWindowObjectController::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CUIColorSelectorWindow::UpdatePageRequest ( UIGUID ControlID, const SPAGEREQUEST& sRequest )
{
    switch ( sRequest.dwID )
    {
    case PAGE_REQ_PANNEL_SELECT:
        {
            m_dwColorBackup = m_pPage_Pannel->GetSelectColor();
            m_pPage_Picker->SetColor( m_dwColorBackup );

            m_pUIObject->PageOpen( m_pPage_Picker );
        }
        break;

    case PAGE_REQ_PANNEL_RANDOM:
        {
            m_pPage_Pannel->GetAttribute( m_InfoArray );
            m_FuncUpdate ( m_InfoArray );
        }
        break;

    case PAGE_REQ_PANNEL_RESET:
        {
            m_pPage_Pannel->SetAttribute( m_InfoArray_Org );
            m_InfoArray = m_InfoArray_Org;
            m_FuncUpdate ( m_InfoArray );
        }
        break;

    case PAGE_REQ_PANNEL_OK:
        {
            m_FuncCommit ( m_InfoArray );
            m_InfoArray_Org = m_InfoArray;
            Close();
        }
        break;

    case PAGE_REQ_PANNEL_CANCEL:
        {
            m_FuncUpdate ( m_InfoArray_Org );
            Close();
        }
        break;

    case PAGE_REQ_PICKER_UPDATE:
        {
            DWORD dwColor      = m_pPage_Picker->GetColor();
            int   nGroupIndex  = m_pPage_Pannel->GetSelectGroupIndex();
            int   nPannelIndex = m_pPage_Pannel->GetSelectPannelIndex();


            if ( nGroupIndex == CUIColorPannelPage::SELECT_NULL || nPannelIndex == CUIColorPannelPage::SELECT_NULL )
            {
                break;
            }

			// 컬러값이 0이 되는 경우 조명 연산이 제대로 되지 않음;
			// Pet의 경우 DB에 컬러값을 WORD(16bit)로 저장하기에 색상값을 5bit만 사용;
			// 그로인해 색상값 9미만의 값은 전부 0으로 저장 되기에 색상값이 9이하라면 9로 강제 변환;
			// Vehicle의 경우 32bit 색상으로 저장;
			const unsigned char cCurARGB[4] = {
				unsigned char(dwColor >> 24), unsigned char((dwColor >> 16) & 0xff), 
				unsigned char((dwColor >> 8) & 0xff), unsigned char(dwColor & 0xff)
			};
			const unsigned char cReARGB[4] = {
				cCurARGB[0],
				((cCurARGB[1] < 9) ? 9 : cCurARGB[1]),
				((cCurARGB[2] < 9) ? 9 : cCurARGB[2]),
				((cCurARGB[3] < 9) ? 9 : cCurARGB[3]),
			};			

			m_InfoArray[nGroupIndex].dwCOLOR[nPannelIndex] = 
				(cReARGB[0] << 24) + (cReARGB[1] << 16) + (cReARGB[2] << 8) + cReARGB[3];

            m_FuncUpdate ( m_InfoArray );        
        }
        break;

    case PAGE_REQ_PICKER_SELECT_OK:
        {
            DWORD dwColor = m_pPage_Picker->GetColor();
            m_pPage_Pannel->SetSelectColor( dwColor );

            m_pUIObject->PageOpen( m_pPage_Pannel );
        }
        break;

    case PAGE_REQ_PICKER_SELECT_CANCEL:
        {
            int nGroupIndex  = m_pPage_Pannel->GetSelectGroupIndex();
            int nPannelIndex = m_pPage_Pannel->GetSelectPannelIndex();

            if ( nGroupIndex == CUIColorPannelPage::SELECT_NULL || nPannelIndex == CUIColorPannelPage::SELECT_NULL )
            {
                break;
            }

            m_InfoArray[nGroupIndex].dwCOLOR[nPannelIndex] = m_dwColorBackup;
            m_FuncUpdate ( m_InfoArray );

            m_pUIObject->PageOpen( m_pPage_Pannel );
        }
        break;
    };
}

void CUIColorSelectorWindow::TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowObjectController::TranslateUIMessage( ControlID, dwMsg );
}

void CUIColorSelectorWindow::SetRestrict( const bool bRestrict )
{
    if ( m_pPage_Pannel )
    {
        m_pPage_Pannel->SetRestrict( (bRestrict ? TRUE : FALSE) );
    }

    if ( m_pPage_Picker )
    {
        m_pPage_Picker->SetRestrict( (bRestrict ? TRUE : FALSE) );
    }
}
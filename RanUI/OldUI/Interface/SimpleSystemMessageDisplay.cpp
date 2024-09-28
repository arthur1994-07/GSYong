#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UIMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogic/RANPARAM.h"

#include "SimpleSystemMessageDisplay.h"

SimpleSystemMessageDisplay::SimpleSystemMessageDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pMessageDisplay(NULL)
, m_bEternity(false)
, m_fLifeTime(0.0f)
, m_bShowtip(false)
{
}

SimpleSystemMessageDisplay::~SimpleSystemMessageDisplay ()
{
}

void SimpleSystemMessageDisplay::CreateSubControl ()
{
    CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
    pTextBox->CreateSub ( this, "SIMPLE_SYSTEM_MESSAGE_TEXT" );

    const long lResolution = CUIMan::GetResolution ();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );
    pTextBox->SetGlobalPos(D3DXVECTOR2(0.0f, 0.0f));
    pTextBox->SetLocalPos(D3DXVECTOR2(0.0f, 0.0f));
    pTextBox->SetControlSize(X_RES, int(Y_RES * 0.5f));
    pTextBox->SetTextAlign(TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
    pTextBox->SetVisibleSingle(TRUE);
    RegisterControl ( pTextBox );
    m_pMessageDisplay = pTextBox;
}

void SimpleSystemMessageDisplay::PrintSimpleSystemMessage( const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, dwFontSize, _DEFAULT_FONT_SHADOW_FLAG );
    m_pMessageDisplay->SetFont(pFont);
    m_pMessageDisplay->SetText(strMessage, dwMessageColor);
    m_bEternity = (fLifeTime == 0.0f);
    m_fLifeTime = fLifeTime;
    SetVisibleSingle(TRUE);
}
void SimpleSystemMessageDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_bEternity == false )
    {
        if ( m_fLifeTime < 0.0f )
        {
            ClearSimpleSystemMessage();
        }
        else
        {
            m_fLifeTime -= fElapsedTime;
        }
    }
}

void SimpleSystemMessageDisplay::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible != IsVisible() )
    {
        if ( bVisible == TRUE )
        {
            m_bShowtip = RANPARAM::bSHOW_TIP ? true : false;
            RANPARAM::bSHOW_TIP = FALSE;
        }
        else
        {
            RANPARAM::bSHOW_TIP = m_bShowtip ? TRUE : FALSE;
        }
    }

    CUIGroup::SetVisibleSingle(bVisible);
}

void SimpleSystemMessageDisplay::ClearSimpleSystemMessage()
{
    m_fLifeTime = 0;
    m_bEternity = true;
    m_pMessageDisplay->ClearText();
    SetVisibleSingle(FALSE);
}

void MySimpleSystemMessageDisplay::CreateUIWindowAndRegisterOwnerShip()
{
    SimpleSystemMessageDisplay::Create( INSTANCE_SYSTEM_MESSAGE_DISPLAY, "SIMPLE_SYSTEM_MESSAGE_TEXT", UI_FLAG_CENTER_X | UI_FLAG_TOP );
    SimpleSystemMessageDisplay::CreateSubControl();
    //SimpleSystemMessageDisplay::SetAlignFlag( UI_FLAG_DEFAULT );

    const long lResolution = CUIMan::GetResolution ();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );
    SimpleSystemMessageDisplay::SetControlSize(X_RES, int(Y_RES * 0.5f));

    SimpleSystemMessageDisplay::m_pInterface->UiRegisterControl(this);
    SimpleSystemMessageDisplay::m_pInterface->UiShowGroupBottom( GetWndID() );
    PrintSimpleSystemMessage( "", NS_UITEXTCOLOR::RED, 18, 0.0f);
    SetVisibleSingle(FALSE);
}

void MySimpleSystemMessageDisplay::PrintSimpleSystemMessage(const CString& strMessage, const D3DCOLOR dwMessageColor, const DWORD dwFontSize, const float fLifeTime)
{
    SimpleSystemMessageDisplay::PrintSimpleSystemMessage( strMessage, dwMessageColor, dwFontSize, fLifeTime );
}

void MySimpleSystemMessageDisplay::ClearSimpleSystemMessage()
{
    SimpleSystemMessageDisplay::ClearSimpleSystemMessage();
}

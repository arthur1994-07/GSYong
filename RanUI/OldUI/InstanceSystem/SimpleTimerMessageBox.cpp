#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "./SimpleTimerMessageBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SimpleProgressTimer::SimpleProgressTimer(EngineDeviceMan* pEngineDevice) :
CUIGroup(pEngineDevice)
,	m_pGauge(NULL)
,   m_pBlank(NULL)
,	m_pText(NULL)
,	m_fTimer(0.0f)
,	m_fCur(0.0f)
{
}

SimpleProgressTimer::~SimpleProgressTimer()
{
}

void SimpleProgressTimer::CreateSubControl()
{
    m_pBlank = new CBasicLineBoxSmart(m_pEngineDevice);
    m_pBlank->CreateSub(this, "TIMER_BAR", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
    m_pBlank->CreateSubControl("TIMER_BAR_BLANK_IMAGE");    
    RegisterControl(m_pBlank);

    m_pGauge = new CBasicLineBoxSmart(m_pEngineDevice);
    m_pGauge->CreateSub(this, "TIMER_BAR", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
    m_pGauge->CreateSubControl("TIMER_BAR_IMAGE");
    RegisterControl(m_pGauge);
}

void SimpleProgressTimer::Start(float fTimer)
{
    if ( fTimer < 0.0f )
        return;

    m_fCur = m_fTimer = fTimer;

    if (m_pGauge)
    {
        m_pGauge->ResetControl(GetGlobalPos(), "TIMER_BAR");
    }
}

void SimpleProgressTimer::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
    CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

    if (m_fCur > 0.0f )
    {
        m_fCur -= fElapsedTime;

        if (m_fCur < 0.0f )
        {
            m_fCur = 0.0f;

            CUIControl* pParent = GetParent();

            if (pParent)
            {
                pParent->TranslateUIMessage(GetWndID(), PT_TIMEOVER);
            }
        }

        if (m_pGauge)
        {
            float fPercent = (m_fCur / m_fTimer);

            UIRECT rcGaugePos = m_pGauge->GetGlobalPos();
            UIRECT rcBlankPos = m_pBlank->GetGlobalPos();

            UIRECT rcNewPos = rcBlankPos;
            rcNewPos.sizeX = floor(rcNewPos.sizeX * fPercent);
            rcNewPos.right = rcNewPos.left + rcNewPos.sizeX;

            m_pGauge->SetLocalPos( D3DXVECTOR2( 0.0f, 0.0f ) );
            m_pGauge->ReSizeControl(rcGaugePos, rcNewPos);

            D3DXVECTOR2 vPos;
            vPos.x = m_pBlank->GetLocalPos().left;
            vPos.y = m_pBlank->GetLocalPos().top;

            m_pGauge->SetLocalPos( vPos );
        }
    }

    if (m_pText)
    {
        TCHAR temp[10] = {0};
        _stprintf_s(temp, 10, _T("%d"), (int)m_fCur);
        m_pText->SetText(temp);
    }
}


SimpleTimerMessageBox::SimpleTimerMessageBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pClock(NULL)
    , m_pTimer(NULL)
    , m_pText(NULL)
    , m_pOk(NULL)
    , m_pCancel(NULL)
    , emResponseType(0)
    , dwResponseParam1(0)
    , dwResponseParam2(0)
{

}

SimpleTimerMessageBox::~SimpleTimerMessageBox()
{
}

void SimpleTimerMessageBox::CreateSubControl()
{
    m_pClock = new CSwapImage(m_pEngineDevice);
    m_pClock->CreateSub(this, "TIMER_IMAGE");
    RegisterControl(m_pClock);

    m_pTimer = new SimpleProgressTimer(m_pEngineDevice);
    m_pTimer->CreateSub(this, "SIMPLE_TIMER_MESSAGEBOX_TIMER", UI_FLAG_DEFAULT, CONTROL_ID_TIMER);
    m_pTimer->CreateSubControl();
    RegisterControl(m_pTimer);

    CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
    m_pText = new CBasicTextBox(m_pEngineDevice);
    m_pText->CreateSub(this, "SIMPLE_TIMER_MESSAGEBOX_TEXT");
    m_pText->SetFont(pFont);
    m_pText->SetTextAlign(TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
    RegisterControl(m_pText);

    CBasicTextButton* pOkButton = new CBasicTextButton(m_pEngineDevice);
    pOkButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_OK);
    pOkButton->CreateBaseButton("SIMPLE_TIMER_MESSAGEBOX_OK", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, "OK");
    RegisterControl(pOkButton);
    m_pOk = pOkButton;

    CBasicTextButton* pCancelButton = new CBasicTextButton(m_pEngineDevice);
    pCancelButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_CANCEL);
    pCancelButton->CreateBaseButton("SIMPLE_TIMER_MESSAGEBOX_CANCEL", CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, "Cancel");
    RegisterControl(pCancelButton);
    m_pCancel = pCancelButton;
}

void SimpleTimerMessageBox::SetParam(const DWORD _emResponseType, const DWORD _dwInstanceID, const DWORD _dwResponseParam1, const DWORD _dwResponseParam2)
{
    emResponseType = _emResponseType;
    dwInstanceID = _dwInstanceID;
    dwResponseParam1 = _dwResponseParam1;
    dwResponseParam2 = _dwResponseParam2;
}

void SimpleTimerMessageBox::SetText(const char* _szContentText, const char* _szOkText, const char* _szCancelText)
{
    m_pText->SetText(_szContentText, NS_UITEXTCOLOR::WHITE);
    m_pOk->SetOneLineText( CString( _szOkText ) );
    m_pCancel->SetOneLineText( CString( _szCancelText ) );
}

void SimpleTimerMessageBox::SetTimer(float fTimer)
{
    if ( fTimer > 0.0f )
    {
        if (m_pTimer)
        {
            m_pClock->SetVisibleSingle(TRUE);
            m_pTimer->SetVisibleSingle(TRUE);
            m_pTimer->Start(fTimer);
        }
    }
    else
    {
        m_pClock->SetVisibleSingle(FALSE);
        m_pTimer->SetVisibleSingle(FALSE);
        m_pTimer->Start(0.0f);
    }
}

void SimpleTimerMessageBox::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
    CUIGroup::TranslateUIMessage(ControlID, dwMsg);

    switch (ControlID)
    {
    case CONTROL_ID_BUTTON_OK:
    case CONTROL_ID_BUTTON_CANCEL:
        {
            if (CHECK_MOUSE_IN(dwMsg))
            {
                if (CHECK_LB_UP_LIKE(dwMsg))
                {
                    if (ControlID == CONTROL_ID_BUTTON_OK)
                    {
                        OnOk();
                    }
                    else
                    {
                        OnCancel();
                    }
                }
            }
        }
        break;

    case CONTROL_ID_TIMER:
        {
            if (dwMsg == SimpleProgressTimer::PT_TIMEOVER)
            {
                OnTimeOver();
            }
        }
        break;
    }
}

void SimpleTimerMessageBox::OnOk()
{
    sendMessageToServer(dwResponseParam1, dwResponseParam2, EM_MESSAGE_OK);    
    m_pInterface->UiHideGroup(GetWndID());
}

void SimpleTimerMessageBox::OnCancel()
{
    sendMessageToServer(dwResponseParam1, dwResponseParam2, EM_MESSAGE_CANCEL);
    m_pInterface->UiHideGroup(GetWndID());
}

void SimpleTimerMessageBox::OnTimeOver()
{
    sendMessageToServer(dwResponseParam1, dwResponseParam2, EM_MESSAGE_TIMEOVER);
    m_pInterface->UiHideGroup(GetWndID());
}


void SimpleTimerMessageBox::sendMessageToServer(const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4)
{
    GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE netMsg(_dwParam1, _dwParam2, _dwParam3, _dwParam4);
    netMsg.emDestType = emResponseType;
    netMsg.dwDestID = this->dwInstanceID;
    netMsg.emSrcType = InstanceSystem::EM_MESSAGE_USER;
    GLCharacter* _pChar = m_pGaeaClient->GetCharacter();
    if ( _pChar == NULL )
        return;

    netMsg.dwSrcID = _pChar->CharDbNum();

    switch ( emResponseType )
    {
    case InstanceSystem::EM_MESSAGE_MY_INSTANCE:
        {
            m_pGaeaClient->NETSENDTOFIELD(&netMsg);
        }
        break;

    case InstanceSystem::EM_MESSAGE_FIELD_INSTANCE:
    case InstanceSystem::EM_MESSAGE_AGENT_BASE:
        {
            m_pGaeaClient->NETSENDTOAGENT(&netMsg);
        }
        break;
    default:
        break;
    }
}


void MySimpleTimerMessageBox::CreateUIWindowAndRegisterOwnership()
{
    SimpleTimerMessageBox::Create( INSTANCE_TIMER_MESSAGE_BOX, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    SimpleTimerMessageBox::CreateBaseWindowBodyLightGray("SIMPLE_TIMER_MESSAGEBOX_WINDOW");
    SimpleTimerMessageBox::CreateSubControl();
    SimpleTimerMessageBox::SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y );
    SimpleTimerMessageBox::m_pInterface->UiRegisterControl(this);
}

void MySimpleTimerMessageBox::ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX pPacket;
    if ( CastNetMsg(pMsg, pPacket) == false )
        return;

    SimpleTimerMessageBox::SetText(pPacket._strContentText.c_str(), pPacket._strOkText.c_str(), pPacket._strCancelText.c_str());
    SimpleTimerMessageBox::SetParam(pPacket.emResponseType, pPacket.dwInstanceID, pPacket.dwResponseMsgParam1, pPacket.dwResponseMsgParam2);
    SimpleTimerMessageBox::SetTimer(pPacket.fTimer);
    m_pInterface->UiShowGroupFocus(GetWndID());
}

void MySimpleTimerMessageBox::Close()
{
    SimpleTimerMessageBox::SetVisibleSingle(FALSE);
}
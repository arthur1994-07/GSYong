#include "stdafx.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "./OldUI/Post/PostBoxSendPage.h"
#include "./OldUI/Post/PostBoxPreviewPage.h"
#include "./OldUI/Post/PostBoxReceivePage.h"
#include "./OldUI/Post/PostBoxHistoryPage.h"
#include "./OldUI/Post/PostBoxWindow.h"
#include "./InnerInterface.h"


//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------


IPostBoxWindow* CInnerInterface::GetPostBoxWindow()
{ 
    return m_pPostBoxWindow;
}

void CInnerInterface::PostBoxProcessReceiveComplete()
{
    if (m_pPostBoxWindow)
        m_pPostBoxWindow->Process_ReceiveComplete();
}

void CInnerInterface::PostBoxReceivePageUpdateItem()
{
    if (m_pPostBoxWindow)
    {
        m_pPostBoxWindow->PostBoxReceivePageUpdateItem();
    }
}

void CInnerInterface::PostBoxHistoryPageUpdateList()
{
    if ( m_pPostBoxWindow )
    {
        m_pPostBoxWindow->PostBoxHistoryPageUpdateList();
    }
}

const BOOL CInnerInterface::PostBoxWindowIsWindowOpenAble(const BOOL bPrint)
{
    if (m_pPostBoxWindow)
        return m_pPostBoxWindow->IsWindowOpenAble(bPrint);
    else
        return FALSE;
}

const BOOL CInnerInterface::PostBoxWindowIsWindowCloseAble(const BOOL bPrint)
{
    if (m_pPostBoxWindow)
        return m_pPostBoxWindow->IsWindowCloseAble(bPrint);
    else
        return FALSE;
}

BOOL CInnerInterface::PostBoxWindowIsVisible() const
{
    if (m_pPostBoxWindow)
        return m_pPostBoxWindow->IsVisible();
    else
        return FALSE;
}

void CInnerInterface::PostBoxWindowDoModalError(const std::string& strError)
{
    if (m_pPostBoxWindow)
        m_pPostBoxWindow->DoModalError(strError);
}

void CInnerInterface::PostBoxWindowDoModalMsg(const std::string& strMsg)
{
    if (m_pPostBoxWindow)
        m_pPostBoxWindow->DoModalMsg(strMsg);
}

void CInnerInterface::PostBoxWindowClose()
{
    if (m_pPostBoxWindow)
        m_pPostBoxWindow->Close();
}

void CInnerInterface::PostBoxWindowSEndPostFbResult()
{
    if ( m_pPostBoxWindow )
    {
        m_pPostBoxWindow->ResetSendPage();

        m_pPostBoxWindow->RequestPushViewPage( CPostBoxWindow::POSTBOX_PAGE_REQ_PREVIEW_CLOSE );

        PostBoxWindowDoModalMsg(ID2GAMEINTEXT("POSTBOX_MSG_SEND"));
    }
}
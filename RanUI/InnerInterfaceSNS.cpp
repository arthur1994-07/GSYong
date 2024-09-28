#include "stdafx.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "./OldUI/SNS/SNSWindow.h"
#include "./OldUI/SNS/FaceBookWindow.h"
#include "./OldUI/SNS/SNSFacebookPage.h"
#include "./OldUI/SNS/SNSTwitterPage.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------
void CInnerInterface::SNSWindowDoModalError(const std::string& strError)
{
    if (m_pSNSWindow)
    {
        m_pSNSWindow->DoModalError(strError);
    }
}

void CInnerInterface::SNSWebFBWindowSetPageType(const DWORD dwType)
{
    if ( m_pFaceBook )
    {
        m_pFaceBook->SetPageType( CFaceBookWindow::PAGETYPE(dwType) );
    }
}

void CInnerInterface::SNSWebFBWindowSetAddFriendURL(const std::string& strURL)
{
    if ( m_pFaceBook )
    {
        m_pFaceBook->SetAddFriendURL(strURL);
    }
}

void CInnerInterface::SNSFBPageClearEditWall()
{
    if (m_pSNSWindow)
    {
        CSNSFacebookPage* pPage = m_pSNSWindow->GetFacebookPage();

        if (pPage)
        {
            pPage->ClearEditWall();
        }
    }
}

void CInnerInterface::SNSFBPageClearEditPicture()
{
    if (m_pSNSWindow)
    {
        CSNSFacebookPage* pPage = m_pSNSWindow->GetFacebookPage();

        if (pPage)
        {
            pPage->ClearEditPicture();
        }
    }
}

void CInnerInterface::SNSFBPageSetEnableWallButton(const BOOL bEnable)
{
    if (m_pSNSWindow)
    {
        CSNSFacebookPage* pPage = m_pSNSWindow->GetFacebookPage();

        if (pPage)
        {
            pPage->SetEnableWallButton(bEnable);
        }
    }
}

void CInnerInterface::SNSFBPageSetEnablePicture(const BOOL bEnable)
{
    if (m_pSNSWindow)
    {
        CSNSFacebookPage* pPage = m_pSNSWindow->GetFacebookPage();

        if (pPage)
        {
            pPage->SetEnablePicture(bEnable);
        }
    }
}

void CInnerInterface::SNSTWPageClearEditTweet()
{
    if (m_pSNSWindow)
    {
        CSNSTwitterPage* pPage = m_pSNSWindow->GetTwitterPage();

        if (pPage)
        {
            pPage->ClearEditTweet();
        }
    }
}

void CInnerInterface::SNSTWPageSetEnableTweet(const BOOL bEnable)
{
    if (m_pSNSWindow)
    {
        CSNSTwitterPage* pPage = m_pSNSWindow->GetTwitterPage();

        if (pPage)
        {
            pPage->SetEnableTweet(bEnable);
        }
    }
}

void CInnerInterface::SNSTWPageSetEnableFollower(const BOOL bEnable)
{
    if (m_pSNSWindow)
    {
        CSNSTwitterPage* pPage = m_pSNSWindow->GetTwitterPage();

        if (pPage)
        {
            pPage->SetEnableFollower(bEnable);
        }
    }
}

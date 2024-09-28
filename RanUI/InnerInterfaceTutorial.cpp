#include "stdafx.h"

#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.
#include "./OldUI/Tutorial/TutorialDialogue.h"

#include "./OldUI/Tutorial/MouseControlDir.h"
#include "./OldUI/Tutorial/TutorialViewStep.h"

#include "./StaticUIManager.h"
#include "./InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::SetTutorialDialogueEnable(bool bSet)
{
    m_pTutorialDialogue->SetEnalbe(bSet);
}

ITutorialDialogue* CInnerInterface::GetTutorialDialogue()
{ 
    return m_pTutorialDialogue;
}

void CInnerInterface::SetTutorialDialogueIndex(DWORD Index)
{
    m_pTutorialDialogue->SetTutorialDialogueIndex( Index );
}

void CInnerInterface::TutorialDialogueTextStepInc()
{
    m_pTutorialDialogue->TutorialDialogueTextStepInc();
}

void CInnerInterface::TutorialDialogueSoundIndexSet(DWORD Index)
{
    m_pTutorialDialogue->SetTutorialDialogueSoundIndex( Index );
}

DWORD CInnerInterface::GetTutorialDialogueSoundIndex()
{
    return m_pTutorialDialogue->GetTutorialDialogueSoundIndex();
}

void CInnerInterface::SetTutorialDialogueExplainText(const CString& strText)
{
    m_pTutorialDialogueExplainText->SetExplainText(strText);
}

ITutorialViewStep* CInnerInterface::GetTutorialViewStep()
{ 
    return m_pTutorialViewStep;
}

void CInnerInterface::OpenTutorialDialogue()
{
    if (UiIsVisibleGroup (TUTORIAL_DIALOGUE_WINDOW))
        return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcMouseTipWindow = m_pTutorialDialogue->GetUIWindowGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = ( X_RES - rcMouseTipWindow.sizeX ) * 0.5f;
        vPos.y = Y_RES * 0.2f;
        m_pTutorialDialogue->SetUIWindowGlobalPos ( vPos );
        m_pTutorialDialogue->SetUIWindowLocalPos ( vPos );
    }

    UiShowGroupTop(TUTORIAL_DIALOGUE_WINDOW);

    GLBaseTutorial* pTutorial = m_pGaeaClient->GetTutorial();
    pTutorial->StopSound();
    pTutorial->PlaySound(m_pTutorialDialogue->GetTutorialDialogueSoundIndex());
    m_pTutorialDialogue->TutorialDialogueSoundIndexInc();
}

void CInnerInterface::OpenTutorialDialogueExplainText()
{
    if (UiIsVisibleGroup(TUTORIAL_DIALOGUE_EXPLAIN_WINDOW))
        return;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcMouseTipWindow = m_pTutorialDialogueExplainText->GetGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = ( X_RES - rcMouseTipWindow.sizeX ) * 0.5f;
        vPos.y = Y_RES * 0.12f;
        m_pTutorialDialogueExplainText->SetGlobalPos ( vPos );
        m_pTutorialDialogueExplainText->SetLocalPos ( vPos );
    }

    UiShowGroupFocus ( TUTORIAL_DIALOGUE_EXPLAIN_WINDOW );

}

void CInnerInterface::OpenTutorialInterfaceLeft()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_LEFT ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceLeft->GetGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES / 3.0f - rcWindow.sizeX;
        vPos.y = Y_RES * 0.5f - rcWindow.sizeY * 0.5f;

        m_pTutorialInterfaceLeft->SetGlobalPos ( vPos );
        m_pTutorialInterfaceLeft->SetLocalPos ( vPos );
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_LEFT );
}

void CInnerInterface::OpenTutorialInterfaceRight()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_RIGHT ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceRight->GetGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES  * 2.0f / 3.0f;
        vPos.y = Y_RES * 0.5f - rcWindow.sizeY * 0.5f;

        m_pTutorialInterfaceRight->SetGlobalPos ( vPos );
        m_pTutorialInterfaceRight->SetLocalPos ( vPos );
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_RIGHT );
}

void CInnerInterface::OpenTutorialInterfaceUp()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_UP ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceUp->GetGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = Y_RES / 3.0f - rcWindow.sizeY;

        m_pTutorialInterfaceUp->SetGlobalPos ( vPos );
        m_pTutorialInterfaceUp->SetLocalPos ( vPos );
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_UP );
}

void CInnerInterface::OpenTutorialInterfaceDown()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_DOWN ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceDown->GetGlobalPos ();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = Y_RES * 2.0f / 3.0f;

        m_pTutorialInterfaceDown->SetGlobalPos ( vPos );
        m_pTutorialInterfaceDown->SetLocalPos ( vPos );
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_DOWN );
}

void CInnerInterface::OpenTutorialInterfaceZoomIn()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_ZOOM_IN ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceZoomIn->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = Y_RES * 5.0f / 6.0f;

        m_pTutorialInterfaceZoomIn->SetGlobalPos(vPos);
        m_pTutorialInterfaceZoomIn->SetLocalPos(vPos);
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_ZOOM_IN );
}
void CInnerInterface::OpenTutorialInterfaceZoomOut()
{
    if ( UiIsVisibleGroup ( TUTORIAL_INTERFACE_ZOOM_OUT ) ) return ;

    const long lResolution = uiman::GetInnerUIMan().GetResolution();
    WORD X_RES = HIWORD ( lResolution );
    WORD Y_RES = LOWORD ( lResolution );

    {
        const UIRECT& rcWindow = m_pTutorialInterfaceZoomOut->GetGlobalPos();

        D3DXVECTOR2 vPos;
        vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
        vPos.y = Y_RES * 2.0f / 3.0f - rcWindow.sizeY;

        m_pTutorialInterfaceZoomOut->SetGlobalPos(vPos);
        m_pTutorialInterfaceZoomOut->SetLocalPos(vPos);
    }

    UiShowGroupFocus ( TUTORIAL_INTERFACE_ZOOM_OUT );
}

void CInnerInterface::SetTutorialDialogText(const CString& strText)
{
    m_pTutorialDialogue->SetDialogueText( UI::ToString( strText ) );
}

void CInnerInterface::SetTutorialViewStep(int Index)
{
    m_pTutorialViewStep->SetOnStep( Index );
}
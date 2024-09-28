#include "stdafx.h"
#include "PieceContainerPage.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "SheetWithTabChar.h"
#include "sheetWithTabFrame.h"
#include "SheetWithTabAnim.h"
#include "sheetWithTabBone.h"
#include "sheetWithTabPiece.h"
#include "sheetWithTabBoardEdit.h"

#include "../../SigmaCore/DebugInclude.h"

CPieceContainerPage::CPieceContainerPage(UINT nIDTemplate) :	
    CPropertyPage(nIDTemplate)
,   m_nParts(0)
,   m_pSheetTabParent(0)
,   m_psheetWithTabFrame(0)
,   m_psheetWithTabAnim(0)
,   m_psheetWithTabBone(0)
,   m_psheetWithTabPiece(0)
,   m_psheetWithTabBoardEdit(0)
{
    
}

CPieceContainerPage::~CPieceContainerPage()
{
    
}

void CPieceContainerPage::PostNcDestroy()
{
    SAFE_DELETE(m_psheetWithTabFrame);
    SAFE_DELETE(m_psheetWithTabAnim);
    SAFE_DELETE(m_psheetWithTabBone);
    SAFE_DELETE(m_psheetWithTabPiece);
    SAFE_DELETE(m_psheetWithTabBoardEdit);

    CPropertyPage::PostNcDestroy();
}

BOOL CPieceContainerPage::OnSetActive()
{
    if (m_psheetWithTabFrame)
    {
        m_psheetWithTabFrame->GetActivePage()->OnSetActive();
    }

    if (m_psheetWithTabAnim)
    {
        m_psheetWithTabAnim->GetActivePage()->OnSetActive();
    }

    if (m_psheetWithTabBone)
    {
        m_psheetWithTabBone->GetActivePage()->OnSetActive();
    }
#ifdef DEF_SKIN_SYNC
    if (m_psheetWithTabPiece)
    {
        m_psheetWithTabPiece->GetActivePage()->OnSetActive();
    }
#endif

    if (m_psheetWithTabBoardEdit)
    {
        m_psheetWithTabBoardEdit->GetActivePage()->OnSetActive();
    }

    return CPropertyPage::OnSetActive();
}

void CPieceContainerPage::SkinPieceClear(int nParts)
{
    if (m_psheetWithTabFrame)
    {
        m_psheetWithTabFrame->m_FramePage.ResetPage();
    }
}

void CPieceContainerPage::SetSkinPiece(DxSkinPiece* pSkinPiece)
{
    if (m_psheetWithTabFrame)
    {
        m_psheetWithTabFrame->m_FramePage.ResetPage();
    }
}

void CPieceContainerPage::SetSkinPiece(int nParts)
{
    m_nParts = nParts;
}

DxSkinPiece* CPieceContainerPage::GetSkinPiece(int nParts)
{
    int nTemp = 0;

    if (nParts != -1)
    {
        nTemp = m_nParts;
        m_nParts = nParts;
    }

    DxSkinPiece* pSkinPiece = 0;
    DxCharPart* pCharPart = GetCharPart();

    if (pCharPart)
    {
        pSkinPiece = pCharPart->GetPart();

        DxSkinPiece* pRealSkinPiece = CreateSkinPiece(m_nParts);

        if (pSkinPiece == 0 || pSkinPiece != pRealSkinPiece)
        {
            SetSkinPieceType(pRealSkinPiece);
        }

        pSkinPiece = pRealSkinPiece;
    }

    if (nParts != -1)
    {
        m_nParts = nTemp;
    }

    return pSkinPiece;
}

void CPieceContainerPage::SetSkinPieceType(DxSkinPiece* pSkinPiece)
{

}

int CPieceContainerPage::GetSkinPieceSize()
{
    return 0;
}

DxCharPart* CPieceContainerPage::GetCharPart()
{
    return 0;
}

bool CPieceContainerPage::IsSameBoneFile(const TCHAR* pName)
{
    return false;
}

bool CPieceContainerPage::HasParts(DxSkinPiece* pTempSkinPiece)
{
    return false;
}

void CPieceContainerPage::ReSetPart()
{
    DxCharPart* pCharPart = GetCharPart();

    if (pCharPart)
    {
        pCharPart->ReSetPart_Detail();
    }
}

void CPieceContainerPage::ReSetPages()
{
    if (m_psheetWithTabAnim)
    {
        m_psheetWithTabAnim->ShowWindow(SW_SHOW);
    }
}

void CPieceContainerPage::SwapSkinPiece(int nParts)
{
    if (m_psheetWithTabFrame)
    {
        m_psheetWithTabFrame->m_FramePage.m_PieceList.SetItemState(m_nParts, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
    }
}

void CPieceContainerPage::ShowWindow(bool bShow)
{
    if (m_psheetWithTabFrame)
    {
        m_psheetWithTabFrame->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    }
}

void CPieceContainerPage::SetSpinControl(int nRadius, int nHeight, int nColorHeight)
{
    if (m_psheetWithTabPiece)
    {
        m_psheetWithTabPiece->SetSpinControl(nColorHeight);
    }
}

void CPieceContainerPage::ResetDialog()
{
    if (m_psheetWithTabPiece)
    {
        m_psheetWithTabPiece->ResetDialog();
    }
}

void CPieceContainerPage::UpdateKeyBoardMouse()
{
    if (m_psheetWithTabPiece)
    {
        m_psheetWithTabPiece->UpdateKeyBoardMouse();
    }
}

void CPieceContainerPage::ReplaceSkinPiece( int nParts )
{

}

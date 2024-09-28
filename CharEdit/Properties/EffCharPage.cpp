#include "stdafx.h"
#include "EffCharPage.h"

#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "sheetWithTabChar.h"

#include "../../SigmaCore/DebugInclude.h"

CEffCharPage::CEffCharPage(UINT nIDTemplate) :	
    CPieceRelativePage(nIDTemplate)
,	m_pEffChar(0)
{

}

CEffCharPage::~CEffCharPage()
{

}

void CEffCharPage::EffectApply()
{
#ifdef DEF_SKIN_SYNC
    CCharEditApp *pApp = (CCharEditApp*) AfxGetApp();
    CMainFrame* pFrame = (CMainFrame *) pApp->m_pMainWnd;

    if (pFrame == 0)
    {
        return ;
    }

    CsheetWithTabChar* pSheetWithTab = pFrame->m_cDlgEdit.GetSheetChar();;

    if (pSheetWithTab != 0)
    {
        return pSheetWithTab->m_pCurPieceContainer->ReSetPart();
    }
#endif
}

void CEffCharPage::ReturnPage()
{
    m_pEffChar = 0;
    CPieceRelativePage::ReturnPage();
}

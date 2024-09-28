#include "stdafx.h"
#include "PieceRelativePage.h"
#include "sheetWithTabChar.h"
#include "sheetWithTabPiece.h"

#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"

#include "../../SigmaCore/DebugInclude.h"

CPieceRelativePage::CPieceRelativePage(UINT nIDTemplate) :	
    CPropertyPage(nIDTemplate)
#ifndef DEF_SKIN_SYNC
,   m_pSheetTab(0)
#endif
{

}

CPieceRelativePage::~CPieceRelativePage()
{

}

void CPieceRelativePage::SetActiveParentPage(int nPage)
{
    CPropertySheet* pParent = static_cast<CPropertySheet*>(GetParent());

    if (pParent)
    {
        pParent->SetActivePage(nPage);
    }
}

void CPieceRelativePage::ReturnPage()
{
#ifndef DEF_SKIN_SYNC
    m_pSheetTab->m_SkinPiecePage.EffectListting ();
    m_pSheetTab->SetActivePage ( CHARTAB_CHARPIECE );
#else    
    SetActiveParentPage(CsheetWithTabPiece::PIECEPAGE);
#endif
}
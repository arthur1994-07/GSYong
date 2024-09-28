// CharEditDoc.cpp : implementation of the CCharEditDoc class
//

#include "stdafx.h"
#include "CharEdit.h"

#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"

#include "MainFrm.h"
#include "CharEditDoc.h"
#include "CharEditView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CCharEditDoc

IMPLEMENT_DYNCREATE(CCharEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CCharEditDoc, CDocument)
	//{{AFX_MSG_MAP(CCharEditDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharEditDoc construction/destruction

CCharEditDoc::CCharEditDoc()
{
}

CCharEditDoc::~CCharEditDoc()
{
}

BOOL CCharEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCharEditDoc serialization

void CCharEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCharEditDoc diagnostics

#ifdef _DEBUG
void CCharEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCharEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCharEditDoc commands


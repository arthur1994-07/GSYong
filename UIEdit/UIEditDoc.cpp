// UIEditDoc.cpp : CUIEditDoc Ŭ������ ����
//

#include "stdafx.h"
#include "UIEdit.h"

#include "UIEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIEditDoc

IMPLEMENT_DYNCREATE(CUIEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CUIEditDoc, CDocument)
END_MESSAGE_MAP()


// CUIEditDoc ����/�Ҹ�

CUIEditDoc::CUIEditDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CUIEditDoc::~CUIEditDoc()
{
}

BOOL CUIEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CUIEditDoc serialization

void CUIEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CUIEditDoc ����

#ifdef _DEBUG
void CUIEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUIEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUIEditDoc ���

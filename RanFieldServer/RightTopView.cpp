// RightTopView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanFieldServer.h"
#include "RightTopView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
// RightTopView

IMPLEMENT_DYNCREATE(RightTopView, CEditView)

RightTopView::RightTopView()
{

}

RightTopView::~RightTopView()
{
}

BEGIN_MESSAGE_MAP(RightTopView, CEditView)
    ON_CONTROL_REFLECT(EN_MAXTEXT, &RightTopView::OnEnMaxtext)
END_MESSAGE_MAP()


// RightTopView �����Դϴ�.

#ifdef _DEBUG
void RightTopView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void RightTopView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// RightTopView �޽��� ó�����Դϴ�.

void RightTopView::OnEnMaxtext()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetEditCtrl().SetWindowText(_T(""));
}

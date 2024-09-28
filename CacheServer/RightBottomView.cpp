// RightBottomView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CacheServer.h"
#include "RightBottomView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// RightBottomView

IMPLEMENT_DYNCREATE(RightBottomView, CEditView)

RightBottomView::RightBottomView()
{

}

RightBottomView::~RightBottomView()
{
}

BEGIN_MESSAGE_MAP(RightBottomView, CEditView)
    ON_CONTROL_REFLECT(EN_MAXTEXT, &RightBottomView::OnEnMaxtext)
END_MESSAGE_MAP()


// RightBottomView �����Դϴ�.

#ifdef _DEBUG
void RightBottomView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void RightBottomView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// RightBottomView �޽��� ó�����Դϴ�.

void RightBottomView::OnEnMaxtext()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetEditCtrl().SetWindowText(_T(""));
}

void RightBottomView::ClearMessage()
{
    GetEditCtrl().SetWindowText(_T(""));
}

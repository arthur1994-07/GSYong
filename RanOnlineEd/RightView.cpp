// RightView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanOnlineEd.h"
#include "RightView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// RightView

IMPLEMENT_DYNCREATE(RightView, CEditView)

RightView::RightView()
{

}

RightView::~RightView()
{
}

BEGIN_MESSAGE_MAP(RightView, CEditView)
    ON_CONTROL_REFLECT(EN_MAXTEXT, &RightView::OnEnMaxtext)
END_MESSAGE_MAP()


// RightView �����Դϴ�.

#ifdef _DEBUG
void RightView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void RightView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// RightView �޽��� ó�����Դϴ�.

void RightView::OnEnMaxtext()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetEditCtrl().SetWindowText("");
}

void RightView::OnInitialUpdate()
{
    CEditView::OnInitialUpdate();

    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
     CEdit& EditCtrl = GetEditCtrl();
    EditCtrl.SetLimitText(100 * 1024); // 100k
}

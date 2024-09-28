// RightView.cpp : 구현 파일입니다.
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


// RightView 진단입니다.

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


// RightView 메시지 처리기입니다.

void RightView::OnEnMaxtext()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetEditCtrl().SetWindowText("");
}

void RightView::OnInitialUpdate()
{
    CEditView::OnInitialUpdate();

    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
     CEdit& EditCtrl = GetEditCtrl();
    EditCtrl.SetLimitText(100 * 1024); // 100k
}

// RightTopView.cpp : 구현 파일입니다.
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


// RightTopView 진단입니다.

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


// RightTopView 메시지 처리기입니다.

void RightTopView::OnEnMaxtext()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetEditCtrl().SetWindowText(_T(""));
}

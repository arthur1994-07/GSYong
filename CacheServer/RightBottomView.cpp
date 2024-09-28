// RightBottomView.cpp : 구현 파일입니다.
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


// RightBottomView 진단입니다.

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


// RightBottomView 메시지 처리기입니다.

void RightBottomView::OnEnMaxtext()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    GetEditCtrl().SetWindowText(_T(""));
}

void RightBottomView::ClearMessage()
{
    GetEditCtrl().SetWindowText(_T(""));
}

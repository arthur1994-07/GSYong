// CToolBarCombo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "CToolBarCombo.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CToolBarCombo

IMPLEMENT_DYNAMIC(CToolBarCombo, CComboBox)
CToolBarCombo::CToolBarCombo()
	: m_pWndMsgParent(NULL)
{
}

CToolBarCombo::~CToolBarCombo()
{
}

void CToolBarCombo::SetWndMsgParent(CWnd *pWnd)
{
	m_pWndMsgParent = pWnd;
}

BEGIN_MESSAGE_MAP(CToolBarCombo, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnCbnSelchange)
END_MESSAGE_MAP()



// CToolBarCombo 메시지 처리기입니다.


void CToolBarCombo::OnCbnSelchange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_pWndMsgParent)
		m_pWndMsgParent->SendMessage(wm_ChangeSelCombo);
}

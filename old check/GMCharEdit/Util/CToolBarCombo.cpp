// CToolBarCombo.cpp : ���� �����Դϴ�.
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



// CToolBarCombo �޽��� ó�����Դϴ�.


void CToolBarCombo::OnCbnSelchange()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_pWndMsgParent)
		m_pWndMsgParent->SendMessage(wm_ChangeSelCombo);
}

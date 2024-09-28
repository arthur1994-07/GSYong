// SelectTreeCtrl.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2011 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "./SelectTreeCtrl.h"
#include "../DoCharEdit/PropPages/PropertyChaItemMix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectTreeCtrl

CSelectTreeCtrl::CSelectTreeCtrl()
: m_pParentDlg(NULL)
{
}

CSelectTreeCtrl::~CSelectTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CSelectTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CSelectTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectTreeCtrl message handlers
void CSelectTreeCtrl::PreSubclassWindow() 
{
	CTreeCtrl::PreSubclassWindow();

	m_pParentDlg = DYNAMIC_DOWNCAST( CPropertyChaItemMix, GetParent() );
	ASSERT_VALID(m_pParentDlg);
}

void CSelectTreeCtrl::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	UNREFERENCED_PARAMETER(pNMTreeView);
	
	// TODO: Add your control notification handler code here
	m_pParentDlg->EnableProperties( pNMTreeView->itemNew.hItem );
	
	*pResult = 0;
}

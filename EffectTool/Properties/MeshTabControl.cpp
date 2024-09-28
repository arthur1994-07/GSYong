// MeshTabControl.cpp : implementation file
//
/////////////////////////////////////////////////////
// This class is provided as is and Ben Hill takes no
// responsibility for any loss of any kind in connection
// to this code.
/////////////////////////////////////////////////////
// Is is meant purely as a educational tool and may
// contain bugs.
/////////////////////////////////////////////////////
// ben@shido.fsnet.co.uk
// http://www.shido.fsnet.co.uk
/////////////////////////////////////////////////////
// Thanks to a mystery poster in the C++ forum on 
// www.codeguru.com I can't find your name to say thanks
// for your Control drawing code. If you are that person 
// thank you very much. I have been able to use some of 
// you ideas to produce this sample application.
/////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffectTool.h"

#include "EffSheetWithTab.h"

#include "MeshDisplayTab.h"
#include "MeshTransformTab.h"

#include "MeshTabControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CMeshTabControl

CMeshTabControl::CMeshTabControl()
{
	m_tabPages[0]=new CMeshTransformTab;
	m_tabPages[1]=new CMeshDisplayTab;
}

CMeshTabControl::~CMeshTabControl()
{
	for(int nCount=0; nCount < NUMBEROFPAGE; nCount++)
	{
		delete m_tabPages[nCount];
	}
}

void CMeshTabControl::Init ( MESH_PROPERTY* pProp, CsheetWithTab* pSheetTab )
{
	m_tabCurrent=0;

	m_tabPages[0]->DestroyWindow();
	m_tabPages[1]->DestroyWindow();

	m_tabPages[0]->Create(IDD_DIALOG_EFFMESH_TRANSFORM, this);
	m_tabPages[1]->Create(IDD_DIALOG_EFFMESH_DISPLAY, this);

	for ( int i=0; i<NUMBEROFPAGE; ++i )
	{
		m_tabPages[i]->ShowWindow(SW_HIDE);
	}
	m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
	m_tabPages[m_tabCurrent]->SetFocus();

	((CMeshTransformTab*)m_tabPages[0])->Init(pProp);
	((CMeshDisplayTab*)m_tabPages[1])->Init(pProp,pSheetTab);

	SetRectangle();
}

void CMeshTabControl::SetRectangle()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < NUMBEROFPAGE; nCount++)
	{
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

void CMeshTabControl::Apply()
{
	((CMeshTransformTab*)m_tabPages[0])->Apply();
	((CMeshDisplayTab*)m_tabPages[1])->Apply();
}

void CMeshTabControl::SetActive()
{
}

BEGIN_MESSAGE_MAP(CMeshTabControl, CTabCtrl)
	//{{AFX_MSG_MAP(CMeshTabControl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshTabControl message handlers

void CMeshTabControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_tabCurrent != GetCurFocus())
	{
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent=GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
	}
}

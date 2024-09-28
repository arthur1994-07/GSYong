// PaneDialog.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2009 Codejock Software, All Rights Reserved.
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
#include "../../VisualMaterialLib/Command/DxVMCommand.h"
#include "../Tool_VisualMaterial.h"
#include "PropertyGridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridDlg dialog

CPropertyGridDlg::CPropertyGridDlg(CWnd* pParent /*=NULL*/)
	: CPropertyGridDlgBase(CPropertyGridDlg::IDD, pParent)
    , m_rVMCommandPrev(NULL)
{
	//{{AFX_DATA_INIT(CPropertyGridDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropertyGridDlg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyGridDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyGridDlg)
    DDX_Control(pDX, IDC_PLACEHOLDER, m_wndPlaceHolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropertyGridDlg, CPropertyGridDlgBase)
	//{{AFX_MSG_MAP(CPropertyGridDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridDlg message handlers

BOOL CPropertyGridDlg::OnInitDialog()
{
	CPropertyGridDlgBase::OnInitDialog();

    // get the size of the place holder, this will be used when creating the grid.
	CRect rc;
	m_wndPlaceHolder.GetWindowRect( &rc );
	ScreenToClient( &rc );

	// TODO: Add extra initialization here
    // create the property grid.
	if ( m_wndPropertyGrid.Create( rc, this, IDC_PROPERTY_GRID ) )
	{
		m_wndPropertyGrid.SetVariableItemsHeight(TRUE);

		LOGFONT lf;
		GetFont()->GetLogFont( &lf );
	}

    m_wndPropertyGrid.EnableToolTips(TRUE);
    m_wndPropertyGrid.ShowHelp(FALSE);
    m_wndPropertyGrid.SetTheme(xtpGridThemeOffice2003);
    m_wndPropertyGrid.SetShowInplaceButtonsAlways(TRUE);

    SetResize(IDC_PROPERTY_GRID, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

typedef void (CXTPPropertyGridItem::*ITEMFUNCTIONPTR)();

void _DoCollapseExpand(CXTPPropertyGridItems* pItems, ITEMFUNCTIONPTR pFunction)
{
	for (int i = 0; i < pItems->GetCount(); i++)
	{
		CXTPPropertyGridItem* pItem = pItems->GetAt(i);
		if (pItem->HasChilds())
		{
			(pItem->*pFunction)();
			_DoCollapseExpand(pItem->GetChilds(), pFunction);
		}
	}
}

void CPropertyGridDlg::SelectCommand( DxVMCommand* pVMCommand )
{
    // 이전 창과 같다면 작업 안함.
    if ( m_rVMCommandPrev == pVMCommand )
        return;

    // 정보를 저장
    m_rVMCommandPrev = pVMCommand;

    // 선택된 것이 없다면 Reset
    if ( !pVMCommand )
    {
        m_wndPropertyGrid.ResetContent();
        return;
    }

    // Note : Add Command
    // Command에 맞게 Properties를 보여줌.
    switch( pVMCommand->GetType() )
    {
    case VMC_CORE:
        m_sPropertyGridCore.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_CONSTANT:
        m_sPropertyGridConstant.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_CONSTANT2VECTOR:
        m_sPropertyGridConstant2Vector.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_CONSTANT3VECTOR:
        m_sPropertyGridConstant3Vector.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_CONSTANT4VECTOR:
        m_sPropertyGridConstant4Vector.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_PANNER:
        m_sPropertyGridPanner.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_ROTATOR:
        m_sPropertyGridRotator.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_TEXTURE_COORDINATE:
        m_sPropertyGridTextureCoordinate.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_SINE:
        m_sPropertyGridSine.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_VECTOR_PARAMETER:
        m_sPropertyGridVectorParameter.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_TEXTURESAMPLE:
        m_sPropertyGridTextureSample.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_FRESNEL:
        m_sPropertyGridFresnel.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;

    case VMC_ADD:
    case VMC_MULTIPLY:
    case VMC_SUBTRACT:
    case VMC_CLAMP:
    case VMC_LINEAR_INTERPOLATE:
    case VMC_TIME:
    case VMC_REFLECTION_VECTOR:
        m_sPropertyGridDesc.CreatePropertyGrid( m_wndPropertyGrid, pVMCommand );
        break;
    };

    // 펼침
    _DoCollapseExpand( m_wndPropertyGrid.GetCategories(), &CXTPPropertyGridItem::Expand );
}
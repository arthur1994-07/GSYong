#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

#include "../Resource.h"

#include "./PropertyGridCore.h"
#include "./PropertyGridDesc.h"
#include "./PropertyGridConstants.h"
#include "./PropertyGridCoordinates.h"
#include "./PropertyGridMath.h"
#include "./PropertyGridParameters.h"
#include "./PropertyGridTexture.h"
#include "./PropertyGridUtility.h"
#include "./PropertyGridVectors.h"
#include "./PropertyGridVectorOps.h"

class DxVMCommand;
class CXTPPropertyGridItems;

#define CPropertyGridDlgBase CXTResizeDialog

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridDlg dialog

class CPropertyGridDlg : public CPropertyGridDlgBase
{
// Construction
public:
	CPropertyGridDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertyGridDlg)
	enum { IDD = IDD_PROPERTYGRID_DIALOG };

	//}}AFX_DATA

private:
    CXTPPropertyGrid    m_wndPropertyGrid;
    CStatic             m_wndPlaceHolder;
    DxVMCommand*        m_rVMCommandPrev;   // 이전에 선택된 Command

    // Note : Add Command
private:
    PropertyGridCore                m_sPropertyGridCore;
    PropertyGridConstant            m_sPropertyGridConstant;
    PropertyGridConstant2Vector     m_sPropertyGridConstant2Vector;
    PropertyGridConstant3Vector     m_sPropertyGridConstant3Vector;
    PropertyGridConstant4Vector     m_sPropertyGridConstant4Vector;
    PropertyGridPanner              m_sPropertyGridPanner;
    PropertyGridRotator             m_sPropertyGridRotator;
    PropertyGridTextureCoordinate   m_sPropertyGridTextureCoordinate;
    PropertyGridSine                m_sPropertyGridSine;
    PropertyGridVectorParameter     m_sPropertyGridVectorParameter;
    PropertyGridTextureSample       m_sPropertyGridTextureSample;
    PropertyGridFresnel             m_sPropertyGridFresnel;
    PropertyGridDesc                m_sPropertyGridDesc;

public:
    void SelectCommand( DxVMCommand* pVMCommand );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyGridDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnOK() {}
	virtual void OnCancel(){}

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyGridDlg)
	//afx_msg void OnNcPaint();
	//afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

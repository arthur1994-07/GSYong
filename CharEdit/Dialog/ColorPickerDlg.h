// ColorPickerDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CColorPickerDlg dialog
#pragma once

#include "../MfcExLib/ExLibs/Dib.h"
#include "../Util/Common.h"

extern COLORREF	g_ColorRef; // by luxes.
extern int		g_iColorResult; // by luxes.
extern int		g_iClickSpinColor; // by luxes.

class CColorPickerDlg : public CDialog
{
// Construction
public:
	CColorPickerDlg(COLORREF c,CWnd* pParent = NULL);   // standard constructor
	~CColorPickerDlg();
	COLORREF GetColor() { return color.color();};

	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL );

	enum { SPIN_NONE = 0, SPIN_RED, SPIN_GREEN, SPIN_BLUE, SPIN_HUE, SPIN_SAT, SPIN_VAL }; // by luxes.
	void SetIndex( int nIndex ) { m_nIndex = nIndex; } // by luxes.
	int GetIndex() { return m_nIndex; } // by luxes.

	void SetSpinControl( int nColorHeight );

// Dialog Data
	//{{AFX_DATA(CColorPickerDlg)
	enum { IDD = IDD_DIALOG_COLORS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorPickerDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditBlue();
	afx_msg void OnChangeEditGreen();
	afx_msg void OnChangeEditHue();
	afx_msg void OnChangeEditRed();
	afx_msg void OnChangeEditSat();
	afx_msg void OnChangeEditVal();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnDeltaposSpinRed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinGreen(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinBlue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinHue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinSat(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinVal(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DrawFilledColor(CDC *pDC,CRect cr,COLORREF c);
	void DrawLines(CDC *pDC);
	void DrawXorRect(CDC *pDC,CRect& cr);
	void CalcSlopes();
	void CalcCuboid();

	void CreateBrightDIB();
	void SetDIBPalette();
	void DrawMarkers(CDC *pDC);
	void TrackPoint(CPoint pt);
	void CalcRects();
		
	BOOL InCircle(CPoint pt);
	BOOL InBright(CPoint pt);


	void SetSpinVals();
	void SetEditVals();
	void DrawAll();

	void DrawRGB(CDC *pDC);
	void DrawHSB(CDC *pDC);

	void LoadMappedBitmap(CBitmap& bitmap,UINT nIdResource,CSize& size);

	CBitmap m_RgbBitmap,m_HsbBitmap;

	CDC memDC;
	CPoint m_Centre;
	CDIB m_BrightDIB;
	int rgbWidth,rgbHeight;
	int hsbWidth,hsbHeight;

	int m_nMouseIn;
	CRect m_CurrentRect,brightMark	;
	CRect brightRect;

	HSVType hsvColor;	

	RGBType color, m_OldColor;
	CPoint Vertex,Top,Left,Right;
	CRect  rects[3];
	CPoint m_Cuboid[8];
	BOOL m_bInMouse;
	int nIndex;
	int RedLen,GreenLen,BlueLen;
	LineDesc lines[3];


	CRect rgbRect;
	CRect hsbRect;
	CRect OldColorRect,NewColorRect;

	BOOL m_bInitOver,m_bInDrawAll;

	BOOL m_bModeless; // by luxes.
	int  m_nIndex; // by luxes.
	float m_fColorHeight;
};

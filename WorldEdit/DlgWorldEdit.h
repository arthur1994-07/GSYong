#if !defined(AFX_DLGEDIT_H__0468B8DD_19FA_4433_A0F1_00AC1A8DCD37__INCLUDED_)
#define AFX_DLGEDIT_H__0468B8DD_19FA_4433_A0F1_00AC1A8DCD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEdit.h : header file
//

struct DxFrame;
class CsheetWithTab;

#include "resource.h"
#include "../MfcExLib/ExLibs/CJFlatComboBox.h"
#include "../MfcExLib/ExLibs/FlatEdit.h"

class DxPiece;
class DxPrefab;
struct DXLANDEFF;

/////////////////////////////////////////////////////////////////////////////
// CDlgEdit dialog

class CDlgEdit : public CDialog
{
protected:
	CsheetWithTab	*m_psheetWithTab;

public:	
	void SelectFrameTree( const char *szFrame );
	void SelectPieceFrameTree( const char *szFrame );
	void SelectCamCollList( const char *szTexture );
    void SelectSingleEffectList( DXLANDEFF* pEff );

	CsheetWithTab* GetSheetChar() { return m_psheetWithTab; }

protected:

public:
    void SelectPieceList( DxPiece* pPiece, int nCurType );
	void SelectPrefabList( DxPrefab* pPrefab );
    void SelectTexEffList( const TSTRING& strName );
	void UpdatePage_Always();
	void UpdatePage_Event( BOOL bUp );
	void UpdatePage_ListChange();
	void UpdatePage_List_AddLastItem();
	void KeyBoard_Enter();
    void UpdateKeyBoardMouse();
	void UpdateCamColl();
	int GetActiveIndex();
    void InsertPiece();
    void SetAcitvePage(int nPage);
	void SetActivePagePrefab();
	void SetActivePagePiecePoint();
	void SetViewFrame();
	void SetViewOctree();
	void SetViewPrefab();

private:
	void SetActiveIndex( const INT emEditType );

public:
	void ResetContent();
	void UpdateContrl();

// Construction
public:
	CDlgEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEdit)
	enum { IDD = IDD_DIALOG_EDITFORM };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEdit)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CCJFlatComboBox m_comboEditType;
	afx_msg void OnCbnSelchangeComboEdittype();
protected:
	virtual void PostNcDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDIT_H__0468B8DD_19FA_4433_A0F1_00AC1A8DCD37__INCLUDED_)

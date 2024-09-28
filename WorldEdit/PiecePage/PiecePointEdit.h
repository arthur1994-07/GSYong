#pragma once
#include "afxcmn.h"

class CsheetWithTab;

// CPiecePointEdit 대화 상자입니다.

class CPiecePointEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPiecePointEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	BOOL m_bInitDlg;
	BOOL m_bFrameSelect;
	D3DXMATRIX* m_rmatPiece;
	void ReSetListCtrl();
    void SetPositionEdit();
    void SetHalfDecimal( float& fResult );
	BOOL UpdatePage_List_AddLastItem2();		// 마지막 데이터를 list 에 올린다.

public:
	void InsertPiece();
	void SelectPieceList( DxPiece* pPiece, int nCurType );

    void UpdateKeyBoardMouse();
	void KeyBoard_Delete();
    void KeyBoard_Enter();
	void UpdatePage_List_AddLastItem();
	void UpdatePageClickUP();

	void SetFocus( CListCtrl& listReplace, int nIndex );

public:
	CPiecePointEdit();
	virtual ~CPiecePointEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_POINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
public:
	CListCtrl m_listReplace;
	afx_msg void OnNMClickListReplace(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButtonRotateInit();
public:
	afx_msg void OnBnClickedButtonRotateRandom();
public:
	afx_msg void OnBnClickedButtonHeightPick();
public:
	afx_msg void OnBnClickedButtonHeightUp10();
public:
	afx_msg void OnBnClickedButtonHeightUp1();
public:
	afx_msg void OnBnClickedButtonHeightDown1();
public:
	afx_msg void OnBnClickedButtonHeightDown10();
public:
	afx_msg void OnBnClickedButtonDelete();
public:
	afx_msg void OnBnClickedButtonHeightResetAll();
public:
	afx_msg void OnBnClickedButtonTimeApply();
public:
	afx_msg void OnBnClickedRadioPush();
public:
	afx_msg void OnBnClickedRadioDontmove();
public:
	afx_msg void OnBnClickedButtonEditMode();
	afx_msg void OnBnClickedButtonFloatZero();
	afx_msg void OnBnClickedButtonSlope();
	afx_msg void OnBnClickedCheckMyRoomTest();
    afx_msg void OnBnClickedButtonSnapBase();
    afx_msg void OnBnClickedButtonSnapMove();
    afx_msg void OnBnClickedButtonReplace();
    afx_msg void OnBnClickedButtonScaleApply();
	afx_msg void OnBnClickedButtonCurPos1();
	afx_msg void OnBnClickedButtonCurPos2();
	afx_msg void OnBnClickedButtonResetFade();
	afx_msg void OnEnChangeEditVisibleRadius();
	afx_msg void OnEnChangeEditInvisibleRadius();
	afx_msg void OnBnClickedButtonChangePie15();
	afx_msg void OnBnClickedButtonRotateL5();
	afx_msg void OnBnClickedButtonRotateR5();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonShader30();
};

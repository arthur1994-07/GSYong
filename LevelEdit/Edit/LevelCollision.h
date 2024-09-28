#pragma once
#include "afxwin.h"


// CLevelCollision ��ȭ �����Դϴ�.

class CLevelCollision : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelCollision)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	static BOOL				g_bCheckViewNameCOLLISION;

private:
	BOOL		m_bLayer;
	D3DXMATRIX	m_matWorld;

public:
	void ActiveCollisionIndex();
	void ActiveCollisionLayerIndex();
	UINT GetLayerIndex();
	void UpdateKeyBoardMouse();

private:
	void UpdateContrl();
	void ViewContrls ( BOOL bView );
	void ListtingCollision();
	void SettingData_Dlg();
	void ChangeCollisionType( GLLandMan* pGLLandMan, const SimpleCollisionBase* pCollision, SimpleCollisionBase *pNew, int nIndex );

public:
	CLevelCollision();
	virtual ~CLevelCollision();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_COLLISION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listCollision;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnEnSetfocusEditName();
	afx_msg void OnBnClickedRadioPlane();
	afx_msg void OnBnClickedRadioBox();
	afx_msg void OnBnClickedRadioCylinder();
	afx_msg void OnEnChangeEditMinusX();
	afx_msg void OnEnChangeEditPlusX();
	afx_msg void OnEnChangeEditMinusZ();
	afx_msg void OnEnChangeEditPlusZ();
	afx_msg void OnEnChangeEditRotationY();
	afx_msg void OnEnChangeEditRadius();
	afx_msg void OnLbnSelchangeListCollision();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckViewName();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
};

#pragma once
#include "afxwin.h"

#include "../MfcExLib/ExLibs/Separator.h"

#include "../EngineLib/DxTools/DxViewPort.h"

// CCameraPage 대화 상자입니다.
class CsheetWithTab;
class DxFrameMesh;
class DxCameraAni;

class CCameraPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCameraPage)

protected:
	CsheetWithTab	*m_pSheetTab;
	BOOL			m_bNewXFile;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	BOOL m_bDlgInit;
	DxCameraAni	m_CameraAni;
	DxFrameMesh* m_pFrameMeshForCamera;

protected:
	void ListCamera ();
	BOOL GetTickCamera ();
	void SetTickCamera ();
	DxCamAniMan* GetCameraAni();
	DxFrameMesh* GetFrameMesh();
	bool LoadXFileForCamera( const TCHAR* pFileName );

public:
	void ActiveCameraEdit ( BOOL bShow, BOOL bNew=TRUE );
	void UpdateContrl ();

public:
	CCameraPage();
	virtual ~CCameraPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CSeparator m_stPageLine;
	CSeparator m_stAniLine;
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonTarget();
	afx_msg void OnBnClickedButtonFrom();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonCancel();
	CListBox m_listAnimation;
	afx_msg void OnBnClickedRadioTargetcamera();
	afx_msg void OnBnClickedRadioFreecamera();
	afx_msg void OnBnClickedButtonTest();
	virtual BOOL OnInitDialog();
	CListBox m_listRegistCamera;
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonXfileLoad();
	afx_msg void OnBnClickedCheckNewXfile();
};

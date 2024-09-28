
#pragma once

class CsheetWithTabChar;
class CsheetWithTabAnim;

#include "../../RanLogic/FlyCamera/GLFlyCameraKeyFrameControl.h"

class CFlyCameraEditPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFlyCameraEditPage)

protected:
	CsheetWithTabChar*	m_pSheetTabParent;
	CsheetWithTabAnim*	m_pSheetTabAnim;

	GLFlyCameraKeyFrameControl	m_FlyCameControl;

public:
	enum { IDD = IDD_DIALOG_FLYCAMPAGE };	

public:
	//UI Resource
	CListCtrl m_listCameraAnchor;

public:
	CFlyCameraEditPage(void);
	virtual ~CFlyCameraEditPage(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void PostNcDestroy();

public:
	void UpdateList(void);

	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabParent = pSheetTab; }

	CsheetWithTabAnim*				GetAnimationTab ()	{ return m_pSheetTabAnim; }
	GLFlyCameraKeyFrameControl&		GetKeyFrameCtrl ()	{ return m_FlyCameControl; }

public:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnAnchorInsert();
	afx_msg void OnBnClickedBtnAnchorDelete();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnNMClickListCameraAnchor(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDMClickListCameraAnchor(NMHDR *pNMHDR, LRESULT *pResult);
};
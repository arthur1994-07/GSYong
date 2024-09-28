#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;
class DxJiggleBone;
struct JiggleBoneData;

// CJiggleBonePage 대화 상자입니다.

class CJiggleBonePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CJiggleBonePage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

private:
	DxJiggleBone*	m_pJiggleBone;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }
	void SetData( DxJiggleBone* pJiggleBone );

private:
	void UpdateListJiggleBone();
	void UpdateListJoint( JiggleBoneData* pJiggleBoneData );
	int GetCurSelJoint( JiggleBoneData** ppJiggleBoneData );

public:
	CJiggleBonePage();
	virtual ~CJiggleBonePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_JIGGLE_BONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnCheckCollision();
	CListBox m_listJigleBone;
	CListBox m_listJoint;
	afx_msg void OnLbnSelchangeListJiggleBone();
	afx_msg void OnLbnSelchangeListJoint();
	afx_msg void OnBnClickedDeleteJoint();
	afx_msg void OnBnClickedResetJoint();
	afx_msg void OnEnChangeEditTwist();
	afx_msg void OnEnChangeEditSwing();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnEnChangeEditDensity();
	afx_msg void OnEnChangeEditRotateX();
	afx_msg void OnEnChangeEditRotateY();
	afx_msg void OnEnChangeEditRotateZ();
	afx_msg void OnEnChangeEditSwingZ();
	afx_msg void OnEnChangeEditTwistRestitution();
	afx_msg void OnEnChangeEditTwistSpring();
	afx_msg void OnEnChangeEditTwistDamping();
	afx_msg void OnEnChangeEditSwingRestitution();
	afx_msg void OnEnChangeEditSwingSpring();
	afx_msg void OnEnChangeEditSwingDamping();
};

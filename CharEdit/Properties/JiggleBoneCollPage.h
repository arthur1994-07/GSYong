#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;
class DxJiggleBoneColl;
struct JiggleBoneCollData;

// CJiggleBoneCollPage ��ȭ �����Դϴ�.

class CJiggleBoneCollPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CJiggleBoneCollPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

private:
	DxJiggleBoneColl*	m_pJiggleBoneColl;
	D3DXMATRIX			m_matWorld;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }
	void SetData( DxJiggleBoneColl* pJiggleBone );

private:
	void UpdateListCollisionMesh();
	int GetCurSelCollisionData( JiggleBoneCollData** ppJiggleBoneCollData );
	void SetCharData();

public:
	CJiggleBoneCollPage();
	virtual ~CJiggleBoneCollPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_JIGGLE_COLLISION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAuto();
	afx_msg void OnBnClickedCheckBox();
	afx_msg void OnBnClickedCheckSphere();
	afx_msg void OnBnClickedCheckCapsule();
	CListBox m_listCollisionMesh;
	afx_msg void OnLbnSelchangeListCollisionMesh();
};

#pragma once

#include "../Resource.h"
#include "afxwin.h"

class CsheetWithTabChar;
class DxClothColl;
struct ClothCollData;

// CClothCollPage ��ȭ �����Դϴ�.

class CClothCollPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CClothCollPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

private:
	DxClothColl*	m_pClothColl;
	D3DXMATRIX		m_matWorld;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }
	void SetData( DxClothColl* pJiggleBone );

private:
	void UpdateListCollisionMesh();
	int GetCurSelCollisionData( ClothCollData** ppJiggleBoneCollData );
	void SetCharData();

public:
	CClothCollPage();
	virtual ~CClothCollPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CLOTH_COLLISION };

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
	CListBox m_listCollisionMesh;
	afx_msg void OnLbnSelchangeListCollisionMesh();
};

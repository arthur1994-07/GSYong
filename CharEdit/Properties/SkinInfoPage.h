#pragma once

#include "PieceRelativePage.h"

// CSkinInfoPage ��ȭ �����Դϴ�.
class CsheetWithTabChar;
struct SKIN_SPEC_SSS_INFO;

class CSkinInfoPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkinInfoPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab )	{ m_pSheetTabChar = pSheetTab; }

private:
	SKIN_SPEC_SSS_INFO* GetSkinInfo();

public:
	CSkinInfoPage();
	virtual ~CSkinInfoPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SKIN_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonFaceSpecColor();
	afx_msg void OnBnClickedButtonBodySpecColor();
	afx_msg void OnBnClickedButtonBodySpec2Color();
	afx_msg void OnEnChangeEditFaceSpecPower();
	afx_msg void OnEnChangeEditBodySpecPower();
	afx_msg void OnEnChangeEditBodySpec2Power();
	afx_msg void OnBnClickedButtonFakeSSSColor();
	//afx_msg void OnEnChangeEditSSS_ColorR();
	//afx_msg void OnEnChangeEditSSS_ColorG();
	//afx_msg void OnEnChangeEditSSS_ColorB();
};

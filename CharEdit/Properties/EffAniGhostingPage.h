#pragma once

#include "../EngineLib/DxEffect/EffAni/DxEffAniGhosting.h"
#include "afxwin.h"

#include <map>
#include <list>
#include <string>

struct DxSkeleton;

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CEffAniGhostingPage ��ȭ �����Դϴ�.

class CEffAniGhostingPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffAniGhostingPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;
	CsheetWithTabAnim	*m_pSheetTabAni;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

protected:
	DxEffAniData*				m_pEffAni;
	EFFANI_PROPERTY_GHOSTING	m_Property;

	CString						m_sAnimName;

public:
	void SetInstance ( DxEffAniData* pEffAni )				{ m_pEffAni = pEffAni; }
	void SetProperty ( EFFANI_PROPERTY *pProp=NULL );

	void SetEtc ( CString sAnimName, CsheetWithTabAnim *pSheetTabAni );

protected:
	void FrameMarking ();

protected:
	void SetInitDialog();

public:
	CEffAniGhostingPage();
	virtual ~CEffAniGhostingPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFANI_GHOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_comboFrame_1;
	CComboBox m_comboFrame_2;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboFrame1();
	afx_msg void OnCbnSelchangeComboFrame2();
	afx_msg void OnBnClickedButtonColor();
};

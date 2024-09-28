#pragma once

#include "../EngineLib/DxEffect/EffAni/DxEffAniGhosting.h"
#include "afxwin.h"

#include <map>
#include <list>
#include <string>

struct DxSkeleton;

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CEffAniGhostingPage 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFANI_GHOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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

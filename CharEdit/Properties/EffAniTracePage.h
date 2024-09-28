#pragma once

#include "../EngineLib/DxEffect/EffAni/DxEffAniTrace.h"
#include "afxwin.h"

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CEffAniTracePage 대화 상자입니다.

class CEffAniTracePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffAniTracePage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;
	CsheetWithTabAnim	*m_pSheetTabAni;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

protected:
	DxEffAniData*				m_pEffAni;
	EFFANI_PROPERTY_TRACE		m_Property;

	CString						m_sAnimName;

public:
	void SetInstance( DxEffAniData* pEffAni )				{ m_pEffAni = pEffAni; }
	void SetProperty( EFFANI_PROPERTY *pProp=NULL );

	void SetEtc( CString sAnimName, CsheetWithTabAnim *pSheetTabAni );

public:
	CEffAniTracePage();
	virtual ~CEffAniTracePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFANI_TRACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTex();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

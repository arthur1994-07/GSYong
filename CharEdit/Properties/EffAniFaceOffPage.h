#pragma once

#include "../EngineLib/DxEffect/EffAni/DxEffAniFaceOff.h"

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CEffAniFaceOffPage 대화 상자입니다.

class CEffAniFaceOffPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffAniFaceOffPage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;
	CsheetWithTabAnim	*m_pSheetTabAni;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

protected:
	DxEffAniData*				m_pEffAni;
	EFFANI_PROPERTY_FACEOFF		m_Property;

	CString						m_sAnimName;

public:
	void SetInstance( DxEffAniData* pEffAni )				{ m_pEffAni = pEffAni; }
	void SetProperty( EFFANI_PROPERTY *pProp=NULL );

	void SetEtc( CString sAnimName, CsheetWithTabAnim *pSheetTabAni );

public:
	CEffAniFaceOffPage();
	virtual ~CEffAniFaceOffPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFANI_FACEOFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTex();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

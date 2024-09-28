#pragma once

#include "../EngineLib/DxEffect/EffAni/DxEffAniSingle.h"
#include "afxwin.h"

#include <map>
#include <list>
#include <string>

struct DxSkeleton;

class CsheetWithTabChar;
class CsheetWithTabAnim;

// CEffAniSinglePage 대화 상자입니다.

class CEffAniSinglePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffAniSinglePage)

protected:
	CsheetWithTabChar	*m_pSheetTabChar;
	CsheetWithTabAnim	*m_pSheetTabAni;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTabChar = pSheetTab; }

protected:
	DxEffAniData*				m_pEffAni;
	EFFANI_PROPERTY_SINGLE		m_Property;
	CString						m_sAnimName;

private:
	D3DXMATRIX	m_matWorld;

public:
	void SetInstance ( DxEffAniData* pEffAni )				{ m_pEffAni = pEffAni; }
	void SetProperty ( EFFANI_PROPERTY *pProp=NULL );

	void SetEtc ( CString sAnimName, CsheetWithTabAnim *pSheetTabAni );

	// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
	void ChangeTimeOfTimeLine( DWORD dwToTime );

	// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
	void OnButtonPlayOfCDlgTimeLine();

protected:
	void FrameMarking ();
	void SetMatrixAndActiveEditMatrix();
	void SetComboPos();

	// 모든데이저가 저장 및 셋팅 시킬때 쓰임.
	void SetDxEffAniData( BOOL bSetKeyFrame );

protected:
	void SetInitDialog();

public:
	CEffAniSinglePage();
	virtual ~CEffAniSinglePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFANI_SINGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSelect();
	CComboBox m_comboBip_1;
	CComboBox m_comboBip_2;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPos1();
	afx_msg void OnCbnSelchangeComboPos2();
	afx_msg void OnBnClickedCheckTimeday();
	afx_msg void OnBnClickedCheckTimenight();
	afx_msg void OnBnClickedCheckSkyFine();
	afx_msg void OnBnClickedCheckSkyrain();
	afx_msg void OnBnClickedCheckSkysnow();
	afx_msg void OnBnClickedCheckSkyleaves();
	afx_msg void OnBnClickedCheckCharmove();
	afx_msg void OnBnClickedRadioMatrix();
	afx_msg void OnBnClickedRadioPoint();
	afx_msg void OnBnClickedRadioRange();
	afx_msg void OnBnClickedRadioDirect();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckNeverDie();
};

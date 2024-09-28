#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharParticle.h"
#include "EffCharPage.h"

// CEffParticlePage 대화 상자입니다.

class CEffParticlePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffParticlePage)

private:
	void ResetParticleList( int nCur );

protected:
	EFFCHAR_PROPERTY_PARTICLE	m_Property; 

private:
	D3DXMATRIX	m_matWorld;

private:
	void ResetEditBox();	// Enable, Disable    Check!!
	void CheckCurserMRS();

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffParticlePage();
	virtual ~CEffParticlePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_PARTICLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_listPoint;
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLbnSelchangeListPoint();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTextureFind();
	afx_msg void OnBnClickedCheckRange();
	afx_msg void OnBnClickedCheckSequence();
	virtual BOOL OnKillActive();
	afx_msg void OnBnClickedCheckRotate();
	afx_msg void OnBnClickedCheckRemain();
};

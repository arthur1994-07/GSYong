#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharAmbient.h"
#include "EffCharPage.h"

// CEffAmbientPage 대화 상자입니다.

struct SAmbientEffList
{
	DxEffChar*		pEffChar;
	int				iSelectNum;
	SAmbientEffList()
	{
		pEffChar = NULL;
	}

};

class CEffAmbientPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffAmbientPage)

protected:
	std::vector<SAmbientEffList>	m_vecEffCharList;
	EFFCHAR_PROPERTY_AMBIENT	    m_Property;

public:
	void SetInstanceList ( std::vector<SAmbientEffList> vecEffList )				{ m_vecEffCharList = vecEffList; }
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffAmbientPage();
	virtual ~CEffAmbientPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_AMBIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadioTime();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedRadioTexcolor();
	afx_msg void OnBnClickedRadioAmbient();
};

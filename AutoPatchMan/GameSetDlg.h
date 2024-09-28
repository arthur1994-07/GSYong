#pragma once

#include "afxcmn.h"
#include "../EngineLib/DxCommon/d3dapp.h"
#include "afxwin.h"
#include "../RanLogic/RANPARAM.h"
#include "TransparentHelper.h"
#include "SliderCtrlEx.h"
#include "BitComboBox.h"
#include "StaticEx.h"
#include "BtnST.h"

// CGameSetDlg 대화 상자입니다.

class CGameSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameSetDlg)

public:
	CGameSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGameSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GAMESETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	long	m_MapVolume;
	long	m_SfxVolume;
	long	m_BgmVolume;

	std::string szCharShadow[5];
	std::string szCharDetail[3];
	std::string sz3DAlgorithm[3];
	std::string szFogRange[4];
	std::string szNationName[10];
	std::string szDefaultOption[5];

private:
	DWORD			m_dwScrWidth;
	DWORD			m_dwScrHeight;
	EMSCREEN_FORMAT	m_emScrFormat;
	UINT			m_uScrRefreshHz;


	//	NOTE
	//		몸통 끌어서 움직이기 관련
	BOOL	m_bLBDown;
	CPoint	m_MouseClickPos;

private:
	CD3DApplication* m_pD3DApp;

private:
	BOOL	InitSubCtrls();
	BOOL	InitCtrls	();
	BOOL	UpdateCtrls ();
	BOOL	InverseUpdateCtrls ();
	void	InitString	();
	void	InitDlgText ();

	void	SetVeryHighLevel();
	void	SetHighLevel();
	void	SetMediumLevel();
	void	SetLowLevel();

	DECLARE_MESSAGE_MAP()
public:	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCompatible();
	afx_msg void OnCbnSelchangeComboResolution();
	afx_msg void OnCbnSelchangeComboHz();
	
	int m_nDownloadArea;
	CString m_strAreaName[RANPARAM::MAX_CHINA_REGION];
	afx_msg void OnCbnSelchangeComboDefaultoption();
	afx_msg void OnBnClickedCheckButton();
	afx_msg void OnCbnSelchangeCombo();

	afx_msg void OnCnbSelChangeComboNation();

	CButtonST m_checkWindowMode;
	CButtonST m_checkRealReflect;
	CButtonST m_checkReflect;	
	CButtonST m_checkGlow;
	CButtonST m_checkBuff;
	CButtonST m_checkPostProcessing;
	CButtonST m_checkGameCursor;
	CButtonST m_checkAllMinimum;
	CButtonST m_checkTextureFont;
	CButtonST m_checkFrameLimit;
	CButtonST m_checkTargetEffect;
	CButtonST m_checkClickEffect;	
	CButtonST m_checkMineEffect;
	CButtonST m_checkNormalMap;
	CButtonST m_check3DSound;
	CButtonST m_checkBGM;
	CButtonST m_checkSFX;
	CButtonST m_checkMAP;
	CButtonST m_checkUsePassiveDN;


	CButtonST m_radioCnDownLoad1;
	CButtonST m_radioCnDownLoad2;
	CButtonST m_radioCnDownLoad3;

	CButtonST m_btnOk;
	CButtonST m_btnCancel;
	CButtonST m_btnSound;




	CSliderCtrlEx m_sliderBGM;
	CSliderCtrlEx m_sliderSFX;
	CSliderCtrlEx m_sliderMAP;

	CBitComboBox m_comboResolution;
	CBitComboBox m_comboHz;

	CBitComboBox m_comboDefaultOption;
	CBitComboBox m_comboFogRange;
	CBitComboBox m_comboShadowChar;
	CBitComboBox m_comboSkinDetail;	

	CBitComboBox m_combo3DAlgorithm;

	CStaticEx m_stTitle;
	CStaticEx m_stDisplay;
	CStaticEx m_stGraphic;
	CStaticEx m_stSound;
	CStaticEx m_stDownload;
	CStaticEx m_stDownArea;


	CStaticEx	m_stDefaultOption;
	CStaticEx	m_stFogRange;
	CStaticEx	m_stShadowChar;
	CStaticEx	m_stSkinDetail;

	CStaticEx	m_stBGM;
	CStaticEx	m_stSFX;
	CStaticEx	m_stMAP;

	CDC* m_pMemDC;
	CBitmap* m_pOldBitmap;
	CBitmap* m_pBitmapMem;

	LRESULT OnTransaprentBk( WPARAM wParam, LPARAM lParam);
	void BuildBkDC();
	afx_msg void OnPaint();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};

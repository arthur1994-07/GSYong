#pragma once
#include "../RanLogic/Character/GLCharDefine.h"

// CWearingDlg 대화 상자입니다.
enum EMSUIT;
enum VEHICLE_TYPE;

class CWearingDlg : public CDialog
{
	DECLARE_DYNAMIC(CWearingDlg)

public:
	CWearingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWearingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WEARINGDLG }; 

public:
	std::string		m_strArrInventoryFile[GLCI_NUM_ACTOR];	//	직업별 인벤토리 형상 파일.
	std::string		m_strWearingFile[GLCI_NUM_ACTOR];	//	형상 파일.
	std::string		m_strWearingFileEx[GLCI_NUM_ACTOR];	//	형상 파일.
	std::string		m_strPetWearingFile;

	EMSUIT			m_emSuit;						//	아이템 착용 타입.
	VEHICLE_TYPE	m_emVehicleType;					// TYPE

	void	LoadWearingfile( int nID );
	void	SetReadMode ();
	void	UpdateList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	afx_msg void OnBnClickedButtonWearingfileC1();
	afx_msg void OnBnClickedButtonWearingfileC2();
	afx_msg void OnBnClickedButtonWearingfileC3();
	afx_msg void OnBnClickedButtonWearingfileC4();
	afx_msg void OnBnClickedButtonWearingfileC5();
	afx_msg void OnBnClickedButtonWearingfileC6();
	afx_msg void OnBnClickedButtonWearingfileC7();
	afx_msg void OnBnClickedButtonWearingfileC8();
	afx_msg void OnBnClickedButtonWearingfileC9();
	afx_msg void OnBnClickedButtonWearingfileC10();
	afx_msg void OnBnClickedButtonWearingfileC11();
	afx_msg void OnBnClickedButtonWearingfileC12();
	afx_msg void OnBnClickedButtonWearingfileC13();
    afx_msg void OnBnClickedButtonWearingfileC14();
    afx_msg void OnBnClickedButtonWearingfileC15();
    afx_msg void OnBnClickedButtonWearingfileC16();
    afx_msg void OnBnClickedButtonWearingfileC17();
	afx_msg void OnBnClickedButtonWearingfileC18();
	afx_msg void OnBnClickedButtonWearingfileC19();
	afx_msg void OnBnClickedButtonWearingfileC20();
	afx_msg void OnBnClickedButtonWearingfileC21();
	virtual INT_PTR DoModal();
	afx_msg void OnBnClickedButtonWearingfileExC1();
	afx_msg void OnBnClickedButtonWearingfileExC2();
	afx_msg void OnBnClickedButtonWearingfileExC3();
	afx_msg void OnBnClickedButtonWearingfileExC4();
	afx_msg void OnBnClickedButtonWearingfileExC5();
	afx_msg void OnBnClickedButtonWearingfileExC6();
	afx_msg void OnBnClickedButtonWearingfileExC7();
	afx_msg void OnBnClickedButtonWearingfileExC8();
	afx_msg void OnBnClickedButtonWearingfileExC9();
	afx_msg void OnBnClickedButtonWearingfileExC10();
	afx_msg void OnBnClickedButtonWearingfileExC12();
	afx_msg void OnBnClickedButtonWearingfileExC13();
    afx_msg void OnBnClickedButtonWearingfileExC14();
    afx_msg void OnBnClickedButtonWearingfileExC15();
    afx_msg void OnBnClickedButtonWearingfileExC16();
    afx_msg void OnBnClickedButtonWearingfileExC17();
	afx_msg void OnBnClickedButtonWearingfileExC18();
	afx_msg void OnBnClickedButtonWearingfileExC19();
	afx_msg void OnBnClickedButtonWearingfileExC20();
	afx_msg void OnBnClickedButtonWearingfileExC21();
	afx_msg void OnBnClickedButtonSubiconfileC8();
};

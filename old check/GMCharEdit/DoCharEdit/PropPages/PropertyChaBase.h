#pragma once
#include "afxwin.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Character/CharacterSlot/GLCharSlotMan.h"

namespace gmce
{
	class IDBManager;
}

class CPropertyChaBase : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaBase)

public:
	CPropertyChaBase();
	virtual ~CPropertyChaBase();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_BASE };

	gmce::IDBManager *m_pDBMan;

public:
    int m_nChaNum;

	// GM 작업 로그를 남기기 위한 Old Data
	int m_nUserNum_old;
    int m_nChaNumDB_old;
    CString m_strChaName_old;
    int m_nChaTribe_old;
    int m_nChaClass_old;
    int m_nChaLevel_old;
    int m_nChaPK_old;
    int m_nChaBright_old;
    GLDWDATA m_chaHP_old;
    GLDWDATA m_chaMP_old;
	GLDWDATA m_chaSP_old;
	int m_nChaCP_old;
    LONGLONG m_llChaExp_old;
    LONGLONG m_llChaMoney_old;
    int m_nStartGate_old;
    int m_nStartMapID_old;
    float m_fStartPosX_old;
    float m_fStartPosY_old;
    float m_fStartPosZ_old;
    int m_nSaveMapID_old;
    float m_fSavePosX_old;
    float m_fSavePosY_old;
    float m_fSavePosZ_old;	
	DWORD m_dwChaSchool_old;
	DWORD m_dwChaFace_old;
	DWORD m_dwChaHair_old;
	int m_nChaLiving_old;    
    int m_nCallMapID_old;
    float m_fCallPosX_old;
    float m_fCallPosY_old;
    float m_fCallPosZ_old;
    int m_nInvenLine_old;
	int m_nCurrentSlot_old;

	SCHAR_SLOT_DATA m_sCharSlotData_old[ EMCHAR_SLOT_DATA_SIZE ];
	int m_nSlotState_old[ EMCHAR_SLOT_DATA_SIZE ];
   
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    void SetData(SCHARDATA2* pChar);
    void GetData(SCHARDATA2* pChar);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    virtual BOOL OnInitDialog();    
    int m_nUserNum;
    int m_nChaNumDB;
    CString m_strChaName;
    int m_nChaTribe;
    int m_nChaClass;
    int m_nChaLevel;
    int m_nChaPK;
    int m_nChaBright;
    GLDWDATA m_chaHP;
    GLDWDATA m_chaMP;
	GLDWDATA m_chaSP;
    int m_nChaCP;
    LONGLONG m_llChaExp;
    LONGLONG m_llChaMoney;
    int m_nStartGate;
    int m_nStartMapID;
    float m_fStartPosX;
    float m_fStartPosY;
    float m_fStartPosZ;
    int m_nSaveMapID;
    float m_fSavePosX;
    float m_fSavePosY;
    float m_fSavePosZ;
	virtual void OnOK();	
	afx_msg void OnBnClickedBtnBaseSave();
	afx_msg void OnBnClickedRadioCharSlotA ( void );
	afx_msg void OnBnClickedRadioCharSlotB ( void );
	afx_msg void OnBnClickedRadioCharSlotC ( void );
	afx_msg void OnBnClickedRadioCharSlotD ( void );
	afx_msg void OnBnClickedRadioCharSlotE ( void );
	DWORD m_dwChaSchool;
	DWORD m_dwChaFace;
	DWORD m_dwChaHair;
	int m_nChaLiving;
    // 귀환지점저장용 맵ID
    int m_nCallMapID;
    float m_fCallPosX;
    float m_fCallPosY;
    float m_fCallPosZ;
    int m_nInvenLine;
	int m_nCurrentSlot;

	SCHAR_SLOT_DATA m_sCharSlotData[ EMCHAR_SLOT_DATA_SIZE ];
	int m_nSlotState[ EMCHAR_SLOT_DATA_SIZE ];

	int m_nSelectSlot;
};

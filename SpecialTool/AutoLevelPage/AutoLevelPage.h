#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../Resource.h"

#include "../../RanLogicServer/AutoLevel/GLAutoLevelMan.h"

class	CsheetWithTab;

// CAutoLevelPage ��ȭ �����Դϴ�.

class CAutoLevelPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAutoLevelPage)

public:
	CAutoLevelPage( LOGFONT logfont );
	virtual ~CAutoLevelPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_AUTOLEVELPAGE };

public:
	CsheetWithTab*	m_pSheetTab;
	CFont*			m_pFont;
    GLAutoLevelMan* m_pAutoLevel;
    GLAutoLevel		m_AutoLevel;

	bool	m_bEdit;

public:
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	void	Init();
	void	UpdateItems ();
	void	AutoLevelUpdateItems();
	void	AutoLevelCommandUpdateItems();

	void	InverseUpdateItems();
	void	InverseAutoLevelUpdateItems();

	bool	NewFile();
	bool	LoadFile();
	bool	LoadFileOld();
	bool	SaveFile();
	
	bool	CsvSaveAll();
	bool	CsvLoadAll();
	bool	CsvSave();
	bool	CsvLoad();

	void	InitDefaultCtrls ();

	bool	IsEdit()	{ return m_bEdit; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_cAutoLevelList;
	afx_msg void OnLbnSelchangeListAutoLevel();
	CListCtrl m_cAutoLevelCMDList;
	afx_msg void OnBnClickedButtonAutolevelLoadold();
	//afx_msg void OnBnClickedButtonAutolevelConvert();
	afx_msg void OnBnClickedButtonAutolevelAdd();
	afx_msg void OnBnClickedButtonAutolevelDel();
	afx_msg void OnBnClickedButtonAutolevelSet();
	afx_msg void OnBnClickedButtonCommnadAdd();
	afx_msg void OnBnClickedButtonCommnadDel();
	afx_msg void OnBnClickedButtonCommnadUp();
	afx_msg void OnBnClickedButtonCommnadDown();
	afx_msg void OnCbnSelchangeComboCommand();
	afx_msg void OnBnClickedButtonAutolevelCsvsave();
	afx_msg void OnBnClickedButtonAutolevelCsvload();
	afx_msg void OnNMClickListAutolevelCmd(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCommnadEdit();
	// ������ ���� ���ķδ� �����;
	afx_msg void OnBnClickedButtonConvertXml();
};


namespace ConvTemp
{ // �� ���� 1.5R3 �̻����� ���� �� �Ŀ��� �Ʒ� �ڵ� ��� ���� ��;
	typedef std::map<std::string, DWORD> LevelFileNameMap;
	typedef LevelFileNameMap::const_iterator LevelFileNameMapCIter;
	typedef LevelFileNameMap::iterator LevelFileNameMapIter;
	typedef std::pair<std::string, DWORD> LevelFileNamePair;

	typedef std::vector<std::string> LevelFileNameVector;
	typedef LevelFileNameVector::const_iterator LevelFileNameVectorCIter;
	typedef LevelFileNameVector::iterator LevelFileNameVectorIter;

	class AutoLevel
	{
	public:
		AutoLevel(const DWORD& _mapID);
		~AutoLevel(void);		

		DWORD mapID;
		LevelFileNameMap mapFileNameMap;
		LevelFileNameVector mapFileNameVector;
	};

	typedef std::map<DWORD, AutoLevel> AutoLevelMap;
	typedef AutoLevelMap::const_iterator AutoLevelMapCIter;
	typedef AutoLevelMap::iterator AutoLevelMapIter;
	typedef std::pair<DWORD, AutoLevel> AutoLevelPair;

	class AutoLevelConverter
	{
	public:
		const unsigned int addMap(const DWORD& _mapID, const std::string& _levelFileName);

		AutoLevelConverter(void);
		~AutoLevelConverter(void);

		AutoLevelMap autoLevelMap;
	};

	
};




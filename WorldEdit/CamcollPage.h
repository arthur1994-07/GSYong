#pragma once
#include "afxwin.h"

#include <map>
#include <string>

class CsheetWithTab;
struct DxFrame;
struct DxMeshes;

struct CAMCOLLINFO
{
	DxMeshes* pMesh;
	BOOL*     pCamColl;
	DWORD     dwSubset;

	CAMCOLLINFO() : pMesh(NULL), pCamColl(FALSE), dwSubset(0)
	{
	}
};

// CCamcollPage 대화 상자입니다.
class CCamcollPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCamcollPage)

protected:
    typedef std::multimap<std::string, CAMCOLLINFO>           MULTIMAPCAMCOLL;
	typedef std::multimap<std::string, CAMCOLLINFO>::iterator MULTIMAPCAMCOLLITER;

protected:
	CsheetWithTab	*m_pSheetTab;

	MULTIMAPCAMCOLL  m_MapCamColl;

	INT32*           m_nBufferCel;
	UINT32           m_nMaxCel;

	BOOL             m_bDlgInit;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void SetList ( DxFrame *pFrameRoot );

	void SetCamCollItem  ( const std::string& strItemName, const BOOL bEnable );
	void SetCamCollRender( const std::string& strItemName, const BOOL bEnable );

protected:

	void AddList ( DxFrame *pFrameCur );

public:

	void SelectMesh ( const std::string& strItemName );

public:
	void UpdateContrl ();

public:
	CCamcollPage();
	virtual ~CCamcollPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CAMCOLL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_ListDisable;
	CListBox m_ListEnable;

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	afx_msg void OnLbnSelchangeListCamcollDisable();
	afx_msg void OnLbnSetFocusListCamcollDisable();
	afx_msg void OnLbnSelchangeListCamcollEnable();
	afx_msg void OnLbnSetFocusListCamcollEnable();
	afx_msg void OnBnClickedButtonCamcollUp();
	afx_msg void OnBnClickedButtonCamcollDown();
};

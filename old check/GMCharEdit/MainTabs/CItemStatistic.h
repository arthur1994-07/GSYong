#pragma once

#include <map>

#include "../../RanLogic/Item/GLItemMan.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CConsoleMessage.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"

#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Quest/GLQuest.h"

#include "../Util/CLogFile.h"
#include "afxwin.h"
#include "afxcmn.h"

namespace gmce
{
	class IDBManager;
}

typedef struct stItem
{
	WORD wMain;
	WORD wSub;
	int  nCount;

	stItem()
	{
		wMain  = 0;
		wSub   = 0;
		nCount = 1;
	};
}STITEM;

typedef std::map<DWORD, STITEM> MAPITEM;
typedef std::pair<DWORD, STITEM> PAIR;

class CItemStatistic : public CFormView
{
	DECLARE_DYNCREATE(CItemStatistic)

protected:
	CItemStatistic();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CItemStatistic();

public:
	enum { IDD = IDD_ITEMSTATISTIC };
	HANDLE m_hThread;
	MAPITEM m_mapItem;
	BOOL m_bInit;

	void StatisticProc();
	BOOL m_bRunning;

	gmce::IDBManager* m_pDBMan;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:	
	virtual void OnInitialUpdate();
	afx_msg void OnEnMaxtextEditItemChangeConsole();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CProgressCtrl m_StaticProgress;
	CEdit m_EditItemStatConsole;
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnStatStart();
	afx_msg void OnBnClickedBtnStatStop();
};
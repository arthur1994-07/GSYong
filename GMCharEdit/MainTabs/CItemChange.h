#pragma once

#include "../SubDialogs/DlgInvenAdd.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CConsoleMessage.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"

#include "../../RanLogicServer/Util/GLItemLimit.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Quest/GLQuest.h"

#include "../Util/CLogFile.h"
#include "afxcmn.h"
#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

class GLItemLimit;

class CItemChange : public CFormView
{
	DECLARE_DYNCREATE(CItemChange)

protected:
	CItemChange();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CItemChange();

public:
	enum { IDD = IDD_ITEMCHANGE };

	void ItemChangeStart();
	void ChangeProc();
	
	BOOL m_bRunning;
	HANDLE m_hThread;
	BOOL m_bInit;

	gmce::IDBManager *m_pDBMan;
    GLItemLimit* m_pItemLimit;


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
	afx_msg void OnBnClickedBtnChangeStop();
	afx_msg void OnBnClickedBtnChangeStart();
	afx_msg void OnBnClickedBtnChangeStart2();
	short m_sFromMain;
	short m_sFromSub;
	short m_sToMain;
	short m_sToSub;
	afx_msg void OnBnClickedBtnChangeSelectItem();
	CProgressCtrl m_ChangeProgress;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_EditItemChangeConsole;
	afx_msg void OnBnClickedBtnSelectItem2();
	afx_msg void OnBnClickedBtnSelectItem();
	afx_msg void OnClose();
};



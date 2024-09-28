// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

#include "FormLeft.h"
#include "FormRight.h"
#include "c_COdbcManager.h"

class IVersionManConfig;
typedef std::pair<int,CString> PAIR_NEWFILE;
typedef std::vector<PAIR_NEWFILE> VECTOR_NEWFILE;

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư��
protected:
	CSplitterWnd m_wndSplitter;
	BOOL m_bInitSplitter;
	
public:
	//COdbcManager*	m_pOdbc;
	CFormLeft*	m_pLeft;
	CFormRight*	m_pRight;
    IVersionManagerDb* m_pDbMan;
    IVersionManConfig* m_pConfig;

//	lst �� �� ���ο� ������ ��� �ӽ������� �����ϱ� ����;
//	�� ���ϸ� isPreDown�����Ҽ� �ְ� �ϱ� ���ؼ� �ӽ������� ����;
private:
	VECTOR_NEWFILE m_vNewFile;

// ������
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    IVersionManagerDb* GetDbMan() { return m_pDbMan; }
    IVersionManConfig* GetConfig() { return m_pConfig; }

	const VECTOR_NEWFILE GetVecNewFileName() const { return m_vNewFile; }
	void InsertNewFileName( const int nIndex, const CString strNewFile ) { m_vNewFile.push_back( std::make_pair(nIndex, strNewFile) ); }
	void ClearVecNewFileName(){ m_vNewFile.clear(); }

// ����
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndToolBar2;

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};



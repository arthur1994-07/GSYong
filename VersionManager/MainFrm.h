// MainFrm.h : CMainFrame 클래스의 인터페이스
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
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성
protected:
	CSplitterWnd m_wndSplitter;
	BOOL m_bInitSplitter;
	
public:
	//COdbcManager*	m_pOdbc;
	CFormLeft*	m_pLeft;
	CFormRight*	m_pRight;
    IVersionManagerDb* m_pDbMan;
    IVersionManConfig* m_pConfig;

//	lst 열 때 새로운 파일일 경우 임시적으로 저장하기 위함;
//	새 파일만 isPreDown설정할수 있게 하기 위해서 임시적으로 저장;
private:
	VECTOR_NEWFILE m_vNewFile;

// 재정의
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    IVersionManagerDb* GetDbMan() { return m_pDbMan; }
    IVersionManConfig* GetConfig() { return m_pConfig; }

	const VECTOR_NEWFILE GetVecNewFileName() const { return m_vNewFile; }
	void InsertNewFileName( const int nIndex, const CString strNewFile ) { m_vNewFile.push_back( std::make_pair(nIndex, strNewFile) ); }
	void ClearVecNewFileName(){ m_vNewFile.clear(); }

// 구현
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndToolBar2;

// 메시지 맵 함수를 생성했습니다.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};



// WorldEditDoc.h : interface of the CWorldEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDEDITDOC_H__1CBF9593_93B8_4179_909F_E0D9F0065929__INCLUDED_)
#define AFX_WORLDEDITDOC_H__1CBF9593_93B8_4179_909F_E0D9F0065929__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	CCheckIntegrityNaviMeshDlg;


class CWorldEditDoc : public CDocument
{
protected: // create from serialization only
	CWorldEditDoc();
	DECLARE_DYNCREATE(CWorldEditDoc)

private:
	TSTRING	m_strTitleOrig;

// Attributes
public:
	CCheckIntegrityNaviMeshDlg	*m_pCheckIntegrityDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorldEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Version 을 추가한다.
	void SetTitleAddVersion( LPCTSTR lpszTitle );
	const TSTRING& GetTitleOrig()	{ return m_strTitleOrig; }

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CWorldEditDoc)
	afx_msg void OnMenuitemExport();
	afx_msg void OnMenuitemExportload();
	afx_msg void OnMenuitemExportsave();
	afx_msg void OnMenuitemSettingSave();
	afx_msg void OnMenuitemSettingLoad();
	afx_msg void OnMenuitemPrefabSave();
	afx_msg void OnMenuitemPrefabLoad();
	void LoadExportFile(const TCHAR* pFileName);
	void LoadSettingFile(const TCHAR* pFileName);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuitemCheckintegrity();
	afx_msg void OnMenuitemExportPiece();
	afx_msg void OnButtonCollisionmap();
	afx_msg void OnFilePieceLoad();
	afx_msg void OnMenuitemVrmlLoad();
	afx_msg void OnMenuitemVrmlSave();
	afx_msg void OnExport2xfile();
	afx_msg void OnExport2navigationmesh();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDITDOC_H__1CBF9593_93B8_4179_909F_E0D9F0065929__INCLUDED_)

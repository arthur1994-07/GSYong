// LevelEditDoc.h : CLevelEditDoc 클래스의 인터페이스
//


#pragma once

class CLevelEditDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CLevelEditDoc();
	DECLARE_DYNCREATE(CLevelEditDoc)

// 특성
public:

// 작업
public:

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~CLevelEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileImport();
	afx_msg void OnFileImportExceptEffect();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	virtual void OnCloseDocument();
	afx_msg void OnFileCsvsave();
	afx_msg void OnFileAllCsvsave();
	afx_msg void OnFileAllCsvload();
	afx_msg void OnFileCsvload();
};



// LevelEditDoc.h : CLevelEditDoc Ŭ������ �������̽�
//


#pragma once

class CLevelEditDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CLevelEditDoc();
	DECLARE_DYNCREATE(CLevelEditDoc)

// Ư��
public:

// �۾�
public:

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CLevelEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
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



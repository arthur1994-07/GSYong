// UIEditDoc.h : CUIEditDoc Ŭ������ �������̽�
//


#pragma once

class CUIEditDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CUIEditDoc();
	DECLARE_DYNCREATE(CUIEditDoc)

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
	virtual ~CUIEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};



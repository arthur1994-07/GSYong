// UIEditDoc.cpp : CUIEditDoc 클래스의 구현
//

#include "stdafx.h"
#include "UIEdit.h"

#include "UIEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIEditDoc

IMPLEMENT_DYNCREATE(CUIEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CUIEditDoc, CDocument)
END_MESSAGE_MAP()


// CUIEditDoc 생성/소멸

CUIEditDoc::CUIEditDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CUIEditDoc::~CUIEditDoc()
{
}

BOOL CUIEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CUIEditDoc serialization

void CUIEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CUIEditDoc 진단

#ifdef _DEBUG
void CUIEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUIEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUIEditDoc 명령

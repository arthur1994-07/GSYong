// DlgTalkFileAdd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcAction.h"
#include "DlgTalkFileAdd.h"


// DlgTalkFileAdd 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgTalkFileAdd, CDialog)

DlgTalkFileAdd::DlgTalkFileAdd(CWnd* pParent /*=NULL*/)
	: CDialog(DlgTalkFileAdd::IDD, pParent)
    , m_TalkName(_T(""))
{

}

DlgTalkFileAdd::~DlgTalkFileAdd()
{
}

void DlgTalkFileAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);    
    DDX_Text(pDX, IDC_EDIT1, m_TalkName);
}


BEGIN_MESSAGE_MAP(DlgTalkFileAdd, CDialog)
END_MESSAGE_MAP()


// DlgTalkFileAdd 메시지 처리기입니다.

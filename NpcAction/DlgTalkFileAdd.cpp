// DlgTalkFileAdd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "NpcAction.h"
#include "DlgTalkFileAdd.h"


// DlgTalkFileAdd ��ȭ �����Դϴ�.

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


// DlgTalkFileAdd �޽��� ó�����Դϴ�.

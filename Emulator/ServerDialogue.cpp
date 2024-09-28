// AgentDialogue.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "./Emulator.h"
#include "./ServerDialogue.h"
#include "../SigmaCore/Log/LogDefine.h"


// CAgentDialogue 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerDialogue, CDialog)

CServerDialogue::CServerDialogue(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDialogue::IDD, pParent)
{

}

CServerDialogue::~CServerDialogue()
{
}

void CServerDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerDialogue, CDialog)
	ON_MESSAGE(WM_UPDATE_LOG_CONSOLE, OnMessageLogConsole)
END_MESSAGE_MAP()


LRESULT CServerDialogue::OnMessageLogConsole(WPARAM wParam, LPARAM lParam)
{
	char *pBuffer = (char *)lParam;

	if(!pBuffer)
		return E_FAIL;

	HWND hEditConsole = (HWND)wParam;

	::SendMessage(hEditConsole, WM_SETFOCUS,		(WPARAM) 0, 0);
	::SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX);
	::SendMessage(hEditConsole, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0);
	::SendMessage(hEditConsole, EM_SCROLLCARET,	(WPARAM) 0,				(LPARAM) 0);
	::SendMessage(hEditConsole, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) lParam);
	::SendMessage(hEditConsole, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1);
	::SendMessage(hEditConsole, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX);

	//SAFE_DELETE_ARRAY(pBuffer);
	
	return S_OK;
}

// CAgentDialogue 메시지 처리기입니다.

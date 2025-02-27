#include "stdafx.h"
#include "CharStateMachine.h"
#include "CharStateMachineType.h"
#include "FileNotifier.h"
#include "CharFileState.h"

#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../Properties/sheetWithTabChar.h"
#include "../Properties/sheetWithTabAnim.h"

#include "../SigmaCore/File/FileFind.h"
#include "../EngineLib/DxTools/TextureManager.h"

using namespace	CharStateMachine;

CCharFileState  g_kCharEditState;

void CharStateMachine::InitRefresher()
{
	
}

void CharStateMachine::DestroyRefresher()
{
	g_kCharEditState.Destroy();
}

void CharStateMachine::SetFocus(int nOld, int nNew)
{
	if (nOld == nNew)
	{
		return;
	}

	CParam kParam;
	kParam << GetActiveGroup(nNew);
    kParam << &CEditState::SetFocus;
    kParam << nOld;
	g_kCharEditState.SendMessage(kParam);

    Asset();
}

void CharStateMachine::Reload(int nAtvIndex)
{
	CParam kParam;
	kParam << GetActiveGroup(nAtvIndex);
    kParam << &CEditState::Reload;
	g_kCharEditState.SendMessage(kParam);
}

void CharStateMachine::Archive(const TCHAR* pRootFolder)
{
    CParam kParam;
    kParam << GetActiveGroup();
    kParam << &CEditState::Archive;
    kParam << pRootFolder;
    g_kCharEditState.SendMessage(kParam);
}

void CharStateMachine::InsertFileNotifier(int nType)
{
	CParam kParam;
    kParam << GetActiveGroup(nType);
    kParam << &CEditState::Insert;
	g_kCharEditState.SendMessage(kParam);    
}

void CharStateMachine::ClearFileNotifier(int nType)
{
	CParam kParam;
    kParam << GetActiveGroup(nType);
    kParam << &CEditState::Clear;
	g_kCharEditState.SendMessage(kParam);
}

void CharStateMachine::ChangeFIle(const TCHAR* pFile, const TCHAR* pChangeFile)
{
	CParam kParam;
	kParam << GetActiveGroup();
    kParam << &CEditState::Change;
    kParam << pFile << pChangeFile;
	g_kCharEditState.SendMessage(kParam);
}

void CharStateMachine::Asset()
{
    CParam kParam;
    kParam << GetActiveGroup();
    kParam << &CEditState::Asset;
    g_kCharEditState.SendMessage(kParam);
}

void CharStateMachine::Update()
{
	static UINT s_unSaveTickTime = 0;

	UINT unCurrentTick = GetTickCount();

	if (s_unSaveTickTime + GetUpdateTime() >= unCurrentTick)
	{
		return;
	}

	s_unSaveTickTime = unCurrentTick;

	int n = 0;

	CParam kParam;
    kParam << GetActiveGroup();
    kParam << &CEditState::Update;
	g_kCharEditState.SendMessage(kParam);

	kParam >> n;

	if (n > CFileNotifier::RETURNCODE_FAIL)
	{
		ForceRender();
	}
}

int CharStateMachine::GetActiveGroup(int nAtvIndex)
{
	static int nPreAtvIndex = CHARTAB_CHARFILE;

	switch (nAtvIndex)
	{
	case CHARTAB_CHARFILE:
		nPreAtvIndex = CHARTAB_CHARFILE;
		return CHARTAB_CHARFILE;

#ifndef DEF_SKIN_SYNC
	case CHARTAB_CHARPIECE:
		nPreAtvIndex = CHARTAB_CHARPIECE;
		return CHARTAB_CHARPIECE;
#endif

	case CHARTAB_CHAREFF:
		nPreAtvIndex = CHARTAB_CHAREFF;
		return CHARTAB_CHAREFF;

	case CHARTAB_ATTBONEFILE:
		nPreAtvIndex = CHARTAB_ATTBONEFILE;
		return CHARTAB_ATTBONEFILE;

	case CHARTAB_ATTACHLINK:
		nPreAtvIndex = CHARTAB_ATTACHLINK;
		return CHARTAB_ATTACHLINK;

	case CHARTAB_VEHICLE:
		nPreAtvIndex = CHARTAB_VEHICLE;
		return CHARTAB_VEHICLE;
	}

	return nPreAtvIndex;
}

DWORD CharStateMachine::GetUpdateTime()
{
	return 500;
}

void CharStateMachine::ForceRender()
{
	CCharEditApp*   pApp    = (CCharEditApp*)AfxGetApp();
	BOOL            bAct	= pApp->IsActive();

	if (bAct == TRUE)
	{
        return;
    }

	CMainFrame*     pFrame	 = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CCharEditView*  pView    = (CCharEditView*)pFrame->GetActiveView();

	if ( FAILED(pView->Render3DEnvironment()) )
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
	}
}

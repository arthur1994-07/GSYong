#include "stdafx.h"
#include "WorldStateMachine.h"
#include "WorldStateMachineType.h"
#include "FileNotifier.h"
#include "WorldEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

#include "../EngineLib/DxTools/TextureManager.h"

using namespace	WorldStateMachine;

CFrameEditState	m_kWorldEditState;

void WorldStateMachine::InitWorldStateMachine()
{
    
}

void WorldStateMachine::DestroyWorldStateMachine()
{
	m_kWorldEditState.Destroy();
}

void WorldStateMachine::SetFocus(int nOld, int nNew)
{
	if (nOld == nNew)
	{
		return;
	}

    CParam kParam;
    kParam << GetActiveGroup(nNew);
    kParam << &CEditState::SetFocus;
    kParam << nOld;
	m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::Reload(int nAtvIndex)
{
	CParam kParam;
	kParam << FRAMEPAGEINDEX;
    kParam << &CEditState::Reload;
	m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::Archive(const TCHAR* pRootFolder)
{
    CParam kParam;
    kParam << FRAMEPAGEINDEX;
    kParam << &CEditState::Archive;
    kParam << pRootFolder;
    m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::InsertFileNotifier(int nType)
{
	CParam kParam;
	kParam << FRAMEPAGEINDEX;
    kParam << &CEditState::Insert;
	m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::ClearFileNotifier(int nType)
{
	CParam kParam;
	kParam << FRAMEPAGEINDEX;
    kParam << &CEditState::Clear;
	m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::Asset(int nType)
{
    CParam kParam;
    kParam << GetActiveGroup(nType);
    kParam << &CEditState::Asset;
    m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::EditAsset(const TCHAR* pItem)
{
    CParam kParam;
    kParam << GetActiveGroup();
    kParam << &CEditState::EditAsset;
    kParam << pItem;
    m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::SelectAsset(DWORD dwData)
{
    CParam kParam;
    kParam << GetActiveGroup();
    kParam << &CEditState::SelectAsset;
    kParam << dwData;
    m_kWorldEditState.SendMessage(kParam);
}

void WorldStateMachine::Update()
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
	m_kWorldEditState.SendMessage(kParam);

	kParam >> n;

	if (n > CFileNotifier::RETURNCODE_FAIL)
	{
		ForceRender();
	}
}

int WorldStateMachine::GetActiveGroup(int nAtvIndex)
{
	static int nPreAtvIndex = FRAMEPAGEINDEX;

	switch (nAtvIndex)
	{
	case FRAMEPAGEINDEX:		// Frame Edit (형상)
		nPreAtvIndex = FRAMEPAGEINDEX;
		return FRAMEPAGEINDEX;

	case LIGHTPAGEINDEX:		// Light Edit (빛)
		nPreAtvIndex = LIGHTPAGEINDEX;
		return LIGHTPAGEINDEX;

	case WEATHERPAGEINDEX:		// Weather State (날씨)
		nPreAtvIndex = WEATHERPAGEINDEX;
		return WEATHERPAGEINDEX;

	case CAMERAPAGEINDEX:		// Camera Edit (카메라 설정)
		nPreAtvIndex = CAMERAPAGEINDEX;
		return CAMERAPAGEINDEX;

	case SOUNDPAGEINDEX:		// Sound Edit (사운드 설정)
		nPreAtvIndex = SOUNDPAGEINDEX;
		return SOUNDPAGEINDEX;

	case SINGLEEFFPAGEINDEX:	// Effect Edit (효과 설정)
		nPreAtvIndex = SINGLEEFFPAGEINDEX;
		return SINGLEEFFPAGEINDEX;

	case SKINOBJPAGEINDEX:		// SkinObj Edit (스킨오브젝트)
		nPreAtvIndex = SKINOBJPAGEINDEX;
		return SKINOBJPAGEINDEX;

	case LANDGATEPAGEINDEX:		// Land-Gate Edit (출입구)
		nPreAtvIndex = LANDGATEPAGEINDEX;
		return LANDGATEPAGEINDEX;

	case BGMEDITPAGEINDEX:		// BGM Edit (배경음악)
		nPreAtvIndex = BGMEDITPAGEINDEX;
		return BGMEDITPAGEINDEX;

	case FOGPAGEINDEX:			// Fog Edit (포그 설정)
		nPreAtvIndex = FOGPAGEINDEX;
		return FOGPAGEINDEX;

	case SKYBOXMANINDEX:		// Sky Box Manage
		nPreAtvIndex = SKYBOXMANINDEX;
		return SKYBOXMANINDEX;

	case MAPEDITPAGEINDEX:		// Map Edit (맵 수정)
		nPreAtvIndex = MAPEDITPAGEINDEX;
		return MAPEDITPAGEINDEX;

	case STATICMESHCREATEINDEX:	// Piece Edit (조각 파일 설정)
		nPreAtvIndex = STATICMESHCREATEINDEX;
		return STATICMESHCREATEINDEX;

	case PIECEMANAGERINDEX:		// Piece Object Manager
		nPreAtvIndex = PIECEMANAGERINDEX;
		return PIECEMANAGERINDEX;

	case PIECEPOINTPAGEINDEX:	// Piece Point Edit
		nPreAtvIndex = PIECEPOINTPAGEINDEX;
		return PIECEPOINTPAGEINDEX;

	case CAMCOLLPAGEINDEX:		// Camcoll Edit (카메라 충돌)
		nPreAtvIndex = CAMCOLLPAGEINDEX;
		return CAMCOLLPAGEINDEX;

	case WATERSURFPAGEINDEX:	// Water Surf (파도 생성)
		nPreAtvIndex = WATERSURFPAGEINDEX;
		return WATERSURFPAGEINDEX;
 	}

	return nPreAtvIndex;
}

DWORD WorldStateMachine::GetUpdateTime()
{
	return 500;
}

void WorldStateMachine::ForceRender()
{
	CWorldEditApp* pApp	= static_cast<CWorldEditApp*>(AfxGetApp());
	BOOL bAct			= pApp->IsActive();

	if (bAct == FALSE)
	{
		CMainFrame* pFrame		= static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd);
		CWorldEditView* pView	= static_cast<CWorldEditView*>(pFrame->GetActiveView());

		if ( FAILED(pView->Render3DEnvironment()) )
		{
			AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
		}
	}
}

bool WorldStateMachine::IsInMouse(CWnd* pWnd)
{
    CPoint kPt;
    GetCursorPos(&kPt);
    HWND hWnd = WindowFromPoint(kPt);

    return pWnd->GetSafeHwnd() == hWnd && GetFocus() == hWnd;
}
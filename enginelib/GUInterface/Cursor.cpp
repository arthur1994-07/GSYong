#include "pch.h"

#include "../DxTools/TextureManager.h"
#include "../DxTools/DxRenderStates.h"

#include "cursor.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CCursor& CCursor::GetInstance()
{
	static CCursor Instance;
	return Instance;
}

CCursor::CCursor(void) :
	m_bGameCursor(TRUE),
	m_emType(CURSOR_NORMAL),
	m_bShowCursor( TRUE ),
	m_hWnd(NULL),

	m_hCurDefault(NULL),
	m_hCurNormal(NULL),
	m_hCurSelect(NULL),
	m_hCurTalk(NULL),
	m_hCurNonAttack(NULL),
	m_hCurPostBox(NULL),
	m_hCurDecompose(NULL)
{
	m_strCurNormal    = "normal.cur";
	m_strCurAttack    = "attack.cur";
	m_strCurSelect    = "select.ani";
	m_strCurScroll    = "scroll.cur";
	m_strCurTalk      = "talk.cur";
	m_strCurNonAttack = "nonattack.cur";
	m_strCurPostBox	  = "post.cur";
	m_strCurDecompose = "decompose.cur";
}

CCursor::~CCursor(void)
{
}

void CCursor::SetCursorType ( EMCURSOR emType )
{
	m_emType = emType;
}

void CCursor::SetGameCursor ( BOOL _bCursor )
{
	m_bGameCursor = _bCursor;

	if ( m_bGameCursor )
	{
		m_strCurNormal    = "normal.cur";
		m_strCurAttack    = "attack.cur";
		m_strCurSelect    = "select.ani";
		m_strCurScroll    = "scroll.cur";
		m_strCurTalk      = "talk.cur";
		m_strCurNonAttack = "nonattack.cur";
		m_strCurPostBox	  = "post.cur";
		m_strCurDecompose = "decompose.cur";
	}
	else
	{
		m_strCurNormal    = "Arrow";
		m_strCurAttack    = "Arrow";
		m_strCurSelect    = "Arrow";
		m_strCurScroll    = "Arrow";
		m_strCurTalk      = "Arrow";
		m_strCurNonAttack = "Arrow";
		m_strCurPostBox	  = "Arrow";
		m_strCurDecompose = "Arrow";
	}
}

HCURSOR CCursor::GetCurCursor ()
{
	HCURSOR hCur(NULL);

	switch(m_emType)
	{
	case CURSOR_NORMAL:		hCur = m_hCurNormal;		break;
	case CURSOR_ATTACK:
		{
			if ( m_vecCurAttack.size() > 1 )
			{
				int nSIZE = (int) m_vecCurAttack.size(); // MEMO
				CTime cTime = CTime::GetCurrentTime();
				int nCUR = cTime.GetMinute() % nSIZE;
				hCur = m_vecCurAttack[nCUR];
			}
			else
			{
				hCur = m_hCurNormal;
			}
		}
		break;

	case CURSOR_HAND:		hCur = m_hCurSelect;		break;
	case CURSOR_SCROLL:		hCur = m_hCurScroll;		break;
	case CURSOR_TALK:		hCur = m_hCurTalk;			break;
	case CURSOR_NONATTACK:  hCur = m_hCurNonAttack;     break;
	case CURSOR_POSTBOX:	hCur = m_hCurPostBox;		break;
	case CURSOR_DECOMPOSE:	hCur = m_hCurDecompose;		break;
	};

	if ( !hCur )			hCur = m_hCurDefault;

	return hCur;
}

void CCursor::SetCursorNow ()
{
	HCURSOR hCur = GetCurCursor ();
	HCURSOR hOld = ::SetCursor(hCur);
}

HRESULT CCursor::OneTimeSceneInit ( char *szPath, HWND hWnd )
{
	m_hWnd = hWnd;
	m_strPath = szPath;

	return S_OK;
}

HRESULT CCursor::InitDeviceObjects()
{
	DeleteDeviceObjects();

	SetCursorType ( m_emType );

	m_hCurDefault = LoadCursor ( NULL, IDC_ARROW );

	std::string strCursor;
	strCursor = m_strPath + m_strCurNormal;
	m_hCurNormal = LoadCursorFromFile ( strCursor.c_str() );
	 
	strCursor = m_strPath + m_strCurAttack;
	if ( m_strCurNormal==m_strCurAttack )
	{
		HCURSOR hCurAttack = LoadCursorFromFile ( strCursor.c_str() );
		m_vecCurAttack.push_back ( hCurAttack );

		CDebugSet::ToListView ( "load cursor : attack(%d)", hCurAttack );
	}
	else
	{
		int nSIZE = 6 + (rand()%6);
		for ( int i=0; i<nSIZE; ++i )
		{
			HCURSOR hCurAttack = LoadCursorFromFile ( strCursor.c_str() );
			m_vecCurAttack.push_back ( hCurAttack );

			CDebugSet::ToListView ( "load cursor : attack(%d)", hCurAttack );
		}
	}

	strCursor = m_strPath + m_strCurSelect;
	m_hCurSelect = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : select(%d)", m_hCurSelect );

	strCursor = m_strPath + m_strCurScroll;
	m_hCurScroll = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : scroll(%d)", m_hCurScroll );

	strCursor = m_strPath + m_strCurTalk;
	m_hCurTalk = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : talk(%d)", m_hCurTalk );

	strCursor = m_strPath + m_strCurNonAttack;
	m_hCurNonAttack = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : nonattack(%d)", m_hCurNonAttack );

	strCursor = m_strPath + m_strCurPostBox;
	m_hCurPostBox = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : PostBox(%d)", m_hCurPostBox );

	strCursor = m_strPath + m_strCurDecompose;
	m_hCurDecompose = LoadCursorFromFile ( strCursor.c_str() );
	CDebugSet::ToListView ( "load cursor : Decompose(%d)", m_hCurDecompose );

	return S_OK;
}

HRESULT CCursor::DeleteDeviceObjects()
{
	::SetCursor ( NULL );

	DestroyCursor ( m_hCurDefault );

	DestroyCursor ( m_hCurNormal );
	m_hCurNormal = NULL;

	for ( DWORD i=0; i<m_vecCurAttack.size(); ++i )
	{
		DestroyCursor ( m_vecCurAttack[i] );
	}
	m_vecCurAttack.clear();

	DestroyCursor ( m_hCurSelect );
	m_hCurSelect = NULL;
	DestroyCursor ( m_hCurScroll );
	m_hCurScroll = NULL;
	DestroyCursor ( m_hCurTalk );
	m_hCurTalk = NULL;
	DestroyCursor( m_hCurNonAttack );
	m_hCurNonAttack = NULL;
	DestroyCursor( m_hCurPostBox );
	m_hCurPostBox = NULL;
	DestroyCursor( m_hCurDecompose );
	m_hCurDecompose = NULL;

	return S_OK;
}

void CCursor::SetShowCursor ( BOOL bShow )
{
	m_bShowCursor = bShow;

	if ( bShow )
	{
		int nShow = ::ShowCursor(TRUE);
		while ( nShow<0 ) { nShow = ShowCursor(TRUE); }
		while ( nShow>0 ) { nShow = ShowCursor(FALSE); }
	}
	else
	{
		int nShow = ::ShowCursor(FALSE);
		while ( nShow>-1 ) { nShow = ShowCursor(FALSE); }
		while ( nShow<-1 ) { nShow = ShowCursor(TRUE); }
	}
}

BOOL CCursor::GetShowCursor()
{
	return m_bShowCursor;
}

HRESULT CCursor::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT CCursor::Render ( LPDIRECT3DDEVICEQ pD3DDevice, int x, int y )
{
	//return S_OK;

	RECT rtWindow;
	::GetWindowRect ( m_hWnd, &rtWindow );

	POINT ptCursorPos;
	::GetCursorPos ( &ptCursorPos );

	bool bMouseIn = rtWindow.left<ptCursorPos.x && ptCursorPos.x<rtWindow.right &&
					rtWindow.top<ptCursorPos.y && ptCursorPos.y<rtWindow.bottom;

	if ( bMouseIn )
	{
		HCURSOR hCurOld = ::GetCursor ();
		HCURSOR hCurNew = GetCurCursor ();
		if ( hCurNew==NULL )		hCurNew = m_hCurDefault;

		HCURSOR hOld = ::SetCursor(hCurNew);
	}

	return S_OK;
}

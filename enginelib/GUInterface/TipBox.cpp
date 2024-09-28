#include "pch.h"
#include "TipBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTipBox::CTipBox (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pLeftTop(NULL)
    , m_pTop(NULL)
    , m_pRightTop(NULL)
    , m_pRight(NULL)
    , m_pRightBottom(NULL)
    , m_pBottom(NULL)
    , m_pLeftBottom(NULL)
    , m_pLeft(NULL)
    , m_pBody(NULL)
{
}

CTipBox::~CTipBox ()
{
}

CUIControl*	CTipBox::CreateControl ( char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl, wAlignFlag );
	RegisterControl ( pControl );
	return pControl;
}

void CTipBox::CreateTipBox ( char* szBoxControl )
{	
	CreateBodyImage ( "TUTORIAL_BOX_BODY" );
	CreateLineImage ( "TUTORIAL_BOX_LEFT_TOP", "TUTORIAL_BOX_TOP", "TUTORIAL_BOX_RIGHT_TOP", "TUTORIAL_BOX_RIGHT",
		"TUTORIAL_BOX_RIGHT_BOTTOM", "TUTORIAL_BOX_BOTTOM", "TUTORIAL_BOX_LEFT_BOTTOM", "TUTORIAL_BOX_LEFT" );
	ResizeControl ( szBoxControl );
}

void CTipBox::CreateMouseTipBox ( char* szBoxControl )
{	
	CreateBodyImage ( "TUTORIAL_BOX_BODY" );
	CreateLineImage ( "TUTORIAL_BOX_LEFT_TOP", "TUTORIAL_BOX_TOP", "TUTORIAL_BOX_RIGHT_TOP", "TUTORIAL_BOX_RIGHT",
		"TUTORIAL_BOX_RIGHT_BOTTOM", "TUTORIAL_BOX_BOTTOM", "TUTORIAL_BOX_LEFT_BOTTOM", "TUTORIAL_BOX_LEFT" );
	ResizeControl ( szBoxControl );
}
void CTipBox::CreateTipBoxNoBody ( char* szBoxControl )
{
	CreateLineImage ( "TUTORIAL_EXPLAIN_BOX_LEFT_TOP", "TUTORIAL_EXPLAIN_BOX_TOP", "TUTORIAL_EXPLAIN_BOX_RIGHT_TOP", "TUTORIAL_EXPLAIN_BOX_RIGHT",
		"TUTORIAL_EXPLAIN_BOX_RIGHT_BOTTOM", "TUTORIAL_EXPLAIN_BOX_BOTTOM", "TUTORIAL_EXPLAIN_BOX_LEFT_BOTTOM", "TUTORIAL_EXPLAIN_BOX_LEFT" );
	ResizeControl ( szBoxControl );
}

void CTipBox::CreatePracticeModeBox ( char* szBoxControl )
{
	CreateBodyImage ( "TUTORIAL_PRACTICE_BOX_BODY" );
	CreateLineImage ( "TUTORIAL_PRACTICE_BOX_LEFT_TOP", "TUTORIAL_PRACTICE_BOX_TOP", "TUTORIAL_PRACTICE_BOX_RIGHT_TOP", "TUTORIAL_PRACTICE_BOX_RIGHT",
		"TUTORIAL_PRACTICE_BOX_RIGHT_BOTTOM", "TUTORIAL_PRACTICE_BOX_BOTTOM", "TUTORIAL_PRACTICE_BOX_LEFT_BOTTOM", "TUTORIAL_PRACTICE_BOX_LEFT" );
	ResizeControl ( szBoxControl );
}

void CTipBox::CreateNoLineBox ( char* szBoxControl )
{	
	CreateBodyImage ( "TUTORIAL_BOX_BODY" );
	/*CreateLineImage ( "TUTORIAL_BOX_LEFT_TOP", "TUTORIAL_BOX_TOP", "TUTORIAL_BOX_RIGHT_TOP", "TUTORIAL_BOX_RIGHT",
		"TUTORIAL_BOX_RIGHT_BOTTOM", "TUTORIAL_BOX_BOTTOM", "TUTORIAL_BOX_LEFT_BOTTOM", "TUTORIAL_BOX_LEFT" );*/
	ResizeControl ( szBoxControl );
}

void CTipBox::CreateQuestList( char* szBoxControl )
{
    CreateBodyImage ( "QUEST_WINDOW_PARENT_BOX_BODY" );
    CreateLineImage ( "QUEST_WINDOW_PARENT_BOX_LEFT_TOP", "QUEST_WINDOW_PARENT_BOX_TOP", "QUEST_WINDOW_PARENT_BOX_RIGHT_TOP", "QUEST_WINDOW_PARENT_BOX_RIGHT",
        "QUEST_WINDOW_PARENT_BOX_RIGHT_BOTTOM", "QUEST_WINDOW_PARENT_BOX_BOTTOM", "QUEST_WINDOW_PARENT_BOX_LEFT_BOTTOM", "QUEST_WINDOW_PARENT_BOX_LEFT" );
    ResizeControl ( szBoxControl );
}

void CTipBox::CreateQuestSelect( char* szBoxControl )
{
    CreateBodyImage ( "QUEST_WINDOW_CHILD_BOX_BODY" );
    CreateLineImage ( "QUEST_WINDOW_CHILD_BOX_LEFT_TOP", "QUEST_WINDOW_CHILD_BOX_TOP", "QUEST_WINDOW_CHILD_BOX_RIGHT_TOP", "QUEST_WINDOW_CHILD_BOX_RIGHT",
        "QUEST_WINDOW_CHILD_BOX_RIGHT_BOTTOM", "QUEST_WINDOW_CHILD_BOX_BOTTOM", "QUEST_WINDOW_CHILD_BOX_LEFT_BOTTOM", "QUEST_WINDOW_CHILD_BOX_LEFT" );
    ResizeControl ( szBoxControl );
}

void CTipBox::CreateLineImage ( char* szLeftTop, char* szTop, char* szRightTop, char* szRight,
									char* szRightBootm, char* szBottom, char* szLeftBotoom, char* szLeft )
{
	m_pLeftTop		= CreateControl ( szLeftTop, UI_FLAG_LEFT | UI_FLAG_TOP );
	m_pTop			= CreateControl ( szTop, UI_FLAG_XSIZE | UI_FLAG_TOP );
	m_pRightTop		= CreateControl ( szRightTop, UI_FLAG_RIGHT | UI_FLAG_TOP );
	m_pRight		= CreateControl ( szRight, UI_FLAG_YSIZE | UI_FLAG_RIGHT );
	m_pRightBottom	= CreateControl ( szRightBootm, UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
	m_pBottom		= CreateControl ( szBottom, UI_FLAG_XSIZE | UI_FLAG_BOTTOM );
	m_pLeftBottom	= CreateControl ( szLeftBotoom, UI_FLAG_LEFT | UI_FLAG_BOTTOM );
	m_pLeft			= CreateControl ( szLeft, UI_FLAG_YSIZE | UI_FLAG_LEFT );
}

void CTipBox::CreateBodyImage ( char* szBody )
{
	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( this, szBody, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RegisterControl ( pBody );
}

void CTipBox::ResizeControl ( char* szBoxControl )
{
	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CTipBox::SetUseRenderLine ( int nLine, BOOL bUseRender )
{
	switch ( nLine )
	{
	case LEFT_TOP:
		{
			if ( m_pLeftTop )		m_pLeftTop->SetUseRender ( bUseRender );
		}
		break;

	case TOP:
		{
			if ( m_pTop )			m_pTop->SetUseRender ( bUseRender );
		}		
		break;

	case RIGHT_TOP:
		{
			if ( m_pRightTop )		m_pRightTop->SetUseRender ( bUseRender );
		}
		break;

	case RIGHT:
		{
			if ( m_pRight )			m_pRight->SetUseRender ( bUseRender );
		}
		break;

	case RIGHT_BOTTOM:
		{
			if ( m_pRightBottom )			m_pRightBottom->SetUseRender ( bUseRender );
		}
		break;

	case BOTTOM:
		{
			if ( m_pBottom )			m_pBottom->SetUseRender ( bUseRender );
		}
		break;

	case LEFT_BOTTOM:
		{
			if ( m_pLeftBottom )			m_pLeftBottom->SetUseRender ( bUseRender );
		}
		break;

	case LEFT:
		{
			if ( m_pLeft )			m_pLeft->SetUseRender ( bUseRender );
		}
		break;
	}
}

void CTipBox::ResetAlignFlagLine ()
{
	if ( m_pLeftTop )		m_pLeftTop->ResetAlignFlag ();
	if ( m_pTop )			m_pTop->ResetAlignFlag ();
	if ( m_pRightTop )		m_pRightTop->ResetAlignFlag ();
	if ( m_pRight )			m_pRight->ResetAlignFlag ();
	if ( m_pRightBottom )	m_pRightBottom->ResetAlignFlag ();
	if ( m_pBottom )		m_pBottom->ResetAlignFlag ();
	if ( m_pLeftBottom )	m_pLeftBottom->ResetAlignFlag ();
	if ( m_pLeft )			m_pLeft->ResetAlignFlag ();
}

void CTipBox::SetAlignFlagLine ( int nLine, WORD wAlignFlag )
{
	switch ( nLine )
	{
	case LEFT_TOP:
		{
			if ( m_pLeftTop )		m_pLeftTop->SetAlignFlag ( wAlignFlag );
		}
		break;

	case TOP:
		{
			if ( m_pTop )			m_pTop->SetAlignFlag ( wAlignFlag );
		}		
		break;

	case RIGHT_TOP:
		{
			if ( m_pRightTop )		m_pRightTop->SetAlignFlag ( wAlignFlag );
		}
		break;

	case RIGHT:
		{
			if ( m_pRight )			m_pRight->SetAlignFlag ( wAlignFlag );
		}
		break;

	case RIGHT_BOTTOM:
		{
			if ( m_pRightBottom )			m_pRightBottom->SetAlignFlag ( wAlignFlag );
		}
		break;

	case BOTTOM:
		{
			if ( m_pBottom )			m_pBottom->SetAlignFlag ( wAlignFlag );
		}
		break;

	case LEFT_BOTTOM:
		{
			if ( m_pLeftBottom )			m_pLeftBottom->SetAlignFlag ( wAlignFlag );
		}
		break;

	case LEFT:
		{
			if ( m_pLeft )			m_pLeft->SetAlignFlag ( wAlignFlag );
		}
		break;
	}
}

void CTipBox::ResizeWindow(const UIRECT& rcNew)
{
	if( m_bCalcResizeWindow )
	{
		UIRECT rcChange;

		rcChange.sizeX	= 5;
		rcChange.sizeY	= 5;
		rcChange.left	= 0;
		rcChange.top	= 0;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pLeftTop->SetLocalPos(rcChange);

		rcChange.sizeX	= rcNew.sizeX - 10;
		rcChange.sizeY	= 5;
		rcChange.left	= 5;
		rcChange.top	= 0;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pTop->SetLocalPos(rcChange);

		rcChange.sizeX	= 5;
		rcChange.sizeY	= 5;
		rcChange.left	= rcNew.sizeX - 5;
		rcChange.top	= 0;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pRightTop->SetLocalPos(rcChange);

		rcChange.sizeX	= 5;
		rcChange.sizeY	= rcNew.sizeY - 10;
		rcChange.left	= rcNew.sizeX - 5;
		rcChange.top	= 5;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pRight->SetLocalPos(rcChange);

		rcChange.sizeX	= 5;
		rcChange.sizeY	= 5;
		rcChange.left	= rcNew.sizeX - 5;
		rcChange.top	= rcNew.sizeY - 5;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pRightBottom->SetLocalPos(rcChange);

		rcChange.sizeX	= rcNew.sizeX - 10;
		rcChange.sizeY	= 5;
		rcChange.left	= 5;
		rcChange.top	= rcNew.sizeY - 5;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pBottom->SetLocalPos(rcChange);

		rcChange.sizeX	= 5;
		rcChange.sizeY	= 5;
		rcChange.left	= 0;
		rcChange.top	= rcNew.sizeY - 5;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pLeftBottom->SetLocalPos(rcChange);

		rcChange.sizeX	= 5;
		rcChange.sizeY	= rcNew.sizeY - 10;
		rcChange.left	= 0;
		rcChange.top	= 5;
		rcChange.right	= rcChange.left + rcChange.sizeX;
		rcChange.bottom	= rcChange.top + rcChange.sizeY;

		m_pLeft->SetLocalPos(rcChange);
	}
}

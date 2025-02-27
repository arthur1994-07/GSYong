#include "pch.h"

#include "BasicTreeNode.h"
#include "BasicButton.h"
//#include "BasicTextBox.h"
#include "../DxTools/DxFontMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicTreeNode::CBasicTreeNode (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)	
	, m_pButton ( NULL )
    , m_pLabel ( NULL )
    , m_pParentNode ( NULL )
    , m_bBranchVisible ( FALSE )
{
}

CBasicTreeNode::~CBasicTreeNode ()
{
}

void	CBasicTreeNode::SetVisibleChild ( const TREENODE_LIST& list, BOOL bVisible )
{
	TREENODE_LIST_CITER iter = list.begin ();
	TREENODE_LIST_CITER iter_end = list.end ();
	for ( ; iter != iter_end; ++iter )
	{		
		(*iter)->SetVisibleSingle ( bVisible );
		SetVisibleChild ( (*iter)->GetChildList (), bVisible );
	}
}

void CBasicTreeNode::SetBranchVisible ( BOOL bBranchVisible )
{
	if ( m_ChildList.empty () ) return ;

	m_bBranchVisible = bBranchVisible;

	if ( bBranchVisible )
	{
		TREENODE_LIST_ITER iter = m_ChildList.begin ();
		TREENODE_LIST_ITER iter_end = m_ChildList.end ();
		for ( ; iter != iter_end; ++iter )
		{
			(*iter)->SetVisibleSingle ( TRUE );
		}
	}
	else
	{
		SetVisibleChild ( m_ChildList, FALSE );
	}
}

void CBasicTreeNode::CreateLabel ( char* szKeyword )
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pLabel = new CBasicTextBox(m_pEngineDevice);
	pLabel->CreateSub ( this, szKeyword, UI_FLAG_DEFAULT, TREENODE_TEXTBOX );
	pLabel->SetFont ( pFont );
	pLabel->SetTextAlign ( TEXT_ALIGN_LEFT );
	RegisterControl ( pLabel );
	m_pLabel = pLabel;
}

void CBasicTreeNode::CreateButton ( char* szKeyword, char* szFlipKeyword )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szKeyword, UI_FLAG_DEFAULT, TREENODE_EXPAND_BUTTON );
	pButton->CreateFlip ( szFlipKeyword, CBasicButton::RADIO_FLIP );
	pButton->SetVisibleSingle ( FALSE );
	RegisterControl ( pButton );
	m_pButton = pButton;
}

void CBasicTreeNode::InsertChildNode ( CBasicTreeNode* pChild )
{
	m_ChildList.push_back ( pChild );
	if ( m_pButton ) m_pButton->SetVisibleSingle ( TRUE );
}

CBasicTreeNode* CBasicTreeNode::GetChildNode ()
{
	if ( m_ChildList.empty () ) return NULL;
	return m_ChildList.front ();
}

void CBasicTreeNode::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	m_pLabel->SetUseOverColor ( FALSE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
}

void CBasicTreeNode::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case TREENODE_EXPAND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetBranchVisible ( !m_bBranchVisible );
				m_pButton->SetFlip ( m_bBranchVisible );
			}
		}
		break;

	case TREENODE_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pLabel->SetUseOverColor ( TRUE );
				if ( dwMsg & UIMSG_LB_DUP )
				{
					SetBranchVisible ( !m_bBranchVisible );
					m_pButton->SetFlip ( m_bBranchVisible );
				}
			}
		}
		break;
	}
}
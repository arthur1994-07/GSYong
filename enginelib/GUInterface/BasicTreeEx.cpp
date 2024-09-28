#include "pch.h"
#include "BasicTreeEx.h"
#include "UITextControl.h"
#include "../../Enginelib/GUInterface/UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicTreeEx::CBasicTreeEx (EngineDeviceMan* pEngineDevice)
    : CBasicTree(pEngineDevice)
	, m_NodeWndID ( TREENODE_BASE )
{
}

CBasicTreeEx::~CBasicTreeEx ()
{
}

CBasicTreeNode* CBasicTreeEx::InsertNode ( CBasicTreeNode* pParent, const CString& strNodeName )
{
	CBasicTreeNode* pNewNode = new CBasicTreeNode(m_pEngineDevice);
	pNewNode->CreateSub ( this, "BASIC_TREE_NODE", UI_FLAG_DEFAULT, m_NodeWndID++ );
	pNewNode->CreateLabel ( "BASIC_TREE_LABEL" );
	pNewNode->CreateButton ( "BASIC_TREE_LABEL_BUTTON", "BASIC_TREE_LABEL_BUTTON_F" );
	pNewNode->SetLabel ( strNodeName );	
	pNewNode->SetLabelOverColor ( NS_UITEXTCOLOR::PRIVATE );
	pNewNode->SetVisibleSingle ( FALSE );
	RegisterControl ( pNewNode );

	InsertNewNode ( pParent, pNewNode );

	return pNewNode;
}
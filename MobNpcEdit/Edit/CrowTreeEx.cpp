// AllItemView.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../MobNpcEdit.h"
#include "./SheetWithTab.h"
#include "./CrowTree.h"

#include "../EngineLib/G-Logic/GLOGIC.h"

#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/GLUseFeatures.h"

// CCrowTree 대화 상자입니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CCrowTree::AddItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )	//	서브로 추가시
	{		
		return AddItemMain( hMainItem );
	}
	else	//	메인으로 추가시
	{
		return AddItemSub ();
	}
	
	return FALSE;
}

BOOL CCrowTree::AddItemMain( HTREEITEM hMainItem )
{
	WORD MID = GetSelectItem()->sNativeID.wMainID;
	WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID ( MID );
	if ( SID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}

	SCROWDATA pItemNode;
	pItemNode.sNativeID.wMainID= MID;
	pItemNode.sNativeID.wSubID = SID;

	//	몬스터 공격셋트 한개 추가
	pItemNode.m_vecCrowAttack.push_back( SCROWATTACK() );

	char szNewName[ITEM_SZNAME];
	sprintf ( szNewName, "CN_%03d_%03d", MID, SID );
	StringCchCopy( pItemNode.m_szName, CHAR_SZNAME,  szNewName );
	GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, &pItemNode );
	PCROWDATA pCROWDATA = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

    HTREEITEM hParentItem = m_ctrlItemTree.GetParentItem ( hMainItem );
	if ( hParentItem != m_TreeRoot )
	{
		UpdateItem ( pCROWDATA, hParentItem );
	}
	else
	{
		UpdateItem ( pCROWDATA, hMainItem );
	}	

	return TRUE;
}

BOOL CCrowTree::AddItemSub ()
{
	WORD MID = GLCrowDataMan::GetInstance().FindFreeCrowDataMID ();
	if ( MID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}
	WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID ( MID );
	if ( SID == ITEMID_NOTFOUND )
	{
		return FALSE;
	}
	
	SCROWDATA pItemNode;
	pItemNode.sNativeID.wMainID= MID;
	pItemNode.sNativeID.wSubID = SID;

	//	몬스터 공격셋트 한개 추가
	pItemNode.m_vecCrowAttack.push_back( SCROWATTACK() );
	
	char szNewName[ITEM_SZNAME];
	sprintf ( szNewName, "CN_%03d_%03d", MID, SID );
	StringCchCopy ( pItemNode.m_szName, CHAR_SZNAME, szNewName );
	GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, &pItemNode );
	PCROWDATA pCROWDATA = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

	UpdateItem ( pCROWDATA, NULL );

	return TRUE;
}

void CCrowTree::OnBnClickedBtnExcelStrTblImport()
{
	CFileDialog dlg( TRUE, "*.*", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK )
		return ;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if ( GLCrowDataMan::GetInstance().LoadStrTblExcelFile(dlg.GetPathName().GetString()) == S_OK )
		{
			UpdateTree();
			MessageBox( _T("String Table CSV Load Success"), _T("Success"), MB_OK );
		}
	}
}

void CCrowTree::OnBnClickedBtnExcelStrTblExport()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strSavePath;
		if ( GLCrowDataMan::GetInstance().SaveStrTblExcelFile(strSavePath) == S_OK )
		{
			std::string OkMsg(
				sc::string::format("\"%1%\" Save Success!", strSavePath.c_str() ) );
			MessageBox( OkMsg.c_str(), _T("Success"), MB_OK );
		}
		else
		{
			MessageBox( _T("Save Fail"), _T("Fail"), MB_OK );
		}
	}
}

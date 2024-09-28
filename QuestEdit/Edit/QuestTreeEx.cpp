#include "stdafx.h"

#include "../QuestEdit.h"
#include "./SheetWithTab.h"
#include "./QuestTree.h"
#include "../Dialogue/InputIDDlg.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CQuestTree::UpdateItem ( PQUESTNODE pQuestNode, HTREEITEM hMainItem )
{
	CString strItem;
	strItem = GetFormatName ( pQuestNode );
	
	if ( !hMainItem )
	{
		hMainItem = m_ctrlQuestTree.InsertItem ( strItem, m_TreeRoot );
		m_ctrlQuestTree.SetItemData ( hMainItem, (DWORD_PTR)pQuestNode );
	}
	else
	{
		HTREEITEM hSubItem = m_ctrlQuestTree.InsertItem ( strItem, hMainItem );
		m_ctrlQuestTree.SetItemData ( hSubItem, (DWORD_PTR)pQuestNode );
	}

	return TRUE;
}

BOOL CQuestTree::DelQuest ()
{
	if ( GetSelectItem() )
	{

		CString strTemp = GLQuestMan::GetInstance().GetPath();

		strTemp += GetSelectItem()->strFILE.c_str();
		if ( ! DeleteFile( strTemp ) )
		{
			MessageBox( GetSelectItem()->strFILE.c_str(), "" , MB_OK );
		}


		DWORD WID = GetSelectItem()->pQUEST->m_sNID.dwID;

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strDeleteKey;
			strDeleteKey.Format("QN_%03d_000", WID);
			CGameTextMan::GetInstance().DeleteString(strDeleteKey.GetString(), CGameTextMan::EM_QUEST_TEXT);
			strDeleteKey.Format("QD_%03d_000", WID);
			CGameTextMan::GetInstance().DeleteString(strDeleteKey.GetString(), CGameTextMan::EM_QUEST_TEXT);

			const int nStepSize = GetSelectItem()->pQUEST->GetSTEPNUM();
			for ( int i = 0; i < nStepSize; ++i )
			{
				strDeleteKey.Format("QN_%03d_000_P_%d", WID, i);
				CGameTextMan::GetInstance().DeleteString(strDeleteKey.GetString(), CGameTextMan::EM_QUEST_TEXT);
				strDeleteKey.Format("QD_%03d_000_D_%d", WID, i);
				CGameTextMan::GetInstance().DeleteString(strDeleteKey.GetString(), CGameTextMan::EM_QUEST_TEXT);
			}
		}

		GLQuestMan::GetInstance().Delete( WID );		
		m_ctrlQuestTree.DeleteItem ( m_ctrlQuestTree.GetSelectedItem () );

		return TRUE;	
	}

	return FALSE;
}

void CQuestTree::UpdateName()
{
	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
	GASSERT ( hItem );
	PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
	GASSERT ( pQuestNode );

	m_ctrlQuestTree.SetItemText ( hItem, GetFormatName ( pQuestNode ) );
}

const char* CQuestTree::GetFormatName ( PQUESTNODE pQuestNode )
{
	GASSERT(pQuestNode->pQUEST);
    static CString strItem;
    if (!pQuestNode)
    {
        strItem = "";
        return strItem.GetString();
    }

	if (pQuestNode->pQUEST->m_strTITLE != "")
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strTitleKey;
			strTitleKey.Format(_T("QN_%03d_%03d"), pQuestNode->pQUEST->m_sNID.wMainID, pQuestNode->pQUEST->m_sNID.wSubID);

			strItem.Format(
				"[%04d]  [%s] [%s]",
				pQuestNode->pQUEST->m_sNID.Id(),
				CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()),
				pQuestNode->strFILE.c_str());
		}
		else
		{
			strItem.Format(
				"[%04d]  [%s] [%s]",
				pQuestNode->pQUEST->m_sNID.Id(),
				pQuestNode->pQUEST->m_strTITLE.c_str(),
				pQuestNode->strFILE.c_str());
		}
	}
	return strItem.GetString ();
}


BOOL	CQuestTree::AddQuest ()
{
	HTREEITEM hMainItem = m_ctrlQuestTree.GetSelectedItem ();

	return AddQuestSub ();

}

BOOL CQuestTree::AddQuestSub ()
{
	CInputIDDlg dlg;
	DWORD MID;
	if ( dlg.DoModal() != IDOK ) return FALSE;
		
	MID = dlg.m_nNewID;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strSubjectKey, strCommentKey;
		strSubjectKey.Format("QN_%03d_000", MID);
		strCommentKey.Format("QD_%03d_000", MID);
		CGameTextMan::GetInstance().InsertString(strSubjectKey.GetString(), strSubjectKey.GetString(), CGameTextMan::EM_QUEST_TEXT);
		CGameTextMan::GetInstance().InsertString(strCommentKey.GetString(), strCommentKey.GetString(), CGameTextMan::EM_QUEST_TEXT);
	}

	QUESTNODE sQuestNode;
	sQuestNode.pQUEST = new GLQUEST;
	sQuestNode.pQUEST->m_sNID = MID;

	CString strFile;
	strFile.Format( "QUEST_%d.qst", MID );
	sQuestNode.strFILE = strFile;


	GLQuestMan::GetInstance().Insert( MID, sQuestNode );
	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos = m_mapQuestMap.find( MID );

	if ( iter_pos == m_mapQuestMap.end() ) return FALSE;;
	PQUESTNODE pQuestNode = (PQUESTNODE) &iter_pos->second;

	pQuestNode->pQUEST->SAVE( pQuestNode->strFILE.c_str() );
	UpdateItem( pQuestNode, NULL );

	return TRUE;
}

void CQuestTree::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlQuestTree.ScreenToClient( &pt );

	UINT uFlag = 0;
	PQUESTNODE pItemNode = NULL;
	HTREEITEM hItem = m_ctrlQuestTree.HitTest ( CPoint(pt), &uFlag );

    SetSelectItem ( NULL );	
	if ( hItem )
	{
		m_ctrlQuestTree.SelectItem ( hItem );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
			SetSelectItem ( (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem ) );
		}
	}
}

BOOL	CQuestTree::PasteQuestNew ()
{
	if ( !m_bDummyHasQuest )
	{
		return FALSE;
	}

	CInputIDDlg dlg;
	DWORD MID;
	if ( GetSelectItem () )	//	����� �߰���
	{
		if ( dlg.DoModal() != IDOK ) return FALSE;
       
		MID = dlg.m_nNewID;
		QUESTNODE sQuestNode;
		sQuestNode.pQUEST = new GLQUEST;
		*(sQuestNode.pQUEST) = *(GetSelectItem()->pQUEST);
		sQuestNode.pQUEST->m_sNID = MID;
		
		CString strFile;
		strFile.Format( "QUEST_%d.qst", MID );
		sQuestNode.strFILE = strFile;

		GLQuestMan::GetInstance().Insert( MID, sQuestNode );
		GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
		GLQuestMan::MAPQUEST_ITER iter_pos = m_mapQuestMap.find( MID );

		if ( iter_pos == m_mapQuestMap.end() ) return FALSE;
		PQUESTNODE pQuestNode = (PQUESTNODE) &iter_pos->second;

		pQuestNode->pQUEST->SAVE( pQuestNode->strFILE.c_str() );
		UpdateItem( pQuestNode, NULL );

		return TRUE;
	}

	return FALSE;
}

BOOL	CQuestTree::PasteQuestValue ()
{
	if ( !m_bDummyHasQuest )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlQuestTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )
	{		
		DWORD MID = GetSelectItem()->pQUEST->m_sNID.dwID;
		PQUESTNODE pQuestNode = GetSelectItem();

		*(pQuestNode->pQUEST) = *(m_pDummyQuestNode->pQUEST);

		pQuestNode->pQUEST->m_sNID.dwID = MID;
		pQuestNode->pQUEST->SAVE( pQuestNode->strFILE.c_str() );

		m_ctrlQuestTree.SetItemText ( hMainItem, GetFormatName ( GetSelectItem() ) );
		
		return TRUE;
	}

	return FALSE;
}


// PropertyChaQuest.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "../../MainFrm.h"
#include "../../GMCharEdit.h"
#include "../PropertySheetCharEdit.h"
#include "./PropertyChaQuest.h"
#include "../../SubDialogs/DlgQuestAdd.h"
#include "../../SubDialogs/DlgQuestElementEdit.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Util/s_CCsvFile.h"
#include ".\propertychaquest.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CPropertyChaQuest ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPropertyChaQuest, CPropertyPage)
CPropertyChaQuest::CPropertyChaQuest()
	: CPropertyPage(CPropertyChaQuest::IDD)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyChaQuest::~CPropertyChaQuest()
{
}

void CPropertyChaQuest::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListQuest);
}


BEGIN_MESSAGE_MAP(CPropertyChaQuest, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BTN_QUEST_SAVE, OnBnClickedBtnQuestSave)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
    ON_BN_CLICKED(IDC_BTN_DELETE_QUEST, OnBnClickedBtnDeleteQuest)
	ON_BN_CLICKED(IDC_BTN_QUEST_EXPORT_TO_CSV, OnBnClickedBtnQuestExportToCsv)
END_MESSAGE_MAP()

BOOL CPropertyChaQuest::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_ListQuest.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();        
    InitListHead();
    SetData(pSheet->m_pChar);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CPropertyChaQuest::InitListHead()
{
    LVCOLUMN Col;
	CString strTemp;

	// Num, Name, Class, Tribe, Level,
	strTemp			= _T("Main");
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuest.InsertColumn(0, &Col);

	strTemp			= _T("Sub");
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuest.InsertColumn(1, &Col);

	strTemp			= _T("Name");
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuest.InsertColumn(2, &Col);

    strTemp			= _T("State");
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuest.InsertColumn(3, &Col);	
}

void CPropertyChaQuest::SetData(SCHARDATA2* pChar)
{
    // Erase all list items
	m_ListQuest.DeleteAllItems();
    m_vQuestProg.clear();

    // ������ ����Ʈ
    GLQuestPlay::MAPQUEST pMapQuest = pChar->m_cQuestPlay.GetQuestProc();

    GLQuestPlay::MAPQUEST_ITER iterStart = pMapQuest.begin();
    GLQuestPlay::MAPQUEST_ITER iterEnd   = pMapQuest.end();

    int nNum=0;

    for(; iterStart!=iterEnd; ++iterStart)
    {
        GLQUESTPROG* pQuestProg = (*iterStart).second;
        
        CPropertyChaQuest::QuestProg qp;        
        qp.pQuestProg = pQuestProg;
        qp.nStatus    = CPropertyChaQuest::PROGRESS;
        m_vQuestProg.push_back(qp);

        GLQUEST* pQuest = GLQuestMan::GetInstance().Find(pQuestProg->m_sNID.dwID);

		if (pQuest)
		{
			/*
			pQuest->m_sNID.wMainID; // Quest ���� Main ID
			pQuest->m_sNID.wSubID; // Quest ���� Sub ID
			pQuest->m_strTITLE;    // Quest Title
			pQuest->m_strCOMMENT; // Quest ����  
			*/

			int nCount=0;
			CString strTemp;
			nCount = m_ListQuest.GetItemCount();

			LV_ITEM lvItem;
			::memset(&lvItem, 0, sizeof(LV_ITEM));

			lvItem.mask = LVIF_TEXT; // �Ӽ� ����
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;
			strTemp.Format(_T("%d"), pQuest->m_sNID.wMainID);
			lvItem.pszText = strTemp.GetBuffer();		
			m_ListQuest.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
	    			
			strTemp.Format(_T("%d"), pQuest->m_sNID.wSubID);
			m_ListQuest.SetItemText(nNum, 1, strTemp);   

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strTitleKey;
				strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				m_ListQuest.SetItemText(nNum, 2, CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()));
			}
			else
			{
				m_ListQuest.SetItemText(nNum, 2, pQuest->m_strTITLE.c_str());
			}
			m_ListQuest.SetItemText(nNum, 3, "Progress");

			nNum++;
		}
    }

    // �Ϸ� ����Ʈ    
    GLQuestPlay::MAPQUEST pMapQuestCompleted = pChar->m_cQuestPlay.GetQuestEnd ();

    GLQuestPlay::MAPQUEST_ITER iterCompletedStart = pMapQuestCompleted.begin();
    GLQuestPlay::MAPQUEST_ITER iterCompletedEnd   = pMapQuestCompleted.end();    

    for(; iterCompletedStart!=iterCompletedEnd; ++iterCompletedStart)
    {
        GLQUESTPROG* pQuestProg = (*iterCompletedStart).second; 
        
        CPropertyChaQuest::QuestProg qp;        
        qp.pQuestProg = pQuestProg;
        qp.nStatus    = CPropertyChaQuest::COMPLETED;
        m_vQuestProg.push_back(qp);        

        GLQUEST* pQuest = GLQuestMan::GetInstance().Find(pQuestProg->m_sNID.dwID);

		if (pQuest)
		{
			/*
			pQuest->m_sNID.wMainID; // Quest ���� Main ID
			pQuest->m_sNID.wSubID; // Quest ���� Sub ID
			pQuest->m_strTITLE;    // Quest Title
			pQuest->m_strCOMMENT; // Quest ����  
			*/

			int nCount=0;
			CString strTemp;
			nCount = m_ListQuest.GetItemCount();

			LV_ITEM lvItem;
			::memset(&lvItem, 0, sizeof(LV_ITEM));

			lvItem.mask = LVIF_TEXT; // �Ӽ� ����
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;
			strTemp.Format(_T("%d"), pQuest->m_sNID.wMainID);
			lvItem.pszText = strTemp.GetBuffer();		
			m_ListQuest.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
	    			
			strTemp.Format(_T("%d"), pQuest->m_sNID.wSubID);
			m_ListQuest.SetItemText(nNum, 1, strTemp);       
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strTitleKey;
				strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
					m_ListQuest.SetItemText(nNum, 2, CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()));
			}
			else
			{
				m_ListQuest.SetItemText(nNum, 2, pQuest->m_strTITLE.c_str());
			}
			m_ListQuest.SetItemText(nNum, 3, _T("Completed"));

			nNum++;
		}
    }
}

// CPropertyChaQuest �޽��� ó�����Դϴ�.
void CPropertyChaQuest::OnBnClickedButton1()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();      

    CDlgQuestAdd dlgQuestAdd;
    if (dlgQuestAdd.DoModal() == IDOK)
    {
        if (dlgQuestAdd.m_nSelected != -1)
        {
            for (int i=0; i<(int) dlgQuestAdd.m_vSelected.size(); i++)
            {
                GLQUEST* pQuest = GLQuestMan::GetInstance().Find(dlgQuestAdd.m_vQuest[dlgQuestAdd.m_vSelected[i]].m_sNID.dwID );

                GLQUESTPROG pQuestProg;
                bool bQuest = pQuestProg.START(pQuest, NULL);

                if (bQuest)
                {               
                    // �����ؾ� �� ����Ʈ
                    pSheet->m_pChar->m_cQuestPlay.InsertProc(pQuestProg, false);
                    // �Ϸ� ����Ʈ
                    // pSheet->m_pChar->m_cQuestPlay.InsertEnd();
                    SetData(pSheet->m_pChar);

					///////////////////////////////////////////////////////////////////////////
					// GM Log
					CString strUserIP = m_pDBMan->GetLocalIP();
					int nGmNum = pFrame->m_nUserNum;
					std::strstream strGmCmd;
					strGmCmd << "Character(" << pSheet->m_pChar->m_CharDbNum << ") ";
					strGmCmd << "Quest(" << dlgQuestAdd.m_vQuest[dlgQuestAdd.m_vSelected[i]].m_sNID.wMainID << ",";
					strGmCmd << dlgQuestAdd.m_vQuest[dlgQuestAdd.m_vSelected[i]].m_sNID.wSubID << ") Add";
					strGmCmd << '\0';
					m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
					strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
					///////////////////////////////////////////////////////////////////////////
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// ĳ���� Quest ���� ����
void CPropertyChaQuest::OnBnClickedBtnQuestSave()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if (m_pDBMan->SaveCharacterQuest(pChar) == DB_ERROR)
	{
		MessageBox(_T("Database Error"));
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////
		// GM Log
		CString strUserIP = m_pDBMan->GetLocalIP();
		int nGmNum = pFrame->m_nUserNum;
		std::strstream strGmCmd;
		strGmCmd << "Character(" << pChar->m_CharDbNum << ") Quest Save";
		strGmCmd << '\0';
		m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
		strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		///////////////////////////////////////////////////////////////////////////
	}
}

void CPropertyChaQuest::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
    int nSelected = m_ListQuest.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
    
    if (nSelected != -1) // ���õ� �׸��� ����
    {
        CDlgQuestElementEdit dlgQuest;

        GLQUESTPROG* pQuestProg = m_vQuestProg[nSelected].pQuestProg;
        if (!pQuestProg)
        {
            MessageBox(_T("Wrong Quest Information")); // �߸��� ����Ʈ ����
            return;
        }

        dlgQuest.m_pQuestProg = pQuestProg;

        if (dlgQuest.DoModal() == IDOK)
        {
            SetData(pSheet->m_pChar);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Delete Quest
void CPropertyChaQuest::OnBnClickedBtnDeleteQuest()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

    int nSelected = m_ListQuest.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
    
    if (nSelected != -1) // ���õ� �׸��� ����
    {
        GLQUESTPROG* pQuestProg = m_vQuestProg[nSelected].pQuestProg;
        if (!pQuestProg)
        {
            MessageBox(_T("Wrong Quest Information")); // �߸��� ����Ʈ ����
            return;
        }

//         if (m_vQuestProg[nSelected].nStatus == CPropertyChaQuest::COMPLETED)
//         {
//             MessageBox(_T("Can not delete completed Quest")); // �Ϸ�� ����Ʈ�� ���� �� �� ����
//             return;
//         }

		GLQUESTPROG QuestDel;
		QuestDel.m_sNID = pQuestProg->m_sNID;
        bool bResult = pChar->m_cQuestPlay.DeleteProc(pQuestProg->m_sNID.dwID);
		if ( !bResult )
		{
			bResult = pChar->m_cQuestPlay.DeleteEnd(pQuestProg->m_sNID.dwID);
		}

        if (bResult)
        {
			///////////////////////////////////////////////////////////////////////////
			// GM Log			
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Character(" << pSheet->m_pChar->m_CharDbNum << ") ";
			strGmCmd << "Quest(" << QuestDel.m_sNID.wMainID << "," << QuestDel.m_sNID.wSubID << ") Delete";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////

            MessageBox(_T("Quest delete successed")); // ����Ʈ ���� ����
        }
        else
        {
            MessageBox(_T("Quest delete failed")); // ����Ʈ ���� ����
        }

        SetData(pSheet->m_pChar);
    }
}

// Property Page �� Save �� ������ OnOK �� ȣ��ȴ�.
// �̰����� Save ���Ѿ� ��ü Save �ÿ� ����� �����Ѵ�.
void CPropertyChaQuest::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	OnBnClickedBtnQuestSave();

	CPropertyPage::OnOK();
}

// ����Ʈ ����� CSV �� Export �Ѵ�.
void CPropertyChaQuest::OnBnClickedBtnQuestExportToCsv()
{	
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();    

	SCHARDATA2* pChar = pSheet->m_pChar;
	// ������ �����̸�
	CString strFileName(pChar->m_szName);
	strFileName += _T("_Quest_");

	SERVER_UTIL::CCsvFile csv(strFileName);

    // ������ ����Ʈ
    GLQuestPlay::MAPQUEST pMapQuest = pChar->m_cQuestPlay.GetQuestProc();

    GLQuestPlay::MAPQUEST_ITER iterStart = pMapQuest.begin();
    GLQuestPlay::MAPQUEST_ITER iterEnd   = pMapQuest.end();

    int nNum=0;

    for(; iterStart!=iterEnd; ++iterStart)
    {
        GLQUESTPROG* pQuestProg = (*iterStart).second;        
        
        GLQUEST* pQuest = GLQuestMan::GetInstance().Find(pQuestProg->m_sNID.dwID);

		if (pQuest)
		{
			/*
			pQuest->m_sNID.wMainID; // Quest ���� Main ID
			pQuest->m_sNID.wSubID; // Quest ���� Sub ID
			pQuest->m_strTITLE;    // Quest Title
			pQuest->m_strCOMMENT; // Quest ����  
			*/
			
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strTitleKey;
				strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				CString strTemp;
				strTemp.Format(_T("%d,%d,%s,progress"), 
					pQuest->m_sNID.wMainID,
					pQuest->m_sNID.wSubID,
					CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()));
				csv.Write(strTemp.GetBuffer());
			}
			else
			{
				CString strTemp;
				strTemp.Format(_T("%d,%d,%s,progress"), 
					pQuest->m_sNID.wMainID,
					pQuest->m_sNID.wSubID,
					pQuest->m_strTITLE.c_str());
				csv.Write(strTemp.GetBuffer());
			}
		}
    }

    // �Ϸ� ����Ʈ    
    GLQuestPlay::MAPQUEST pMapQuestCompleted = pChar->m_cQuestPlay.GetQuestEnd ();

    GLQuestPlay::MAPQUEST_ITER iterCompletedStart = pMapQuestCompleted.begin();
    GLQuestPlay::MAPQUEST_ITER iterCompletedEnd   = pMapQuestCompleted.end();    

    for(; iterCompletedStart!=iterCompletedEnd; ++iterCompletedStart)
    {
        GLQUESTPROG* pQuestProg = (*iterCompletedStart).second;         
        
        GLQUEST* pQuest = GLQuestMan::GetInstance().Find(pQuestProg->m_sNID.dwID);

		if (pQuest)
		{
			/*
			pQuest->m_sNID.wMainID; // Quest ���� Main ID
			pQuest->m_sNID.wSubID; // Quest ���� Sub ID
			pQuest->m_strTITLE;    // Quest Title
			pQuest->m_strCOMMENT; // Quest ����  
			*/
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strTitleKey;
				strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				CString strTemp;
				strTemp.Format(_T("%d,%d,%s,completed"), 
					pQuest->m_sNID.wMainID,
					pQuest->m_sNID.wSubID,
					CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()));
				csv.Write(strTemp.GetBuffer());
			}
			else
			{
				CString strTemp;
				strTemp.Format(_T("%d,%d,%s,completed"),
					pQuest->m_sNID.wMainID,
					pQuest->m_sNID.wSubID,
					pQuest->m_strTITLE.c_str());
				csv.Write(strTemp.GetBuffer());
			}
		}
	}
}

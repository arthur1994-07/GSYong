// PropertyChaSkill.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../MainFrm.h"
#include "../../GMCharEdit.h"
#include "./PropertyChaSkill.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgSkillAdd.h"
#include "../../SubDialogs/DlgSkillElementEdit.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CPropertyChaSkill 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyChaSkill, CPropertyPage)
CPropertyChaSkill::CPropertyChaSkill()
	: CPropertyPage(CPropertyChaSkill::IDD)
	, m_nSelectSlot(0)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyChaSkill::~CPropertyChaSkill()
{
}

void CPropertyChaSkill::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CHAR_EDIT_SKILL, m_ListSkill);
}


BEGIN_MESSAGE_MAP(CPropertyChaSkill, CPropertyPage)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_CHAR_EDIT_SKILL, OnNMDblclkListCharEditSkill)
    ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(ID_BTN_SKILL_SAVE, OnBnClickedBtnSkillSave)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_A, OnBnClickedRadioCharSlotA)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_B, OnBnClickedRadioCharSlotB)	
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_C, OnBnClickedRadioCharSlotC)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_D, OnBnClickedRadioCharSlotD)
	ON_BN_CLICKED(IDC_RADIO_CHARSLOT_E, OnBnClickedRadioCharSlotE)
END_MESSAGE_MAP()


// CPropertyChaSkill 메시지 처리기입니다.

BOOL CPropertyChaSkill::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ListSkill.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();        
    InitListHead();
    //SetData(pSheet->m_pChar);

	CButton* pRadio = static_cast< CButton* >( GetDlgItem( IDC_RADIO_CHARSLOT_A ) );
	if ( pRadio )
	{
		pRadio->SetCheck(BST_CHECKED);
	}

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return FALSE;

	switch ( pChar->m_wSlotIndex )
	{
	case 0:
		OnBnClickedRadioCharSlotA();
		break;

	case 1:
		OnBnClickedRadioCharSlotB();
		break;

	case 2:
		OnBnClickedRadioCharSlotC();
		break;

	case 3:
		OnBnClickedRadioCharSlotD();
		break;

	case 4:
		OnBnClickedRadioCharSlotE();
		break;
	}

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 리스트 컨트롤 헤더 초기화
void CPropertyChaSkill::InitListHead() 
{
	LVCOLUMN Col;
	CString strTemp;
	// Num, Name, Class, Tribe, Level,
	strTemp			= "Main";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(0, &Col);

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(1, &Col);

	strTemp			= "Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(2, &Col);

    strTemp			= "Level";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(3, &Col);

	strTemp			= "Version";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(4, &Col);

    strTemp			= "Size";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkill.InsertColumn(5, &Col);
}

void CPropertyChaSkill::SetData(SCHARDATA2* pChar)
{
    // Erase all list items
	m_ListSkill.DeleteAllItems();

    EMCHARINDEX emCharIndex = CharClassToIndex ( pChar->m_emClass );

    m_vSkill.erase(m_vSkill.begin(), m_vSkill.end());

    SKILL_MAP &sExpSkills = pChar->m_ExpSkills; // 익힌 스킬
    SKILL_MAP_ITER iter = sExpSkills.begin();
    SKILL_MAP_ITER iter_end = sExpSkills.end();

    int nNum=0;

    for ( ; iter!=iter_end; ++iter )
    {
        const SCHARSKILL &sCharSkill = (*iter).second;

		CString strSKILLNAME;
        PGLSKILL pSkillData = GLSkillMan::GetInstance().GetData ( sCharSkill.sNativeID.wMainID, sCharSkill.sNativeID.wSubID );
		if ( pSkillData )
		{
			strSKILLNAME = pSkillData->GetName();
		}
        
        //sCharSkill.wLevel;

        int nCount;
		CString strTemp;
		nCount = m_ListSkill.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
        strTemp.Format("%d", sCharSkill.sNativeID.wMainID);
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListSkill.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
				
        strTemp.Format("%d", sCharSkill.sNativeID.wSubID);
		m_ListSkill.SetItemText(nNum, 1, strTemp);

		m_ListSkill.SetItemText(nNum, 2, strSKILLNAME);
        
        strTemp.Format("%d", sCharSkill.wLevel);
		m_ListSkill.SetItemText(nNum, 3, strTemp);

        /*strTemp.Format("%d", sCharSkill.VERSION);
        m_ListSkill.SetItemText(nNum, 4, strTemp);

        strTemp.Format("%d", sCharSkill.SIZE);
        m_ListSkill.SetItemText(nNum, 5, strTemp);*/

        SCHARSKILL sSkill;
        sSkill.sNativeID.wMainID = sCharSkill.sNativeID.wMainID;
        sSkill.sNativeID.wSubID = sCharSkill.sNativeID.wSubID;
        m_vSkill.push_back(sSkill);

        nNum++;
    }
}

///////////////////////////////////////////////////////////////////////////////
// 스킬추가
void CPropertyChaSkill::OnBnClickedButton1()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* m_pChar = pSheet->m_pChar;

    CDlgSkillAdd dlgSkillAdd(false);
    if (dlgSkillAdd.DoModal() == IDOK)
    {
        if (dlgSkillAdd.m_nSelected != -1)
        {
            for (int i=0; i<(int) dlgSkillAdd.m_vSelected.size(); i++)
            {
                SCHARSKILL sCHARSKILL;                
                sCHARSKILL = dlgSkillAdd.m_vSkill[dlgSkillAdd.m_vSelected[i]];
                sCHARSKILL.wLevel = 0;

				// SKILL::MAX_LEVEL; // 최대 스킬레벨 체크

                std::pair<SKILL_MAP::iterator, bool> status = m_pChar->m_ExpSkills.insert(std::make_pair(sCHARSKILL.sNativeID.dwID, sCHARSKILL));
                if (status.second)
                {
                    SetData(m_pChar);
                    TRACE("Insert OK \n");

					///////////////////////////////////////////////////////////////////////////
					// GM Log
					CString strUserIP = m_pDBMan->GetLocalIP();
					int nGmNum = pFrame->m_nUserNum;
					std::strstream strGmCmd;
					strGmCmd << "Character(" << m_pChar->m_CharDbNum << ") ";
					strGmCmd << "Skill(" << sCHARSKILL.sNativeID.wMainID << "," << sCHARSKILL.sNativeID.wSubID << ") Add";
					strGmCmd << '\0';
					m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
					strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					///////////////////////////////////////////////////////////////////////////
                }
                else
                {
				    CString strTemp;
				    strTemp.Format("wMainID=%d wSubID=%d Insert Skill Failed", sCHARSKILL.sNativeID.wMainID, sCHARSKILL.sNativeID.wSubID);
				    MessageBox(strTemp);
                }
            }
        }
    }
}

void CPropertyChaSkill::OnBnClickedButton2()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* m_pChar = pSheet->m_pChar;

    int nSelected = m_ListSkill.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // 선택된 항목이 있음
	{	
        CDlgSkillElementEdit dlgEdit;

        dlgEdit.m_strMainID    = m_ListSkill.GetItemText(nSelected, 0);
        dlgEdit.m_strSubID     = m_ListSkill.GetItemText(nSelected, 1);
        dlgEdit.m_strSkillName = m_ListSkill.GetItemText(nSelected, 2);
        dlgEdit.m_strLevel     = m_ListSkill.GetItemText(nSelected, 3);

		int nLevel_old = ::atoi(dlgEdit.m_strLevel.GetString()); // 처음 스킬레벨

        if (dlgEdit.DoModal() == IDOK)
        {            
            // 수정된 레벨을 가져온다
            int nLevel = ::atoi(dlgEdit.m_strLevel.GetString()); // 수정된 스킬레벨
            if (nLevel > SKILL::MAX_LEVEL) // 최대 레벨보다 크다면 제한한다.
            {
                nLevel = SKILL::MAX_LEVEL;
            }

            // 캐릭터 스킬의 map 에서 위치를 가져온다.
			SCHARSKILL sSkill = m_vSkill[nSelected];
            SKILL_MAP_ITER iter = m_pChar->m_ExpSkills.find(sSkill.sNativeID.dwID);
			if( iter==m_pChar->m_ExpSkills.end() )
			{
				MessageBox("CPropertyChaSkill::OnBnClickedButton2() -- iter==m_pChar->m_ExpSkills.end()", "Error", MB_OK);
			}
            
			// 스킬 레벨을 수정한다.
            iter->second.wLevel = (WORD) nLevel;
			// 리스트 컨트롤 리플래쉬
            SetData(m_pChar);

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Character(" << m_pChar->m_CharDbNum << ") ";
			strGmCmd << "Skill(" << sSkill.sNativeID.wMainID << "," << sSkill.sNativeID.wSubID << ") Edit Level ";
			strGmCmd << nLevel_old << " To " << nLevel;
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////
        }
    }
    else
    {
        MessageBox("Select skill", "SKILL", MB_OK);
    }
}

void CPropertyChaSkill::OnNMDblclkListCharEditSkill(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    OnBnClickedButton2();

    *pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Skill Delete
void CPropertyChaSkill::OnBnClickedButton3()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
    int nSelected = m_ListSkill.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // 선택된 항목이 있음
	{        
        if (MessageBox("DELETE SELETED SKILL", "SKILL", MB_YESNO) == IDYES)
        {
            // 리스트 컨트롤 리플래쉬
			SCHARSKILL sSkill = m_vSkill[nSelected];
            pSheet->m_pChar->m_ExpSkills.erase((DWORD) sSkill.sNativeID.dwID);
            SetData(pSheet->m_pChar);

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Character(" << pSheet->m_pChar->m_CharDbNum << ") ";
			strGmCmd << "Skill(" << sSkill.sNativeID.wMainID << "," << sSkill.sNativeID.wSubID << ") Delete";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////
        }
    }   
    else
    {
        MessageBox("Select skill", "SKILL", MB_OK);        
    }
}

void CPropertyChaSkill::OnBnClickedBtnSkillSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}    

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;	
	m_pDBMan->SaveCharacterSlots(pChar);
	SetData(pChar);

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "Character(" << pChar->m_CharDbNum << ") Skill Save";
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////
}

void CPropertyChaSkill::OnBnClickedRadioCharSlotA ( void )
{
	CPropertySheetCharEdit* pSheet = static_cast< CPropertySheetCharEdit* >( GetParent() );
	if ( NULL == pSheet )
		return;

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return;

	if ( false == pChar->CHANGE_CHARACTER_SLOT( 0, true ) )
	{
		return;
	}

	m_nSelectSlot = 0;

	SetData( pChar );
}

void CPropertyChaSkill::OnBnClickedRadioCharSlotB ( void )
{
	CPropertySheetCharEdit* pSheet = static_cast< CPropertySheetCharEdit* >( GetParent() );
	if ( NULL == pSheet )
		return;

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return;

	if ( false == pChar->CHANGE_CHARACTER_SLOT( 1, true ) )
	{
		return;
	}

	m_nSelectSlot = 1;

	SetData( pChar );
}

void CPropertyChaSkill::OnBnClickedRadioCharSlotC ( void )
{
	CPropertySheetCharEdit* pSheet = static_cast< CPropertySheetCharEdit* >( GetParent() );
	if ( NULL == pSheet )
		return;

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return;

	if ( false == pChar->CHANGE_CHARACTER_SLOT( 2, true ) )
	{
		return;
	}

	m_nSelectSlot = 2;

	SetData( pChar );
}

void CPropertyChaSkill::OnBnClickedRadioCharSlotD ( void )
{
	CPropertySheetCharEdit* pSheet = static_cast< CPropertySheetCharEdit* >( GetParent() );
	if ( NULL == pSheet )
		return;

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return;

	if ( false == pChar->CHANGE_CHARACTER_SLOT( 3, true ) )
	{
		return;
	}

	m_nSelectSlot = 3;

	SetData( pChar );
}

void CPropertyChaSkill::OnBnClickedRadioCharSlotE ( void )
{
	CPropertySheetCharEdit* pSheet = static_cast< CPropertySheetCharEdit* >( GetParent() );
	if ( NULL == pSheet )
		return;

	GLCHARLOGIC* pChar = pSheet->m_pChar;
	if ( NULL == pChar )
		return;

	if ( false == pChar->CHANGE_CHARACTER_SLOT( 4, true ) )
	{
		return;
	}

	m_nSelectSlot = 4;

	SetData( pChar );
}

void CPropertyChaSkill::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	OnBnClickedBtnSkillSave();
	CPropertyPage::OnOK();
}

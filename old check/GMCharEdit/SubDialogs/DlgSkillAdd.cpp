// DlgSkillAdd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgSkillAdd.h"
#include "../MainFrm.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgSkillAdd 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSkillAdd, CDialog)
CDlgSkillAdd::CDlgSkillAdd(bool bAllSkill, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkillAdd::IDD, pParent)
    , m_bAllSkill(bAllSkill)
{
    m_nSelected = -1;
}

CDlgSkillAdd::~CDlgSkillAdd()
{
}

void CDlgSkillAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SKILL_ADD, m_ListSkillAdd);
}


BEGIN_MESSAGE_MAP(CDlgSkillAdd, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SKILL_ADD, OnNMDblclkListSkillAdd)
END_MESSAGE_MAP()


// CDlgSkillAdd 메시지 처리기입니다.

BOOL CDlgSkillAdd::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ListSkillAdd.SetExtendedStyle (LVS_EX_FULLROWSELECT ); 
    InitListHead();
    SetData();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgSkillAdd::InitListHead() 
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
	m_ListSkillAdd.InsertColumn(0, &Col);

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkillAdd.InsertColumn(1, &Col);

	strTemp			= "Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSkillAdd.InsertColumn(2, &Col);    
}

void CDlgSkillAdd::SetData()
{
    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

    // Erase all list items
	m_ListSkillAdd.DeleteAllItems();

    //EMCHARINDEX emCharIndex = CharClassToIndex ( pSheet->m_pChar->m_emClass );

    /*
    SCHARDATA2::SKILL_MAP &sExpSkills = pChar->m_ExpSkills; // 익힌 스킬
    SCHARDATA2::SKILL_MAP_ITER iter = sExpSkills.begin();
    SCHARDATA2::SKILL_MAP_ITER iter_end = sExpSkills.end();
    */

    m_vSkill.clear(); //erase(m_vSkill.begin(), m_vSkill.end());

	WORD wSkillM(0), wSkillEnd(0);
    if (!m_bAllSkill)
    {
	    GetCharSkillClassIndex( pSheet->m_pChar->m_emClass, wSkillM, wSkillEnd );
    }
    else
    {        
        wSkillM = 0;
        wSkillEnd = EMSKILLCLASS_NSIZE;
    }

    for (int nNum=0; wSkillM <= wSkillEnd; ++wSkillM)
    {
        WORD wMaxSkill = GLSkillMan::MAX_CLASSSKILL;
    
        for (int i=0; i<wMaxSkill; ++i)
        {        
            GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(wSkillM, i);
            if (!pSkillData)
                continue;
            
            int nCount;
		    CString strTemp;
		    nCount = m_ListSkillAdd.GetItemCount();

		    LV_ITEM lvItem;
		    ::memset(&lvItem, 0, sizeof(LV_ITEM));

		    lvItem.mask = LVIF_TEXT; // 속성 지정
		    lvItem.iItem = nCount;
		    lvItem.iSubItem = 0;
            strTemp.Format("%d", pSkillData->m_sBASIC.sNATIVEID.wMainID);
		    lvItem.pszText = strTemp.GetBuffer();		
		    m_ListSkillAdd.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
    				
            strTemp.Format("%d", pSkillData->m_sBASIC.sNATIVEID.wSubID);
		    m_ListSkillAdd.SetItemText(nNum, 1, strTemp);

		    m_ListSkillAdd.SetItemText(nNum, 2, pSkillData->GetName());

            SCHARSKILL sSkill;
            sSkill.sNativeID.wMainID = pSkillData->m_sBASIC.sNATIVEID.wMainID;
            sSkill.sNativeID.wSubID = pSkillData->m_sBASIC.sNATIVEID.wSubID;

            m_vSkill.push_back(sSkill);

            nNum++;
        }
    }
}

void CDlgSkillAdd::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    UINT uSelectedCount = m_ListSkillAdd.GetSelectedCount();
    int  nItem = -1;

    // Check selected item
    if (uSelectedCount > 0)
    {
        for (UINT i=0;i < uSelectedCount;i++)
        {
	        nItem = m_ListSkillAdd.GetNextItem(nItem, LVNI_ALL | LVNI_SELECTED);
        		
	        if (nItem != -1) // 선택된 항목이 있음
	        {	
                m_vSelected.push_back(nItem);
                m_nSelected = 1;
            }
            else
            {
                nItem = -1;
            }
        }
    }

    OnOK();
}

void CDlgSkillAdd::OnNMDblclkListSkillAdd(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedOk();

	*pResult = 0;
}

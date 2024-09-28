// DlgSkillSearch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <boost/timer.hpp>
#include "../SigmaCore/File/TxtFile.h"
#include "../SigmaCore/Util/MinMessageBox.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "../RanLogic/GLogicDataMan.h"

#include "./Database/DatabaseTable.h"
#include "./Util/CGmCfg.h"
#include "./SubDialogs/DlgSkillAdd.h"
#include "GMCharEdit.h"
#include "DlgSkillSearch.h"


// DlgSkillSearch

IMPLEMENT_DYNCREATE(DlgSkillSearch, CFormView)

DlgSkillSearch::DlgSkillSearch()
	: CFormView(DlgSkillSearch::IDD)
    , m_bSearchProgress(false)
{
    m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

DlgSkillSearch::~DlgSkillSearch()
{
}

void DlgSkillSearch::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SKILL_SEARCH_CHOOSE_SKILL, m_ListSelectedSkill);
    DDX_Control(pDX, IDC_PROGRESS_SKILL_SEARCH, m_ProgressSearch);
    DDX_Control(pDX, IDC_STATIC_SKILL_SEARCH_PROGRESS, m_ProgressText);
}

BEGIN_MESSAGE_MAP(DlgSkillSearch, CFormView)
    ON_BN_CLICKED(IDC_BTN_SKILL_SEARCH_SKILL_CHOOSE, &DlgSkillSearch::OnBnClickedBtnSkillSearchSkillChoose)
    ON_BN_CLICKED(IDC_BTN_SKILL_SEARCH_START, &DlgSkillSearch::OnBnClickedBtnSkillSearchStart)
END_MESSAGE_MAP()


// DlgSkillSearch 진단입니다.

#ifdef _DEBUG
void DlgSkillSearch::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void DlgSkillSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// DlgSkillSearch 메시지 처리기입니다.
BOOL DlgSkillSearch::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    return CFormView::PreCreateWindow(cs);
}

void DlgSkillSearch::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    m_ProgressSearch.SetRange(0, 100);

    m_ListSelectedSkill.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

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
    m_ListSelectedSkill.InsertColumn(0, &Col);

    strTemp			= "Sub";
    Col.mask		= LVCF_TEXT | LVCF_WIDTH;
    Col.cx			= 50;
    Col.fmt			= LVCFMT_CENTER;	
    Col.pszText		= strTemp.GetBuffer();	
    Col.iSubItem	= 0;
    Col.iImage		= 0;
    m_ListSelectedSkill.InsertColumn(1, &Col);

    strTemp			= "Name";
    Col.mask		= LVCF_TEXT | LVCF_WIDTH;
    Col.cx			= 200;
    Col.fmt			= LVCFMT_CENTER;	
    Col.pszText		= strTemp.GetBuffer();	
    Col.iSubItem	= 0;
    Col.iImage		= 0;
    m_ListSelectedSkill.InsertColumn(2, &Col);
}

void DlgSkillSearch::OnBnClickedBtnSkillSearchSkillChoose()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDlgSkillAdd DlgSkillAdd(true);
    if (DlgSkillAdd.DoModal() == IDOK)
    {
        m_SelectedSkill.clear();
        m_ListSelectedSkill.DeleteAllItems();

        int nNum = 0;
        for (size_t i=0; i<DlgSkillAdd.m_vSelected.size(); ++i)
        {
            SCHARSKILL CharSkill = DlgSkillAdd.m_vSkill[DlgSkillAdd.m_vSelected[i]];
            CharSkill.wLevel = 0;
            GLSKILL* pSkillData = GLogicData::GetInstance().GetSkillData(CharSkill.sNativeID);
            if (!pSkillData)
                continue;

            m_SelectedSkill.push_back(CharSkill);

            int nCount = m_ListSelectedSkill.GetItemCount();

            CString strTemp;
            LV_ITEM lvItem;
            ::memset(&lvItem, 0, sizeof(LV_ITEM));

            lvItem.mask = LVIF_TEXT; // 속성 지정
            lvItem.iItem = nCount;
            lvItem.iSubItem = 0;
            strTemp.Format("%d", CharSkill.sNativeID.wMainID);
            lvItem.pszText = strTemp.GetBuffer();		
            m_ListSelectedSkill.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.

            strTemp.Format("%d", CharSkill.sNativeID.wSubID);
            m_ListSelectedSkill.SetItemText(nNum, 1, strTemp);
            
            m_ListSelectedSkill.SetItemText(nNum, 2, pSkillData->GetName());

            ++nNum;
        }
    }
}

void DlgSkillSearch::OnBnClickedBtnSkillSearchStart()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_SelectedSkill.empty())
    {
        sc::ErrorMessageBox(std::string("There is no slelected skill."), false);
        return;
    }

    if (!m_bSearchProgress)
    {
        boost::thread SearchThread(boost::bind(&DlgSkillSearch::SearchThreadProc, this));
        m_bSearchProgress = true;
    }
    else
    {
        m_bSearchProgress = false;
        SetDlgItemText(IDC_BTN_SKILL_SEARCH_START, "Search");
    }
}

void DlgSkillSearch::SearchThreadProc()
{
    SetDlgItemText(IDC_BTN_SKILL_SEARCH_START, "Stop");

    if (m_pDBMan->GetConnectionType() == gmce::ODBC)
    {
        sc::ErrorMessageBox(std::string("ODBC not support this function."), false);
        SetDlgItemText(IDC_BTN_SKILL_SEARCH_START, "Search");
        return;
    }

    m_ProgressText.SetWindowText("Search start");
    
    // Get Character list
    m_ProgressText.SetWindowText("Get character list");
    std::vector<AllCharNum> ChaList;
    m_pDBMan->GetAllChar(ChaList);
    size_t ChaSize = ChaList.size();
    m_ProgressText.SetWindowText(sc::string::format("Server have %1% characters.", ChaSize).c_str());

    std::string FilePath(sc::getAppPath());
    FilePath.append("\\SkillSearch.csv");
    
    sc::TxtFile TxtFile(FilePath, true);
    TxtFile.write("DbNum,Name,Skill Mid,SkillSid,SkillName");

    size_t SearchCount = 0;
    boost::timer ElspTime;
    // Get Character skill and check    
    for (size_t i=0; i<ChaSize; ++i)
    {
        if (!m_bSearchProgress)
            break;
        
        GLCHARLOGIC* pChar = new GLCHARLOGIC;
        m_pDBMan->GetCharacterSkill(ChaList[i].ChaNum, pChar);
        for (size_t j=0; j<m_SelectedSkill.size(); ++j)
        {
            SNATIVEID SkillId = m_SelectedSkill[j].sNativeID;
            if (pChar->ISLEARNED_SKILL(SkillId, 0))
            {
                GLSKILL* pSkillData = GLogicData::GetInstance().GetSkillData(SkillId);
                std::string ResultStr(
                    sc::string::format(
                        "%1%,%2%,%3%,%4%,%5%",
                        pChar->CharDbNum(),
                        sc::string::ansi_to_utf8(std::string(pChar->m_szName)),
                        SkillId.wMainID,
                        SkillId.wSubID,
                        sc::string::ansi_to_utf8(std::string(pSkillData->GetName()))));
                TxtFile.write(ResultStr);                    
                ++SearchCount;
            }
        }
        delete pChar;

        int Percentage = static_cast<int> ((i*100)/(ChaSize-1));
        m_ProgressSearch.SetPos(Percentage);

        std::string ProgressMsg(
            sc::string::format(
                "%1%%% %2%/%3% %4%(Sec) Match %5%",
                Percentage, i, ChaSize, ElspTime.elapsed(), SearchCount));
        m_ProgressText.SetWindowText(ProgressMsg.c_str());
    }

    // Save result
    //m_ProgressText.SetWindowText("Search end");
    AfxMessageBox(TxtFile.FileName().c_str(), MB_OK);

    SetDlgItemText(IDC_BTN_SKILL_SEARCH_START, "Search");
}


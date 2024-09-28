// DlgNpcAction.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <boost/lexical_cast.hpp>
#include "../RanLogic/Quest/GLQuest.h"
#include "../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../RanLogic/NpcTalk/NpcDialogue.h"
#include "../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../enginelib/GUInterface/GameTextControl.h"

#include "QuestEdit.h"
#include "DlgNpcAction.h"


// DlgNpcAction 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgNpcAction, CDialog)

DlgNpcAction::DlgNpcAction(CWnd* pParent /*=NULL*/)
	: CDialog(DlgNpcAction::IDD, pParent)
    , m_StaticNpcAction1(_T(""))
    , m_TalkIndex(0)
{
}

DlgNpcAction::~DlgNpcAction()
{
}

void DlgNpcAction::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_NPC_ACTION_SUMMARY, m_ListNpcAction);
    DDX_Text(pDX, IDC_STATIC_NPC_ACTION1, m_StaticNpcAction1);
    DDX_Control(pDX, IDC_LIST_NPC_ACTION_NOTE, m_ListTalkNote);
}


BEGIN_MESSAGE_MAP(DlgNpcAction, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// DlgNpcAction 메시지 처리기입니다.
void DlgNpcAction::SetQuest(GLQUEST* pQuest)
{
    if (!pQuest)
        return;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		std::string QuestName(
			sc::string::format(
			"(%1%/%2%) %3% %4%",
			pQuest->m_sNID.Mid(),
			pQuest->m_sNID.Sid(),
			pQuest->m_sNID.Id(),
			CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString()));
		m_StaticNpcAction1 = QuestName.c_str();
	}
	else
	{
		std::string QuestName(
			sc::string::format(
			"(%1%/%2%) %3% %4%",
			pQuest->m_sNID.Mid(),
			pQuest->m_sNID.Sid(),
			pQuest->m_sNID.Id(),
			pQuest->GetTitleStr()));
		m_StaticNpcAction1 = QuestName.c_str();
	}
    UpdateList(pQuest);
    UpdateData(FALSE);
}

BOOL DlgNpcAction::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    InitListNpcAction();
    InitListTalkNote();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgNpcAction::InitListNpcAction()
{
    RECT rectCtrl;
    LONG lnWidth;
    LV_COLUMN lvColumn;

    m_ListNpcAction.SetExtendedStyle(m_ListNpcAction.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_ListNpcAction.GetClientRect(&rectCtrl);
    lnWidth = rectCtrl.right - rectCtrl.left;

    const int nColumnCount = 3;	
    char* szColumnName1[nColumnCount] = { "Step", "Npc", "Talk", };
    int nColumnWidthPercent1[nColumnCount] = { 30, 40, 30, };

    for (int i = 0; i < nColumnCount; ++i)
    {		
        lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.pszText = szColumnName1[i];
        lvColumn.iSubItem = i;
        lvColumn.cx = (lnWidth*nColumnWidthPercent1[i]) / 100;
        m_ListNpcAction.InsertColumn(i, &lvColumn);
    }
}

void DlgNpcAction::InitListTalkNote()
{
    RECT rectCtrl;
    LONG lnWidth;
    LV_COLUMN lvColumn;

    m_ListTalkNote.SetExtendedStyle(m_ListTalkNote.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_ListTalkNote.GetClientRect(&rectCtrl);
    //lnWidth = rectCtrl.right - rectCtrl.left;

    const int nColumnCount = 8;	
    char* szColumnName1[nColumnCount] =      { "Talk",  "",  "",  "",  "",  "",  "", ""  };
    int nColumnWidthPercent1[nColumnCount] = {    120,  40, 150, 150, 150, 150, 100, 100 };

    for (int i = 0; i < nColumnCount; ++i)
    {		
        lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.pszText = szColumnName1[i];
        lvColumn.iSubItem = i;
        //lvColumn.cx = (lnWidth*nColumnWidthPercent1[i]) / 100;
        lvColumn.cx = nColumnWidthPercent1[i];
        m_ListTalkNote.InsertColumn(i, &lvColumn);
    }
}

void DlgNpcAction::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    CRect Pos;
    m_ListNpcAction.GetWindowRect(Pos);
    ScreenToClient(&Pos);
    m_ListNpcAction.SetWindowPos(NULL, Pos.top, Pos.left, cx, Pos.bottom-Pos.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
    
    m_ListTalkNote.GetWindowRect(Pos);
    ScreenToClient(&Pos);
    m_ListTalkNote.SetWindowPos(NULL, Pos.top, Pos.left, cx, cy-Pos.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void DlgNpcAction::UpdateList(GLQUEST* pQuest)
{
    m_ListNpcAction.DeleteAllItems();
    m_ListTalkNote.DeleteAllItems();
    m_TalkIndex = 0;

    if (!pQuest)
        return;
    const GLQUEST::VECQSTEP& vecQuestStep = pQuest->GetSTEP();
    for (size_t i=0; i<vecQuestStep.size(); ++i)
    {
        const GLQUEST_STEP& QuestStep = vecQuestStep[i];
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
			m_ListNpcAction.InsertItem(i,CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
			UpdateQuestStep(i, pQuest->m_sNID, QuestStep);
		}
		else
		{
			m_ListNpcAction.InsertItem(i, QuestStep.GetTITLE());
			UpdateQuestStep(i, pQuest->m_sNID, QuestStep);
		}
    }
}

void DlgNpcAction::UpdateQuestStep(
    int ListIndex,
    const SNATIVEID& QuestId,
    const GLQUEST_STEP& QuestStep)
{
    int Column = 1;
    if (QuestStep.m_dwNID_NPCTALK != UINT_MAX)
    {
        SNATIVEID NpcId(QuestStep.m_dwNID_NPCTALK);
        SCROWDATA* pCrow = GLCrowDataMan::GetInstance().GetCrowData(QuestStep.m_dwNID_NPCTALK);
        if (pCrow)
        {
            std::string CrowName(
                sc::string::format(
                    "%1% %2%/%3%", pCrow->GetName(), NpcId.Mid(), NpcId.Sid()));
            m_ListNpcAction.SetItemText(ListIndex, Column++, CrowName.c_str());
            m_ListNpcAction.SetItemText(ListIndex, Column++, pCrow->GetTalkFile());
            UpdateNpcTalk(ListIndex, Column, QuestId, pCrow);
        }
        else
        {            
            std::string ErrorMsg(
                sc::string::format(
                    "%1%/%2% not exist", NpcId.Mid(), NpcId.Sid()));
            m_ListNpcAction.SetItemText(ListIndex, Column++, ErrorMsg.c_str());
            m_ListNpcAction.SetItemText(ListIndex, Column++, "");
        }
    }
}

void DlgNpcAction::UpdateNpcTalk(
    int ListIndex,
    int& Column,
    const SNATIVEID& QuestId,
    SCROWDATA* pCrow)
{
    if (!pCrow)
        return;
    std::tr1::shared_ptr<CNpcDialogueSet> spNpcDlgSet = pCrow->m_spNpcTalkDlg;
    if (!spNpcDlgSet)
        return;

    UpdateNpcDialogue(spNpcDlgSet, QuestId);
    //UpdateNpcTalkVec(spNpcDlgSet, QuestId);
}

void DlgNpcAction::AddTalkNote(
    const std::string& TalkName,
    const std::string& Param1,
    const std::string& Param2,
    const std::string& Param3,
    const std::string& Param4,
    const std::string& Param5,
    const std::string& Param6,
    const std::string& Param7)
{
    m_ListTalkNote.InsertItem(m_TalkIndex, TalkName.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 1, Param1.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 2, Param2.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 3, Param3.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 4, Param4.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 5, Param5.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 6, Param6.c_str());
    m_ListTalkNote.SetItemText(m_TalkIndex, 7, Param7.c_str());
    ++m_TalkIndex;
}

void DlgNpcAction::UpdateNpcDialogue(std::tr1::shared_ptr<CNpcDialogueSet> spNpcDlgSet, const SNATIVEID& QuestId)
{
    const std::string& DlgSetName = spNpcDlgSet->GetDlgSetName();
    const CNpcDialogueSet::DIALOGMAP& DlgMap = spNpcDlgSet->GetDialogueSet();
    for (CNpcDialogueSet::DIALOGMAP_CITER iter=DlgMap.begin(); iter!=DlgMap.end(); ++iter)
    {        
        std::tr1::shared_ptr<CNpcDialogue> spNpcDlg = iter->second;
        if (spNpcDlg)
        {
            DWORD dwIndex = spNpcDlg->FindNonCondition();
            if (dwIndex == UINT_MAX)
                dwIndex = 0;
            UpdateDlgCase(spNpcDlg, QuestId, DlgSetName, dwIndex);
        }
    }
}

void DlgNpcAction::UpdateDlgCase(
    std::tr1::shared_ptr<CNpcDialogue> spNpcDlg,
    const SNATIVEID& QuestId,
    const std::string& DlgSetName,
    DWORD CaseIndex) 
{
    std::string CaseBaseZeroTalk;

    DWORD DlgNID = spNpcDlg->GetDlgNID();        
    const CNpcDialogue::DLGCASE& vecDlgCase = spNpcDlg->GetDlgCaseVec();
    for (size_t i=0; i<vecDlgCase.size(); ++i)
    {
        std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = vecDlgCase[i];
        if (!spDlgCase)
            continue;
        if (i == 0)
            CaseBaseZeroTalk = spDlgCase->GetBasicTalk();
        const std::string& CaseBasicTalk = spDlgCase->GetBasicTalk();

        // Talk Condition
        UpdateTalkCondition(spDlgCase, QuestId, DlgSetName, DlgNID, CaseBaseZeroTalk, CaseBasicTalk);

        // Talk Control
        UpdateTalkControl(spDlgCase, QuestId, DlgSetName, DlgNID, CaseBaseZeroTalk, CaseBasicTalk);
    }
}

void DlgNpcAction::UpdateTalkCondition(
    std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase,
    const SNATIVEID& QuestId,
    const std::string& DlgSetName,
    DWORD DlgNID,
    std::string CaseBaseZeroTalk,
    const std::string& CaseBasicTalk) 
{
    std::tr1::shared_ptr<SNpcTalkCondition> spTalkCondition = spDlgCase->GetCondition();
    if (!spTalkCondition)
        return;
    if (spTalkCondition->ProgressQuestId() == QuestId.Id())
    {
        AddTalkNote(
            DlgSetName,                    
            boost::lexical_cast<std::string> (DlgNID),
            CaseBaseZeroTalk,
            CaseBasicTalk,                    
            "Talk Condition Progress Quest",
            "",
            "",
            "");
    }

    const SNpcTalkCondition::PROCQUEST& vecCompeteQuest = spTalkCondition->GetCompleteQuest();
    for (size_t j=0; j<vecCompeteQuest.size(); ++j)
    {
        if (vecCompeteQuest[j] == QuestId.Id())
        {
            AddTalkNote(
                DlgSetName,                        
                boost::lexical_cast<std::string> (DlgNID),
                CaseBaseZeroTalk,
                CaseBasicTalk,
                "Talk Condition Complete Quest",
                "",
                "",
                "");
        }
    }

    const SNpcTalkCondition::PROCQUEST& vecDisQuest = spTalkCondition->GetDisQuest();
    for (size_t j=0; j<vecDisQuest.size(); ++j)
    {
        if (vecDisQuest[j] == QuestId.Id())
        {
            AddTalkNote(
                DlgSetName,                        
                boost::lexical_cast<std::string> (DlgNID),
                CaseBaseZeroTalk,
                CaseBasicTalk,
                "Talk Condition Not Complete Quest",
                "",
                "",
                "");
        }
    }
}

void DlgNpcAction::UpdateTalkControl(
    std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase,
    const SNATIVEID& QuestId,
    const std::string& DlgSetName,
    DWORD DlgNID,
    std::string CaseBaseZeroTalk,
    const std::string& CaseBasicTalk) 
{
    std::tr1::shared_ptr<CNpcTalkControl> spTalkControl = spDlgCase->GetTalkControl();
    if (!spTalkControl)
        return;

    const CNpcTalkControl::NPCMAP& TalkMap = spTalkControl->GetTalkMapConst();
    for (CNpcTalkControl::NPCMAP_CIT iterTalk=TalkMap.begin(); iterTalk!=TalkMap.end(); ++iterTalk)
    {
        DWORD TalkID = iterTalk->first;
        std::tr1::shared_ptr<SNpcTalk> spNpcTalk = iterTalk->second;
        if (!spNpcTalk)
            continue;
        DWORD TalkId = spNpcTalk->m_dwNID;
        DWORD GlobalTalkId = spNpcTalk->m_dwGLOB_ID;

        if (spNpcTalk->GetAction() == SNpcTalk::EM_QUEST_START)
        {
            for (int j=0; j<MAX_QUEST_START; ++j)
            {
                if (spNpcTalk->QuestStartID(j) == QuestId.Id())
                {
                    std::string Msg(
                        sc::string::format(
                        "Global %1% Talk ID %2% Start Quest %3%",
                        GlobalTalkId,
                        TalkId,
                        spNpcTalk->GetTalkStr()));
                    AddTalkNote(
                        DlgSetName,
                        boost::lexical_cast<std::string> (DlgNID),
                        CaseBaseZeroTalk,
                        CaseBasicTalk,
                        spNpcTalk->GetTalkStr(),
                        Msg,
                        "",
                        "");
                }
            }
        }
        else if (spNpcTalk->GetAction() == SNpcTalk::EM_QUEST_STEP)
        {
            if (spNpcTalk->QuestProgressID() == QuestId.Id())
            {
                std::string Msg(
                    sc::string::format(
                    "Global %1% Talk ID %2% Progress Quest Step %3% %4%",
                    GlobalTalkId,
                    TalkId,
                    spNpcTalk->QuestProgressStepID(),
                    spNpcTalk->GetTalkStr()));
                AddTalkNote(
                    DlgSetName,
                    boost::lexical_cast<std::string> (DlgNID),
                    CaseBaseZeroTalk,
                    CaseBasicTalk,
                    spNpcTalk->GetTalkStr(),
                    Msg,
                    "",
                    "");
            }
        }
        else
        {

        }

        std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spNpcTalk->m_spCondition;
        if (!spCondition)
            continue;
        if (spCondition->ProgressQuestId() == QuestId.Id())
        {
            std::string Msg(
                sc::string::format(
                "Global %1% Talk ID %2% Talk Condition Progress Quest Step %3% %4%",
                GlobalTalkId,
                TalkId,
                spCondition->ProgressQuestStep(),
                spNpcTalk->GetTalkStr()));
            AddTalkNote(
                DlgSetName,
                boost::lexical_cast<std::string> (DlgNID),
                CaseBaseZeroTalk,
                CaseBasicTalk,
                spNpcTalk->GetTalkStr(),
                Msg,
                "",
                "");
        }

        const SNpcTalkCondition::PROCQUEST& vecCompleteQuest = spCondition->GetCompleteQuest();
        for (int k=0; k<vecCompleteQuest.size(); ++k)
        {
            if (vecCompleteQuest[k] == QuestId.Id())
            {
                std::string Msg(
                    sc::string::format(
                    "Global %1% Talk ID %2% Talk Condition Complete Quest %3%",
                    GlobalTalkId,
                    TalkId,
                    spNpcTalk->GetTalkStr()));
                AddTalkNote(
                    DlgSetName,                            
                    boost::lexical_cast<std::string> (DlgNID),
                    CaseBaseZeroTalk,
                    CaseBasicTalk,
                    spNpcTalk->GetTalkStr(),
                    Msg,
                    "",
                    "");
            }
        }

        const SNpcTalkCondition::PROCQUEST& vecDisQuest = spCondition->GetDisQuest();
        for (int l=0; l<vecDisQuest.size(); ++l)
        {
            if (vecDisQuest[l] == QuestId.Id())
            {
                std::string Msg(
                    sc::string::format(
                    "Global %1% Talk ID %2% Talk Condition not have Quest %3%",
                    GlobalTalkId,
                    TalkId,
                    spNpcTalk->GetTalkStr()));
                AddTalkNote(
                    DlgSetName,
                    boost::lexical_cast<std::string> (DlgNID),
                    CaseBaseZeroTalk,
                    CaseBasicTalk,
                    spNpcTalk->GetTalkStr(),
                    Msg,
                    "",
                    "");
            }
        }

    }
}
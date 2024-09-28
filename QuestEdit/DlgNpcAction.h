#pragma once

#include "afxcmn.h"
//#include "../=XtremeToolkitInclude/XTToolkitPro.h"

class GLQUEST;

struct SCROWDATA;
class CNpcDialogueSet;
class CNpcDialogue;
class CNpcDialogueCase;


class DlgNpcAction : public CDialog
{
	DECLARE_DYNAMIC(DlgNpcAction)

public:
	DlgNpcAction(CWnd* pParent = NULL);   
	virtual ~DlgNpcAction();

    int m_TalkIndex;

public:
    void SetQuest(GLQUEST* pQuest);
    void UpdateList(GLQUEST* pQuest);
    void UpdateQuestStep(
        int ListIndex,
        const SNATIVEID& QuestId,
        const GLQUEST_STEP& QuestStep);
    void UpdateNpcTalk(
        int ListIndex,
        int& Column,
        const SNATIVEID& QuestId,
        SCROWDATA* pCrow);

    void UpdateNpcDialogue(
        std::tr1::shared_ptr<CNpcDialogueSet> spNpcDlgSet,
        const SNATIVEID& QuestId);

    void UpdateDlgCase(
        std::tr1::shared_ptr<CNpcDialogue> spNpcDlg,
        const SNATIVEID& QuestId,
        const std::string& DlgSetName,
        DWORD CaseIndex);

    void UpdateTalkControl(
        std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase,
        const SNATIVEID& QuestId,
        const std::string& DlgSetName,
        DWORD DlgNID,
        std::string CaseBaseZeroTalk,
        const std::string& CaseBasicTalk); 
    
    void UpdateTalkCondition(
        std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase,
        const SNATIVEID& QuestId,
        const std::string& DlgSetName,
        DWORD DlgNID,
        std::string CaseBaseZeroTalk, 
        const std::string& CaseBasicTalk);

    void InitListNpcAction();
    void InitListTalkNote();
    void AddTalkNote(
        const std::string& TalkName,
        const std::string& Param1,
        const std::string& Param2,
        const std::string& Param3,
        const std::string& Param4,
        const std::string& Param5,
        const std::string& Param6,
        const std::string& Param7);

	enum { IDD = IDD_DIALOG_NPC_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CListCtrl m_ListNpcAction;
    CString m_StaticNpcAction1;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CListCtrl m_ListTalkNote;
};

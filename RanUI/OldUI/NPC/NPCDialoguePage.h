#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/NpcActionable.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CBasicTextBoxEx;
class CBasicLineBox;
class CBasicScrollBarEx;

const int NUM_TALK_BUTTON = 6;

class CNPCDialoguePage : public CUIGroup, private CUIControlNpcActionable
{
private:
    static const int nSTARTINDEX;
    static const int nSTARTLINE;
    static const float fANSWER_PART_INTERVAL;

private:
    enum
    {
        RANDOM_TIME_BUTTON = NO_ID + 1,
        RANDOM_TIME_TEXT = NO_ID + 2,

        DIALOGUE_TALK_TEXT = NO_ID + 3,
        DIALOGUE_TALK_SCROLL = NO_ID + 4,

        DIUALOGUE_TALK_BUTTON_0 = NO_ID + 5,
        DIUALOGUE_TALK_BUTTON_1 = NO_ID + 6,
        DIUALOGUE_TALK_BUTTON_2 = NO_ID + 7,
        DIUALOGUE_TALK_BUTTON_3 = NO_ID + 8,
        DIUALOGUE_TALK_BUTTON_4 = NO_ID + 9,
        DIUALOGUE_TALK_BUTTON_5 = NO_ID + 10,
        DIUALOGUE_TALK_BUTTON_TEXT_0 = NO_ID + 11,
        DIUALOGUE_TALK_BUTTON_TEXT_1 = NO_ID + 12,
        DIUALOGUE_TALK_BUTTON_TEXT_2 = NO_ID + 13,
        DIUALOGUE_TALK_BUTTON_TEXT_3 = NO_ID + 14,
        DIUALOGUE_TALK_BUTTON_TEXT_4 = NO_ID + 15,
        DIUALOGUE_TALK_BUTTON_TEXT_5 = NO_ID + 16,
        DIUALOGUE_TALK_BUTTON_SCROLL_BAR = NO_ID + 17,
    };

    enum PRIORITY_TALK_ORDER
    {
        TALK_FIRST_TIME = 1,
        TALK_QUEST_STEP = 2,
        TALK_QUEST_START = 3,
        TALK_QUEST_ING = 4,
        TALK_BASIC_TALK = 5,
    };

public:
    CNPCDialoguePage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CNPCDialoguePage();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    GLCHARLOGIC* m_pCharLogic;

    std::tr1::shared_ptr<CNpcDialogueSet> m_spDialogueSet;
    std::tr1::shared_ptr<CNpcDialogue> m_spDialogue;
    std::tr1::shared_ptr<CNpcDialogueCase> m_spDialogueCase;

    bool m_bButtonRandom;
    float m_fRandomTime;		// 흐른시간
    DWORD m_dwRandomTime;		// 제한시간

    int m_nTalkButtonTotalLine;
    int m_nTalkButtonStartIndex;
	int m_nCaseIdx;

private:
    CBasicTextButton* m_pRandomTimeButton;
    CBasicTextBox* m_pRandomTimeText;
    CBasicLineBox* m_pRandomTimeBack;

    CBasicLineBoxSmart* m_pTalkBack;
    CBasicTextBox* m_pTalkText;
    CBasicScrollBarEx* m_pTalkScrollBar;

    CBasicLineBoxSmart* m_pTalkButton[ NUM_TALK_BUTTON ];
    CBasicTextBoxEx* m_pTalkButtonText[ NUM_TALK_BUTTON ];
    CBasicScrollBarEx* m_pTalkButtonScrollBar;

public:
    void CreateSubControl();

    bool SetDialogueData( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC* pCharLogic );

private:
    void LoadNode( DWORD dwID, bool bRandom = false, DWORD dwRandomTime = 0 );
    void LoadBasicTalk(const DWORD dwIndex = 0 );
    void LoadButtonTalk( bool bRandom = false, const DWORD dwIndex = 0 );

    void TranslateTalkMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk );
    void TranslateBasicMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk );
    void TranslateQuestStartMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk );
    void TranslateQuestStepMessage( std::tr1::shared_ptr<SNpcTalk> spNpcTalk );

    bool CheckQuestProc( std::tr1::shared_ptr<SNpcTalk> spNpcTalk, DWORD dwPartyMemberNum );
};
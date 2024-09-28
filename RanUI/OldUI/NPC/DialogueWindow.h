#pragma	once

#include <boost/tr1/memory.hpp>
#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/NpcTalk/NpcDialogueSet.h"

class	CInnerInterface;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
struct	SNpcTalk;
class	CNpcDialogueCase;
class	CBasicTextButton;
class	CBasicLineBox;
class GLGaeaClient;

class CDialogueWindow 
    : public  CUIWindowEx
    , private CUIControlNpcActionable
{
private:
static	const	int		nSTARTINDEX;
static	const	int		nSTARTLINE;
static	const	float	fANSWER_PART_INTERVAL;

private:
	enum
	{
		DIALOGUE_QUESTION_TEXTBOX = ET_CONTROL_NEXT,
		BASIC_DIALOGUE_SCROLLBAR,
		DIALOGUE_ANSWER_TEXTBOX,
		BASIC_DIALOGUE_ANSWER_SCROLLBAR,
		RANDOM_TIME_BUTTON,
		RANDOM_TIME_TEXT
	};

public:
	CDialogueWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CDialogueWindow ();
	
	float	m_fRandomTime;		// 흐른시간
	DWORD	m_dwRandomTime;		// 제한시간

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	BOOL	SetDialogueData ( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID, GLCHARLOGIC *pCHAR );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    SNATIVEID    GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD        GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

private:
    void TranslateTalkMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk);
	void TranslateBasicMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk);
	void TranslateQuestStartMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk);
	void TranslateQuestStepMessage(std::tr1::shared_ptr<SNpcTalk> spNpcTalk);

private:
	void LoadNode(DWORD NativeID, bool bRandom = false, DWORD dwRandomTime = 0 );
	void LoadBasicTalk ();
	void LoadButtonTalk ( bool bRandom = false );	

private:
	CBasicTextBox*		m_pQuestionBox;
	CBasicScrollBarEx*	m_pScrollBar;
	CBasicTextBoxEx*	m_pAnswerBox;
	CBasicScrollBarEx*	m_pAnswerScrollBar;
	CBasicTextButton*	m_pRandomTimeButton;
	CBasicTextBox*		m_pRandomTimeText;
	CBasicLineBox*		m_pRandomTimeBack;

private:
    std::tr1::shared_ptr<CNpcDialogueSet>  m_spDialogueSet;
	std::tr1::shared_ptr<CNpcDialogue>     m_spDialogue;
	std::tr1::shared_ptr<CNpcDialogueCase> m_spDialogueCase;

private:
	GLCHARLOGIC*	m_pCHAR;

};
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;
class CBasicTextBoxEx;
class CBasicButton;

const float fDISPLAY_INTERVAL = 5.0f;
const float fDISPLAY_SHOW_INTERVAL = 2.0f;

class CQuestHelper : public CUIGroup
{
private:
    enum
    {
        QUEST_HELPER_TEXT_0 = NO_ID + 1,
        QUEST_HELPER_TEXT_1,
        QUEST_HELPER_TEXT_2,
        QUEST_HELPER_TEXT_3,
        QUEST_HELPER_TEXT_4,
        QUEST_HELPER_SHOW_DETAIL_0,
        QUEST_HELPER_SHOW_DETAIL_1,
        QUEST_HELPER_SHOW_DETAIL_2,
        QUEST_HELPER_SHOW_DETAIL_3,
        QUEST_HELPER_SHOW_DETAIL_4,
    };

private:
    GLGaeaClient*       m_pGaeaClient;

    DWORD*			    m_pQuestIDArray;
    CBasicTextBoxEx*    m_pQuestText    [ RANPARAM::MAX_QUEST_DISPLAY ];
    CBasicButton*	    m_pShowDetail   [ RANPARAM::MAX_QUEST_DISPLAY ];
    DWORD			    m_dwDetailID    [ RANPARAM::MAX_QUEST_DISPLAY ];

    int				    m_nIndex;

    int				    m_nHelperNum;

public:
	CQuestHelper(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CQuestHelper();

	void	CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );

public:	
	bool	SetQuestID( DWORD dwQuestID, bool bPrintMsg = TRUE );
	void	ReSetQuestID( DWORD dwQuestID );

	void	UpdateQuestInfo();

	bool	IsQuestPrint();
	bool	IsSameQuest( DWORD dwQuestID );

    int		GetHelperNumber();
    int		GetMaxHelperNumber()	{ return RANPARAM::MAX_QUEST_DISPLAY; }

protected:
	CInnerInterface* m_pInterface;

};

class MyQuestHelper : public IQuestHelper, private CQuestHelper
{
public:
    MyQuestHelper( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyQuestHelper()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual bool SetQuestID( DWORD dwQuestID, bool bPrintMsg = TRUE );
    virtual void ReSetQuestID( DWORD dwQuestID );
    virtual bool IsSameQuest( DWORD dwQuestID );
    virtual int GetHelperCount();
    virtual int GetHelperMaxCount();
    virtual void UpdateQuestInfo();
};

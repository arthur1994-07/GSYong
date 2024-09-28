#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
//#include "SkillTrayTab.h"
#include "../../InnerInterface.h"

const INT MAX_TAB_INDEX = 4;
const INT QUICK_SKILL_SLOT_MAX = 10;

class CInnerInterface;
class CBasicSkillTray;
class CBasicQuickSkillSlot;
class CUITextButton;
class CBasicButton;
class GLGaeaClient;

class CSkillTrayTab : public CUIGroup
{
    static INT NEXT_INDEX[MAX_TAB_INDEX];

public:
    enum
    {
        BASIC_QUICK_SKILL_SLOT    = NO_ID + 1,

        BASIC_QUICK_SKILL_TRAY_F1,
        BASIC_QUICK_SKILL_TRAY_F2,
        BASIC_QUICK_SKILL_TRAY_F3,
        BASIC_QUICK_SKILL_TRAY_F4,

        BASIC_TEXT_BUTTON_F1,
        BASIC_TEXT_BUTTON_F2,
        BASIC_TEXT_BUTTON_F3,
        BASIC_TEXT_BUTTON_F4,

        QUICK_SKILL_TRAY_TURN_BUTTON,
        QUICK_SKILL_TRAY_OPEN_BUTTON,
        QUICK_SKILL_TRAY_CLOSE_BUTTON,

        QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN,
        QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE,
        QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_OPEN_H,
        QUICK_SKILL_TRAY_MOVE_LOCK_BUTTON_CLOSE_H,
    };

    enum
    {
        TABBUTTON_FOLDGROUP_ID = 1
    };

public:
    static const FLOAT fTURN_RECT_GAP_DISTANCE;
    static const FLOAT fTURN_RECT_SIZE_RATIO;
    static const FLOAT fTURN_RECT_SIZE_BORDER;

public:
    void CreateSubControl();

    void SetTabIndex( INT nIndex );
    INT GetTabIndex()						{ return m_nTabIndex; }

    const UIRECT& GetCurrentSkillSlotGlobalPos();
	const WORD	  FindSkillSlot( const SNATIVEID& sSkillID );
	const UIRECT& FindSkillSlotGlobalPos( const SNATIVEID& sSkillID );

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual	void SetVisibleSingle( BOOL bVisible );
    void	SetShotcutText ( DWORD nID, CString& strTemp );

public:
    virtual HRESULT InfoFileLoad ();
    virtual HRESULT InfoFileSave ();

public:
    virtual void TurnHorizontal ();
    virtual void TurnVertical   ();

public:
    void    TrayOpen  ();
    void    TrayClose ();

    void SetMoveLock( bool bMoveLock );
    bool IsMoveLock();

public:
	void	   ResetSlotText();
    const BOOL IsTrayClose() { return m_bClose; }
    const BOOL CollisionCheck  ( const int x, const int y, const UIRECT& rcDest );

public:
    CSkillTrayTab(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CSkillTrayTab(void);

protected:
    GLGaeaClient* m_pGaeaClient;
    CBasicQuickSkillSlot* m_pCurrentSkillSlot;
    CBasicSkillTray * m_pSkillTray[MAX_TAB_INDEX];
    CUITextButton* m_pTextButton[MAX_TAB_INDEX];
    CBasicButton* m_pTurnButton;
    CBasicButton* m_pOpenButton;
    CBasicButton* m_pCloseButton;

    CBasicButton* m_pLockButtonOpenUnlock;
    CBasicButton* m_pLockButtonCloseUnlock;
    CBasicButton* m_pLockButtonOpenUnlockH;
    CBasicButton* m_pLockButtonCloseUnlockH;
    CBasicButton* m_pLockButtonOpenLock;
    CBasicButton* m_pLockButtonCloseLock;
    CBasicButton* m_pLockButtonOpenLockH;
    CBasicButton* m_pLockButtonCloseLockH;

    CUIControl* m_pGap0;
    CUIControl* m_pGap1;
    CUIControl* m_pGap2;

	CUIControl* m_pQuickSkillTrayMoveLockButtonOpen;
	CUIControl* m_pQuickSkillTrayMoveLockButtonOpenH;
	CUIControl* m_pQuickSkillTrayMoveLockButtonClose;
	CUIControl* m_pQuickSkillTrayMoveLockButtonCloseH;

    BOOL m_bClose;
    BOOL m_bDrag;
    BOOL m_bTurnHorizontal;
    INT m_nTabIndex;
    INT m_nCount;
    bool m_bMoveLock;

protected:
    D3DXVECTOR2 m_vPos;
    UIRECT      m_rcBackPos;
    UIRECT      m_rcBackBasic;

protected:
    CInnerInterface* m_pInterface;

};

class MySkillTrayTab : public ISkillTrayTab, private CSkillTrayTab
{
public:
    MySkillTrayTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MySkillTrayTab()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual const UIRECT& GetUIWindowGlobalPos ();
	virtual const UIRECT& GetCurrentSkillSlotGlobalPos();
	virtual const WORD	  FindSkillSlot( const SNATIVEID& sSkillID );
	virtual const UIRECT& FindSkillSlotGlobalPos( const SNATIVEID& sSkillID );

    virtual void SetShotcutText( DWORD nID, UI::String& strTemp );
    virtual INT GetTabIndex();
    virtual void SetTabIndex( INT nIndex );
	virtual void SelectSlot( INT nIndex );
	virtual void ResetSlotText();
    virtual void TrayToggle ();
    virtual void TrayOpen ();
	virtual BOOL IsMoveLock();
};
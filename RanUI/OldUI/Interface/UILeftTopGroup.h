//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.21]
//			@ 작성
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"
#include "BasicLevelDisplay.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSE_IN = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CInnerInterface;
class	CBasicPotionTray;
class	CBasicQuickPotionSlot;
class	CBasicButton;
class GLGaeaClient;

class CUILeftTopGroup : public CUIGroup
{
private:
	enum
	{
		BASIC_QUICK_POTION_SLOT = NO_ID + 1,
		QUICK_POTION_TRAY_OPEN_BUTTON,
		BASIC_QUICK_POTION_TRAY,
		BASIC_LEVEL_DISPLAY
	};

public:
	CUILeftTopGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CUILeftTopGroup();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void	SetShotcutText( DWORD nID, CString& strTemp );
	void SetQuickPotionSlotIndex(int nIndex);
	CBasicPotionTray*	GetBasicPotionTray() { return m_pPotionTray; }

public:
	void ResetQuickSlotText();

private:
	CBasicButton*		m_pPotionTrayOpenButton;
	CBasicPotionTray*	m_pPotionTray;
	CBasicQuickPotionSlot*	m_pBasicQuickSlot;
	CBasicLevelDisplay* m_pLevelDisplay;

protected:
	CInnerInterface* m_pInterface;

};

class MyUILeftTopGroup : public IUILeftTopGroup, private CUILeftTopGroup
{
public:
    MyUILeftTopGroup( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyUILeftTopGroup() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetShotcutText( DWORD nID, UI::String& strTemp );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual CBasicPotionTray* GetBasicPotionTray();
	virtual void SetQuickPotionSlotIndex(int nIndex);
	virtual void ResetQuickSlotText();
};
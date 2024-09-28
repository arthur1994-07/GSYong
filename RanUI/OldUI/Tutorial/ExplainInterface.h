#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CTipBox;
class CBasicTextBox;
class CQuestList;
class CBasicTextBoxEx;
class GLGaeaClient;

const float SPEED_OF_UI_BLINKING = 0.5f;

class CExplainInterface : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
        SLOTBOX_SIZE = 2,
	};

public:
	CExplainInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CExplainInterface(void);

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    float			m_fFlowTime;
    int				m_dwCount;
    UIRECT			m_SlotBoxOrigin_rc[SLOTBOX_SIZE];
    D3DXVECTOR2		m_vSlotBoxOrigin[SLOTBOX_SIZE];
    BOOL			m_bSlotBoxVisibleAtInven;
    BOOL			m_bSlotBoxVisible[SLOTBOX_SIZE];

    CTipBox*		m_pSkillTrayTab;
    CTipBox*		m_pBasicInfo;
    CTipBox*		m_pMiniMap;
    CTipBox*		m_pChatBox;
    CTipBox*		m_pQuickPostion;
    CTipBox*		m_pGameMenu;
    CTipBox*		m_pInventoryWindow;
    CTipBox*		m_pSkillWindow;
    CUIControl*		m_pLeftDir;
    CUIControl*		m_pRightDir;
    CUIControl*		m_pUpDir;
    CUIControl*		m_pDownDir;
    CTipBox*		m_pSlotBox;
    CTipBox*		m_pQuestList;
    CTipBox*		m_pQuestMain;
    CTipBox*		m_pQuestStep;
    CTipBox*		m_pQuestMoreButton;

public:
	void CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

	CTipBox* GetQuickPosion() { return m_pQuickPostion; }
	CTipBox* GetSlotBox() { return m_pSlotBox; }

	void SetSlotBoxOriginPos( D3DXVECTOR2 vPos, int nIndex );
	void SetSlotBoxOriginPos( UIRECT rcWindow, int nIndex );

    BOOL IsSlotBoxVisibleAtInven() const { return m_bSlotBoxVisibleAtInven; }
    void SetSlotBoxVisibleAtInven(BOOL State) { m_bSlotBoxVisibleAtInven=State; }
    BOOL IsSlotBoxVisible(unsigned int Index) const;
    void SetSlotBoxVisible(unsigned int Index, BOOL Visible);
    void SetSlotBoxVisibleSingle( BOOL bVisible );
    const UIRECT& GetSlotBoxGlobalPos();
};

class CTutorialDamageAlarm : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialDamageAlarm(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialDamageAlarm(void);

public:
	void	CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;

private:
	CBasicTextBox*	m_pText;

protected:
	CInnerInterface* m_pInterface;

};

class MyExplainInterface : public IExplainInterface, private CExplainInterface
{
public:
    MyExplainInterface( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyExplainInterface()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual BOOL IsSlotBoxVisible( unsigned int Index ) const;
    virtual BOOL IsSlotBoxVisibleAtInven() const;
    virtual void SetSlotBoxVisible( unsigned int Index, BOOL Visible );
    virtual void SetSlotBoxVisibleAtInven( BOOL State );
    virtual CTipBox* GetSlotBox();
    virtual void SetSlotBoxOriginPos( D3DXVECTOR2 vPos, int nIndex );
    virtual void SetSlotBoxOriginPos( UIRECT rcWindow, int nIndex );
    virtual void SetSlotBoxVisibleSingle( BOOL bVisible );
    virtual const UIRECT& GetSlotBoxGlobalPos();
};
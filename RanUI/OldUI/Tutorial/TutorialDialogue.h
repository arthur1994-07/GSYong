#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CTipBox;
class CBasicTextBox;
class CBasicButton;
class CTutorialNextButton;
class CBasicVarTextBox;
class GLGaeaClient;

class CTutorialDialogue : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
		TUTORIAL_NEXT_BUTTON,
	};

public:
	CTutorialDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialDialogue(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetEnalbe( bool bEnableWork ) { m_bEnableWork = bEnableWork; }
	bool GetEnalbe() { return m_bEnableWork; }
	void SetDialogueText( const CString& strText );

    void SetTutorialDialogueIndex( DWORD Index );
    void TutorialDialogueTextStepInc();
    DWORD GetTutorialDialogueIndex();
    void SetTutorialDialogueSoundIndex( DWORD Index );
    DWORD GetTutorialDialogueSoundIndex();
    void TutorialDialogueSoundIndexInc();

private:
	float			m_fFlowTime;
	bool			m_bEnableWork;
	bool			m_bEnableClick;	// 튜토리얼 다음 대화문 진행을 위한 변수.
	float			m_fClickTime;	// 튜토리얼 다음 대화문 진행을 위한 변수.

	DWORD			m_dwIndex;
	DWORD			m_dwTextStep;
	DWORD			m_dwSoundIndex;

private:
	CTipBox*		m_pMain;
	CUIControl*		m_pMouse;
	CUIControl*		m_pMouseButton;
	CUIControl*		m_pTeacher;
	CBasicTextBox*	m_pExplainText;
	CBasicTextBox*	m_pName;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialDialogueExplainText : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialDialogueExplainText(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialDialogueExplainText(void);

public:
	void	CreateSubControl ();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetExplainText(const CString& strText);

private:
	float			m_fFlowTime;

private:
	CTipBox*		m_pMain;
	CBasicTextBox*	m_pExplainText;

protected:
	CInnerInterface* m_pInterface;

};

class MyTutorialDialogue : public ITutorialDialogue, private CTutorialDialogue
{
public:
    MyTutorialDialogue( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyTutorialDialogue()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetDialogueText( const UI::String& strText );
    virtual void SetEnalbe( bool bEnableWork );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual void SetUIWindowLocalPos( const D3DXVECTOR2& vPos );
    virtual void SetTutorialDialogueIndex( DWORD Index );
    virtual DWORD GetTutorialDialogueIndex();
    virtual void TutorialDialogueTextStepInc();
    virtual void SetTutorialDialogueSoundIndex( DWORD Index );
    virtual DWORD GetTutorialDialogueSoundIndex();
    virtual void TutorialDialogueSoundIndexInc();
};
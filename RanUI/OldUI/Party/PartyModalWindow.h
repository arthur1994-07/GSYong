//	UIModal 클래스 - 모달 기반 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.04] - 이 부분, 리팩토링이 끝나면, RanClientLib로 내려간다.
//					 모달은 컨트롤과 메시지가 함께 붙어 있는것이 맞다.
//		[2003.10.31] - 시작
//					 enginelib을 RanClientLib와 분리

#pragma	once

#include "../../../EngineLib/GUInterface/UIWindow.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class CBasicComboBox;
class CBasicComboBoxRollOver;
class CBasicButton;
class GLGaeaClient;

class CPartyModalWindow : public CUIWindow
{
protected:
	enum
	{
		MODAL_PARTY_OK = ET_CONTROL_NEXT,
		MODAL_PARTY_CANCEL,
		MODAL_PARTY_YES,
		MODAL_PARTY_NO,		
		PARTY_MODAL_ITEM_COMBO_OPEN,
		PARTY_MODAL_ITEM_COMBO_ROLLOVER,
		PARTY_MODAL_MONEY_COMBO_OPEN,		
		PARTY_MODAL_MONEY_COMBO_ROLLOVER,
	};

public:
	CPartyModalWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPartyModalWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateBaseModal( char* szWidowKeyword );
	void	CreateSubControl();

protected:
	CUIControl*		CreateControl( char* szControl );
	CBasicTextButton*	CreateTextButton( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetCallerID( UIGUID ControlID )			{ m_CallerID = ControlID; }
	UIGUID	GetCallerID()								{ return m_CallerID; }

	virtual void	SetPartyMode( bool bExpedition )	{ m_bExpedition = bExpedition; }
	virtual void SetPartyOption(const SPartyOption& PartyOption) { m_ReqPartyOption = PartyOption; }

private:
	CBasicTextBox*	CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
	CBasicButton*	CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID );

public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	void	LoadComboData();

public:
	void	LoadDefaultCondition();

	void	LoadItem();	
	void	LoadMoney();

public:
	void	DoModal( CString strText, UIGUID CallerID );

public:
    void    SetCharDbNum( DWORD dwCharDbNum );
    DWORD   GetCharDbNum();

protected:
	void	ResizeControl( char* szWindowKeyword );

private:
    UIGUID	m_CallerID;
	BOOL		m_bReqModal;
	BOOL		m_bFIX;
	bool		m_bExpedition;
	SPartyOption		m_ReqPartyOption;
    DWORD               m_dwCharDbNum;

private:
	CBasicTextBox*	m_pTextBox;

private:
	CBasicTextButton*	m_pOK;
	CBasicTextButton*	m_pCANCEL;
	CBasicTextButton*	m_pYES;
	CBasicTextButton*	m_pNO;

	CBasicComboBox*			m_pComboBoxItemOpen;
	CBasicComboBoxRollOver*	m_pComboBoxItemRollOver;

	CBasicComboBox*			m_pComboBoxMoneyOpen;	
	CBasicComboBoxRollOver*	m_pComboBoxMoneyRollOver;

private:
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

protected:
	CInnerInterface* m_pInterface;

};

class MyPartyModalWindow : public IPartyModalWindow, private CPartyModalWindow
{
public:
    MyPartyModalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPartyModalWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void DoModal( UI::String strText, UIGUID CallerID );
	virtual void SetPartyMode( bool bExpedition );
	virtual void SetPartyOption(const SPartyOption& PartyOption);
    virtual UIGUID	GetCallerID();

    virtual void SetCharDbNum( DWORD dwCharDbNum );
    virtual DWORD GetCharDbNum();

};
#pragma once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CTipBox;
class CBasicTextBox;
class GLGaeaClient;

class CTutorialInterfaceLeft : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceLeft(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceLeft(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyLeft;
	CUIControl* m_pFullLeft;
	CUIControl* m_pDoneLeft;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialInterfaceRight : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceRight(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceRight(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyRight;
	CUIControl* m_pFullRight;
	CUIControl* m_pDoneRight;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialInterfaceUp : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceUp(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceUp(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyUp;
	CUIControl* m_pFullUp;
	CUIControl* m_pDoneUp;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialInterfaceDown : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceDown(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceDown(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyDown;
	CUIControl* m_pFullDown;
	CUIControl* m_pDoneDown;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialInterfaceZoomIn : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceZoomIn(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceZoomIn(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyZoomIn;
	CUIControl* m_pFullZoomIn;
	CUIControl* m_pDoneZoomIn;

protected:
	CInnerInterface* m_pInterface;

};

class CTutorialInterfaceZoomOut : public CUIGroup
{
	enum
	{
		TUTORIAL_TIP_BOX = NO_ID+1,
	};

public:
	CTutorialInterfaceZoomOut(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CTutorialInterfaceZoomOut(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	float			m_fFlowTime;
	UIRECT			m_rcOriginRect;
	UIRECT			m_rcOriginTextureRect;

private:
	CUIControl* m_pEmptyZoomOut;
	CUIControl* m_pFullZoomOut;
	CUIControl* m_pDoneZoomOut;

protected:
	CInnerInterface* m_pInterface;

};
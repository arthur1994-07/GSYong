#pragma	once

#include "../Util/UIWindowEx.h"

class	CInnerInterface;
class	CItemBankPage;
class	CBasicTextBox;
class	CBasicTextButton;
struct	SINVENITEM;
class GLGaeaClient;
class	CPrivateMarketPage;

class	CPrivateMarketWindow : public CUIWindowEx
{
protected:
	enum
	{
		PRIVATE_MARKET_PAGE = ET_CONTROL_NEXT,
		PRIVATE_MARKET_START,
		PRIVATE_MARKET_STOP
	};

public:
	CPrivateMarketWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPrivateMarketWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual	HRESULT	RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

public:
	const bool IsOPENER ()									{ return m_bOPENER; }

	void	InitPrivateMarket ( const bool& bOPENER, const DWORD& dwGaeaID );
	DWORD	GetPrivateMarketID ()							{ return m_dwGaeaID; }

	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );	

	const SINVENITEM* GetItem(int nPosX, int nPosY) const;

private:
	CPrivateMarketPage*	m_pPage;
	CBasicTextButton*	m_pSTART;
	CBasicTextButton*	m_pSTOP;
	CBasicTextBox*		m_pTextBox;

private:
	bool	m_bOPENER;
	DWORD	m_dwGaeaID;

private:
	int		m_nPosX;
	int		m_nPosY;

};
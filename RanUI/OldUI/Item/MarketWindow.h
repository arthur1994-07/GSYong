//	마켓 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Crow/GLCrowData.h"
#include "../../InnerInterface.h"
#include "../Util/NpcActionable.h"

class CInnerInterface;
class CMarketPage;
class CBasicTextButton;
struct SINVENITEM;
class GLGaeaClient;

class CMarketWindow : 
    public  CUIWindowEx, 
    private CUIControlNpcActionable
{
private:
static	const	int		nOUTOFRANGE;
protected:
	enum
	{
		nMAXPAGE = SCROWACTION::SALENUM
	};

	enum
	{
		MARKET_PAGE0 = ET_CONTROL_NEXT,
		MARKET_PAGE1,
		MARKET_PAGE2,
		MARKET_PAGE_END = MARKET_PAGE0 + nMAXPAGE,
		MARKET_WINDOW_PAGE_BUTTON0,
		MARKET_WINDOW_PAGE_BUTTON1,
		MARKET_WINDOW_PAGE_BUTTON2,
		MARKET_WINDOW_PAGE_BUTTON_END = MARKET_WINDOW_PAGE_BUTTON0 + nMAXPAGE,
	};

public:
	CMarketWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CMarketWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();
	void	SetVisiblePage( int nPage );	

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void		InitMarket( SNATIVEID sNPCID, DWORD dwGlobID );

public:
    SNATIVEID	GetMarketID();
    SNATIVEID   GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD       GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

	void	SetItemIndex( int nPosX, int nPosY );
	void	GetItemIndex( int* pnPosX, int* pnPosY );	

	int		GetPageIndex();

	const SINVENITEM* GetItem(int nPosX, int nPosY) const;

	BOOL	IsUsingMaketPage( int nPage );

private:
	int		m_nPosX;
	int		m_nPosY;

	int		m_nPageIndex;

protected:
	CMarketPage*	CreateMarketPage( UIGUID ControlID );
	CBasicTextButton*	CreateTextButton( const char* szButton, UIGUID ControlID, const char* szText );

private:
	CMarketPage*		m_pPage[nMAXPAGE];
	CBasicTextButton*	m_pPageButton[nMAXPAGE];

};

class MyMarketWindow : public IMarketWindow, private CMarketWindow
{
public:
    MyMarketWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyMarketWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void InitMarket( SNATIVEID sNPCID, DWORD dwNpcGlobID );
    virtual void GetItemIndex( int* pnPosX, int* pnPosY );
    virtual SNATIVEID GetNPCID();
    virtual DWORD GetNPCGlobalID();
    virtual const SINVENITEM* GetItem( int nPosX, int nPosY ) const;
    virtual SNATIVEID GetMarketID();
    virtual int GetPageIndex();
};

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
#include "../../InnerInterface.h"

class CInnerInterface;
class CTradeControlPage;
class GLGaeaClient;

class CTradeWindow : public CUIWindowEx
{
private:
	enum
	{
		TAR_TRADE_PAGE = ET_CONTROL_NEXT,
		MY_TRADE_PAGE
	};

public:
	CTradeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CTradeWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SetTradeInfo( CString strPlayerName, DWORD dwMasterID );
	DWORD	GetMasterID();

private:
	CTradeControlPage*	CreateTradeControlPage( char *szControl, BOOL bMyPage, UIGUID ControlID );

private:
	DWORD	m_dwMasterID;
	std::string m_strPlayerName;

private:
	CTradeControlPage*	m_pTarPage;
	CTradeControlPage*	m_pMyPgae;

};

class MyTradeWindow : public ITradeWindow, private CTradeWindow
{
public:
    MyTradeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyTradeWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetTradeInfo( UI::String strPlayerName, DWORD dwMasterID );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
    virtual DWORD GetMasterID();
};
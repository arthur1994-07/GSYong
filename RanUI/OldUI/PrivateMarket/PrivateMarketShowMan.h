#pragma	once

#include <boost/pool/object_pool.hpp>
#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../../SigmaCore/Memory/CMemPool.h"
#include "../../InnerInterface.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_MARKET_ADVERTISE_LBDNUP = UIMSG_USER5;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CPrivateMarketShow;
class GLGaeaClient;

class CPrivateMarketShowMan : public CUIGroup
{
private:
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 5000,
	};

public:	
	typedef std::map<DWORD,CPrivateMarketShow*>	PRIVATE_MARKET_SHOW_MAP;
	typedef PRIVATE_MARKET_SHOW_MAP::iterator	PRIVATE_MARKET_SHOW_MAP_ITER;

    //typedef sc::CMemPool<CPrivateMarketShow>		PRIVATE_MARKET_POOL;
	typedef sc::BOOST_OBJECT_POOL_CHECKER<CPrivateMarketShow> PRIVATE_MARKET_POOL;

public:
	CPrivateMarketShowMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPrivateMarketShowMan();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void RemoveAll();

public:
	void	ADD_MARKET( const CROWREN &sDISP );
	void	DEL_MARKET( DWORD dwGaeaID );
	void	UPDATE_MARKET( const CROWREN &sDISP );

	void	UPDATE_MARKET_POS( CPrivateMarketShow* pVarTextBox, const CROWREN& sDISP );
	bool	IS_VISIBLE_NAME( const CROWREN& sDISP, D3DXVECTOR3 & vScreen );

	CPrivateMarketShow*	MAKE_PRIVATEMARKET();

	// MEMO : 재정의
	bool DeleteControl( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	UIGUID	m_CONTROL_NEWID;
	bool	m_bFIRST_TOUCH;

	PRIVATE_MARKET_SHOW_MAP	m_mapPrivateMarketShow;
	PRIVATE_MARKET_POOL		m_PrivateMarketPool;
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

protected:
	CInnerInterface* m_pInterface;

};

class MyPrivateMarketShowMan : public IPrivateMarketShowMan, private CPrivateMarketShowMan
{
public:
    MyPrivateMarketShowMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPrivateMarketShowMan()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void ADD_MARKET( const CROWREN &sDISP );
    virtual void DEL_MARKET( DWORD dwGaeaID );
    virtual void UPDATE_MARKET( const CROWREN &sDISP );
    virtual void RemoveAll();
};
#pragma	once

#include <boost/pool/object_pool.hpp>
#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../../SigmaCore/Memory/CMemPool.h"

class CItemShopIcon;
class GLGaeaClient;

class CItemShopIconMan : public CUIGroup
{
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 1000,
	};

	typedef std::map<DWORD,CItemShopIcon*>	  SHOPICON_MAP;
	typedef SHOPICON_MAP::iterator			  SHOPICON_MAP_ITER;

    //typedef sc::CMemPool<CItemShopIcon>		  SHOPICON_POOL;
    typedef sc::BOOST_OBJECT_POOL_CHECKER<CItemShopIcon> SHOPICON_POOL;

private:

	UIGUID			m_CONTROL_NEWID;
	SHOPICON_MAP	m_mapIcon;
	SHOPICON_POOL	m_IconPool;	
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	
public:
	CItemShopIconMan(GLGaeaClient* pGaeaClient, LPDIRECT3DDEVICEQ pd3dDevice, EngineDeviceMan* pEngineDevice);
	virtual	~CItemShopIconMan ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:

	// MEMO : 재정의
	virtual bool DeleteControl(UIGUID ControlID, int refactoring) override; // 컨트롤을 삭제한다.

private:
	virtual	HRESULT	InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void RemoveAll();

private:
	CItemShopIcon*	MAKE_SHOPICON ();

private:
	void	UPDATE_POS ( CItemShopIcon* pShopIcon, D3DXVECTOR3 vPos );
	bool	IS_VISIBLE_NAME ( D3DXVECTOR3 vPos, D3DXVECTOR3 & vScreen );

public:
	void	ADD_SHOP_ICON ( DWORD dwGaeaID );
	void	DEL_SHOP_ICON ( DWORD dwGaeaID );	
	void	SetRender( DWORD dwGaeaID );
	
};
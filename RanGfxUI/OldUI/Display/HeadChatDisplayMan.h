#pragma	once

#include <boost/pool/object_pool.hpp>
#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../../SigmaCore/Memory/CMemPool.h"
#include "../OldInterface.h"

class	OldInterface;
class	CHeadChatDisplay;
class	GLGaeaClient;

class CHeadChatDisplayMan : public CUIGroup
{
	static const float fVISIBLE_TIME;

	enum
	{
		BASE_CONTROL_ID = NO_ID + 1,
		BASE_CONTROL_ID_END = BASE_CONTROL_ID + 10000,
	};

private:
    typedef std::map<CString,CHeadChatDisplay*>	CHATMAP;
    typedef CHATMAP::iterator					CHATMAP_ITER;
    //typedef sc::CMemPool<CHeadChatDisplay>	HEADCHAT_POOL;
    typedef sc::BOOST_OBJECT_POOL_CHECKER<CHeadChatDisplay> HEADCHAT_POOL;

public:
	CHeadChatDisplayMan(GLGaeaClient* pGaeaClient, OldInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CHeadChatDisplayMan();

private:
	virtual	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void RemoveAll();

public:
	void AddChat( const CString& strName, const D3DCOLOR& dwIDColor, const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos );
    //CHATMAP& GetChatMap() { return m_mapChat; }

	// MEMO : 재정의
	bool DeleteControl( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

	CHeadChatDisplay*	MAKE_HEADCHATDISPLAY();
    void CheckOmitDisplayName();

private:
    CHATMAP	m_mapChat;
	UIGUID	m_CurID;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
    HEADCHAT_POOL		m_HeadChatPool;

protected:
	GLGaeaClient* m_pGaeaClient;
    OldInterface* m_pInterface;

};


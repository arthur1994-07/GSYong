#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/Summon/GLSummonClient.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
//#include "../../../SigmaCore/Memory/CMemPool.h"
#include "../../InnerInterface.h"

class CSummonNameDisplay;
class GLGaeaClient;
class CInnerInterface;
struct	GLPartySlotClient;

enum
{
	SUMMON_DISPLAY_DEFAULT = 1,
};

class CSummonNameDisplayMan : public CUIGroup
{
	enum
	{
		BASE_CONTROLID     = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 10000,
        BASE_CONTROLID_DEFAULT = 100,
	};

	typedef std::map<DWORD,CSummonNameDisplay*>	 SUMMONDISP_LIST;
	typedef SUMMONDISP_LIST::iterator		     SUMMONDISP_LIST_ITER;

    //typedef sc::CMemPool<CSummonNameDisplay>		 DISPLAY_POOL;

private:
	UIGUID				m_CONTROL_MAXID;

	int					m_nTYPE;
	SUMMONDISP_LIST  	m_listDISP;
	//DISPLAY_POOL    	m_Pool;
	DWORD				m_dwCOUNT;

    std::vector<DWORD>  m_vUIGUIDList;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	CSummonNameDisplayMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSummonNameDisplayMan();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void                SetType( int nType ) { m_nTYPE = nType; }
	int                 GetType()             { return m_nTYPE;  }

	// MEMO : 재정의
	bool                DeleteControl( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	virtual	HRESULT  	InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CSummonNameDisplay*	MAKE_SUMMONDISPLAY();
	void			    UpdateHold( DWORD _dwCOUNT );

private:
	void	            UPDATE_POS( CSummonNameDisplay* pDisplay, const D3DXVECTOR3& vPosition );
	bool	            IS_VISIBLE( const D3DXVECTOR3& vPosition, D3DXVECTOR3 & vScreen );

public:
	void	            SET_COUNT( DWORD _dwCOUNT )	{ m_dwCOUNT = _dwCOUNT; }
	void	            ADD_DISP( DWORD dwGlobID );
	void	            VisibleTargetInfoCtrl( DWORD dwGlobID, BOOL bVISIBLE );

public:
	//const CROWREN* GET_DISP_INFO ( EMCROW emCROW, DWORD dwCtrID );

public:
	void	            ClearList();
};

class MySummonNameDisplayMan : public ISummonNameDisplayMan, private CSummonNameDisplayMan
{
public:
    MySummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MySummonNameDisplayMan()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void ADD_DISP( DWORD dwGlobID );
    virtual void SET_COUNT( DWORD _dwCOUNT );
};
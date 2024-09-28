#pragma	once

#include <boost/pool/object_pool.hpp>
#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../../SigmaCore/Memory/CMemPool.h"
#include "../../InnerInterface.h"

class	CInnerInterface;
class	CNameDisplay;
class	GLGaeaClient;
struct	GLPartySlotClient;

enum
{
	NAME_DISPLAY_DEFAULT = 1,
	NAME_DISPLAY_ALL,
	NAME_DISPLAY_ITEMMONEY,
	NAME_DISPLAY_MOBCHAR,
	NAME_DISPLAY_PARTY,			//	파티 이름 표시
	NAME_DISPLAY_PARTY_CONFT,	//	파티 대련 중일 경우
	NAME_DISPLAY_CLUB_CONFT,	//	CLUB CONFT
};

class CNameDisplayMan : public CUIGroup
{
	enum
	{
		BASE_CONTROLID = NO_ID + 1,
		BASE_CONTROLID_END = BASE_CONTROLID + 10000,
        BASE_CONTROLID_DEFAULT = 100,
	};

	typedef std::pair<EMCROW,DWORD>						CROWRENCOPY_PAIR;
	typedef std::map<CROWRENCOPY_PAIR,CNameDisplay*>	CROWRENCOPY_MAP;
	typedef CROWRENCOPY_MAP::iterator					CROWRENCOPY_MAP_ITER;

    //typedef sc::CMemPool<CNameDisplay>		NAMEDISPLAY_POOL;
    //typedef BOOST_OBJECT_POOL_CHECKER<CNameDisplay>	NAMEDISPLAY_POOL;

private:
	UIGUID				m_CONTROL_MAXID;

	int					m_nNAMETYPE;
	CROWRENCOPY_MAP   	m_DispNameMap;
	//NAMEDISPLAY_POOL	m_NameDisplayPool;
	DWORD				m_dwCOUNT;

    std::vector<DWORD>  m_vUIGUIDList;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	CNameDisplayMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CNameDisplayMan();

public:
	void SetNameType( int nType )		{ m_nNAMETYPE = nType; }
	int GetNameType()					{ return m_nNAMETYPE; }

	// MEMO : 재정의
	bool DeleteControl( UIGUID ControlID, int refactoring ); // 컨트롤을 삭제한다.

private:
	virtual	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    
public:
    virtual void RemoveAll();

private:
	CNameDisplay*	MAKE_NAMEDISPLAY();
	void			UpdateHold( DWORD _dwCOUNT );

private:
	void	UPDATE_NAME_POS( CNameDisplay* pNameDisplay, const CROWREN& sCrowRen );
	bool	IS_VISIBLE_NAME( const CROWREN& sCrowRen, D3DXVECTOR3 & vScreen );

public:
	void	SET_COUNT( DWORD _dwCOUNT )	{ m_dwCOUNT = _dwCOUNT; }
	void	ADD_DISP_NAME( CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL );
	void	VisibleTargetInfoCtrl( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE );

	void	SetShowCountryMark ( bool bShowCountry );

public:
	const CROWREN* GET_DISP_INFO( EMCROW emCROW, DWORD dwCtrID );
    void OmitDisplayName( const EMCROW emCrow, const DWORD dwCrowID );

protected:
	CInnerInterface*	m_pInterface;
	GLGaeaClient*		m_pGaeaClient;

	bool				m_bShowCountryMark;

};

class MyNameDisplayMan : public INameDisplayMan, private CNameDisplayMan
{
public:
    MyNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyNameDisplayMan() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void VisibleTargetInfoCtrl( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE );
    virtual void ClearList();
    virtual void SET_COUNT( DWORD _dwCOUNT );
    virtual void ADD_DISP_NAME( CROWREN &sDISP_NAME, const TCHAR * szOwnerName = NULL );
    virtual int GetNameType();
    virtual void SetNameType( int nType );
    virtual const CROWREN* GetDisplayInfo( EMCROW emCROW, DWORD dwCtrID );
    virtual void OmitDisplayName( const EMCROW emCrow, const DWORD dwCrowID );

	virtual void SetShowCountryMark ( bool bShowCountry );
};
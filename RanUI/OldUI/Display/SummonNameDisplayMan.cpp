#include "StdAfx.h"

#include "SummonNameDisplayMan.h"
#include "SummonNameDisplay.h"
#include "NameDisplayMan.h"

#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"

#include "TargetInfoDisplay.h"

#include "HeadChatDisplayMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSummonNameDisplayMan::CSummonNameDisplayMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_nTYPE(SUMMON_DISPLAY_DEFAULT)
    , m_dwCOUNT(0)
    , m_CONTROL_MAXID(BASE_CONTROLID)
{
    m_vUIGUIDList.reserve(BASE_CONTROLID_DEFAULT);
    
    for ( int i = m_CONTROL_MAXID; i < m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT; ++i ) 
    {
        m_vUIGUIDList.push_back( i );
    }

    m_CONTROL_MAXID = m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT;
}

CSummonNameDisplayMan::~CSummonNameDisplayMan()
{
	ClearList();
}

CSummonNameDisplay* CSummonNameDisplayMan::MAKE_SUMMONDISPLAY()
{
	static UIRECT rCSummonNameDisplay; // MEMO : 기본 로컬 포지션을 저장
	CSummonNameDisplay* pSummonDisplay = new CSummonNameDisplay( m_pGaeaClient, m_pEngineDevice );

	if( !pSummonDisplay->IsMemPool() )
	{
//		if (BASE_CONTROLID_END <= m_CONTROL_NEWID)
//          m_CONTROL_NEWID = BASE_CONTROLID;	
//  	else
//          m_CONTROL_NEWID += BASE_CONTROLID;

        if ( m_vUIGUIDList.empty() ) 
        {
            if ( m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT >=  BASE_CONTROLID_END )
            {
                // 더이상 컨트롤 ID를 등록할수 없다.
                // 이 경우라면 BASE_CONTROLID_END 만큼 화면에 표시되고 있다는 상황이므로 말이 안된다.
                GASSERT( 0 && "CNameDisplayMan 유휴 ID가 없습니다." );

                SAFE_DELETE( pSummonDisplay );
                return NULL;
            }

            for ( int i = m_CONTROL_MAXID; i < m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT; ++i ) 
            {
                m_vUIGUIDList.push_back( i );
            }
            
            m_CONTROL_MAXID = m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT;
        }

        int nNewID = m_vUIGUIDList.back();
        m_vUIGUIDList.pop_back();

		pSummonDisplay->SetMemPool();
		pSummonDisplay->CreateSub( this, "SUMMON_NAME_DISPLAY", UI_FLAG_XSIZE | UI_FLAG_YSIZE, nNewID );
		pSummonDisplay->CreateSubControl();
		pSummonDisplay->InitDeviceObjects( m_pd3dDevice );
		pSummonDisplay->RestoreDeviceObjects( m_pd3dDevice );

		if ( !rCSummonNameDisplay.left )
            rCSummonNameDisplay = pSummonDisplay->GetLocalPos();
	}
	else
	{
		pSummonDisplay->AlignSubControl( pSummonDisplay->GetLocalPos(), rCSummonNameDisplay );
		pSummonDisplay->SetGlobalPos( rCSummonNameDisplay );
		pSummonDisplay->INITIALIZE();
	}

	RegisterControl( pSummonDisplay );
	return pSummonDisplay;
}

void CSummonNameDisplayMan::ADD_DISP( DWORD dwGlobID )
{
    std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(dwGlobID);
	if (!spSummon)
		return;

	SUMMONDISP_LIST_ITER found = m_listDISP.find(dwGlobID);
	BOOL bFlag = FALSE;
	// 없거나 다를 경우.
	if (found == m_listDISP.end())
	{
		bFlag = TRUE;
	}
	else
	{        
		CSummonNameDisplay* pDISP = found->second;
        if (pDISP->DIFFERENT(spSummon))
        {
		    //	Note : 종전 콘트롤 삭제.
		    //m_Pool.ReleaseNonInit( pDISP );
		    DeleteControl(pDISP->GetWndID(), 0);
		    m_listDISP.erase(found);
		    bFlag = TRUE;
        }
	}
	
	if (bFlag)
	{
		//	Note : 새로 생성.
		CSummonNameDisplay* pDISP_NEW = MAKE_SUMMONDISPLAY();
        if ( !pDISP_NEW ) return;

		DWORD dwNICK_COLOR(UINT_MAX);

		if ( !pDISP_NEW->SetSummmon(spSummon))
		{
			//m_Pool.ReleaseNonInit( pDISP_NEW );
			DeleteControl( pDISP_NEW->GetWndID(), 0 );
			m_listDISP.erase (m_listDISP.find(dwGlobID) );
			return;
		}

		pDISP_NEW->UPDATE ( m_dwCOUNT );
		UPDATE_POS( pDISP_NEW, pDISP_NEW->GETPOSITION() );	

		m_listDISP.insert( std::make_pair(dwGlobID, pDISP_NEW) );
	}
	else
	{
		CSummonNameDisplay* pDISP = found->second;

		pDISP->UPDATE( m_dwCOUNT );
		UPDATE_POS( pDISP, pDISP->GETPOSITION() );
	}
}

void CSummonNameDisplayMan::UpdateHold( DWORD _dwCOUNT )
{
	SUMMONDISP_LIST_ITER pos = m_listDISP.begin();	
	while (pos != m_listDISP.end())
	{		
		CSummonNameDisplay* pDISP = pos->second;
		if ( pDISP->GETCOUNT() != _dwCOUNT )
		{
			//m_Pool.ReleaseNonInit( pDISP );
			DeleteControl( pDISP->GetWndID(), 0 );
			pos = m_listDISP.erase(pos);
			continue;
		}

        ++pos;

		/*
		BOOL bVISIBLE = FALSE;
		switch ( m_nTYPE )
		{
		case NAME_DISPLAY_ALL:	:
			bVISIBLE = TRUE;
			break;

		case NAME_DISPLAY_ITEMMONEY:
			if ( emCROW==CROW_ITEM||emCROW==CROW_MONEY )	bVISIBLE = TRUE;
			break;

		case NAME_DISPLAY_PARTY:
		case NAME_DISPLAY_PARTY_CONFT:
		case NAME_DISPLAY_CLUB_CONFT:
			if ( pDISP->IsRELAY() )							bVISIBLE = TRUE;
			break;
		};

		//	 적대관계만...
		if ( pDISP->IsTARGET() )							bVISIBLE = TRUE;
		if ( pDISP->IsClubTARGET() )						bVISIBLE = TRUE;

		if ( emCROW==CROW_NPC )								bVISIBLE = TRUE;
		if ( !pDISP->GET_DISP() )							bVISIBLE = FALSE;
		*/

		pDISP->SetVisibleSingle( TRUE );
	}
}

void CSummonNameDisplayMan::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	

	//	NOTE : 등록된 이름 
	//
	UpdateHold( m_dwCOUNT );
}

void CSummonNameDisplayMan::ClearList()
{
	m_listDISP.clear();
    m_vUIGUIDList.clear();
    m_CONTROL_MAXID = BASE_CONTROLID;
	m_ControlContainer.RemoveAll();
}

HRESULT CSummonNameDisplayMan::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );
}

bool CSummonNameDisplayMan::IS_VISIBLE( const D3DXVECTOR3& vPosition, D3DXVECTOR3 & vScreen )
{
	D3DXVECTOR3 vPOS = vPosition;
	vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPOS, NULL );

	long lResolution = CUIMan::GetResolution();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<40) || (vScreen.y<40) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	return true;
}

void CSummonNameDisplayMan::UPDATE_POS( CSummonNameDisplay* pNameDisplay, const D3DXVECTOR3& vPosition )
{
	if ( !pNameDisplay )	return;

	D3DXVECTOR3 vScreen;

	if ( !IS_VISIBLE( vPosition, vScreen ) )
	{
		pNameDisplay->SET_DISP( false );
	}
	else
	{
		const UIRECT& rcOriginPos = pNameDisplay->GetGlobalPos();

		D3DXVECTOR2 vpos;
		vpos.x = floor( vScreen.x - ( rcOriginPos.sizeX * 0.5f ) ); // MEMO
		vpos.y = floor( vScreen.y - rcOriginPos.sizeY );

		pNameDisplay->SET_DISP( true );

		//if ( rcOriginPos.left!=vpos.x || rcOriginPos.top!=vpos.y )

		if ( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
		{
			pNameDisplay->SetGlobalPos( vpos );
		}
	}
}

void CSummonNameDisplayMan::VisibleTargetInfoCtrl( DWORD dwGlobID, BOOL bVISIBLE )
{
	SUMMONDISP_LIST_ITER iter = m_listDISP.find( dwGlobID );
	if ( iter!=m_listDISP.end() )
	{
		CSummonNameDisplay* pDISP = (*iter).second;

		pDISP->SetVisibleSingle( bVISIBLE );
	}
}

bool CSummonNameDisplayMan::DeleteControl( UIGUID ControlID, int refactoring )
{
    //  유휴 ID 반납
    m_vUIGUIDList.push_back ( ControlID );
	return m_ControlContainer.EraseControl( ControlID, TRUE );
}

MySummonNameDisplayMan::MySummonNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CSummonNameDisplayMan( pGaeaClient, pInterface, pEngineDevice )
{
}

void MySummonNameDisplayMan::CreateUIWindowAndRegisterOwnership()
{
    CSummonNameDisplayMan::Create( SUMMON_NAME_DISPLAY_MAN, "NAME_DISPLAY_MAN" );
    CSummonNameDisplayMan::m_pInterface->UiRegisterControl( this );
    CSummonNameDisplayMan::m_pInterface->UiShowGroupBottom( SUMMON_NAME_DISPLAY_MAN );
}

void MySummonNameDisplayMan::ADD_DISP( DWORD dwGlobID )
{
    CSummonNameDisplayMan::ADD_DISP( dwGlobID );
}

void MySummonNameDisplayMan::SET_COUNT( DWORD _dwCOUNT )
{
    CSummonNameDisplayMan::SET_COUNT( _dwCOUNT );
}

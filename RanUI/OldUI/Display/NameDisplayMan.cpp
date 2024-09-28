#include "StdAfx.h"

#include "NameDisplayMan.h"
#include "NameDisplay.h"

#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"

#include "TargetInfoDisplay.h"
#include "../PrivateMarket/PrivateMarketShowMan.h"

#include "HeadChatDisplayMan.h"

#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


CNameDisplayMan::CNameDisplayMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface( pInterface )
	, m_pGaeaClient( pGaeaClient )
	, m_nNAMETYPE( NAME_DISPLAY_DEFAULT )
	, m_dwCOUNT(0)
	, m_CONTROL_MAXID(BASE_CONTROLID)
	, m_bShowCountryMark(false)
{
    m_vUIGUIDList.reserve(BASE_CONTROLID_DEFAULT);
    
    for ( int i = m_CONTROL_MAXID; i < m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT; ++i ) 
    {
        m_vUIGUIDList.push_back( i );
    }

    m_CONTROL_MAXID = m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT;

}

CNameDisplayMan::~CNameDisplayMan()
{
	RemoveAll();
}

CNameDisplay* CNameDisplayMan::MAKE_NAMEDISPLAY()
{
	static UIRECT rcNameDisplay; // MEMO : 기본 로컬 포지션을 저장
	CNameDisplay* pNameDisplay = new CNameDisplay( m_pGaeaClient, m_pInterface, m_pEngineDevice ); //m_NameDisplayPool.New();
	if ( NULL == pNameDisplay )
		return NULL;

	// Show Country Mark 세팅;
	pNameDisplay->SetShowCountryMark( m_bShowCountryMark );

    //CNameDisplay* pNameDisplay = m_NameDisplayPool.construct(m_pGaeaClient, m_pRenderDevice);

	if ( !pNameDisplay->IsMemPool() )
	{
//		if ( BASE_CONTROLID_END <= m_CONTROL_NEWID ) m_CONTROL_NEWID = BASE_CONTROLID;	
//		else m_CONTROL_NEWID += BASE_CONTROLID;

        if ( m_vUIGUIDList.empty() ) 
        {
            if ( m_CONTROL_MAXID + BASE_CONTROLID_DEFAULT >=  BASE_CONTROLID_END )
            {
                // 더이상 컨트롤 ID를 등록할수 없다.
                // 이 경우라면 BASE_CONTROLID_END 만큼 화면에 표시되고 있다는 상황이므로 말이 안된다.
                GASSERT ( 0 && "CNameDisplayMan 유휴 ID가 없습니다." );
                SAFE_DELETE( pNameDisplay );
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

		pNameDisplay->SetMemPool();
		pNameDisplay->CreateSub( this, "NAME_DISPLAY", UI_FLAG_XSIZE | UI_FLAG_YSIZE, nNewID );
		pNameDisplay->CreateSubControl();
		pNameDisplay->InitDeviceObjects( m_pd3dDevice );
		pNameDisplay->RestoreDeviceObjects( m_pd3dDevice );

		if( !rcNameDisplay.left )
            rcNameDisplay = pNameDisplay->GetLocalPos();
	}
	else
	{
		pNameDisplay->AlignSubControl( pNameDisplay->GetLocalPos(), rcNameDisplay );
		pNameDisplay->SetGlobalPos( rcNameDisplay );
		pNameDisplay->INITIALIZE();
	}

	RegisterControl( pNameDisplay );
	return pNameDisplay;
}

void CNameDisplayMan::ADD_DISP_NAME( CROWREN &sDISP_NAME, const TCHAR * szOwnerName )
{
	if( sDISP_NAME.m_szNAME[0] == _T('\0') ) return;

	CROWRENCOPY_PAIR key = std::make_pair( sDISP_NAME.m_emCROW,sDISP_NAME.m_dwID );
	CROWRENCOPY_MAP_ITER iter = m_DispNameMap.find( key );

    CNameDisplay* pDISP = NULL;
    bool          bNew  = false;

	if ( iter!=m_DispNameMap.end() )
    {
        pDISP = iter->second;
        DWORD dwCtrlID = pDISP->GETCTRLID();

		// 다를 경우.
		if ( pDISP->DIFFERENT( sDISP_NAME ) )
		{
			//m_NameDisplayPool.ReleaseNonInit( pDISP );
            m_DispNameMap.erase( iter );            
            // 종전 콘트롤 삭제.
            DeleteControl( pDISP->GetWndID(), 0 );
            //m_NameDisplayPool.destroy(pDISP);

			//	Note : 새로 생성.
			pDISP = MAKE_NAMEDISPLAY();
            if ( !pDISP )
                return;
			
            pDISP->SET_INFO( sDISP_NAME, szOwnerName );
			m_DispNameMap.insert( std::make_pair(key,pDISP) );  

            bNew = true;
		}
	}
	else
	{
		pDISP = MAKE_NAMEDISPLAY();
        if ( !pDISP ) return;
		
        pDISP->SET_INFO( sDISP_NAME, szOwnerName );
		m_DispNameMap.insert( std::make_pair(key,pDISP) );

        bNew = true;
	}

    if ( !pDISP )
    {
        return;
    }

    //	Note : 상점 윈도우 관련
    IPrivateMarketShowMan* pPRIVATE_MARKET_SHOW = m_pInterface->GetPMarketShow();

    if ( bNew )
    {
        if ( pDISP->IsPMARKET() )
        {
            pPRIVATE_MARKET_SHOW->ADD_MARKET( sDISP_NAME );
        }
        else
        {
            pPRIVATE_MARKET_SHOW->DEL_MARKET( pDISP->GETCTRLID() );
        }
    }
    else
    {
        pPRIVATE_MARKET_SHOW->UPDATE_MARKET( sDISP_NAME );
    }

    UPDATE_NAME_POS( pDISP, sDISP_NAME );

    sDISP_NAME.m_dwCOUNT = m_dwCOUNT;
    pDISP->UPDATE( sDISP_NAME );
}

void CNameDisplayMan::SetShowCountryMark ( bool bShowCountry )
{
	m_bShowCountryMark = bShowCountry;

	CROWRENCOPY_MAP_ITER pos = m_DispNameMap.begin();
	CROWRENCOPY_MAP_ITER end = m_DispNameMap.end();
	while ( pos!=end )
	{
		CROWRENCOPY_MAP_ITER cur = pos++;
		CNameDisplay* pDISP = cur->second;

		if ( pDISP )
			pDISP->SetShowCountryMark( bShowCountry );
	}
}

void CNameDisplayMan::UpdateHold( DWORD _dwCOUNT )
{
	IPrivateMarketShowMan* pPRIVATE_MARKET_SHOW = m_pInterface->GetPMarketShow();
    IHeadChatDisplayMan* pHeadChatDisplayMan = m_pInterface->GetHeadChatDisplayMan();

	CROWRENCOPY_MAP_ITER pos = m_DispNameMap.begin();
	CROWRENCOPY_MAP_ITER end = m_DispNameMap.end();
	while ( pos!=end )
	{
		CROWRENCOPY_MAP_ITER cur = pos++;
		CNameDisplay *pDISP = cur->second;

		if ( pDISP->GETCOUNT() != _dwCOUNT )
		{
			//	Note : 상점 윈도우 생성.
			if ( pPRIVATE_MARKET_SHOW && pDISP->IsPMARKET() )
			{
				pPRIVATE_MARKET_SHOW->DEL_MARKET( pDISP->GETCTRLID() );
			}

			//m_NameDisplayPool.ReleaseNonInit( pDISP );
			DeleteControl( pDISP->GetWndID(), 0 );
			m_DispNameMap.erase( cur );
            //m_NameDisplayPool.destroy(pDISP);

			continue;
		}

		BOOL bVISIBLE = FALSE;
		EMCROW emCROW = pDISP->GETCROW();
		switch ( m_nNAMETYPE )
		{
		case NAME_DISPLAY_ALL:	
		case NAME_DISPLAY_MOBCHAR:
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

		pDISP->SetVisibleSingle( bVISIBLE );
	}
}

void CNameDisplayMan::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//	의도적으로 호출하지 않음
	//CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	//	NOTE : 등록된 이름 
	//
	UpdateHold( m_dwCOUNT );

    //	채팅 출력시
    //	기본 정보 출력 생략
    IHeadChatDisplayMan* pHeadChatDisplayMan = m_pInterface->GetHeadChatDisplayMan();
    if ( pHeadChatDisplayMan )
    {
        pHeadChatDisplayMan->CheckOmitDisplayName();
    }
}

HRESULT CNameDisplayMan::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );
}

bool CNameDisplayMan::IS_VISIBLE_NAME( const CROWREN& sDIS_NAME, D3DXVECTOR3 & vScreen )
{
	D3DXVECTOR3 vPOS = sDIS_NAME.m_vPOS;
	vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPOS, NULL );

	long lResolution = CUIMan::GetResolution();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<40) || (vScreen.y<40) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	D3DXVECTOR3 LookAtPt = DxViewPort::GetInstance().GetLookatPt() - DxViewPort::GetInstance().GetFromPt();
    LookAtPt.y = 0.0f;
    D3DXVECTOR3 TargetPos = sDIS_NAME.m_vPOS - DxViewPort::GetInstance().GetFromPt();
    TargetPos.y = 0.0f;
    float LookAtLength = D3DXVec3Length( &LookAtPt );
    float PosLength = D3DXVec3Length( &TargetPos );
    float CosTheta = D3DXVec3Dot( &LookAtPt, &TargetPos ) / ( LookAtLength * PosLength );
    if ( CosTheta < 0 )
        return false;

	return true;
}

void CNameDisplayMan::UPDATE_NAME_POS( CNameDisplay* pNameDisplay, const CROWREN& sDIS_NAME )
{
	if ( !pNameDisplay )	return;

	if ( pNameDisplay->IsPMARKET() )
	{
		pNameDisplay->SET_DISP( false );
		return;
	}

	D3DXVECTOR3 vScreen;

	if ( !IS_VISIBLE_NAME( sDIS_NAME, vScreen ) )
	{
		pNameDisplay->SET_DISP( false );
	}
	else
	{
		const UIRECT& rcOriginPos = pNameDisplay->GetGlobalPos();

		D3DXVECTOR2 vpos;
		vpos.x = floor( vScreen.x - ( rcOriginPos.sizeX * 0.5f ) + 0.5f ); // MEMO
		vpos.y = floor( vScreen.y - rcOriginPos.sizeY + 0.5f );

		pNameDisplay->SET_DISP( true );

		//if ( rcOriginPos.left!=vpos.x || rcOriginPos.top!=vpos.y )
		
		if ( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
		{
			pNameDisplay->SetGlobalPos( vpos );
		}
	}
}

void CNameDisplayMan::VisibleTargetInfoCtrl( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE )
{
	CROWRENCOPY_MAP_ITER iter = m_DispNameMap.find( CROWRENCOPY_PAIR(emCROW,dwCtrID) );
	if ( iter!=m_DispNameMap.end() )
	{
		CROWRENCOPY_PAIR sKEY = (*iter).first; // MEMO
		CNameDisplay* pDISP = (*iter).second;
		
		pDISP->SetVisibleSingle( bVISIBLE );
	}
}

const CROWREN* CNameDisplayMan::GET_DISP_INFO( EMCROW emCROW, DWORD dwCtrID )
{
	CROWRENCOPY_MAP_ITER iter = m_DispNameMap.find( CROWRENCOPY_PAIR(emCROW,dwCtrID) );
	if ( iter==m_DispNameMap.end() )		return NULL;

	CROWRENCOPY_PAIR sKEY = (*iter).first; // MEMO
	CNameDisplay* pDISP = (*iter).second;
	return pDISP->GET_INFO();
}

bool CNameDisplayMan::DeleteControl( UIGUID ControlID, int refactoring )
{
    //  유휴 ID 반납
    m_vUIGUIDList.push_back( ControlID );
	return m_ControlContainer.EraseControl( ControlID, TRUE );
}

void CNameDisplayMan::OmitDisplayName( const EMCROW emCrow, const DWORD dwCrowID )
{
    CROWRENCOPY_MAP_ITER iter = m_DispNameMap.find( CROWRENCOPY_PAIR( emCrow, dwCrowID ) );
    if ( iter!=m_DispNameMap.end() )
        (*iter).second->SetVisibleSingle( FALSE );
}

void CNameDisplayMan::RemoveAll()
{
    IPrivateMarketShowMan* pPRIVATE_MARKET_SHOW = m_pInterface->GetPMarketShow();
    if ( pPRIVATE_MARKET_SHOW )
        pPRIVATE_MARKET_SHOW->RemoveAll();

    m_DispNameMap.clear();
    m_vUIGUIDList.clear();
    m_CONTROL_MAXID = BASE_CONTROLID;
    m_ControlContainer.RemoveAll();
}

MyNameDisplayMan::MyNameDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CNameDisplayMan( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyNameDisplayMan::CreateUIWindowAndRegisterOwnership()
{
    CNameDisplayMan::Create( NAME_DISPLAY_MAN, "NAME_DISPLAY_MAN" );
    CNameDisplayMan::m_pInterface->UiRegisterControl( this );
    CNameDisplayMan::m_pInterface->UiShowGroupBottom( NAME_DISPLAY_MAN );
    CNameDisplayMan::m_pInterface->UiHideGroup( NAME_DISPLAY_MAN );
}

void MyNameDisplayMan::VisibleTargetInfoCtrl( EMCROW emCROW, DWORD dwCtrID, BOOL bVISIBLE )
{
    CNameDisplayMan::VisibleTargetInfoCtrl( emCROW, dwCtrID, bVISIBLE );
}

void MyNameDisplayMan::ClearList()
{
    CNameDisplayMan::RemoveAll();
}

void MyNameDisplayMan::SET_COUNT( DWORD _dwCOUNT )
{
    CNameDisplayMan::SET_COUNT( _dwCOUNT );
}

void MyNameDisplayMan::ADD_DISP_NAME( CROWREN &sDISP_NAME, const TCHAR * szOwnerName /*= NULL */ )
{
    CNameDisplayMan::ADD_DISP_NAME( sDISP_NAME, szOwnerName );
}

int MyNameDisplayMan::GetNameType()
{
    return CNameDisplayMan::GetNameType();
}

void MyNameDisplayMan::SetNameType( int nType )
{
    CNameDisplayMan::SetNameType( nType );
}

const CROWREN* MyNameDisplayMan::GetDisplayInfo( EMCROW emCROW, DWORD dwCtrID )
{
    return CNameDisplayMan::GET_DISP_INFO( emCROW, dwCtrID );
}

void MyNameDisplayMan::OmitDisplayName( const EMCROW emCrow, const DWORD dwCrowID )
{
    CNameDisplayMan::OmitDisplayName( emCrow, dwCrowID );
}

void MyNameDisplayMan::SetShowCountryMark ( bool bShowCountry )
{
	CNameDisplayMan::SetShowCountryMark( bShowCountry );
}
#include "StdAfx.h"
#include "../../../RanLogicClient/ClientActor.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "HeadChatDisplayMan.h"
#include "HeadChatDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float	CHeadChatDisplayMan::fVISIBLE_TIME = 8.0f;

CHeadChatDisplayMan::CHeadChatDisplayMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
	, m_CurID( 0 )

	, m_HeadChatPool(_T("HEADCHAT_POOL"))
{
}

CHeadChatDisplayMan::~CHeadChatDisplayMan()
{
    RemoveAll();
}

void CHeadChatDisplayMan::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIControl::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( !IsVisible() )
        return ;

	std::vector<UIGUID> vecDELETE_CTRL;
    std::vector<CHeadChatDisplay*> vecDELETE_PTR; 
	const CUIControlContainer::UICONTROL_LIST& vec = m_ControlContainer.GetControl();
	CUIControlContainer::UICONTROL_LIST_CITER citer = vec.begin();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = vec.end();

	for ( ; citer != citer_end; ++citer )
	{
		CUIControl* pControl = (*citer);

		//	보이지 않는 컨트롤은 업데이트 하지 않는다.
		if ( !pControl->IsVisible() )
            continue;

		pControl->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		UIGUID ControlID = pControl->GetWndID();	
		DWORD dwMsg = pControl->GetMessageEx();

		if ( CHECK_KEYFOCUSED( dwMsg ) )
            CUIControl::AddMessageEx( UIMSG_KEY_FOCUSED );
		if ( CHECK_TOPARENT( dwMsg ) )
            CUIControl::AddMessageEx( CUT_LOW24BIT ( dwMsg ) );

		if ( bFirstControl )
		{
			NS_UIDEBUGSET::BlockBegin();
			if ( dwMsg && ( ControlID != NO_ID ) )
                TranslateUIMessage( ControlID, dwMsg );
			NS_UIDEBUGSET::BlockEnd();
		}

		CHeadChatDisplay* pHeadChatDisplay = (CHeadChatDisplay*) pControl;
		float fLifeTime = pHeadChatDisplay->GetLifeTime();
		if ( fLifeTime < 0.0f )
		{
			//m_HeadChatPool.ReleaseNonInit( pHeadChatDisplay );
			
			CHATMAP_ITER iter = m_mapChat.find( pHeadChatDisplay->GetName() );
			if ( iter != m_mapChat.end() )
			{
				m_mapChat.erase( iter );
			}

			vecDELETE_CTRL.push_back( pControl->GetWndID() );
            vecDELETE_PTR.push_back(pHeadChatDisplay);
		}
	}

	for ( size_t i=0; i<vecDELETE_CTRL.size(); ++i )
	{
        DeleteControl( vecDELETE_CTRL[i], 0 );
        m_HeadChatPool.destroy(vecDELETE_PTR[i]);
	}
}


HRESULT CHeadChatDisplayMan::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );
}

void CHeadChatDisplayMan::AddChat( const CString& strName, const D3DCOLOR& dwIDColor, const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos )
{
	
	CHATMAP_ITER iter = m_mapChat.find( strName );
	if ( iter != m_mapChat.end() )
	{
		//m_HeadChatPool.ReleaseNonInit( (*iter).second );
        CHeadChatDisplay* pControl = iter->second;
		UIGUID ControlID = pControl->GetWndID();
		DeleteControl( ControlID, 0 );
		m_mapChat.erase( iter );
        m_HeadChatPool.destroy(pControl);
	}

	CHeadChatDisplay* pHeadChatDisplay = MAKE_HEADCHATDISPLAY();

	pHeadChatDisplay->SetLifeTime( fVISIBLE_TIME );
	pHeadChatDisplay->SetChat( strName, dwIDColor, strChat, dwChatColor );
	pHeadChatDisplay->SetGlobalPos( vPos );

	m_mapChat.insert( std::make_pair(strName,pHeadChatDisplay) );
}

CHeadChatDisplay* CHeadChatDisplayMan::MAKE_HEADCHATDISPLAY()
{
	//CHeadChatDisplay* pHeadChatDisplay = new CHeadChatDisplay( m_pGaeaClient, m_pRenderDevice );//m_HeadChatPool.New();

	

    CHeadChatDisplay* pHeadChatDisplay = m_HeadChatPool.construct(m_pGaeaClient, m_pEngineDevice);

	if( !pHeadChatDisplay->IsMemPool() )
	{
		if ( BASE_CONTROL_ID_END <= m_CurID ) m_CurID = BASE_CONTROL_ID;
		else m_CurID += BASE_CONTROL_ID;

		pHeadChatDisplay->SetMemPool();
		pHeadChatDisplay->CreateSub( this, "HEAD_CHAT_DISPLAY", UI_FLAG_XSIZE | UI_FLAG_YSIZE, m_CurID );
		pHeadChatDisplay->CreateSubControl();
		pHeadChatDisplay->InitDeviceObjects( m_pd3dDevice );
		pHeadChatDisplay->RestoreDeviceObjects( m_pd3dDevice );
	}

	RegisterControl( pHeadChatDisplay );
	return pHeadChatDisplay;
}

bool CHeadChatDisplayMan::DeleteControl( UIGUID ControlID, int refactoring )
{
	//return m_ControlContainer.EraseControl( ControlID, TRUE );
    return m_ControlContainer.EraseControl( ControlID, FALSE );
}

void CHeadChatDisplayMan::CheckOmitDisplayName()
{
    //  채팅 출력시 기본 정보 출력 생략.
    //CHATMAP & pChatMap = GetChatMap ();
    CHATMAP_ITER iter = m_mapChat.begin(); // pChatMap.begin();
    CHATMAP_ITER iter_end = m_mapChat.end(); //pChatMap.end();
    for (; iter!=iter_end; ++iter)
    {
        ClientActor* pCOPY = m_pGaeaClient->GetCopyActor( std::string( (*iter).first ) );
        if ( !pCOPY )
            continue;

        INameDisplayMan* pNameDisplayMan = m_pInterface->GetDispName();
        if ( pNameDisplayMan )
        {
            pNameDisplayMan->OmitDisplayName( pCOPY->GetCrow(), pCOPY->GetGaeaID() );
        }
    }
}

void CHeadChatDisplayMan::RemoveAll()
{
    const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl();
    CUIControlContainer::UICONTROL_LIST_CITER iter = list.begin();
    CUIControlContainer::UICONTROL_LIST_CITER iter_end = list.end();

    for ( ; iter != iter_end; ++iter )
    {
        CHeadChatDisplay* pControl = dynamic_cast<CHeadChatDisplay*> (*iter);
        m_HeadChatPool.destroy(pControl);
    }

    m_ControlContainer.RemoveAll( false );
    m_mapChat.clear();
}

MyHeadChatDisplayMan::MyHeadChatDisplayMan( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CHeadChatDisplayMan( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyHeadChatDisplayMan::CreateUIWindowAndRegisterOwnership()
{
    CHeadChatDisplayMan::Create( HEADCHAT_MAN, "HEADCHAT_MAN" );
    CHeadChatDisplayMan::m_pInterface->UiRegisterControl( this );
    CHeadChatDisplayMan::m_pInterface->UiShowGroupBottom( HEADCHAT_MAN );
}

void MyHeadChatDisplayMan::AddChatMsg( const UI::String& strName, const D3DCOLOR& dwIDColor, const UI::String& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos )
{
    CHeadChatDisplayMan::AddChat( UI::ToString( strName ), dwIDColor, UI::ToString( strChat ), dwChatColor, vPos );
}

void MyHeadChatDisplayMan::CheckOmitDisplayName()
{
    CHeadChatDisplayMan::CheckOmitDisplayName();
}
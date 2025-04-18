#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../StaticUIManager.h"
#include "../../InnerInterface.h"
#include "ItemShopIcon.h"
#include "ItemShopIconMan.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

CItemShopIconMan::CItemShopIconMan(GLGaeaClient* pGaeaClient, LPDIRECT3DDEVICEQ pd3dDevice, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_CONTROL_NEWID(0)
    , m_pd3dDevice(pd3dDevice)

	, m_IconPool(_T("CItemShopIcon"))
{
}

CItemShopIconMan::~CItemShopIconMan ()
{
	RemoveAll ();
}

CItemShopIcon* CItemShopIconMan::MAKE_SHOPICON ()
{
	static UIRECT rcShopIcon; // MEMO : 기본 로컬 포지션을 저장
	//CItemShopIcon* pShopIcon = m_IconPool.New();
    CItemShopIcon* pShopIcon = m_IconPool.construct(m_pEngineDevice);

    //  메모리풀 사용안할때 ControlID 지정하는 프로세서 변경해야 합니다.
    //  CNameDisplayMan을 참고하세요.

	if( !pShopIcon->IsMemPool() )
	{
		if (BASE_CONTROLID_END <= m_CONTROL_NEWID)
            m_CONTROL_NEWID = BASE_CONTROLID;	
		else
            m_CONTROL_NEWID += BASE_CONTROLID;

		pShopIcon->SetMemPool();
		pShopIcon->CreateSub ( this, "ITEM_SHOP_ICON", UI_FLAG_XSIZE | UI_FLAG_YSIZE, m_CONTROL_NEWID );
		pShopIcon->CreateSubControl ();
		pShopIcon->InitDeviceObjects ( m_pd3dDevice );
		pShopIcon->RestoreDeviceObjects ( m_pd3dDevice );

		if (!rcShopIcon.left)
            rcShopIcon = pShopIcon->GetLocalPos();
	}
	else
	{
		pShopIcon->AlignSubControl( pShopIcon->GetLocalPos(), rcShopIcon );
		pShopIcon->SetGlobalPos( rcShopIcon );
		pShopIcon->INITIALIZE();
	}

	RegisterControl(pShopIcon);
	return pShopIcon;
}

void CItemShopIconMan::ADD_SHOP_ICON ( DWORD dwGaeaID )
{
	SHOPICON_MAP_ITER found = m_mapIcon.find ( dwGaeaID );
	if ( found!=m_mapIcon.end() )
	{
		CItemShopIcon* pIcon = (*found).second;

		D3DXVECTOR3* vPos = m_pGaeaClient->FindCharHeadPos(dwGaeaID);
		if (!vPos)
            return;

		UPDATE_POS ( pIcon, *vPos );		
	}
	else
	{
		CItemShopIcon* pIcon_NEW = MAKE_SHOPICON ();
		pIcon_NEW->SetVisibleSingle( FALSE );

		D3DXVECTOR3* vPos = m_pGaeaClient->FindCharHeadPos(dwGaeaID);
		if ( vPos )
		{
			UPDATE_POS ( pIcon_NEW, *vPos );
		}

		m_mapIcon.insert ( std::make_pair(dwGaeaID,pIcon_NEW) );	
	}
}

void CItemShopIconMan::DEL_SHOP_ICON ( DWORD dwGaeaID )
{
	SHOPICON_MAP_ITER iter = m_mapIcon.find ( dwGaeaID );	
	if ( iter!=m_mapIcon.end() )
	{
		CItemShopIcon* pIcon = iter->second;
		//m_IconPool.ReleaseNonInit( pIcon );	
		DeleteControl(pIcon->GetWndID(), 0);
		m_mapIcon.erase( iter );
        m_IconPool.destroy(pIcon);
	}
}

void CItemShopIconMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//	의도적으로 호출하지 않음
	//CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	SHOPICON_MAP_ITER pos = m_mapIcon.begin();
	SHOPICON_MAP_ITER end = m_mapIcon.end();
	while ( pos!=end )
	{
		CItemShopIcon* pIcon = (*pos).second;
		DWORD dwGaeaID = (*pos).first;

		D3DXVECTOR3* vPos = m_pGaeaClient->FindCharHeadPos( dwGaeaID );
		if ( vPos )
		{
			UPDATE_POS( pIcon, *vPos );
		}
		
		pIcon->SetVisibleSingle( pIcon->IsVisible() );      

		++pos;
	}
}

void CItemShopIconMan::RemoveAll()
{
    const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl();
    CUIControlContainer::UICONTROL_LIST_CITER iter = list.begin();
    CUIControlContainer::UICONTROL_LIST_CITER iter_end = list.end();

    for ( ; iter != iter_end; ++iter )
    {
        CItemShopIcon* pControl = dynamic_cast<CItemShopIcon*> (*iter);
        m_IconPool.destroy(pControl);
    }

    m_ControlContainer.RemoveAll( false );
    m_mapIcon.clear();
}

HRESULT CItemShopIconMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

bool CItemShopIconMan::IS_VISIBLE_NAME ( D3DXVECTOR3 vPos, D3DXVECTOR3 & vScreen )
{
	vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	long lResolution = CUIMan::GetResolution ();
	WORD X_RES = HIWORD(lResolution) - 30;
	WORD Y_RES = LOWORD(lResolution) - 30;

	if ( (vScreen.x<60) || (vScreen.y<60) || (X_RES<vScreen.x) || (Y_RES<vScreen.y) )
	{
		return false;
	}

	return true;
}

void CItemShopIconMan::UPDATE_POS ( CItemShopIcon* pShopIcon, D3DXVECTOR3 vPos )
{
	if ( !pShopIcon )	return;	

	D3DXVECTOR3 vScreen;

	if( !IS_VISIBLE_NAME( vPos, vScreen ) || !pShopIcon->IsRender() )
	{
		pShopIcon->SetVisible( false );
	}
	else
	{
		const UIRECT& rcOriginPos = pShopIcon->GetGlobalPos ();

		D3DXVECTOR2 vpos;
		vpos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f )); // MEMO
		vpos.y = floor(vScreen.y - rcOriginPos.sizeY);
		vpos.y -= ( rcOriginPos.sizeY * 2.0f );
		

		pShopIcon->SetVisible ( true );
		pShopIcon->SetRender( false );
		
		if( abs( rcOriginPos.left - vpos.x ) > 1 || abs( rcOriginPos.top - vpos.y ) > 1 )
		{
			pShopIcon->SetGlobalPos( vpos );
		}
	}
}

bool CItemShopIconMan::DeleteControl( UIGUID ControlID, int refactoring )
{
	return m_ControlContainer.EraseControl(ControlID, FALSE);
}

void CItemShopIconMan::SetRender( DWORD dwGaeaID )
{
	SHOPICON_MAP_ITER iter = m_mapIcon.find(dwGaeaID);
	if (iter != m_mapIcon.end())
	{
		CItemShopIcon* pIcon = iter->second;
		pIcon->SetRender(true);
	}
}
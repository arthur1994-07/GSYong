#include "StdAfx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../enginelib/GUInterface/SwapImage.h"
#include "../../../enginelib/DxTools/DxClubMan.h"
#include "../../../enginelib/DeviceMan.h"
#include "../../InnerInterface.h"
#include "RnOldLargeMapWindow.h"

#include "RnMapGuidanceUI.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

RnMapGuidanceUI::RnMapGuidanceUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, RnOldLargeMapWindow* pOldLargeMap)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pOldLargeMap(pOldLargeMap)
{

}

RnMapGuidanceUI::~RnMapGuidanceUI()
{
}

void RnMapGuidanceUI::SetGuidanceAuthInfo( const NET_MSG_GENERIC* pMessage )
{
    GLMSG::NET_INSTANCE_GUIDANCE_UI_MINIMAP_FC netMsg;
    if ( CastNetMsg(pMessage, netMsg) == false )
        return;

    GLCharacter* _pCharacter = m_pGaeaClient->GetCharacter();
    if ( _pCharacter == NULL )
        return;

    dwGuidanceMapID = netMsg.dwGuidanceMapID;
    const size_t AuthClubNum = netMsg.vecAuthClub.size();
    const size_t AcceleratorNum = netMsg.vecAccelerator.size();
    // 扁粮 UI 昏力;
    for ( GUIDANCE_MARK_INFO_VECTOR_ITER iter = m_vecGuidanceMarkInfo.begin(); iter != m_vecGuidanceMarkInfo.end(); iter++)
    {
        DeleteControl(iter->pMarkImage->GetWndID(), 0);
    }
    const DWORD _myClubID = _pCharacter->GetClubDbNum();
    // 货 UI 积己;
    m_vecGuidanceMarkInfo.clear();
    m_vecGuidanceMarkInfo.reserve(AuthClubNum + AcceleratorNum);
    for ( size_t i = 0; i < AuthClubNum; i++ )
    {
        CSwapImage* pImage = new CSwapImage(m_pEngineDevice);

        if ( netMsg.vecAuthClub[i].clubID == CLUB_NULL )
        {
			switch ( netMsg.dwIconType )
			{
			case 0:
				pImage->CreateSub(this,"GUIDANCE_MAP_CLUB_MARK_NONE");
				break;

			case 1:
				pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_NONE");
				break;
			}
           
        }
        else if ( netMsg.vecAuthClub[i].clubID == _myClubID )
        {
			switch ( netMsg.dwIconType )
			{
			case 0:
				pImage->CreateSub(this,"GUIDANCE_MAP_CLUB_MARK_MYCLUB");
				break;

			case 1:
				pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_MYCLUB");
				break;
			}
        }
        else
        {
			switch ( netMsg.dwIconType )
			{
			case 0:
				pImage->CreateSub(this,"GUIDANCE_MAP_CLUB_MARK_OTEHRCLUB");
				break;

			case 1:
				pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_OTEHRCLUB");
				break;
			}
        }
        RegisterControl(pImage);
        pImage->InitDeviceObjects(m_pEngineDevice->GetDevice());

        GuidanceMarkInfo _info;
        _info.vMarkPos = D3DXVECTOR3(netMsg.vecAuthClub[i].fXpos, 0.0f, netMsg.vecAuthClub[i].fZpos);
        _info.pMarkImage = pImage;
        m_vecGuidanceMarkInfo.push_back(_info);
    }
    for ( size_t i = 0; i < AcceleratorNum; i++ )
    {
        CSwapImage* pImage = new CSwapImage(m_pEngineDevice);

		switch ( netMsg.dwIconType )
		{
		case 0:
			pImage->CreateSub(this,"GUIDANCE_MAP_ACCELERATOR_MARK");
			break;

		case 1:
			pImage->CreateSub( this,"CTI_MAP_ACCELERATOR_MARK" );
			break;
		}

        RegisterControl(pImage);
        pImage->InitDeviceObjects(m_pEngineDevice->GetDevice());

        GuidanceMarkInfo _info;
        _info.vMarkPos = D3DXVECTOR3(netMsg.vecAccelerator[i].fXpos, 0.0f, netMsg.vecAccelerator[i].fZpos);
        _info.pMarkImage = pImage;
        m_vecGuidanceMarkInfo.push_back(_info);
    }
}

void RnMapGuidanceUI::SetCTIAuthInfo ( const NET_MSG_GENERIC* pMsg )
{
	GLMSG::NET_INSTANCE_CTI_UI_MINIMAP_FC netMsg;
	if ( false == CastNetMsg( pMsg, netMsg ) )
		return;

	dwGuidanceMapID = netMsg.dwGuidanceMapID;
	const size_t AuthCountryNum = netMsg.vecAuthCountry.size();
	const size_t AcceleratorNum = netMsg.vecAccelerator.size();

	// 扁粮 UI 昏力;
	for ( GUIDANCE_MARK_INFO_VECTOR_ITER iter = m_vecGuidanceMarkInfo.begin(); iter != m_vecGuidanceMarkInfo.end(); iter++ )
	{
		DeleteControl( iter->pMarkImage->GetWndID(), 0 );
	}

	const Country::SCOUNTRY_INFO sCountryInfo = m_pGaeaClient->GetCountryInfo();

	// 货 UI 积己;
	m_vecGuidanceMarkInfo.clear();
	m_vecGuidanceMarkInfo.reserve( AuthCountryNum + AcceleratorNum );

	for ( size_t i = 0; i < AuthCountryNum; i++ )
	{
		CSwapImage* pImage = new CSwapImage( m_pEngineDevice );

		if ( netMsg.vecAuthCountry[i].sCountryInfo.ISNONCOUNTRY() )
		{
			pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_NONE");
		}
		else if ( netMsg.vecAuthCountry[i].sCountryInfo == sCountryInfo )
		{
			pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_MYCLUB");
		}
		else
		{
			pImage->CreateSub(this,"CTI_MAP_COUNTRY_MARK_OTEHRCLUB");
		}

		RegisterControl( pImage );
		pImage->InitDeviceObjects( m_pEngineDevice->GetDevice() );

		GuidanceMarkInfo _info;
		_info.vMarkPos = D3DXVECTOR3(
			netMsg.vecAuthCountry[i].fXpos,
			0.0f,
			netMsg.vecAuthCountry[i].fZpos );
		_info.pMarkImage = pImage;

		m_vecGuidanceMarkInfo.push_back( _info );
	}

	for ( size_t i = 0; i < AcceleratorNum; i++ )
	{
		CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
		pImage->CreateSub( this,"CTI_MAP_ACCELERATOR_MARK" );
		RegisterControl( pImage );
		pImage->InitDeviceObjects( m_pEngineDevice->GetDevice() );

		GuidanceMarkInfo _info;
		_info.vMarkPos = D3DXVECTOR3(
			netMsg.vecAccelerator[i].fXpos,
			0.0f,
			netMsg.vecAccelerator[i].fZpos );
		_info.pMarkImage = pImage;

		m_vecGuidanceMarkInfo.push_back( _info );
	}
}

void RnMapGuidanceUI::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
    for ( GUIDANCE_MARK_INFO_VECTOR_ITER iter = m_vecGuidanceMarkInfo.begin(); iter != m_vecGuidanceMarkInfo.end(); iter++ )
    {
        GuidanceMarkInfo& pInfo = *iter;
        CSwapImage* pImage = pInfo.pMarkImage;
        int nMapPosX(0), nMapPosY(0);

        const UIRECT& rcMarkPos = pImage->GetGlobalPos ();
        const float _fMarkHalfSizeX = rcMarkPos.sizeX * 0.5f;
        const float _fMarkHalfSizeY = rcMarkPos.sizeY * 0.5f;
        if ( m_pOldLargeMap->Convert_WordlPos2MapRenderPos(pInfo.vMarkPos, nMapPosX, nMapPosY, (int)_fMarkHalfSizeX, (int)_fMarkHalfSizeY) == false )
        {
            pImage->SetVisibleSingle(FALSE);
            continue;
        }

        pImage->SetVisibleSingle(TRUE);
        const UIRECT& rcGlobalPos = this->GetGlobalPos ();        
        D3DXVECTOR2 vNewMarkPos ( nMapPosX + rcGlobalPos.left - _fMarkHalfSizeX, nMapPosY + rcGlobalPos.top - _fMarkHalfSizeY );
        pImage->SetGlobalPos ( vNewMarkPos );
    }
}

HRESULT RnMapGuidanceUI::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    const GLLevelFileClient* pLevFile = m_pOldLargeMap->GetMapLevelFile();

    if ( pLevFile->GetBaseMapID() != dwGuidanceMapID )
        return S_OK;

    CUIGroup::Render(pd3dDevice);
    return S_OK;
}
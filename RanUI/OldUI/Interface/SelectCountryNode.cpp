#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./SelectCountryNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCountryNode::CSelectCountryNode(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
}

CSelectCountryNode::~CSelectCountryNode()
{
}

void CSelectCountryNode::CreateSubControl()
{
    {
        CSwapImage* pControl = new CSwapImage( m_pEngineDevice );
        pControl->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE_FLAG", UI_FLAG_DEFAULT, NODE_FLAG );
        RegisterControl( pControl );
        m_pFlag = pControl;
    }
}

void CSelectCountryNode::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectCountryNode::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

}

void CSelectCountryNode::SetCountryInfo( const wb::COUNTRY_INFO& CountryInfo )
{
    m_CountryInfo = CountryInfo;
    if ( !m_pFlag )
        return;

    m_pFlag->SetImageTexture( m_CountryInfo.m_Pos.m_TextureName.c_str() );
    float fTextureSizeX = static_cast<float>(GLogicData::GetInstance().wbGetTextureWidth());
    float fTextureSizeY = static_cast<float>(GLogicData::GetInstance().wbGetTextureHeight());

    float fFlagSizeX = static_cast<float>(m_pGaeaClient->wbGetFlagWidth());
    float fFlagSizeY = static_cast<float>(m_pGaeaClient->wbGetFlagHeight());

    m_pFlag->SetTexturePos( UIRECT(
        (m_CountryInfo.m_Pos.m_Left+0.25f)/fTextureSizeX,
        (m_CountryInfo.m_Pos.m_Top+0.25f)/fTextureSizeY,
        fFlagSizeX/fTextureSizeX,
        fFlagSizeY/fTextureSizeY ) );
}

const wb::COUNTRY_INFO& CSelectCountryNode::GetCountyInfo()
{
    return m_CountryInfo;
}

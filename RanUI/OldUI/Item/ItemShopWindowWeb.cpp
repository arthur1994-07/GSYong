#include "StdAfx.h"
#include "ItemShopWindowWeb.h"

#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/Common/SubPath.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../../EngineLib/Common/CommonWeb.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CItemShopWindowWeb::CItemShopWindowWeb( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CWebWindowBase( pInterface, pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
{
	EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

	if (sp == SP_CHINA)
	{
//#if defined( CH_PARAM ) 
		SetAddress( RANPARAM::China_Region[ RANPARAM::nChinaRegion ].strItemShop );
	}
	else
	{
//#else
		SetAddress( RANPARAM::ItemShopAddress );
	}
//#endif
}

VOID CItemShopWindowWeb::SetVisibleSingle( BOOL bVisible )
{
    m_pGaeaClient->GetCharacter()->ReqItemShopOpen( bVisible ? true : false );

    CWebWindowBase::SetVisibleSingle( bVisible );
}
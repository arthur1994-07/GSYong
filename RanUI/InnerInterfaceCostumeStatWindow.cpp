#include "stdafx.h"

#include "OldUI/CostumeStat/CotStatWindow.h"
#include "./InnerInterface.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"

void CInnerInterface::OpenCostumeStatWindow(const SINVENITEM* itemSrc, const SINVENITEM* itemDest)
{
	if( itemSrc == NULL || itemDest == NULL ) 
	{
		if( m_pCostumeStatWindow ) m_pCostumeStatWindow->CloseWindow();

		UiHideGroup ( COSTUME_STATS_WINDOW );

		m_pGaeaClient->GetCharacter()->CloseCostumeStat();
		return;
	}

	if( m_pCostumeStatWindow )
	{
		if( m_pCostumeStatWindow->SetItemInfo( itemSrc, itemDest ) )
		{
			m_pCostumeStatWindow->SetVisibleSingle(TRUE);
			UiShowGroupFocus( COSTUME_STATS_WINDOW );
			m_pGaeaClient->GetCharacter()->OpenCostumeStat();
		}
		
		//m_pCostumeStatWindow->SetItemInfo( itemSrc, itemDest );
		
	}
}

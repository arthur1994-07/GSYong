#include "stdafx.h"
#include "./InnerInterface.h"

#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"

#include "./OldUI/Item/ItemPreviewWindow.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::ItemPreviewWindowOpen( const SITEMCUSTOM& sItemCustom )
{ 
    if ( !m_pItemPreviewWindow || !m_pGaeaClient )
    {
        return;
    }

    if ( !UiIsVisibleGroup ( ITEMPREVIEW_WINDOW ) )
    {
        m_pItemPreviewWindow->LoadObjectData ( m_pGaeaClient->GetCharacter() );
        UiShowGroupFocus ( ITEMPREVIEW_WINDOW );
    }

    m_pItemPreviewWindow->SetItem( sItemCustom );
}

const BOOL CInnerInterface::ItemPreviewWindowIsPreviewItem ( const SITEMCUSTOM& sItemCustom )
{
    if ( !m_pItemPreviewWindow || !m_pGaeaClient )
    {
        return FALSE;
    }

    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);

    return m_pItemPreviewWindow->IsPreviewItem( sItemCustom, emIndex );
}

const DWORD CInnerInterface::ItemPreviewWindowItem_2_Slot( const SITEMCUSTOM& sItemCustom )
{ 
    if ( !m_pItemPreviewWindow || !m_pGaeaClient )
    {
        return SLOT_TSIZE;
    }

   return (DWORD) m_pItemPreviewWindow->Item_2_Slot( sItemCustom );
}

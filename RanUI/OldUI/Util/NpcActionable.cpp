#include "StdAfx.h"

#include "../../../Enginelib/G-Logic/GLDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Crow/GLCrowClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"

#include "NpcActionable.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIControlNpcActionable::CUIControlNpcActionable()
: m_sNPCID(SNATIVEID(false))
, m_dwNPCGlobalID(EMTARGET_NULL)
, m_bRemoteOpen(false)
{
    //Blank
}

CUIControlNpcActionable::~CUIControlNpcActionable()
{
    //Blank
}

const bool CUIControlNpcActionable::IsUseable ( GLGaeaClient* pGaeaClient )
{
    if ( !pGaeaClient )
    {
        return false;
    }

    if ( !m_bRemoteOpen )
    {
        GLLandManClient* pLandMan = pGaeaClient->GetActiveMap();

        if ( !pLandMan )
        {
            return false;
        }

        std::tr1::shared_ptr<GLCrowClient> pCrow = pLandMan->GetCrow( m_dwNPCGlobalID );

        if ( !pCrow )
        {
            return false;
        }

        else if ( !pCrow->IsNpcActionable(pGaeaClient->GetCharacter()->GetPosition(), pGaeaClient->GetCharacter()->GETBODYRADIUS() ) )
        {
            return false;
        }
    }

    return true;
}

void CUIControlNpcActionable::SetNpcActionableNPCID( const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID )
{
    m_sNPCID = sNPCID;
    m_dwNPCGlobalID = dwNPCGlobalID;
    m_bRemoteOpen = false;
}

void CUIControlNpcActionable::SetNpcActionableRemote()
{
    m_sNPCID = SNATIVEID(false);
    m_dwNPCGlobalID = EMTARGET_NULL;
    m_bRemoteOpen = true;
}

#include "pch.h"

#include "../../DxTools/DxVertexFVF.h"
#include "./LightMapObject.h"

#include "LightMapObjectVEC.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SVEC_LIGHTMAPOBJ::SVEC_LIGHTMAPOBJ()
{
}

SVEC_LIGHTMAPOBJ::~SVEC_LIGHTMAPOBJ()
{
}

WORDWORD SVEC_LIGHTMAPOBJ::GetStartTexel()
{
    WORDWORD wwBaseTexel(4095,4095);
    for( DWORD i=0; i<m_vecLightMapObject.size(); ++i )
    {
        if ( i == 0 )
        {
            wwBaseTexel = m_vecLightMapObject[i]->GetBaseTexelXY();
        }
        else
        {
            if ( wwBaseTexel.wA > m_vecLightMapObject[i]->GetBaseTexelXY().wA )
                wwBaseTexel.wA = m_vecLightMapObject[i]->GetBaseTexelXY().wA;
            if ( wwBaseTexel.wB > m_vecLightMapObject[i]->GetBaseTexelXY().wB )
                wwBaseTexel.wB = m_vecLightMapObject[i]->GetBaseTexelXY().wB;
        }
    }

    return wwBaseTexel;
}

void SVEC_LIGHTMAPOBJ::DeleteTexelColor()
{
    for( DWORD i=0; i<m_vecLightMapObject.size(); ++i )
    {
        m_vecLightMapObject[i]->DeleteTexelColor();
    }
}
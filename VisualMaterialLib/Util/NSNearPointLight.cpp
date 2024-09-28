#include "stdafx.h"

#include "NSNearPointLight.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSNearPointLight
{
    typedef std::vector<PointLightData>     VEC_POINT_LIGHT_DATA;
    typedef VEC_POINT_LIGHT_DATA::iterator  VEC_POINT_LIGHT_DATA_ITER;

    VEC_POINT_LIGHT_DATA    g_vecPointLightData;
    VEC_POINT_LIGHT_DATA    g_vecPointLightData_Main;

    void InsertPointLight( const D3DLIGHTQ* pLight, float fOverLighting )
    {
        g_vecPointLightData.push_back( PointLightData( pLight->Diffuse, 
                                                        pLight->Position, 
                                                        pLight->Range, 
                                                        pLight->Attenuation0,
                                                        pLight->Attenuation1,
                                                        pLight->Attenuation2,
														fOverLighting ) );
    }

    DWORD GetPointLightDataSize()
    {
        return static_cast< DWORD >( g_vecPointLightData.size() );
    }

    const PointLightData* GetPointLightData( DWORD dwIndex )
    {
        return &g_vecPointLightData[dwIndex];
    }

    void CleanUp()
    {
        g_vecPointLightData.clear();
    }
};
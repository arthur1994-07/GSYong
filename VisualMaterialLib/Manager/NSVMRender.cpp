#include "stdafx.h"

#include <boost/tr1/memory.hpp>

#include "../Util/DxTangentMesh.h"

#include "NSVMRender.h"

// ----------------------------------------------------------------------------
#include "../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


namespace NSVMRender
{
	DxVMStack g_sTranslucent;	// 반투명 데이터를 기록해 놓는다.
	DxVMStack g_sStarGlow;		// StarGlow 데이터를 기록해 놓는다.

	void DxVMStack::Insert( RENDER_TYPE emLightMapMode,
							BYTE* pPointer, 
							DxTangentMesh* pTangentMesh, 
							DWORD dwAttrib, 
							BOOL bRealPL, 
							const D3DXMATRIX& matWorld,
							LPDIRECT3DTEXTUREQ pTextureDay, 
							LPDIRECT3DTEXTUREQ pTextureNight,
							LPDIRECT3DTEXTUREQ pTextureAvgDay,
							LPDIRECT3DTEXTUREQ pTextureAvgNight,
							const D3DXVECTOR2& vLightMapUV_Offset,
							float fAlpha )
    {
        std::tr1::shared_ptr<DATA> spNew = std::tr1::shared_ptr<DATA>( new DATA( emLightMapMode,
																			pPointer,
                                                                            pTangentMesh,
                                                                            dwAttrib,
                                                                            bRealPL,
                                                                            matWorld,
                                                                            pTextureDay,
                                                                            pTextureNight,
                                                                            pTextureAvgDay,
                                                                            pTextureAvgNight,
                                                                            vLightMapUV_Offset,
                                                                            fAlpha ) );
        g_vecTranslucentData.push_back( spNew );
    }

    void DxVMStack::CleanUp()
    {
        g_vecTranslucentData.clear();
    }

    // Translucent 될 수 있는 갯수
    DWORD DxVMStack::GetTranslucentNum()
    {
        return static_cast< DWORD >( g_vecTranslucentData.size() );
    }

    // Translucent Data 를 가져온다.
    const DATA* DxVMStack::GetTranslucentData( size_t i )
    {
        if ( i > g_vecTranslucentData.size() )
            return NULL;

        return g_vecTranslucentData[i].get();
    }
};
#pragma once

namespace NSNearPointLight
{
    struct PointLightData
    {
        D3DCOLORVALUE   m_cDiffuse;
        D3DVECTOR       m_vPosition;
        float           m_fRange;
        D3DXVECTOR3     m_vAttenuation;
		float			m_fOverLighting;

        PointLightData( D3DCOLORVALUE cDiffuse, 
                        D3DVECTOR vPosition, 
                        float fRange,
                        float fAttenuation0,
                        float fAttenuation1,
                        float fAttenuation2,
						float fOverLighting )
            : m_cDiffuse(cDiffuse)
            , m_vPosition(vPosition)
            , m_fRange(fRange)
            , m_vAttenuation(fAttenuation0,fAttenuation1,fAttenuation2)
			, m_fOverLighting(fOverLighting)
        {
        }
    };

    // PointLight를 셋팅한다.
    void InsertPointLight( const D3DLIGHTQ* pLight, float fOverLighting );

    // PointLight 갯수를 넘겨준다.
    DWORD GetPointLightDataSize();

    // PointLight 정보를 넘겨준다.
    const PointLightData* GetPointLightData( DWORD dwIndex );

    // 새로운 형상의 PointLight를 위해 정보를 지운다.
    void CleanUp();
};
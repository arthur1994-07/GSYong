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

    // PointLight�� �����Ѵ�.
    void InsertPointLight( const D3DLIGHTQ* pLight, float fOverLighting );

    // PointLight ������ �Ѱ��ش�.
    DWORD GetPointLightDataSize();

    // PointLight ������ �Ѱ��ش�.
    const PointLightData* GetPointLightData( DWORD dwIndex );

    // ���ο� ������ PointLight�� ���� ������ �����.
    void CleanUp();
};
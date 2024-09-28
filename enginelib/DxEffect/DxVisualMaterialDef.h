#pragma once

struct ParameterData
{
    BOOL        m_bNight;
    D3DXVECTOR3 m_vParameter;
    D3DXVECTOR3 m_vNight;
};
typedef std::map<TSTRING,ParameterData>             MAP_PARAMETER_NAME_DATA2;
typedef MAP_PARAMETER_NAME_DATA2::iterator          MAP_PARAMETER_NAME_DATA2_ITER;
typedef MAP_PARAMETER_NAME_DATA2::const_iterator    MAP_PARAMETER_NAME_DATA2_CITER;

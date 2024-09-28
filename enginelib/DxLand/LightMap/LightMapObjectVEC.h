#pragma once

#include "./LightMapDef.h"

struct WORDWORD;


class SVEC_LIGHTMAPOBJ
{
public:
    VEC_LIGHTMAPOBJ m_vecLightMapObject;
    DWORD	        m_dwLightMapFaceNUM;

public:
    WORDWORD GetStartTexel();
    void DeleteTexelColor();

public:
    SVEC_LIGHTMAPOBJ();
    ~SVEC_LIGHTMAPOBJ();
};
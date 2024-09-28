#pragma once

#include "../../RanThirdParty/DirectX/Include/d3dx9math.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"

//! Sigma Engine
namespace se
{
    struct Vector2 : public D3DXVECTOR2
    {    
        Vector2() {}

        Vector2(CONST FLOAT* pf)
            : D3DXVECTOR2(pf)
        {
        }

        Vector2(CONST D3DXFLOAT16* pf)
            : D3DXVECTOR2(pf)
        {
        }

        Vector2(FLOAT x, FLOAT y)
            : D3DXVECTOR2(x, y)
        {
        }

        MSGPACK_DEFINE(x, y);
    };

    struct Vector3 : public D3DXVECTOR3
    {
        Vector3() {}
        Vector3(CONST FLOAT* pf)
            : D3DXVECTOR3(pf)
        {
        }

        Vector3(CONST D3DVECTOR& v)
            : D3DXVECTOR3(v)
        {
        }

        Vector3(CONST D3DXFLOAT16* pf)
            : D3DXVECTOR3(pf)
        {
        }

        Vector3(FLOAT fx, FLOAT fy, FLOAT fz)
            : D3DXVECTOR3(fx, fy, fz)
        {
        }

        MSGPACK_DEFINE(x, y, z);
    };    

    struct Vector4 : public D3DXVECTOR4
    {
        Vector4() {}
        Vector4(CONST FLOAT* pf)
            : D3DXVECTOR4(pf)
        {
        }

        Vector4(CONST D3DXFLOAT16* pf)
            : D3DXVECTOR4(pf)
        {
        }

        Vector4(CONST D3DVECTOR& xyz, FLOAT w)
            : D3DXVECTOR4(xyz, w)
        {
        }

        Vector4(FLOAT x, FLOAT y, FLOAT z, FLOAT w)
            : D3DXVECTOR4(x, y, z, w)
        {
        }
    };
} // namespace se

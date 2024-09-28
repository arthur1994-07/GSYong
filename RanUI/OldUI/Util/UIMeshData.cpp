#include "stdafx.h"

#include "./UIMeshData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IUIMeshData::IUIMeshData () :
    m_pd3dDevice (NULL)
{
    D3DXMatrixIdentity(&m_matWorld);
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);
}

IUIMeshData::~IUIMeshData ()
{
    // blank
}

void IUIMeshData::SetPosition ( const D3DXVECTOR3& vPos )
{
    m_matWorld._41 = vPos.x;
    m_matWorld._42 = vPos.y;
    m_matWorld._43 = vPos.z;
}
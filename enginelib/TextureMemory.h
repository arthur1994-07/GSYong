/**
 * \date	2010/11/12
 * \author  Jun-Hyuk Choi
 */
#pragma once

#include "../InternalCommonLib/Define/TextureAddData.h"
#include "../InternalCommonLib/dxstdafx.h"

class CD3DFontPar;

namespace TextureMemory
{
    extern bool g_bEnable;

    VOID Reset();
    VOID CheckLeak();
    VOID SetMemoryForce( DWORD dwBytes, TEXTURE_CATEGORY emCategory );

    VOID OnCreate( LPDIRECT3DBASETEXTURE9 pTexture, TEXTURE_CATEGORY emCategory );
    VOID OnRelease( LPDIRECT3DBASETEXTURE9 pTexture, TEXTURE_CATEGORY emCategory );
    DWORD GetTextureBytes( LPDIRECT3DBASETEXTURE9 pTexture );

    VOID Display( CD3DFontPar* pFont, LPDIRECT3DDEVICEQ pd3dDevice );
}
#include "pch.h"
#include "./TextureMemory.h"

#include "./DxTools/DebugSet.h"
#include "./DxTools/DxFontMan.h"

#include "../SigmaCore/DebugInclude.h"

namespace
{
    const DWORD c_dwFontColor = D3DCOLOR_ARGB( 255, 255, 0, 255 );
    const std::string c_strIdentity[ TEXTURE_CATEGORY_SIZE ] =
    {
        _T( "TEXTURE_SKYBOX" ),
        _T( "TEXTURE_CHAR" ),
        _T( "TEXTURE_EFFECT" ),
        _T( "TEXTURE_LAND" ),
        _T( "TEXTURE_SIMPLEMESH" ),
        _T( "TEXTURE_ETC" ),
        _T( "TEXTURE_VISUALMATERIAL" ),
        _T( "TEXTURE_UI" ),
		_T( "TEXTURE_MATERIALS" ),
    };

    UINT GetBitsPerTexel( D3DFORMAT emFormat );
}

namespace TextureMemory
{
    bool g_bEnable = true;
    static DWORD s_nMemoryBytes[ TEXTURE_CATEGORY_SIZE ];

    VOID Reset()
    {
#ifdef _RELEASED
        for( INT i=0; i<TEXTURE_CATEGORY_SIZE; ++i )
            s_nMemoryBytes[ i ] = 0;
#endif
    }

    VOID CheckLeak()
    {
#ifdef _RELEASED
        for( INT i=0; i<TEXTURE_CATEGORY_SIZE; ++i )
        {
            if( s_nMemoryBytes[ i ] <= 0 )
                continue;

            std::string strError = _T( "ERROR : TextureMemory::CheckLeak, " );
            strError.append( c_strIdentity[ i ] );
            CDebugSet::ToLogFile( strError );

            strError.append( _T( "\n" ) );
            OutputDebugString( strError.c_str() );
        }
#endif
    }

    VOID SetMemoryForce( DWORD dwBytes, TEXTURE_CATEGORY emCategory )
    {
#ifdef _RELEASED
        if( !g_bEnable )
            return;

        if( emCategory >= TEXTURE_CATEGORY_SIZE )
            return;

        s_nMemoryBytes[ emCategory ] = dwBytes;
#endif
    }

	//std::set<TSTRING> g_setName;

    VOID OnCreate( LPDIRECT3DBASETEXTURE9 pTexture, TEXTURE_CATEGORY emCategory )
    {
#ifdef _RELEASED
        if( !g_bEnable )
            return;

        if( emCategory >= TEXTURE_CATEGORY_SIZE )
            return;

        s_nMemoryBytes[ emCategory ] += GetTextureBytes( pTexture );
#endif
    }

    VOID OnRelease( LPDIRECT3DBASETEXTURE9 pTexture, TEXTURE_CATEGORY emCategory )
    {
#ifdef _RELEASED
        if( !g_bEnable )
            return;

        if( emCategory >= TEXTURE_CATEGORY_SIZE )
            return;

        DWORD nBytes = GetTextureBytes( pTexture );
        if( s_nMemoryBytes[ emCategory ] >= nBytes )
        {
            s_nMemoryBytes[ emCategory ] -= nBytes;
        }
        else
        {
            s_nMemoryBytes[ emCategory ] = 0;

            std::string strError = _T( "ERROR : TextureMemory::OnRelease, " );
            strError.append( c_strIdentity[ emCategory ] );
            CDebugSet::ToLogFile( strError );

            strError.append( _T( "\n" ) );
            OutputDebugString( strError.c_str() );
        }
#endif
    }

    DWORD GetTextureBytes( LPDIRECT3DBASETEXTURE9 pTexture )
    {
        if( !pTexture )
            return 0;

        DWORD nBytes = 0;
        DWORD nLevels = pTexture->GetLevelCount();
        D3DSURFACE_DESC sDesc;

        switch( pTexture->GetType() )
        {
        case D3DRTYPE_TEXTURE:
            {
                LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)pTexture;

                for( DWORD i=0; i<nLevels; ++i )
                {
                    pTex->GetLevelDesc( i, &sDesc );

                    UINT nBitsTexel = GetBitsPerTexel( sDesc.Format );
                    UINT nBitsMem = sDesc.Width * sDesc.Height * nBitsTexel;
                    nBytes += DWORD( nBitsMem / 8 );
                }
            }
            break;

        case D3DRTYPE_CUBETEXTURE:
            {
                LPDIRECT3DCUBETEXTURE9 pTex = (LPDIRECT3DCUBETEXTURE9)pTexture;

                for( DWORD i=0; i<nLevels; ++i )
                {
                    pTex->GetLevelDesc( i, &sDesc );

                    UINT nBitsTexel = GetBitsPerTexel( sDesc.Format );
                    UINT nBitsMem = sDesc.Width * sDesc.Height * nBitsTexel;
                    nBytes += DWORD( nBitsMem / 8 * 6 );
                }
            }
            break;
        }

        return nBytes;
    }

    VOID Display( CD3DFontPar* pFont, LPDIRECT3DDEVICEQ pd3dDevice )
    {
#ifdef _RELEASED
        if( !g_bEnable || !pFont || !pd3dDevice )
            return;

        FLOAT fPosX = 2.0f;
	    FLOAT fPosY = 62.0f;
	    CString strText;
        DWORD dwTotal = 0;

	    for( INT i=0; i<TEXTURE_CATEGORY_SIZE; ++i )
	    {
		    strText.Format(
			    _T( "%03d mb, %09d bytes, %s" ),
                s_nMemoryBytes[ i ] / 1024 / 1024,
                s_nMemoryBytes[ i ],
                c_strIdentity[ i ].c_str() );

		    pFont->DrawText( fPosX, fPosY, c_dwFontColor, strText.GetString(), 0, FALSE );
		    fPosY += 20.0f;

            dwTotal += s_nMemoryBytes[ i ];
	    }

        strText.Format(
            _T( "%03d mb, %09d bytes, TEXTURE_TOTAL" ),
            dwTotal / 1024 / 1024,
            dwTotal );

        pFont->DrawText( fPosX, fPosY, c_dwFontColor, strText.GetString(), 0, FALSE );
        fPosY += 20.0f;
#endif
    }
}

namespace
{
    UINT GetBitsPerTexel( D3DFORMAT emFormat )
    {
        struct FMTINFO
        {
            D3DFORMAT fmt;
            UINT nBitsPerTexel;
        };

        const FMTINFO fmtInfoArray[] =
        {
            D3DFMT_A4L4, 8,
            D3DFMT_A8, 8,
            D3DFMT_L8, 8,
            D3DFMT_P8, 8,
            D3DFMT_R3G3B2, 8,
            D3DFMT_A1R5G5B5, 16,
            D3DFMT_A4R4G4B4, 16,
            D3DFMT_A8L8, 16,
            D3DFMT_A8P8, 16,
            D3DFMT_A8R3G3B2, 16,
            D3DFMT_L16, 16,
            D3DFMT_R5G6B5, 16,
            D3DFMT_X1R5G5B5, 16,
            D3DFMT_X4R4G4B4, 16,
            D3DFMT_R8G8B8, 24,
            D3DFMT_A2B10G10R10, 32,
            D3DFMT_A2R10G10B10, 32,
            D3DFMT_A8B8G8R8, 32,
            D3DFMT_A8R8G8B8, 32,
            D3DFMT_G16R16, 32,
            D3DFMT_X8B8G8R8, 32,
            D3DFMT_X8R8G8B8, 32,
            D3DFMT_A16B16G16R16, 64,
            D3DFMT_CxV8U8, 16,
            D3DFMT_V8U8, 16,
            D3DFMT_Q8W8V8U8, 32,
            D3DFMT_V16U16, 32,
            D3DFMT_Q16W16V16U16, 64,
            D3DFMT_L6V5U5, 16,
            D3DFMT_A2W10V10U10, 32,
            D3DFMT_X8L8V8U8, 32,
            D3DFMT_DXT1, 4,
            D3DFMT_DXT2, 8,
            D3DFMT_DXT3, 8,
            D3DFMT_DXT4, 8,
            D3DFMT_DXT5, 8,
            D3DFMT_G8R8_G8B8, 16,
            D3DFMT_R8G8_B8G8, 16,
            D3DFMT_UYVY, 16,
            D3DFMT_YUY2, 16,
            D3DFMT_R16F, 16,
            D3DFMT_G16R16F, 32,
            D3DFMT_A16B16G16R16F, 64,
            D3DFMT_R32F, 32,
            D3DFMT_G32R32F, 64,
            D3DFMT_A32B32G32R32F, 128,
        };
        const UINT fmtInfoArraySize = sizeof( fmtInfoArray ) / sizeof( fmtInfoArray[ 0 ] );

        for( UINT i=0; i<fmtInfoArraySize; ++i )
        {
            if( fmtInfoArray[ i ].fmt == emFormat )
                return fmtInfoArray[ i ].nBitsPerTexel;
        }

        return 0;
    }
}
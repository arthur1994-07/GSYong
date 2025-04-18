#include "pch.h"
#include "FreetypeLib.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_BITMAP_H
#include FT_SYNTHESIS_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_CACHE_H
#include FT_CACHE_CHARMAP_H
#include FT_CACHE_IMAGE_H

#include "../DxTools/d3dfont.h"
#include "../DxTools/DxFontMan.h"
#include "../TextTexture/TextUtil.h"
#include "../../SigmaCore/gassert.h"

#include "internal.hpp"

bool FreetypeLib::InitFreetype(int lang)
{
	if ( FT_FAILED(FT_Init_FreeType(&s_pLibrary)) )
	{		
		return false;
	}

    if ( FT_FAILED(FTC_Manager_New(s_pLibrary, 0, 0, 0, &Face_Requester, 0, &s_pManager)) )
    {        
        return false;
    }

    if ( FT_FAILED(FTC_ImageCache_New(s_pManager, &s_pImageCache)) )
    {        
        return false;
    }

    if ( FT_FAILED(FTC_CMapCache_New(s_pManager, &s_pCMapCache)) )
    {        
        return false;
    }

    if ( FT_FAILED(FTC_SBitCache_New(s_pManager, &s_pSBitCache)) )
    {        
        return false;
    }

    s_pScaler = new FTC_ScalerRec;

    if (s_pScaler == 0)
    {        
        return false;
    }

    HDC hDc = CreateCompatibleDC(0);

    if (hDc == 0)
    {        
        return false;
    }

    s_pScaler->face_id  = reinterpret_cast<FTC_FaceID>(lang);
    s_pScaler->x_res    = GetDeviceCaps(hDc, LOGPIXELSX);
    s_pScaler->y_res    = GetDeviceCaps(hDc, LOGPIXELSX);
    s_pScaler->width    = 0;
    s_pScaler->height   = 0;
    s_pScaler->pixel    = 0;

    DeleteDC(hDc);

    if ( FT_FAILED(FTC_Manager_LookupFace(s_pManager, s_pScaler->face_id, &s_pFace)) )
    {        
        return false;
    }

	return true;
}

void FreetypeLib::DestroyFreetype()
{
    SAFE_DELETE(s_pScaler);

	if ( s_pManager )
	{
		FTC_Manager_Done(s_pManager);
		s_pManager = NULL;
	}

	if ( s_pLibrary )
	{
		FT_Done_FreeType(s_pLibrary);
		s_pLibrary = NULL;
	}
}

bool FreetypeLib::GetExtentText(const wchar_t* pStr, int nFontSize, int& nWidth, int& nHeight)
{
    int ascender = SetFontSize(nFontSize);

    FT_UInt unPrv = 0, unCur = 0;

	while (*pStr)
	{
        int nKerning = 0;

        if (LoadGlyph(*pStr, unCur, unPrv, nKerning))
        {
            if (RenderGlyph(unCur))
            {
                nWidth  += nKerning;
                nWidth	+= s_pSbit->xadvance;
                nHeight = std::max(ascender + (s_pSbit->height - s_pSbit->top), nHeight);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

		unPrv = unCur;
		++pStr;
	}

	return true;
}

bool FreetypeLib::CopyBitmap(const wchar_t* pStr, int nFontSize, void* pBuff, BYTE byBuffSize, RECT rt, int nTSize, DWORD dwFlags)
{
    BYTE* s = static_cast<BYTE*>(pBuff) + (rt.left + rt.top * nTSize) * byBuffSize;
    DWORD w = (rt.right - rt.left) * byBuffSize;

    for (int h = rt.top; h < rt.bottom; ++h, s += nTSize * byBuffSize)
    {
        // 영역 초기화
        memset(s, 0, w);
    }

    SetFontSize(nFontSize);

    FT_UInt unPrv = 0, unCur = 0;

	while (*pStr)
	{
        int nKerning = 0;

        if (LoadGlyph(*pStr, unCur, unPrv, nKerning))
        {
            if (byBuffSize == 2)
            {
                SetFontEffect(rt, static_cast<WORD*>(pBuff), nTSize, dwFlags, unCur);
                DrawChar(static_cast<WORD*>(pBuff), s_pSbit, rt, nTSize, static_cast<WORD>(0xffff));
            }
            else if (byBuffSize == 4)
            {
                SetFontEffect(rt, static_cast<DWORD*>(pBuff), nTSize, dwFlags, unCur);
                DrawChar(static_cast<DWORD*>(pBuff), s_pSbit, rt, nTSize, static_cast<DWORD>(0xffffffff));
            }

            rt.left += nKerning;
            rt.left += s_pSbit->xadvance;
            rt.top  += s_pSbit->yadvance;
        }
        else
        {
            return false;
        }

		unPrv = unCur;
		++pStr;
	}

	return true;
}
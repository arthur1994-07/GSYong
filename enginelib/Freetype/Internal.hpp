#define	        SET_26DOT6(x)	((x) << 6)
#define	        GET_26DOT6(x)	((x) >> 6)
#define	        FT_FAILED(x)	((x) != 0)

namespace FreetypeLib
{

FT_Library      s_pLibrary	    = 0;
FT_Face	        s_pFace		    = 0;
FTC_Manager		s_pManager      = 0;
FTC_ImageCache	s_pImageCache   = 0;
FTC_CMapCache	s_pCMapCache    = 0;
FTC_SBitCache	s_pSBitCache    = 0;
FTC_Scaler      s_pScaler       = 0;
FTC_SBit        s_pSbit         = 0;

/*	<프리타입 내부 함수>

	프리타입은 DC 의 폰트와는 달리 폰트 파일명을 알아야 한다.
	이에 나라코드에 따라 폰트파일을 입력 한다.

	현재 한국만 입력 되어 있다.
    현재는 코드에 박혀 있지만 향후 설정 파일로 빼도록 해야 한다.
*/
void SetFontInfo(int lang, TCHAR*& aFontFile, int& nFamilyIndex)
{
    switch(lang)
    {
    case language::DEFAULT:
        break;
    case language::TRADITIONAL_CHINESE:
        break;
    case language::JAPANESE:
        break;
    case language::KOREAN:
        aFontFile = _T("gulim.ttc");
        nFamilyIndex = 3;
        break;
    case language::SIMPLIFIED_CHINESE:
        break;
    case language::THAILAND:
        break;
    case language::VIETNAM:
        break;
    }
}

/** <프리타입 내부 함수>

    서브 케쉬 시스템에 의해 페이스가 생성 될 때 콜백 되는 함수이다.
    외부로 부터 FTC_FaceID 를 전달 받아 이것으로 생성 할 수 있다.

    우리는 언어 타입을 받지만 이것을 구조체의 주소 등으로 받아
    다양하게 처리할 수도 있다.

    넘겨 받은 페이스(aface)는 캐쉬 되므로 삭제하면 안되는 점에 주의해야 한다.
 */
FT_Error Face_Requester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface)
{
    int nFamilyIndex = 0;
    TCHAR* aFontFile = 0;

    SetFontInfo((int)face_id, aFontFile, nFamilyIndex);

    if (aFontFile == 0)
    {
        return true;
    }

    TCHAR aWndFolder[MAX_PATH] = {0};
    TCHAR aFontsPath[MAX_PATH] = {0};
    GetWindowsDirectory(aWndFolder,MAX_PATH);
    _stprintf_s(aFontsPath, MAX_PATH, _T("%s%s%s"), aWndFolder, _T("\\fonts\\"), aFontFile);

    return FT_New_Face(library, aFontsPath, nFamilyIndex, aface);
}

/** <프리타입 내부 함수>
	
    문자의 사이즈를 설정 한다.
 */
int SetFontSize(int nFontSize)
{   
    FT_Size ftSize;
    int ascender = 0;

    s_pScaler->width = SET_26DOT6(nFontSize);
    s_pScaler->height = 0;    

    if ( FT_FAILED(FTC_Manager_LookupSize(s_pManager, s_pScaler, &ftSize)) )
    {        
        return ascender;
    }

    if (FT_IS_SCALABLE(s_pFace))
    {
        float y_scale = s_pFace->size->metrics.y_scale * 1.0f/64.0f * (1.0f/65536.0f);
        ascender = int(s_pFace->ascender * y_scale);

    }
    else
    {
        ascender = int(s_pFace->size->metrics.ascender * 1.0f/64.0f);
    }

    return ascender;
}

/** <프리타입 내부 함수>

	문자의 정보(Glyph) 를 로딩 한다.
 */
bool LoadGlyph(const wchar_t charcode, FT_UInt& unCur, FT_UInt& unPrv, int& nKerning)
{
    unCur = FTC_CMapCache_Lookup(s_pCMapCache, s_pScaler->face_id, 0, charcode);

    if (unCur == 0)
    {        
        return false;
    }

    if (unPrv != 0 && FT_HAS_KERNING(s_pFace))
    {
        FT_Vector sKerning;
        FT_Get_Kerning(s_pFace, unPrv, unCur, FT_KERNING_DEFAULT, &sKerning);
        nKerning = GET_26DOT6(sKerning.x);
    }

    FT_Glyph glyph = 0;

    if ( FT_FAILED(FTC_ImageCache_LookupScaler(s_pImageCache, s_pScaler, FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT, unCur, &glyph, 0)) )
    {        
        return false;
    }

    return true;
}

/** <프리타입 내부 함수>
	
    문자를 비트맵으로 랜더링 한다.
 */
bool RenderGlyph(FT_UInt unCur)
{
    if ( FT_FAILED(FTC_SBitCache_LookupScaler(s_pSBitCache, s_pScaler, FT_RENDER_MODE_NORMAL, unCur, &s_pSbit, 0)) )
    {        
        return false;
    }

    return true;
}

/*	<프리타입 내부 함수>

	프리타입의 비트맵 정보를 버퍼(텍스처 등)에 복사하는 함수	
*/
template<typename T>
void DrawChar(T* pOut, FTC_SBit sbit, RECT& rt, int nTSize, T color = 0)
{
	BYTE* pBuffer	= sbit->buffer;
	BYTE byMode		= sbit->format;
	int nWidth		= sbit->width;
	int nHeight		= sbit->height;
	int nPitch		= sbit->pitch;
	BYTE byAlpha	= 0;
	int ascender	= 0;

	if (FT_IS_SCALABLE(s_pFace))
	{
		float y_scale = s_pFace->size->metrics.y_scale * 1.0f / 64.0f * (1.0f / 65536.0f);
		ascender = int(s_pFace->ascender * y_scale);
	}
	else
	{
		ascender = int(s_pFace->size->metrics.ascender * 1.0f / 64.0f);
	}

    BYTE byShiftBitSize = sizeof(T) * 2 * 3;
    BYTE byPerColoBitSize = sizeof(T) * 2;
    BYTE byRem = 0;

    for (int i = 0; i < byPerColoBitSize; ++i)
    {
        byRem |= 1 << i;
    }

    byRem = 0xff / byRem;

	for (int h = 0; h < nHeight; ++h)
	{
		for (int w = 0; w < nWidth; ++w)
		{
			if (byMode == FT_PIXEL_MODE_MONO)
			{
				/* 
					 (w >> 3) = w / 8 로써 8개당 1바이씩 이동하기 위함
					 (w  & 7) = 0x80 을 오른쪽으로 이동 하기 위함
					 00000000 <- 8비트가 있다면 오른쪽에서 왼쪽으로 검색해야 한다.
					 그러므로 0x80 >> (w & 7) 이 된다.
				 */
				byAlpha = pBuffer[h * nPitch + (w >> 3)] & (0x80 >> (w & 7)) ? 0xff : 0;                
			}
			else
			{
				byAlpha = pBuffer[h * nPitch + w];				
			}

            if (byAlpha == 0)
            {
                continue;
            }

            T alpha = (byAlpha / byRem) << byShiftBitSize;

			/*
				CTextUtil::m_iSpaceSizeHalf = 쉐도우 효과를 염두해둔 여분 공간 이다.
				s_pFace->glyph->bitmap_left = 수평 베어링이다.
				s_pFace->glyph->bitmap_top = 수직 베어링이다.
				ascender = 수직 최상 값이다. 이를 통해 글자들을 수직 정렬 시킨다.				
			*/
			int x	= rt.left + w + sbit->left + CTextUtil::m_iSpaceSizeHalf;
			int y	= nTSize * (rt.top + h + ascender - sbit->top + CTextUtil::m_iSpaceSizeHalf);

			/* 
				프리타입 비트맵 값은 0 ~ 최대 255까지의 음영값이다.
				이를 알파로 쓰면 0일 때 투명 255일때 불투명이 되게 한다.
			*/
			pOut[x + y] = alpha + color;
		}
	}
}

/*	<프리타입 내부 함수>

	폰트에 효과를 구현 한다.
	효과는 볼드, 이탤릭, 아웃라인, 쉐도우 효과이다.
*/
template<typename T>
bool SetFontEffect(RECT& rt, T* pBuff, int nTSize, DWORD eFlags, FT_UInt unCur)
{
	if (eFlags & D3DFONT_BOLD)
	{
		/* 5% 더 두껍게 */
		int THICKNESS = 5;
		FT_Outline_Embolden(&s_pFace->glyph->outline,  (s_pFace->size->metrics.x_ppem * THICKNESS / 100) * 64);
	}

	if (eFlags & D3DFONT_ITALIC)
	{
		/* 10도 기울임 */
		float ANGLE = 10;
		FT_Matrix matrix;
		double ang	= (ANGLE / 360.f) * D3DX_PI * 2.f;
		matrix.xx	= (FT_Fixed)(cos(ang) * 0x10000);
		matrix.xy	= (FT_Fixed)(sin(ang) * 0x10000);
		matrix.yx	= (FT_Fixed)(-sin(ang) * 0x10000);
		matrix.yy	= (FT_Fixed)(cos(ang) * 0x10000);
		FT_Outline_Transform(&s_pFace->glyph->outline, &matrix);
	}

    /* Outline 함수들(FT_Outline_Transform,...)은
 	   렌더 전에 설정해야 함에 주의 한다. */
    if ( RenderGlyph(unCur) == false )
    {        
        return false;
    }

	if (eFlags & D3DFONT_SHADOW)
	{
		/* 8방향 출력
		   아래 그림의 x 가 현재, 순서 대로 이동 출력 후 원위치로 이동
		    _________			
		   | 2  3  4 |
		   | 1  x  5 |
		   | 8  7  6 |
		    ---------
		*/
		rt.left -= 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.top -= 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.left += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.left += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.top += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.top += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.left -= 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.left -= 1;
        DrawChar(pBuff, s_pSbit, rt, nTSize);

		rt.left += 1;
		rt.top -= 1;
	}
	else if (eFlags & D3DFONT_SHADOW_EX)
	{
		/* 3방향 출력
		   아래 그림의 x 가 현재, 순서 대로 이동 출력 후 원위치로 이동
		    ________			
		   |        |
		   |   x  1 |
		   |   3  2 |
		    --------
		*/
		rt.left += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);
		rt.top += 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);
		rt.left -= 1;
		DrawChar(pBuff, s_pSbit, rt, nTSize);
		rt.top -= 1;
	}

	return true;
}

}; // end namespace
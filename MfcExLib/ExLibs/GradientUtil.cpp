#include "stdafx.h"
#include "GradientUtil.h"
#include "MemBitmap.h"

#define	IS_VALID_GRADIENT_TYPE(_nType)		( GFT_FIRST <= (_nType) && (_nType) < GFT_SIZE)

#define GFT_SINGLE_DIRECTION_FIRST			GFT_TOP_BOTTOM
#define GFT_SINGLE_DIRECTION_LAST			GFT_DIAG_TR_BL
#define GFT_CENTER_OUT_IN_FIRST				GFT_VERTICAL_CENTER_OUT
#define GFT_CENTER_OUT_IN_LAST				GFT_DIAG_DOWN_CENTER_OUT
#define GFT_FROM_CORNER_FIRST				GFT_FROM_CORNER_TL
#define GFT_FROM_CORNER_LAST				GFT_FROM_CORNER_BL

#define IS_GFT_BETWEEN(_nType, _t1, _t2)	( (_t1) <= (_nType) && (_nType) <= (_t2) )
#define IS_GFT_SINGLE_DIRECTION(_nType)		IS_GFT_BETWEEN((_nType), GFT_SINGLE_DIRECTION_FIRST, GFT_SINGLE_DIRECTION_LAST)
#define IS_GFT_CENTER_OUT_IN(_nType)		IS_GFT_BETWEEN((_nType), GFT_CENTER_OUT_IN_FIRST, GFT_CENTER_OUT_IN_LAST)
#define IS_GFT_FROM_CORNER(_nType)			IS_GFT_BETWEEN((_nType), GFT_FROM_CORNER_FIRST, GFT_FROM_CORNER_LAST)

#define IS_GFT_CENTER_OUT(_nType)			( GFT_VERTICAL_CENTER_OUT == (_nType) \
											|| GFT_HORIZONTAL_CENTER_OUT == (_nType) \
											|| GFT_DIAG_UP_CENTER_OUT == (_nType) \
											|| GFT_DIAG_DOWN_CENTER_OUT == (_nType) )

#define IS_GFT_LINEAR_DIRECTION(_nType)		IS_GFT_BETWEEN((_nType), GFT_SINGLE_DIRECTION_FIRST, GFT_CENTER_OUT_IN_LAST)

#define IS_GFT_FROM_CENTER(_nType)			( GFT_FROM_CENTER == (_nType) )

#define GET_GFT_CORNER_INDEX(_nType)		( (_nType) - GFT_FROM_CORNER_FIRST )

#define HEADER_FIRST_HALF_PERCENTAGE			41

#define ALPHA_SELECTION_FILL					70

#define IMPLEMENT_COLORREF_TO_16(cName)	\
	inline COLOR16 Get##cName##16Value(COLORREF cc)\
	{\
	return MAKEWORD(0x00, Get##cName##Value(cc));\
	}\
	\
	inline COLOR16 Get##cName##16Value(COLORREF c1, COLORREF c2)\
	{\
	return MAKEWORD(0x00, (Get##cName##Value(c1) + Get##cName##Value(c2))/2 );\
	}

IMPLEMENT_COLORREF_TO_16(R)
IMPLEMENT_COLORREF_TO_16(G)
IMPLEMENT_COLORREF_TO_16(B)

#define DECLARE_COLOR16(cName, cRef)	\
	const COLOR16 cName##_R = GetR16Value(cRef);\
	const COLOR16 cName##_G = GetG16Value(cRef);\
	const COLOR16 cName##_B = GetB16Value(cRef);

#define DECLARE_COLOR16_MID(cName, cRef1, cRef2)	\
	const COLOR16 cName##_R = GetR16Value(cRef1, cRef2);\
	const COLOR16 cName##_G = GetG16Value(cRef1, cRef2);\
	const COLOR16 cName##_B = GetB16Value(cRef1, cRef2);

static BOOL _gradient_fill_rect_single_direction(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	BOOL bRet = FALSE;
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	DECLARE_COLOR16_MID(cMid, c0, c1)
	TRIVERTEX vtx[4] = {
		{pRect->left,	pRect->top,		c0_R,	c0_G,	c0_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
		{pRect->left,	pRect->bottom,	cMid_R,	cMid_G,	cMid_B,	0}, 
		{pRect->right,	pRect->top,		cMid_R,	cMid_G,	cMid_B,	0}
	};
	ULONG meshIndices[] = {0, 1, 2, 0, 1, 3};

	switch (nGradientType)
	{
	case GFT_TOP_BOTTOM:
		bRet = GradientFill(hDC, vtx, 2, meshIndices, 1, GRADIENT_FILL_RECT_V);
		break;
	case GFT_LEFT_RIGHT:
		bRet = GradientFill(hDC, vtx, 2, meshIndices, 1, GRADIENT_FILL_RECT_H);
		break;
	case GFT_DIAG_TL_BR:
		bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		break;
	case GFT_DIAG_TR_BL:
		vtx[0].x = pRect->right;
		vtx[3].x = pRect->left;
		vtx[1].x = pRect->left;
		vtx[2].x = pRect->right;
		bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return bRet;
}

static BOOL _gradient_fill_rect_from_center(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	const int nMidX = pRect->left + (pRect->right - pRect->left)/2;
	const int nMidY = pRect->top + (pRect->bottom - pRect->top)/2;
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	TRIVERTEX vtx[5] = {
		{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
		{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
		{nMidX,			nMidY,			c0_R,	c0_G,	c0_B,	0},
	};

	ULONG meshIndices[] = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
	return GradientFill(hDC, vtx, 5, meshIndices, 4, GRADIENT_FILL_TRIANGLE);
}

static BOOL _gradient_fill_rect_from_corner(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	TRIVERTEX vtx[4] = {
		{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
		{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}
	};

	int nCorner = GET_GFT_CORNER_INDEX(nGradientType);
	vtx[nCorner].Red   = c0_R;
	vtx[nCorner].Green = c0_G;
	vtx[nCorner].Blue  = c0_B;

	ULONG meshIndices[] = {nCorner, (nCorner+1)%4, (nCorner+2)%4, nCorner, (nCorner+3)%4, (nCorner+2)%4};
	return GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
}

static BOOL _gradient_fill_rect_center_out(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	BOOL bRet = FALSE;
	RECT rect1 = *pRect;
	RECT rect2 = *pRect;;
	switch (nGradientType)
	{
	case GFT_VERTICAL_CENTER_OUT:
	case GFT_HORIZONTAL_CENTER_OUT:
		{
			const bool bHorizontal = GFT_VERTICAL_CENTER_OUT == nGradientType;
			nGradientType = bHorizontal ? GFT_TOP_BOTTOM : GFT_LEFT_RIGHT;

			if ( bHorizontal )
				rect1.bottom = rect2.top = pRect->top + (pRect->bottom - pRect->top)/2;
			else
				rect1.right = rect2.left = pRect->left + (pRect->right - pRect->left)/2;

			// Fill the two parts
			bRet = _gradient_fill_rect_single_direction(hDC, &rect1, c1, c0, nGradientType);
			bRet &= _gradient_fill_rect_single_direction(hDC, &rect2, c0, c1, nGradientType);
		}
		break;
	case GFT_DIAG_UP_CENTER_OUT:
	case GFT_DIAG_DOWN_CENTER_OUT:
		{
			const bool bDiagonalUp = GFT_DIAG_UP_CENTER_OUT == nGradientType;
			DECLARE_COLOR16(c0, c0)
			DECLARE_COLOR16(c1, c1)
			TRIVERTEX vtx[4] = {
				{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
				{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
				{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
				{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0}
			};
			const int nC1IndexBegin = bDiagonalUp ? 2 : 0;	// the index of vertices that should be filled in color c1
			for (int ii = nC1IndexBegin; ii < nC1IndexBegin + 2; ++ii)
			{
				vtx[ii].Red		= c0_R;
				vtx[ii].Green	= c0_G;
				vtx[ii].Blue	= c0_B;
			}
			ULONG meshIndices[] = {0, 2, 3, 2, 1, 3};
			if ( !bDiagonalUp )
			{
				meshIndices[2] = 1;	// change 3 to 1
				meshIndices[3] = 0;	// change 2 to 0
			}
			bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return bRet;
}

#define CHECK_CORRECT_GRADIENT_COLOR(_nGrad1, _nGrad2, _nGradientType, _c0, _c1)	\
{\
	if ( _nGrad1 == _nGradientType || _nGrad2 == _nGradientType )\
{\
	--_nGradientType;\
	COLORREF ctmp = _c0;\
	_c0 = _c1;\
	_c1 = ctmp;\
}\
}

BOOL FillGradient(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, int nGradientType /*= GFT_TOP_BOTTOM*/, BYTE byAlpha /*= 255*/)
{
#ifdef _DEBUG
	if ( !pRect || !IS_VALID_GRADIENT_TYPE(nGradientType) )
	{
		ASSERT(FALSE);
		return FALSE;
	}
#endif

	if (0 == byAlpha)
		return TRUE;

	BOOL bAlphaBlend = byAlpha < 255;
	BOOL bRet = FALSE;
	HDC hDrawDC = hDC;
	CRect rectDraw(pRect);

	CMemBitmap bitmapDrawer(CDC::FromHandle(hDC), rectDraw);

	int cx = rectDraw.Width();
	int cy = rectDraw.Height();

	hDrawDC = bitmapDrawer.GetMemDC().GetSafeHdc();

	CRect rectGradient(pRect);

	if (bAlphaBlend)
	{
		BLENDFUNCTION bf;
		bf.BlendFlags			= 0;
		bf.AlphaFormat			= 0;
		bf.BlendOp				= AC_SRC_OVER;
		bf.SourceConstantAlpha	= byAlpha;

		bitmapDrawer.SetBitBltInfo(bf);
	}

	if ( IS_GFT_FROM_CENTER(nGradientType) )
	{
		bRet = _gradient_fill_rect_from_center(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else if ( IS_GFT_FROM_CORNER(nGradientType) )
	{
		bRet = _gradient_fill_rect_from_corner(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else if ( IS_GFT_CENTER_OUT_IN(nGradientType) )
	{
		CHECK_CORRECT_GRADIENT_COLOR ( GFT_VERTICAL_CENTER_IN, GFT_HORIZONTAL_CENTER_IN, nGradientType, c1, c2);
		bRet = _gradient_fill_rect_center_out(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else
	{
		// must be single direction now.
		ASSERT( IS_GFT_SINGLE_DIRECTION(nGradientType) );
		CHECK_CORRECT_GRADIENT_COLOR ( GFT_BOTTOM_TOP, GFT_RIGHT_LEFT, nGradientType, c1, c2);
		bRet = _gradient_fill_rect_single_direction(hDrawDC, rectGradient, c1, c2, nGradientType);
	}

	return bRet;
}

void DrawSimpleRoundRectBorder( CDC* pDC, RECT rect, COLORREF clrLine, bool bForClipping )
{
	const int nOffset = 2;

	CPen penLine(PS_SOLID, 1, clrLine);
	CPen* pOldPen = pDC->SelectObject(&penLine);

	int nExtraOffset;
	if ( bForClipping )
	{
		nExtraOffset = 1;
		++rect.right;
		++rect.bottom;
	}
	else
	{
		nExtraOffset = 0;
	}

	// top border
	pDC->MoveTo(rect.left + nOffset, rect.top);
	pDC->LineTo(rect.right - nOffset - 1, rect.top);

	// top-right corner
	pDC->LineTo(rect.right - 1, rect.top + nOffset);

	// right border
	pDC->LineTo(rect.right - 1, rect.bottom - 1 - nOffset - nExtraOffset);

	// bottom-right corner
	pDC->LineTo(rect.right - nOffset - 1, rect.bottom - 1 - nExtraOffset);

	// bottom border
	pDC->LineTo(rect.left + nOffset + nExtraOffset, rect.bottom - 1);

	// bottom-left corner
	pDC->LineTo(rect.left, rect.bottom - 1 - nOffset - nExtraOffset);

	// left border
	pDC->LineTo(rect.left, rect.top + nOffset);

	// top-left corner
	pDC->LineTo(rect.left + nOffset, rect.top);

	pDC->SelectObject(pOldPen);
}

BOOL SimpleGradientRoundRect(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, COLORREF clrBorder, int nGradientType, BYTE byAlpha)
{
	if ( !pRect || !hDC || !IS_VALID_GRADIENT_TYPE(nGradientType) )
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Set the clip region.
	CDC* pDC = CDC::FromHandle(hDC);

	VERIFY( ::BeginPath(hDC) );

	DrawSimpleRoundRectBorder(pDC, *pRect, clrBorder, true);

	VERIFY( ::EndPath(hDC) );

	::SelectClipPath(hDC, RGN_COPY);

	BOOL bRet = FillGradient(hDC, pRect, c1, c2, nGradientType, byAlpha);

	if ( clrBorder != CLR_INVALID )
	{
		DrawSimpleRoundRectBorder(pDC, *pRect, clrBorder);
	}

	// Restore the clipping region
	::SelectClipRgn(hDC, NULL);

	return bRet;
}

void DrawRoundRectItem(CDC* pDC, CRect rect, COLORREF clrFillColor1, COLORREF clrFillColor2, COLORREF clrInnerBorder, COLORREF clrOuterBorder, BYTE byAlpha )
{
	CRect rectInner(rect);
	rectInner.DeflateRect(1, 1);
	SimpleGradientRoundRect(pDC->GetSafeHdc(), rectInner, clrFillColor1, clrFillColor2, clrInnerBorder, GFT_TOP_BOTTOM, byAlpha);
	DrawSimpleRoundRectBorder(pDC, rect, clrOuterBorder);
}
#ifndef GradientUtil_H
#define GradientUtil_H

// Reference: <Windows Graphics Programming Win32 GDI and DirectDraw> by FengYuan
// Chapter 9.7 GRADIENT FILLS

// Color for item
#define RGB_ITEM_SELECTED_FILL_COLOR1			RGB(235, 244, 254)
#define RGB_ITEM_SELECTED_FILL_COLOR2			RGB(207, 228, 254)

#define RGB_ITEM_SELFOCUS_FILL_COLOR1			RGB(220, 235, 252)
#define RGB_ITEM_SELFOCUS_FILL_COLOR2			RGB(193, 219, 252)
#define RGB_ITEM_SELFOCUS_OUTER_BORDER_COLOR	RGB(125, 162, 206)
#define RGB_ITEM_SELFOCUS_INNER_BORDER_COLOR	RGB(235, 244, 253)

#define RGB_ITEM_SELNOFOCUS_FILL_COLOR1			RGB(248, 248, 248)
#define RGB_ITEM_SELNOFOCUS_FILL_COLOR2			RGB(229, 229, 229)
#define RGB_ITEM_SELNOFOCUS_OUTER_BORDER_COLOR	RGB(217, 217, 217)
#define RGB_ITEM_SELNOFOCUS_INNER_BORDER_COLOR	RGB(240, 240, 240)

#define RGB_ITEM_HOT_FILL_COLOR1				RGB(250, 251, 253)
#define RGB_ITEM_HOT_FILL_COLOR2				RGB(235, 243, 253)
#define RGB_ITEM_HOT_OUTER_BORDER_COLOR			RGB(184, 214, 251)
#define RGB_ITEM_HOT_INNER_BORDER_COLOR			RGB(252, 243, 254)

enum
{
	// Type							// Gradient filling direction
	GFT_INVALID					= -1,
	GFT_FIRST					= 0,

	// Single direction
	GFT_TOP_BOTTOM = GFT_FIRST,		// Vertical, top-to-bottom direction
	GFT_BOTTOM_TOP,					// Vertical, bottom-to-top direction
	GFT_LEFT_RIGHT,					// Horizontal, left-to-right direction
	GFT_RIGHT_LEFT,					// Horizontal, right-to-left direction
	GFT_DIAG_TL_BR,					// Diagonal, from top-left to bottom-right
	GFT_DIAG_TR_BL,					// Diagonal, from top-right to bottom-left

	// Center out
	GFT_VERTICAL_CENTER_OUT,		// Vertical, from center to the two sides (top & bottom)
	GFT_VERTICAL_CENTER_IN,			// Vertical, from two sides (top & bottom) to the center
	GFT_HORIZONTAL_CENTER_OUT,		// Horizontal, from center to the two sides (left & right)
	GFT_HORIZONTAL_CENTER_IN,		// Horizontal, from two sides (left & right) to the center
	GFT_DIAG_UP_CENTER_OUT,			// Diagonal, from center to the two corners
	GFT_DIAG_DOWN_CENTER_OUT,		// Diagonal, from center to the two corners

	// From corner
	GFT_FROM_CORNER_TL,				// From top-left corner to the other 3 corners
	GFT_FROM_CORNER_TR,				// From top-right corner to the other 3 corners
	GFT_FROM_CORNER_BR,				// From bottom-right corner to the other 3 corners
	GFT_FROM_CORNER_BL,				// From bottom-right corner to the other 3 corners

	GFT_FROM_CENTER,				// From the center to the 4 corners outside.

	GFT_SIZE,
};

typedef struct
{
	COLORREF c1;
	COLORREF c2;
	COLORREF c3;
	COLORREF c4;

} COLORS4;

BOOL FillGradient(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, int nGradientType = GFT_TOP_BOTTOM, BYTE byAlpha = 255);
void DrawRoundRectItem(CDC* pDC, CRect rect, COLORREF clrFillColor1, COLORREF clrFillColor2, COLORREF clrInnerBorder, COLORREF clrOuterBorder, BYTE byAlpha = 255);
void DrawSimpleRoundRectBorder( CDC* pDC, RECT rect, COLORREF clrLine, bool bForClipping = false );


#endif
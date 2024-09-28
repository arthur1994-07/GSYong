#ifndef CCustomDrawHelper_H
#define CCustomDrawHelper_H

#include "GradientUtil.h"

class CCustomDrawHelper
{
public:
		CCustomDrawHelper();
virtual ~CCustomDrawHelper();

		void		DrawItemBackground(CDC* pDC, CRect rect, BYTE byAlpha = 255);
		void		DrawFocus(CDC* pDC, CRect rect, BYTE byAlpha = 255);

protected:
		bool		ShouldFillBackground()
		{
			bool bShouldFill = m_bSelected || m_bIsHotItem;
			return bShouldFill;
		}

public:
		bool		m_bWndHasFocus;
		bool		m_bFocusItem;
		bool		m_bSelected;
		bool		m_bIsHotItem;
		bool		m_bDrawBorderWhenFill;
		bool		m_bIsPressed;
		CRect		m_rcBounds;

		bool		m_bWndEnabled;
		HWND		m_hWnd;
};

#endif
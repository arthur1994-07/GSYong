// XTPOffice2003Theme.cpp : implementation of the CXTPOffice2003Theme class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
// (c)1998-2011 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

#include "Common/XTPVC80Helpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPHookManager.h"

#include "XTPControlButton.h"
#include "XTPControlPopup.h"
#include "XTPToolBar.h"
#include "XTPPopupBar.h"
#include "XTPDialogBar.h"
#include "XTPControlComboBox.h"
#include "XTPControls.h"
#include "XTPMessageBar.h"
#include "XTPOffice2003Theme.h"
#include "XTPStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CXTPOffice2003Theme

IMPLEMENT_DYNAMIC(CXTPOffice2003Theme, CXTPOfficeTheme)

CXTPOffice2003Theme::CXTPOffice2003Theme()
{
	m_iconsInfo.bUseFadedIcons = FALSE;
	m_iconsInfo.bIconsWithShadow = FALSE;

	m_grcLunaChecked.SetStandardValue(RGB(255, 213, 140), RGB(255, 173, 85));
	m_grcLunaPushed.SetStandardValue(RGB(254, 142, 75), RGB(255, 207, 139));
	m_grcLunaSelected.SetStandardValue(RGB(255, 242, 200), RGB(255, 212, 151));

	m_bThemedStatusBar = TRUE;
	m_bPlainSeparators = FALSE;

	m_bFlatGripper = FALSE;

	m_nToolbarSeparatorMargin = 4;
}

CXTPOffice2003Theme::~CXTPOffice2003Theme()
{

}

void CXTPOffice2003Theme::RefreshMetrics()
{
	CXTPOfficeTheme::RefreshMetrics();

	HWND hWnd = AfxGetMainWnd() ? AfxGetMainWnd()->GetSafeHwnd() : 0;
	m_themeStatusBar.OpenThemeData(hWnd, L"STATUS");

	m_clrMenuExpandedGripper.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_WINDOW), GetXtremeColor(XPCOLOR_MENUBAR_EXPANDED), 550), GetXtremeColor(XPCOLOR_MENUBAR_EXPANDED));

	m_clrCommandBar.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd), GetXtremeColor(COLOR_3DFACE));
	m_clrDockBar.SetStandardValue(GetXtremeColor(COLOR_3DFACE), XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 0xcd));

	m_clrTearOffGripper.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_FACE));
	m_clrToolbarShadow.SetStandardValue(GetXtremeColor(XPCOLOR_TOOLBAR_FACE));
	m_clrFloatingGripper.SetStandardValue(GetXtremeColor(COLOR_3DSHADOW));
	m_clrPopupControl.SetStandardValue(GetXtremeColor(XPCOLOR_MENUBAR_FACE),
		XTPColorManager()->LightColor(GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DFACE), 550));

	m_clrToolbarExpand.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(XPCOLOR_3DFACE), GetXtremeColor(COLOR_3DSHADOW), 51), GetXtremeColor(COLOR_3DSHADOW));

	m_bLunaTheme = FALSE;

	m_clrMenuExpand.SetStandardValue(GetXtremeColor(XPCOLOR_MENUBAR_FACE),
		XTPColorManager()->LightColor(GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DFACE), 550));

	m_clrStatusPane.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 70));


	m_arrColor[XPCOLOR_SPLITTER_FACE].SetStandardValue(XTPColorManager()->grcDockBar.clrDark);

	if (XTPColorManager()->IsLowResolution())
	{
		m_clrCommandBar.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		m_clrDockBar.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		m_clrPopupControl.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
		m_clrToolbarExpand.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	}

	switch (GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeRoyale:
	case xtpSystemThemeAero:
		{
			m_clrDockBar.SetStandardValue(RGB(158, 190, 245), RGB(196, 218, 250));
			m_clrToolbarShadow.SetStandardValue(RGB(59, 97, 156));
			m_clrCommandBar.SetStandardValue(RGB(221, 236, 254), RGB(129, 169, 226), 0.75f);

			m_clrFloatingGripper.SetStandardValue(RGB(42, 102, 201));
			m_clrPopupControl.SetStandardValue(RGB(227, 239, 255), RGB(147, 181, 231));
			m_clrToolbarExpand.SetStandardValue(RGB(117, 166, 241), RGB(0, 53, 145), 0.75f);
			m_clrMenuExpandedGripper.SetStandardValue(RGB(203, 221, 246), RGB(121, 161, 220));

			m_clrMenuExpand.SetStandardValue(0xFEEFE3, 0xE2A981);

			m_clrStatusPane.SetStandardValue(RGB(211, 211, 211));
			m_pShadowManager->SetShadowColor(RGB(0x16, 0x1E, 0x36));
			m_clrTearOffGripper.SetStandardValue(RGB(169, 199, 240));

			m_clrDisabledIcon.SetStandardValue(RGB(233, 236, 242), RGB(97, 122, 172));

			m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(RGB(203, 225, 252));
			m_arrColor[XPCOLOR_3DFACE].SetStandardValue(RGB(216, 231, 252));
			m_arrColor[XPCOLOR_3DSHADOW].SetStandardValue(RGB(158, 190, 245));
			m_arrColor[COLOR_APPWORKSPACE].SetStandardValue(RGB(144, 153, 174));
			m_arrColor[XPCOLOR_LABEL].SetStandardValue(RGB(189, 211, 247));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(39, 65, 118), RGB(106, 140, 203), RGB(109, 150, 208), RGB(246, 246, 246), RGB(0, 45, 150), RGB(255, 238, 194), RGB(0, 0, 128), RGB(0, 0, 128), RGB(0, 0, 128), RGB(254, 128, 62), RGB(255, 192, 111), RGB(196, 219, 249), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;
		}
		break;

	case xtpSystemThemeOlive:
		{
			m_clrDockBar.SetStandardValue(RGB(217, 217, 167), RGB(242, 241, 228));
			m_clrCommandBar.SetStandardValue(RGB(244, 247, 222), RGB(183, 198, 145), 0.3f);

			m_clrToolbarShadow.SetStandardValue(RGB(96, 128, 88));
			m_clrFloatingGripper.SetStandardValue(RGB(116, 134, 94));
			m_clrPopupControl.SetStandardValue(RGB(236, 240, 213), RGB(194, 206, 159));
			m_clrToolbarExpand.SetStandardValue(RGB(176, 194, 140), RGB(96, 119, 107), 0.3f);

			m_clrMenuExpandedGripper.SetStandardValue(RGB(230, 230, 209), RGB(164, 180, 120), 0.3f);

			m_clrMenuExpand.SetStandardValue(0xD5F0EC, 0x93C6B8);

			m_clrStatusPane.SetStandardValue(RGB(211, 211, 211));
			m_pShadowManager->SetShadowColor(RGB(0x1F, 0x22, 0x19));

			m_clrDisabledIcon.SetStandardValue(RGB(243, 244, 240), RGB(131, 144, 113));
			m_clrTearOffGripper.SetStandardValue(RGB(197, 212, 159));

			m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(RGB(216, 227, 182));
			m_arrColor[XPCOLOR_3DFACE].SetStandardValue(RGB(226, 231, 191));
			m_arrColor[XPCOLOR_3DSHADOW].SetStandardValue(RGB(171, 192, 138));
			m_arrColor[COLOR_APPWORKSPACE].SetStandardValue(RGB(151, 160, 123));
			m_arrColor[XPCOLOR_LABEL].SetStandardValue(RGB(222, 227, 189));


			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(81, 94, 51), RGB(96, 128, 88), RGB(159, 174, 122), RGB(244, 244, 238), RGB(117, 141, 94), RGB(255, 238, 194), RGB(63, 93, 56), RGB(63, 93, 56), RGB(63, 93, 56), RGB(254, 128, 62), RGB(255, 192, 111), RGB(209, 222, 173), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;
		}
		break;

	case xtpSystemThemeSilver:
		{
			m_clrDockBar.SetStandardValue(RGB(215, 215, 229), RGB(243, 243, 247));
			m_clrToolbarShadow.SetStandardValue(RGB(124, 124, 148));
			m_clrCommandBar.SetStandardValue(RGB(249, 249, 255), RGB(156, 155, 183), 0.75f);

			m_clrFloatingGripper.SetStandardValue(RGB(122, 121, 153));
			m_clrPopupControl.SetStandardValue(RGB(233, 231, 241), RGB(186, 185, 205));
			m_clrToolbarExpand.SetStandardValue(RGB(179, 178, 200), RGB(118, 116, 146), 0.75f);
			m_clrMenuExpandedGripper.SetStandardValue(RGB(215, 215, 226), RGB(133, 131, 162));

			m_clrMenuExpand.SetStandardValue(0xF1E8E8, 0xC5AEB0);

			m_clrStatusPane.SetStandardValue(RGB(236, 234, 218));
			m_pShadowManager->SetShadowColor(RGB(0x24, 0x20, 0x29));

			m_clrDisabledIcon.SetStandardValue(RGB(247, 245, 249), RGB(122, 121, 153));
			m_clrTearOffGripper.SetStandardValue(RGB(192, 192, 211));

			m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(RGB(214, 211, 231));
			m_arrColor[XPCOLOR_3DFACE].SetStandardValue(RGB(223, 223, 234));
			m_arrColor[XPCOLOR_3DSHADOW].SetStandardValue(RGB(177, 176, 195));
			m_arrColor[COLOR_APPWORKSPACE].SetStandardValue(RGB(155, 154, 179));
			m_arrColor[XPCOLOR_LABEL].SetStandardValue(RGB(214, 215, 231));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(84, 84, 117), RGB(110, 109, 143), RGB(168, 167, 190), RGB(253, 250, 255), RGB(124, 124, 148), RGB(255, 238, 194), RGB(75, 75, 111), RGB(75, 75, 111), RGB(75, 75, 111), RGB(254, 128, 62), RGB(255, 192, 111), RGB(219, 218, 228), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_bLunaTheme = TRUE;
		}
		break;
	}

	if (m_bLunaTheme)
	{
		m_arrColor[XPCOLOR_HIGHLIGHT_DISABLED_BORDER].SetStandardValue(RGB(141, 141, 141));
		m_arrColor[XPCOLOR_TOOLBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));
		m_arrColor[XPCOLOR_MENUBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));
		m_arrColor[XPCOLOR_FLOATBAR_BORDER].SetStandardValue(m_clrFloatingGripper);
	}

	m_clrMenuGripper.SetStandardValue(m_clrCommandBar);


	CreateGradientCircle();
}



void CXTPOffice2003Theme::FillDockBar(CDC* pDC, CXTPDockBar* pBar)
{
	if (((CWnd*)pBar)->GetStyle() & CBRS_RIGHT)
		pDC->FillSolidRect(CXTPClientRect((CWnd*)pBar), m_clrDockBar.clrDark);
	else
	{
		CXTPClientRect rc((CWnd*)pBar);
		CRect rcFill(rc.left, rc.top, max(rc.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2), rc.bottom);

		XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);
	}
}

void CXTPOffice2003Theme::FillMessageBar(CDC* pDC, CXTPMessageBar* pBar)
{
	CXTPClientRect rc((CWnd*)pBar);
	CRect rcFill(rc.left, rc.top, max(rc.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2), rc.bottom);

	XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);

	CRect rcMessage = pBar->GetMessageRect();
	pDC->Draw3dRect(rcMessage, m_clrToolbarShadow, m_clrToolbarShadow);

	rcMessage.DeflateRect(1, 1);
	pDC->FillSolidRect(rcMessage, GetXtremeColor(COLOR_INFOBK));
}

void CXTPOffice2003Theme::ExcludeCorners(CDC* pDC, CRect rc)
{
	pDC->ExcludeClipRect(rc.left, rc.top, rc.left + 1, rc.top + 1);
	pDC->ExcludeClipRect(rc.right - 1, rc.top, rc.right, rc.top + 1);
	pDC->ExcludeClipRect(rc.left, rc.bottom - 1, rc.left + 1, rc.bottom);
	pDC->ExcludeClipRect(rc.right - 1, rc.bottom - 1, rc.right, rc.bottom);
}

void CXTPOffice2003Theme::DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded)
{
	XTPDrawHelpers()->GradientFill(pDC, CRect(CPoint(x, y), CSize(cx, cy)),
		bExpanded ? m_clrMenuExpandedGripper : m_clrMenuGripper, TRUE);
}

BOOL CXTPOffice2003Theme::HasFloatingBarGradientEntry(CXTPCommandBar* pBar)
{
	return !IsFlatToolBar(pBar) && ((pBar->GetFlags() & xtpFlagAlignAny) != 0);
}

void CXTPOffice2003Theme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CXTPClientRect rc((CWnd*)pBar);

	if (pBar->GetPosition() == xtpBarPopup)
	{
		Rectangle(pDC, rc, XPCOLOR_MENUBAR_BORDER, XPCOLOR_MENUBAR_FACE);

		FillIntersectRect(pDC, (CXTPPopupBar*)pBar, GetXtremeColor(XPCOLOR_MENUBAR_FACE));
	}
	else if (IsFlatToolBar(pBar))
	{
		CRect rcFill(rc.left, rc.top, max(rc.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2), rc.bottom);
		if (IsDockingPosition(pBar->GetPosition()))
		{
			CWnd* pParent = pBar->GetParent();
			if (pParent)
			{
				CRect rcClient;
				pParent->GetWindowRect(rcClient);
				pBar->ScreenToClient(rcClient);
				rcFill = CRect(rcClient.left, rc.top, rcClient.left + max(rcClient.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2), rc.bottom);
			}
		}
		XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);

		if (pBar->GetPosition() == xtpBarFloating)
		{
			COLORREF clr = GetXtremeColor(XPCOLOR_FLOATBAR_BORDER);
			pDC->Draw3dRect(rc, clr, clr);
			rc.DeflateRect(1, 1);
			pDC->Draw3dRect(rc, clr, clr);

			pDC->SetPixel(2, 2, clr);
			pDC->SetPixel(rc.right - 2, 2, clr);
			pDC->SetPixel(2, rc.bottom - 2, clr);
			pDC->SetPixel(rc.right - 2, rc.bottom - 2, clr);
		}
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		COLORREF clr3DFace = GetXtremeColor(COLOR_3DFACE);
		pDC->FillSolidRect(rc, clr3DFace);

		CWnd* pWnd = pBar->GetParent();
		if (pWnd)
		{
			CXTPWindowRect rcFill((CWnd*)pWnd);
			pBar->ScreenToClient(rcFill);
			rcFill.right = rcFill.left + max(rcFill.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2);
			XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);

			pDC->ExcludeClipRect(rc.left, rc.top, rc.left + 3, rc.bottom);
			ExcludeCorners(pDC, CRect(rc.left + 3, rc.top + 1, rc.right, rc.bottom - 1));
		}

		CRect rcFill = rc;
		rcFill.top += 1;

		int nFirst = pBar->GetControls()->GetNext(-1, +1, FALSE, TRUE);
		if (nFirst != -1)
		{
			CRect rcRow = pBar->GetControls()->GetAt(nFirst)->GetRowRect();
			rcFill.bottom = rcRow.bottom + 1;
		}
		XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrCommandBar, FALSE);
		pDC->FillSolidRect(rcFill.left, rcFill.bottom, rcFill.Width(), rc.Height() - rcFill.Height() - 2, m_clrCommandBar.clrDark);

		pDC->FillSolidRect(rc.left + 5, rc.bottom - 1, rc.Width() - 7, 1, m_clrToolbarShadow);

		pDC->SelectClipRgn(0);
	}
	else if (pBar->GetPosition() == xtpBarLeft || pBar->GetPosition() == xtpBarRight)
	{
		COLORREF clr3DFace = GetXtremeColor(COLOR_3DFACE);
		pDC->FillSolidRect(rc, clr3DFace);

		CWnd* pWnd = pBar->GetParent();
		if (pWnd)
		{
			if (pWnd->GetStyle() & CBRS_RIGHT)
				pDC->FillSolidRect(rc, m_clrDockBar.clrDark);
			else
			{
				CXTPWindowRect rcFill((CWnd*)pWnd);
				pBar->ScreenToClient(rcFill);
				rcFill.right = rcFill.left + max(rcFill.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2);
				XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);
			}

			pDC->ExcludeClipRect(rc.left, rc.top, rc.right, rc.top + 3);
			ExcludeCorners(pDC, CRect(rc.left + 1, rc.top + 3, rc.right - 1, rc.bottom));
		}

		CRect rcFill = rc;
		rcFill.left += 1;

		int nFirst = pBar->GetControls()->GetNext(-1, +1, FALSE, TRUE);
		if (nFirst != -1)
		{
			CRect rcRow = pBar->GetControls()->GetAt(nFirst)->GetRowRect();
			rcFill.right = rcRow.right + 1;
		}
		XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrCommandBar, TRUE);
		pDC->FillSolidRect(rc.right - 1, rc.top + 5 , 1, rc.Height() - 7, m_clrToolbarShadow);

		pDC->SelectClipRgn(0);
	}
	else if (pBar->GetPosition() == xtpBarFloating && HasFloatingBarGradientEntry(pBar))
	{
		pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_TOOLBAR_FACE));

		COLORREF clrBorder = GetXtremeColor(XPCOLOR_FLOATBAR_BORDER);
		pDC->Draw3dRect(rc, clrBorder, clrBorder);
		rc.DeflateRect(1, 1);
		pDC->Draw3dRect(rc, clrBorder, clrBorder);

		CXTPFontDC font(pDC, GetSmCaptionFont());
		CSize sz = pDC->GetTextExtent(_T(" "), 1);

		CRect rcFill(rc.left + 2, rc.top + 2 + max(15, sz.cy), rc.right - 2, rc.bottom - 2);

		for (int i = 0; i < pBar->GetControlCount(); i++)
		{
			CXTPControl* pControl = pBar->GetControl(i);

			if (pControl->GetWrap() && pControl->IsVisible() && pControl->GetRect().top > rcFill.top)
			{
				CRect rcRow(rcFill.left, rcFill.top, rcFill.right, pControl->GetRowRect().top);
				XTPDrawHelpers()->GradientFill(pDC, rcRow, m_clrCommandBar, FALSE);

				rcFill.top = rcRow.bottom;
			}
		}

		XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrCommandBar, FALSE);

		pDC->SetPixel(2, 2, clrBorder);
		pDC->SetPixel(rc.right - 2, 2, clrBorder);
		pDC->SetPixel(2, rc.bottom - 2, clrBorder);
		pDC->SetPixel(rc.right - 2, rc.bottom - 2, clrBorder);

	}
	else
	{
		CXTPOfficeTheme::FillCommandBarEntry(pDC, pBar);
	}
}

CSize CXTPOffice2003Theme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	BOOL bSkipWrapSeparators = pBar->GetPosition() == xtpBarFloating && pBar->GetType() != xtpBarTypePopup;

	if (!bDraw && bSkipWrapSeparators)
	{
		return CSize(6, 0);
	}

	if (!bDraw)
	{
		return CXTPOfficeTheme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);
	}

	CRect rc;
	pBar->GetClientRect(&rc);
	CRect rcControl = pControl->GetRect();
	CXTPPenDC pen (*pDC, GetXtremeColor(XPCOLOR_SEPARATOR));
	CRect rcRow = pControl->GetRowRect();

	if (pBar->GetPosition() == xtpBarRight || pBar->GetPosition() == xtpBarLeft)
	{
		if (!pControl->GetWrap())
		{
			Line(pDC, rcRow.left + m_nToolbarSeparatorMargin, rcControl.top - 4, rcRow.right - m_nToolbarSeparatorMargin, rcControl.top - 4, XPCOLOR_SEPARATOR);
			if (!m_bPlainSeparators) Line(pDC, rcRow.left + m_nToolbarSeparatorMargin + 1, rcControl.top - 3, rcRow.right - m_nToolbarSeparatorMargin + 1, rcControl.top - 3, COLOR_3DHIGHLIGHT);
		}
		else
		{
			Line(pDC, rcRow.right + 2, rcRow.top + m_nToolbarSeparatorMargin, rcRow.right + 2, rcRow.bottom, XPCOLOR_SEPARATOR);
			Line(pDC, rcRow.right + 3, rcRow.top + m_nToolbarSeparatorMargin + 1, rcRow.right + 3, rcRow.bottom, COLOR_3DHIGHLIGHT);
		}

	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom || bSkipWrapSeparators)
	{
		if (!pControl->GetWrap())
		{
			Line(pDC, rcControl.left - 4, rcRow.top + m_nToolbarSeparatorMargin, rcControl.left - 4, rcRow.bottom - m_nToolbarSeparatorMargin, XPCOLOR_SEPARATOR);
			if (!m_bPlainSeparators) Line(pDC, rcControl.left - 3, rcRow.top + m_nToolbarSeparatorMargin + 1, rcControl.left - 3, rcRow.bottom - m_nToolbarSeparatorMargin + 1, COLOR_3DHIGHLIGHT);
		}
		else if (!bSkipWrapSeparators)
		{
			Line(pDC, rcRow.left + m_nToolbarSeparatorMargin, rcRow.top - 4, rcRow.right, rcRow.top - 4, XPCOLOR_SEPARATOR);
			Line(pDC, rcRow.left + m_nToolbarSeparatorMargin + 1, rcRow.top - 3, rcRow.right, rcRow.top - 3, COLOR_3DHIGHLIGHT);
		}
	}
	else return CXTPOfficeTheme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);

	return 0;
}

CSize CXTPOffice2003Theme::DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw)
{
	CSize sz(8, max(25, m_nTextHeight + 6));

	if (pDC && bDraw)
	{
		CRect rcGripper;
		pBar->GetClientRect(&rcGripper);

		rcGripper.DeflateRect(3, 3);
		rcGripper.bottom = rcGripper.top + sz.cy;

		if (pBar->IsActive())
		{
			if (m_bLunaTheme)
				XTPDrawHelpers()->GradientFill(pDC, rcGripper, m_grcLunaChecked, FALSE);
			else
				pDC->FillSolidRect(rcGripper, GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
		}
		else
		{
			XTPDrawHelpers()->GradientFill(pDC, rcGripper, m_clrCommandBar, FALSE);
		}

		for (int y = rcGripper.top + 4; y < rcGripper.bottom - 5; y += 4)
		{
			pDC->FillSolidRect(CRect(4 + 2, y + 1, 6 + 2, y + 3), GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(CRect(3 + 2, y, 5 + 2, y + 2), GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
	}

	return sz;
}

CSize CXTPOffice2003Theme::DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw)
{
	if (pBar->IsDialogBar())
		return DrawDialogBarGripper(pDC, (CXTPDialogBar*)pBar, bDraw);

	CRect rc;
	pBar->GetClientRect(&rc);

	if (pBar->GetPosition() == xtpBarPopup && pBar->GetType() == xtpBarTypePopup)
	{
		return CXTPOfficeTheme::DrawCommandBarGripper(pDC, pBar, bDraw);
	}
	else if (pBar->GetPosition() == xtpBarFloating)
	{
		return CXTPOfficeTheme::DrawCommandBarGripper(pDC, pBar, bDraw);
	}
	else if (pBar->GetPosition() == xtpBarRight || pBar->GetPosition() == xtpBarLeft)
	{
		if (!pDC || !bDraw) return CSize(0, 8 + 1);
		for (int x = 5; x < rc.Width() - 7; x += 4)
		{
			if (!m_bFlatGripper) pDC->FillSolidRect(CRect(x + 1, 4 + 3, x + 3, 6 + 3), GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(CRect(x, 3 + 3, x + 2, 5 + 3), GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		if (!pDC || !bDraw) return CSize(6, 0);
		for (int y = 5; y < rc.Height() - 7; y += 4)
		{
			if (!m_bFlatGripper) pDC->FillSolidRect(CRect(7, y + 1, 9, y + 3), GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(CRect(6, y, 8, y + 2), GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
	}
	return 0;

}

void CXTPOffice2003Theme::RectangleEx(CDC* pDC, CRect rc, int nColorBorder, BOOL bHoriz, CXTPPaintManagerColorGradient& color)
{
	GradientFill(pDC, rc, color.clrLight, color.clrDark, bHoriz);
	Draw3dRect(pDC, rc, nColorBorder, nColorBorder);
}

void CXTPOffice2003Theme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition /*= xtpBarPopup*/)
{
	if (bPopuped)
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_clrPopupControl, FALSE);
		Draw3dRect(pDC, rc, XPCOLOR_MENUBAR_BORDER, XPCOLOR_MENUBAR_BORDER);
	}
	else if (m_bLunaTheme && barType != xtpBarTypePopup && (IsDockingPosition(barPosition) || (barPosition == xtpBarFloating)))
	{
		BOOL bHoriz = IsVerticalPosition(barPosition);
		if (!bEnabled)
		{
			if (IsKeyboardSelected(bSelected))
				RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, bHoriz, bChecked ? m_grcLunaPushed : m_grcLunaSelected);
			else if (bChecked)
				RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_DISABLED_BORDER, bHoriz, m_grcLunaChecked);
			return;
		}
		else if (bChecked == 2 && !bSelected && !bPressed)
		{
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_3DFACE);
		}
		else if (bChecked && !bSelected && !bPressed) RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, bHoriz, m_grcLunaChecked);
		else if (bChecked && bSelected && !bPressed) RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, bHoriz, m_grcLunaPushed);
		else if (IsKeyboardSelected(bPressed) || (bSelected && bPressed)) RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, bHoriz, m_grcLunaPushed);
		else if (bSelected || bPressed) RectangleEx(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, bHoriz, m_grcLunaSelected);

	}
	else CXTPOfficeTheme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

void CXTPOffice2003Theme::DrawExpandSymbols(CDC* pDC, BOOL bVertical, CRect rc, BOOL bHiddenExists, COLORREF clr)
{
	if (!bVertical)
	{
		CPoint pt = CPoint(rc.left + rc.Width()/2, rc.bottom - 6);
		CXTPPenDC pen(*pDC, clr);
		Line(pDC, CPoint(pt.x -2 , pt.y - 4), CPoint(pt.x + 3, pt.y - 4));
		Triangle(pDC, CPoint(pt.x -2 , pt.y - 1), CPoint(pt.x + 2, pt.y - 1), CPoint (pt.x, pt.y + 1), clr);

		if (bHiddenExists)
		{
			Triangle(pDC, CPoint(rc.left + 3, rc.top + 5), CPoint(rc.left + 4, rc.top + 6), CPoint(rc.left + 3, rc.top + 7), clr);
			Triangle(pDC, CPoint(rc.left + 7, rc.top + 5), CPoint(rc.left + 8, rc.top + 6), CPoint(rc.left + 7, rc.top + 7), clr);
		}
	}
	else
	{
		CPoint pt = CPoint(rc.right - 5, rc.CenterPoint().y);
		CXTPPenDC pen(*pDC, clr);
		Line(pDC, CPoint(pt.x - 5 , pt.y - 2), CPoint(pt.x - 5, pt.y + 3));

		Triangle(pDC, CPoint(pt.x , pt.y), CPoint(pt.x - 2, pt.y - 2), CPoint (pt.x - 2, pt.y + 2), clr);

		if (bHiddenExists)
		{
			Triangle(pDC, CPoint(rc.left + 5, rc.top + 3), CPoint(rc.left + 6, rc.top + 4), CPoint(rc.left + 7, rc.top + 3), clr);
			Triangle(pDC, CPoint(rc.left + 5, rc.top + 7), CPoint(rc.left + 6, rc.top + 8), CPoint(rc.left + 7, rc.top + 7), clr);
		}
	}
}

CSize CXTPOffice2003Theme::DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam)
{
	if (controlType == xtpButtonExpandToolbar)
	{
		if (!bDraw)
		{
			CSize szBar = *(CSize*)lpParam;
			if (!IsVerticalPosition(pBar->GetPosition()))
				pButton->SetRect(CRect(szBar.cx - (11 + 1), 1, szBar.cx - 0, szBar.cy - 0));
			else
				pButton->SetRect(CRect(0, szBar.cy - (11 + 1), szBar.cx - 0, szBar.cy - 0));
		}
		else
		{
			CRect rc = pButton->GetRect();
			if (!IsVerticalPosition(pBar->GetPosition()))
			{
				pDC->ExcludeClipRect(rc.right - 1, rc.top, rc.right, rc.top + 1);
				pDC->ExcludeClipRect(rc.right - 1, rc.bottom - 1, rc.right, rc.bottom);
				pDC->ExcludeClipRect(rc.left, rc.top + 1, rc.left + 1, rc.bottom - 1);
			}
			else
			{
				pDC->ExcludeClipRect(rc.left, rc.bottom - 1, rc.left + 1, rc.bottom);
				pDC->ExcludeClipRect(rc.right - 1, rc.bottom - 1, rc.right, rc.bottom);
				pDC->ExcludeClipRect(rc.left + 1, rc.top, rc.right - 1, rc.top + 1);
			}

			if (pButton->GetPopuped())
			{
				if (m_bLunaTheme) GradientFill(pDC, rc, m_grcLunaPushed.clrLight, m_grcLunaPushed.clrDark, IsVerticalPosition(pBar->GetPosition()));
				else pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT_CHECKED));
			}
			else if (pButton->GetSelected())
			{
				if (m_bLunaTheme) GradientFill(pDC, rc, m_grcLunaSelected.clrLight, m_grcLunaSelected.clrDark, IsVerticalPosition(pBar->GetPosition()));
				else pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT));
			}
			else
			{
				XTPDrawHelpers()->GradientFill(pDC, rc, m_clrToolbarExpand, IsVerticalPosition(pBar->GetPosition()));
			}

			COLORREF clrText = GetXtremeColor(pButton->GetPopuped() ? XPCOLOR_TOOLBAR_TEXT: pButton->GetSelected() ? XPCOLOR_HIGHLIGHT_TEXT : XPCOLOR_TOOLBAR_TEXT);
			BOOL bHiddenExists = *(BOOL*)lpParam;

			DrawExpandSymbols(pDC, IsVerticalPosition(pBar->GetPosition()), CRect(CPoint(rc.left + 1, rc.top + 1), rc.Size()) , bHiddenExists, GetXtremeColor(COLOR_BTNHIGHLIGHT));
			DrawExpandSymbols(pDC, IsVerticalPosition(pBar->GetPosition()), rc, bHiddenExists, clrText);

			pDC->SelectClipRgn(0);
		}
		return 0;
	}
	if (controlType == xtpButtonExpandMenu)
	{
		if (bDraw)
		{
			CXTPPaintManager::DrawControlEntry(pDC, pButton);
			CRect rcButton = pButton->GetRect();
			int nCenter = rcButton.CenterPoint().x;

			m_ilGradientCircle.Draw(pDC, 0, CPoint(nCenter - 7, rcButton.top + 1), ILD_NORMAL);

			int nTop = rcButton.top + 5;
			CXTPPenDC pen(pDC->GetSafeHdc(), 0);
			pDC->MoveTo(nCenter - 2, nTop);
			pDC->LineTo(nCenter, nTop + 2);
			pDC->LineTo(nCenter + 3, nTop - 1);
			pDC->MoveTo(nCenter - 2, nTop + 1);
			pDC->LineTo(nCenter, nTop + 3);
			pDC->LineTo(nCenter + 3, nTop);

			pDC->MoveTo(nCenter - 2, nTop + 4);
			pDC->LineTo(nCenter, nTop + 6);
			pDC->LineTo(nCenter + 3, nTop + 3);
			pDC->MoveTo(nCenter - 2, nTop + 5);
			pDC->LineTo(nCenter, nTop + 7);
			pDC->LineTo(nCenter + 3, nTop + 4);

		}
		return CSize(0, 18);
	}

	return CXTPOfficeTheme::DrawSpecialControl(pDC, controlType, pButton, pBar, bDraw, lpParam);
}


void CXTPOffice2003Theme::CreateGradientCircle()
{

	const BYTE clrMask[16 * 16] =
	{
		0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xEA, 0xC6, 0xB4, 0xB4, 0xB4, 0xC6, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xEA, 0xEA, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x7E, 0x7E, 0xC6, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xEA, 0xFC, 0xFC, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x7E, 0x6C, 0x5A, 0x90, 0xFF, 0xFF,
		0xFF, 0xEA, 0xFC, 0xFC, 0xFC, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x7E, 0x6C, 0x5A, 0x36, 0x7E, 0xFF,
		0xEA, 0xFC, 0xFC, 0xFC, 0xFC, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x7E, 0x6C, 0x5A, 0x36, 0x24, 0xEA,
		0xEA, 0xFC, 0xFC, 0xFC, 0xFC, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x7E, 0x6C, 0x48, 0x36, 0x24, 0x5A,
		0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xEA, 0xC6, 0xB4, 0x90, 0x90, 0x7E, 0x6C, 0x48, 0x24, 0x12, 0x24,
		0xC6, 0xEA, 0xEA, 0xEA, 0xC6, 0xC6, 0xC6, 0xB4, 0x90, 0x7E, 0x7E, 0x48, 0x36, 0x24, 0x12, 0x12,
		0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xB4, 0xB4, 0x90, 0x7E, 0x7E, 0x6C, 0x48, 0x36, 0x12, 0x12, 0x00,
		0xC6, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0x90, 0x7E, 0x7E, 0x6C, 0x5A, 0x36, 0x24, 0x12, 0x00, 0x00,
		0xC6, 0x90, 0x90, 0x90, 0x90, 0x90, 0x7E, 0x7E, 0x6C, 0x6C, 0x48, 0x36, 0x24, 0x12, 0x00, 0x36,
		0xFC, 0x90, 0x7E, 0x7E, 0x7E, 0x7E, 0x6C, 0x6C, 0x5A, 0x5A, 0x36, 0x24, 0x12, 0x00, 0x00, 0xB4,
		0xFF, 0xC6, 0x6C, 0x6C, 0x6C, 0x6C, 0x5A, 0x5A, 0x5A, 0x48, 0x36, 0x24, 0x12, 0x00, 0x48, 0xFF,
		0xFF, 0xFF, 0xB4, 0x6C, 0x5A, 0x5A, 0x5A, 0x48, 0x48, 0x36, 0x24, 0x12, 0x00, 0x36, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xC6, 0x5A, 0x48, 0x48, 0x48, 0x36, 0x24, 0x24, 0x12, 0x6C, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xB4, 0x6C, 0x48, 0x36, 0x5A, 0x7E, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF
	};

	m_ilGradientCircle.DeleteImageList();

	COLORREF clrLight = m_clrMenuExpand.clrLight;
	COLORREF clrDark = m_clrMenuExpand.clrDark;

	CWindowDC dc(CWnd::GetDesktopWindow());

	CDC dcCircle;
	dcCircle.CreateCompatibleDC(&dc);
	CBitmap bmp;
	VERIFY(bmp.CreateCompatibleBitmap(&dc, 16, 16));
	CBitmap* pOldBitmap = (CBitmap*)dcCircle.SelectObject(&bmp);

	for (int i = 0; i < 16 * 16; i++)
	{
		int n = clrMask[i];

		int nRed = GetRValue(clrDark) - n * (GetRValue(clrDark) - GetRValue(clrLight)) / 0xFF;
		int nGreen = GetGValue(clrDark) - n * (GetGValue(clrDark) - GetGValue(clrLight)) / 0xFF;
		int nBlue = GetBValue(clrDark) - n * (GetBValue(clrDark) - GetBValue(clrLight)) / 0xFF;
		dcCircle.SetPixel(i / 16, i % 16, n == 0xFF ? RGB(0, 0xFF, 0) : RGB(nRed, nGreen, nBlue));
	}

	dcCircle.SelectObject(pOldBitmap);

	m_ilGradientCircle.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	m_ilGradientCircle.Add(&bmp, RGB(0, 0xFF, 0));

	ASSERT(m_ilGradientCircle.GetImageCount() == 1);

}


#define RGB_TO_RGBQUAD(r, g, b) (RGB(b, g, r))


HBITMAP CXTPOffice2003Theme::GetModernBitmap(UINT nIDResource)
{
	static const struct
	{
		DWORD rgbqFrom;
		DWORD rgbqTo;
	}
	modernColorMap[] =
	{
		{ RGB_TO_RGBQUAD(0, 128, 0), RGB_TO_RGBQUAD(0, 151, 94) },
		{ RGB_TO_RGBQUAD(255, 0, 0), RGB_TO_RGBQUAD(255, 87, 42) },
		{ RGB_TO_RGBQUAD(128, 0, 0), RGB_TO_RGBQUAD(213, 79, 74) },
		{ RGB_TO_RGBQUAD(0, 0, 255), RGB_TO_RGBQUAD(58, 123, 252) },
		{ RGB_TO_RGBQUAD(0, 0, 128), RGB_TO_RGBQUAD(1, 81, 205) },
		{ RGB_TO_RGBQUAD(128, 128, 128), RGB_TO_RGBQUAD(149, 147, 145) },
		{ RGB_TO_RGBQUAD(128, 128, 0), RGB_TO_RGBQUAD(228, 167, 8) },
		{ RGB_TO_RGBQUAD(192, 192, 192), RGB_TO_RGBQUAD(205, 198, 187) },
		{ RGB_TO_RGBQUAD(255, 255, 0), RGB_TO_RGBQUAD(255, 218, 33) }
	};


	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResource);
	ASSERT(lpszResourceName != NULL);
	if (!lpszResourceName)
		return NULL;

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	if (!hInst)
		return NULL;

	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_BITMAP);
	if (hRsrc == NULL)
		return NULL;

	HGLOBAL hglb = LoadResource(hInst, hRsrc);
	if (hglb == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;

	MEMCPY_S(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	COLORREF clrMask = XTPImageManager()->GetMaskColor();

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < _countof(modernColorMap); i++)
		{
			if (pColorTable[iColor] == modernColorMap[i].rgbqFrom
				&& clrMask != pColorTable[iColor])
			{
				pColorTable[iColor] = modernColorMap[i].rgbqTo;
				break;
			}
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);

	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += ((size_t)1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	return hbm;
}

BOOL CXTPOffice2003Theme::LoadModernToolbarIcons(UINT nIDResource)
{

	HBITMAP hbm = GetModernBitmap(nIDResource);
	if (!hbm)
		return FALSE;

	CBitmap bmp;
	bmp.Attach(hbm);

	return XTPImageManager()->SetIcons(nIDResource, bmp);
}

BOOL CXTPOffice2003Theme::LoadModernToolbarIcons(UINT nIDResource, UINT* pCommands, int nCount, CSize szIcon)
{
	HBITMAP hbm = GetModernBitmap(nIDResource);
	if (!hbm)
		return FALSE;

	CBitmap bmp;
	bmp.Attach(hbm);

	return XTPImageManager()->SetIcons(bmp, pCommands, nCount, szIcon);
}



CRect CXTPOffice2003Theme::GetCommandBarBorders(CXTPCommandBar* pBar)
{
	if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
		return CRect(4, 2, 2, 2);
	else return CXTPOfficeTheme::GetCommandBarBorders(pBar);
}

CSize CXTPOffice2003Theme::DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw)
{
	if (bDraw)
	{
		CRect rc(1, 1, rcGripper.right + 1, rcGripper.Height() + 1);
		pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_MENUBAR_FACE));
		rc.DeflateRect(1, 1);
		int nLeft = rc.CenterPoint().x - min(32, rcGripper.Width() /2 - 10);
		int nRight = rc.CenterPoint().x + min(32, rcGripper.Width() /2 - 10);

		if (bSelected)
			Rectangle(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT);
		else
			pDC->FillSolidRect(rc, m_clrTearOffGripper);

		for (int x = nLeft; x < nRight; x += 4)
		{
			pDC->FillSolidRect(CRect(x + 1, 4 + 1, x + 3, 6 + 1), GetXtremeColor(COLOR_BTNHIGHLIGHT));
			pDC->FillSolidRect(CRect(x, 3 + 1, x + 2, 5 + 1), GetXtremeColor(XPCOLOR_TOOLBAR_GRIPPER));
		}
	}

	return CSize(0, 9);
}

void CXTPOffice2003Theme::DrawStatusBarGripper(CDC* pDC, CRect rcClient)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		CRect rcGripper(rcClient.right - 18, rcClient.top, rcClient.right, rcClient.bottom);
		m_themeStatusBar.DrawThemeBackground(pDC->GetSafeHdc(), SP_GRIPPER, 0, &rcGripper, 0);

	}
	else
	{
		CXTPPenDC penBorder(*pDC, GetXtremeColor(COLOR_3DSHADOW));
		int i;
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(rcClient.Width() - i * 5, rcClient.Height() - 1);
			pDC->LineTo(rcClient.Width(), rcClient.Height() - 1 - i * 5);

			pDC->MoveTo(rcClient.Width() - 1 - i * 5, rcClient.Height() - 1);
			pDC->LineTo(rcClient.Width(), rcClient.Height() - 2 - i * 5);

			pDC->MoveTo(rcClient.Width() - 2 - i * 5, rcClient.Height() - 1);
			pDC->LineTo(rcClient.Width(), rcClient.Height() - 3 - i * 5);
		}

		CXTPPenDC penBorderWhite(*pDC, GetXtremeColor(COLOR_BTNHIGHLIGHT));
		for (i = 0; i < 3; i++)
		{
			pDC->MoveTo(rcClient.Width() - 3 - i * 5, rcClient.Height() - 1);
			pDC->LineTo(rcClient.Width(), rcClient.Height() - 4 - i * 5);
		}

	}
}

void CXTPOffice2003Theme::DrawStatusBarPaneBorder(CDC* pDC, CRect rc, CXTPStatusBarPane* pPane, BOOL bGripperPane)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		CXTPPaintManager::DrawStatusBarPaneBorder(pDC, rc, pPane, bGripperPane);
	}
	else
	{
		pDC->Draw3dRect(rc, m_clrStatusPane, m_clrStatusPane);
	}
}


//////////////////////////////////////////////////////////////////////////
// CXTPWhidbeyTheme

IMPLEMENT_DYNAMIC(CXTPWhidbeyTheme, CXTPOffice2003Theme)

CXTPWhidbeyTheme::CXTPWhidbeyTheme()
{
	m_systemTheme = xtpSystemThemeUnknown;
	m_bPlainSeparators = TRUE;
}

CXTPWhidbeyTheme::~CXTPWhidbeyTheme()
{

}

void CXTPWhidbeyTheme::RefreshMetrics()
{
	CXTPOffice2003Theme::RefreshMetrics();

	m_clrToolbarExpand.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_WINDOW), GetXtremeColor(COLOR_3DFACE), 550),
		GetXtremeColor(COLOR_3DSHADOW));

	if (XTPColorManager()->IsLowResolution())
	{
		m_clrToolbarExpand.SetStandardValue(GetXtremeColor(COLOR_3DFACE));
	}

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
		m_clrCommandBar.SetStandardValue(RGB(250, 249, 245), RGB(192, 192, 168), 0.75f);
		m_clrToolbarExpand.SetStandardValue(RGB(238, 237, 234), RGB(152, 152, 126), 0.75f);
		m_clrDockBar.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		m_clrToolbarShadow.SetStandardValue(RGB(163, 163, 124));
		m_clrMenuExpandedGripper.SetStandardValue(RGB(204, 199, 186));
		m_clrMenuExpand.SetStandardValue(m_clrCommandBar);
		m_clrDisabledIcon.SetStandardValue(RGB(247, 245, 249), RGB(122, 121, 153));
		break;

	case xtpSystemThemeOlive:
		m_clrCommandBar.SetStandardValue(RGB(250, 249, 245), RGB(192, 192, 168), 0.75f);
		m_clrToolbarExpand.SetStandardValue(RGB(238, 237, 234), RGB(176, 172, 158), 0.75f);
		m_clrDockBar.SetStandardValue(RGB(229, 229, 215), RGB(244, 241, 231));
		m_clrToolbarShadow.SetStandardValue(RGB(163, 163, 124));
		m_clrMenuExpandedGripper.SetStandardValue(RGB(204, 199, 186));
		m_clrMenuExpand.SetStandardValue(m_clrCommandBar);
		m_clrDisabledIcon.SetStandardValue(RGB(236, 233, 216), RGB(172, 168, 153));

		m_arrColor[XPCOLOR_HIGHLIGHT].SetStandardValue(RGB(182, 198, 141));
		m_arrColor[XPCOLOR_HIGHLIGHT_PUSHED].SetStandardValue(RGB(147, 160, 112));
		m_arrColor[XPCOLOR_HIGHLIGHT_CHECKED].SetStandardValue(RGB(182, 198, 141));
		m_arrColor[XPCOLOR_PUSHED_TEXT].SetStandardValue(RGB(255, 255, 255));
		break;

	case xtpSystemThemeSilver:
		m_clrCommandBar.SetStandardValue(RGB(243, 244, 250), RGB(153, 151, 181), 0.75f);
		m_clrToolbarExpand.SetStandardValue(RGB(179, 178, 200), RGB(118, 116, 146), 0.75f);
		m_clrDockBar.SetStandardValue(RGB(215, 215, 229), RGB(243, 243, 247));
		m_clrMenuExpandedGripper.SetStandardValue(RGB(215, 215, 226), RGB(133, 131, 162));
		m_clrMenuExpand.SetStandardValue(0xF1E8E8, 0xC5AEB0);
		m_clrToolbarShadow.SetStandardValue(RGB(124, 124, 148));
		m_clrDisabledIcon.SetStandardValue(RGB(247, 245, 249), RGB(122, 121, 153));

		m_arrColor[XPCOLOR_TOOLBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));
		m_arrColor[XPCOLOR_MENUBAR_GRAYTEXT].SetStandardValue(RGB(141, 141, 141));
		m_arrColor[XPCOLOR_SEPARATOR].SetStandardValue(RGB(110, 109, 143));
		m_arrColor[XPCOLOR_TOOLBAR_GRIPPER].SetStandardValue(RGB(84, 84, 117));
		m_arrColor[XPCOLOR_TOOLBAR_FACE].SetStandardValue(RGB(219, 218, 228));
		m_clrTearOffGripper.SetStandardValue(RGB(192, 192, 211));
		m_clrFloatingGripper.SetStandardValue(RGB(122, 121, 153));
		m_arrColor[XPCOLOR_FLOATBAR_BORDER].SetStandardValue(RGB(122, 121, 153));

		INT nElements[] =
		{
			XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_PUSHED_TEXT
		};
		COLORREF clrValues[] =
		{
			RGB(255, 238, 194), RGB(75, 75, 111), RGB(75, 75, 111), RGB(75, 75, 111), RGB(254, 128, 62), RGB(255, 192, 111), 0
		};
		SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

		break;
	}

	m_clrMenuGripper.SetStandardValue(m_clrCommandBar);

	CreateGradientCircle();
}

//////////////////////////////////////////////////////////////////////////
// CXTPVisualStudio2008Theme

CXTPVisualStudio2008Theme::CXTPVisualStudio2008Theme()
{
	m_nPopupBarTextPadding = 10;
	m_pShadowManager->SetShadowOptions(xtpShadowOfficeAlpha);
}

CXTPVisualStudio2008Theme::~CXTPVisualStudio2008Theme()
{
}

void CXTPVisualStudio2008Theme::RefreshMetrics()
{
	CXTPWhidbeyTheme::RefreshMetrics();

	m_clrMenuGripper.SetStandardValue(XTPColorManager()->LightColor(GetXtremeColor(COLOR_3DFACE), GetXtremeColor(COLOR_WINDOW), 40), GetXtremeColor(COLOR_3DFACE));
	m_clrMenuSelection.SetStandardValue(GetXtremeColor(COLOR_HIGHLIGHT));
	m_clrMenuSelectionBorder.SetStandardValue(GetXtremeColor(COLOR_HIGHLIGHT));
	m_clrMenuSelectionText.SetStandardValue(GetXtremeColor(COLOR_HIGHLIGHTTEXT));

	switch (XTPColorManager()->GetCurrentSystemTheme())
	{
	case xtpSystemThemeBlue:
	case xtpSystemThemeRoyale:
		{
			m_clrMenuSelectionBorder.SetStandardValue(RGB(152, 181, 226));
			m_clrMenuSelection.SetStandardValue(RGB(239,243,250), RGB(193,210,238));
			m_clrMenuSelectionText.SetStandardValue(0);
			m_clrMenuGripper.SetStandardValue(RGB(241, 241, 241));
			m_clrMenuExpandedGripper.SetStandardValue(RGB(215, 227, 234));

			m_arrColor[XPCOLOR_LABEL].SetStandardValue(RGB(221, 231, 238));
		}
		break;

	case xtpSystemThemeAero:
		{
			m_clrMenuSelectionBorder.SetStandardValue(RGB(150,217,249));
			m_clrMenuSelection.SetStandardValue(RGB(241,248,251), RGB(186,228,246));
			m_clrMenuSelectionText.SetStandardValue(0);
			m_clrMenuGripper.SetStandardValue(RGB(244,247,252), RGB(212,216,230));

			m_arrColor[XPCOLOR_SEPARATOR].SetStandardValue(RGB(160,160,180));


			m_clrDockBar.SetStandardValue(RGB(233,236,250), RGB(244,247,252));
			m_clrToolbarShadow.SetStandardValue(RGB(181,190,206));
			m_clrCommandBar.SetStandardValue(RGB(250,250,253), RGB(196,203,219), 0.75f);

			m_clrFloatingGripper.SetStandardValue(RGB(128, 128, 128));
			m_clrPopupControl.SetStandardValue(RGB(255, 255, 255), RGB(118, 184, 219));
			m_clrToolbarExpand.SetStandardValue(RGB(195,202,218), RGB(233,236,250), 0.75f);

			m_clrStatusPane.SetStandardValue(RGB(160,160,180));
			m_pShadowManager->SetShadowColor(0);

			m_clrDisabledIcon.SetStandardValue(RGB(244,247,252), RGB(113,124,152));

			m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(RGB(240, 240, 240));

			INT nElements[] =
			{
				XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED, XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER, XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
			};
			COLORREF clrValues[] =
			{
				RGB(160,160,180), RGB(160,160,180), RGB(128,128,128), RGB(240, 240, 240), RGB(160, 160, 160), RGB(182,228,247), RGB(41,153,255), RGB(41,153,255), RGB(48,127,177), RGB(206,237,250), RGB(206,237,250), RGB(233,236,250), 0
			};
			SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);

			m_grcLunaChecked.SetStandardValue(RGB(237,248,253), RGB(129,192,224));
			m_grcLunaPushed.SetStandardValue(RGB(228,245,252), RGB(124,177,204));
			m_grcLunaSelected.SetStandardValue(RGB(237,248,253), RGB(129,192,224));

			m_bLunaTheme = TRUE;

		}
		break;

	case xtpSystemThemeOlive:
		{
			m_clrMenuSelectionBorder.SetStandardValue(RGB(147,160,112));
			m_clrMenuSelection.SetStandardValue(RGB(239,242,227), RGB(194,207,158));
			m_clrMenuSelectionText.SetStandardValue(0);
			m_clrMenuGripper.SetStandardValue(RGB(247,246,239), RGB(233,230,214));
		}
		break;

	case xtpSystemThemeSilver:
		{
			m_clrMenuSelectionBorder.SetStandardValue(RGB(255,192,111));
			m_clrMenuSelection.SetStandardValue(RGB(255,246,204), RGB(255,194,115));
			m_clrMenuSelectionText.SetStandardValue(0);
			m_clrMenuGripper.SetStandardValue(RGB(232,233,242), RGB(193,193,211));
			m_arrColor[XPCOLOR_SEPARATOR].SetStandardValue(RGB(110,109,143));
			m_clrPopupControl.SetStandardValue(RGB(232,233,241), RGB(192,191,209));
		}
		break;
	}
}

void CXTPVisualStudio2008Theme::AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL /*bVertical*/)
{
	CXTPCommandBar* pParent = pControl->GetParent();

	if (pParent->GetType() == xtpBarTypePopup)
		rc.InflateRect(1, 0);
}

CRect CXTPVisualStudio2008Theme::GetCommandBarBorders(CXTPCommandBar* pBar)
{
	if (pBar->GetPosition() == xtpBarPopup && pBar->GetType() == xtpBarTypePopup)
		return CRect(4, 3, 4, 3);

	return CXTPWhidbeyTheme::GetCommandBarBorders(pBar);
}

void CXTPVisualStudio2008Theme::DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded)
{
	XTPDrawHelpers()->GradientFill(pDC, CRect(CPoint(x, y), CSize(cx, cy)),
		bExpanded ? m_clrMenuExpandedGripper : m_clrMenuGripper, TRUE);

	pDC->FillSolidRect(x + cx, y, 1, cy, GetXtremeColor(XPCOLOR_SEPARATOR));
	pDC->FillSolidRect(x + cx + 1, y, 1, cy, GetXtremeColor(COLOR_3DHILIGHT));
}

void CXTPVisualStudio2008Theme::AlphaPixel(CDC* pDC, int x, int y, COLORREF clr, int nAlpha)
{
	if (nAlpha == 255)
	{
		pDC->SetPixel(x, y, clr);
	}
	else
	{
		COLORREF clrOld = pDC->GetPixel(x, y);
		pDC->SetPixel(x, y, CXTPDrawHelpers::BlendColors(clr, clrOld, (double)nAlpha / 255));
	}
}

void CXTPVisualStudio2008Theme::AlphaBorder(CDC* pDC, CRect rc, COLORREF clrBorder)
{
	pDC->FillSolidRect(rc.left + 3, rc.top, rc.Width() - 6, 1, clrBorder);
	pDC->FillSolidRect(rc.left + 3, rc.bottom - 1, rc.Width() - 6, 1, clrBorder);
	pDC->FillSolidRect(rc.left, rc.top + 3, 1, rc.Height() - 6, clrBorder);
	pDC->FillSolidRect(rc.right - 1 , rc.top + 3, 1, rc.Height() - 6, clrBorder);

	AlphaPixel(pDC, rc.left + 1, rc.top + 1, clrBorder, 255);
	AlphaPixel(pDC, rc.left + 1, rc.top, clrBorder, 60);
	AlphaPixel(pDC, rc.left + 2, rc.top, clrBorder, 120);
	AlphaPixel(pDC, rc.left, rc.top + 1, clrBorder, 60);
	AlphaPixel(pDC, rc.left, rc.top + 2, clrBorder, 120);

	AlphaPixel(pDC, rc.right - 2, rc.top + 1, clrBorder, 255);
	AlphaPixel(pDC, rc.right - 3, rc.top, clrBorder, 120);
	AlphaPixel(pDC, rc.right - 2, rc.top, clrBorder, 60);
	AlphaPixel(pDC, rc.right - 1, rc.top + 1, clrBorder, 60);
	AlphaPixel(pDC, rc.right - 1, rc.top + 2, clrBorder, 120);

	AlphaPixel(pDC, rc.left + 1, rc.bottom - 2, clrBorder, 255);
	AlphaPixel(pDC, rc.left + 1, rc.bottom - 1, clrBorder, 60);
	AlphaPixel(pDC, rc.left + 2, rc.bottom - 1, clrBorder, 120);
	AlphaPixel(pDC, rc.left, rc.bottom - 2, clrBorder, 60);
	AlphaPixel(pDC, rc.left, rc.bottom - 3, clrBorder, 120);


	AlphaPixel(pDC, rc.right - 2, rc.bottom - 2, clrBorder, 255);
	AlphaPixel(pDC, rc.right - 3, rc.bottom - 1, clrBorder, 120);
	AlphaPixel(pDC, rc.right - 2, rc.bottom - 1, clrBorder, 60);
	AlphaPixel(pDC, rc.right - 1, rc.bottom - 2, clrBorder, 60);
	AlphaPixel(pDC, rc.right - 1, rc.bottom - 3, clrBorder, 120);
}

void CXTPVisualStudio2008Theme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	if (barPosition == xtpBarPopup && barType == xtpBarTypePopup && bSelected && !bChecked && !bPressed)
	{
		XTPDrawHelpers()->GradientFill(pDC, CRect(rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1), m_clrMenuSelection, FALSE);

		COLORREF clrBorder = m_clrMenuSelectionBorder;
		AlphaBorder(pDC, rc, clrBorder);
		return;
	}

	if (barType == xtpBarTypeMenuBar && bSelected && !bChecked && !bPressed && bEnabled)
	{
		COLORREF clrShadow = GetXtremeColor(COLOR_WINDOW);
		COLORREF clrBorder = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);

		pDC->FillSolidRect(rc.left + 1, rc.top, rc.Width() - 2, 1, clrShadow);
		pDC->FillSolidRect(rc.left + 1, rc.bottom - 1, rc.Width() - 2, 1, clrShadow);
		pDC->FillSolidRect(rc.left, rc.top + 1, 1, rc.Height() - 2, clrShadow);
		pDC->FillSolidRect(rc.right - 1 , rc.top + 1, 1, rc.Height() - 2, clrShadow);
		rc.DeflateRect(1, 1);

		pDC->Draw3dRect(rc, clrBorder, clrBorder);
		COLORREF clrPixel = CXTPDrawHelpers::BlendColors(clrBorder, clrShadow, 0.5);
		pDC->SetPixel(rc.left, rc.top, clrPixel);
		pDC->SetPixel(rc.left, rc.bottom - 1, clrPixel);
		pDC->SetPixel(rc.right - 1, rc.top, clrPixel);
		pDC->SetPixel(rc.right - 1, rc.bottom - 1, clrPixel);

		rc.DeflateRect(1, 1);

		if (!bPopuped)
		{
			if (m_bLunaTheme) XTPDrawHelpers()->GradientFill(pDC, rc, m_grcLunaSelected, FALSE);
			else pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT));
		}
		else
			XTPDrawHelpers()->GradientFill(pDC, rc, m_clrPopupControl, FALSE);

		return;
	}
	if (barType != xtpBarTypePopup && bPopuped && bEnabled)
	{
		XTPDrawHelpers()->GradientFill(pDC, rc, m_clrPopupControl, FALSE);
		Draw3dRect(pDC, rc, XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_BORDER);
		return;
	}

	CXTPWhidbeyTheme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

COLORREF CXTPVisualStudio2008Theme::GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	if (barPosition == xtpBarPopup && barType == xtpBarTypePopup && bSelected && !bPressed && bEnabled)
	{
		return m_clrMenuSelectionText;
	}

	return CXTPWhidbeyTheme::GetRectangleTextColor(bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

CSize CXTPVisualStudio2008Theme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (!bDraw || pBar->GetType() != xtpBarTypePopup || pBar->GetPosition() != xtpBarPopup)
		return CXTPWhidbeyTheme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);

	CRect rc;
	pBar->GetClientRect(&rc);
	CRect rcControl = pControl->GetRect();
	CRect rcRow = pControl->GetRowRect();

	if (!pControl->GetWrap())
	{
		HorizontalLine(pDC, rcControl.left - 1 + GetPopupBarGripperWidth(pBar) + m_nPopupBarTextPadding, rcControl.top - 2, rcControl.right + 1, GetXtremeColor(XPCOLOR_SEPARATOR));
		HorizontalLine(pDC, rcControl.left - 1 + GetPopupBarGripperWidth(pBar) + m_nPopupBarTextPadding, rcControl.top - 1, rcControl.right + 1, GetXtremeColor(COLOR_3DHILIGHT));
	}
	else
	{
		VerticalLine(pDC, rcControl.left - 2, rcRow.top - 1, rcRow.bottom + 1, GetXtremeColor(XPCOLOR_SEPARATOR));
		VerticalLine(pDC, rcControl.left - 1, rcRow.top - 1, rcRow.bottom + 1, GetXtremeColor(COLOR_3DHILIGHT));
	}

	return 0;
}





//////////////////////////////////////////////////////////////////////////
// CXTPVisualStudio2010Theme

CXTPVisualStudio2010Theme::CXTPVisualStudio2010Theme()
{
	m_nPopupBarTextPadding = 10;
	m_pShadowManager->SetShadowOptions(xtpShadowOfficeAlpha);

	m_bFlatGripper = TRUE;
	m_bPlainSeparators = TRUE;

	m_bThemedStatusBar = TRUE;
	m_nToolbarSeparatorMargin = 2;
}

CXTPVisualStudio2010Theme::~CXTPVisualStudio2010Theme()
{
}

void CXTPVisualStudio2010Theme::RefreshMetrics()
{
	CXTPOffice2003Theme::RefreshMetrics();

	HWND hWnd = AfxGetMainWnd() ? AfxGetMainWnd()->GetSafeHwnd() : 0;
	m_themeStatusBar.OpenThemeData(hWnd, L"STATUS");


	m_clrCommandBar.SetStandardValue(RGB(188, 199, 216));
	m_clrDockBar.SetStandardValue(RGB(156,170,193));

	m_bLunaTheme = FALSE;

	m_arrColor[XPCOLOR_SPLITTER_FACE].SetStandardValue(XTPColorManager()->grcDockBar.clrDark);

	m_clrToolbarShadow.SetStandardValue(RGB(201, 210, 225));

	m_clrFloatingGripper.SetStandardValue(RGB(72, 92, 127));
	m_clrPopupControl.SetStandardValue(RGB(233, 236, 238));
	m_clrToolbarExpand.SetStandardValue(RGB(213,220,232));

	m_clrMenuExpandedGripper.SetStandardValue(RGB(203, 221, 246), RGB(121, 161, 220)); // TODO
	m_clrMenuExpand.SetStandardValue(0xFEEFE3, 0xE2A981); // TODO

	m_clrStatusPane.SetStandardValue(RGB(255, 255, 255));
	m_pShadowManager->SetShadowColor(RGB(0, 0, 0));
	m_clrTearOffGripper.SetStandardValue(RGB(169, 199, 240)); // TODO

	m_clrDisabledIcon.SetStandardValue(RGB(221, 227, 235), RGB(95, 100, 108));

	m_arrColor[XPCOLOR_EDITCTRLBORDER].SetStandardValue(RGB(133, 145, 162));
	m_arrColor[XPCOLOR_3DFACE].SetStandardValue(RGB(156, 170, 193));
	m_arrColor[XPCOLOR_3DSHADOW].SetStandardValue(RGB(96,114,140));
	m_arrColor[COLOR_APPWORKSPACE].SetStandardValue(RGB(47, 65, 95));
	m_arrColor[XPCOLOR_LABEL].SetStandardValue(RGB(233, 236, 238));

	INT nElements[] =
	{
		XPCOLOR_TOOLBAR_GRIPPER, XPCOLOR_SEPARATOR, XPCOLOR_DISABLED,
		XPCOLOR_MENUBAR_FACE, XPCOLOR_MENUBAR_BORDER, XPCOLOR_HIGHLIGHT,
		XPCOLOR_HIGHLIGHT_BORDER, XPCOLOR_HIGHLIGHT_PUSHED_BORDER, XPCOLOR_HIGHLIGHT_CHECKED_BORDER,
		XPCOLOR_HIGHLIGHT_PUSHED, XPCOLOR_HIGHLIGHT_CHECKED, XPCOLOR_TOOLBAR_FACE, XPCOLOR_PUSHED_TEXT
	};
	COLORREF clrValues[] =
	{
		RGB(97,115,140), RGB(133,145,162), RGB(128, 128, 128),
		RGB(221,226,232), RGB(155,167,183), RGB(255, 243, 206),
		RGB(231, 195, 99), RGB(229,195,101), RGB(229,195,101),
		RGB(255, 235, 165), RGB(255, 239, 189), RGB(188,199,216), 0
	};
	SetColors(sizeof(nElements)/sizeof(INT), nElements, clrValues);



	m_arrColor[XPCOLOR_HIGHLIGHT_DISABLED_BORDER].SetStandardValue(RGB(128, 128, 128));
	m_arrColor[XPCOLOR_TOOLBAR_GRAYTEXT].SetStandardValue(RGB(128, 128, 128));
	m_arrColor[XPCOLOR_MENUBAR_GRAYTEXT].SetStandardValue(RGB(128, 128, 128));
	m_arrColor[XPCOLOR_FLOATBAR_BORDER].SetStandardValue(m_clrFloatingGripper);

	m_clrMenuGripper.SetStandardValue(RGB(233, 236, 238));

	m_bLunaTheme = XTPSystemVersion()->IsWinVistaOrGreater();
	m_grcLunaChecked.SetStandardValue(RGB(255,251,241), RGB(255,238,190));
	m_grcLunaPushed.SetStandardValue(RGB(255,232,166), RGB(255,232,166));
	m_grcLunaSelected.SetStandardValue(RGB(255,252,242), RGB(255,236,181));

	m_clrStatusPane.SetStandardValue(RGB(41, 57, 85));
	m_clrStatusTextColor = RGB(255, 255, 255);

	m_arrColor[XPCOLOR_TOOLBAR_TEXT].SetStandardValue(RGB(28, 42, 63));
	m_clrMenuSeparator.SetStandardValue(RGB(190, 195, 203));

	if (!m_bLunaTheme)
	{
		m_clrDockBar.SetStandardValue(RGB(173, 186, 206));
		m_clrCommandBar.SetStandardValue(RGB(189, 199, 222));
		m_clrToolbarShadow.SetStandardValue(RGB(206, 211, 231));

		m_arrColor[XPCOLOR_MENUBAR_FACE].SetStandardValue(RGB(239, 239, 239));
		m_arrColor[XPCOLOR_MENUBAR_BORDER].SetStandardValue(RGB(156, 166, 181));
		m_arrColor[XPCOLOR_TOOLBAR_GRIPPER].SetStandardValue(RGB(99, 113, 140));
		m_arrColor[XPCOLOR_SEPARATOR].SetStandardValue(RGB(132, 146, 165));

		m_arrColor[XPCOLOR_TOOLBAR_TEXT].SetStandardValue(RGB(24, 40, 57));

		m_clrMenuSeparator.SetStandardValue(RGB(189, 195, 206));
	}

	CreateGradientCircle();

}

void CXTPVisualStudio2010Theme::AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical)
{
	CXTPOffice2003Theme::AdjustExcludeRect(rc, pControl, bVertical);
}

CRect CXTPVisualStudio2010Theme::GetCommandBarBorders(CXTPCommandBar* pBar)
{
	if (pBar->GetPosition() == xtpBarPopup && pBar->GetType() == xtpBarTypePopup)
		return CRect(3, 3, 3, 3);

	return CXTPOffice2003Theme::GetCommandBarBorders(pBar);
}

void CXTPVisualStudio2010Theme::DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded)
{
	CXTPOffice2003Theme::DrawPopupBarGripper(pDC, x, y, cx, cy, bExpanded);
}

void CXTPVisualStudio2010Theme::DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	if (barPosition == xtpBarPopup && barType == xtpBarTypePopup && bSelected && !bChecked && !bPressed)
	{
		if (m_bLunaTheme)
		{
			XTPDrawHelpers()->GradientFill(pDC, CRect(rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1), m_grcLunaSelected, FALSE);
		}
		else
		{
			pDC->FillSolidRect(CRect(rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1), GetXtremeColor(XPCOLOR_HIGHLIGHT));
		}

		COLORREF clrBorder = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);
		CXTPVisualStudio2008Theme::AlphaBorder(pDC, rc, clrBorder);

		return;
	}

	if (barType == xtpBarTypeMenuBar && bSelected && !bChecked && !bPressed && bEnabled && !bPopuped)
	{
		COLORREF clrBorder = GetXtremeColor(XPCOLOR_HIGHLIGHT_BORDER);
		CXTPVisualStudio2008Theme::AlphaBorder(pDC, rc, clrBorder);
		rc.DeflateRect(1, 1);
		ExcludeCorners(pDC, rc);

		if (m_bLunaTheme) XTPDrawHelpers()->GradientFill(pDC, rc, m_grcLunaSelected, FALSE);
		else pDC->FillSolidRect(rc, GetXtremeColor(XPCOLOR_HIGHLIGHT));

		return;
	}

	CXTPOffice2003Theme::DrawRectangle(pDC, rc, bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

COLORREF CXTPVisualStudio2010Theme::GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition)
{
	return CXTPOffice2003Theme::GetRectangleTextColor(bSelected, bPressed, bEnabled, bChecked, bPopuped, barType, barPosition);
}

CSize CXTPVisualStudio2010Theme::DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw)
{
	if (bDraw && pBar->GetType() == xtpBarTypePopup)
	{
		CRect rc;
		pBar->GetClientRect(&rc);
		CRect rcControl = pControl->GetRect();
		CRect rcRow = pControl->GetRowRect();

		if (!pControl->GetWrap())
			HorizontalLine(pDC, rcControl.left - 1 + GetPopupBarGripperWidth(pBar) + m_nPopupBarTextPadding,
				rcControl.top - 2, rcControl.right + 1, m_clrMenuSeparator);
		else
			VerticalLine(pDC, rcControl.left - 2, rcRow.top - 1, rcRow.bottom + 1, m_clrMenuSeparator);

		return 0;

	}
	return CXTPOffice2003Theme::DrawCommandBarSeparator(pDC, pBar, pControl, bDraw);
}

void CXTPVisualStudio2010Theme::FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar)
{
	CXTPClientRect rc((CWnd*)pBar);

	if (pBar->GetType() == xtpBarTypeMenuBar && pBar->GetPosition() != xtpBarFloating)
	{
		if (m_bLunaTheme)
			XTPDrawHelpers()->GradientFill(pDC, rc, RGB(200,209,224), RGB(174,185,205), IsVerticalPosition(pBar->GetPosition()));
		else
			pDC->FillSolidRect(rc, m_clrDockBar);
	}
	else if (pBar->GetPosition() == xtpBarTop || pBar->GetPosition() == xtpBarBottom)
	{
		COLORREF clr3DFace = GetXtremeColor(COLOR_3DFACE);
		pDC->FillSolidRect(rc, clr3DFace);

		CWnd* pWnd = pBar->GetParent();
		if (pWnd)
		{
			CXTPWindowRect rcFill((CWnd*)pWnd);
			pBar->ScreenToClient(rcFill);
			rcFill.right = rcFill.left + max(rcFill.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2);
			XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);
		}

		CRect rcFill = rc;
		rcFill.top += 1;
		rcFill.left += 2;

		pDC->FillSolidRect(rcFill.left + 1, rcFill.top, rcFill.Width() - 2, 1, m_clrCommandBar.clrDark);
		pDC->FillSolidRect(rcFill.left, rcFill.top + 1, rcFill.Width(), rcFill.Height() - 2, m_clrCommandBar.clrDark);
		pDC->FillSolidRect(rcFill.left + 1, rcFill.bottom - 1, rcFill.Width() - 2, 1, m_clrCommandBar.clrDark);

		CXTPVisualStudio2008Theme::AlphaBorder(pDC, rcFill, m_clrToolbarShadow);
	}
	else if (pBar->GetPosition() == xtpBarLeft || pBar->GetPosition() == xtpBarRight)
	{
		COLORREF clr3DFace = GetXtremeColor(COLOR_3DFACE);
		pDC->FillSolidRect(rc, clr3DFace);

		CWnd* pWnd = pBar->GetParent();
		if (pWnd)
		{
			if (pWnd->GetStyle() & CBRS_RIGHT)
				pDC->FillSolidRect(rc, m_clrDockBar.clrDark);
			else
			{
				CXTPWindowRect rcFill((CWnd*)pWnd);
				pBar->ScreenToClient(rcFill);
				rcFill.right = rcFill.left + max(rcFill.Width(), GetSystemMetrics(SM_CXFULLSCREEN) / 2);
				XTPDrawHelpers()->GradientFill(pDC, rcFill, m_clrDockBar, TRUE, rc);
			}
		}

		CRect rcFill = rc;
		rcFill.top += 2;
		rcFill.left += 1;

		pDC->FillSolidRect(rcFill.left + 1, rcFill.top, rcFill.Width() - 2, 1, m_clrCommandBar.clrDark);
		pDC->FillSolidRect(rcFill.left, rcFill.top + 1, rcFill.Width(), rcFill.Height() - 2, m_clrCommandBar.clrDark);
		pDC->FillSolidRect(rcFill.left + 1, rcFill.bottom - 1, rcFill.Width() - 2, 1, m_clrCommandBar.clrDark);

		CXTPVisualStudio2008Theme::AlphaBorder(pDC, rcFill, m_clrToolbarShadow);
	}
	else
	{
		CXTPOffice2003Theme::FillCommandBarEntry(pDC, pBar);
	}
}

void CXTPVisualStudio2010Theme::FillStatusBar(CDC* pDC, CXTPStatusBar* pBar)
{
	pDC->FillSolidRect(CXTPClientRect(pBar), RGB(41, 57, 85));
}

void CXTPVisualStudio2010Theme::DrawStatusBarGripper(CDC* pDC, CRect rcClient)
{
	if (m_bThemedStatusBar && m_themeStatusBar.IsAppThemed())
	{
		for (int y = 0; y <= 3; y++)
		{
			for (int x = 0; x <= 3 - y; x++)
			{
				int left = rcClient.right - x * 3 - 4;
				int top = rcClient.bottom - y * 3 - 4;

				pDC->SetPixel(left, top, RGB(248, 249, 250));
				pDC->SetPixel(left + 1, top, RGB(198, 206, 221));
				pDC->SetPixel(left, top + 1, RGB(227, 231, 238));
				pDC->SetPixel(left + 1, top + 1, RGB(170, 183, 205));

			}
		}

	}
	else
	{
		CXTPOffice2003Theme::DrawStatusBarGripper(pDC, rcClient);

	}
}
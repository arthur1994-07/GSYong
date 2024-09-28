#include "stdafx.h"
#include "MemBitmap.h"

enum
{
	BT_NORMAL,
	BT_TRANSPARENT,
	BT_ALPHABLEND,
};

CMemBitmap::CMemBitmap(CDC* pDC, const CRect& rect) : 
	m_pDC(pDC)
,	m_rect(rect)
,	m_dwType(BT_NORMAL)
,	m_pBmpOld(0)
,	m_dwRop(SRCCOPY)
{
	if (BeginDraw())
	{
		m_ptOldWndOrg = m_dcMemory.SetWindowOrg(m_rect.left, m_rect.top);
	}
}

CMemBitmap::~CMemBitmap()
{
	EndDraw();
}

bool CMemBitmap::BeginDraw(BOOL bAutoCreate)
{
	ASSERT(m_dcMemory.GetSafeHdc() == 0);
	ASSERT(m_rect.Width() >= 0 && m_rect.Height() >= 0);

	CWindowDC dctemp(0);
	CDC* pDC = &dctemp;

	if (m_dcMemory.GetSafeHdc() == 0 && m_dcMemory.CreateCompatibleDC(pDC) == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}

	CBitmap* pBmp = &m_bmp;

	if (bAutoCreate)
	{
		if (pBmp->GetSafeHandle())
		{
			pBmp->DeleteObject();
		}

		if (pBmp->CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height()) == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	else
	{
		ASSERT(pBmp->GetSafeHandle());
	}

	m_pBmpOld = (CBitmap*)m_dcMemory.SelectObject(&m_bmp);

	return true;
}

void CMemBitmap::EndDraw()
{
	if (m_pBmpOld)
	{
		BitBlt();

		m_dcMemory.SelectObject(m_pBmpOld);
	}
}

void CMemBitmap::BitBlt()
{
	CRect rectClip;
	int nClipType = m_pDC->GetClipBox(rectClip);

	CRect rectDraw;

	m_dcMemory.SetWindowOrg(m_ptOldWndOrg);

	if (rectDraw.IntersectRect(rectClip, m_rect))
	{
		int nSrcX = rectDraw.left - m_rect.left;
		int nSrcY = rectDraw.top - m_rect.top;

		if (m_dwType == BT_NORMAL)
		{
			m_pDC->BitBlt(rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
				&m_dcMemory, nSrcX, nSrcY, m_dwRop);
		}
		else if (m_dwType == BT_TRANSPARENT)
		{
			m_pDC->TransparentBlt(rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(), 
				&m_dcMemory, nSrcX, nSrcY, rectDraw.Width(), rectDraw.Height(), m_sBitBltInfo.clrTransparent);
		}
		else if (m_dwType == BT_ALPHABLEND)
		{
			m_pDC->AlphaBlend(rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
				&m_dcMemory, nSrcX, nSrcY, rectDraw.Width(), rectDraw.Height(), m_sBitBltInfo.sBlendFunction);
		}
	}
}

void CMemBitmap::SetBitBltInfo(COLORREF clrTransparent)
{
	m_dwType = BT_TRANSPARENT;
	m_sBitBltInfo.clrTransparent = clrTransparent;
}

void CMemBitmap::SetBitBltInfo(BLENDFUNCTION sBlendFunction)
{
	m_dwType = BT_ALPHABLEND;
	m_sBitBltInfo.sBlendFunction = sBlendFunction;
}

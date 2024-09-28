#include "stdafx.h"
#include "ControlSizer.h"

CControlSizer::CControlSizer() :
	m_pParent(0)
{

}

void CControlSizer::Init(CWnd* pParent)
{
	m_pParent = pParent;
}

bool CControlSizer::Insert(CWnd* pCtrl, eSizeType eWidthType, eSizeType eHeightType)
{
	CRect kParentRt, kCtrlRt;
	pCtrl->GetWindowRect(&kCtrlRt);

	m_pParent->GetWindowRect(&m_sParentRect);
	m_pParent->ScreenToClient(&kCtrlRt);

	if (m_sParentRect.Width() == 0 || m_sParentRect.Height() == 0)
	{
		return false;
	}

	CCtrlInfo kInfo;
	kInfo.eWidthType	= eWidthType;
	kInfo.eHeightType	= eHeightType;
	kInfo.pCtrl			= pCtrl;
	kInfo.sRect			= kCtrlRt;
	kInfo.fWidthRation	= static_cast<double>(kCtrlRt.left)	/ m_sParentRect.Width();
	kInfo.fHeightRation = static_cast<double>(kCtrlRt.top)	/ m_sParentRect.Height();

	m_kCtrlInfoContainer.Add(kInfo);

	return true;
}

void CControlSizer::OnSize(UINT nType, int cx, int cy)
{
	int nSize = m_kCtrlInfoContainer.GetSize();

    if (nSize > 0)
    {
	    HDWP hDwp = BeginDeferWindowPos(nSize);

	    CRect kParentRt = m_sParentRect;
	    m_pParent->GetWindowRect(&kParentRt);

	    for (int i = 0; i < nSize; ++i)
	    {
		    CCtrlInfo& kInfo = m_kCtrlInfoContainer.GetAt(i);

		    Calc(kParentRt, kInfo);

		    DeferWindowPos(hDwp, *kInfo.pCtrl, 0, 
			    kInfo.sRect.left, kInfo.sRect.top, kInfo.sRect.Width(), kInfo.sRect.Height(),
			    SWP_NOZORDER);
	    }

	    m_sParentRect = kParentRt;

	    EndDeferWindowPos(hDwp);
    }
}

void CControlSizer::Calc(CRect& sParent, CCtrlInfo& kInfo)
{
	CSize kDiff;
	CRect& kCtrlRect = kInfo.sRect;

	kDiff.cx = sParent.Width()	- m_sParentRect.Width();
	kDiff.cy = sParent.Height() - m_sParentRect.Height();

	switch(kInfo.eWidthType)
	{
	case ST_RESIZE:
		kCtrlRect.right		    += kDiff.cx;
		break;
	case ST_REPOS:
		kCtrlRect.left		    += kDiff.cx;
		kCtrlRect.right		    += kDiff.cx;
		break;
	case ST_RELATIVE:
		{
            LONG fLeft			= static_cast<LONG>(sParent.Width() * kInfo.fWidthRation);
			LONG fWidth			= kCtrlRect.Width();
			kCtrlRect.left		= fLeft;
			kCtrlRect.right		= kCtrlRect.left + fWidth;
		}
		break;
	}

	switch(kInfo.eHeightType)
	{
	case ST_RESIZE:
		kCtrlRect.bottom	    += kDiff.cy;
		break;
	case ST_REPOS:
		kCtrlRect.top		    += kDiff.cy;
		kCtrlRect.bottom	    += kDiff.cy;
		break;
	case ST_RELATIVE:
		{
			LONG fTop			= static_cast<LONG>(sParent.Height() * kInfo.fHeightRation);
			LONG fHeight		= kCtrlRect.Height();
			kCtrlRect.top		= fTop;
			kCtrlRect.bottom	= kCtrlRect.top + fHeight;
		}
		break;
	}
}

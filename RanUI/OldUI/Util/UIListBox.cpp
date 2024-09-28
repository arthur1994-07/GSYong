#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "UIListBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIListBox::CUIListBox(EngineDeviceMan* pEngineDevice) :
CUIGroup(pEngineDevice)
,	m_pSeleted(0)
,	m_pScrollBar(0)
,	m_fPercent(0)
,	m_nSelectedLine(-1)
,	m_fLineInterval(0.f)
,	m_fFrontInterval(0.f)
,	m_fBackInterval(0.f)
,	m_nTotalLine(0)
,	m_nVisibleLine(0)
,	m_nVisibleFirstIdx(0)
#ifndef RELEASE
,	m_bDebug(false)
#endif
{
	SetUseRender(FALSE);
}

CUIListBox::~CUIListBox()
{
}

bool CUIListBox::Create(const TCHAR* ctrl, const TCHAR* outline, const TCHAR* scroll, const float fLine, const float fFront, const float fBack)
{
	if (outline && CreateLineBox(outline) == false)
	{
		return false;
	}

	if (ctrl && CreateCtrl(ctrl) == false)
	{
		return false;
	}

	if (scroll && CreateScroll(scroll) == false)
	{
		return false;
	}

	m_fLineInterval = fLine;
	m_fFrontInterval = fFront;
	m_fBackInterval = fBack;

	CreateDebugLineBox(this);

	return true;
}

bool CUIListBox::CreateCtrl(const TCHAR* ctrl)
{
	m_strControl = ctrl;
	return true;
}

bool CUIListBox::CreateScroll(const TCHAR* scroll)
{
	if (scroll)
	{
		m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);

		if (m_pScrollBar)
		{
			m_pScrollBar->CreateSub(this, _T("BASIC_SCROLLBAR"), UI_FLAG_RIGHT | UI_FLAG_YSIZE);
			m_pScrollBar->CreateBaseScrollBar(const_cast<TCHAR*>(scroll), true);
			m_pScrollBar->GetThumbFrame()->SetScrollMoveLine(1);
			RegisterControl(m_pScrollBar);
			CreateDebugLineBox(m_pScrollBar);
			return true;
		}
	}

	return false;
}

bool CUIListBox::CreateLineBox(const TCHAR* outline)
{
	m_pSeleted = new CBasicLineBoxSmart(m_pEngineDevice);

	if (m_pSeleted)
	{
		m_pSeleted->CreateSub(this, _T("BASIC_LISTBOX_SELECTED"));
		m_pSeleted->CreateSubControl(outline);
		m_pSeleted->SetVisibleSingle(FALSE);
		RegisterControl(m_pSeleted);		
		return true;
	}

	return false;
}

void CUIListBox::CreateDebugLineBox(CUIControl* pControl)
{
#ifndef RELEASE
	if (m_bDebug)
	{
		CBasicLineBoxSmart* pDebug = new CBasicLineBoxSmart(m_pEngineDevice);

		if (pDebug)
		{
			pDebug->CreateSub(this, _T("DEBUG_LISTBOX"));
			pDebug->CreateSubControl(_T("DEBUG_LISTBOX_TEXTURE"));
			RegisterControl(pDebug);
			SetControlGlobalRect(pDebug, pControl->GetGlobalPos());
		}
	}
#endif
}

bool CUIListBox::UpdateIsScrolled()
{
	if (m_pScrollBar)
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame();

		if (pThumbFrame)
		{
			float fPercent = pThumbFrame->GetPercent();

			if (m_fPercent != pThumbFrame->GetPercent())
			{
				m_fPercent = fPercent;
				return true;
			}
		}
	}

	return false;
}

void CUIListBox::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);	

	if (bFirstControl == FALSE)
	{
		return;
	}

	if (UpdateMousePos(x, y))
	{
		UpdateSelected();
	}

	if (UpdateIsScrolled())
	{
		UpdateBeScrolled();
	}
}

CUIControl* CUIListBox::GetControl(int nIndex)
{
	CUIControl* pControl = 0;

	if (m_kControls.size() > size_t(nIndex))
	{
		pControl = m_kControls[nIndex];
	}

	return pControl;
}

const UIRECT CUIListBox::GetRectLineFromControl(int nLine)
{
	CUIControl* pControl = GetControl(nLine);

	UIRECT rt;

	if (pControl)
	{
		rt = pControl->GetGlobalPos();
	}

	return rt;
}

const UIRECT CUIListBox::GetRectLine(int nLine)
{
	const UIRECT& rcGlobalPos = GetRectLineFromControl(nLine);

	return UIRECT(
		rcGlobalPos.left - m_fFrontInterval, 
		rcGlobalPos.top - m_fLineInterval,
		rcGlobalPos.sizeX + m_fFrontInterval + m_fBackInterval, 
		rcGlobalPos.sizeY + m_fLineInterval * 2);
}

const UIRECT CUIListBox::GetRectLine(int nLine, CUIControl* pControl)
{
	const UIRECT& rcParentPos = GetGlobalPos();
	const UIRECT& rcGlobalPos = pControl->GetGlobalPos();

	return UIRECT(
		rcParentPos.left, 
		rcParentPos.top + (rcGlobalPos.sizeY + m_fLineInterval * 2) * nLine + m_fLineInterval, 
		rcGlobalPos.sizeX + m_fFrontInterval + m_fBackInterval, 
		rcGlobalPos.sizeY + m_fLineInterval * 2);
}

bool CUIListBox::UpdateMousePos(int x, int y)
{
	if (m_kControls.empty() || m_pSeleted == 0)
	{
		return false;
	}

	DWORD dwMsg = GetMessageEx();

	if (CHECK_MOUSE_IN(dwMsg) == false || CHECK_LB_UP_LIKE(dwMsg) == false)
	{
		return false;
	}

	UIRECT rcPart;

	for (int i = m_nVisibleFirstIdx; i < m_nVisibleFirstIdx + m_nVisibleLine; ++i)
	{
		rcPart = GetRectLine(i);

		if ((x >= rcPart.left && x <= rcPart.right) && (y >= rcPart.top && y <= rcPart.bottom))
		{
			if (i != m_nSelectedLine)
			{
				m_nSelectedLine = i;

				CUIControl* pParent = GetParent();

				if (pParent)
				{
					pParent->TranslateUIMessage(GetWndID(), LB_SELCHANGED);
				}

				return true;
			}

			break;
		}
	}

	return false;
}

void CUIListBox::UpdateScrollThumb()
{
	// 문자열이 변경 될 때마다
	// 스크롤의 썸을 변경 한다.
	if (m_pScrollBar)
	{
		if (m_nTotalLine != 0)
		{
			m_pScrollBar->GetThumbFrame()->SetState(m_nTotalLine, m_nVisibleLine);
		}
		else
		{
			m_pScrollBar->GetThumbFrame()->SetState(1, 1);
		}
	}
}

void CUIListBox::UpdateBeScrolled()
{
	UpdateVisibleLineToUnVisible();
	UpdateVisibleLineFirstIdx();
	UpdateVisibleLinePosWithVisible();
	UpdateSelected();
}

void CUIListBox::UpdateSelected()
{
	if (m_pSeleted)
	{
		BOOL bVisible = FALSE;
		const int nVisibleIndex = m_nSelectedLine - m_nVisibleFirstIdx;

		// 현재 선택된 라인이 보여지는지 체크 한다.
		if (nVisibleIndex >= 0 && nVisibleIndex < m_nVisibleLine)
		{
			UIRECT global = GetRectLine(m_nSelectedLine);
			SetControlGlobalRect(m_pSeleted, global);
			bVisible = TRUE;
		}

		m_pSeleted->SetVisibleSingle(bVisible);
	}
}

void CUIListBox::UpdateVisibleLine()
{
	UIRECT area, ctrl;	
	ctrl = GetGlobalPos();

	m_nVisibleLine = 0;	

	for (int i = m_nVisibleFirstIdx; i < m_nTotalLine; ++i)
	{
		area = GetRectLine(i);

		if (ctrl.left > area.left || ctrl.top > area.top || 
			ctrl.right < area.right || ctrl.bottom < area.bottom)
		{
			continue;
		}

		++m_nVisibleLine;
	}
}

void CUIListBox::UpdateVisibleLineFirstIdx()
{
	if (m_pScrollBar)
	{
		CBasicScrollThumbFrame* pThumbFrame = m_pScrollBar->GetThumbFrame();

		if (pThumbFrame)
		{
			const int nMovableLine = m_nTotalLine - m_nVisibleLine;
			float fPercent = pThumbFrame->GetPercent();

			m_nVisibleFirstIdx = (int)floor(fPercent * nMovableLine);

			if (m_nVisibleFirstIdx < 0)
			{
				m_nVisibleFirstIdx = 0;
			}
		}
	}
}

void CUIListBox::UpdateVisibleLinePosWithVisible()
{
	UIRECT rt = GetGlobalPos();

	for (int i = m_nVisibleFirstIdx; i < m_nVisibleFirstIdx + m_nVisibleLine; ++i)
	{
		CUIControl* pControl = GetControl(i);

		if (pControl)
		{
			UIRECT global = GetRectLine(i - m_nVisibleFirstIdx, pControl);
			SetControlGlobalPos(pControl, D3DXVECTOR2(global.left + m_fFrontInterval, global.top + m_fLineInterval));
			pControl->SetVisibleSingle(GetVisible(i));
		}
	}
}

void CUIListBox::UpdateVisibleLineToVisible()
{
	for (int i = m_nVisibleFirstIdx; i < m_nVisibleFirstIdx + m_nVisibleLine; ++i)
	{
		CUIControl* pControl = GetControl(i);

		if (pControl)
		{
			pControl->SetVisibleSingle(GetVisible(i));
		}
	}
}

void CUIListBox::UpdateVisibleLineToUnVisible()
{
	for (int i = m_nVisibleFirstIdx; i < m_nVisibleFirstIdx + m_nVisibleLine; ++i)
	{
		CUIControl* pControl = GetControl(i);

		if (pControl)
		{
			pControl->SetVisibleSingle(FALSE);
		}
	}
}


void CUIListBox::SetControlGlobalPos(CUIControl* pCtrl, const D3DXVECTOR2& rtGlobalPos)
{
	if (pCtrl)
	{
		CUIControl* pParent = pCtrl->GetParent();

		if (pParent)
		{
			UIRECT parentGlobalPos = pParent->GetGlobalPos();

			D3DXVECTOR2 rtLocalPos;
			rtLocalPos.x = rtGlobalPos.x - parentGlobalPos.left;
			rtLocalPos.y = rtGlobalPos.y - parentGlobalPos.top;

			pCtrl->SetLocalPos(rtLocalPos);
		}

		pCtrl->SetGlobalPos(rtGlobalPos);
	}
}

void CUIListBox::SetControlGlobalRect(CUIControl* pCtrl, const UIRECT& rtGlobalRect)
{
	if (pCtrl)
	{
		CUIControl* pParent = pCtrl->GetParent();

		if (pParent)
		{
			UIRECT parentGlobalPos = pParent->GetGlobalPos();

			UIRECT rtLocalPos;
			rtLocalPos.left = rtGlobalRect.left - parentGlobalPos.left;
			rtLocalPos.top = rtGlobalRect.top - parentGlobalPos.top;
			rtLocalPos.sizeX = rtGlobalRect.sizeX;
			rtLocalPos.sizeY = rtGlobalRect.sizeY;

			pCtrl->SetLocalPos(rtLocalPos);
		}

		pCtrl->SetGlobalPos(rtGlobalRect);
	}
}

void CUIListBox::SetPosControl(CUIControl* pControl)
{
	if (pControl)
	{
		UIRECT box;
		D3DXVECTOR2 pos;

		if (m_nTotalLine > 0)
		{
			box = GetRectLine(m_nTotalLine - 1);
			box.top = box.bottom;
		}
		else
		{
			box = GetRectLine(0, pControl);
		}

		pos = D3DXVECTOR2(box.left + m_fFrontInterval, box.top + m_fLineInterval);

		SetControlGlobalPos(pControl, pos);
	}
}

CUIControl* CUIListBox::CreateControl(const TCHAR* pControl)
{
	CUIControl* pUIControl = new CUIControl(m_pEngineDevice);

	if (pUIControl == 0)
	{
		return pUIControl;
	}

	pUIControl->CreateSub(this, pControl, UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
	pUIControl->LoadTexture(m_pEngineDevice->GetDevice());
	pUIControl->SetVisibleSingle(FALSE);    
	RegisterControl(pUIControl);
	SetPosControl(pUIControl);

	return pUIControl;
}

void CUIListBox::AddItem(const TCHAR* item, ...)
{
	va_list va;
	va_start(va, item);

	CUIControl* pControl = CreateControl(item);

	if (pControl)
	{
		m_kControls.push_back(pControl);

		++m_nTotalLine;

		UpdateVisibleLineToUnVisible();
		UpdateVisibleLine();
		UpdateVisibleLineToVisible();
		UpdateScrollThumb();
	}

	va_end(va);
}

void CUIListBox::SetItem(int nIndex, const TCHAR* item, ...)
{
	CUIControl* pControl = GetControl(nIndex);

	if (pControl)
	{
		pControl->CreateSub(this, item, UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);

		UpdateVisibleLineToUnVisible();
		UpdateVisibleLine();
		UpdateVisibleLineToVisible();
		UpdateScrollThumb();
	}
}

void CUIListBox::DelItem(int nIndex)
{
	CUIControl* pControl = GetControl(nIndex);

	if (pControl)
	{
		DeleteControl(pControl->GetWndID(), 0);
		m_kControls.erase(m_kControls.begin() + nIndex);
		m_kVisible.erase(nIndex);

		--m_nTotalLine;

		UpdateVisibleLineToUnVisible();
		UpdateVisibleLineFirstIdx();
		UpdateVisibleLinePosWithVisible();
		UpdateVisibleLine();
		UpdateScrollThumb();
	}
}

void CUIListBox::ClearItem()
{
	for (ControlContainerIterator it = m_kControls.begin(); it != m_kControls.end(); ++it)
	{
		DeleteControl((*it)->GetWndID(), false);
	}

	m_kControls.clear();
	m_kVisible.clear();

	m_nTotalLine = 0;
	m_nVisibleFirstIdx = 0;
	m_nVisibleLine = 0;
	m_nSelectedLine = -1;
	m_fPercent = 0.f;

	UpdateScrollThumb();
}

void CUIListBox::SetCurSel(int nSelect)
{
	if (nSelect != m_nSelectedLine)
	{
		m_nSelectedLine = nSelect;
		UpdateSelected();
	} 
}

void CUIListBox::SetVisible(int nIndex, bool bVisible)
{
	if (nIndex >= 0)
	{
		m_kVisible[nIndex] = bVisible;

		CUIControl* pCtrl = GetControl(nIndex);

		if (pCtrl)
		{
			pCtrl->SetVisibleSingle(bVisible ? TRUE : FALSE);
		}
	}
}

bool CUIListBox::GetVisible(int nIndex)
{
	if (m_kVisible.find(nIndex) != m_kVisible.end())
	{
		return m_kVisible[nIndex];
	}

	return true;
}

#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"

#include "UIStringListBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIStringListBox::CUIStringListBox(EngineDeviceMan* pEngineDevice, const DWORD dwFont, const int nTextAlign) :
	CUIListBox(pEngineDevice)
,	m_dwDefaultFont(dwFont)
,	m_nDefaultAlign(nTextAlign)
{
	
}

CUIStringListBox::~CUIStringListBox()
{
}

void CUIStringListBox::AddItem(const TCHAR* item, ...)
{
	va_list va;
	va_start(va, item);

	CBasicTextBoxEx* pTextBox = CreateTextBox(m_dwDefaultFont, m_nDefaultAlign);

	if (pTextBox)
	{
		pTextBox->SetOneLineText(item, va_arg(va, DWORD));
		m_kControls.push_back(pTextBox);
		++m_nTotalLine;

		UpdateVisibleLineToUnVisible();
		UpdateVisibleLine();
		UpdateVisibleLineToVisible();
		UpdateScrollThumb();
	}

	va_end(va);
}

CBasicTextBoxEx* CUIStringListBox::CreateTextBox(DWORD font, WORD align)
{
	CBasicTextBoxEx* pTextBox = 0;

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, font, _DEFAULT_FONT_FLAG);

	if (pFont)
	{
		pTextBox = new CBasicTextBoxEx(m_pEngineDevice);

		if (pTextBox)
		{
			pTextBox->CreateSub(this, m_strControl.c_str(), UI_FLAG_DEFAULT);
			pTextBox->SetFont(pFont);
			pTextBox->SetTextAlign(align);
			pTextBox->SetVisibleSingle(FALSE);

			// 센시티브는 영역 밖을 클릭했을 경우
			// 선택 라인을 초기화 하는 옵션 이다.
			pTextBox->SetSensitive(false);
			RegisterControl(pTextBox);
			SetPosControl(pTextBox);

			CreateDebugLineBox(pTextBox);
		}
	}

	return pTextBox;
}

void CUIStringListBox::SetItem(int nIndex, const TCHAR* item, ...)
{
    va_list va;
    va_start(va, item);

    CBasicTextBoxEx* pTextBox = dynamic_cast<CBasicTextBoxEx*>(GetControl(nIndex));

    if (pTextBox)
    {
        pTextBox->SetOneLineText(item, va_arg(va, DWORD));
    }

    va_end(va);
}

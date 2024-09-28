#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Msg/PVEMsg.h"

#include "UIStringListBox.h"
#include "UIOrdinal.h"
#include "PVEResult.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVEResult::CPVEResult(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIWindowEx(pInterface, pEngineDevice)
,	m_pExit(0)
,	m_pTime(0)
,	m_nTimer(0)
,	m_fCur(0.f)
,   m_pGaeaClient(pGaeaClient)
,   m_pOrdinal(0)
,   m_pClear(0)
{
    memset(m_pListBox, 0, sizeof(m_pListBox));
    memset(m_pColumn, 0, sizeof(m_pColumn));
}

CPVEResult::~CPVEResult()
{
}

bool CPVEResult::CreateSubControl()
{
    CreateLineBox("COMPETITION_PVE_CLEAR_UPLINEBOX", "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO");
    CreateLineBox("COMPETITION_PVE_CLEAR_DOWNLINEBOX", "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO");

    CreateListBox(LB_CLASS, _T("COMPETITION_PVE_CLEAR_CLASS_COLUMN_TEXTBOX"), ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 2),
        _T("COMPETITION_PVE_CLEAR_CLASS_LISTBOX"), _T("COMPETITION_PVE_CLEAR_CLASS_IMAGEBOX"), 1, 0, 0);

    CreateListBox(LB_CLASSNAME, _T("COMPETITION_PVE_CLEAR_CHARNAME_COLUMN_TEXTBOX"), ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 3),
        _T("COMPETITION_PVE_CLEAR_CHARNAME_LISTBOX"), _T("COMPETITION_PVE_CLEAR_CHARNAME_TEXTBOX"), 9, 0, 0);

    CreateListBox(LB_DAMAGE, _T("COMPETITION_PVE_CLEAR_DAMAGE_COLUMN_TEXTBOX"), ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 9),
        _T("COMPETITION_PVE_CLEAR_DAMAGE_LISTBOX"), _T("COMPETITION_PVE_CLEAR_DAMAGE_TEXTBOX"), 9, 0, 0);

    CreateListBox(LB_HEAL, _T("COMPETITION_PVE_CLEAR_HEAL_COLUMN_TEXTBOX"), ID2GAMEWORD("CTF_RESULT_TEXT_BASE", 6),
        _T("COMPETITION_PVE_CLEAR_HEAL_LISTBOX"), _T("COMPETITION_PVE_CLEAR_HEAL_TEXTBOX"), 9, 0, 0);

    CreateListBox(LB_BONUS, _T("COMPETITION_PVE_CLEAR_BONUS_COLUMN_TEXTBOX"), ID2GAMEWORD("COMPETITION_PVE", 20),
        _T("COMPETITION_PVE_CLEAR_BONUS_LISTBOX"), _T("COMPETITION_PVE_CLEAR_BONUS_TEXTBOX"), 9, 0, 0);

    CreateListBox(LB_MVP, 0, 0, _T("COMPETITION_PVE_CLEAR_MVP_LISTBOX"), _T("COMPETITION_PVE_CLEAR_MVP_IMAGEBOX"), 1, 0, 0);

	m_pExit = CreateButton(_T("COMPETITION_PVE_CLEAR_EXIT_BUTTON"), ID2GAMEWORD(_T("CTF_RESULT_BUTTON"), 4), CONTROL_ID_BUTTON_EXIT);
	m_pTime = CreateTextBox(9, _DEFAULT_FONT_SHADOW_FLAG, _T("COMPETITION_PVE_CLEAR_TIMER_TEXTBOX"), UI_FLAG_CENTER_X, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);

    if (m_pOrdinal == 0)
    {
        m_pOrdinal = new CUIOrdinal(m_pEngineDevice);

        if (m_pOrdinal == 0)
        {
            return false;
        }

        m_pOrdinal->CreateSub(this, _T("COMPETITION_PVE_CLEAR_STAGE_ORDINAL"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
        m_pOrdinal->CreateSubControl();
        RegisterControl(m_pOrdinal);
    }

    if (m_pClear == 0)
    {
        m_pClear = new CUIControl(m_pEngineDevice);

        if (m_pClear == 0)
        {
            return false;
        }

        m_pClear->CreateSub(this, _T("STAGE_ALLCLEAR_IMAGE"), UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
        RegisterControl(m_pClear);
    }

	return true;
}

void CPVEResult::SetTimer(int nTime)
{
	m_nTimer = nTime;
	m_fCur = static_cast<float>(nTime);
}

CBasicTextButton* CPVEResult::CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id)
{
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);

	if (pButton == 0)
	{
		return pButton;
	}

	pButton->CreateSub(this, _T("BASIC_TEXT_BUTTON22"), UI_FLAG_XSIZE, id);
	pButton->CreateBaseButton(name, CBasicTextButton::SIZE22, CBasicButton::CLICK_FLIP, text);
	RegisterControl(pButton);	

	return pButton;
}

bool CPVEResult::CreateListBox(const int nIndex, const TCHAR* pColumn, const TCHAR* pColumnWord, 
                               const TCHAR* pListBox, const TCHAR* pTextBoxInListBox, float fLine, float fFront, float fBack)
{
    if (nIndex == LB_CLASS || nIndex == LB_MVP)
    {
        m_pListBox[nIndex] = new CUIListBox(m_pEngineDevice);
    }
    else
    {
	    m_pListBox[nIndex] = new CUIStringListBox(m_pEngineDevice, 9, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
    }

    if (pColumn)
    {
        m_pColumn[nIndex] = CreateTextBox(9, _DEFAULT_FONT_SHADOW_FLAG, pColumn, UI_FLAG_CENTER_X, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
		if ( m_pColumn[nIndex] )
		{
			m_pColumn[nIndex]->AddOneLineText(pColumnWord, TRUE, NS_UITEXTCOLOR::WHITESMOKE);
		}
    }

	if (pListBox && m_pListBox[nIndex])
	{
        m_pListBox[nIndex]->CreateSub(this, pListBox, UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y);
	    m_pListBox[nIndex]->Create(pTextBoxInListBox, 0, 0, fLine, fFront, fBack);
	    RegisterControl(m_pListBox[nIndex]);
    }

	return m_pListBox[nIndex];
}

CBasicTextBox* CPVEResult::CreateTextBox(DWORD dwFontSize, DWORD dwFlags, const TCHAR* pControl, WORD wUIAlign, int nTextAlign)
{
	CBasicTextBox* pText = new CBasicTextBox(m_pEngineDevice);

	if (pText == 0)
	{
		return NULL;
	}

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, dwFontSize, dwFlags);

	if (pFont == 0)
	{
		return NULL;
	}

	pText->CreateSub(this, pControl, wUIAlign);
	pText->SetFont(pFont);
	pText->SetTextAlign(nTextAlign);
	RegisterControl(pText);

	return pText;
}

void CPVEResult::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroup::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CONTROL_ID_BUTTON_EXIT:
		{
			if (CHECK_MOUSE_IN(dwMsg))
			{
				if (CHECK_LB_UP_LIKE(dwMsg))
				{
					OnExit();
				}
			}
		}
		break;
	}
}

void CPVEResult::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	if (m_fCur > 0)
	{
		m_fCur -= fElapsedTime;

		if (m_fCur < 0)
		{
			m_fCur = 0;
			OnCompleted();
		}
	}

	if (m_pTime)
	{
		TCHAR temp[55] = {0};
		_stprintf_s(temp, 55, ID2GAMEWORD(_T("COMPETITION_PVE"), 16), int(m_fCur) / 60, int(m_fCur) % 60);
		m_pTime->SetOneLineText(temp, NS_UITEXTCOLOR::LIGHTYELLOW);
	}
}

void CPVEResult::OnExit()
{
    if (m_pGaeaClient)
    {
        m_pGaeaClient->ReqPVEExit();
    }

    if (m_pInterface)
    {
        m_pInterface->UiHideGroup(COMPETITION_PVECLEAR);
    }
}

void CPVEResult::OnCompleted()
{
    if (m_pInterface)
    {
        m_pInterface->UiHideGroup(COMPETITION_PVECLEAR);
    }
}

void CPVEResult::AddClass(DWORD dwClass)
{
	if (m_pListBox[LB_CLASS] && dwClass < GLCI_NUM_ETC && dwClass >= 0)
	{
		static const std::string strClass[GLCI_NUM_ETC] = 
		{
			"CREATE_CHAR_CLASS_BRAWLER_MALE",
			"CREATE_CHAR_CLASS_SWORDMAN_MALE",
			"CREATE_CHAR_CLASS_ARCHER_FEMALE",
			"CREATE_CHAR_CLASS_SHAMAN_FEMALE",
			"CREATE_CHAR_CLASS_EXTREME_MALE",
			"CREATE_CHAR_CLASS_EXTREME_FEMALE",
			"CREATE_CHAR_CLASS_BRAWLER_FEMALE",
			"CREATE_CHAR_CLASS_SWORDMAN_FEMALE",
			"CREATE_CHAR_CLASS_ARCHER_MALE",
			"CREATE_CHAR_CLASS_SHAMAN_MALE",
			"CREATE_CHAR_CLASS_SCIENTIST_MALE",
			"CREATE_CHAR_CLASS_SCIENTIST_FEMALE",
			"CREATE_CHAR_CLASS_ASSASSIN_MALE",
			"CREATE_CHAR_CLASS_ASSASSIN_FEMALE",
			"CREATE_CHAR_CLASS_TRICKER_MALE",
			"CREATE_CHAR_CLASS_TRICKER_FEMALE",
			"CREATE_CHAR_CLASS_ETC_MALE",
			"CREATE_CHAR_CLASS_ETC_FEMALE",
		};

		m_pListBox[LB_CLASS]->AddItem(strClass[dwClass].c_str());
	}
}

void CPVEResult::AddCharName(const TCHAR* pCharName)
{
	if (m_pListBox[LB_CLASSNAME])
	{
		m_pListBox[LB_CLASSNAME]->AddItem(pCharName, NS_UITEXTUTIL::WHITESMOKE);
	}
}

void CPVEResult::AddDamage(int nDamage)
{
	if (m_pListBox[LB_DAMAGE])
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nDamage);
		m_pListBox[LB_DAMAGE]->AddItem(temp, NS_UITEXTUTIL::WHITESMOKE);
	}
}

void CPVEResult::AddHeal(int nHeal)
{
	if (m_pListBox[LB_HEAL])
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nHeal);
		m_pListBox[LB_HEAL]->AddItem(temp, NS_UITEXTUTIL::WHITESMOKE);
	}
}

void CPVEResult::AddBonus(bool bExp, bool bMoney)
{
	if (m_pListBox[LB_BONUS])
	{
		TCHAR temp[255] = {0};

        if (bExp && bMoney)
        {
            _stprintf_s(temp, 255, _T("%s, %s"), ID2GAMEWORD(_T("COMPETITION_PVE"), 17), ID2GAMEWORD(_T("COMPETITION_PVE"), 18));
        }
        else if (bExp)
        {
            _stprintf_s(temp, 255, _T("%s"), ID2GAMEWORD(_T("COMPETITION_PVE"), 17));
        }
        else if (bMoney)
        {
            _stprintf_s(temp, 255, _T("%s"), ID2GAMEWORD(_T("COMPETITION_PVE"), 18));
        }

		m_pListBox[LB_BONUS]->AddItem(temp, D3DCOLOR_ARGB(0xFF,0x00,0xF0,0x00));
	}
}

void CPVEResult::AddMVP(bool bMVP)
{
    if (m_pListBox[LB_MVP])
    {
        m_pListBox[LB_MVP]->AddItem(_T("MVP_IMAGE"));

        if (bMVP == false)
        {
            const int nCur = m_pListBox[LB_MVP]->GetTotalLine();
            m_pListBox[LB_MVP]->SetVisible(nCur - 1, false);
        }
    }
}

void CPVEResult::SetStageResult()
{
    int nTimer = 0;
    int nStage = 0;
    bool bLast = 0;
    GLMSG::StageResult results;

	Clear();

    if (m_pGaeaClient)
    {
        m_pGaeaClient->GetPVEStageResult(results, nStage, bLast, nTimer);

        SetStage(nStage, bLast);
        SetTimer(nTimer);

        for (GLMSG::StageResultIt it = results.begin(); it != results.end(); ++it)
        {
            AddClass(it->dwClass);
            AddCharName(it->chaName.c_str());
            AddDamage(it->dwDamage);
            AddHeal(it->dwHeal);
            AddBonus(it->bExp, it->bMoney);
            AddMVP(it->bMVP);
        }
    }

    if (m_pInterface)
    {
        m_pInterface->UiShowGroupFocus(COMPETITION_PVECLEAR);
    }
}

void CPVEResult::SetStage(int nStage, bool bLast)
{
    if (m_pOrdinal)
    {
        m_pOrdinal->SetOrdinal(nStage, bLast == false);
    }

    if (m_pClear)
    {
        m_pClear->SetVisibleSingle(bLast ? TRUE : FALSE);
    }
}

void CPVEResult::Clear()
{
    for (int i = 0; i < LB_QUANTITY; ++i)
    {
        if (m_pListBox[i])
        {
            m_pListBox[i]->ClearItem();
        }
    }
}

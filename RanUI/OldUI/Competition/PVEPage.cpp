#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Land/GLMapList.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/PVE/GLPVEClient.h"
#include "../../../RanLogicClient/TriggerSystem/ClientTriggerSystem.h"

#include "UIStringListBox.h"
#include "PVEPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;
using namespace TriggerSystem;

CPVEPage::CPVEPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CCompetitionPage(pGaeaClient, pInterface, pEngineDevice)
,	m_pGaeaClient(pGaeaClient)
,	m_pInterface(pInterface)
,	m_pMap(0)
,	m_pText_Title(0)
,	m_pText_Info(0)
,	m_pListbox(0)
,	m_pLogic(0)
{
    if (m_pGaeaClient)
    {
		m_pLogic = m_pGaeaClient->GetPVEClient();
    }
}

CPVEPage::~CPVEPage()
{
    
}

void CPVEPage::CreateSubControl()
{
	// 박스 라인들
	CreateLineBox("COMPETITION_CTF_PAGE_INFO_LINEBOX_HEAD",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");
	CreateLineBox("COMPETITION_CTF_PAGE_INFO_LINEBOX_BODY",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
	CreateLineBox("COMPETITION_PVE_PAGE_INFO_LINEBOX_MAP", "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
	CreateLineBox("COMPETITION_PVE_DOWN_LINEBOX", "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO");
	CreateLineBox("COMPETITION_PVE_LINE_LISTBOX", "COMPETITION_PVE_PAGE_LISTBOX_TEXINFO_W_BLACK");

	// 입장 버튼
	m_pEntranceButton = new CBasicTextButton(m_pEngineDevice);
	m_pEntranceButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_ENTRANCE);
	m_pEntranceButton->CreateBaseButton("COMPETITION_OBJECT_ENTRANCE", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_PVE", 0));
	RegisterControl(m_pEntranceButton);

    // 귀속 해제 버튼
	m_pClearOfBelongingButton = new CBasicTextButton(m_pEngineDevice);
	m_pClearOfBelongingButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_CLEAROFBELONGING);
	m_pClearOfBelongingButton->CreateBaseButton("COMPETITION_OBJECT_CLEAROFBELONGING", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_PVE", 1));
	RegisterControl(m_pClearOfBelongingButton);

	// 문자열
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG);
	CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG);
	const int nAlignLeftTop = TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP;	
	const int nAlignLeftCenter = TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y;	
	m_pText_Title = CreateStaticControl("COMPETITION_CTF_PAGE_INFO_TITLE", pFont_Shadow, nAlignLeftCenter);	
	m_pText_Info  = CreateStaticControl("COMPETITION_PVE_PAGE_INFO_TEXT", pFont, nAlignLeftTop);    

	// 이미지
	m_pMap = CreateSwapImage("COMPETITION_PVE_PAGE_INFO_MAP");

	// 리스트 박스
	m_pListbox = new CUIStringListBox(m_pEngineDevice);
	m_pListbox->CreateSub(this, _T("COMPETITION_PVE_LISTBOX"), UI_FLAG_DEFAULT, CONTROL_ID_LISTBOX_INSTANCEMAP);
	m_pListbox_Title = CreateStaticControl("COMPETITION_PVE_LISTBOX_TITLE", pFont_Shadow, nAlignLeftCenter);
	m_pListbox_Title->SetOneLineText(ID2GAMEWORD("COMPETITION_PVE", 2), NS_UITEXTCOLOR::WHITESMOKE);

	if (m_pListbox)
	{
		m_pListbox->Create("COMPETITION_PVE_LISTBOX_TEXTBOX", "COMPETITION_PVE_LISTBOX_SELECTED_YELLOWLINE", "COMPETITION_PVE_LISTBOX_SCROOL", 1, 10, 1);
		RegisterControl(m_pListbox);
	}

    Init();
}

void CPVEPage::Init()
{
	UpdateListBox();
    SetCurSel(0);
    Update();
}

void CPVEPage::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIPage::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
    UpdateListBox();
}

void CPVEPage::Update()
{
    UpdateSelCur();
}

void CPVEPage::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroup::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CONTROL_ID_BUTTON_ENTRANCE:
	case CONTROL_ID_BUTTON_CLEAROFBELONGING:
		{
			if (CHECK_MOUSE_IN(dwMsg))
			{
				if (CHECK_LB_UP_LIKE(dwMsg))
				{
					if (ControlID == CONTROL_ID_BUTTON_ENTRANCE)
					{
						OnEntrance();
					}
					else
					{
						OnClearOfBelonging();
					}
				}
			}
		}
		break;

    case CONTROL_ID_LISTBOX_INSTANCEMAP:
        {
            if (dwMsg == CUIListBox::LB_SELCHANGED)
            {
                UpdateSelCur();
            }
        }
        break;
	}
}

void CPVEPage::SetTitle(const TCHAR* pTitle)
{
	if (m_pText_Title)
	{
		m_pText_Title->SetOneLineText(pTitle, NS_UITEXTCOLOR::ORANGE);
	}
}

void CPVEPage::SetMapInfo(const TCHAR* pMapInfo)
{
    if (m_pText_Info)
    {
        m_pText_Info->SetOneLineText(pMapInfo, NS_UITEXTCOLOR::WHITE);
    }
}

void CPVEPage::SetReward(const TCHAR* pReward)
{
    if (m_pText_Info)
    {
        m_pText_Info->ReplaceWord(0, _T("%s5"), _T(" "));
        m_pText_Info->ReplaceText(0, _T("%s6"), pReward, NS_UITEXTCOLOR::PALEGREEN);
    }
}

void CPVEPage::SetLevel(int nMin, int nMax)
{
	if (m_pText_Info)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nMin);
		m_pText_Info->ReplaceWord(0, _T("%s1"), temp, NS_UITEXTCOLOR::WHITE);
		_stprintf_s(temp, 10, _T("%d"), nMax);
		m_pText_Info->ReplaceWord(0, _T("%s7"), temp, NS_UITEXTCOLOR::WHITE);
	}
}

void CPVEPage::SetPerson(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s2"), temp, NS_UITEXTCOLOR::WHITE);
	}
}

void CPVEPage::SetNumber(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s3"), temp, NS_UITEXTCOLOR::WHITE);
	}
}

void CPVEPage::SetRemNumber(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[256] = {0};
		_stprintf_s(temp, 256, ID2GAMEINTEXT("COMPETITION_PVE_REMTODAY_TEXT", 0), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s4"), temp, NS_UITEXTCOLOR::ORANGE);
	}
}

void CPVEPage::SetInstanceMap(const TCHAR* pTitle, int nLevel, bool bPossible, _TIME_TYPE sec, int nIndex)
{
    if (m_pListbox)
    {
        TSTRING str;
        DWORD dwColor = bPossible ? NS_UITEXTUTIL::WHITESMOKE : NS_UITEXTCOLOR::RED;
        int nWordIndex = 21 + nLevel;

        str = ID2GAMEWORD("COMPETITION_PVE", nWordIndex);
        str += pTitle;

        if (bPossible == false)
        {
            TCHAR temp[50] = {0};
            _stprintf_s(temp, 50, _T("(%s)"), ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3));
            str += temp;
        }
        else if (sec > 0)
        {
            TCHAR temp[128] = {0};

            int nMin = int(sec) / 60;
            int nSec = int(sec) % 60;

            if (nMin > 0)
            {
                _stprintf_s(temp, 128, _T("(%s %02d%s %02d%s)"), ID2GAMEWORD("COMPETITION_PVE", 24), nMin, 
                    ID2GAMEWORD("ITEM_BASIC_TIME", 1), nSec, ID2GAMEWORD("ITEM_BASIC_TIME", 2));
            }
            else
            {
                _stprintf_s(temp, 128, _T("(%s %02d%s)"), ID2GAMEWORD("COMPETITION_PVE", 24), nSec,
                    ID2GAMEWORD("ITEM_BASIC_TIME", 2));
            }

            str += temp;
        }

        if (m_pListbox->GetTotalLine() > nIndex)
        {
            m_pListbox->SetItem(nIndex, str.c_str(), dwColor);
        }
        else
        {
			m_pListbox->AddItem(str.c_str(), dwColor);            
        }
    }
}

void CPVEPage::SetMapImage(SNATIVEID mid)
{
	const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(mid);	

	if (pMapNode)
	{
		m_pMap->SetImageTexture(pMapNode->m_LoadingImageName.c_str());
	}
}

void CPVEPage::OnEntrance()
{
	if (m_pGaeaClient)
	{
        int nLevel = 0;		
        TriggerMapInfo* pInfo = 0;

        if (GetSelCurInfo(pInfo, nLevel) && pInfo->levelinfo[nLevel].possible)
        {
			MAP_TABLE* pTable = 0;

			if (GetSelCurTable(pTable))
			{
				m_pGaeaClient->ReqPVEEntrance(pTable->dwCharDbNum, pInfo->mapid.dwID, pInfo->gateid, nLevel);
			}
			else
			{
				GLCharacter* pChar = m_pGaeaClient->GetCharacter();

				if (pChar)
				{
					m_pGaeaClient->ReqPVEEntrance(pChar->GetCharID(), pInfo->mapid.dwID, pInfo->gateid, nLevel);
				}
			}
        }
	}
}

void CPVEPage::OnClearOfBelonging()
{
    if (m_pGaeaClient)
    {
        MAP_TABLE* pTable = 0;

        if (GetSelCurTable(pTable) && pTable->tGenTime > 0.f)
        {
            m_pGaeaClient->ReqPVEClearOfBelonging(pTable->dwCharDbNum, pTable->dwMapID, pTable->nLevel);
        }
    }
}

bool CPVEPage::FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable)
{
    if (m_pLogic)
    {
		return m_pLogic->FindTable(mid.dwID, nLevel, pTable);
    }

    return false;
}

bool CPVEPage::GetSelCurInfo(TriggerMapInfo*& info, int& level)
{
    if (m_pListbox && m_pLogic)
    {
        size_t sel = static_cast<size_t>(m_pListbox->GetCurSel());
        size_t cur = 0;

		TriggerSystem::TriggerMapInfoList& kMapInfo = m_pLogic->GetMapInfo();

        for (TriggerSystem::TriggerMapInfoListIt it = kMapInfo.begin(); it !=  kMapInfo.end(); ++it)
        {
            for (TriggerLevelInfoIt dit = it->levelinfo.begin(); dit != it->levelinfo.end(); ++dit, ++cur)
            {
                if (cur == sel)
                {
                    info = &*it;
                    level = dit->second.level;
                    return true;
                }
            }
        }
    }

    return false;
}

bool CPVEPage::GetSelCurTable(MAP_TABLE*& pTable)
{
    int nLevel = 0;
    TriggerMapInfo* pInfo = 0;

    if (GetSelCurInfo(pInfo, nLevel))
    {
		return FindTable(pInfo->mapid.dwID, nLevel, pTable);
    }

    return false;
}

void CPVEPage::UpdateListBox()
{
	if (m_pListbox == 0 || m_pGaeaClient == 0 || m_pLogic == 0)
	{
		return;
	}

    int nIndex = 0;
	MAP_TABLE* table = 0;
	TriggerSystem::TriggerMapInfoList& kMapInfo = m_pLogic->GetMapInfo();

	if (m_pLogic)
	{
		if (m_pLogic->IsUpdateDB())
		{
			Update();
			m_pLogic->ToggleUpdateDB();
		}
	}

    for (TriggerSystem::TriggerMapInfoListIt it = kMapInfo.begin(); it !=  kMapInfo.end(); ++it)
    {
        for (TriggerLevelInfoIt dit = it->levelinfo.begin(); dit != it->levelinfo.end(); ++dit, ++nIndex)
        {
			int nLevel = dit->second.level;

            if (FindTable(it->mapid, nLevel, table))
            {
				SetInstanceMap(ID2GAMEWORD(it->mapname.c_str(), 0), nLevel, it->levelinfo[nLevel].possible, table->tGenTime, nIndex);
            }
            else
            {
				SetInstanceMap(ID2GAMEWORD(it->mapname.c_str(), 0), nLevel, it->levelinfo[nLevel].possible, 0, nIndex);
            }
        }
    }
}

void CPVEPage::SetCurSel(int nIndex)
{
    if (m_pListbox)
    {
        m_pListbox->SetCurSel(nIndex);
    }
}

void CPVEPage::UpdateSelCur()
{
    int nLevel = 0;
    TriggerMapInfo* pInfo = 0;

    if (GetSelCurInfo(pInfo, nLevel))
    {
        SetTitle(ID2GAMEWORD(pInfo->mapname.c_str(), 0));

        SetMapInfo(ID2GAMEINTEXT(pInfo->mapinfo.c_str(), 0));

        SetLevel(pInfo->levelinfo[nLevel].minlevel, pInfo->levelinfo[nLevel].maxlevel);
        SetPerson(pInfo->person);
        SetMapImage(pInfo->mapid);
        SetNumber(pInfo->dailylimit);

        int nRemNumber = pInfo->dailylimit;

        MAP_TABLE* pTable = 0;

        if (GetSelCurTable(pTable))
        {
            nRemNumber -= pTable->nInCount;
        }

        SetRemNumber(nRemNumber >= 0 ? nRemNumber : 0);

        SetReward(ID2GAMEINTEXT(pInfo->mapreward.c_str(), 0));
    }
}

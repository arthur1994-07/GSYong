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

//#include "UIStringListBox.h"
#include "InstanceListBox.h"
#include "WailBuilding.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;
using namespace TriggerSystem;

RnWailBuinding::RnWailBuinding(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
: RnCompetitionObject(pGaeaClient, pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
//, m_pMap(0)
, m_pText_Title(0)
, m_pText_Info(0)
//, m_pListbox(0)
, m_pLogic(0)
{
	if (m_pGaeaClient)
	{
		m_pLogic = m_pGaeaClient->GetPVEClient();
	}
}

RnWailBuinding::~RnWailBuinding()
{

}

void RnWailBuinding::CreateSubControl()
{
	RnCompetitionObject::CreateSubControlEx();
	// 입장 버튼
	m_pEntranceButton = new CBasicTextButton(m_pEngineDevice);
	m_pEntranceButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_ENTRANCE);
	m_pEntranceButton->CreateBaseButton("RN_WAIL_BUILDING_ENTRANCE", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_PVE", 0));
	RegisterControl(m_pEntranceButton);

	// 귀속 해제 버튼
	m_pClearOfBelongingButton = new CBasicTextButton(m_pEngineDevice);
	m_pClearOfBelongingButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_CLEAROFBELONGING);
	m_pClearOfBelongingButton->CreateBaseButton("RN_WAIL_BUILDING_CLEAROFBELONGING", CBasicTextButton::SIZE22, 
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
	//m_pMap = CreateSwapImage("COMPETITION_PVE_PAGE_INFO_MAP");

	//// 리스트 박스
	//m_pListbox = new CUIStringListBox(m_pEngineDevice);
	//m_pListbox->CreateSub(this, _T("COMPETITION_PVE_LISTBOX"), UI_FLAG_DEFAULT, CONTROL_ID_LISTBOX_INSTANCEMAP);
	//m_pListbox_Title = CreateStaticControl("COMPETITION_PVE_LISTBOX_TITLE", pFont_Shadow, nAlignLeftCenter);
	//m_pListbox_Title->SetOneLineText(ID2GAMEWORD("COMPETITION_PVE", 2), NS_UITEXTCOLOR::WHITESMOKE);

	//if (m_pListbox)
	//{
	//	m_pListbox->Create("COMPETITION_PVE_LISTBOX_TEXTBOX", "COMPETITION_PVE_LISTBOX_SELECTED_YELLOWLINE", "COMPETITION_PVE_LISTBOX_SCROOL", 1, 10, 1);
	//	RegisterControl(m_pListbox);
	//}

	SetNextWar(ID2GAMEWORD("RNCOMPETITION_NEXT_WAR"));
	SetRankText(ID2GAMEWORD("RNCOMPETITION_RANK"));
	SetInstanceListText(ID2GAMEWORD("RNCOMPETITION_INSTANCE_LIST"));
	SetObjectText(ID2GAMEWORD("RNCOMPETITION_OBJECT"));
	SetMoreInfoText(ID2GAMEWORD("RNCOMPETITION_MORE_INFO"));
	SetRewardText(ID2GAMEWORD("RNCOMPETITION_REWARD"));
    RnCompetitionObject::_SetEnableInfoButton(false);
    RnCompetitionObject::_SetEnableButton1(true);

	SetVisibleSingleContribution(FALSE);
	SetVisibleSingleExp(FALSE);
	SetVisibleSingleMoney(FALSE);
	SetVisibleSingleItem(FALSE);
	SetVisibleSingleBuff(FALSE);

	Init();
}

void RnWailBuinding::Init()
{
	UpdateListBox();
	SetCurSel(0);
	Update();
}

void RnWailBuinding::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIWindowObject::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	UpdateListBox();
	if(m_pButton1->IsVisible())
		m_pButton1->SetVisibleSingle(FALSE);
	if(m_pButton2->IsVisible())
		m_pButton2->SetVisibleSingle(FALSE);

}

void RnWailBuinding::Update()
{
	UpdateSelCur();
}

void RnWailBuinding::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
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

	case RnCompetitionObject::INSTANCE_LIST_BOX:
		{
			if (dwMsg == CUIListBox::LB_SELCHANGED)
			{
				UpdateSelCur();
			}
		}
		break;
	}
}

void RnWailBuinding::SetTitle(const TCHAR* pTitle)
{
	SetInstanceName(pTitle);
}

void RnWailBuinding::SetMapInfo(const TCHAR* pMapInfo)
{
	//if (m_pText_Info)
	//{
	//	m_pText_Info->SetOneLineText(pMapInfo, NS_UITEXTCOLOR::WHITE);
	//}
	SetMoreInfoInfoText(pMapInfo);
}

void RnWailBuinding::SetReward(const TCHAR* pReward)
{
	//if (m_pText_Info)
	//{
	//	m_pText_Info->ReplaceWord(0, _T("%s5"), _T(" "));
	//	m_pText_Info->ReplaceText(0, _T("%s6"), pReward, NS_UITEXTCOLOR::PALEGREEN);
	//}
	SetRewardInfoText(pReward);
}

void RnWailBuinding::SetLevel(int nMin, int nMax)
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

void RnWailBuinding::SetPerson(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s2"), temp, NS_UITEXTCOLOR::WHITE);
	}
}

void RnWailBuinding::SetNumber(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[10] = {0};
		_stprintf_s(temp, 10, _T("%d"), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s3"), temp, NS_UITEXTCOLOR::WHITE);
	}
}

void RnWailBuinding::SetRemNumber(int nNum)
{
	if (m_pText_Info)
	{
		TCHAR temp[256] = {0};
		_stprintf_s(temp, 256, ID2GAMEINTEXT("COMPETITION_PVE_REMTODAY_TEXT", 0), nNum);
		m_pText_Info->ReplaceWord(0, _T("%s4"), temp, NS_UITEXTCOLOR::ORANGE);
	}
}

void RnWailBuinding::SetInstanceMap(const TCHAR* pTitle, const std::string strEnterNum, int nLevel, bool bPossible, _TIME_TYPE sec, int nIndex)
{
	if (RnCompetitionObject::m_pInstanceListBox)
	{
		TSTRING str;
		DWORD dwColor = bPossible ? NS_UITEXTUTIL::WHITESMOKE : NS_UITEXTCOLOR::RED;
		int nWordIndex = 21 + nLevel;

		str = ID2GAMEWORD("COMPETITION_PVE", nWordIndex);
		str += pTitle;
		//m_pStatus->ClearText();
		if (bPossible == false)
		{
			// 여기에 신청가능 신청 불가 텍스트 갱신
			//TCHAR temp[50] = {0};
			//_stprintf_s(temp, 50, _T("(%s)"), ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3));
			//str += temp;
			//m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );

			str += strEnterNum;
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
			str += strEnterNum;
		}
		else
		{
			//m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 6), NS_UITEXTCOLOR::GREENYELLOW );
			str += strEnterNum;
		}

		if (RnCompetitionObject::m_pInstanceListBox->GetTotalLine() > nIndex)
		{
			RnCompetitionObject::m_pInstanceListBox->SetItem(nIndex, str.c_str(), dwColor);
		}
		else
		{
			RnCompetitionObject::m_pInstanceListBox->AddItem(str.c_str(), dwColor);            
		}
	}
}

void RnWailBuinding::SetMapImage(SNATIVEID mid)
{
	const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(mid);	

	if (pMapNode)
	{
		m_pMap->SetImageTexture(pMapNode->m_LoadingImageName.c_str());
	}
}

void RnWailBuinding::OnEntrance()
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

void RnWailBuinding::OnClearOfBelonging()
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

bool RnWailBuinding::FindTable(const DWORD dwChrNum, const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable)
{
	if (m_pLogic)
	{
		return m_pLogic->FindTable(dwChrNum, mid.dwID, nLevel, pTable);
	}

	return false;
}

bool RnWailBuinding::FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable)
{
	if (m_pLogic)
	{
		return m_pLogic->FindTable(mid.dwID, nLevel, pTable);
	}

	return false;
}

bool RnWailBuinding::GetSelCurInfo(TriggerMapInfo*& info, int& level)
{
	if (RnCompetitionObject::m_pInstanceListBox && m_pLogic)
	{
		size_t sel = static_cast<size_t>(RnCompetitionObject::m_pInstanceListBox->GetCurSel());
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

bool RnWailBuinding::GetSelCurTable(MAP_TABLE*& pTable)
{
	int nLevel = 0;
	TriggerMapInfo* pInfo = 0;

	if (GetSelCurInfo(pInfo, nLevel))
	{
		return FindTable(pInfo->mapid.dwID, nLevel, pTable);
	}

	return false;
}

void RnWailBuinding::UpdateListBox()
{
	if (RnCompetitionObject::m_pInstanceListBox == 0 || m_pGaeaClient == 0 || m_pLogic == 0)
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

	TriggerMapInfo* pInfo = 0;

	std::string strTextremToday ="";

	for (TriggerSystem::TriggerMapInfoListIt it = kMapInfo.begin(); it !=  kMapInfo.end(); ++it)
	{
		for (TriggerLevelInfoIt dit = it->levelinfo.begin(); dit != it->levelinfo.end(); ++dit, ++nIndex)
		{
			int nLevel = dit->second.level;

			if (GetSelCurInfo(pInfo, nLevel))
			{
				SetTitle(ID2GAMEWORD(pInfo->mapname.c_str(), 0));
				SetMapObject(ID2GAMEINTEXT(pInfo->mapObject.c_str()));

				
				int nRemNumber = it->dailylimit;

				MAP_TABLE* pTable = 0;
				FindTable(m_pGaeaClient->GetCharacter()->GetCharID(), it->mapid.dwID, nLevel, pTable);
				

				if(pTable)
					nRemNumber -= pTable->nInCount;
			

				SetRemNumber(nRemNumber >= 0 ? nRemNumber : 0);

				strTextremToday = sc::string::format( ID2GAMEINTEXT("WAIL_OF_THE_BUILDING_REMTODAY_TEXT"),nRemNumber);
			}

			TriggerSystem::TriggerMapInfoListIt itSelect = kMapInfo.begin();
			int sel = RnCompetitionObject::m_pInstanceListBox->GetCurSel();

			for(int i = 0; i < sel; ++i)
			{
				++itSelect;
			}

			m_pStatus->ClearText();

			if(itSelect->levelinfo[nLevel].possible)
				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 6), NS_UITEXTCOLOR::GREENYELLOW );
			else
				m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );

			if (FindTable(it->mapid, nLevel, table))
			{
				SetInstanceMap(ID2GAMEWORD(it->mapname.c_str(), 0), strTextremToday, nLevel, it->levelinfo[nLevel].possible, table->tGenTime, nIndex);
			}
			else
			{
				SetInstanceMap(ID2GAMEWORD(it->mapname.c_str(), 0), strTextremToday, nLevel, it->levelinfo[nLevel].possible, 0, nIndex);
			}
		}
	}
}

void RnWailBuinding::SetCurSel(int nIndex)
{
	if (RnCompetitionObject::m_pInstanceListBox)
	{
		RnCompetitionObject::m_pInstanceListBox->SetCurSel(nIndex);
	}
}

void RnWailBuinding::UpdateSelCur()
{
	int nLevel = 0;
	TriggerMapInfo* pInfo = 0;

	if (GetSelCurInfo(pInfo, nLevel))
	{
		SetTitle(ID2GAMEWORD(pInfo->mapname.c_str(), 0));
		SetMapObject(ID2GAMEINTEXT(pInfo->mapObject.c_str()));

		std::string strText = sc::string::format( ID2GAMEINTEXT(pInfo->mapinfoRn.c_str()) ,
			pInfo->dailylimit,
			"",
			pInfo->person, 
			pInfo->levelinfo[nLevel].minlevel,
			pInfo->levelinfo[nLevel].maxlevel);

		SetMapInfo(strText.c_str());
		SetMapImage(pInfo->mapid);
		SetReward(ID2GAMEINTEXT(pInfo->mapreward.c_str(), 0));
		//SetReward(pInfo->mapreward.c_str());
	}
}

CBasicTextBox* RnWailBuinding::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void RnWailBuinding::SetMapObject( const TCHAR* pTitle )
{
	SetObjectInfoText(pTitle);
}

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
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

#include "../Competition/UIStringListBox.h"
#include "TournamentPage.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;
using namespace TriggerSystem;
using namespace MatchingSystem;

CTournamentPage::CTournamentPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
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
		m_pLogic = m_pGaeaClient->GetTournamentClient();
	}
}

CTournamentPage::~CTournamentPage()
{

}

void CTournamentPage::CreateSubControl()
{
	// 박스 라인들
	CreateLineBox("COMPETITION_CTF_PAGE_INFO_LINEBOX_HEAD",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_GRAY");
	CreateLineBox("COMPETITION_CTF_PAGE_INFO_LINEBOX_BODY",	"COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
	CreateLineBox("COMPETITION_PVE_PAGE_INFO_LINEBOX_MAP", "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK");
	CreateLineBox("COMPETITION_PVE_DOWN_LINEBOX", "COMPETITION_CTF_PAGE_BATTLE_LINBOX_TEXINFO");
	CreateLineBox("COMPETITION_PVE_LINE_LISTBOX", "COMPETITION_PVE_PAGE_LISTBOX_TEXINFO_W_BLACK");

	// 참여하기 버튼
	m_pEntranceButton = new CBasicTextButton(m_pEngineDevice);
	m_pEntranceButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_ENTRANCE);
	m_pEntranceButton->CreateBaseButton("COMPETITION_OBJECT_ENTRANCE", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("TOURNAMENT_UI_TEXT_JOIN_BUTTON", 0));
	RegisterControl(m_pEntranceButton);

	// 정보보기
	m_pClearOfBelongingButton = new CBasicTextButton(m_pEngineDevice);
	m_pClearOfBelongingButton->CreateSub(this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, CONTROL_ID_BUTTON_MOREINFO);
	m_pClearOfBelongingButton->CreateBaseButton("COMPETITION_OBJECT_CLEAROFBELONGING", CBasicTextButton::SIZE22, 
		CBasicButton::CLICK_FLIP, ID2GAMEWORD("TOURNAMENT_UI_TEXT_INFO_BUTTON", 0));
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
	m_pListbox_Title->SetOneLineText(ID2GAMEWORD("COMPETITION_TOURNAMENT", 0), NS_UITEXTCOLOR::WHITESMOKE);

	if (m_pListbox)
	{
		m_pListbox->Create("COMPETITION_PVE_LISTBOX_TEXTBOX", "COMPETITION_PVE_LISTBOX_SELECTED_YELLOWLINE", "COMPETITION_PVE_LISTBOX_SCROOL", 1, 10, 1);
		RegisterControl(m_pListbox);
	}

	Init();
}

void CTournamentPage::Init()
{
	UpdateListBox();
	SetCurSel(0);
	Update();
}

void CTournamentPage::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIPage::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	UpdateListBox();
}

void CTournamentPage::Update()
{
	UpdateSelCur();
}

void CTournamentPage::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroup::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CONTROL_ID_BUTTON_ENTRANCE:
	case CONTROL_ID_BUTTON_MOREINFO:
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
						size_t sel = static_cast<size_t>(m_pListbox->GetCurSel());
						m_pInterface->OpenTournamentDashboard(sel);
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

void CTournamentPage::SetTitle(const TCHAR* pTitle)
{
	if (m_pText_Title)
	{
		m_pText_Title->SetOneLineText(pTitle, NS_UITEXTCOLOR::ORANGE);
	}
}

void CTournamentPage::SetMapInfo(const TCHAR* pMapInfo)
{
	if (m_pText_Info)
	{
		m_pText_Info->SetOneLineText(pMapInfo, NS_UITEXTCOLOR::WHITE);
	}
}

void CTournamentPage::AddMapInfo(const TCHAR* pMapInfo)
{
	if (m_pText_Info)
	{
		m_pText_Info->AddText(pMapInfo, NS_UITEXTCOLOR::WHITE);
		//m_pText_Info->AddTextWithCRLF();
	}
}

void CTournamentPage::SetReward(const TCHAR* pReward)
{
	if (m_pText_Info)
	{
		m_pText_Info->AddText(pReward, NS_UITEXTCOLOR::PALEGREEN);
	}
}

void CTournamentPage::SetTournamentMap(const TCHAR* pTitle, DWORD TornamentType, DWORD NowStatus, int nIndex)
{
	if (m_pListbox)
	{
		TSTRING str;
		DWORD dwColor;

		MATCHING_CONTENTS_STATUS _status = (MATCHING_CONTENTS_STATUS)NowStatus;

		switch(_status)
		{
		case MatchingSystem::EMMATCHING_CONTENTS_OPENED:
			dwColor= NS_UITEXTUTIL::LIGHTGREEN;
			break;
			/*
		case MatchingSystem::EMTORNAMENTSTATUS_PENDING_APPLICATION:
			dwColor= NS_UITEXTUTIL::LIGHTGREEN;
			*/
			break;
		case MatchingSystem::EMMATCHING_CONTENTS_STANDBY_JOIN:
			dwColor= NS_UITEXTUTIL::ORENGE;
			break;
		case MatchingSystem::EMMATCHING_CONTENTS_CLOSED:
			dwColor= NS_UITEXTUTIL::RED;
			break;
		case MatchingSystem::EMMATCHING_CONTENTS_IN_PROGRESS:
			dwColor= NS_UITEXTUTIL::WHITE;
			break;
		}

		//정시 상시 임시?
		int nWordIndex = TornamentType;


		int nstatusindex = _status;

		

		TCHAR temp[128] = {0};
		_stprintf_s(temp, 128, _T("[%s]%s(%s)"), ID2GAMEWORD("TOURNAMENT_EM_TORNAMENT_TYPE", nWordIndex), pTitle,
			ID2GAMEWORD("TOURNAMENT_EM_TORNAMENT_STATUS", nstatusindex));

		str += temp;

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

void CTournamentPage::SetMapImage(SNATIVEID mid)
{
	const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(mid);	

	if (pMapNode)
	{
		m_pMap->SetImageTexture(pMapNode->m_LoadingImageName.c_str());
	}
}

void CTournamentPage::OnEntrance()
{
	if (m_pGaeaClient)
	{
		int nLevel = 0;		
		GLMatchingInfo* pInfo = 0;

		if (GetSelCurInfo(pInfo) &&
			( pInfo->m_ContentsStatus == MatchingSystem::EMMATCHING_CONTENTS_OPENED ))
		{
			GLCharacter* pChar = m_pGaeaClient->GetCharacter();

			if (pChar)
			{
				//m_pGaeaClient->ReqPVEEntrance(pChar->GetCharID(), pInfo->mapid.dwID, pInfo->gateid, nLevel);
				m_pLogic->JoinTournament(pInfo->m_ScriptID);
			}

		}
	}
}

void CTournamentPage::OnPageVisible(bool _isOpen)
{
	if(_isOpen)
	{
		MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
		m_pLogic->GetTournamentInfomation();
	}
}


void CTournamentPage::OnClearOfBelonging()
{
	if (m_pGaeaClient)
	{
		GLMatchingInfo* pTable = 0;

		if (GetSelCurInfo(pTable))
		{
			//m_pGaeaClient->ReqPVEClearOfBelonging(pTable->dwCharDbNum, pTable->dwMapID, pTable->nLevel);
			//m_pGaeaClient->ReqPVEClearOfBelonging(pTable->dwCharDbNum, pTable->dwMapID, pTable->nLevel);
		}
	}
}


bool CTournamentPage::GetSelCurInfo(GLMatchingInfo*& info)
{
	if (m_pListbox && m_pLogic)
	{
		size_t sel = static_cast<size_t>(m_pListbox->GetCurSel());
		size_t cur = 0;

		GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = m_pLogic->m_VecMatchingInfo.begin();
		GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = m_pLogic->m_VecMatchingInfo.end();

		for (; _bitor != _eitor; ++_bitor)
		{
			if (cur == sel)
			{
				info = *_bitor;
				return true;
			}
			cur++;
		}
	}

	return false;
}


void CTournamentPage::UpdateListBox()
{
	if (m_pListbox == 0 || m_pGaeaClient == 0 || m_pLogic == 0)
	{
		return;
	}

	int nIndex = 0;
	MAP_TABLE* table = 0;

	BOOST_FOREACH(GLMatchingInfo* _pinfo,m_pLogic->m_VecMatchingInfo)
	{
		SetTournamentMap(ID2GAMEINTEXT(_pinfo->m_strContentsName.c_str(), 0), _pinfo->m_ContentsDateType,_pinfo->m_ContentsStatus, nIndex);
		nIndex++;
	}
}

void CTournamentPage::SetCurSel(int nIndex)
{
	if (m_pListbox)
	{
		m_pListbox->SetCurSel(nIndex);
	}
}

// 요기서 타이틀 집어 넣자.
void CTournamentPage::UpdateSelCur()
{
	int nLevel = 0;
	GLMatchingInfo* pInfo = 0;

	


	if (GetSelCurInfo(pInfo))
	{
		SetTitle(ID2GAMEINTEXT(pInfo->m_strContentsName.c_str(), 0));

		if(m_pEntranceButton )
		{
			MATCHING_CONTENTS_STATUS _status = (MATCHING_CONTENTS_STATUS)pInfo->m_ContentsStatus;
			//if(_status == MatchingSystem::EMTORNAMENTSTATUS_PENDING_APPLICATION)
			//	m_pEntranceButton->SetOneLineText(ID2GAMEWORD("TOURNAMENT_UI_TEXT_JOIN_BUTTON", 1));
		}
		SetMapInfo(ID2GAMEWORD("TOURNAMENT_INFOMATION", 0));
		AddMapInfo(ID2GAMEINTEXT(pInfo->m_strMissionInfo.c_str(),0));
		AddMapInfo(ID2GAMEWORD("TOURNAMENT_REWARD",0));
		SetReward(ID2GAMEINTEXT(pInfo->m_strRewardInfo.c_str(),0));
		SetMapImage(pInfo->m_dwContentsMapID);
	}
}

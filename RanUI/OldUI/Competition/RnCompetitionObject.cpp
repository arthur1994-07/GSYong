#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/MatchingJoinManagerClient.h"

#include "../../InnerInterface.h"

#include "../Util/UITextButton.h"

#include "./RnCompetitionObject.h"

#include "RnCompetitionRewardBuff.h"
#include "RnCompetitionRewardItem.h"

#include "InstanceListBox.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define InfoButtonDelayTime 1.0f
#define Button1_DelayTime 3.0f
//#define Button2_DelayTime 1.0f

RnCompetitionObject::RnCompetitionObject ( GLGaeaClient* pGaeaClient,  CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowObject ( pInterface, pEngineDevice )
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pInfoButton(NULL)
, m_pButton1(NULL)
, m_pButton2(NULL)
, m_pInstanceListBox(NULL)
, m_pMapNameTextBox(NULL)
, m_pNextWarTextBox(NULL)
, m_pRankTextBox(NULL)
, m_pInstanceListTextBox(NULL)
, m_pObjectTextBox(NULL)
, m_pObjectTextInfoBox(NULL)
, m_pMoreInfoTextBox(NULL)
, m_pMoreInfoInfoTextBox(NULL)
, m_pRewardTextBox(NULL)
, m_pRewardInfoTextBox(NULL)
, m_pStatus(NULL)
, m_pBuffText(NULL)
, m_pItemText(NULL)
, m_pMoneyText(NULL)
, m_pExpText(NULL)
, m_pContributionText(NULL)
, m_pMap(NULL)
, m_dwSelectID(-1)
, m_fInfoButton_Delay(0.0f)
, m_fButton1_Delay(0.0f)
, m_bInfoButton_Enable(false)
, m_bButton1_Enable(false)
{
}

RnCompetitionObject::~RnCompetitionObject ()
{
	// Blank
}

void RnCompetitionObject::CreateSubControlEx ()
{	
	// Note : 페이지 프레임
	{
		//CreateLineBox  ( "RNCOMPETITION_INSTANCE_TYPE_BG_LINE", "COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO" );
		CreateLineBox  ( "RNCOMPETITION_OBJECT_PAGE", "COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO" );
		CreateLineBox  ( "RNCOMPETITION_OBJECT_PAGE_RIGHT", "COMPETITION_OBJECT_PAGE_LINEBOX_TEXINFO" );
		CreatePageFrame( "RNCOMPETITION_OBJECT_PAGE" );
	}

	{
		//던전이름
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_MAP_BG",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_MAP_NAME",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );

		//다음 전투
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_NEXT_WAR",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_NEXT_WAR",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );

		//RANK
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_RANK",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );	
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_RANK",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );

		//인스턴스 전장리스트
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_INSTANCE_LIST",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );	
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_INSTANCE_LIST",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );

		CreateLineBox  ( "RNCOMPETITION_LINEBOX_INSTANCE_LIST_INFO",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );	
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_INSTANCE_LIST_INFO",  "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );

		//목표
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_OBJECT_INFO", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_OBJECT", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

		//상세정보
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_MORE_INFO_INFO", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_MORE_INFO", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );

		//보상
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_REWARD_INFO", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
		CreateLineBox  ( "RNCOMPETITION_LINEBOX_REWARD", "GRAY_BACKGROUND_BLACK_LINE_TEXTURE" );
	}

	{
		//던전 이름
		m_pMapNameTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_MAP_NAME_TEXT", " " ); //발전소 점령전
		// 다음 전투
		m_pNextWarTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_NEXT_WAR_TEXT", " " ); //다음전투
		// 다음 전투 시간
		m_pNextWarTimeTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_NEXT_WAR_TIME_TEXT", " ", TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y ); //다음전투
		// RANK
		m_pRankTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_RANK_TEXT", " " ); //RANK
		// 인스턴스 전장리스트
		m_pInstanceListTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_INSTANCE_LIST_TEXT", " " ); //인스턴스 전장리스트
		// 목표
		m_pObjectTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_OBJECT_TEXT", " " ); //목표
		// 상세정보
		m_pMoreInfoTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_MORE_INFO_TEXT", " "); //상세정보
		// 보상
		m_pRewardTextBox = 
			CreateTextBox( "RNCOMPETITION_LINEBOX_REWARD_TEXT", " " ); //보상
	}

	{
		//목표
		m_pObjectTextInfoBox =
			 CreateTextBox  ( "RNCOMPETITION_LINEBOX_OBJECT_INFO_TEXT", " ", TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
		//상세정보
		m_pMoreInfoInfoTextBox = 
			CreateTextBox  ( "RNCOMPETITION_LINEBOX_MORE_INFO_INFO_TEXT", " ", TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP  );
		//보상
		m_pRewardInfoTextBox = 
			CreateTextBox  ( "RNCOMPETITION_LINEBOX_REWARD_INFO_TEXT", " ", TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP  );
	}

	//! CTF 정보 맵 이미지
	{
		m_pMap =  new CSwapImage(m_pEngineDevice);
		m_pMap->CreateSub( this, "RNCOMPETITION_CTF_PAGE_INFO_MAP" );
		RegisterControl( m_pMap );
	}

	// Note : 정보보기 버튼
	{
		m_pInfoButton = new CBasicTextButton(m_pEngineDevice);
		m_pInfoButton->CreateSub(this, "RNCOMPETITION_INFO_RANK_BUTTON", UI_FLAG_XSIZE, INFO_BUTTON);
		m_pInfoButton->CreateBaseButton( "RNCOMPETITION_RANK_BUTTON", CBasicTextButton::SIZE18, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_BUTTON_TEXT_INFO", 0));
		RegisterControl( m_pInfoButton );
	}

	// Note : 닫기 버튼
	{
		m_pButton1 = new CBasicTextButton(m_pEngineDevice);
		m_pButton1->CreateSub(this, "RNCOMPETITION_BUTTON", UI_FLAG_XSIZE, BUTTON_1);
		m_pButton1->CreateBaseButton( "RNCOMPETITION_BUTTON_1", CBasicTextButton::SIZE22, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_WINDOW", 1) );
		RegisterControl( m_pButton1 );
	}

	// Note : 닫기 버튼
	{
		m_pButton2 = new CBasicTextButton(m_pEngineDevice);
		m_pButton2->CreateSub(this, "RNCOMPETITION_BUTTON", UI_FLAG_XSIZE, BUTTON_2);
		m_pButton2->CreateBaseButton( "RNCOMPETITION_BUTTON_2", CBasicTextButton::SIZE22, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD("COMPETITION_WINDOW", 1) );
		RegisterControl( m_pButton2 );
	}
	{
		// List Box;
		{
			m_pInstanceListBox = new InstanceListBox( m_pInterface, m_pEngineDevice );
			m_pInstanceListBox->CreateSub( this, "RNCOMPETITION_LINEBOX_LISTBOX", UI_FLAG_DEFAULT, INSTANCE_LIST_BOX );
			m_pInstanceListBox->Create( "RNCOMPETITION_LINEBOX_LISTBOX_ROW", 
				"YELLOW_LINE_GRAY_BACKGROUND", "RNCOMPETITION_LISTBOX_SCROOL", 1, 5, 1 );
			RegisterControl( m_pInstanceListBox );	
		}

		//m_pInstanceListBox->SetCurSel( 0 );
	}

	//보상
	{
		//기여도
		m_pContributionText = 
			CreateTextBox  ( "RNCOMPETITION_CONTRIBUTION_TEXT", 
			ID2GAMEWORD("RNCOMPETITION_CONTRIBUTION_TEXT"), 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
			NS_UITEXTUTIL::WHITE  );

		//경험치
		m_pExpText = 
			CreateTextBox  ( "RNCOMPETITION_EXP_TEXT", 
			ID2GAMEWORD("RNCOMPETITION_EXP_TEXT"), 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
			NS_UITEXTUTIL::WHITE   );
		//머니
		m_pMoneyText = 
			CreateTextBox  ( "RNCOMPETITION_MONEY_TEXT", 
			ID2GAMEWORD("RNCOMPETITION_MONEY_TEXT"), 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
			NS_UITEXTUTIL::WHITE   );
		//아이템
		m_pItemText = 
			CreateTextBox  ( "RNCOMPETITION_ITEM_TEXT", 
			ID2GAMEWORD("RNCOMPETITION_ITEM_TEXT"), 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
			NS_UITEXTUTIL::WHITE   );
		//버프
		m_pBuffText =
			CreateTextBox  ( "RNCOMPETITION_BUFF_TEXT", 
			ID2GAMEWORD("RNCOMPETITION_BUFF_TEXT"), 
			TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP, 
			NS_UITEXTUTIL::WHITE  );

		SetVisibleSingleContribution(FALSE);
		SetVisibleSingleExp(FALSE);
		SetVisibleSingleMoney(FALSE);
		SetVisibleSingleItem(FALSE);
		SetVisibleSingleBuff(FALSE);

		SetContributionText(" - ");
		SetExpText(" - ");
		SetMoneyText(" - ");
		SetItemText(" - ");
		SetBuffText(" - ");
	}

	for( int i = 0; i< 6; i++ )
	{
		RnCompetitionRewardBuff* pRnCompetitionRewardBuff = new RnCompetitionRewardBuff(m_pGaeaClient,m_pInterface, m_pEngineDevice );
		pRnCompetitionRewardBuff->CreateSub( this, 
			"RNCOMPETITION_CTF_PAGE_HISTORY_REWARDBUFF_ITEM", UI_FLAG_DEFAULT,CTF_HISTORY_BUFF_BEGIN_1 + i );
		pRnCompetitionRewardBuff->CreateSubControl();
		RegisterControl( pRnCompetitionRewardBuff );

		pRnCompetitionRewardBuff->SetSkill( SNATIVEID(29,1) );

		UIRECT rcSkillImageGPos = pRnCompetitionRewardBuff->GetGlobalPos();
		UIRECT rcSkillImageLPos = pRnCompetitionRewardBuff->GetLocalPos();

		int nGap = 5;

		pRnCompetitionRewardBuff->SetGlobalPos( D3DXVECTOR2((rcSkillImageGPos.left + rcSkillImageGPos.sizeX * i) + nGap * i, rcSkillImageGPos.top ) );
		pRnCompetitionRewardBuff->SetLocalPos( D3DXVECTOR2((rcSkillImageLPos.left + rcSkillImageLPos.sizeX * i) + nGap * i, rcSkillImageLPos.top ) );
		pRnCompetitionRewardBuff->SetVisibleSingle(FALSE);
		m_vecRnCompetitionRewardBuff.push_back(pRnCompetitionRewardBuff);
	}


	for(int i = 0; i < 6; i++ )
	{
		RnCompetitionRewardItem* pRnCompetitionRewardItem = new RnCompetitionRewardItem(m_pGaeaClient,m_pInterface, m_pEngineDevice );
		pRnCompetitionRewardItem->CreateSub( this, 
			"RNCOMPETITION_ITEM", UI_FLAG_DEFAULT);
		pRnCompetitionRewardItem->CreateSubControl();
		RegisterControl( pRnCompetitionRewardItem );

		pRnCompetitionRewardItem->SetItemId( SNATIVEID(83,5) );
		pRnCompetitionRewardItem->SetTurnNum(i*rand()%10 + 1);

		UIRECT rcSkillImageGPos = pRnCompetitionRewardItem->GetGlobalPos();
		UIRECT rcSkillImageLPos = pRnCompetitionRewardItem->GetLocalPos();

		int nGap = 5;

		pRnCompetitionRewardItem->SetGlobalPos( D3DXVECTOR2((rcSkillImageGPos.left + rcSkillImageGPos.sizeX * i) + nGap * i, rcSkillImageGPos.top ) );
		pRnCompetitionRewardItem->SetLocalPos( D3DXVECTOR2((rcSkillImageLPos.left + rcSkillImageLPos.sizeX * i) + nGap * i, rcSkillImageLPos.top ) );
		pRnCompetitionRewardItem->SetVisibleSingle(FALSE);
		m_vecRnCompetitionRewardItem.push_back(pRnCompetitionRewardItem);
	}

	//입장가능 상태
	{
		CD3DFontPar* pFont14 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 15, _DEFAULT_FONT_SHADOW_FLAG );
		m_pStatus =
			CreateTextBox  ( "RNCOMPETITION_CTF_PAGE_BATTLE_STATUS_TEXT", "", 
			TEXT_ALIGN_TOP | TEXT_ALIGN_CENTER_X, NS_UITEXTUTIL::WHITE, 14 );
	}

    // 기본적인 세팅;
    SetNextWar(ID2GAMEWORD("RNCOMPETITION_NEXT_WAR"));
    SetRankText(ID2GAMEWORD("RNCOMPETITION_RANK"));
    SetInstanceListText(ID2GAMEWORD("RNCOMPETITION_INSTANCE_LIST"));
    SetObjectText(ID2GAMEWORD("RNCOMPETITION_OBJECT"));
    SetMoreInfoText(ID2GAMEWORD("RNCOMPETITION_MORE_INFO"));
    SetRewardText(ID2GAMEWORD("RNCOMPETITION_REWARD"));
    SetButton1Text(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0));
}

void RnCompetitionObject::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case BUTTON_1:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				// 버튼1의 버튼 딜레이가 남아 있다면
				if ( m_fButton1_Delay > 0.0f )
				{
					m_pInterface->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT",10));
					break;
				}

				m_fButton1_Delay = Button1_DelayTime;  // 신청 딜레이 1초;
                m_pButton1->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
                m_pButton1->SetDiffuseAlpha( 0xDD );
                m_pButton1->SetNoMessage( TRUE );

                if ( m_dwSelectID != DWORD(-1) )
                {
                    MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
					if(m_pLogic->m_ClientMatchingStatus.size() == 0)
					{
						m_pLogic->JoinTournament(m_dwSelectID);
						break;
					}

					BOOST_FOREACH(MatchingSystem::MatchingClientInfo* m_ClientMatchingStatus,m_pLogic->m_ClientMatchingStatus  )
					{
						if(m_ClientMatchingStatus->ScriptID == m_dwSelectID)
						{
							switch ( m_ClientMatchingStatus->ClientStatus )
							{
							case MatchingSystem::EMSTATUS_GROUP_IN:     // 입장 대기 중;
								m_pLogic->CanselJoinTournament();
								break;
							default:
								m_pLogic->JoinTournament(m_dwSelectID);
								break;
							}
							break;
						}
					}

					
					
                }

				//for(int i = 0; i <50; i++)
				//{
				//	m_pInstanceListBox->AddItem(sc::string::format("%d",i).c_str());
				//}
			}
		}
		break;
	case BUTTON_2:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->UiHideGroup( RNCOMPETITION_WINDOW );
			}
		}
		break;
	case  INFO_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                // 정보보기 버튼 딜레이가 남아 있다면
                if ( m_fInfoButton_Delay > 0.0f )
				{
					m_pInterface->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT",10));
                    break;
				}

                m_fInfoButton_Delay = InfoButtonDelayTime;  // 정보보기 딜레이 1초;
				m_pInfoButton->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
				m_pInfoButton->SetDiffuseAlpha( 0xDD );
				m_pInfoButton->SetNoMessage( TRUE );

                if ( m_dwSelectID != DWORD(-1) )
                {
                    MatchingSystem::GLTouranmentClient* pTournament =  m_pGaeaClient->GetTournamentClient();
                    if ( pTournament == NULL )
                        return;

                    pTournament->SendCustomMessage(m_dwSelectID, MatchingSystem::EM_CUSTOM_MESSAGE_INFO_BUTTON, 0);

                    DWORD Infosize = pTournament->m_VecMatchingInfo.size();
                    for ( int i(Infosize); i--; )
                    {
                        MatchingSystem::GLMatchingInfo* _Info = (pTournament->m_VecMatchingInfo[i]);
                        if ( _Info->m_ScriptID == m_dwSelectID )
                        {
                            if ( _Info->m_ContentsType == MatchingSystem::EMMATCHING_CONTENTS_TOURNAMENT )
                            {
                                m_pInterface->OpenTournamentDashboard(m_dwSelectID);
                                break;
                            }
                        }

                    }
                }
			}
		}
		break;
	case CTF_HISTORY_BUFF_BEGIN_1:
	case CTF_HISTORY_BUFF_BEGIN_2:
	case CTF_HISTORY_BUFF_BEGIN_3:
	case CTF_HISTORY_BUFF_BEGIN_4:
	case CTF_HISTORY_BUFF_BEGIN_5:
	case CTF_HISTORY_BUFF_BEGIN_6:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( SNATIVEID(29,1) );

				if ( pSkill )
				{	
					CString strName;
					strName = pSkill->GetName();

					CString strDesc;
					pSkill->GetDescFormatByLevel( strDesc, 1, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );

					m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT );
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strName, NS_UITEXTCOLOR::GREENYELLOW );
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strDesc, NS_UITEXTCOLOR::WHITE );
					m_pInterface->END_COMMON_LINEINFO_MULTI();
				}
			}
		}
	}

	CUIWindowObject::TranslateUIMessage ( ControlID, dwMsg );
}

void RnCompetitionObject::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowObject::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    MatchingSystem::GLTouranmentClient* pTournament =  m_pGaeaClient->GetTournamentClient();
    if ( pTournament == NULL )
        return;

    const DWORD Infosize = pTournament->m_VecMatchingInfo.size();
    for ( int i(Infosize); i--; )
    {
        const MatchingSystem::GLMatchingInfo* _Info = (pTournament->m_VecMatchingInfo[i]);
        if ( _Info->m_ScriptID == m_dwSelectID )
        {
            _UpdateContentsInfo(_Info);
            break;
        }
    }

    if ( m_fInfoButton_Delay > 0.0f )
	{
        m_fInfoButton_Delay -= fElapsedTime;
		if ( m_fInfoButton_Delay < 0.0f )
		{
			m_fInfoButton_Delay = 0.0f;
            if ( m_bInfoButton_Enable == true )
            {
                m_pInfoButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
                m_pInfoButton->SetDiffuseAlpha( 0xFF );
                m_pInfoButton->SetNoMessage( FALSE );
            }
		}
	}

	if ( m_fButton1_Delay > 0.0f )
	{
		m_fButton1_Delay -= fElapsedTime;
		if ( m_fButton1_Delay < 0.0f )
		{
			m_fButton1_Delay = 0.0f;
            if ( m_bButton1_Enable == true )
            {
                m_pButton1->SetDiffuse( NS_UITEXTCOLOR::WHITE );
                m_pButton1->SetDiffuseAlpha( 0xFF );
                m_pButton1->SetNoMessage( FALSE );
            }
		}
	}
}

CBasicLineBoxSmart* RnCompetitionObject::CreateLineBox( const std::string& strControl, const std::string& strTexInfo )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControl.c_str() );
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );

	return pLineBox;
}

CBasicTextBox* RnCompetitionObject::CreateTextBox( const std::string& strControl, 
												  const std::string& strText, 
												  int nAlingflag, 
												  const DWORD TextColor, DWORD dwFontSize)
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, dwFontSize, _DEFAULT_FONT_SHADOW_FLAG );

	//던전 이름
	CBasicTextBox* pTextbox =	new CBasicTextBox( m_pEngineDevice );
	pTextbox->CreateSub( this, strControl.c_str(), UI_FLAG_DEFAULT );
	pTextbox->SetFont( pFont );
	pTextbox->SetTextAlign( nAlingflag );
	//pTextbox->SetText( strText.c_str() );
	//pTextbox->SetUseTextColor( 0, TRUE );
	//pTextbox->SetTextColor(0,NS_UITEXTCOLOR::ORANGE);
	RegisterControl( pTextbox );

	pTextbox->SetTextNoSplit( strText.c_str(), TextColor );
	return pTextbox;
}

void RnCompetitionObject::SetInstanceName( const char* szText )
{
	m_pMapNameTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetNextWar( const char* szText )
{
	m_pNextWarTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetNextWarTime( const char* szText )
{
    m_pNextWarTimeTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::WHITE );
}

void RnCompetitionObject::SetRankText( const char* szText )
{
	m_pRankTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}
void RnCompetitionObject::SetInstanceListText( const char* szText )
{
	m_pInstanceListTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetInfoButtonText( const char* szText )
{
	m_pInfoButton->SetOneLineText( szText );
}

void RnCompetitionObject::SetButton1Text( const char* szText )
{
	m_pButton1->SetOneLineText( szText );
}

void RnCompetitionObject::SetButton2Text( const char* szText )
{
	m_pButton2->SetOneLineText( szText );
}

void RnCompetitionObject::SetObjectText( const char* szText )
{
	m_pObjectTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetObjectInfoText( const char* szText )
{
	m_pObjectTextInfoBox->SetOneLineText( szText, NS_UITEXTCOLOR::WHITE );
}

void RnCompetitionObject::SetMoreInfoText( const char* szText )
{
	m_pMoreInfoTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetRewardText( const char* szText )
{
	m_pRewardTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::ORANGE );
}

void RnCompetitionObject::SetRewardInfoText( const char* szText )
{
	m_pRewardInfoTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::GREENYELLOW );
}

void RnCompetitionObject::SetContributionText( const char* szText )
{
	m_pContributionText->ClearText();
	m_pContributionText->SetTextNoSplit(CString(ID2GAMEWORD("RNCOMPETITION_CONTRIBUTION_TEXT")) + szText,
		NS_UITEXTCOLOR::WHITE);
}

void RnCompetitionObject::SetExpText( const char* szText )
{
	m_pExpText->ClearText();
	m_pExpText->SetTextNoSplit(CString(ID2GAMEWORD("RNCOMPETITION_EXP_TEXT")) + szText,
		NS_UITEXTCOLOR::WHITE);
}

void RnCompetitionObject::SetMoneyText( const char* szText )
{
	m_pMoneyText->ClearText();
	m_pMoneyText->SetTextNoSplit(CString(ID2GAMEWORD("RNCOMPETITION_MONEY_TEXT")) + szText,
		NS_UITEXTCOLOR::WHITE);
}

void RnCompetitionObject::SetItemText( const char* szText )
{
	m_pItemText->ClearText();
	m_pItemText->SetTextNoSplit(CString(ID2GAMEWORD("RNCOMPETITION_ITEM_TEXT")) + szText,
		NS_UITEXTCOLOR::WHITE);
}

void RnCompetitionObject::SetBuffText( const char* szText )
{
	m_pBuffText->ClearText();
	m_pBuffText->SetTextNoSplit(CString(ID2GAMEWORD("RNCOMPETITION_BUFF_TEXT")) + szText,
		NS_UITEXTCOLOR::WHITE);
}

void RnCompetitionObject::SetMoreInfoInfoText( const char* szText )
{
	m_pMoreInfoInfoTextBox->SetOneLineText( szText, NS_UITEXTCOLOR::WHITE );
}

void RnCompetitionObject::SetVisibleSingleContribution( bool bVisible )
{
	m_pContributionText->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleExp( bool bVisible )
{
	m_pExpText->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleMoney( bool bVisible )
{
	m_pMoneyText->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleItem( bool bVisible )
{
	m_pItemText->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleBuff( bool bVisible )
{
	m_pBuffText->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleButton1( bool bVisible )
{
	m_pButton1->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetVisibleSingleButton2( bool bVisible )
{
	m_pButton2->SetVisibleSingle(bVisible);
}

void RnCompetitionObject::SetInstanceList( DWORD dwInstanceID, const TCHAR* szName )
{
	m_pInstanceListBox->SetPropertyText( dwInstanceID, szName );
}

void RnCompetitionObject::AddInstanceList( const TCHAR* pInstNaem, DWORD dwInstanceID )
{
	m_pInstanceListBox->AddItem( pInstNaem, dwInstanceID );
}

void RnCompetitionObject::ClearInstanceList()
{
	m_pInstanceListBox->ClearItem();
}

void RnCompetitionObject::SetCurSel( int nSelect )
{
	m_pInstanceListBox->SetCurSel(nSelect);
}

DWORD RnCompetitionObject::GetSelectInstanceID()
{
	
	return m_pInstanceListBox->GetInstanceID();
}

void RnCompetitionObject::SelectContents( const DWORD _dwSelectScriptID )
{
    if ( m_dwSelectID == _dwSelectScriptID )
        return;

    MatchingSystem::GLTouranmentClient* pTournament =  m_pGaeaClient->GetTournamentClient();
    if ( pTournament == NULL )
        return;

    const DWORD Infosize = pTournament->m_VecMatchingInfo.size();
    for ( int i(Infosize); i--; )
    {
        const MatchingSystem::GLMatchingInfo* _Info = (pTournament->m_VecMatchingInfo[i]);
        if ( _Info->m_ScriptID == _dwSelectScriptID )
        {
            m_dwSelectID = _dwSelectScriptID;
            _SelectContentsInfo(_Info);
            return;
        }
    }
}

void RnCompetitionObject::_SelectContentsInfo( const MatchingSystem::GLMatchingInfo* _Info )
{
    SetInstanceName(ID2GAMEINTEXT(_Info->m_strContentsName.c_str()));
    SetObjectInfoText(ID2GAMEINTEXT(_Info->m_strMissionInfo.c_str()));
    SetMoreInfoInfoText(ID2GAMEINTEXT(_Info->m_strDetailInfo.c_str()));    
    SetRewardInfoText(ID2GAMEINTEXT(_Info->m_strRewardInfo.c_str()));
    _SetEnableInfoButton(_Info->m_bEnableInfoButton);

    const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(_Info->m_dwContentsMapID);
    if (pMapNode)
    {
        m_pMap->SetImageTexture(pMapNode->m_LoadingImageName.c_str());
    }
}

void RnCompetitionObject::_UpdateContentsInfo( const MatchingSystem::GLMatchingInfo* _Info )
{
    MatchingSystem::GLTouranmentClient* pTournament =  m_pGaeaClient->GetTournamentClient();
    if ( pTournament == NULL )
        return;

    m_pNextWarTimeTextBox->ClearText();
    switch( _Info->m_ContentsDateType )
    {
    case MatchingSystem::EMMATCHING_TIME_PERIODIC:
        {
            CTime cCurTime = m_pGaeaClient->GetCurrentTime();
            //	서버 타임으로 변환( TimeZone 계산 );
            m_pGaeaClient->GetConvertServerTime( cCurTime );
            int nDayOfWeek = cCurTime.GetDayOfWeek ();
            int nDay = cCurTime.GetDay ();
            int nHour = cCurTime.GetHour();
            int nMinute = cCurTime.GetMinute ();

            int nNearestDayInterval(0); // 가장 가까운 진행까지 남은 날짜;
            int nNearestInterval(INT_MAX);    // 가장 가까운 진행까지 남은 분;
            int nNearestTime(0);        // 가장 가까운 진행시간의 번호;

            const unsigned int numTimeInfo = _Info->m_vecTimeInfo.size();
            for ( unsigned int i(0); i < numTimeInfo; i++ )
            {
                int nDayInterval(0);
                if ( _Info->m_vecTimeInfo[i].wWeekDay != 0 )
                {
                    nDayInterval = _Info->m_vecTimeInfo[i].wWeekDay - nDayOfWeek;
                    if ( nDayInterval < 0 )
                    {
                        nDayInterval += 7; // 다음주에 열림;
                    }
                }

                const int nHourInterval = _Info->m_vecTimeInfo[i].wStartHour - nHour;
                const int nMinInterval = _Info->m_vecTimeInfo[i].wStartMin - nMinute;

                int nInterval = nDayInterval * 24 * 60;
                nInterval += nHourInterval * 60;
                nInterval += nMinInterval;

                if ( nInterval <= 0 )
                {

                    if ( -nInterval <= (int)_Info->m_vecTimeInfo[i].dwDurationMin ) // 진행중;
                    {
                        if ( _Info->m_ContentsType == MatchingSystem::EMMATCHING_CONTENTS_TOURNAMENT  &&
                            _Info->m_ContentsStatus == MatchingSystem::EMMATCHING_CONTENTS_CLOSED)
                        {
                            nDayInterval = (_Info->m_vecTimeInfo[i].wWeekDay != 0) ? 7 : 1 ;
                            nInterval += nDayInterval * 24 * 60; // 다음주 or 다음날 에 열림; 
                        }
                        else
                        {
                            SetNextWarTime(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7));
                            nNearestTime = -1;
                            break;
                        }
                    }
                    else // 진행시간 경과시;
                    {
                        nDayInterval = (_Info->m_vecTimeInfo[i].wWeekDay != 0) ? 7 : 1 ;
                        nInterval += nDayInterval * 24 * 60; // 다음주 or 다음날 에 열림; 
                    }
                }

                if ( nNearestInterval > nInterval )
                {
                    nNearestDayInterval = nDayInterval;
                    nNearestInterval = nInterval;
                    nNearestTime = i;
                }
            }

            if ( nNearestTime != -1 )
            {
                if ( nNearestDayInterval > 1 ) // 다음 진행이 1일 이상 남은 경우 날짜 표시;
                {
                    std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 9), nNearestDayInterval);
                    SetNextWarTime(strFormat.c_str());
                }
                else
                {
                    std::string strFormat = sc::string::format(ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 8), (nNearestInterval / 60), (nNearestInterval % 60));
                    SetNextWarTime(strFormat.c_str());
                }
            }
        }
        break;
    case MatchingSystem::EMMATCHING_TIME_ALWAYS:
        break;
    case MatchingSystem::EMMATCHING_TIME_EVENTUAL:
        break;
    }

    bool isCompetitionJoined = false;
    MatchingSystem::GLTouranmentClient* m_pLogic = m_pGaeaClient->GetTournamentClient();
    BOOST_FOREACH(MatchingSystem::MatchingClientInfo* m_ClientMatchingStatus,m_pLogic->m_ClientMatchingStatus  )
    {
        if( m_ClientMatchingStatus->ClientStatus == MatchingSystem::EMSTATUS_GROUP_IN)
            isCompetitionJoined = true;

        if(m_ClientMatchingStatus->ScriptID == m_dwSelectID)
        {
            pTournament->m_ClientMatchingStatus_Select.ClientStatus = m_ClientMatchingStatus->ClientStatus;
            pTournament->m_ClientMatchingStatus_Select.ScriptID = m_ClientMatchingStatus->ScriptID;
            break;
        }
    }

    if(m_pLogic->m_ClientMatchingStatus.size() == 0)
    {
        pTournament->m_ClientMatchingStatus_Select.ClientStatus = MatchingSystem::EMSTATUS_GROUP_OUT;
        pTournament->m_ClientMatchingStatus_Select.ScriptID = m_dwSelectID;
    }

    // 현재 참여중이거나 입장대기 중인 인던이 있는가?
    if ( pTournament->m_ClientMatchingStatus_Select.ClientStatus != MatchingSystem::EMSTATUS_GROUP_OUT)
    {
        // 현재 선택된 전장에 참여나 대기중인가?
        if ( pTournament->m_ClientMatchingStatus_Select.ScriptID == _Info->m_ScriptID )
        {
            switch ( pTournament->m_ClientMatchingStatus_Select.ClientStatus )
            {
            case MatchingSystem::EMSTATUS_GROUP_IN:     // 입장 대기 중;
                {
                    m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 5), NS_UITEXTCOLOR::WHITE );
                    m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 1) ); // 참여 취소하기;
                    _SetEnableButton1(true);
                }
                break;

            case MatchingSystem::EMSTATUS_GROUP_PLAYING:    // 참여 중;
                {
                    if ( _Info->m_ContentsType == MatchingSystem::EMMATCHING_CONTENTS_NOMAL_REJOINALBE )
                    {
                        m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
                        m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 11) );
                        _SetEnableButton1(true);
                    }
                    else
                    {
                        m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
                        m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
                        _SetEnableButton1(false);
                    }
                }
                break;
            }
        }
        else
        {
            switch( _Info->m_ContentsStatus )
            {
            case MatchingSystem::EMMATCHING_CONTENTS_OPENED:    // 신청 가능이어도 신청 불가;
            case MatchingSystem::EMMATCHING_CONTENTS_CLOSED:    // 신청 불가;
                m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
                m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
                _SetEnableButton1(false);
                break;

            case MatchingSystem::EMMATCHING_CONTENTS_STANDBY_JOIN:  // 입장 대기;
                m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 5), NS_UITEXTCOLOR::WHITE );
                m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
                _SetEnableButton1(false);
                break;

            case MatchingSystem::EMMATCHING_CONTENTS_IN_PROGRESS:   // 진행 중;
                m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
                m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
                _SetEnableButton1(false);
                break;
            }
        }
    }
    else
    {
		// Join Count 체크;
		// Max Join Count 가 0 일 경우 체크하지 않는다;
		const DWORD dwJoinCount = MatchingSystem::CMatchingJoinManagerClient::Instance()->GetJoinCount( _Info->m_ScriptID );
		DWORD dwCurJoinCount = _Info->m_dwMaxJoinCount - dwJoinCount;
		if ( dwCurJoinCount < 0 )
			dwCurJoinCount = 0;

		if ( _Info->m_dwMaxJoinCount > 0 && 0 >= dwCurJoinCount )
		{
			m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
			m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
			_SetEnableButton1(false);

			return;
		}

		/*std::string strText = ID2GAMEINTEXT( _Info->m_strContentsName.c_str() );
		if ( _Info->m_dwMaxJoinCount > 0 )
		{
			strText += sc::string::format( " ( %1% )", dwCurJoinCount );
		}

		SetInstanceList( _Info->m_ScriptID, strText.c_str() );*/

        switch( _Info->m_ContentsStatus )
        {
        case MatchingSystem::EMMATCHING_CONTENTS_OPENED:    // 신청 가능;
            m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 6), NS_UITEXTCOLOR::GREENYELLOW );
            m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) ); // 참여하기;
            _SetEnableButton1(true);
            break;

        case MatchingSystem::EMMATCHING_CONTENTS_CLOSED:    // 신청 불가;
            m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 3), NS_UITEXTCOLOR::RED );
            m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
            _SetEnableButton1(false);
            break;

        case MatchingSystem::EMMATCHING_CONTENTS_STANDBY_JOIN:  // 입장 대기;
            m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 5), NS_UITEXTCOLOR::WHITE );
            m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
            _SetEnableButton1(false);
            break;

        case MatchingSystem::EMMATCHING_CONTENTS_IN_PROGRESS:   // 진행 중;
            m_pStatus->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 7), NS_UITEXTCOLOR::WHITE );
            m_pButton1->SetOneLineText( ID2GAMEWORD("COMPETITION_CTF_BATTLE_TEXT", 0) );
            _SetEnableButton1(false);
            break;
        }
    }
}


void RnCompetitionObject::_SetEnableInfoButton(const bool _bEnable)
{
    m_bInfoButton_Enable =  _bEnable;
    if ( _bEnable )
    {
        if ( m_fInfoButton_Delay <= 0.0f )
        {
            m_pInfoButton->SetDiffuse( NS_UITEXTCOLOR::WHITE );
            m_pInfoButton->SetDiffuseAlpha( 0xFF );
            m_pInfoButton->SetNoMessage( FALSE );
        }
    }
    else
    {
        m_pInfoButton->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
        m_pInfoButton->SetDiffuseAlpha( 0xDD );
        m_pInfoButton->SetNoMessage( TRUE );
    }
}

void RnCompetitionObject::_SetEnableButton1(const bool _bEnable)
{
    m_bButton1_Enable =  _bEnable;
    if ( _bEnable )
    {
        if ( m_fButton1_Delay <= 0.0f )
        {
            m_pButton1->SetDiffuse( NS_UITEXTCOLOR::WHITE );
            m_pButton1->SetDiffuseAlpha( 0xFF );
            m_pButton1->SetNoMessage( FALSE );
        }
    }
    else
    {
        m_pButton1->SetDiffuse( NS_UITEXTCOLOR::DIMGREY );
        m_pButton1->SetDiffuseAlpha( 0xDD );
        m_pButton1->SetNoMessage( TRUE );
    }
}
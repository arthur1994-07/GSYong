
#include "stdafx.h"

#include "RnOldLargeMapWindow.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogic/Crow/GLMobScheduleMan.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../../RanLogic/Quest/GLQuest.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"


#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/Land/GLLandMark.h"

#include "../../InnerInterface.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../enginelib/DxTools/DxFontMan.h"

#include "RnMapHelper.h"


namespace
{
	const char* ID2GAMEWORD_TRADE_ON = "거래 가능";
	const char* ID2GAMEWORD_TRADE_OFF = "거래 불가능";

	const char* ID2GAMEWORD_QUEST = "퀘스트";

	const char* ID2GAMEWORD_CAN_FINISH = "완료 가능";
	const char* ID2GAMEWORD_CAN_STEP = "수행 중";
	const char* ID2GAMEWORD_CAN_START = "수행 가능";

	const char* ID2GAMEWORD_AUTHENTICATOR_TEXT = "공격하여 점령 할 수 있습니다.";
	const char* ID2GAMEWORD_POSTBOX_TEXT = "우편을 보내거나, 자신에게 온 우편을 확인할 수 있습니다.";
	const char* ID2GAMEWORD_BUS_STATION_TEXT = "버스를 이용하여 먼 곳을 한 번에 이동할 수 있습니다.";
	const char* ID2GAMEWORD_LOCKER_TEXT = "아이템 창고를 이용할 수 있습니다.";
	const char* ID2GAMEWORD_START_POS_TEXT = "시작 위치를 이곳으로 변경할 수 있습니다.";
	const char* ID2GAMEWORD_CLUB_MANAGER_TEXT = "클럽을 창설할 수 있습니다.";
	const char* ID2GAMEWORD_ITEM_SEARCH_TEXT = "해당 지역에 개설돼 있는 모든 개인 상점에서 판매 중인 아이템을 검색 할 수 있습니다.";
	const char* ID2GAMEWORD_ITEM_MIX_TEXT = "몇 가지 재료 아이템을 조합하여 다양한 아이템을 만들 수 있습니다.";

	const char* ID2GAMEWORD_GATE_TO_MOVE = "지역으로 이동";
	const char* ID2GAMEWORD_GATE_TO_MOVE_TEXT = "마우스 우 클릭하여 이곳을 통해 도착하는 지역을 표시합니다.";



	const char* ID2GAMEWORD_RELATIVE_QUEST = "관련 퀘스트";
	const char* ID2GAMEEXTEXT_MONSTER_EXPLAIN = "우측의 돋보기 버튼을 클릭하시면 다음 레벨에 도달하기까지 남은 몬스터 수를 화면에 표시할 수 있습니다.";

	const char* ID2GAMEEXTEXT_MONSTER_WEAK = "약합니다 동시에 8명 이상과 상대할 수 있습니다.";
	const char* ID2GAMEEXTEXT_MONSTER_NORMAL = "적당히 강합니다. 동시에 8명 이상과 상대하기엔 위험합니다.";
	const char* ID2GAMEEXTEXT_MONSTER_STRONG = "매우 강합니다. 동시에 2명 이상과 상대하기에는 위험합니다";
	const char* ID2GAMEEXTEXT_MONSTER_VERY_STRONG = "당신을 한 방에 쓰러뜨릴 수 있습니다.";

	//////////////////////////////////////////////////////////////////////////
	inline std::string MAKE_TEXT(const char* text) { return std::string() + "\"" + text + "\""; }
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	bool FindMarkDeleteFromMarkVec(CUIGroup* This, RnOldLargeMapWindow::MarkVector& markVec,const std::string& Name)
	{
		for (size_t i = 0; i < markVec.size(); ++i)
		{
			if( markVec[i]->m_Name == Name )
			{
				This->DeleteControl(markVec[i]->GetWndID(),true);
				markVec.erase(markVec.begin()+i);
				return true;
			}
		}

		return false;
	}


	void GetCrowDataFromNpcType( const GLLevelFileClient* pLevelFile,int NpcType,std::vector<GLMobSchedule*>* pCrowVec )
	{
		const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
		MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

		while(pCur)
		{
			const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
			if( !pCrowData )
			{
				pCur = pCur->pNext;
				continue;
			}

			if( pCrowData->m_emNPCType == NpcType )
			{
				pCrowVec->push_back(pCur->Data);
			}

			pCur = pCur->pNext;
		}
	}

	enum QUEST_TYPE { QUEST_START,QUEST_STEP,QUEST_FINISH };
	void GetCrowDataFromQuest( const GLLevelFileClient* pLevelFile,GLGaeaClient* pGaeaClient,QUEST_TYPE questType,std::vector<GLMobSchedule*>* pCrowVec,std::vector< std::vector<std::string> >* pQuestStringsVec)
	{
		const DWORD nPartyMember = pGaeaClient->GetMyPartyClient()->GetNMember();

		const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
		MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

		GLCharacter* pChar = pGaeaClient->GetCharacter();
		CTime cCurSvrTime = pGaeaClient->GetCurrentTime();

		while(pCur)
		{
			const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
			if( !pCrowData )
			{
				pCur = pCur->pNext;
				continue;
			}

			CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk( pCrowData->GetTalkFile() );
			if( !pTalkMap )
			{
				pCur = pCur->pNext;
				continue;
			}

			std::vector<std::string> questTitleVec;

			VEC_DWORD vecTalk;
			BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
			{
				std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
				if( !spTalk )
					continue;


				bool isOK = false;
				switch(questType)
				{
				case QUEST_START:
					{
						if( spTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
						{
							if( spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
							{
								isOK = true;

								for (int i = 0; i < MAX_QUEST_START; ++i)
								{
									if( spTalk->m_dwQuestStartID[i] != INT_MAX)
									{
										GLQUEST* pQuest = GLQuestMan::GetInstance().Find(spTalk->m_dwQuestStartID[i]);
										if( pQuest )
										{
											if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
											{
												CString strKey;
												strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
												// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
												questTitleVec.push_back( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
											}
											else
											{
												questTitleVec.push_back( pQuest->GetTITLE() );
											}
										}
									}
								}

							}
						}
						break;
					}
				case QUEST_STEP:
					{
						if( spTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP )
						{
							if(spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
							{
								isOK = true;
						
								GLQUEST* pQuest = GLQuestMan::GetInstance().Find(spTalk->m_dwACTION_PARAM1);
								if( pQuest )
								{
									if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
									{
										CString strKey;
										strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
										// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
										questTitleVec.push_back( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
									}
									else
									{
										questTitleVec.push_back( pQuest->GetTITLE() );
									}
								}
							}
						}
						break;
					}
				case QUEST_FINISH:
					{
						if( spTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP )
						{
							GLQUESTPROG* pQuestProg = pChar->m_cQuestPlay.FindProc(spTalk->m_dwACTION_PARAM1);
							if( pQuestProg )
							{

								if(pQuestProg->IsLastStep() && spTalk->DoTEST( pChar, nPartyMember, cCurSvrTime.GetHour() ) )
								{
									isOK = true;

									GLQUEST* pQuest = GLQuestMan::GetInstance().Find(spTalk->m_dwACTION_PARAM1);
									if( pQuest )
									{
										if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
										{
											CString strKey;
											strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
											// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
											questTitleVec.push_back( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
										}
										else
										{
											questTitleVec.push_back( pQuest->GetTITLE() );
										}
									}							
								}
							}
						}
						break;
					}
				}
				
				if( isOK )
				{
					// 똑같은건 두번 담지 않게 한다 , 이렇게 한 이유는  퀘스트 목록때문에 break할수 없다
					if( pCrowVec->end() == std::find( pCrowVec->begin(),pCrowVec->end(), pCur->Data) )
						pCrowVec->push_back(pCur->Data);
				}
			}

			if( !questTitleVec.empty() )
				pQuestStringsVec->push_back( questTitleVec );

			pCur = pCur->pNext;
		}

		GASSERT( pCrowVec->size() == pQuestStringsVec->size() ); 
	}

	bool haveInvenItem(const SCROWDATA* pCrowData)
	{
		for(int i=0; i < SCROWACTION::SALENUM; ++i)
		{
			if( !pCrowData->m_sSaleItems[i].IsEmpty() )
				return true;
		}

		return false;
	}

	void GetCrowDataFromNpcInven(const GLLevelFileClient* pLevelFile,std::vector<GLMobSchedule*>* pCrowVec)
	{
		const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
		MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

		while(pCur)
		{
			const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
			if( !pCrowData )
			{
				pCur = pCur->pNext;
				continue;
			}

			if( haveInvenItem(pCrowData) )
			{
				pCrowVec->push_back(pCur->Data);
			}

			pCur = pCur->pNext;
		}
	}

	void GetCrowDataFromNpcTalk(const GLLevelFileClient* pLevelFile,GLGaeaClient* pGaeaClient,int m_talkType,std::vector<GLMobSchedule*>* pCrowVec)
	{
		const MOBSCHEDULELIST* pMobScheduleList = pLevelFile->GetMobSchMan()->GetMobSchList();
		MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

		GLCharacter* pChar = pGaeaClient->GetCharacter();

		while(pCur)
		{
			const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
			if( !pCrowData )
			{
				pCur = pCur->pNext;
				continue;
			}

			CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk( pCrowData->GetTalkFile() );
			if( !pTalkMap )
			{
				pCur = pCur->pNext;
				continue;
			}
			
			VEC_DWORD vecTalk;
			BOOST_FOREACH( CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap )
			{
				std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
				if( !spTalk )
					continue;

				if( spTalk->m_nACTION != SNpcTalk::EM_BASIC )
					break;

				if( spTalk->m_dwACTION_NO == m_talkType )
				{
					pCrowVec->push_back(pCur->Data);
					break;
				}
			}


			pCur = pCur->pNext;
		}
	}

	

	//////////////////////////////////////////////////////////////////////////
	void SetQuestCommentHelper(RnOldLargeMapWindow::MarkVector& questMarkVec,std::vector< std::vector<std::string> >& QuestStringsVec,
		const std::string& canString,DWORD color )
	{
		for (size_t markIdx = 0; markIdx < questMarkVec.size(); ++markIdx)
		{
			GASSERT( questMarkVec[markIdx] );
			if( markIdx < QuestStringsVec.size() )
			{
				const std::vector<std::string>& QuestTitleVec = QuestStringsVec[markIdx];
				for (size_t QIdx = 0; QIdx < QuestTitleVec.size(); ++QIdx)
				{
					questMarkVec[markIdx]->m_CommentVec.push_back(
						RnOldLargeMapWindow::Mark::Comment( std::string() + ID2GAMEWORD_QUEST + " : " + QuestTitleVec[QIdx] + "("+canString+")"  , color ) );
				}
			}
			else
			{
				questMarkVec[markIdx]->m_CommentVec.push_back(
					RnOldLargeMapWindow::Mark::Comment( "there is a quest problem, send this message to us!", 0xFFFF0000 ) );
			} //if..else
		}

	}

	//////////////////////////////////////////////////////////////////////////

	void MakeToolTip(RnOldLargeMapWindow::Mark* pMark,CInnerInterface* pInterface)
	{
		DWORD dwMsg = pMark->GetMessageEx();

		if( pMark->m_Name.empty() )
			return;

		bool bIsBegin = pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
        if ( !bIsBegin )
            return;

		pInterface->ADD_COMMON_LINEINFO_MULTI(pMark->m_Name.c_str(),D3DCOLOR_XRGB(255,255,255));

		DWORD color;
		if( pMark->m_canTrade != RnOldLargeMapWindow::Mark::NO_VISIBLE_TRADE )
		{
			color = pMark->m_canTrade == RnOldLargeMapWindow::Mark::CAN_TRADE ? D3DCOLOR_XRGB(255,255,0) : D3DCOLOR_XRGB(125,125,125);
			const char* pTradeText = pMark->m_canTrade == RnOldLargeMapWindow::Mark::CAN_TRADE ? ID2GAMEWORD_TRADE_ON : ID2GAMEWORD_TRADE_OFF;
			pInterface->ADD_COMMON_LINEINFO_MULTI(pTradeText,color);
		}

		for (size_t i = 0; i < pMark->m_CommentVec.size(); ++i)
		{
			color = D3DCOLOR_XRGB(255,255,255);
			std::string& comment = pMark->m_CommentVec[i].text;
			DWORD color = pMark->m_CommentVec[i].color;
			pInterface->ADD_COMMON_LINEINFO_MULTI(comment.c_str(),color);
		}

		pInterface->END_COMMON_LINEINFO_MULTI();
	}


}

namespace RnMapPrivate
{
	class RegionName : public CUIGroup
	{
    public:
		RegionName(EngineDeviceMan* pEngineDevice) 
            : CUIGroup(pEngineDevice) 
            , m_pTextMark( NULL )
            , m_pTextBG( NULL )
		{}    

        void CreateSubControl( const char* szControlKeyword, const char* szBGControlKeyword = NULL )
        {
            CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

            if( szBGControlKeyword )
            {
                CUIControl* pMark = new CUIControl(m_pEngineDevice);
                pMark->CreateSub( this, szBGControlKeyword );
                pMark->InitDeviceObjects( m_pEngineDevice->GetDevice() );
                pMark->RestoreDeviceObjects( m_pEngineDevice->GetDevice() );
                pMark->SetDiffuseAlpha( 192 );
                RegisterControl(pMark);

                m_pTextBG = pMark;
            }

            CBasicTextBox* pTextMark = new CBasicTextBox( m_pEngineDevice );
            pTextMark->CreateSub ( this, szControlKeyword );
            pTextMark->SetFont ( pFont );
            pTextMark->SetTextAlign ( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            RegisterControl ( pTextMark );
            m_pTextMark = pTextMark;

        }
        void SetData( const GLMobSchedule* pMob, const D3DCOLOR& TextColor )
        {
            if( pMob )
            {
                SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pMob->m_CrowID);
                if( pCrowData == NULL )
                    return;

                m_Position = pMob->m_pAffineParts->vTrans;

                if( m_pTextMark )
                {
                    std::string strKey = "  ";

                    std::string strZoneName = pCrowData->GetName();
                    bool bLoof = true;
                    std::basic_string<char>::size_type start = 0;
                    std::basic_string<char>::size_type npos = -1;

                    //		int nLine = -1;
                    long lTBSizeX = 0, lTBSizeY = 0;
                    UIRECT rcTBSize;

                    std::vector<std::string> vecStrSplit;
                    UI::StringSplit( strZoneName, strKey, vecStrSplit );

                    SIZE sizeStr; sizeStr.cx=0;sizeStr.cy=0;
                    size_t i = 0;
                    for( ; i < vecStrSplit.size(); ++i )
                    {
                        m_pTextMark->AddTextNoSplit( vecStrSplit[i].c_str(), TextColor );
                        m_pTextMark->GetPartInfo( i, rcTBSize );
                        lTBSizeY += (long)rcTBSize.sizeY;
                        if(rcTBSize.sizeX > lTBSizeX)
                            lTBSizeX = (long)rcTBSize.sizeX;

                        // 사이즈 조절      
                        SIZE sizeStrTemp;
                        m_pTextMark->GetFont()->GetTextExtent ( vecStrSplit[i].c_str(), sizeStrTemp );

                        sizeStr.cx = max( sizeStrTemp.cx, sizeStr.cx );
                        sizeStr.cy = max( sizeStrTemp.cy, sizeStr.cy );
                    }
                    lTBSizeY += (long)(m_pTextMark->fDEFAULT_LINE_INTERVAL * (i-1) );
                    m_pTextMark->SetControlSize( lTBSizeX, lTBSizeY );

                    UIRECT rcGroup = UIRECT( GetGlobalPos().left, GetGlobalPos().top, (float)sizeStr.cx + 10, (float)vecStrSplit.size() * (sizeStr.cy + 6) );
                    SetGlobalPos( rcGroup );
                    m_pTextMark->SetGlobalPos( rcGroup );

                    if( m_pTextBG )
                        m_pTextBG->SetGlobalPos( rcGroup );
                }
            }
        }
        CBasicTextBox*      m_pTextMark;
        CUIControl*         m_pTextBG;
        D3DXVECTOR3	        m_Position;
	};
}


void RnOldLargeMapWindow::Initialize()
{
	m_fPercentH_BACK = 2.0f;
	m_fPercentV_BACK = 2.0f;
	m_nWORLD_START_X = 1;
	m_nWORLD_START_Y = 1;
	m_nWORLD_SIZE_X = 1;
	m_nWORLD_SIZE_Y = 1;
	m_nMAP_TEXSIZE_X = 1;
	m_nMAP_TEXSIZE_Y = 1;
	m_nVIEW_START_X = 1;
	m_nVIEW_START_Y = 1;
	m_nVIEW_SIZE_X = 1;
	m_nVIEW_SIZE_Y = 1;
	m_bFirstGap = FALSE;
	//m_CONTROL_NEWID = BASE_CONTROLID;
	m_bAUTO_MOVE = true;

	ID2GAMEWORD_TRADE_ON = ID2GAMEINTEXT("RN_MAP_WINDOW_TRADE_ON");
	ID2GAMEWORD_TRADE_OFF = ID2GAMEINTEXT("RN_MAP_WINDOW_TRADE_OFF");

	ID2GAMEWORD_QUEST = ID2GAMEINTEXT("RN_MAP_WINDOW_QUEST");

	ID2GAMEWORD_CAN_FINISH = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_FINISH");
	ID2GAMEWORD_CAN_STEP = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_STEP");
	ID2GAMEWORD_CAN_START = ID2GAMEINTEXT("RN_MAP_WINDOW_CAN_START");

	ID2GAMEWORD_AUTHENTICATOR_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_AUTHENTICATOR_TEXT");
	ID2GAMEWORD_POSTBOX_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_POSTBOX_TEXT");
	ID2GAMEWORD_BUS_STATION_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_BUS_STATION_TEXT");
	ID2GAMEWORD_LOCKER_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_LOCKER_TEXT");
	ID2GAMEWORD_START_POS_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_START_POS_TEXT");
	ID2GAMEWORD_CLUB_MANAGER_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_CLUB_MANAGER_TEXT");
	ID2GAMEWORD_ITEM_SEARCH_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_ITEM_SEARCH_TEXT");
	ID2GAMEWORD_ITEM_MIX_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_ITEM_MIX_TEXT");

	ID2GAMEWORD_GATE_TO_MOVE = ID2GAMEINTEXT("RN_MAP_WINDOW_GATE_TO_MOVE");
	ID2GAMEWORD_GATE_TO_MOVE_TEXT = ID2GAMEINTEXT("RN_MAP_WINDOW_GATE_TO_MOVE_TEXT");

	ID2GAMEWORD_RELATIVE_QUEST = ID2GAMEINTEXT("RN_MAP_WINDOW_RELATIVE_QUEST");
	ID2GAMEEXTEXT_MONSTER_EXPLAIN = ID2GAMEEXTEXT("RN_MAP_WINDOW_MONSTER_EXPLAIN");

	ID2GAMEEXTEXT_MONSTER_WEAK = ID2GAMEEXTEXT("RN_MAP_WINDOW_MONSTER_WEAK");
	ID2GAMEEXTEXT_MONSTER_NORMAL = ID2GAMEEXTEXT("RN_MAP_WINDOW_MONSTER_NORMAL");
	ID2GAMEEXTEXT_MONSTER_STRONG = ID2GAMEEXTEXT("RN_MAP_WINDOW_MONSTER_STRONG");
	ID2GAMEEXTEXT_MONSTER_VERY_STRONG = ID2GAMEEXTEXT("RN_MAP_WINDOW_MONSTER_VERY_STRONG");

	ConvIntDataFloatData();
}

void RnOldLargeMapWindow::CreateZoneName( const std::vector<GLMobSchedule*>& crowVec, std::vector<RnMapPrivate::RegionName*>& MarkVec )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
    
	std::string strKey = "  ";

    BOOST_FOREACH( const GLMobSchedule* pMob, crowVec )
    {
        SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pMob->m_CrowID);
        if( pCrowData == NULL )
            return;

        RnMapPrivate::RegionName* pTextMark = new RnMapPrivate::RegionName(m_pEngineDevice);
        pTextMark->CreateSub( this, "RNMAP_GETE_NAME_SIZE", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
        pTextMark->CreateSubControl( "RNMAP_GETE_NAME_SIZE" );
        RegisterControl(pTextMark);

        pTextMark->SetData( pMob, D3DCOLOR_ARGB(0xFF,0xF3,0x9F,0x06) );
        MarkVec.push_back(pTextMark);
    }
}

void RnOldLargeMapWindow::CreateGateName( const std::vector<GLMobSchedule*>& crowVec, std::vector<RnMapPrivate::RegionName*>& MarkVec )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
    std::string strKey = "  ";

        BOOST_FOREACH( const GLMobSchedule* pMob, crowVec )
    {
        SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pMob->m_CrowID);
        if( pCrowData == NULL )
            return;

        RnMapPrivate::RegionName* pTextMark = new RnMapPrivate::RegionName(m_pEngineDevice);
        pTextMark->CreateSub( this, "RNMAP_GETE_NAME_SIZE", UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
        pTextMark->CreateSubControl( "RNMAP_GETE_NAME_SIZE","RNMAP_GETE_NAME" );
        RegisterControl(pTextMark);

        pTextMark->SetData( pMob, D3DCOLOR_ARGB( 255, 162, 239, 45 ) );
        MarkVec.push_back(pTextMark);
    }
}




void RnOldLargeMapWindow::CreateMarkHelper(const std::string& ImageKey,std::vector<GLMobSchedule*>& crowVec,
										   MarkVector& MarkVec)
{
	for (size_t i = 0; i < crowVec.size(); ++i )
	{
		Mark* pMark = new Mark(m_pEngineDevice);
		pMark->CreateSub(this,ImageKey.c_str());
		pMark->InitDeviceObjects( m_pd3dDevice );
		//  [12/4/2012 gbgim]
		pMark->RestoreDeviceObjects( m_pd3dDevice );
		//pMark->ReCalcPos();
		RegisterControl(pMark);
		pMark->m_Position = crowVec[i]->m_pAffineParts->vTrans;
		
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(crowVec[i]->m_CrowID);
		pMark->m_Name = pCrowData->GetName();
		MarkVec.push_back(pMark);
	}
}


template<class T>
void ClearMark(CUIGroup* pThis,std::vector<T>* pVec )
{
	for(size_t i=0; i < pVec->size(); ++i)
		pThis->DeleteControl( (*pVec)[i]->GetWndID(),true);

	pVec->clear();
}


void RnOldLargeMapWindow::MakeGate()
{
	ClearMark(this, &m_GateMarkVec);

	DxLandGateMan* pGateMan = m_pLevelFile->GetLandGateMan();
	for (PDXLANDGATE pGate = pGateMan->GetListLandGate(); pGate != NULL ; pGate = pGate->m_pNext)
	{
		// IN OUT 두개 만든다 아이콘이 없어서 일단 하나만
		if( pGate->GetFlags() & DxLandGate::GATE_OUT )
		{
			Mark* pGateMark = new Mark(m_pEngineDevice);
			pGateMark->CreateSub(this,"MAP_WINDOW_SETTING_GATE_MARK");
			pGateMark->InitDeviceObjects ( m_pd3dDevice );
			pGateMark->RestoreDeviceObjects ( m_pd3dDevice );
			RegisterControl(pGateMark);
			pGateMark->m_Position = *pGate->GetStartPos();
			pGateMark->m_canTrade = Mark::NO_VISIBLE_TRADE;
			pGateMark->m_Name = std::string() + "[" + m_pChangeMap->GetMapName(pGate->GetToMapID().Id()) + "]" + " " + ID2GAMEWORD_GATE_TO_MOVE;
			pGateMark->m_CommentVec.push_back( Mark::Comment() );
			pGateMark->m_CommentVec.push_back( Mark::Comment(ID2GAMEWORD_GATE_TO_MOVE_TEXT) );
			pGateMark->m_iAddData1 = (int)pGate->GetToMapID().Id();
			
			m_GateMarkVec.push_back(pGateMark);
		}
	}
}

namespace
{
	void DeleteFromName(const std::string& name,std::vector<GLMobSchedule*>* crowVec)	
	{
		for (size_t i=0; i < crowVec->size(); ++i)
		{
			SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( (*crowVec)[i]->m_CrowID);
			if( pCrowData->GetName() == name )
			{
				crowVec->erase( crowVec->begin() + i );
			}
		}
	}
}

void RnOldLargeMapWindow::MakeNormalNpc()
{
	ClearMark(this,&m_NormalNpcMarkVec);

	std::vector<GLMobSchedule*> crowVec;

//	MarkVector AuthenticatorMarkVec;
	// 인증기는 필요 없다
// 	GetCrowDataFromNpcType(m_LevelFile,NPC_TYPE_AUTHENTICATOR,&crowVec);
// 	CreateMarkHelper("MAP_WINDOW_SETTING_AUTHENTICATOR_MARK",crowVec,AuthenticatorMarkVec);
// 
// 	for (size_t i = 0; i < AuthenticatorMarkVec.size(); ++i)
// 		AuthenticatorMarkVec[i]->m_CommentVec.push_back( MAKE_TEXT(ID2GAMEWORD_AUTHENTICATOR_TEXT) );
	


	//////////////////////////////////////////////////////////////////////////
	crowVec.clear();

	std::vector< std::vector<std::string> > QuestStringsVec;
	//////////////////////////////////////////////////////////////////////////
	QuestStringsVec.clear();
	MarkVector questStartMarkVec;
	GetCrowDataFromQuest(m_pLevelFile,m_pGaeaClient,QUEST_START,&crowVec,&QuestStringsVec);
	CreateMarkHelper("MAP_WINDOW_SETTING_QUEST_START_MARK",crowVec,questStartMarkVec);

	SetQuestCommentHelper(questStartMarkVec,QuestStringsVec,ID2GAMEWORD_CAN_START,D3DCOLOR_XRGB(255,255,255));
	
	
	//////////////////////////////////////////////////////////////////////////
	crowVec.clear();
	QuestStringsVec.clear();

	MarkVector questStepMarkVec;
	GetCrowDataFromQuest(m_pLevelFile,m_pGaeaClient,QUEST_STEP,&crowVec,&QuestStringsVec);
	CreateMarkHelper("MAP_WINDOW_SETTING_QUEST_STEP_MARK",crowVec,questStepMarkVec);
	SetQuestCommentHelper(questStepMarkVec,QuestStringsVec,ID2GAMEWORD_CAN_STEP,D3DCOLOR_XRGB(125,125,125));

	//////////////////////////////////////////////////////////////////////////
	crowVec.clear();
	QuestStringsVec.clear();

	MarkVector questFinishMarkVec;
	GetCrowDataFromQuest(m_pLevelFile,m_pGaeaClient,QUEST_FINISH,&crowVec,&QuestStringsVec);
	CreateMarkHelper("MAP_WINDOW_SETTING_QUEST_FINISH_MARK",crowVec,questFinishMarkVec);
	SetQuestCommentHelper(questFinishMarkVec,QuestStringsVec,ID2GAMEWORD_CAN_FINISH,D3DCOLOR_XRGB(0,255,0));

	//////////////////////////////////////////////////////////////////////////
	

	m_NormalNpcMarkVec.insert(m_NormalNpcMarkVec.end(),questStartMarkVec.begin(),questStartMarkVec.end());
	m_NormalNpcMarkVec.insert(m_NormalNpcMarkVec.end(),questStepMarkVec.begin(),questStepMarkVec.end());
	m_NormalNpcMarkVec.insert(m_NormalNpcMarkVec.end(),questFinishMarkVec.begin(),questFinishMarkVec.end());


	//////////////////////////////////////////////////////////////////////////

	std::vector<GLMobSchedule*> marketCrowVec;
	MarkVector marketMarkVec;
	// 대체 , 상점 거래가능 여부
	//	GetCrowDataFromNpcTalk(m_pLevelFile,m_pGaeaClient,SNpcTalk::EM_MARKET,&crowVec);
	GetCrowDataFromNpcInven(m_pLevelFile,&marketCrowVec);
	CreateMarkHelper("MAP_WINDOW_SETTING_MARKET_MARK",marketCrowVec,marketMarkVec);

	for (size_t normalIndex=0; normalIndex < m_NormalNpcMarkVec.size(); ++normalIndex)
	{
		bool ret = FindMarkDeleteFromMarkVec(this,marketMarkVec,m_NormalNpcMarkVec[normalIndex]->m_Name);
		if( ret )
		{
			m_NormalNpcMarkVec[normalIndex]->m_canTrade = Mark::CAN_TRADE;
		}		
	}

	for (size_t i=0; i < marketMarkVec.size(); ++i)
	{
		marketMarkVec[i]->m_canTrade = Mark::CAN_TRADE;
	}
	
	m_NormalNpcMarkVec.insert(m_NormalNpcMarkVec.end(),marketMarkVec.begin(),marketMarkVec.end());

	//////////////////////////////////////////////////////////////////////////

	std::vector<GLMobSchedule*> AndSoOnCrowVec;
	GetCrowDataFromNpcType(m_pLevelFile,NPC_TYPE_NORMAL,&AndSoOnCrowVec);
	
	for (size_t i=0; i < m_NormalNpcMarkVec.size(); ++i)
	{
		DeleteFromName(m_NormalNpcMarkVec[i]->m_Name,&AndSoOnCrowVec);
	}


	std::vector<GLMobSchedule*> NpcCrowVec;
	for(size_t i = 0; i < AndSoOnCrowVec.size(); ++i )
	{
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( AndSoOnCrowVec[i]->m_CrowID );
// 		if( pCrowData->m_emCrow == CROW_NPC )
// 			NpcCrowVec.push_back(AndSoOnCrowVec[i]);
    
        if( 0 < strlen( pCrowData->GetTalkFile() ) )
            NpcCrowVec.push_back(AndSoOnCrowVec[i]);
	}
	
	
	MarkVector NormalNpcMark;
	CreateMarkHelper("MAP_WINDOW_SETTING_NORMAL_MARK",NpcCrowVec,NormalNpcMark);
	m_NormalNpcMarkVec.insert(m_NormalNpcMarkVec.end(),NormalNpcMark.begin(),NormalNpcMark.end() );

}

void RnOldLargeMapWindow::MakeZoneNameNpc()
{
    BOOST_FOREACH( RnMapPrivate::RegionName* pTextMark, m_ZoneNameVec )
        this->DeleteControl( pTextMark->GetWndID(), true );

    m_ZoneNameVec.clear();

    std::vector<GLMobSchedule*> AndSoOnCrowVec;
    GetCrowDataFromNpcType(m_pLevelFile,NPC_TYPE_NORMAL,&AndSoOnCrowVec);

    std::vector<GLMobSchedule*> NpcZoneName;
    for(size_t i = 0; i < AndSoOnCrowVec.size(); ++i )
    {
        SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( AndSoOnCrowVec[i]->m_CrowID );
        //if( pCrowData->m_emCrow == CROW_ZONENAME )
        if( pCrowData->m_emCrow == CROW_ZONENAME )
            NpcZoneName.push_back(AndSoOnCrowVec[i]);
    }

    CreateZoneName( NpcZoneName, m_ZoneNameVec );
}

void RnOldLargeMapWindow::MakeGateNameNpc()
{
    BOOST_FOREACH( RnMapPrivate::RegionName* pTextMark, m_GateNameVec )
        this->DeleteControl( pTextMark->GetWndID(), true );

    std::vector<GLMobSchedule*> AndSoOnCrowVec;
    GetCrowDataFromNpcType(m_pLevelFile,NPC_TYPE_NORMAL,&AndSoOnCrowVec);

    std::vector<GLMobSchedule*> NpcGateName;
    for(size_t i = 0; i < AndSoOnCrowVec.size(); ++i )
    {
        SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData( AndSoOnCrowVec[i]->m_CrowID );
        if( pCrowData->m_emCrow == CROW_GATENAME )
            NpcGateName.push_back(AndSoOnCrowVec[i]);
    }

    CreateGateName( NpcGateName, m_GateNameVec );
}

void RnOldLargeMapWindow::MakeBusStation()
{
	ClearMark(this,&m_BusStationMarkVec);

	std::vector<GLMobSchedule*> crowVec;
	GetCrowDataFromNpcTalk(m_pLevelFile,m_pGaeaClient,SNpcTalk::EM_BUSSTATION,&crowVec);

	CreateMarkHelper("MAP_WINDOW_SETTING_BUS_MARK",crowVec,m_BusStationMarkVec);

	for (size_t i = 0; i < m_BusStationMarkVec.size(); ++i)
		m_BusStationMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_BUS_STATION_TEXT));

}

void RnOldLargeMapWindow::MakeLookerRoom()
{
	ClearMark(this,&m_LockerManagerMarkVec);

	std::vector<GLMobSchedule*> crowVec;
	GetCrowDataFromNpcTalk(m_pLevelFile,m_pGaeaClient,SNpcTalk::EM_STORAGE,&crowVec);

	CreateMarkHelper("MAP_WINDOW_SETTING_LOOKER_MARK",crowVec,m_LockerManagerMarkVec);

	for (size_t i = 0; i < m_LockerManagerMarkVec.size(); ++i)
		m_LockerManagerMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_LOCKER_TEXT));
}

void RnOldLargeMapWindow::MakeStartPosition()
{
	ClearMark(this,&m_StartPositionMarkVec);

	std::vector<GLMobSchedule*> crowVec;
	GetCrowDataFromNpcTalk(m_pLevelFile,m_pGaeaClient,SNpcTalk::EM_STARTPOINT,&crowVec);

	CreateMarkHelper("MAP_WINDOW_SETTING_START_POS_MARK",crowVec,m_StartPositionMarkVec);

	for (size_t i = 0; i < m_StartPositionMarkVec.size(); ++i)
		m_StartPositionMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_START_POS_TEXT));
}

void RnOldLargeMapWindow::MakeClubManager()
{
	ClearMark(this,&m_ClubManagerMarkVec);

	std::vector<GLMobSchedule*> crowVec;
	GetCrowDataFromNpcTalk(m_pLevelFile,m_pGaeaClient,SNpcTalk::EM_CLUB_STORAGE,&crowVec);

	CreateMarkHelper("MAP_WINDOW_SETTING_CLUB_MANAGER_MARK",crowVec,m_ClubManagerMarkVec);

	for (size_t i = 0; i < m_ClubManagerMarkVec.size(); ++i)
		m_ClubManagerMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_CLUB_MANAGER_TEXT));
}
// 
// void RnOldLargeMapWindow::MakeItemSearch()
// {
// 	ClearMark(this,&m_ItemSearchMarkVec);
// 
// 	std::vector<GLMobSchedule*> crowVec;
// 	GetCrowDataFromNpcTalk(m_LevelFile,m_pGaeaClient,SNpcTalk::EM_ITEMSEARCH_PAGE,&crowVec);
// 
// 	CreateMarkHelper("MAP_WINDOW_SETTING_ITEM_SEARCH_MARK",crowVec,m_ItemSearchMarkVec);
// 
// 	for (size_t i = 0; i < m_ItemSearchMarkVec.size(); ++i)
// 		m_ItemSearchMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_ITEM_SEARCH_TEXT));
// }
// 
// void RnOldLargeMapWindow::MakeItemMix()
// {
// 	ClearMark(this,&m_ItemMixMarkVec);
// 
// 	std::vector<GLMobSchedule*> crowVec;
// 	GetCrowDataFromNpcTalk(m_LevelFile,m_pGaeaClient,SNpcTalk::EM_ITEM_MIX,&crowVec);
// 
// 	CreateMarkHelper("MAP_WINDOW_SETTING_ITEM_MIX_MARK",crowVec,m_ItemMixMarkVec);
// 
// 	for (size_t i = 0; i < m_ItemMixMarkVec.size(); ++i)
// 		m_ItemMixMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_ITEM_MIX_TEXT));
// }

void RnOldLargeMapWindow::MakePostBox()
{
	ClearMark(this,&m_PostBoxMarkVec);

	std::vector<GLMobSchedule*> crowVec;
	GetCrowDataFromNpcType(m_pLevelFile,NPC_TYPE_POSTBOX,&crowVec);
	CreateMarkHelper("MAP_WINDOW_SETTING_POST_BOX_MARK",crowVec,m_PostBoxMarkVec);

	for (size_t i = 0; i < m_PostBoxMarkVec.size(); ++i)
		m_PostBoxMarkVec[i]->m_CommentVec.push_back(MAKE_TEXT(ID2GAMEWORD_POSTBOX_TEXT));
}


void RnOldLargeMapWindow::MakePartyMember()
{
	ClearMark(this,&m_PartyMarkVec);

// 	CString strMark[MAXPARTY] =
// 	{
// 		"TEST_MARK0", 
// 		"TEST_MARK1", 
// 		"TEST_MARK2", u
// 		"TEST_MARK3",
// 		"TEST_MARK4", 
// 		"TEST_MARK5", 
// 		"TEST_MARK6", 
// 		"TEST_MARK7"
// 	};

	for ( int i = 0; i < MAXPARTY; ++i )
	{
		Mark* pMark = new Mark(m_pEngineDevice);
//		pMark->CreateSub( this, strMark[i].GetString() );
		pMark->CreateSub(this, "MAP_WINDOW_SETTING_PARTY_MEMBER_MARK");
		pMark->InitDeviceObjects(m_pd3dDevice);
		//pMark->SetUseRender( TRUE );
		pMark->SetVisibleSingle( FALSE );
		RegisterControl( pMark );
		m_PartyMarkVec.push_back(pMark);
	}
}

void RnOldLargeMapWindow::MakeSearchCharacter()
{
   if(m_pSearchCharacter)
      DeleteControl(m_pSearchCharacter->GetWndID(),true);

	m_pSearchCharacter = new Mark(m_pEngineDevice);
	m_pSearchCharacter->CreateSub(this, "MAP_WINDOW_SETTING_SEARCH_CHARACTER_MARK");
	m_pSearchCharacter->InitDeviceObjects(m_pd3dDevice);
	m_pSearchCharacter->SetVisibleSingle( FALSE );
	RegisterControl( m_pSearchCharacter );

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace
{
	template<class T>
	void SetVisibleOff(std::vector<T>* pVec)
	{
		for (size_t i = 0; i < pVec->size(); ++i)
		{
			(*pVec)[i]->SetVisibleSingle(FALSE);
		}

		return;
	}

}

void RnOldLargeMapWindow::UpdateGate()
{
	if( m_pSettingInterface->IsVisibleGate() == false )
	{
		SetVisibleOff(&m_GateMarkVec);
		return;
	}
	
	for (size_t i = 0; i < m_GateMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_GateMarkVec[i],m_GateMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}


void RnOldLargeMapWindow::UpdateNormalNpc()
{
	if( m_pSettingInterface->IsVisibleNormalNpc() == false )
	{
		SetVisibleOff(&m_NormalNpcMarkVec);		
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 퀘스트가 갱신될 경우 다시 만들어줘야함
	const GLQuestPlay::MAPQUEST& mapQuest = m_pGaeaClient->GetCharacter()->m_cQuestPlay.GetQuestProc();
	std::map<DWORD,int> testProcQuest;	
	for (GLQuestPlay::MAPQUEST::const_iterator it = mapQuest.begin(); it != mapQuest.end(); ++it)
	{
		testProcQuest.insert( std::make_pair(it->first,it->second->m_dwSTEP) );
	}

	// 윈도우 자체가 단일 객체이고 다른데서 쓰이는데가 없으므로 static 으로 간단히 처리하자
	static	std::map<DWORD,int> testBeforeQuest;
	
	if( testProcQuest != testBeforeQuest )
	{
		if( !testBeforeQuest.empty() )
			MakeNpcInfo();
		
		testBeforeQuest = testProcQuest;
	}
	//////////////////////////////////////////////////////////////////////////

	for (size_t i = 0; i < m_NormalNpcMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_NormalNpcMarkVec[i],m_NormalNpcMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdateZoneName()
{
	if( m_pSettingInterface->IsVisiblePlaceName() == false )
	{
		SetVisibleOff(&m_ZoneNameVec);
		return;
	}


	for (size_t i = 0; i < m_ZoneNameVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_ZoneNameVec[i],m_ZoneNameVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdateGateName()
{
    if( m_pSettingInterface->IsVisibleGate() == false )
    {
        SetVisibleOff(&m_GateNameVec);
        return;
    }


    for (size_t i = 0; i < m_GateNameVec.size(); ++i)
    {
        
        UPDATE_CHAR_POS(m_GateNameVec[i],m_GateNameVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
    }
}

void RnOldLargeMapWindow::UpdateBusStation()
{
	if( m_pSettingInterface->IsVisibleBusStation() == false )
	{
		SetVisibleOff(&m_BusStationMarkVec);
		return;
	}

	for (size_t i = 0; i < m_BusStationMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_BusStationMarkVec[i],m_BusStationMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdateLookerRoom()
{
	if( m_pSettingInterface->IsVisibleLockerManager() == false )
	{
		SetVisibleOff(&m_LockerManagerMarkVec);
		return;
	}

	for (size_t i = 0; i < m_LockerManagerMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_LockerManagerMarkVec[i],m_LockerManagerMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdateStartPosition()
{
	if( m_pSettingInterface->IsVisibleStartPosition() == false )
	{
		SetVisibleOff(&m_StartPositionMarkVec);
		return;
	}
	for (size_t i = 0; i < m_StartPositionMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_StartPositionMarkVec[i],m_StartPositionMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdateClubManager()
{
	if( m_pSettingInterface->IsVisibleClubManager() == false )
	{
		SetVisibleOff(&m_ClubManagerMarkVec);
		return;
	}
	for (size_t i = 0; i < m_ClubManagerMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_ClubManagerMarkVec[i],m_ClubManagerMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

// void RnOldLargeMapWindow::UpdateItemSearch()
// {
// 	if( m_pSettingInterface->IsVisibleItemSearch() == false )
// 	{
// 		SetVisibleOff(&m_ItemSearchMarkVec);
// 		return;
// 	}
// 	for (size_t i = 0; i < m_ItemSearchMarkVec.size(); ++i)
// 	{
// 		UPDATE_CHAR_POS(m_ItemSearchMarkVec[i],m_ItemSearchMarkVec[i]->m_Position,m_LevelFile.GetMapID());
// 	}
// }
// 
// void RnOldLargeMapWindow::UpdateItemMix()
// {
// 	if( m_pSettingInterface->IsVisibleItemMix() == false )
// 	{
// 		SetVisibleOff(&m_ItemMixMarkVec);
// 		return;
// 	}
// 	for (size_t i = 0; i < m_ItemMixMarkVec.size(); ++i)
// 	{
// 		UPDATE_CHAR_POS(m_ItemMixMarkVec[i],m_ItemMixMarkVec[i]->m_Position,m_LevelFile.GetMapID());
// 	}
// }

void RnOldLargeMapWindow::UpdatePostBox()
{
	if( m_pSettingInterface->IsVisiblePostBox() == false )
	{
		SetVisibleOff(&m_PostBoxMarkVec);
		return;
	}
	for (size_t i = 0; i < m_PostBoxMarkVec.size(); ++i)
	{
		UPDATE_CHAR_POS(m_PostBoxMarkVec[i],m_PostBoxMarkVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
	}
}

void RnOldLargeMapWindow::UpdatePartyMember()
{
	if( m_pSettingInterface->IsVisiblePartyMember() == false )
	{
		SetVisibleOff(&m_PartyMarkVec);
		return;
	}
// 
// 	for (size_t i = 0; i < m_PartyMarkVec.size(); ++i)
// 	{ 
// 		UPDATE_CHAR_POS(m_PartyMarkVec[i],m_PartyMarkVec[i]->m_Position,m_pLevelFile->GetMapID());
// 	}

	UPDATE_PARTY();
}

void RnOldLargeMapWindow::UpdateSearchCharacter()
{
	int level;
	std::string name;
	DWORD mapID;
	D3DXVECTOR3 pos;
	if( m_pSearchInterface->GetSearchedCharacter(&level,&name,&mapID,&pos) == false )
	{
		m_pSearchCharacter->SetVisibleSingle(FALSE);
		return;
	}

	m_pSearchCharacter->m_Name = sc::string::format("Lv.%1% %2%",level,name);
	m_pSearchCharacter->m_Position = pos;

	UPDATE_CHAR_POS(m_pSearchCharacter,m_pSearchCharacter->m_Position,mapID);
}


void RnOldLargeMapWindow::ShowTooltip( DWORD monsterID )
{
	const MarkVector& markVec = m_MonsterMarkMap[monsterID];
	GASSERT(!markVec.empty());
	MakeToolTip(markVec[0],m_pInterface);
}


void RnOldLargeMapWindow::UpdateWithManualFor_NoID_UI( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//NO_ID로 생성된 넘들이기때문에 수동으로 Translate를 해줘야한다
	for (MarkVector::reverse_iterator it = m_MarkTotalVec.rbegin(); it != m_MarkTotalVec.rend(); ++it)
	{
		if( !(*it)->IsVisible() )
			continue;

		(*it)->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		DWORD dwMsg = (*it)->GetMessageEx();
		if( CHECK_MOUSE_IN(dwMsg) )
			MakeToolTip(*it,m_pInterface);
	}

	for (std::map<DWORD,MarkVector>::reverse_iterator it = m_MonsterMarkMap.rbegin(); it != m_MonsterMarkMap.rend(); ++it )
	{
		const MarkVector& markVec = it->second;
		for (size_t i =0; i < markVec.size(); ++i)
		{
			if( !markVec[i]->IsVisible() )
				continue;

			markVec[i]->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
			DWORD dwMsg = markVec[i]->GetMessageEx();
			if( CHECK_MOUSE_IN(dwMsg) )
				MakeToolTip(markVec[i],m_pInterface);
		}
	}

	if( m_pSearchCharacter )
	{
		if( m_pSearchCharacter->IsVisible() )
		{
			m_pSearchCharacter->Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
			DWORD dwMsg = m_pSearchCharacter->GetMessageEx();
			if( CHECK_MOUSE_IN(dwMsg) )
				MakeToolTip(m_pSearchCharacter,m_pInterface);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for(MarkVector::reverse_iterator it = m_GateMarkVec.rbegin(); it != m_GateMarkVec.rend(); ++it)
	{
		Mark* pMark = *it;
		if( !pMark->IsVisible() )
			continue;

		pMark->Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		DWORD dwMsg = pMark->GetMessageEx();

		if( CHECK_MOUSE_IN(dwMsg) & CHECK_RB_UP_LIKE(dwMsg) )
		{
			m_pChangeMap->SetMapLevelFile(pMark->m_iAddData1);
			return;
		}
	}
}

void RnOldLargeMapWindow::GetMobSchVec( std::vector<GLMobSchedule*>* pCrowVec )
{
	const MOBSCHEDULELIST* pMobScheduleList = m_pLevelFile->GetMobSchMan()->GetMobSchList();
	MOBSCHEDULENODE* pCur = pMobScheduleList->m_pHead;

	while(pCur)
	{
		const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(pCur->Data->m_CrowID);
		if( pCrowData )
		{
			if( pCrowData->m_emCrow == CROW_MOB )//&& pCrowData->m_bHideMinimapInfo == false )
			{
				pCrowVec->push_back(pCur->Data);
			}
		}

		pCur = pCur->pNext;
	}
}

void RnOldLargeMapWindow::MakeNpcInfo()
{
	MakeGate();
	MakeNormalNpc();
    MakeZoneNameNpc();
	MakeGateNameNpc();
	MakeBusStation();
	MakeLookerRoom();
	MakeStartPosition();
	MakeClubManager();
	// 	MakeItemSearch();
	// 	MakeItemMix();	
	MakePostBox();
	MakePartyMember();

	m_MarkTotalVec.clear();

	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_GateMarkVec.begin(),m_GateMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_NormalNpcMarkVec.begin(),m_NormalNpcMarkVec.end());
	//m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_PlaceNameTextVec.begin(),m_PlaceNameTextVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_BusStationMarkVec.begin(),m_BusStationMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_LockerManagerMarkVec.begin(),m_LockerManagerMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_StartPositionMarkVec.begin(),m_StartPositionMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_ClubManagerMarkVec.begin(),m_ClubManagerMarkVec.end());
	// 	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_ItemSearchMarkVec.begin(),m_ItemSearchMarkVec.end());
	// 	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_ItemMixMarkVec.begin(),m_ItemMixMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_PostBoxMarkVec.begin(),m_PostBoxMarkVec.end());
	m_MarkTotalVec.insert(m_MarkTotalVec.end(),m_PartyMarkVec.begin(),m_PartyMarkVec.end());
}

void RnOldLargeMapWindow::MakeMonsterInfo()
{
	//////////////////////////////////////////////////////////////////////////

	for(std::map<DWORD,MarkVector>::iterator it = m_MonsterMarkMap.begin(); it != m_MonsterMarkMap.end(); ++it)
	{
		MarkVector& markVec = it->second;
		for (size_t i=0; i < markVec.size(); ++i)
		{
			DeleteControl(markVec[i]->GetWndID(),true);
		}
	}

	m_MonsterMarkMap.clear();

	//////////////////////////////////////////////////////////////////////////


	m_MobScheduleVecMap.clear();
	MobScheduleVec crowVec;
	GetMobSchVec(&crowVec);

	for (size_t i=0; i < crowVec.size(); ++i)
	{
		if( m_MobScheduleVecMap[ crowVec[i]->m_CrowID.Id() ].empty() )
			m_MobScheduleVecMap[ crowVec[i]->m_CrowID.Id() ].reserve(150);

		m_MobScheduleVecMap[ crowVec[i]->m_CrowID.Id() ].push_back(crowVec[i]);
	}


	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
//	NS_UITEXTUTIL::MULTILINETEXT multiLine = NS_UITEXTUTIL::GetMLTextWithoutCRLF(ID2GAMEEXTEXT_MONSTER_EXPLAIN,350,pFont);

	//////////////////////////////////////////////////////////////////////////

	for (std::map<DWORD,MobScheduleVec>::iterator it = m_MobScheduleVecMap.begin(); it != m_MobScheduleVecMap.end(); ++it)
	{
		DWORD monsterID = it->first;
		m_MonsterMarkMap[monsterID].reserve(150);
		SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData(monsterID);

		const MobScheduleVec& mobSchVec = it->second;
		std::vector<DWORD> questIdVec;
		questIdVec.reserve(30);
		RnMapPrivate::GetQuestFromMonsterID(m_pGaeaClient,monsterID,&questIdVec);

		std::vector<Mark::Comment> questCommectVec;
		for (size_t i = 0; i < questIdVec.size(); ++i)
		{
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find(questIdVec[i]);
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
				questCommectVec.push_back( Mark::Comment(std::string() + ID2GAMEWORD_RELATIVE_QUEST + " : " + CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString(),D3DCOLOR_XRGB(0,255,0)) );
			}
			else
			{
				questCommectVec.push_back( Mark::Comment(std::string() + ID2GAMEWORD_RELATIVE_QUEST + " : " + pQuest->GetTITLE(),D3DCOLOR_XRGB(0,255,0)) );
			}
		}

		for (size_t i = 0; i < mobSchVec.size(); ++i)
		{
			Mark* pIcon = new Mark(m_pEngineDevice);
			pIcon->CreateSub(this,"MAP_WINDOW_INFO_TAP_MONSTER1_ICON");
			pIcon->InitDeviceObjects ( m_pd3dDevice );
			//  [12/4/2012 gbgim]
			pIcon->RestoreDeviceObjects ( m_pd3dDevice );
			//pIcon->ReCalcPos();
			pIcon->m_Position = mobSchVec[i]->m_pAffineParts->vTrans;
			
			pIcon->m_CommentVec.reserve(10);

			pIcon->m_Name = pCrowData->GetName();
			pIcon->m_canTrade = Mark::NO_VISIBLE_TRADE;
			//////////////////////////////////////////////////////////////////////////
			// 이름
//			pIcon->m_CommentVec.push_back( Mark::Comment(pCrowData->GetName(),D3DCOLOR_XRGB(255,255,255)) );
			// 난이도
			RnMapPrivate::MONSTER_TYPE StrongType = RnMapPrivate::GetMonsterStrongType(m_pGaeaClient,pCrowData);
			const char* pStrongText = NULL;
			DWORD color = D3DCOLOR_XRGB(255,255,255);
			if( RnMapPrivate::MONSTER_TYPE_VERY_WEAK == StrongType)
			{
				pStrongText = ID2GAMEEXTEXT_MONSTER_WEAK;
				color		= D3DCOLOR_XRGB(0,255,0);
			}
			else if( RnMapPrivate::MONSTER_TYPE_WEAK == StrongType)
			{
				pStrongText = ID2GAMEEXTEXT_MONSTER_NORMAL;
				color		= D3DCOLOR_XRGB(255,255,255);
			}
			else if( RnMapPrivate::MONSTER_TYPE_NORMAL == StrongType )
			{
				pStrongText = ID2GAMEEXTEXT_MONSTER_STRONG;
				color = D3DCOLOR_XRGB(255,255,0);
			}
			else if( RnMapPrivate::MONSTER_TYPE_STRONG == StrongType )
			{
				pStrongText = ID2GAMEEXTEXT_MONSTER_VERY_STRONG;
				color = D3DCOLOR_XRGB(255,0,0);
			}
			else
			{
				GASSERT(false);
			}

			pIcon->m_CommentVec.push_back( Mark::Comment(pStrongText,color) );


//			pIcon->m_CommentVec.push_back( Mark::Comment() );
			//////////////////////////////////////////////////////////////////////////
			// 관련 퀘스트
			if(!questCommectVec.empty())
			{
				pIcon->m_CommentVec.push_back( Mark::Comment() );
				pIcon->m_CommentVec.insert( pIcon->m_CommentVec.end(),questCommectVec.begin(),questCommectVec.end());
			}

//			pIcon->m_CommentVec.push_back( Mark::Comment() );

			//////////////////////////////////////////////////////////////////////////
			// 조작 설명
// 			for (size_t i=0; i < multiLine.size(); ++i)
// 				pIcon->m_CommentVec.push_back( Mark::Comment(multiLine[i].strLine.GetString(),D3DCOLOR_XRGB(255,255,255)) );

			//////////////////////////////////////////////////////////////////////////
			RnMapPrivate::ChangeTexture(pIcon,RnMapPrivate::GetMonsterIconType(m_pGaeaClient,pCrowData));

			RegisterControl(pIcon);
			m_MonsterMarkMap[monsterID].push_back(pIcon);
		}
	}


}

void RnOldLargeMapWindow::UpdateMonsterInfo()
{
	for (std::map<DWORD,MarkVector>::iterator it = m_MonsterMarkMap.begin(); it != m_MonsterMarkMap.end(); ++it)
	{
		DWORD monsterID = it->first;
		bool isVisible = m_pInfoInterface->IsVisibleMonster(monsterID);

		MarkVector& markVec = it->second;
		for (size_t i = 0; i < markVec.size(); ++i)
		{
			markVec[i]->SetVisibleSingle(isVisible);
			if(isVisible)
				UPDATE_CHAR_POS(markVec[i],markVec[i]->m_Position,m_pLevelFile->GetBaseMapID());
		}
	}
	

}



// void RnOldLargeMapWindow::GetMonster(int startIndex,int endIndex,std::vector<DWORD>* pVec)
// {
// 	std::map<DWORD,MobScheduleVec>::iterator begin_it = m_MobScheduleVecMap.begin();
// 	std::map<DWORD,MobScheduleVec>::iterator end_it = m_MobScheduleVecMap.begin();
// 
// 	if( startIndex >= m_MobScheduleVecMap.size() )
// 		begin_it = m_MobScheduleVecMap.end();
// 	else
// 		std::advance(begin_it,startIndex);
// 
// 	if( std::distance(begin_it,m_MobScheduleVecMap.end()) < endIndex - startIndex )
// 	{
// 		end_it = m_MobScheduleVecMap.end();
// 	}
// 	else
// 	{
// 		std::advance(end_it,endIndex);
// 	}
// 
// 	for(std::map<DWORD,MobScheduleVec>::iterator it = begin_it; it != end_it; ++it)
// 	{
// 		pVec->push_back( it->first );
// 	}
// }

void RnOldLargeMapWindow::GetMonster(std::vector<DWORD>* pVec)
{
	for(std::map<DWORD,MobScheduleVec>::iterator it = m_MobScheduleVecMap.begin(); it != m_MobScheduleVecMap.end(); ++it)
	{
		pVec->push_back( it->first );
	}
}



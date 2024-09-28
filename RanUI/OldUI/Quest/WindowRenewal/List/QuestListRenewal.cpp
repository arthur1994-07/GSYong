#include "StdAfx.h"

#include "./QuestListRenewal.h"
#include "../../../../../EngineLib/DeviceMan.h"
#include "../../../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../QuestWindowRenewal.h"
#include "../Main/QuestMainRenewal.h"
#include "../Main/QuestMainEndRenewal.h"
#include "../Node/QuestListParent.h"
#include "../Node/QuestListChild.h"
#include "../../../../../EngineLib/Common/ByteStream.h"
#include "../../../../../RanLogic/Quest/GLQuestPlay.h"
#include "../../../../../RanLogic/Quest/GLQuest.h"
#include "../../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../InnerInterface.h"
#include "../../../../../EngineLib/GUInterface/TipBox.h"
#include "../../../../../RanLogic/RANPARAM.h"
#include "../../../../../RanLogic/GLUseFeatures.h"
#include "../../../../../EngineLib/GUInterface/GameTextControl.h"

const	int CQuestListRenewal::nLimitQuest = 1000;

CQuestListRenewal::CQuestListRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
	, m_pListScrollBarProg(NULL)
	, m_pListScrollBarEnd(NULL)
	, m_pLineBox(NULL)
	, m_iStartIndexForIng(0)
    , m_iStartIndexForEnd(0)
	, m_pStartTemp(NULL)
	, m_nTotalLine(0)
	, m_nLinePerOneView(0)
	, m_dwQuestIngID( NATIVEID_NULL().dwID )
	, m_dwQuestEndID( NATIVEID_NULL().dwID )
	, m_pSelect(NULL)
{
	for ( int i = 0; i < g_nLimitQuestCount; i++ )
	{
		m_pParentList[ i ] = NULL;
		m_pChildList[ i ] = NULL;
	}

	m_multimapArea.clear();
	m_multimapEndArea.clear();
}

CQuestListRenewal::~CQuestListRenewal ()
{
}

void CQuestListRenewal::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "QUEST_WINDOW_LIST_PAGE_BACK_LUXES" );
		RegisterControl ( pBasicLineBox );
		m_pLineBox = pBasicLineBox;
	}

	{
		CUIControl* pControl = new CUIControl(m_pEngineDevice);
		pControl->CreateSub ( this, "QUEST_WINDOW_LIST_TEXT_START_LUXES" );
		RegisterControl ( pControl );
		m_pStartTemp = pControl;
		m_pStartTemp->SetVisibleSingle( FALSE );
	}

	{

		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT,
            static_cast<DWORD>(m_pStartTemp->GetGlobalPos().sizeY), _DEFAULT_FONT_FLAG );

		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
		pTextBox->CreateSub ( this, "QUEST_WINDOW_LIST_PAGE_LUXES", UI_FLAG_DEFAULT, QUEST_LIST_TEXT_BOX );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetSensitive ( true );
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetLimitLine ( nLimitQuest );
		RegisterControl ( pTextBox );
		//m_pListTextEnd = pTextBox;	

		m_nTotalLine = pTextBox->GetVisibleLine ();
		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_LIST_SCROLL_BAR_PROG );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_LIST_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, m_nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pLineBox );
		RegisterControl ( pScrollBar );
		m_pListScrollBarProg = pScrollBar;

		pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_LIST_SCROLL_BAR_END );
		pScrollBar->CreateBaseScrollBar ( "QUEST_WINDOW_LIST_SCROLL_LUXES" );
		pScrollBar->GetThumbFrame()->SetState ( 1, m_nTotalLine );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pLineBox );
		RegisterControl ( pScrollBar );
		m_pListScrollBarEnd = pScrollBar;

		m_nLinePerOneView = g_nLimitQuestCount;
	}

	{
		for ( int i = 0; i < g_nLimitQuestCount; i++ )
		{
			m_pParentList[ i ] = new CQuestListParent( m_pInterface, m_pEngineDevice );
			m_pParentList[ i ]->CreateSub ( this, "QUEST_WINDOW_LIST_PARENT_BOX_LUXES", UI_FLAG_DEFAULT, ( QUEST_LIST_PARENT + i ) );
			m_pParentList[ i ]->CreateSubControl();
			RegisterControl ( m_pParentList[ i ] );
			m_pParentList[ i ]->SetVisibleSingle( FALSE );
		}

		for ( int i = 0; i < g_nLimitQuestCount; i++ )
		{
			m_pChildList[ i ] = new CQuestListChild( m_pGaeaClient, m_pInterface, m_pEngineDevice );
			m_pChildList[ i ]->CreateSub ( this, "QUEST_WINDOW_LIST_CHILD_BOX_LUXES", UI_FLAG_DEFAULT, ( QUEST_LIST_CHILD + i ) );
			m_pChildList[ i ]->CreateSubControl();
			RegisterControl ( m_pChildList[ i ] );
			m_pChildList[ i ]->SetVisibleSingle( FALSE );
		}
	}

	{
		m_pSelect = new CTipBox(m_pEngineDevice);(m_pEngineDevice);
		m_pSelect->CreateSub( this, "QUEST_WINDOW_CHILD_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, QUEST_LIST_SELECT );
		m_pSelect->CreateQuestSelect( "QUEST_WINDOW_LIST_CHILD_BOX_LUXES" );
		RegisterControl ( m_pSelect );
		m_pSelect->SetVisibleSingle( FALSE );
	}
}

void CQuestListRenewal::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{
		m_pListScrollBarProg->SetVisibleSingle( TRUE );
		m_pListScrollBarEnd->SetVisibleSingle( FALSE );

		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBarProg->GetThumbFrame ();

		//const int nTotalLine = m_pListTextProg->GetTotalLine ();
		//const int nLinePerOneView = m_pListTextProg->GetVisibleLine ();
		//CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );		
		pThumbFrame->SetState ( m_nTotalLine, m_nLinePerOneView );
		if ( m_nLinePerOneView < m_nTotalLine )
		{
			const int nMovableLine = m_nTotalLine - m_nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			//m_pListTextProg->SetCurLine ( nPos );
			if ( nPos != m_iStartIndexForIng )
			{
				m_iStartIndexForIng = nPos;
				LoadQuestList();
			}
			else
				m_iStartIndexForIng = nPos;
		}
	}

	else if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
	{
		m_pListScrollBarProg->SetVisibleSingle( FALSE );
		m_pListScrollBarEnd->SetVisibleSingle( TRUE );

		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBarEnd->GetThumbFrame ();

		//const int nTotalLine = m_pListTextEnd->GetTotalLine ();
		//const int nLinePerOneView = m_pListTextEnd->GetVisibleLine ();
		//CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( m_nTotalLine, m_nLinePerOneView );
		if ( m_nLinePerOneView < m_nTotalLine )
		{
			const int nMovableLine = m_nTotalLine - m_nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			//m_pListTextEnd->SetCurLine ( nPos );
			if ( nPos != m_iStartIndexForEnd )
			{
				m_iStartIndexForEnd = nPos;
				LoadQuestList();
			}
			else
				m_iStartIndexForEnd = nPos;
		}
	}
}

void CQuestListRenewal::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CQuestListRenewal::AutoLoadQuestList( DWORD dwQuestID )
{
	int nIndex = 0;

	MULTIMAP_AREA_ITER iter_area;

	if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{
		for ( iter_area = m_multimapArea.begin(); iter_area != m_multimapArea.end(); iter_area++ )
		{
			SAREA_LIST& sArea = iter_area->second;

			nIndex += 1;

			MAP_QUEST_LIST mapQuest = sArea.GetQuestList();
			MAP_QUEST_ITER iter = mapQuest.find( dwQuestID );

			if ( iter != mapQuest.end() )
			{
				sArea.m_bOpen = true; // 강제로 UI 연다.
				RecalSize();

				MAP_QUEST_ITER iter_index = mapQuest.begin();
				for ( ; iter_index != mapQuest.end(); iter_index++, nIndex++ )
				{
					if ( dwQuestID == iter_index->first )
					{
						if ( nIndex + m_nLinePerOneView > m_nTotalLine )
							nIndex = m_nTotalLine - m_nLinePerOneView;

						m_iStartIndexForIng = nIndex;

						if ( m_nLinePerOneView < m_nTotalLine )
						{
							const int nMovableLine = m_nTotalLine - m_nLinePerOneView;
							float fPercent = m_iStartIndexForIng / (float)nMovableLine;
							m_pListScrollBarProg->GetThumbFrame()->SetPercent( fPercent );
						}

						m_dwQuestIngID = dwQuestID;

						LoadQuestList();
					}
				}

			}
			else
			{
				nIndex += static_cast<int>(sArea.GetQuestList().size());
			}
		}
	}
}

void CQuestListRenewal::SetQuestID( DWORD dwID )
{

	if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{	// 진행 중 페이지일때 셀렉트.
		bool bFind = false;
		if ( dwID != NATIVEID_NULL().dwID )
		{
			for ( int i = 0; i < g_nLimitQuestCount; i++ )
			{
				if ( m_pChildList[ i ]->IsVisible()
					&& m_pChildList[ i ]->GetID() == dwID )
				{
					m_pSelect->SetLocalPos( m_pChildList[ i ]->GetLocalPos() );
					m_pSelect->SetGlobalPos( m_pChildList[ i ]->GetGlobalPos() );

					m_pSelect->SetVisibleSingle( TRUE );

					m_pChildList[ i ]->SetSelect();

					m_pInterface->GetQuestWindowRenewal()->LoadQuestMain( dwID );

					m_dwQuestIngID = dwID;

					bFind = true;
				}
				else
					m_pChildList[ i ]->UnSelect();
			}

			if ( !bFind )
			{
				m_pSelect->SetVisibleSingle( FALSE );

				//m_pInterface->GetQuestWindowRenewal()->GetQuestMain()->UnLoadMain();
			}
		}
		else
			m_pSelect->SetVisibleSingle( FALSE );
	}
	else if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
	{	// 종료된 페이지 셀렉트.
		bool bFind = false;
		if ( dwID != NATIVEID_NULL().dwID )
		{
			for ( int i = 0; i < g_nLimitQuestCount; i++ )
			{
				if ( m_pChildList[ i ]->IsVisible()
					&& m_pChildList[ i ]->GetID() == dwID )
				{
					m_pSelect->SetLocalPos( m_pChildList[ i ]->GetLocalPos() );
					m_pSelect->SetGlobalPos( m_pChildList[ i ]->GetGlobalPos() );

					m_pSelect->SetVisibleSingle( TRUE );

					m_pChildList[ i ]->SetSelect();

					m_pInterface->GetQuestWindowRenewal()->LoadQuestMainEnd( dwID );

					m_dwQuestEndID = dwID;

					bFind = true;
				}
				else
					m_pChildList[ i ]->UnSelect();
			}

			if ( !bFind )
			{
				m_pSelect->SetVisibleSingle( FALSE );

				//m_pInterface->GetQuestWindowRenewal()->GetQuestMain()->UnLoadMain();
			}
		}
		else
			m_pSelect->SetVisibleSingle( FALSE );
	}
}

void CQuestListRenewal::RecalSize()
{
	m_nTotalLine = 0;

	MULTIMAP_AREA_ITER iter_area;

	if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{
		for ( iter_area = m_multimapArea.begin(); iter_area != m_multimapArea.end(); iter_area++ )
		{
			SAREA_LIST& sArea = iter_area->second;

			m_nTotalLine += 1;

			if ( sArea.m_bOpen )
			{
				m_nTotalLine += static_cast<int>(sArea.GetQuestList().size());
			}
		}
	}
	else if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
	{
		for ( iter_area = m_multimapEndArea.begin(); iter_area != m_multimapEndArea.end(); iter_area++ )
		{
			SAREA_LIST& sArea = iter_area->second;

			m_nTotalLine += 1;

			if ( sArea.m_bOpen )
			{
				m_nTotalLine += static_cast<int>(sArea.GetQuestList().size());
			}
		}
	}
}

void CQuestListRenewal::CheckDeleteList()
{
	CheckDeleteIngList();
	CheckDeleteEndList();
}

void CQuestListRenewal::CheckDeleteIngList()
{
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;

	GLQuestPlay::MAPQUEST QuestProcMap = cQuestPlay.GetQuestProc ();
	GLQuestPlay::MAPQUEST_ITER iter = QuestProcMap.begin ();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestProcMap.end ();

	MULTIMAP_AREA_ITER iter_area = m_multimapArea.begin();

	for ( ;iter_area != m_multimapArea.end(); )
	{
		SAREA_LIST& sArea = iter_area->second;
		MAP_QUEST_LIST& questList = sArea.GetQuestList();

		MAP_QUEST_ITER iter_list = questList.begin();
		for ( ; iter_list != questList.end(); )
		{
			GLQuestPlay::MAPQUEST_ITER iter_for_del = QuestProcMap.find( iter_list->first );

			if ( iter_for_del == QuestProcMap.end() )
			{	// 자식의 퀘스트가 진행중인 퀘스트 목록에 없다면, 삭제해야 한다.
				questList.erase( iter_list++ );
			}
			else
				iter_list++;
		}

		if ( questList.size() == 0 )
		{	// 등록된 자식 퀘스트가 없다면 해당 지역은 삭제한다.
			m_multimapArea.erase( iter_area++ );
		}
		else
			iter_area++;
	}
}

void CQuestListRenewal::CheckDeleteEndList()
{
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;

	GLQuestPlay::MAPQUEST QuestEndMap = cQuestPlay.GetQuestEnd ();
	GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin ();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end ();

	MULTIMAP_AREA_ITER iter_area = m_multimapEndArea.begin();

	for ( ;iter_area != m_multimapEndArea.end(); )
	{
		SAREA_LIST& sArea = iter_area->second;
		MAP_QUEST_LIST& questList = sArea.GetQuestList();

		MAP_QUEST_ITER iter_list = questList.begin();
		for ( ; iter_list != questList.end(); )
		{
			GLQuestPlay::MAPQUEST_ITER iter_for_del = QuestEndMap.find( iter_list->first );

			if ( iter_for_del == QuestEndMap.end() )
			{	// 자식의 퀘스트가 종료된 퀘스트 목록에 없다면, 삭제해야 한다.
				questList.erase( iter_list++ );
			}
			else
				iter_list++;
		}

		if ( questList.size() == 0 )
		{	// 등록된 자식 퀘스트가 없다면 해당 지역은 삭제한다.
			m_multimapEndArea.erase( iter_area++ );
		}
		else
			iter_area++;
	}
}

bool CQuestListRenewal::LoadQuestList()
{
	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter ()->m_cQuestPlay;

	for ( int i = 0; i < g_nLimitQuestCount; i++ )
	{
		m_pParentList[ i ]->SetVisibleSingle( FALSE );
		m_pChildList[ i ]->SetVisibleSingle( FALSE );
	}

	CheckDeleteList();
	RecalSize();

	if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
	{
		GLQuestPlay::MAPQUEST QuestProcMap = cQuestPlay.GetQuestProc ();
		GLQuestPlay::MAPQUEST_ITER iter = QuestProcMap.begin ();
		GLQuestPlay::MAPQUEST_ITER iter_end = QuestProcMap.end ();

		for ( ; iter != iter_end; iter++ )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			DWORD dwNID = pQuestProg->m_sNID.dwID;
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwNID );
			if ( !pQuest ) continue;

			if ( m_multimapArea.end() == m_multimapArea.find( pQuest->m_dwQuestArea ) )
			{	// 이 지역이 등록된 적이 없다면.

				SAREA_LIST sArea( pQuest->m_dwQuestArea );
				m_multimapArea.insert( std::make_pair( pQuest->m_dwQuestArea, sArea ) );
			}

			MULTIMAP_AREA_ITER iter_area = m_multimapArea.find( pQuest->m_dwQuestArea );
			if ( iter_area != m_multimapArea.end() )
			{
				SAREA_LIST& sArea = iter_area->second;
				MAP_QUEST_LIST& questList = sArea.GetQuestList();

				MAP_QUEST_ITER iter_quest = questList.find( pQuest->m_sNID.dwID );
				if ( iter_quest == questList.end() )
				{	// 이 퀘스가 등록된 적이 없다면.
                    DWORD dwQuestID = pQuest->m_sNID.dwID;
					SQUEST_LIST sQuestList( dwQuestID );

                    // 일치하는 퀘스트 아이디를 찾는다.
                    for ( DWORD i=0; i<RANPARAM::MAX_QUEST_DISPLAY; ++i )
                    {
                        if ( dwQuestID == RANPARAM::QuestDisplayID[ i ] )
                        {
                            sQuestList.m_bCheck = true;
                            break;
                        }
                    }

					questList.insert( std::make_pair( pQuest->m_sNID.dwID, sQuestList ) );
				}
			}
		}

		//CheckDeleteIngList();

		MULTIMAP_AREA_ITER iter_area;
		int iCurCount = 0;
		int iCurParent = 0;
		int iCurChild = 0;
		int iCurIndex = 0;

		for ( iter_area = m_multimapArea.begin(); iter_area != m_multimapArea.end(); iter_area++ )
		{
			SAREA_LIST& sArea = iter_area->second;

			if ( iCurCount >= g_nLimitQuestCount )
				break;

			CString strQuestAreaMap;
			//SMAPNODE* pNODE = m_pGaeaClient->FindMapNode( SNATIVEID( iter_area->first ) );
			//if ( pNODE )	strQuestAreaMap = pNODE->strMapName.c_str();
			//else			strQuestAreaMap = ID2GAMEINTEXT( "QUEST_AREA_ETC" );
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strQuestAreaMap.Format( "%s(%d)", CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", iter_area->first).GetString(), sArea.GetQuestList().size() );
			else
				strQuestAreaMap.Format( "%s(%d)", COMMENT::QUEST_AREA_NAME.at( iter_area->first ).c_str(), sArea.GetQuestList().size() );

			if ( iCurIndex >= m_iStartIndexForIng )
			{
				m_pParentList[ iCurParent ]->SetID( iter_area->first, sArea.m_bOpen );
				m_pParentList[ iCurParent ]->SetText( strQuestAreaMap );

				const UIRECT& rcWindow = m_pStartTemp->GetLocalPos();
				D3DXVECTOR2 vPos;
				vPos.x = rcWindow.left;
				vPos.y = rcWindow.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

				m_pParentList[ iCurParent ]->SetLocalPos( vPos );

				const UIRECT& rcWindowG = m_pStartTemp->GetGlobalPos();
				vPos.x = rcWindowG.left;
				vPos.y = rcWindowG.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

				m_pParentList[ iCurParent ]->SetGlobalPos( vPos );

				m_pParentList[ iCurParent ]->SetVisibleSingle( TRUE );

				iCurParent++;
				iCurCount++; // 한 개 추가.
			}
			else
				iCurIndex++;

			if ( sArea.m_bOpen == true ) // 부모UI 열림.
			{	// 자식들 그림.

				MAP_QUEST_LIST& questList = sArea.GetQuestList();
				MAP_QUEST_ITER iter_list;

				for ( iter_list = questList.begin(); iter_list != questList.end(); iter_list++ )
				{
					if ( iCurCount >= g_nLimitQuestCount )
						break;

					GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( iter_list->first );
					if ( !pQuest ) continue;

					if ( iCurIndex >= m_iStartIndexForIng )
					{
						SQUEST_LIST& sQuest = iter_list->second;

						m_pChildList[ iCurChild ]->SetID( iter_list->first, sQuest.m_bCheck );
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							CString strKey;
							strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
							m_pChildList[ iCurChild ]->SetText(CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
						}
						else
						{
							m_pChildList[ iCurChild ]->SetText( pQuest->GetTITLE() );
						}

						const UIRECT& rcWindow = m_pStartTemp->GetLocalPos();
						D3DXVECTOR2 vPos;
						vPos.x = rcWindow.left;
						vPos.y = rcWindow.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

						m_pChildList[ iCurChild ]->SetLocalPos( vPos );

						const UIRECT& rcWindowG = m_pStartTemp->GetGlobalPos();
						vPos.x = rcWindowG.left;
						vPos.y = rcWindowG.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

						m_pChildList[ iCurChild ]->SetGlobalPos( vPos );

						m_pChildList[ iCurChild ]->SetVisibleSingle( TRUE );

						iCurChild++;
						iCurCount++; // 한 개 추가.
					}
					else
						iCurIndex++;
				}
			}
		}

		SetQuestID( m_dwQuestIngID );
	}

	else if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
	{
		GLQuestPlay::MAPQUEST QuestEndMap = cQuestPlay.GetQuestEnd ();
		GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin ();
		GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end ();

		for ( ; iter != iter_end; iter++ )
		{
			GLQUESTPROG* pQuestProg = (*iter).second;
			DWORD dwNID = pQuestProg->m_sNID.dwID;
			GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwNID );
			if ( !pQuest ) continue;

			if ( m_multimapEndArea.end() == m_multimapEndArea.find( pQuest->m_dwQuestArea ) )
			{	// 이 지역이 등록된 적이 없다면.

				SAREA_LIST sArea( pQuest->m_dwQuestArea );
				m_multimapEndArea.insert( std::make_pair( pQuest->m_dwQuestArea, sArea ) );
			}

			MULTIMAP_AREA_ITER iter_area = m_multimapEndArea.find( pQuest->m_dwQuestArea );
			if ( iter_area != m_multimapEndArea.end() )
			{
				SAREA_LIST& sArea = iter_area->second;
				MAP_QUEST_LIST& questList = sArea.GetQuestList();

				MAP_QUEST_ITER iter_quest = questList.find( pQuest->m_sNID.dwID );
				if ( iter_quest == questList.end() )
				{	// 이 퀘스가 등록된 적이 없다면.
					SQUEST_LIST sQuestList( pQuest->m_sNID.dwID );
					questList.insert( std::make_pair( pQuest->m_sNID.dwID, sQuestList ) );
				}
			}
		}

		MULTIMAP_AREA_ITER iter_area;
		int iCurCount = 0;
		int iCurParent = 0;
		int iCurChild = 0;
		int iCurIndex = 0;

		for ( iter_area = m_multimapEndArea.begin(); iter_area != m_multimapEndArea.end(); iter_area++ )
		{
			SAREA_LIST& sArea = iter_area->second;

			if ( iCurCount >= g_nLimitQuestCount )
				break;

			CString strQuestAreaMap;
			//SMAPNODE* pNODE = m_pGaeaClient->FindMapNode( SNATIVEID( iter_area->first ) );
			//if ( pNODE )	strQuestAreaMap = pNODE->strMapName.c_str();
			//else			strQuestAreaMap = ID2GAMEINTEXT( "QUEST_AREA_ETC" );
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strQuestAreaMap.Format( "%s(%d)", CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", iter_area->first).GetString(), sArea.GetQuestList().size() );
			else
				strQuestAreaMap.Format( "%s(%d)", COMMENT::QUEST_AREA_NAME.at( iter_area->first ).c_str(), sArea.GetQuestList().size() );

			if ( iCurIndex >= m_iStartIndexForEnd )
			{
				m_pParentList[ iCurParent ]->SetID( iter_area->first, sArea.m_bOpen );
				m_pParentList[ iCurParent ]->SetText( strQuestAreaMap );

				const UIRECT& rcWindow = m_pStartTemp->GetLocalPos();
				D3DXVECTOR2 vPos;
				vPos.x = rcWindow.left;
				vPos.y = rcWindow.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

				m_pParentList[ iCurParent ]->SetLocalPos( vPos );

				const UIRECT& rcWindowG = m_pStartTemp->GetGlobalPos();
				vPos.x = rcWindowG.left;
				vPos.y = rcWindowG.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

				m_pParentList[ iCurParent ]->SetGlobalPos( vPos );

				m_pParentList[ iCurParent ]->SetVisibleSingle( TRUE );

				iCurParent++;
				iCurCount++; // 한 개 추가.
			}
			else
				iCurIndex++;

			if ( sArea.m_bOpen == true ) // 부모UI 열림.
			{	// 자식들 그림.

				MAP_QUEST_LIST& questList = sArea.GetQuestList();
				MAP_QUEST_ITER iter_list;

				for ( iter_list = questList.begin(); iter_list != questList.end(); iter_list++ )
				{
					if ( iCurCount >= g_nLimitQuestCount )
						break;

					GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( iter_list->first );
					if ( !pQuest ) continue;

					if ( iCurIndex >= m_iStartIndexForEnd )
					{
						GLQuestPlay::MAPQUEST_ITER iter_comcplete = QuestEndMap.find( iter_list->first );

						bool bComplete = false;
						if ( iter_comcplete != QuestEndMap.end() )
						{
							GLQUESTPROG* pQuestProg = (*iter_comcplete).second;

							if ( pQuestProg )
								bComplete = pQuestProg->m_bCOMPLETE;
						}

						m_pChildList[ iCurChild ]->SetID( iter_list->first, false, bComplete );
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							CString strKey;
							strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
							m_pChildList[ iCurChild ]->SetText(CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
						}
						else
						{
							m_pChildList[ iCurChild ]->SetText( pQuest->GetTITLE() );
						}

						const UIRECT& rcWindow = m_pStartTemp->GetLocalPos();
						D3DXVECTOR2 vPos;
						vPos.x = rcWindow.left;
						vPos.y = rcWindow.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

						m_pChildList[ iCurChild ]->SetLocalPos( vPos );

						const UIRECT& rcWindowG = m_pStartTemp->GetGlobalPos();
						vPos.x = rcWindowG.left;
						vPos.y = rcWindowG.top + ( ( rcWindow.sizeY + 2 ) * iCurCount );

						m_pChildList[ iCurChild ]->SetGlobalPos( vPos );

						m_pChildList[ iCurChild ]->SetVisibleSingle( TRUE );

						iCurChild++;
						iCurCount++; // 한 개 추가.
					}
					else
						iCurIndex++;
				}
			}
		}

		SetQuestID( m_dwQuestEndID );
	}

	return true;
}
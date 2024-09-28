#include "StdAfx.h"

#include "PostBoxHistoryList.h"
#include "PostBoxHistoryItem.h"

#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxHistoryList::CPostBoxHistoryList(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CPOSTBOX_HISTORY_TYPE historyType)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_dwPage(0)
    , m_dwPageMax(0)
    , m_historyType(historyType)
{
	GASSERT ( (MAX_ITEM != 0) && "MAX_ITEM 이 0입니다." );

	for ( UINT32 i=0; i<MAX_ITEM; ++i )
	{
		m_pItem[i] = 0;
	}
}

CPostBoxHistoryList::~CPostBoxHistoryList()
{
	// Blank
}

void CPostBoxHistoryList::CreateSubControl()
{
    switch ( m_historyType )
    {
        case POSTBOX_HISTORY_ALL:
	        m_pItem[0] = CreateItem ( "POSTBOX_PAGE_HISTORY_ITEM0", POSTBOX_PAGE_HISTORY_ITEM0 );
	        m_pItem[1] = CreateItem ( "POSTBOX_PAGE_HISTORY_ITEM1", POSTBOX_PAGE_HISTORY_ITEM1 );
	        m_pItem[2] = CreateItem ( "POSTBOX_PAGE_HISTORY_ITEM2", POSTBOX_PAGE_HISTORY_ITEM2 );
	        m_pItem[3] = CreateItem ( "POSTBOX_PAGE_HISTORY_ITEM3", POSTBOX_PAGE_HISTORY_ITEM3 );
	        m_pItem[4] = CreateItem ( "POSTBOX_PAGE_HISTORY_ITEM4", POSTBOX_PAGE_HISTORY_ITEM4 );
            break;

        case POSTBOX_HISTORY_SEND:
            m_pItem[0] = CreateItem ( "POSTBOX_PAGE_RECV_HISTORY_ITEM0", POSTBOX_PAGE_HISTORY_ITEM0 );
            m_pItem[1] = CreateItem ( "POSTBOX_PAGE_RECV_HISTORY_ITEM1", POSTBOX_PAGE_HISTORY_ITEM1 );
            m_pItem[2] = CreateItem ( "POSTBOX_PAGE_RECV_HISTORY_ITEM2", POSTBOX_PAGE_HISTORY_ITEM2 );
            m_pItem[3] = CreateItem ( "POSTBOX_PAGE_RECV_HISTORY_ITEM3", POSTBOX_PAGE_HISTORY_ITEM3 );
            m_pItem[4] = CreateItem ( "POSTBOX_PAGE_RECV_HISTORY_ITEM4", POSTBOX_PAGE_HISTORY_ITEM4 );
            break;

        case POSTBOX_HISTORY_RECV:
            m_pItem[0] = CreateItem ( "POSTBOX_PAGE_SEND_HISTORY_ITEM0", POSTBOX_PAGE_HISTORY_ITEM0 );
            m_pItem[1] = CreateItem ( "POSTBOX_PAGE_SEND_HISTORY_ITEM1", POSTBOX_PAGE_HISTORY_ITEM1 );
            m_pItem[2] = CreateItem ( "POSTBOX_PAGE_SEND_HISTORY_ITEM2", POSTBOX_PAGE_HISTORY_ITEM2 );
            m_pItem[3] = CreateItem ( "POSTBOX_PAGE_SEND_HISTORY_ITEM3", POSTBOX_PAGE_HISTORY_ITEM3 );
            m_pItem[4] = CreateItem ( "POSTBOX_PAGE_SEND_HISTORY_ITEM4", POSTBOX_PAGE_HISTORY_ITEM4 );
            break;
    }
}

void CPostBoxHistoryList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	UpdateItem ();

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxHistoryList::UpdateItem ( const BOOL bForcing )
{
    switch ( m_historyType )
    {
        case POSTBOX_HISTORY_ALL:
            UpdateItemWithHistoryInfoSet( m_pGaeaClient->GetPostClient()->GetHistoryInfoSet(), bForcing );
            break;

        case POSTBOX_HISTORY_SEND:
            UpdateItemWithHistoryInfoSet( m_pGaeaClient->GetPostClient()->GetSendHistoryInfoSet(), bForcing );
            break;

        case POSTBOX_HISTORY_RECV:
            UpdateItemWithHistoryInfoSet( m_pGaeaClient->GetPostClient()->GetRecvHistoryInfoSet(), bForcing );
            break;
    }
}

void CPostBoxHistoryList::UpdateItemWithHistoryInfoSet ( GLPostClient::SPOSTHISTORYINFOSET& historyInfoSet, const BOOL bForcing )
{
    GLPostClient::SPOSTHISTORYVEC& RcvHistoryVec = historyInfoSet.HistoryVec;

	// Note : 현황은 가상현황까지 포함하기 때문에 최대치 이상이 될수있다.
	//
	DWORD dwRealMaxHistory = historyInfoSet.GetMaxHistory();
	DWORD dwMaxHistory	   = dwRealMaxHistory;
	DWORD dwMaxPage		   = 0;

	if ( dwMaxHistory == 0 )
	{
		dwMaxPage = 1;
	}
	else
	{
		// Note : 최대치를 넘으면 자른다
		//
		if ( historyInfoSet.dwStateHowManyGet <= dwMaxHistory )
		{
			dwMaxHistory = historyInfoSet.dwStateHowManyGet;
		}

		dwMaxPage = ( (dwMaxHistory-1) /MAX_ITEM) + 1;
	}

	DWORD dwBegin   = (m_dwPage*MAX_ITEM);
	DWORD dwSkip    = 0;
	DWORD dwVirtual = 0;

	// Note : 최대 페이지 세팅
	m_dwPageMax = dwMaxPage;

	// Note : 최대치보다 넘어선다면 초기화
	if ( dwBegin >= dwMaxHistory )
	{
		dwBegin = 0;
	}

	// Note : 진짜 최대 갯수가 최대치를 초과하면
	//		  가상 우편의 갯수를 구한다.
	if ( historyInfoSet.dwStateHowManyGet < dwRealMaxHistory )
	{
		dwVirtual += (dwRealMaxHistory - historyInfoSet.dwStateHowManyGet);
	}

	for ( DWORD i=0; i<MAX_ITEM; ++i )
	{
		if ( !m_pItem[i] )
		{
			continue;
		}

		if ( dwMaxHistory < (dwBegin + i + 1 + dwSkip) )
		{
			m_pItem[i]->SetHistory ( NULL );
			continue;
		}

		DWORD dwIndex = (DWORD)(dwMaxHistory - dwBegin - i - 1 - dwSkip);
		m_pItem[i]->SetHistory ( RcvHistoryVec[dwIndex + dwVirtual], bForcing );
	}
}

void CPostBoxHistoryList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxHistoryList::PageNext ()
{
	if ( m_dwPageMax <= m_dwPage+1 )
	{
		return;
	}

	++m_dwPage;

	UpdateItem ();
}

void CPostBoxHistoryList::PagePrev ()
{
	if ( m_dwPage == 0 )
	{
		return;
	}

	--m_dwPage;

	UpdateItem ();
}

CPostBoxHistoryItem* CPostBoxHistoryList::CreateItem ( const char* szControl, UIGUID ControlID )
{
	CPostBoxHistoryItem* pItem = new CPostBoxHistoryItem ( m_pGaeaClient, m_pEngineDevice );
	pItem->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItem->CreateSubControl ();
	RegisterControl ( pItem );

	return pItem;
}

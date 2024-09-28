#include "StdAfx.h"

#include "PostBoxReceiveList.h"
#include "PostBoxReceiveItem.h"

#include "../../../RanLogicClient/Post/GLPostClient.h"
#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxReceiveList::CPostBoxReceiveList(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_dwPage(0)
	, m_dwPageMax(0)
	, m_dwPostMax(0)
	, m_dwSelectIndex(SELECT_NULL)
{
	GASSERT ( (MAX_ITEM != 0) && "MAX_ITEM 이 0입니다." );

	for ( UINT32 i=0; i<MAX_ITEM; ++i )
	{
		m_pItem[i] = 0;
	}
}

CPostBoxReceiveList::~CPostBoxReceiveList()
{
	// Blank
}

void CPostBoxReceiveList::CreateSubControl()
{
	m_pItem[0] = CreateItem ( "POSTBOX_PAGE_RECEIVE_ITEM0", POSTBOX_PAGE_RECEIVE_ITEM0 );
	m_pItem[1] = CreateItem ( "POSTBOX_PAGE_RECEIVE_ITEM1", POSTBOX_PAGE_RECEIVE_ITEM1 );
	m_pItem[2] = CreateItem ( "POSTBOX_PAGE_RECEIVE_ITEM2", POSTBOX_PAGE_RECEIVE_ITEM2 );
	m_pItem[3] = CreateItem ( "POSTBOX_PAGE_RECEIVE_ITEM3", POSTBOX_PAGE_RECEIVE_ITEM3 );
	m_pItem[4] = CreateItem ( "POSTBOX_PAGE_RECEIVE_ITEM4", POSTBOX_PAGE_RECEIVE_ITEM4 );
}

void CPostBoxReceiveList::OpenSubControl ()
{
	// Note : 본래의 아이템을 Unselect 한다.
	m_dwSelectIndex = SELECT_NULL;
}

void CPostBoxReceiveList::CloseSubControl ()
{
	// Note : 본래의 아이템을 Unselect 한다.
	m_dwSelectIndex = SELECT_NULL;
}

void CPostBoxReceiveList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	UpdateItem ();

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxReceiveList::UpdateItem ( const BOOL bForcing )
{
	GLPostClient::SPOSTRCVVEC& RcvPostVec = m_pGaeaClient->GetPostClient()->GetReceiveVec();

	DWORD dwMaxPost	= m_pGaeaClient->GetPostClient()->GetMaxPost();
	DWORD dwMaxPage	= 0;

	if ( dwMaxPost == 0 )
	{
		dwMaxPage = 1;
	}
	else
	{
		dwMaxPage = ( (dwMaxPost-1) /MAX_ITEM) + 1;
	}

	DWORD dwBegin = (m_dwPage*MAX_ITEM);
	DWORD dwSkip  = 0;

	// Note : 우편 최대 갯수
	m_dwPostMax = dwMaxPost;

	// Note : 최대 페이지 세팅
	m_dwPageMax = dwMaxPage;

	// Note : 최대치보다 넘어선다면 줄인다.
	while ( dwBegin >= dwMaxPost )
	{
		if ( m_dwPage )
		{
			m_dwPage--;
		}

		if ( m_dwPage == 0 )
		{
			dwBegin = 0;
			break;
		}

		dwBegin = (m_dwPage*MAX_ITEM);
	}

	for ( DWORD i=0; i<MAX_ITEM; ++i )
	{
		if ( !m_pItem[i] )
		{
			continue;
		}

		while (1)
		{
			if ( dwMaxPost < (dwBegin + i + 1 + dwSkip) )
			{
				m_pItem[i]->SetPost ( NULL );
				break;
			}

			DWORD dwPostIndex = (DWORD)(dwMaxPost - dwBegin - i - 1 - dwSkip);

			// Note : 삭제 대기중인 우편이라면 스킵한다.
			if ( RcvPostVec[dwPostIndex] && RcvPostVec[dwPostIndex]->dwSTATE&SAPOSTCLIENT::EMSTATE_WAIT_DELETE )
			{
				++dwSkip;
				continue;
			}

			m_pItem[i]->SetPost ( RcvPostVec[dwPostIndex], bForcing );
			break;
		}
	}

	// Npte : 선택 상자
	DWORD dwItem = GetSelectedItem ();

	for ( DWORD i=0; i<MAX_ITEM; ++i )
	{
		if ( dwItem == i )
		{
			m_pItem[i]->SetSelected( TRUE );
		}
		else
		{
			m_pItem[i]->SetSelected( FALSE );
		}
	}
}

void CPostBoxReceiveList::UpdateItem ( SAPOSTCLIENT* pPostData )
{
	if ( !pPostData )
	{
		return;
	}

	for ( DWORD i=0; i<MAX_ITEM; ++i )
	{
		if ( !m_pItem[i] )
		{
			continue;
		}

		SAPOSTCLIENT* pPost = m_pItem[i]->GetPost();

		if ( pPost && pPost->m_llPostID == pPostData->m_llPostID )
		{
			m_pItem[i]->SetPost ( pPostData, TRUE );
			break;
		}
	}
}

void CPostBoxReceiveList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case POSTBOX_PAGE_RECEIVE_ITEM0:
	case POSTBOX_PAGE_RECEIVE_ITEM1:
	case POSTBOX_PAGE_RECEIVE_ITEM2:
	case POSTBOX_PAGE_RECEIVE_ITEM3:
	case POSTBOX_PAGE_RECEIVE_ITEM4:
		{
			if ( dwMsg&UIMSG_POSTBOX_RECEIVE_ITEM_READ )
			{
				DWORD dwSelectedItem = (ControlID - POSTBOX_PAGE_RECEIVE_ITEM_BEGIN_INDEX);
				SelectPost   ( dwSelectedItem );
				AddMessageEx ( UIMSG_POSTBOX_RECEIVE_LIST_READ );
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxReceiveList::SelectPost ( const DWORD dwItem )
{
	DWORD dwIndex = ItemToIndex( dwItem );

	// Note : 페이지가 같을경우
	if ( m_dwPage == IndexToPage(m_dwSelectIndex) )
	{
		DWORD dwItem = GetSelectedItem ();

		if ( dwItem != SELECT_NULL )
		{
			m_pItem[dwItem]->SetSelected( FALSE );
		}
	}

	m_dwSelectIndex = dwIndex;
}

const BOOL CPostBoxReceiveList::ReturnItem ( const DWORD dwItem )
{
	if ( dwItem == SELECT_NULL )
	{
		return FALSE;
	}

	if ( MAX_ITEM <= dwItem )
	{
		return FALSE;
	}

	if ( !m_pItem[dwItem] )
	{
		return FALSE;
	}

	SAPOSTCLIENT* pPost = m_pItem[dwItem]->GetPost();
	
	if (!m_pGaeaClient->GetPostClient()->ReqReturnPost(pPost))
	{
		return FALSE;
	}

	if ( m_dwSelectIndex == ItemToIndex( dwItem ) )
	{
		m_dwSelectIndex = SELECT_NULL;
	}

	UpdateItem ();

	return TRUE;
}

const BOOL CPostBoxReceiveList::DeleteItem ( const DWORD dwItem )
{
	if ( dwItem == SELECT_NULL )
	{
		return FALSE;
	}

	if ( MAX_ITEM <= dwItem )
	{
		return FALSE;
	}

	if ( !m_pItem[dwItem] )
	{
		return FALSE;
	}

	SAPOSTCLIENT* pPost = m_pItem[dwItem]->GetPost();

	if (!m_pGaeaClient->GetPostClient()->ReqDeletePost(pPost))
	{
		return FALSE;
	}

	if ( m_dwSelectIndex == ItemToIndex( dwItem ))
	{
		m_dwSelectIndex = SELECT_NULL;
	}

	UpdateItem ();

	return TRUE;
}

const BOOL CPostBoxReceiveList::ReturnIndex ( const DWORD dwIndex )
{
	if ( dwIndex == SELECT_NULL )
	{
		return FALSE;
	}

	GLPostClient::SPOSTRCVVEC& RcvPostVec = m_pGaeaClient->GetPostClient()->GetReceiveVec();
	DWORD dwMaxPost	= m_pGaeaClient->GetPostClient()->GetMaxPost();

	if ( dwMaxPost <= dwIndex )
	{
		return FALSE;
	}

	if (!m_pGaeaClient->GetPostClient()->ReqReturnPost(RcvPostVec[dwIndex]))
	{
		return FALSE;
	}

	if ( m_dwSelectIndex == dwIndex )
	{
		m_dwSelectIndex = SELECT_NULL;
	}

	UpdateItem ();

	return TRUE;
}

const BOOL CPostBoxReceiveList::DeleteIndex ( const DWORD dwIndex )
{
	if ( dwIndex == SELECT_NULL )
	{
		return FALSE;
	}

	GLPostClient::SPOSTRCVVEC& RcvPostVec = m_pGaeaClient->GetPostClient()->GetReceiveVec();
	DWORD dwMaxPost	= m_pGaeaClient->GetPostClient()->GetMaxPost();

	if ( dwMaxPost <= dwIndex )
	{
		return FALSE;
	}

	if (!m_pGaeaClient->GetPostClient()->ReqDeletePost(RcvPostVec[dwIndex]))
	{
		return FALSE;
	}

	if ( m_dwSelectIndex == dwIndex )
	{
		m_dwSelectIndex = SELECT_NULL;
	}

	UpdateItem ();

	return TRUE;
}

void CPostBoxReceiveList::PageNext ()
{
	if ( m_dwPageMax <= m_dwPage+1 )
	{
		return;
	}

	++m_dwPage;

	UpdateItem ();
}

void CPostBoxReceiveList::PagePrev ()
{
	if ( m_dwPage == 0 )
	{
		return;
	}

	--m_dwPage;

	UpdateItem ();
}

CPostBoxReceiveItem* CPostBoxReceiveList::CreateItem ( const char* szControl, UIGUID ControlID )
{
	CPostBoxReceiveItem* pItem = new CPostBoxReceiveItem(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pItem->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItem->CreateSubControl ();
	RegisterControl ( pItem );

	return pItem;
}

SAPOSTCLIENT* CPostBoxReceiveList::GetSelectedPost ()
{
	if ( m_dwSelectIndex == SELECT_NULL )
	{
		return NULL;
	}

	GLPostClient::SPOSTRCVVEC& RcvPostVec = m_pGaeaClient->GetPostClient()->GetReceiveVec();
	DWORD dwMaxPost	= m_pGaeaClient->GetPostClient()->GetMaxPost();

	if ( dwMaxPost <= m_dwSelectIndex )
	{
		return NULL;
	}

	return RcvPostVec [ m_dwSelectIndex ];
}

void CPostBoxReceiveList::GetCheckedItems ( DWORD* pInOutItemArray, DWORD& dwOutCount )
{
	dwOutCount = 0;

	if ( !pInOutItemArray )
	{
		return;
	}

	DWORD dwIndex = 0;

	for ( DWORD i=0; i<MAX_ITEM; ++i )
	{
		if ( m_pItem[i] )
		{
			if ( m_pItem[i]->IsChecked() )
			{
				pInOutItemArray[dwIndex++] = i;
			}
		}
	}

	dwOutCount = dwIndex;
}

const DWORD	CPostBoxReceiveList::GetSelectedItem ()
{
	if ( m_dwPostMax == 0 )
	{
		return SELECT_NULL;
	}

	if ( m_dwSelectIndex == SELECT_NULL )
	{
		return SELECT_NULL;
	}

	if ( m_dwPage != IndexToPage (m_dwSelectIndex) )
	{
		return SELECT_NULL;
	}

	return (m_dwPostMax - m_dwSelectIndex - 1)%MAX_ITEM;
}

const DWORD CPostBoxReceiveList::ItemToIndex ( const DWORD dwItem )
{
	if ( m_dwPostMax == 0 )
	{
		return SELECT_NULL;
	}

	return m_dwPostMax - (dwItem + (m_dwPage*MAX_ITEM) + 1);
}

const DWORD CPostBoxReceiveList::IndexToPage ( const DWORD dwIndex )
{
	if ( m_dwPostMax == 0 )
	{
		return 0;
	}

	return (m_dwPostMax - dwIndex - 1)/MAX_ITEM;
}

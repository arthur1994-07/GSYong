#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "SelectCharacterPage.h"
#include "SelectCharacterPageList.h"
#include "SelectCharacterPageListItem.h"
#include "./LockSeletCharacterPageListItem.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterPageList::CSelectCharacterPageList(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_nSelectNum( -1 )
    , m_nSelectPage( 0 )
    , m_pSelectCharInfo( NULL )
{
}

CSelectCharacterPageList::~CSelectCharacterPageList ()
{
}

void CSelectCharacterPageList::CreateSubControl ()
{
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "SELECT_CHAR_WINDOW_CHAR_LIST_BACK" );
		RegisterControl ( pBasicLineBox );		
	}

	CString strLineKeyword[ 7 ] = 
	{
		"SELECT_CHAR_WINDOW_LINE_0",
		"SELECT_CHAR_WINDOW_LINE_1",
		"SELECT_CHAR_WINDOW_LINE_2",
		"SELECT_CHAR_WINDOW_LINE_3",
		"SELECT_CHAR_WINDOW_LINE_4",
		"SELECT_CHAR_WINDOW_LINE_5",
		"SELECT_CHAR_WINDOW_LINE_6",
	};

	CString strMemberKeyword[ MAX_CHAR_LIST_NUM ] = 
	{
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_0",
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_1",
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_2",
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_3",
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_4",
		"SELECT_CHAR_WINDOW_CHAR_LIST_ITEM_5",
	};

	for( int i = 0; i < 7; i++ )
	{
		CreateControl( strLineKeyword[ i ].GetBuffer( 0 ) );
	}

	for ( int i = 0; i < MAX_CHAR_LIST_NUM; i++ )
	{
		CSelectCharacterPageListItem* pListItem = new CSelectCharacterPageListItem ( m_pInterface, m_pEngineDevice );
		pListItem->CreateSub ( this, strMemberKeyword[ i ].GetString (), UI_FLAG_DEFAULT );
		pListItem->CreateSubControl ();
		RegisterControl ( pListItem );
		m_pListItem[ i ] = pListItem;
		m_pListItem[ i ]->SetVisibleSingle( FALSE );

        CLockSelectCharacterPageListItem* pLockItem = new CLockSelectCharacterPageListItem( m_pInterface, m_pEngineDevice );
        pLockItem->CreateSub( this, strMemberKeyword[ i ].GetString (), UI_FLAG_DEFAULT );
        pLockItem->CreateSubControl();
        RegisterControl( pLockItem );
        m_pLockItem[ i ] = pLockItem;
        m_pLockItem[ i ]->SetVisibleSingle( FALSE );
	}

	for ( int i = 0; i < MAX_CHAR_LIST_NUM; i++ )
	{
		CUIControl* pItemFocus = new CUIControl(m_pEngineDevice);
		pItemFocus->CreateSub ( this, strMemberKeyword[ i ].GetString(), UI_FLAG_DEFAULT, SELECT_CHAR_FOCUS_0 + i );
		RegisterControl ( pItemFocus );
		m_pItemFocus[ i ] = pItemFocus;
		m_pItemFocus[ i ]->SetVisibleSingle( FALSE );
	}

	int nTotalLine = 6;

	//	스크롤바
	m_pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, SELECT_CHARACTER_SCROLLBAR );
	m_pScrollBar->CreateBaseScrollBar ( "SELECT_CHAR_WINDOW_SCROLLBAR" );
	m_pScrollBar->GetThumbFrame()->SetState ( 2, nTotalLine );
	RegisterControl ( m_pScrollBar );
	m_pScrollBar->SetVisibleSingle ( FALSE );
}

CUIControl*	CSelectCharacterPageList::CreateControl( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


void CSelectCharacterPageList::SetSelectCharIndex( DWORD dwCharSelect, INT nCharStart )
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const int nChaSNum = pGlobalStage->GetLobyStage()->GetChaSNum ();
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nTotalLine = ( nChaSNum / MAX_SELECTCHAR ) + 1;
		const FLOAT nPercent = ( ( FLOAT ) nCharStart + 1 ) / ( FLOAT ) nTotalLine;
	
		pThumbFrame->SetPercent( nPercent );
	}

	SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( ( nCharStart * MAX_SELECTCHAR ) + dwCharSelect );
	if( !pCharInfo )
		return;		
	m_pSelectCharInfo = pCharInfo;	
}

void CSelectCharacterPageList::ResetAll()
{
	m_pSelectCharInfo = NULL;
}

void CSelectCharacterPageList::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SELECT_CHAR_FOCUS_0:
	case SELECT_CHAR_FOCUS_1:
	case SELECT_CHAR_FOCUS_2:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				if( m_nSelectNum != ControlID - SELECT_CHAR_FOCUS_0 )
					return;
				
                pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE;
				
				GLMSG::SNETLOBBY_REQ_GAME_JOIN NetMsgReq;
				m_pInterface->MsgProcess ( &NetMsgReq );
				return;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_nSelectNum = ControlID - SELECT_CHAR_FOCUS_0;
				INT nIndex = ( m_nSelectPage * MAX_SELECTCHAR ) + m_nSelectNum;

				pGlobalStage->GetLobyStage()->ShiftCharIndex( m_nSelectPage );
				SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( nIndex );
				if( !pCharInfo )
					return;

				m_pSelectCharInfo = pCharInfo;
				pGlobalStage->GetGameStage()->SetSelectChar( pCharInfo );
				pGlobalStage->GetLobyStage()->SelectCharacter( m_nSelectNum );				
			}
		}
		break;

	case SELECT_CHAR_FOCUS_3:
	case SELECT_CHAR_FOCUS_4:
	case SELECT_CHAR_FOCUS_5:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				if( m_nSelectNum != ControlID - SELECT_CHAR_FOCUS_0 )
					return;
                
				pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE;

				GLMSG::SNETLOBBY_REQ_GAME_JOIN NetMsgReq;
				m_pInterface->MsgProcess ( &NetMsgReq );
				return;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_nSelectNum = ControlID - SELECT_CHAR_FOCUS_0;
				INT nIndex = ( m_nSelectPage * MAX_SELECTCHAR ) + m_nSelectNum;
				
				pGlobalStage->GetLobyStage()->ShiftCharIndex( m_nSelectPage + 1 );
				SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( nIndex );
				if( !pCharInfo )
					return;				

				m_pSelectCharInfo = pCharInfo;
				pGlobalStage->GetGameStage()->SetSelectChar( pCharInfo );
				pGlobalStage->GetLobyStage()->SelectCharacter( m_nSelectNum - SELECT_CHAR_FOCUS_2 );
			}
		}
		break;

	}
}

void CSelectCharacterPageList::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	for ( int i = 0; i < MAX_CHAR_LIST_NUM; i++ )
	{
		m_pItemFocus[ i ]->SetVisibleSingle( TRUE );
		m_pListItem[ i ]->SetVisibleSingle( FALSE );
	}

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const int nChaSNum = pGlobalStage->GetLobyStage()->GetChaSNum ();
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();

		const int nTotalLine = ( nChaSNum / MAX_SELECTCHAR ) + 1;
		const int nLinePerOneView = 2;
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );		
		if ( nLinePerOneView < nTotalLine )
		{
			m_pScrollBar->SetVisibleSingle( TRUE );
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_nSelectPage = nPos;
		}
		else 
		{
			m_pScrollBar->SetVisibleSingle( FALSE );
		}
	}

    for (int i = 0; i < MAX_CHAR_LIST_NUM; i++)
	{
		SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetCharInfo( ( m_nSelectPage * MAX_SELECTCHAR ) + i );
		if( !pCharInfo )
		{
			m_pListItem[ i ]->ResetData();
			m_pItemFocus[ i ]->SetVisibleSingle( FALSE );
			continue;
		}
		m_pListItem[ i ]->SetVisibleSingle( TRUE );
		m_pListItem[ i ]->SetListItemInfo( pCharInfo, pCharInfo == m_pSelectCharInfo  ? TRUE : FALSE );

        // Note : 잠금 캐릭터에 잠금 이미지 출력.
        if ( pCharInfo->IsLock() )
        {
            m_pLockItem[ i ]->SetVisibleSingle( TRUE );
        }
        else
        {
            m_pLockItem[ i ]->SetVisibleSingle( FALSE );
        }
	}
}
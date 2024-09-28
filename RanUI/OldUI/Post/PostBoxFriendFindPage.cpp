#include "StdAfx.h"

#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowObject.h"
#include "PostBoxFriendFindPage.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../RanLogic/Club/GLClub.h"

#include "../../../RanLogicClient/Club/GLClubClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPostBoxFriendFindPage::CPostBoxFriendFindPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CPostBoxPage(pGaeaClient, pInterface, pEngineDevice)
    , m_pButton_Ok(NULL)
    , m_pButton_Close(NULL)
    , m_pComboBox_Friend(NULL)
    , m_pComboBox_Club(NULL)
    , m_SelectName("")
    , m_SelectIndex(-1)
{
}

CPostBoxFriendFindPage::~CPostBoxFriendFindPage ()
{
}

void CPostBoxFriendFindPage::CreateSubControl()
{
	// Note : 탭 버튼
	{ 
		CTapSelector::CreateTapButton ( this, "BASIC_TEXT_BUTTON19", "POSTBOX_PAGE_FRIENDFIND_TAP_FRIEND", (char*)ID2GAMEWORD( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 0 ),
			CBasicTextButton::SIZE19, POSTBOX_PAGE_FRIENDFIND_TAP_FRIEND, m_pEngineDevice );

		CTapSelector::CreateTapButton ( this, "BASIC_TEXT_BUTTON19", "POSTBOX_PAGE_FRIENDFIND_TAP_CLUB",   (char*)ID2GAMEWORD( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 1 ),
			CBasicTextButton::SIZE19, POSTBOX_PAGE_FRIENDFIND_TAP_CLUB, m_pEngineDevice );

		CTapSelector::TapSelect ( TAP_DEFAULT );
	}

	// Note : 일반 버튼
	{
		CBasicTextButton* pButton;

		m_pButton_Ok = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_FRIENDFIND_BUTTON_OK );
		pButton->CreateBaseButton( "POSTBOX_PAGE_FRIENDFIND_BUTTON_OK", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 2 ) );
		RegisterControl( pButton );

		m_pButton_Close = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE );
		pButton->CreateBaseButton( "POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "POSTBOX_BUTTON", 3 ) );
		RegisterControl( pButton );
	}

	// Note : 콤보박스
	{
		// Note : 콤보박스 배경
		{
			CreateBackgroundControl ( "POSTBOX_PAGE_FRIENDFIND_COMBOBOX_BACKGROUND" );
		}

		CSmartComboBoxRollOver* pComboBox;

		m_pComboBox_Friend = pComboBox = new CSmartComboBoxRollOver(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_FRIENDFIND_COMBOBOX_FRIEND );
		pComboBox->CreateSmartComboBoxRollOver( "POSTBOX_PAGE_FRIENDFIND_COMBOBOX_FRIEND", "POSTBOX_PAGE_FRIENDFIND_LINEBOX_TEXINFO" );
		RegisterControl ( pComboBox );

		m_pComboBox_Club = pComboBox = new CSmartComboBoxRollOver(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, POSTBOX_PAGE_FRIENDFIND_COMBOBOX_CLUB );
		pComboBox->CreateSmartComboBoxRollOver( "POSTBOX_PAGE_FRIENDFIND_COMBOBOX_CLUB", "POSTBOX_PAGE_FRIENDFIND_LINEBOX_TEXINFO" );
		RegisterControl ( pComboBox );

		CTapSelector::TapSelect( TAP_DEFAULT );
	}
}

void CPostBoxFriendFindPage::OpenSubControl ()
{
	if ( m_pComboBox_Friend && m_pComboBox_Club )
	{
		LoadFriendList();
		LoadClubList();

		CTapSelector::TapSelect( TAP_DEFAULT );
		UIGUID cID = CTapSelector::GetTapID();

		switch ( CTapSelector::GetTapID() )
		{
		case TAP_FRIEND:
			{
				m_pComboBox_Friend	->SetVisibleSingle( TRUE  );
				m_pComboBox_Club	->SetVisibleSingle( FALSE );
			}
			break;
		case TAP_CLUB:
			{
				m_pComboBox_Friend	->SetVisibleSingle( FALSE );
				m_pComboBox_Club	->SetVisibleSingle( TRUE  );
			}
			break;
		};
	}

	SetEnableOkButton ( FALSE );
}

void CPostBoxFriendFindPage::LoadFriendList()
{
	if ( !m_pComboBox_Friend )
	{
		return;
	}

	//	친구 목록을 갱신한다.
	m_pComboBox_Friend->ClearText();

	const friendlogic::FRIENDMAP& FriendMap = m_pGaeaClient->GetFriendClient()->GetList();
    for (friendlogic::FRIENDMAP_CITER iter=FriendMap.begin(); iter != FriendMap.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if (sFriend->IsBLOCK())
            continue;

		int nIndex = m_pComboBox_Friend->AddText(sFriend->m_ChaName.c_str(), false, false, cCOLOR_TEXT_DEFAULT);
        if ( nIndex < m_pComboBox_Friend->GetCount() )
		    m_pComboBox_Friend->SetTextData( nIndex, DATA_DEFAULT );
        else
        {
            std::string strError = sc::string::format( "void CPostBoxFriendFindPage::LoadFriendList() index error. %1%, %2%",
                sFriend->m_ChaName, nIndex );
            sc::writeLogError( strError );
        }
	}
}

void CPostBoxFriendFindPage::LoadClubList ()
{
	if ( !m_pComboBox_Club )
	{
		return;
	}

	//	클럽 목록을 갱신한다.
	m_pComboBox_Club->ClearText();

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	if (!pMyClub->IsValid())
        return;

	CLUBMEMBERS& mapMembers = pMyClub->GetMembers();
	CLUBMEMBERS_ITER iter	  = mapMembers.begin ();
	CLUBMEMBERS_ITER iter_end = mapMembers.end ();

	// Note : Online 유저
	int nIndex = 0;

	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;

		if ( sClubMember.m_Online )
		{
			m_pComboBox_Club->AddText		( sClubMember.m_ChaName.c_str(), false, false, cCOLOR_TEXT_DEFAULT );
			m_pComboBox_Club->SetTextData	( nIndex, DATA_DEFAULT );
			++nIndex;
		}

	}

	// Note : Offline 유저
	iter = mapMembers.begin ();

	for ( ; iter != iter_end; ++iter )
	{
		GLCLUBMEMBER& sClubMember = (*iter).second;

		if ( !sClubMember.m_Online )
		{
			m_pComboBox_Club->AddText		( sClubMember.m_ChaName.c_str(), false, false, cCOLOR_TEXT_OFFLINE );
			m_pComboBox_Club->SetTextData	( nIndex, DATA_OFFLINE );
			++nIndex;
		}
	}

}

void CPostBoxFriendFindPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CPostBoxPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPostBoxFriendFindPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	switch ( ControlID )
	{
	case POSTBOX_PAGE_FRIENDFIND_TAP_FRIEND :
	case POSTBOX_PAGE_FRIENDFIND_TAP_CLUB   :
		{
			CTapSelector::EMTAPSELECTORRESULT emResult = CTapSelector::EMTAPSELECTORRESULT_NONE;
			CTapSelector::TapSelectorUIMessage( ControlID, dwMsg, emResult );

			if ( emResult == CTapSelector::EMTAPSELECTORRESULT_SELECT )
			{
				CSmartComboBoxRollOver* pPreComboBox = NULL;

				switch ( CTapSelector::GetTapID() )
				{
				case TAP_FRIEND:
					{
						m_pComboBox_Friend	->SetVisibleSingle( TRUE  );
						m_pComboBox_Club	->SetVisibleSingle( FALSE );

						pPreComboBox = m_pComboBox_Club;
					}
					break;
				case TAP_CLUB:
					{
						m_pComboBox_Friend	->SetVisibleSingle( FALSE );
						m_pComboBox_Club	->SetVisibleSingle( TRUE  );

						pPreComboBox = m_pComboBox_Friend;
					}
					break;
				};

				// Note : 이전 콤보박스의 색깔을 되돌림.
				if ( pPreComboBox )
				{
					if ( m_SelectIndex != -1 )
					{
						DWORD dwDATA = pPreComboBox->GetTextData( m_SelectIndex );

						switch ( dwDATA )
						{
						case DATA_DEFAULT:
							{
								pPreComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_DEFAULT );
							}
							break;

						case DATA_OFFLINE:
							{
								pPreComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_OFFLINE );
							}
							break;
						};

						m_SelectIndex = -1;

						SetEnableOkButton ( FALSE );
					}

				}
			}
		}
		break;

	case POSTBOX_PAGE_FRIENDFIND_COMBOBOX_CLUB	 :
	case POSTBOX_PAGE_FRIENDFIND_COMBOBOX_FRIEND :
		{
			CSmartComboBoxRollOver* pComboBox = NULL;

			switch ( ControlID )
			{
			case POSTBOX_PAGE_FRIENDFIND_COMBOBOX_FRIEND: pComboBox = m_pComboBox_Friend; break;
			case POSTBOX_PAGE_FRIENDFIND_COMBOBOX_CLUB	: pComboBox = m_pComboBox_Club;	  break;
			}

			if ( !pComboBox )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = pComboBox->GetSelectIndex ();
				if ( nIndex < 0 )
				{
					if ( m_SelectIndex != -1 )
					{
						DWORD dwDATA = pComboBox->GetTextData( m_SelectIndex );

						switch ( dwDATA )
						{
						case DATA_DEFAULT:
							{
								pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_DEFAULT );
							}
							break;

						case DATA_OFFLINE:
							{
								pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_OFFLINE );
							}
							break;
						};

						m_SelectIndex = -1;
					}

					SetEnableOkButton ( FALSE );
					break;
				}

				if ( m_SelectIndex != -1 )
				{
					DWORD dwDATA = pComboBox->GetTextData( m_SelectIndex );

					switch ( dwDATA )
					{
					case DATA_DEFAULT:
						{
							pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_DEFAULT );
						}
						break;

					case DATA_OFFLINE:
						{
							pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_OFFLINE );
						}
						break;
					};

					m_SelectIndex = -1;
				}

				pComboBox->SetTextColor( nIndex, cCOLOR_TEXT_SELECT  );

				m_SelectName  = pComboBox->GetSelectText( nIndex );
				m_SelectIndex = nIndex;
				SetEnableOkButton ( TRUE );

				// Note : 더블클릭
				if ( dwMsg&UIMSG_LB_DUP )
				{
					RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_FRIENDFIND_SELECT_TO_SEND );
					AddMessageEx ( UIMSG_POSTBOX_PAGE_CLOSE );
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
			{
				if ( m_SelectIndex != -1 )
				{
					DWORD dwDATA = pComboBox->GetTextData( m_SelectIndex );

					switch ( dwDATA )
					{
					case DATA_DEFAULT:
						{
							pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_DEFAULT );
						}
						break;

					case DATA_OFFLINE:
						{
							pComboBox->SetTextColor( m_SelectIndex, cCOLOR_TEXT_OFFLINE );
						}
						break;
					};

					m_SelectIndex = -1;
				}


				SetEnableOkButton ( FALSE );
			}
		}
		break;

	case POSTBOX_PAGE_FRIENDFIND_BUTTON_OK :
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				RequestPush( CPostBoxWindow::POSTBOX_PAGE_REQ_FRIENDFIND_SELECT_TO_SEND );
				AddMessageEx ( UIMSG_POSTBOX_PAGE_CLOSE );
			}
		}
		break;

	case POSTBOX_PAGE_FRIENDFIND_BUTTON_CLOSE :
		{
			if ( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				AddMessageEx ( UIMSG_POSTBOX_PAGE_CLOSE );
			}
		}
		break;
	}

	CPostBoxPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CPostBoxFriendFindPage::SetEnableOkButton ( const BOOL bEnable )
{
	if ( bEnable )
	{
		m_pButton_Ok->SetDiffuse( cCOLOR_BUTTON_ENABLE );
		m_pButton_Ok->SetNoMessage( FALSE );
		m_pButton_Ok->SetNoUpdate ( FALSE );
	}
	else
	{
		m_pButton_Ok->SetDiffuse( cCOLOR_BUTTON_DISABLE );
		m_pButton_Ok->SetNoMessage( TRUE );
		m_pButton_Ok->SetNoUpdate ( TRUE );
	}
}

#include "StdAfx.h"
#include "RandPassKeyboard.h"

#include "../Util/ModalWindow.h" // 2차비번
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "LoginPage.h"
#include "SecPassCheckPage.h"
#include "SecPassSetPage.h"
#include "../../OuterInterface.h"

#include "../../../SigmaCore/Log/LogMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRandPassKeyboard::CRandPassKeyboard(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper( pEngineDevice )
, m_pInterface(pInterface)
, m_pModalWindow(NULL) // 2차비번
{
	memset( m_pNumImage, 0, sizeof(m_pNumImage) );
}

// 2차비번
CRandPassKeyboard::CRandPassKeyboard(CModalWindow* pModalWindow, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper( pEngineDevice )
, m_pInterface(NULL)
, m_pModalWindow(pModalWindow)
{
	memset( m_pNumImage, 0, sizeof(m_pNumImage) );
}
///////////////////////////////////////////////////

CRandPassKeyboard::~CRandPassKeyboard()
{
}

void CRandPassKeyboard::CreateSubControl ()
{
	CreateLineBox( "SECOND_PASSWORD_RANDOM_BACKGROUND_POS", "BLACK_LINE_DARKGRAY_BACKGROUND" );

	// 버튼
	{
		// 숫자 이미지 없는 버튼
		// 위치 랜덤
		{
			const float eventPressdTime = 0.05f;

			RnButton::CreateArg arg;
			arg.defaultTextureKey = "SECOND_PASSWORD_RANDOM_BASIC_BUTTON";					// 기본 이미지
			arg.mouseOverTextureKey = "SECOND_PASSWORD_RANDOM_MOUSE_OVER_EVENT_BUTTON";		// 마우스 오버 이미지
			arg.mouseClickTextureKey = "SECOND_PASSWORD_RANDOM_MOUSE_DOWN_EVENT_BUTTON";	// 마우스 클릭 이미지

			for( int i = 0; i < KEY_BUTTON_SIZE; i++ )
			{
				RnButton* pButton = new RnButton( m_pEngineDevice );

				if( pButton )
				{
					pButton->CreateSub( this, sc::string::format( "SECOND_PASSWORD_RANDOM_BUTTON_%d", i ).c_str(), UI_FLAG_DEFAULT, KEY_BUTTON_0 + i );
					pButton->CreateSubControl( arg );
					pButton->SetPressedEventTime( eventPressdTime );
					RegisterControl( pButton );
				}
			}
		}

		// 백스페이스키
		// 위치 고정
		{
			const float eventPressdTime = 0.05f;

			RnButton::CreateArg arg;
			arg.defaultTextureKey = "SECOND_PASSWORD_RANDOM_BASIC_BUTTON";					// 기본 이미지
			arg.mouseOverTextureKey = "SECOND_PASSWORD_RANDOM_MOUSE_OVER_EVENT_BUTTON";		// 마우스 오버 이미지
			arg.mouseClickTextureKey = "SECOND_PASSWORD_RANDOM_MOUSE_DOWN_EVENT_BUTTON";	// 마우스 클릭 이미지

			RnButton* pButton = new RnButton( m_pEngineDevice );

			if( pButton )
			{
				pButton->CreateSub( this, "SECOND_PASSWORD_RANDOM_BUTTON_BACKSPACE_KEY", UI_FLAG_DEFAULT, KEY_BUTTON_BACKSPACE_KEY );
				pButton->CreateSubControl( arg );
				pButton->SetPressedEventTime( eventPressdTime );
				RegisterControl( pButton );
			}
		}
	}

	// 이미지
	{
		// 숫자 + 숫자 이미지 없는 ( 위치 임시로 그냥 넣음 )
		{
			for( int i = 0; i < KEY_BUTTON_SIZE; i++ )
			{
				CSwapImage* pSwapImage = new CSwapImage( m_pEngineDevice );

				if( pSwapImage )
				{
					pSwapImage->CreateSub ( this, sc::string::format( "SECOND_PASSWORD_RANDOM_NUM_%d", i ).c_str(), UI_FLAG_DEFAULT );
					RegisterControl( pSwapImage );
					m_pNumImage[ i ] = pSwapImage;
				}
			}
		}

		// ← ( 백스페이스키 ) 이미지
		{
			CSwapImage* pSwapImage = new CSwapImage( m_pEngineDevice );

			if( pSwapImage )
			{
				pSwapImage->CreateSub( this, "SECOND_PASSWORD_RANDOM_BACKSPACE_KEY", UI_FLAG_DEFAULT );
				RegisterControl( pSwapImage );
			}
		}
	}
}

void CRandPassKeyboard::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CRandPassKeyboard::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
		SetNumberImage();
}

void CRandPassKeyboard::SetNumberImage()
{
	m_vecNum.clear();

	// 0 ~ 9 + 숫자 이미지 없는 빈 공백 총 11개
	// ECOND_PASSWORD_RANDOM_NUM_10 숫자 이미지 없는 빈 공백
	// 빈 공백은 키 입력만 안되면은 상관없으므로 이미지 효과 주기로 하였음
	static const std::string strPassRandNum[ KEY_BUTTON_SIZE ] = 
	{
		"SECOND_PASSWORD_RANDOM_NUM_0",
		"SECOND_PASSWORD_RANDOM_NUM_1",
		"SECOND_PASSWORD_RANDOM_NUM_2",
		"SECOND_PASSWORD_RANDOM_NUM_3",
		"SECOND_PASSWORD_RANDOM_NUM_4",
		"SECOND_PASSWORD_RANDOM_NUM_5",
		"SECOND_PASSWORD_RANDOM_NUM_6",
		"SECOND_PASSWORD_RANDOM_NUM_7",
		"SECOND_PASSWORD_RANDOM_NUM_8",
		"SECOND_PASSWORD_RANDOM_NUM_9",
		"SECOND_PASSWORD_RANDOM_NUM_10",
	};

	// 숫자 세팅 셔플;
	for( unsigned int i = 0; i < KEY_BUTTON_SIZE; ++i )
	{
		m_vecNum.push_back( i );
	}
	std::random_shuffle( m_vecNum.begin(), m_vecNum.end() );

	// 셔플 된 순서에 따라 구성;
	for( unsigned int i = 0; i < KEY_BUTTON_SIZE; ++i )
	{
		int iIndex = m_vecNum[ i ];
		if ( iIndex >= 0 && iIndex < KEY_BUTTON_SIZE )
		{
			if ( m_pNumImage[ i ] )
			{
				m_pNumImage[ i ]->SetImage( strPassRandNum[ iIndex ].c_str() );
				m_pNumImage[ i ]->SetVisibleSingle( TRUE );
			}
		}
	}
}


void CRandPassKeyboard::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	// 숫자 이미지는 클릭 안하고 버튼 이미지를 클릭할 수 있게
	// 숫자 이미지를 클릭하면 클릭되는 영역이 작음
	// 숫자 이미지를 확대하면 이미지가 깨져서 버튼을 클릭

	if( ControlID >= KEY_BUTTON_0 && ControlID <= KEY_BUTTON_BACKSPACE_KEY )
	{
		TranslateKeyMsg( ControlID, dwMsg, TRUE );
	}
}

void CRandPassKeyboard::TranslateKeyMsg( UIGUID ControlID, DWORD dwMsg, BOOL bNumber )
{
	TCHAR Key = 0;

	if ( KEY_BUTTON_BACKSPACE_KEY != ControlID )
	{
		// 인덱스 획득 및 유효성 검사;
		int iIndex = ControlID - KEY_BUTTON_0;
		if ( iIndex < 0 || iIndex >= static_cast< int >( m_vecNum.size() ) )
			return;

		// 10이 들어갈 경우 공백으로 설정;
		if ( m_vecNum[ iIndex ] != 10 )
			Key = *sc::string::format( "%d", m_vecNum[ iIndex ] ).c_str();
	}

	if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
	{
		// 2차비번
		if(m_pModalWindow)
		{
			if ( m_pModalWindow && !m_pModalWindow->Is2ndPWDModal() )
			{
				CUIEditBox *pEditBox = m_pModalWindow->GetEditBox();

				if( !pEditBox ) return;

				CString strTemp;
				strTemp = pEditBox->GetEditString();

				// 백 스페이스키
				if ( strTemp.GetLength() && KEY_BUTTON_BACKSPACE_KEY == ControlID )
				{
					strTemp.Delete( strTemp.GetLength()-1, strTemp.GetLength() );
				}
				else
				{
					strTemp += Key;
				}

				pEditBox->SetEditString( strTemp );
			}
			else
			{
				CUIEditBoxMan* pEditBox = m_pModalWindow->GetEditBoxMan();

				if( !pEditBox ) return;

				CString strTemp;
				strTemp = pEditBox->GetEditString();

				// 백 스페이스키
				if ( strTemp.GetLength() && KEY_BUTTON_BACKSPACE_KEY == ControlID )
				{
					strTemp.Delete( strTemp.GetLength()-1, strTemp.GetLength() );
				}
				else
				{
					strTemp += Key;
				}

				pEditBox->SetEditString( strTemp );
			}

			return; 
		}
		////////////////////////////////

		if( m_pInterface->UiIsVisibleGroup( LOGIN_PAGE ) )
		{
			CLoginPage * pLoginPage = m_pInterface->GetLoginPage();
			if( !pLoginPage ) return;
			pLoginPage->GoThisTab();
			pLoginPage->SetCharToEditBox(Key);

		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
		{
			CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
			if( !pSecPassSetPage ) return;
			pSecPassSetPage->GoThisTab();
			pSecPassSetPage->SetCharToEditBox(Key);
		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
		{
			CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
			if( !pSecPassCheckPage ) return;
			pSecPassCheckPage->GoThisTab();
			pSecPassCheckPage->SetCharToEditBox(Key);
		}

		////////////////////
		if( m_pInterface->UiIsVisibleGroup( SECPASS_SETPAGE ) )
		{
			CSecPassSetPage * pSecPassSetPage = m_pInterface->GetSecPassSetPage();
			if( !pSecPassSetPage ) return;
			pSecPassSetPage->GoThisTab();
			pSecPassSetPage->SetCharToEditBox(Key);
		}
		else if( m_pInterface->UiIsVisibleGroup( SECPASS_CHECKPAGE ) )
		{
			CSecPassCheckPage * pSecPassCheckPage = m_pInterface->GetSecPassCheckPage();
			if( !pSecPassCheckPage ) return;
			pSecPassCheckPage->GoThisTab();
			pSecPassCheckPage->SetCharToEditBox(Key);
		}
	}
}
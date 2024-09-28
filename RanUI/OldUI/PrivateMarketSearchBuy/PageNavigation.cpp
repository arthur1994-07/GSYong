#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../Interface/BasicTextBoxExLink.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../Util/RnButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "Pagenavigation.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace
{
	const char* strTexLeft ="<";
	const char* strTexRight =">";
	const char* strTexLeftEnd ="<<";
	const char* strTexRightEnd =">>";
	const char* szButtonNormal = "RNBUTTON_DEFAULT_TEXTURE";
	const char* szButtonOver = "RNBUTTON_OVER_TEXTURE";
	const char* szButtonDown = "RNBUTTON_CLICK_TEXTURE";
}

Pagenavigation::Pagenavigation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_fWaiteTime(0)
, m_count(1)
, m_nIndex(0)
, m_nCountBox(0)
, m_fCountPage(0)
, m_pLeftTapButton(NULL)
, m_pRightTapButton(NULL)
, m_pLeftTapButtonEnd(NULL)
, m_pRightTapButtonEnd(NULL)
, m_IsClickEnevt(FALSE)
{

}

Pagenavigation::~Pagenavigation()
{

}

void Pagenavigation::CreateSubControl()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//Index 숫자 텍스트 박스
	for (int i = 0; i< MAX_PAGE; i++)
	{
		char* strPageNumber = NULL;
		//itoa( i + 1, strPageNumber, 10 );
		CBasicTextBoxExLink* pPageNumber =  new CBasicTextBoxExLink(m_pEngineDevice);
		pPageNumber->CreateSub( this, "PAGE_NAVIGATION_PAGE_NUMBER",UI_FLAG_DEFAULT, ID_INDEX_1 + i );
		pPageNumber->SetFont( pFont );
		pPageNumber->SetText( "1",NS_UITEXTCOLOR::WHITE );
		pPageNumber->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
		pPageNumber->SetSensitive ( true );
		pPageNumber->SetMouseOverColor(NS_UITEXTCOLOR::AQUA);
		pPageNumber->SetVisibleSingle(TRUE);
		pPageNumber->SetAutoChangeFontSize(TRUE);
		RegisterControl( pPageNumber );
		m_pPageNumber.push_back(pPageNumber);
	}
	m_pPageNumber[m_nIndex]->SetOneLineText( m_pPageNumber[m_nIndex]->GetText(0), NS_UITEXTCOLOR::ORANGE );
	m_StarBoxPox = m_pPageNumber[ 0 ]->GetLocalPos();
	
	CD3DFontPar* pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//인덱스 탭 버튼 왼쪽
	{
	RnButton* pLineBoxComboButton = NULL;
	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = szButtonNormal;
	arg.mouseOverTextureKey = szButtonOver;
	arg.mouseClickTextureKey = szButtonDown;
	//////////////////////////////////////////////////////////////////////////

	arg.pFont = pFont8;
	arg.dwColor = NS_UITEXTCOLOR::WHITE;
	arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
	arg.text = sc::string::format( "%s",strTexLeft);


	pLineBoxComboButton = new RnButton(m_pEngineDevice);
	pLineBoxComboButton->CreateSub(this, "PAGE_NAVIGATION_PAGE_NUMBER_DEFAULT_BUTTON_POS_LEFT", UI_FLAG_RIGHT,ID_INDEX_LEFT_BUTTON );
	pLineBoxComboButton->CreateSubControl(arg);
	RegisterControl(pLineBoxComboButton);
	m_pLeftTapButton = pLineBoxComboButton;
	}

	//인덱스 탭 버튼 오른쪽
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",strTexRight);


		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "PAGE_NAVIGATION_PAGE_NUMBER_DEFAULT_BUTTON_POS_RIGHT", UI_FLAG_RIGHT, ID_INDEX_RIGHT_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		RegisterControl(pLineBoxComboButton);
		m_pRightTapButton = pLineBoxComboButton;
	}

	pFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );
	//인덱스 탭 버튼 왼쪽으로 10페이지
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",strTexLeftEnd);


		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "PAGE_NAVIGATION_PAGE_NUMBER_DEFAULT_BUTTON_POS_LEFT", UI_FLAG_RIGHT, ID_INDEX_LEFT_JUMP_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		pLineBoxComboButton->SetVisibleSingle(FALSE);
		RegisterControl(pLineBoxComboButton);
		m_pLeftTapButtonEnd = pLineBoxComboButton;
	}

	//인덱스 탭 버튼 오른쪽으로 10페이지
	{
		RnButton* pLineBoxComboButton = NULL;
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = szButtonNormal;
		arg.mouseOverTextureKey = szButtonOver;
		arg.mouseClickTextureKey = szButtonDown;
		//////////////////////////////////////////////////////////////////////////

		arg.pFont = pFont8;
		arg.dwColor = NS_UITEXTCOLOR::WHITE;
		arg.nTextAlign = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
		arg.text = sc::string::format( "%s",strTexRightEnd);


		pLineBoxComboButton = new RnButton(m_pEngineDevice);
		pLineBoxComboButton->CreateSub(this, "PAGE_NAVIGATION_PAGE_NUMBER_DEFAULT_BUTTON_POS_RIGHT", UI_FLAG_RIGHT, ID_INDEX_RIGHT_JUMP_BUTTON );
		pLineBoxComboButton->CreateSubControl(arg);
		pLineBoxComboButton->SetVisibleSingle(FALSE);
		RegisterControl(pLineBoxComboButton);
		m_pRightTapButtonEnd = pLineBoxComboButton;
	}

	SortTextBox();
	SortTapButton();
}

void Pagenavigation::SortTextBox(int nIndex)
{
	if ( nIndex == 0 )
		nIndex = 1;

	if ( nIndex <=1 )
		m_nIndex = 0;

	m_nCountBox = nIndex;

	m_mapIndex.clear();

	for (int i = 0; i < nIndex; i++ )
	{
		std::string strUItext = sc::string::format( "%d",i + 1);
		m_mapIndex.insert(std::map<CString, int>::value_type(strUItext.c_str(), i));
	}

	if (nIndex>MAX_PAGE)
		nIndex = MAX_PAGE;

	UIRECT rcNewPos = m_pPageNumber[START_INDEX]->GetGlobalPos();
	UIRECT rcParentsPos  = GetGlobalPos();
	float fBoxGap = 1.0f;
	//페이지 넘버가 100 이상 즉 3자리수 이상일때 간격을 더 늘려준다
	if ( 1 + ( MAX_PAGE * m_fCountPage) > 100 )
	{
		fBoxGap = 1.3f;
	}
	else if ( 1 + ( MAX_PAGE * m_fCountPage) > 1000 )
	{
		fBoxGap = 1.5f;
	}

	for (int i = 0; i < MAX_PAGE; i ++)
	{
		UIRECT rcNewPos = m_pPageNumber[i]->GetLocalPos();
		m_pPageNumber[i]->SetLocalPos(UIRECT(rcNewPos.left, rcNewPos.top, m_StarBoxPox.sizeX * fBoxGap, rcNewPos.sizeY));
	}


	float fNewLeftPos = rcParentsPos.left + ( ( rcParentsPos.sizeX * 0.5f ) - ( ( rcNewPos.sizeX * nIndex  ) * 0.5f ) ) - 1;

	m_pPageNumber[START_INDEX]->SetGlobalPos(D3DXVECTOR2( fNewLeftPos, rcNewPos.top));
	rcNewPos = m_pPageNumber[START_INDEX]->GetGlobalPos();
	m_pPageNumber[START_INDEX]->SetVisibleSingle(TRUE);

	for(int i = 1; i< MAX_PAGE; i ++)
	{
		m_pPageNumber[i]->SetVisibleSingle(FALSE);
	}
	

	for ( int i = 0; i < nIndex; i ++)
	{
		std::string strUItext = sc::string::format( "%d",1 + ( MAX_PAGE * m_fCountPage));
		m_pPageNumber[START_INDEX]->SetText( strUItext.c_str(), NS_UITEXTCOLOR::WHITE );
		if ( i > 0 )
		{
			if ((1 + ( MAX_PAGE * m_fCountPage) > m_nCountBox) || (i + 1 + ( MAX_PAGE * m_fCountPage) > m_nCountBox))
				return;

			std::string strUItext = sc::string::format( "%d",1 + ( MAX_PAGE * m_fCountPage));
			m_pPageNumber[START_INDEX]->SetText( strUItext.c_str(), NS_UITEXTCOLOR::WHITE );
			strUItext = sc::string::format( "%d",i + 1 + ( MAX_PAGE * m_fCountPage));
			UIRECT rcFrontPos = m_pPageNumber[ i - 1 ]->GetGlobalPos();
			m_pPageNumber[ i ]->SetText( strUItext.c_str(), NS_UITEXTCOLOR::WHITE );
			m_pPageNumber[ i ]->SetGlobalPos(D3DXVECTOR2( rcFrontPos.right, rcNewPos.top));
			m_pPageNumber[ i ]->SetAutoChangeFontSize(TRUE);
			m_pPageNumber[ i ]->SetVisibleSingle(TRUE);
		}
	}

}

void Pagenavigation::SortTapButton( int nIndex )
{
	if ( nIndex == 0 )
		nIndex = 1;

	if ( nIndex >= MAX_PAGE )
		nIndex = MAX_PAGE;

	UIRECT rcLeftPos = m_pPageNumber[ 0 ]->GetGlobalPos();
	UIRECT rcRightPos = m_pPageNumber[ nIndex - 1 ]->GetGlobalPos();

	m_pLeftTapButton->SetGlobalPos(D3DXVECTOR2(rcLeftPos.left - rcLeftPos.sizeX  - 5, rcLeftPos.top));

	if ( nIndex >= MAX_PAGE )
	{
		m_pRightTapButton->SetGlobalPos(D3DXVECTOR2(rcLeftPos.left + 5 + (rcLeftPos.sizeX * MAX_PAGE), rcLeftPos.top));

		UIRECT rcLeftPos = m_pLeftTapButton->GetGlobalPos();
		UIRECT rcRightPos = m_pRightTapButton->GetGlobalPos();
		m_pLeftTapButtonEnd->SetGlobalPos(D3DXVECTOR2(rcLeftPos.left - rcLeftPos.sizeX - 5, rcLeftPos.top));
		m_pRightTapButtonEnd->SetGlobalPos(D3DXVECTOR2(rcRightPos.right + 5 , rcRightPos.top));
		m_pLeftTapButtonEnd->SetVisibleSingle(TRUE);
		m_pRightTapButtonEnd->SetVisibleSingle(TRUE);
	}
	else
	{
		m_pRightTapButton->SetGlobalPos(D3DXVECTOR2(rcRightPos.right + 5, rcLeftPos.top));

		m_pLeftTapButtonEnd->SetVisibleSingle(FALSE);
		m_pRightTapButtonEnd->SetVisibleSingle(FALSE);
	}
}

void Pagenavigation::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void Pagenavigation::PagenavigationUpdate( int nIndex /*= 1 */ )
{
	SortTextBox( nIndex );
	SortTapButton( nIndex );
}


void Pagenavigation::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
	//SortTextBox( m_count );

	if ( ControlID >= ID_INDEX_START && ControlID <= ID_INDEX_END )
	{
		int nIndex = ControlID - ID_INDEX_START;
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{

			m_pPageNumber[nIndex]->SetOneLineText( m_pPageNumber[nIndex]->GetText(0), NS_UITEXTCOLOR::YELLOW );

			if ( CHECK_LB_UP_LIKE ( dwMsg ))
			{
				ClickIndex( ControlID );
				for (int i = 0; i< MAX_PAGE; i ++)
				{
					std::string strtemp(m_pPageNumber[i]->GetText(0));
					if (m_nIndex != i)
						m_pPageNumber[i]->SetOneLineText( strtemp.c_str(), NS_UITEXTCOLOR::WHITE );
				}
				m_IsClickEnevt = TRUE;
			}
		}
		else
		{
			m_pPageNumber[nIndex]->SetOneLineText( m_pPageNumber[nIndex]->GetText(0), NS_UITEXTCOLOR::WHITE );
		}
		m_pPageNumber[m_nIndex]->SetOneLineText( m_pPageNumber[m_nIndex]->GetText(0), NS_UITEXTCOLOR::ORANGE );
	}

	switch(ControlID)
	{
	case ID_INDEX_LEFT_BUTTON:
		{
			if(CHECK_MOUSEIN_LBUPLIKE ( dwMsg ))
			{

				if ( m_mapIndex[m_pPageNumber[m_nIndex]->GetText(0)] > START_INDEX &&  m_nIndex > START_INDEX )
				{
					m_nIndex--;
				}
				else
				{
					if ( m_nIndex == START_INDEX && m_mapIndex[m_pPageNumber[m_nIndex]->GetText(0)] > END_INDEX )
					{
						m_fCountPage--;
						m_nIndex = END_INDEX;
					}
					else
						return;
				}
				m_IsClickEnevt = TRUE;
			}
		}
		break;

	case ID_INDEX_RIGHT_BUTTON:
		{
			if(CHECK_MOUSEIN_LBUPLIKE ( dwMsg ))
			{
				if ( m_nIndex < END_INDEX && 
					(m_mapIndex[m_pPageNumber[m_nIndex]->GetText(0)] < m_nCountBox - 1))
				{
					m_nIndex++;
				}
				else
				{
					int nMapSize = static_cast<int>(m_mapIndex.size() - 1);
					int nIndex = m_mapIndex[m_pPageNumber[m_nIndex]->GetText(0)];
					if ( m_nIndex == END_INDEX && ( nIndex < nMapSize ) )
					{
						m_fCountPage++;
						m_nIndex = 0;
					}
					else
						return;
				}
				m_IsClickEnevt = TRUE;
			}
		}
		break;

	case ID_INDEX_LEFT_JUMP_BUTTON:
		{
			if(CHECK_MOUSEIN_LBUPLIKE ( dwMsg ))
			{
				if ( m_fCountPage > 0 )
				{
					m_fCountPage--;

					if ( m_nIndex < START_INDEX )
						m_nIndex = START_INDEX;
				}
				else
				{
					m_nIndex = START_INDEX;
				}
				m_IsClickEnevt = TRUE;
			}
		}
		break;

	case ID_INDEX_RIGHT_JUMP_BUTTON:
		{
			if(CHECK_MOUSEIN_LBUPLIKE ( dwMsg ))
			{
				int nMapSize = m_mapIndex.size() - 1;
				CString strIndex = m_pPageNumber[m_nIndex]->GetText(0);
				int nIndex = m_mapIndex[strIndex];
				int nTempIndex =  (nMapSize+MAX_PAGE)%MAX_PAGE;

				if ( nIndex < nMapSize - nTempIndex )
				{
					m_fCountPage++;

					if (nIndex + MAX_PAGE > nMapSize)
					{
						m_nIndex =  (nMapSize+MAX_PAGE)%MAX_PAGE;
					}
				}
				else
					m_nIndex =  (nMapSize+MAX_PAGE)%MAX_PAGE;
				
				m_IsClickEnevt = TRUE;
			}

		}
		break;
	}
}

void Pagenavigation::ClickIndex( UIGUID ControlID )
{
	m_nIndex = (ControlID - ID_INDEX_START);
}

int Pagenavigation::GetIndex()
{
	return m_nIndex;
}

int Pagenavigation::GetPageIndex()
{
	return m_nIndex + ( MAX_PAGE * static_cast<int>(m_fCountPage) );
}

BOOL Pagenavigation::IsClickEvent()
{
	if (m_IsClickEnevt)
	{
		m_IsClickEnevt = FALSE;
		return TRUE;
	}
	return FALSE;
}

void Pagenavigation::SetPage( int nPage )
{
	m_nIndex = nPage;
}

void Pagenavigation::SetCountPage( float fCountPage )
{
	m_fCountPage = fCountPage;
}



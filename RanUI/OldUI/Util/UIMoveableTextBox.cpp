#include "stdafx.h"

#include "./UIMoveableTextBox.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"

#include "../../../SigmaCore/DebugInclude.h"

CMoveableTextBox::CMoveableTextBox( CInnerInterface* pInterface
								   , EngineDeviceMan* pEngineDevice )
								   : CUIGroupHelper( pEngineDevice )
								   , m_pInterface( pInterface )
								   , m_pDescText( NULL )
								   , m_textColor( NS_UITEXTCOLOR::WHITE )
								   , m_bAniDesc( true )
								   , m_bOverTextLength( false )
								   , m_nAniDescCur( 0 )
								   , m_nAniDescMax( 0 )
								   , m_nStrCur( 0 )
								   , m_nStrMax( 0 )
								   , m_fAniTime( 0.f )
								   , m_fDESC_ANIMATION_SPEED_DELAY( 0.5f )
								   , m_moveableType( MOVE_LEFT_TYPE )
{

}

CMoveableTextBox::~CMoveableTextBox() { }

void CMoveableTextBox::CreateSub( CUIControl* pParent
								 , const char* szControlKeyword
								 , WORD wAlignFlag
								 , UIGUID WndID )
{
	CUIGroupHelper::CreateSub( pParent, szControlKeyword, wAlignFlag, WndID );
}

void CMoveableTextBox::CreateSubControl( const char* szControlKeyword
										, CD3DFontPar* pFont
										, int nAlign )
{
	{
		m_pDescText = new CBasicTextBox( m_pEngineDevice );
		m_pDescText->CreateSub( this, szControlKeyword, UI_FLAG_YSIZE );
		m_pDescText->SetFont( pFont );
		m_pDescText->SetTextAlign( nAlign );
		m_pDescText->AddText( "", NS_UITEXTCOLOR::WHITE );
		RegisterControl( m_pDescText );
	}
}

void CMoveableTextBox::SetVisibleSingle( BOOL bVisible )
{
	CUIGroupHelper::SetVisibleSingle( bVisible );
}

void CMoveableTextBox::Update( int x
							  , int y
							  , BYTE LB
							  , BYTE MB
							  , BYTE RB
							  , int nScroll
							  , float fElapsedTime
							  , BOOL bFirstControl )
{
	if( m_bAniDesc )
	{
		if ( m_fDESC_ANIMATION_SPEED_DELAY < m_fAniTime )
		{
			if( m_nAniDescCur < m_nAniDescMax )
			{
				if( !m_bOverTextLength )
				{
					m_nAniDescCur++;

					UpdateText();
				}
				else
				{
					const char* pDesc = m_strDesc.c_str();

					if( NS_UITEXTUTIL::IsDBCS( *(pDesc + m_nAniDescCur) ) )
					{
						m_nAniDescCur += 2;
					}
					else
					{
						m_nAniDescCur += 1;
					}

					m_pDescText->SetText(
						m_strDesc.c_str() + m_nAniDescCur,
						m_textColor,
						false );
				}

				m_fAniTime = 0.f;
			}
			else
			{
				if( m_nStrCur+1 < m_nStrMax )
				{
					UpdateTextString();
				}
				else
				{
					m_nAniDescCur = 0;
					m_nStrCur = 0;

					ResetTextAnimation();
				}

				m_fAniTime = 0.f;
			}
		}

		m_fAniTime += fElapsedTime;
	}
	else
	{
		if( m_nAniDescCur )
		{
			m_pDescText->SetText(
				m_strDesc.c_str(),
				m_textColor,
				false );
			m_nAniDescCur = 0;
		}

		m_fAniTime = 0.f;
	}

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CMoveableTextBox::TranslateUIMessage( UIGUID ControlID
										  , DWORD dwMsg )
{
	CUIGroupHelper::TranslateUIMessage( ControlID, dwMsg );
}

void CMoveableTextBox::SetText( const std::string& text )
{
	m_changeString = text;
}

void CMoveableTextBox::SetTextColor( const D3DCOLOR& textColor )
{
	m_textColor = textColor;
}

void CMoveableTextBox::SetTextSpeed( float textSpeed )
{
	m_fDESC_ANIMATION_SPEED_DELAY = textSpeed;
}

void CMoveableTextBox::SetMoveableType( EMOVEABLE_TEXT_TYPE type )
{
	m_moveableType = type;
}

void CMoveableTextBox::SetTextWithResetAnimation( const std::string& text )
{
	SetText( text );
	ResetTextAnimation();
}

void CMoveableTextBox::ResetTextAnimation()
{
	//m_nAniDescCur = 0;
	//m_nStrCur = 0;
	ChangeText();
	UpdateText();
}

void CMoveableTextBox::StartAnimation()
{
	m_bAniDesc = true;
}

void CMoveableTextBox::StopAnimation()
{
	m_bAniDesc = false;
}

void CMoveableTextBox::ChangeText()
{
	// 초기화 한다;
	m_originString = m_changeString;
	m_strDesc = m_changeString;
	m_nAniDescMax = 0;
	m_nStrMax = 0;
	m_nAniDescCur = 0;
	m_nStrCur = 0;

	{
		NS_UITEXTUTIL::ClearCRLFText ();
		NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF(
			m_strDesc.c_str(),
			m_pDescText->GetLocalPos().sizeX,
			m_pDescText->GetFont() );

		// 텍스트의 길이가 짧다면;
		if( 1 >= MLText.size() )
		{

			// UI의 크기만큼 텍스트를 늘린다;
			m_bOverTextLength = false;

			while( 1 >= MLText.size() )
			{
				if( m_moveableType == MOVE_LEFT_TYPE )
					m_strDesc = " " + m_strDesc;
				else if( m_moveableType == MOVE_RIGHT_TYPE )
					m_strDesc = m_strDesc + " ";

				NS_UITEXTUTIL::ClearCRLFText ();
				MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF(
					m_strDesc.c_str(),
					m_pDescText->GetLocalPos().sizeX,
					m_pDescText->GetFont() );
				m_nAniDescMax++;
			}

			// 텍스트가 UI의 크기보다 커진 사이즈가 되므로 1을 빼준다;
			m_nAniDescMax-=1;

			// 텍스트의 길이를 구한다;
			m_nStrMax = m_originString.length();
		}
		// 텍스트의 길이가 길다면;
		else
		{
			m_bOverTextLength = true;

			NS_UITEXTUTIL::MULTILINETEXT::iterator iter =
				MLText.begin() + 1;
			for ( ; iter != MLText.end(); ++iter )
			{
				NS_UITEXTUTIL::SSTRING& sString = (*iter);
				m_nAniDescMax += (UINT)sString.strLine.GetLength();
			}

			// 텍스트의 길이를 구한다;
			//m_nStrMax = m_strDesc.length();
			m_nStrMax = m_originString.length();
		}
	}

	// 텍스트를 세팅한다;
	m_pDescText->SetText( m_strDesc.c_str(), m_textColor, false ); 
}

void CMoveableTextBox::UpdateText()
{
	// Str Desc 초기화;
	m_strDesc = m_originString;

	// Str Desc 갱신;
	for( unsigned int i=0; i<m_nAniDescMax; ++i )
	{
		if( m_nAniDescCur > i )
		{
			if( m_moveableType == MOVE_LEFT_TYPE )
				m_strDesc = m_strDesc + " ";
			else if( m_moveableType == MOVE_RIGHT_TYPE )
				m_strDesc = " " + m_strDesc;
		}
		else
		{
			if( m_moveableType == MOVE_LEFT_TYPE )
				m_strDesc = " " + m_strDesc;
			else if( m_moveableType == MOVE_RIGHT_TYPE )
				m_strDesc = m_strDesc + " ";
		}
	}

	// 텍스트를 세팅한다;
	m_pDescText->SetText( m_strDesc.c_str(), m_textColor, false ); 
}

void CMoveableTextBox::UpdateTextString()
{
	int nDesc = 1;

	const char* pDesc = m_strDesc.c_str();
	if( NS_UITEXTUTIL::IsDBCS( *(pDesc + m_nAniDescCur) ) )
		nDesc = 2;
	else
		nDesc = 1;

	m_nStrCur+=nDesc;

	// 원본 텍스트의 길이를 이용하여 텍스트 내에서의;
	// 이동을 수행한다;
	if( m_moveableType == MOVE_LEFT_TYPE )
	{
		std::string tempStr1 = m_strDesc.substr( nDesc, m_strDesc.length()-nDesc );
		std::string tempStr2 = m_strDesc.substr( 0, nDesc );

		m_strDesc = tempStr1 + tempStr2;
	}
	else if( m_moveableType == MOVE_RIGHT_TYPE )
	{
		std::string tempStr1 = m_strDesc.substr( m_strDesc.length()-nDesc, m_strDesc.length() );
		std::string tempStr2 = m_strDesc.substr( 0, m_strDesc.length()-nDesc );

		m_strDesc = tempStr1 + tempStr2;
	}
	
	if( !m_bOverTextLength )
		m_pDescText->SetText( m_strDesc.c_str(), m_textColor, false ); 
	else
	{
		m_pDescText->SetText(
			m_strDesc.c_str() + m_nAniDescCur,
			m_textColor,
			false );
	}
}
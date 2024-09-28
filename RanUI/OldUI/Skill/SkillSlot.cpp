#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "SkillSlot.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "SkillImage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../InnerInterface.h"
#include "SkillWindowToTray.h"
//#include "../../ModalCallerID.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSkillSlot::CSkillSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pInterface(pInterface)
    , m_pNotLearnImage(NULL)
    , m_pSkillImage(NULL)
    , m_pSkillUpButton(NULL)
    , m_pTextBox(NULL)
    , m_sNativeID(NATIVEID_NULL())
    , m_bLearnSkill(FALSE)
    , m_bLEVELUP_ENABLE(FALSE)
    , m_wLevel(0)
    , m_bViewSkillSlot(TRUE)
{
}

CSkillSlot::~CSkillSlot()
{
}

void CSkillSlot::CreateSubControl( SNATIVEID sNativeID )
{
	D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	int	nTextAlign = TEXT_ALIGN_LEFT;

	CreateControl( "SKILL_SLOT_BACK_SKILL_IMAGE" );
    CreateControl( "SKILL_SLOT_BACK_SKILL_TEXT_L" );	
	CreateControl( "SKILL_SLOT_BACK_SKILL_TEXT_M" );
	CreateControl( "SKILL_SLOT_BACK_SKILL_TEXT_R" );

	if ( sNativeID == NATIVEID_NULL() )
	{
//		GASSERT ( 0 && "무효화한 ID 입니다. 확인하십시오." );
	}
	else
	{
		{
			CSkillImage* pSkillImage = CreateSkillImage();
			pSkillImage->SetSkill( sNativeID );
			m_pSkillImage = pSkillImage;		
		}

		{
			m_pNotLearnImage = CreateControl( "SKILL_SLOT_IMAGE_NOTLEARN" );
		}	

		{
			m_pTextBox = CreateStaticControl( "SKILL_SLOT_TEXT", pFont, dwColor, nTextAlign );			
			m_pTextBox->SetLineInterval( 4.0f );
		}

		{
			m_pSkillUpButton = CreateFlipButton( "SKILL_SLOT_LEVEL_UP", "SKILL_SLOT_LEVEL_UP_F", "SKILL_SLOT_LEVEL_UP_OVER", SKILL_SLOT_LEVEL_UP );	
		}
	}

	m_sNativeID = sNativeID;
}

CSkillImage* CSkillSlot::CreateSkillImage()
{
	CSkillImage* pSkillImage = new CSkillImage ( m_pGaeaClient, m_pEngineDevice );
	pSkillImage->CreateSub( this, "SKILL_SLOT_IMAGE", UI_FLAG_DEFAULT, SKILL_SLOT_IMAGE );
	pSkillImage->CreateSubControl();
	pSkillImage->SetUseRender( TRUE );
    RegisterControl( pSkillImage );
	return pSkillImage;
}

CUIControl*	CSkillSlot::CreateControl( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	RegisterControl( pControl );
	return pControl;
}

CBasicButton* CSkillSlot::CreateFlipButton( char* szButton, char* szButtonFlip, char* szMouseOver, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->CreateMouseOver( szMouseOver );
	RegisterControl( pButton );
	return pButton;
}

CBasicTextBox* CSkillSlot::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub( this, szControlKeyword );
	pStaticText->SetFont( pFont );
	pStaticText->SetTextAlign( nAlign );
	RegisterControl( pStaticText );
	return pStaticText;
}

void CSkillSlot::SetViewSkillSlot( const BOOL bView )
{ 
	if(m_pSkillImage) m_pSkillImage->SetVisibleSingle(bView);
	if(m_pTextBox) m_pTextBox->SetVisibleSingle(bView);
	if(m_pSkillUpButton) m_pSkillUpButton->SetVisibleSingle(bView);
	if(m_pNotLearnImage) m_pNotLearnImage->SetVisibleSingle(bView);

	m_bViewSkillSlot = bView; 
}

void CSkillSlot::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	if ( !IsVisible() ) return ;

	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_sNativeID == NATIVEID_NULL() )
	{
//		GASSERT ( 0 && "무효화한 ID 입니다. 확인하십시오." );
		return ;
	}

	if ( !m_bViewSkillSlot ) return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sNativeID.wMainID, m_sNativeID.wSubID );
	if ( !pSkill )
	{
		GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
		return ;
	}

	m_bLearnSkill = FALSE;
	if ( m_pTextBox ) 
	{
		SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL( m_sNativeID );
		BOOL bMASTER = m_pGaeaClient->GetCharacter()->ISMASTER_SKILL( m_sNativeID );
		if ( pCharSkill )	//	배운 스킬이 있을 때
		{
			m_bLearnSkill = TRUE;

			WORD wLevel = pCharSkill->wLevel + 1;
			if ( m_wLevel != wLevel )
			{
				m_wLevel = wLevel;
				SKILL::SLEARN_LVL sLVL = pSkill->m_sLEARN.sLVL_STEP[wLevel];

				m_strLine1.Format ( "[%s]%s.%d", pSkill->GetName(), (char*)ID2GAMEWORD( "SKILL_SLOT", 0 ), pSkill->m_sBASIC.dwGRADE );
				if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) m_strLine1 += (char*)ID2GAMEWORD( "SKILL_SLOT", 1 );

				if ( !bMASTER )
				{
					CString strAA = (char*)ID2GAMEWORD( "SKILL_SLOT", 2 );
					m_strLine2.Format( "%s.%d %s.%d", (char*)ID2GAMEWORD( "SKILL_SLOT", 3 ), wLevel,
						strAA, sLVL.dwSKP );
				}
				else
					m_strLine2.Format( "%s", (char*)ID2GAMEWORD( "SKILL_SLOT", 4 ) );			

				m_pTextBox->ClearText();
				m_pTextBox->AddTextNoSplit( m_strLine1, NS_UITEXTUTIL::ENABLE );
				m_pTextBox->AddTextNoSplit( m_strLine2, NS_UITEXTUTIL::ENABLE );

				//	NOTE
				//		레벨이 바뀌면 바뀌었다고 메시지를 발생시킨다.
				//		UP버튼에서 뿌려주는 다음 레벨에 대한 정보를 위한것임
				AddMessageEx( UIMSG_LEVEL_UP | UIMSG_TOTOPPARENT );
			}
		}		
		else	//	배운 스킬이 없을 경우
		{
			m_pTextBox->ClearText();

			m_strLine1.Format ( "[%s]%s.%d", pSkill->GetName(), (char*)ID2GAMEWORD( "SKILL_SLOT", 0 ), pSkill->m_sBASIC.dwGRADE );
			if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) m_strLine1 += (char*)ID2GAMEWORD( "SKILL_SLOT", 1 );
			m_pTextBox->AddTextNoSplit( m_strLine1, NS_UITEXTUTIL::ENABLE );

			SKILL::SLEARN_LVL sLVL = pSkill->m_sLEARN.sLVL_STEP[0];
			m_strLine2.Format( "%s.%d", (char*)ID2GAMEWORD( "SKILL_SLOT", 2 ), sLVL.dwSKP );
			m_pTextBox->AddTextNoSplit( m_strLine2, NS_UITEXTUTIL::ENABLE );
		}
	}

	//	배운 스킬
	if ( m_pNotLearnImage ) m_pNotLearnImage->SetVisibleSingle( !m_bLearnSkill );
	
	if ( m_pSkillUpButton )
	{
		if ( m_bLearnSkill )
		{
			m_bLEVELUP_ENABLE = m_pGaeaClient->GetCharacter()->CHECKLEARNABLE_SKILL( m_sNativeID ) == EMSKILL_LEARN_OK;
		}
		else
		{
			m_bLEVELUP_ENABLE = FALSE;
		}
		
		m_pSkillUpButton->SetFlip( m_bLEVELUP_ENABLE );
		m_pSkillUpButton->SetVisibleMouseOver( m_bLEVELUP_ENABLE );
	}

	if ( m_bLearnSkill )
	{
		if ( m_pSkillImage ) m_pSkillImage->SetSkillProcess( m_sNativeID );
	}
}

void CSkillSlot::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case SKILL_SLOT_IMAGE:
		{
			//	스킬 들기
			if ( CHECK_MOUSE_IN( dwMsg ) && m_bLearnSkill )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sNativeID.wMainID, m_sNativeID.wSubID );
					if ( !pSkill )
					{
						GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
						return ;
					}
					//	패시브 스킬은 들 수 없다.
					if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) return ;

					ISkillWindowToTray* pSkillWindowToTray = m_pInterface->GetSkillWindowToTray();
					if ( !pSkillWindowToTray ) return ;

					pSkillWindowToTray->SetSkill( m_sNativeID );
					m_pInterface->SkilSlotTrayOpen();

					// 튜토리얼때 필요한 스킬 들고 있는지 확인. by luxes.
					if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
					{
						if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
							m_pGaeaClient->GetTutorial()->IsSubSixStep() 
							|| m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
							m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
						{
							m_pGaeaClient->GetTutorial()->m_sHoldSkill = m_sNativeID;
						}
					}
					//
				}
			}
		}
		break;

	case SKILL_SLOT_LEVEL_UP:
		{
			//	스킬 레벨 업
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( m_bLearnSkill )
				{
					AddMessageEx( UIMSG_MOUSEIN_BUTTON | UIMSG_TOTOPPARENT );
				}

				if ( m_bLEVELUP_ENABLE && dwMsg & UIMSG_LB_UP )
				{
					SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL( m_sNativeID );					
					BOOL bMASTER = m_pGaeaClient->GetCharacter()->ISMASTER_SKILL( m_sNativeID );
					if ( pCharSkill )
					{
						WORD wLevel = pCharSkill->wLevel + 1;
						if ( !bMASTER )
						{
							PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sNativeID.wMainID, m_sNativeID.wSubID );
							if ( !pSkill )
							{
								GASSERT ( 0 && "찾을 수 없는 스킬입니다." );
								return ;
							}

							m_pInterface->SetSkillUpID( m_sNativeID.dwID );

							std::string strLevelUp = sc::string::format( ID2GAMEINTEXT("SKILL_LEVEL_UP_CONFIRM"), pSkill->GetName() ).c_str();
							m_pInterface->DoModal( strLevelUp, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SKILL_LEVEL_UP_CONFIRM );//							
						}
					}
				}
			}
		}
		break;
	}

	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

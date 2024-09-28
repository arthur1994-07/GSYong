#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Pet/GLPetDefine.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../InnerInterface.h"
#include "PetStyleCard.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define BOUND_OF_INDEX	-1

CPetStyleCard::CPetStyleCard(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pStyleName(NULL)
	, m_wPet(0)
	, m_wPetBack(0)
	, m_emTYPE(PETTYPE_NONE)
	, m_bOKButton(FALSE)
{
    m_vecPetStyle.clear();
}

CPetStyleCard::~CPetStyleCard ()
{
    m_vecPetStyle.clear();
}

CBasicTextBox* CPetStyleCard::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CUIControl* CPetStyleCard::CreateControl( const char * szControl )
{
	GASSERT( szControl );

	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub( this, szControl );
	pControl->SetVisibleSingle( FALSE );
	RegisterControl( pControl );

	return pControl;
}

void CPetStyleCard::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_L" );
	CUIWindowEx::CreateControl( "HAIRSTYLECARD_TITLE_BACK_R" );
	m_pStyleName = CreateStaticControl( "HAIRSTYLECARD_TITLE", pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : 스타일 변경 버튼
		CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLECARD_LEFT", UI_FLAG_DEFAULT, PETSTYLECARD_LEFT );
		pButton->CreateFlip( "HAIRSTYLECARD_LEFT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );

		pButton = new CBasicButton(m_pEngineDevice);
		pButton->CreateSub( this, "HAIRSTYLECARD_RIGHT", UI_FLAG_DEFAULT, PETSTYLECARD_RIGHT );
		pButton->CreateFlip( "HAIRSTYLECARD_RIGHT_FLIP", CBasicButton::MOUSEIN_FLIP );
		RegisterControl( pButton );
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, PETSTYLECARD_OK );
		pButton->CreateBaseButton( "HAIRSTYLECARD_OK", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, PETSTYLECARD_CANCEL );
		pButton->CreateBaseButton( "HAIRSTYLECARD_CANCEL", CBasicTextButton::SIZE14, 
									CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}

	{
		CString strCombine;

        int nIndex = 0;
		for (size_t nType = 0; nType < COMMENT::PetStyleCardUI.size(); ++nType )
		{
			m_vecPetStyle.push_back( CreateControlFromPetCommentLua( nIndex++ ) );
		}
        /*for (size_t nType = 0; nType < COMMENT::PET_DEFINE.size(); ++nType )
        {
            for (WORD i = 0; i<GLCONST_PET::sPETSTYLE[nType].wSTYLENum; ++i )
                m_vecPetStyle.push_back( CreateControlFromPetCommentLua( nIndex++ ) );
        }*/
	}
}
CUIControl* CPetStyleCard::CreateControlFromPetCommentLua( int nIndex )
{
    if ( nIndex >= static_cast<int> (COMMENT::PetStyleCardUI.size()) )
    {
        GASSERT( 0 && "CUIControl* CPetStyleCard::CreateControlFromPetCommentLua : index error." );
        return NULL;
    }

    PET_STYLECARD_UI& sPetUI = COMMENT::PetStyleCardUI[nIndex];

    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub( this, "BASIC_PETSTYLE_CARD" );
    pControl->SetVisibleSingle( FALSE );
    pControl->SetLocalPos( UIRECT(sPetUI.wWindowPosX, sPetUI.wWindowPosY, sPetUI.wWindowWidth, sPetUI.wWindowHeight) );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		pControl->SetTextureName( CGameTextMan::GetInstance().GetText( "PET_STYLECARDUI", nIndex, CGameTextMan::EM_COMMENT_TEXT).GetString() );
	else
		pControl->SetTextureName( sPetUI.strTexture.c_str() );

    pControl->SetTexturePos( UIRECT(
        static_cast<float>(sPetUI.wTexturePosX+0.25f)/sPetUI.wTextureSizeX,
        static_cast<float>(sPetUI.wTexturePosY+0.25f)/sPetUI.wTextureSizeY,
        static_cast<float>(sPetUI.wTextureWidth)/sPetUI.wTextureSizeX,
        static_cast<float>(sPetUI.wTextureHeight)/sPetUI.wTextureSizeY) );

    RegisterControl( pControl );

    return pControl;
}

void CPetStyleCard::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PETSTYLECARD_LEFT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                if ( m_wPet-1 < 0 )
                    return;

                m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( FALSE );

                m_wPet -= 1;

                m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( TRUE );
                m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText.c_str(), m_wPet ) );
                m_pGaeaClient->GetPetClient()->StyleChange( m_wPet );
			}
		}
		break;

	case PETSTYLECARD_RIGHT:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
                if ( m_wPet+1 >= GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLENum )
                    return;

                m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( FALSE );

                m_wPet += 1;

                m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( TRUE );
                m_pStyleName->SetText( ID2GAMEINTEXT( m_pId2GameText.c_str(), m_wPet ) );
                m_pGaeaClient->GetPetClient()->StyleChange( m_wPet );
			}
		}
		break;

	case PETSTYLECARD_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case PETSTYLECARD_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void CPetStyleCard::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		m_emTYPE = COMMENT::GetPetDefineIndex( m_pGaeaClient->GetPetClient()->m_emTYPE );

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			m_pId2GameText = CGameTextMan::GetInstance().GetText("PET_STYLECARDTEXT", m_emTYPE, CGameTextMan::EM_COMMENT_TEXT).GetString();
		}
		else
		{			
			m_pId2GameText = COMMENT::szPetStyleCardText[m_emTYPE];
		}

        if ( m_emTYPE == PETTYPE_NONE )
        {
            GASSERT( 0 && "PETTYPE::PETTYPE_NONE" );
            return;
        }

		m_wPet = m_pGaeaClient->GetPetClient()->m_wStyle;
		m_wPetBack = m_wPet;

		m_pStyleName->AddText( ID2GAMEINTEXT( m_pId2GameText.c_str(), m_wPet ) );
		m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( TRUE );

		m_pGaeaClient->GetPetClient()->StyleInitData();
	}
	else
	{
		if( m_bOKButton )
		{
			m_pGaeaClient->GetPetClient()->ReqChangeStyle( m_wPet );
			m_bOKButton = FALSE;
		}
		else
		{
			m_pGaeaClient->GetPetClient()->StyleChange( m_wPetBack );
		}

		m_pStyleName->ClearText();
		m_vecPetStyle[ GetPetStyleIndex() ]->SetVisibleSingle( FALSE );
		
		m_pGaeaClient->GetPetClient()->StyleEnd();
	}
}

int CPetStyleCard::GetPetStyleIndex()
{
    int nPetStyleIndex = 0;
	for ( int i = 0; i < static_cast<int>(COMMENT::PetStyleCardUI.size()); i++ )
	{
		if ( m_emTYPE == COMMENT::PetStyleCardUI[i].wPetType &&
			m_wPet == COMMENT::PetStyleCardUI[i].wPetStyle )
			return nPetStyleIndex;

		nPetStyleIndex++;
	}

    return 0;
}

//int CPetStyleCard::GetPetStyleIndex()
//{
//    int nPetStyleIndex = 0;
//
//    for ( int i = 0; i < static_cast<int>(COMMENT::PET_DEFINE.size()); i++ )
//    {
//        if ( m_emTYPE == COMMENT::GetPetDefineIndex(i) )
//        {
//            nPetStyleIndex += m_wPet;
//            return nPetStyleIndex ;
//        }
//
//        nPetStyleIndex += GLCONST_PET::sPETSTYLE[i].wSTYLENum;
//    }
//
//    return nPetStyleIndex - 1;
//}

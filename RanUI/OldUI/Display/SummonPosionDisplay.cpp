#include "StdAfx.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../Item/ItemImage.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include ".\summonposiondisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const D3DCOLOR CSummonPosionDisplay::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 160, 255, 255, 255 );;
const D3DCOLOR CSummonPosionDisplay::dwFULL_TRANSPARENCY = D3DCOLOR_ARGB ( 255, 255, 255, 255 );;

CSummonPosionDisplay::CSummonPosionDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pPosionImage(NULL)
    , m_pAmountText(NULL)
{
}

CSummonPosionDisplay::~CSummonPosionDisplay(void)
{
}

void CSummonPosionDisplay::CreateSubControl()
{
	m_pPosionImage = CreateItemImage( "SUMMON_POSION_IMAGE", SUMMON_POSION_IMAGE );

	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );
	m_pAmountText = CreateStaticControl ( "SUMMON_POTION_IMAGE_AMOUNT_SINGLE", pFont, TEXT_ALIGN_LEFT );
}

CBasicTextBox* CSummonPosionDisplay::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

CItemImage*	CSummonPosionDisplay::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}


void CSummonPosionDisplay::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		/*
		if( !m_pGaeaClient->GetSummon() ) return;
		GLSummonClient* pSummonData = m_pGaeaClient->GetSummon();
		if( pSummonData && pSummonData->IsVALID() )
		{
		}else{
			m_pInterface->UiHideGroup( SUMMON_POSION_DISPLAY );
		}
		*/
	}else
	{
		m_pInterface->UiHideGroup( SUMMON_POSION_DISPLAY );
	}
}

void CSummonPosionDisplay::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
/*
	switch ( ControlID )
	{
	case SUMMON_POSION_IMAGE:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			GLSummonClient* pSummonData = m_pGaeaClient->GetSummon();
			if( pSummonData && pSummonData->m_sPosionID != NATIVEID_NULL() )
			{
				SITEMCUSTOM sItemCustom = SITEMCUSTOM( pSummonData->m_sPosionID.dwID );
				m_pInterface->SHOW_ITEM_INFO ( sItemCustom, FALSE, FALSE, FALSE, USHRT_MAX, USHRT_MAX );
			}
		}
		break;
	}
*/
}

void CSummonPosionDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
/*
	GLSummonClient* pSummonData = m_pGaeaClient->GetSummon();
	if( pSummonData && pSummonData->IsVALID() )
	{
		if( pSummonData->m_sPosionID != NATIVEID_NULL() )
		{
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter ();
			DWORD dwAmount = pCharacter->m_cInventory.CountTurnItem( pSummonData->m_sPosionID );

			CString strNumber;
			strNumber.Format ( "%d", dwAmount );
			if ( m_pAmountText ) m_pAmountText->SetOneLineText ( strNumber, NS_UITEXTCOLOR::WHITE );

			const SITEM* pItemData = GLogicData::GetInstance().GetItem ( pSummonData->m_sPosionID );
			m_pPosionImage->SetItem ( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );

			m_pPosionImage->SetVisibleSingle( TRUE );
			m_pAmountText->SetVisibleSingle( TRUE );

			if ( dwAmount <= 0 )	m_pPosionImage->SetDiffuse ( dwDEFAULT_TRANSPARENCY );
			else					m_pPosionImage->SetDiffuse ( dwFULL_TRANSPARENCY );
		}else{
			m_pPosionImage->SetVisibleSingle( FALSE );
			m_pAmountText->SetVisibleSingle( FALSE );
		}
	}else{
		m_pInterface->UiHideGroup( SUMMON_POSION_DISPLAY );
	}

*/	

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

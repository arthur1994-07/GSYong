#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "PetQuickSkillSlot.h"
#include "PetWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const D3DCOLOR CPetWindow::COLOR_SKILLSLOT_ENABLE  = 0xFFFFFFFF;
const D3DCOLOR CPetWindow::COLOR_SKILLSLOT_DISABLE = 0xFF888888;

CPetWindow::CPetWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pPetName(NULL)
	, m_pPetType(NULL)
	, m_pMouseOver(NULL)
	, m_pSatietyText(NULL)
	, m_pLoyaltyText(NULL)
	, m_pSatiety(NULL)
	, m_pLoyalty(NULL)
	, m_RollOverID(NO_ID)
	, m_nSatietyBack(0)
	, m_wLoyaltyBack(USHRT_MAX)
{
	SecureZeroMemory( m_pSlotDisplay, sizeof(m_pSlotDisplay) );
	SecureZeroMemory( m_pSlotImage, sizeof(m_pSlotImage) );

	SecureZeroMemory(m_pcbSkill, sizeof(m_pcbSkill));
	SecureZeroMemory(m_pcbSkillRollOver, sizeof(m_pcbSkillRollOver));
}

CPetWindow::~CPetWindow ()
{
}

void CPetWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "PET_BG_LLINEBOX" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "PET_BG_RLINEBOX" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CString strPetSlot[SLOT_IMAGE_SIZE] = 
		{
			"PET_SLOT_IMAGE0",
			"PET_SLOT_IMAGE1",
		};

		CString strPetSlotOver[SLOT_IMAGE_SIZE] = 
		{
			"PET_SLOT_OVER_IMAGE0",
			"PET_SLOT_OVER_IMAGE1",
		};

		for ( int i = 0; i < SLOT_IMAGE_SIZE; i++ )
		{
			m_pSlotImage[i] = CreateItemImage( strPetSlot[i].GetString(), SLOT_IMAGE_TYPE1 + i );
			m_pSlotDisplay[i] = CreateControl( strPetSlotOver[i].GetString() );
			m_pSlotDisplay[i]->SetVisibleSingle( FALSE );
		}

		m_pMouseOver = CreateControl( "ITEM_MOUSE_OVER" );
		m_pMouseOver->SetVisibleSingle( FALSE );
	}

	{
		CBasicTextBox* pTextBox(NULL);

		// 펫 이름
		pTextBox = CreateTextBox ( "PET_NAME_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("PET_TEXT_STATIC",0) );
		m_pPetName = CreateTextBox ( "PET_NAME_TEXTBOX", pFont9 );
	
		// 펫 종류
		pTextBox = CreateTextBox ( "PET_TYPE_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("PET_TEXT_STATIC",1) );
		m_pPetType = CreateTextBox ( "PET_TYPE_TEXTBOX", pFont9 );

		// 스킬 콤보박스 앞 텍스트
		pTextBox = CreateTextBox  ( "PET_SKILL_COMBOBOX_STATIC0", pFont9 );
		pTextBox->AddText		  ( COMMENT::PET_SKILLSLOT_TYPE[0].c_str(), COLOR_SKILLSLOT_ENABLE );
		m_pPetSkillComboBoxStatic[0] = pTextBox;

		pTextBox = CreateTextBox  ( "PET_SKILL_COMBOBOX_STATIC1", pFont9 );
		pTextBox->AddText		  ( COMMENT::PET_SKILLSLOT_TYPE[1].c_str(), COLOR_SKILLSLOT_DISABLE  );
		m_pPetSkillComboBoxStatic[1] = pTextBox;

		// 포만
		pTextBox = CreateTextBox ( "PET_SATIETY_STATIC", pFont9 );
		pTextBox->AddText ( ID2GAMEWORD("PET_TEXT_STATIC",2) );

		CBasicProgressBar* pSatiety = new CBasicProgressBar(m_pEngineDevice);
		pSatiety->CreateSub ( this, "PET_SATIETY_PROGRESSBAR" );
		pSatiety->CreateOverImage ( "PET_SATIETY_PROGRESSBAR_OVERIMAGE" );
		RegisterControl ( pSatiety );
		m_pSatiety = pSatiety;

		CBasicTextBox* pSatietyText = new CBasicTextBox(m_pEngineDevice);
		pSatietyText->CreateSub( this, "PET_SATIETY_TEXT", UI_FLAG_DEFAULT, SATIETY_TEXT );
		pSatietyText->SetFont( pFont8 );
		pSatietyText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pSatietyText );
		m_pSatietyText = pSatietyText;
	
		// 충성
		//pTextBox = CreateTextBox ( "PET_LOYALTY_STATIC", pFont9 );
		//pTextBox->AddText ( ID2GAMEWORD("PET_TEXT_STATIC",3) );

		//CBasicProgressBar* pLoyalty = new CBasicProgressBar(m_pRenderDevice);
		//pLoyalty->CreateSub ( this, "PET_LOYALTY_PROGRESSBAR" );
		//pLoyalty->CreateOverImage ( "PET_LOYALTY_PROGRESSBAR_OVERIMAGE" );
		//RegisterControl ( pLoyalty );
		//m_pLoyalty = pLoyalty;

		//CBasicTextBox* pLoyaltyText = new CBasicTextBox(m_pRenderDevice);
		//pLoyaltyText->CreateSub ( this, "PET_LOYALTY_TEXT", UI_FLAG_DEFAULT, LOYALTY_TEXT );
		//pLoyaltyText->SetFont ( pFont8 );
		//pLoyaltyText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		//RegisterControl ( pLoyaltyText );
		//m_pLoyaltyText = pLoyaltyText;
		
		// 스킬
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON16", UI_FLAG_XSIZE, SKILL_BUTTON );
		pButton->CreateBaseButton ( "PET_SKILL_BUTTON", CBasicTextButton::SIZE16, CBasicButton::CLICK_FLIP, ID2GAMEWORD("PET_TEXT_STATIC",4) );
		RegisterControl ( pButton );

		CBasicComboBox * pComboBox; 
		pComboBox = new CBasicComboBox(m_pEngineDevice);
		pComboBox->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, SKILL_COMBO_OPEN0 );
		pComboBox->CreateBaseComboBox( "PET_SKILL_COMBO_OPEN0" );			
		RegisterControl( pComboBox );
		m_pcbSkill[0] = pComboBox;

		pComboBox = new CBasicComboBox(m_pEngineDevice);
		pComboBox->CreateSub( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, SKILL_COMBO_OPEN1 );
		pComboBox->CreateBaseComboBox( "PET_SKILL_COMBO_OPEN1" );
		RegisterControl( pComboBox );
		m_pcbSkill[1] = pComboBox;

		CBasicComboBoxRollOver * pComboBoxRollOver;
		pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, SKILL_COMBO_ROLLOVER0 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver( "PET_SKILL_COMBO_ROLLOVER0" );
		//pComboBoxRollOver->SetVisibleThumb( false );
		pComboBoxRollOver->SetVisibleSingle( FALSE );
		RegisterControl( pComboBoxRollOver );
		m_pcbSkillRollOver[0] = pComboBoxRollOver;

		pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, SKILL_COMBO_ROLLOVER1 );
		pComboBoxRollOver->CreateBaseComboBoxRollOver( "PET_SKILL_COMBO_ROLLOVER1" );
		//pComboBoxRollOver->SetVisibleThumb( false );
		pComboBoxRollOver->SetVisibleSingle( FALSE );
		RegisterControl( pComboBoxRollOver );
		m_pcbSkillRollOver[1] = pComboBoxRollOver;


		CPetQuickSkillSlot* pPetSkillSlot;
		pPetSkillSlot = new CPetQuickSkillSlot ( m_pGaeaClient, m_pEngineDevice );
		pPetSkillSlot->CreateSub( this, "PET_SKILL_SLOT0", UI_FLAG_DEFAULT, PET_SKILL_SLOT0 );
		pPetSkillSlot->CreateSubControl( 0 );
		RegisterControl ( pPetSkillSlot );

		pPetSkillSlot = new CPetQuickSkillSlot ( m_pGaeaClient, m_pEngineDevice );
		pPetSkillSlot->CreateSub( this, "PET_SKILL_SLOT1", UI_FLAG_DEFAULT, PET_SKILL_SLOT1 );
		pPetSkillSlot->CreateSubControl( 1 );
		RegisterControl ( pPetSkillSlot );
	}
}

CBasicTextBox*	CPetWindow::CreateTextBox ( char* szKeyword, CD3DFontPar* pFont )
{
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, szKeyword );
	pTextBox->SetFont ( pFont );
	RegisterControl ( pTextBox );

	return pTextBox;
}

CUIControl*	CPetWindow::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CItemImage*	CPetWindow::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

EMSUIT CPetWindow::IMAGE2EMSLOT ( UIGUID ControlID )
{
	switch ( ControlID )
	{	
	case SLOT_IMAGE_TYPE1:	return SUIT_PET_A;
	case SLOT_IMAGE_TYPE2:	return SUIT_PET_B;
	}

	GASSERT ( 0 && "CPetWindow::IMAGE2EMSLOT(), 범위를 벗어납니다." );
	return SUIT_NSIZE;
}

void CPetWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if( !m_pGaeaClient->GetPetClient()->IsVALID() )
	{
		m_pInterface->UiHideGroup( GetWndID() );
		return ;
	}

	// Note : 스킬 슬롯 창 비활성화
	GLPetClient* pPet = m_pGaeaClient->GetPetClient();

	if ( pPet )
	{
		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			if ( pPet->m_wMaxSkillSlot <= i )
			{
				m_pPetSkillComboBoxStatic[i]->SetUseTextColor ( 0, TRUE );
				m_pPetSkillComboBoxStatic[i]->SetTextColor	  ( 0, COLOR_SKILLSLOT_DISABLE );
				m_pcbSkill				 [i]->SetTextColor	  ( COLOR_SKILLSLOT_DISABLE );
				m_pcbSkill				 [i]->SetDiffuse	  ( COLOR_SKILLSLOT_DISABLE );
			}
			else
			{
				m_pPetSkillComboBoxStatic[i]->SetUseTextColor ( 0, TRUE );
				m_pPetSkillComboBoxStatic[i]->SetTextColor	  ( 0, COLOR_SKILLSLOT_ENABLE );
				m_pcbSkill				 [i]->SetTextColor	  ( COLOR_SKILLSLOT_ENABLE );
				m_pcbSkill				 [i]->SetDiffuse	  ( COLOR_SKILLSLOT_ENABLE );
			}
		}
	}

	if( m_RollOverID == NO_ID )
	{
		if( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

		//	스냅, 스킬 이미지 붙이기
		IItemMove* pItemMove = m_pInterface->GetItemMove();
		SNATIVEID sHOLD_ITEM_ID(false);
		if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();

		for( int i=0; i<SLOT_IMAGE_SIZE; ++i )
		{
			SITEMCUSTOM & sItemCustomOld = m_ItemCustomArray[i];
			SITEMCUSTOM & sItemCustom = m_pGaeaClient->GetPetClient()->m_PutOnItems[i];

			if ( sItemCustom != sItemCustomOld )
			{		
				if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
				{
					LoadItem ( i, sItemCustom );
				}
				else
				{
					UnLoadItem ( i );
				}
			}

			m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

			if( sHOLD_ITEM_ID != NATIVEID_NULL() )
			{
				// 장착될 위치 표시
				if( m_pGaeaClient->GetPetClient()->CheckSlotItem( sHOLD_ITEM_ID, IMAGE2EMSLOT( i + SLOT_IMAGE_TYPE1 ) ) )
				{
					const UIRECT& rcImagePos = m_pSlotImage[i]->GetGlobalPos ();
					D3DXVECTOR2 vPos( rcImagePos.left, rcImagePos.top );

					m_pSlotDisplay[i]->SetVisibleSingle( TRUE );
				}
			}
		}

		CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	int nFull = m_pGaeaClient->GetPetClient()->Satiation();
	SetSatiety( nFull );
}

void CPetWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( PET_WINDOW );
			}
		}
		break;
	case SLOT_IMAGE_TYPE1:
	case SLOT_IMAGE_TYPE2:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{	
				int nIndex = ControlID - SLOT_IMAGE_TYPE1;

				if ( !m_pInterface->IsFirstItemSlot () ) return ;

				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pSlotImage[nIndex]->GetGlobalPos();
					pItemMove->SetUIWindowGlobalPos( rcSlotPos );						

					AddMessageEx( UIMSG_MOUSEIN_WEARSLOT1 | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pSlotImage[nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pSlotImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos( rcImagePos );
					m_pMouseOver->SetVisibleSingle( TRUE );
				}

				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pGaeaClient->GetPetClient()->ReqChangeAccessory( IMAGE2EMSLOT(ControlID) );
				}

				if ( dwMsg & UIMSG_RB_UP )
				{
					m_pGaeaClient->GetPetClient()->ReqRemoveSlotItem( IMAGE2EMSLOT(ControlID) );
				}

				if( m_ItemCustomArray[nIndex].GetNativeID() != NATIVEID_NULL () )
				{
					m_pInterface->SHOW_ITEM_INFO(
						m_ItemCustomArray[nIndex],
						FALSE, FALSE, FALSE, FALSE, FALSE,
						USHRT_MAX, USHRT_MAX );
				}
			}
		}
		break;

	case SKILL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( !m_pInterface->UiIsVisibleGroup( PET_SKILL_WINDOW ) )
				{
					m_pInterface->UiShowGroupFocus( PET_SKILL_WINDOW );
				}
				else
				{
					m_pInterface->UiHideGroup( PET_SKILL_WINDOW );
				}
			}
		}
		break;

	case SKILL_COMBO_OPEN0:
	case SKILL_COMBO_OPEN1:
		{
			WORD wSlot = (WORD)(ControlID - SKILL_COMBO_OPEN_START);

			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				// Note : 최대 스킬 슬롯 만큼 사용할 수 있다.
				if ( m_pGaeaClient->GetPetClient()->m_wMaxSkillSlot <= wSlot )
				{
					break;
				}
				
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pcbSkillRollOver[wSlot]->SetVisibleSingle( TRUE );
					m_pcbSkillRollOver[wSlot]->SetScrollPercent ( 0.0f );
					
					m_RollOverID = SKILL_COMBO_ROLLOVER_START + wSlot;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case SKILL_COMBO_ROLLOVER0:
	case SKILL_COMBO_ROLLOVER1:
		{
			WORD wSlot = (WORD)(ControlID - SKILL_COMBO_ROLLOVER_START);

			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				GLPetClient* pPet = m_pGaeaClient->GetPetClient();

				if ( !pPet ) return;

				int nIndex = m_pcbSkillRollOver[wSlot]->GetSelectIndex ();
				if ( nIndex < 0 ) return ;

				SNATIVEID sID( m_pcbSkillRollOver[wSlot]->GetTextData( nIndex ) );

				if( nIndex > 0 )
				{
					if( !pPet->CheckSkill( sID, wSlot ) )
					{
						return;
					}
				}

				if( nIndex == 0 ) sID = NATIVEID_NULL();

				// Note : 임시로 세팅한다.
				SNATIVEID sPreID = pPet->m_sActiveSkillID[wSlot];
				pPet->m_sActiveSkillID[wSlot] = sID; 

				// Note : 특수한 스킬 조합에 대해 처리를 한다.
				EMPETEQUALITY_SKILL emEquality = pPet->IsSkillDuplicatedEx ();

				// Note : 되돌린다.
				pPet->m_sActiveSkillID[wSlot] = sPreID;

				switch ( emEquality )
				{
					// Note : 없다.
				case EMPETEQUALITY_SKILL_NONE: 
					break;

					// Note : 스킬 자체가 중복
				case EMPETEQUALITY_SKILL_DUPLICATED:
					{
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("PET_SKILL_DUPLICATION_FORBID") );

						// Note : 스킬을 다시 되돌린다.
						m_pcbSkillRollOver[wSlot]->SetSelectIndex( pPet->m_sActiveSkillID[wSlot].dwID );
						return;
					}
					break;

					// Note : 스텟 증가가 중복
				case EMPETEQUALITY_SKILL_DUPLICATIONSTATS: 
					{
						m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("PET_SKILL_DUPLICATIONSTATS_MSG") );
					}
					break;

				}

				CString strText = m_pcbSkillRollOver[wSlot]->GetSelectText( nIndex );
				m_pcbSkill[wSlot]->SetText( strText );

				pPet->ReqChangeActiveSkill( sID, wSlot );
			}
		}
		break;
	}
}

void CPetWindow::SetName( const std::string & strName )
{
	GASSERT( m_pPetName );
	m_pPetName->SetOneLineText( strName.c_str() );
}

void CPetWindow::SetType( const std::string & strType )
{
	GASSERT( m_pPetType );
	m_pPetType->SetOneLineText( strType.c_str() );
}

void CPetWindow::SetSkillCombo()
{
	GLPetClient* pPet = m_pGaeaClient->GetPetClient();

	if ( !pPet )
		return;

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		GASSERT( m_pcbSkillRollOver[i] );
		m_pcbSkillRollOver[i]->ClearText();

		int nIndex = m_pcbSkillRollOver[i]->AddText( ID2GAMEWORD("PET_TEXT_STATIC",5) );
		m_pcbSkillRollOver[i]->SetTextData( nIndex, SNATIVEID( false ).dwID );

		SNATIVEID & sID	= pPet->m_sActiveSkillID[i];

		// Note : 다른 활성화된 스킬 배열을 가져온다.
		BYTE* pActiveSkillArray = NULL;
		pPet->IsSkillDuplicatedEx ( &pActiveSkillArray );

		PETSKILL_MAP & mapPetSkill = pPet->m_ExpSkills;
		PETSKILL_MAP_ITER pos	   = mapPetSkill.begin();
		PETSKILL_MAP_ITER end	   = mapPetSkill.end();
		for ( ; pos!=end; ++pos )
		{
			PETSKILL petSkill( (*pos).second );

			PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( petSkill.sNativeID );
			if( !pSKILL ) continue;

			int nIndex = 0;
 
			// Note : 다른 인덱스에서 활성화 된 스킬은 다른색으로 처리, 단 자기 자신은 제외
			if ( pActiveSkillArray && pActiveSkillArray[pSKILL->m_sAPPLY.emBASIC_TYPE] && sID != petSkill.sNativeID )
			{
				nIndex = m_pcbSkillRollOver[i]->AddText( pSKILL->GetName(), FALSE, FALSE, NS_UITEXTCOLOR::RED );
			}
			else
			{
				nIndex = m_pcbSkillRollOver[i]->AddText( pSKILL->GetName(), FALSE, FALSE, NS_UITEXTCOLOR::WHITE );
			}

			m_pcbSkillRollOver[i]->SetTextData( nIndex, petSkill.sNativeID.dwID );
		}

		if ( sID == NATIVEID_NULL() )
		{
			m_pcbSkill[i]->SetText( ID2GAMEWORD("PET_TEXT_STATIC",5) );
			continue;
		}

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sID );
		if( !pSKILL )
		{
			m_pcbSkill[i]->SetText( ID2GAMEWORD("PET_TEXT_STATIC",5) );
		}
		else
		{
			m_pcbSkill[i]->SetText( pSKILL->GetName() );
		}
	}
}

void CPetWindow::SetSatiety( int nNOW )
{
	GASSERT( m_pSatietyText && m_pSatiety );

	if ( m_nSatietyBack != nNOW )
	{
		m_nSatietyBack = nNOW;

		CString strTemp;
		strTemp.Format( "%.2f%%", nNOW/10.0f );

		m_pSatietyText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );
		m_pSatiety->SetPercent( nNOW/1000.0f ); //< 입력값은 0 과 1 사이의 값이다.
	}
}

//void CPetWindow::SetLoyalty( WORD wNOW, WORD wMAX )
//{
//	GASSERT( m_pLoyaltyText && m_pLoyalty );
//
//	if ( m_wLoyaltyBack != wNOW )
//	{
//		m_wLoyaltyBack = wNOW;
//
//		float fPercent(0.0f);
//		if( wNOW != 0 ) fPercent = float(wNOW) / float(wMAX);
//
//		CString strTemp;
//		strTemp.Format( "%.2f%%", fPercent );
//
//		m_pLoyaltyText->SetOneLineText( strTemp, NS_UITEXTCOLOR::WHITE );
//		m_pLoyalty->SetPercent( fPercent );
//	}
//}

void CPetWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		std::string strPetName(m_pGaeaClient->GetPetClient()->GetName());
		SetName( strPetName );

		int emTYPE = m_pGaeaClient->GetPetClient()->m_emTYPE;

		if( emTYPE == PETTYPE_NONE )
		{
			SetType( ID2GAMEWORD("PET_TEXT_STATIC",5) );
		}
		else 
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				SetType( CGameTextMan::GetInstance().GetText("PET_TYPE", emTYPE, CGameTextMan::EM_COMMENT_TEXT).GetString() );
			}
			else
			{
				SetType( COMMENT::PET_TYPE[emTYPE] );
			}
		}

		SetSkillCombo();
	}
	else
	{
		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			m_pcbSkillRollOver[i]->SetVisibleSingle ( FALSE );
		}

		m_pInterface->UiHideGroup( PET_SKILL_WINDOW );
	}
}

void CPetWindow::LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( ref_sItemCustom.GetNativeID() );
	m_pSlotImage[nIndex]->SetItem ( ref_sItemCustom );
}

void CPetWindow::UnLoadItem ( int nIndex )
{
	m_ItemCustomArray[nIndex] = SITEMCUSTOM( NATIVEID_NULL() );
	m_pSlotImage[nIndex]->ResetItem ();
}

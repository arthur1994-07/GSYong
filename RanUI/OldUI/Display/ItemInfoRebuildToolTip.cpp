#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Item/ItemDurability.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/RandomOption/RandomOption.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "../Util/CheckBox.h"

#include "../../InnerInterface.h"

#include "ItemInfoRebuildToolTip.h"

ItemInfoRebuildRandomOptionSlot::ItemInfoRebuildRandomOptionSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
{
}

ItemInfoRebuildRandomOptionSlot::~ItemInfoRebuildRandomOptionSlot(void)
{
}

void ItemInfoRebuildRandomOptionSlot::CreateSubControl(void)
{
	m_pCheckBox = new CCheckBox( m_pEngineDevice ); 
	m_pCheckBox->CreateSub( this, "ITEM_REBUILD_RENEWAL_RANDOM_OPTION_CHECK_BOX" , UI_FLAG_DEFAULT , EMCHECK_BOX );
	m_pCheckBox->CreateSubControl ( "PARTY_INFO_BUFF_CHECK_BOX_ON", "PARTY_INFO_BUFF_CHECK_BOX_OFF" );
	m_pCheckBox->SetCheck( FALSE );	
	RegisterControl( this->m_pCheckBox );

	CD3DFontPar* pFont10 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );
	m_pInformation = new CBasicTextBox(m_pEngineDevice);
	m_pInformation->CreateSub(this, "ITEM_REBUILD_RENEWAL_RANDOM_OPTION_TEXT_BOX");
	m_pInformation->SetFont(pFont10);
	m_pInformation->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
	RegisterControl( m_pInformation );	
}

void ItemInfoRebuildRandomOptionSlot::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);
	switch ( ControlID )
	{
	case EMCHECK_BOX:
		if ( CHECK_MOUSE_IN(dwMsg) == TRUE )
		{
			// 체크 박스 위에 커서를 올렸을 때, 띄울 툴팁 메세지;
			// 기존에는 인장 카드로 고정 가능한 횟수가 표시 되었으나,
			// 고정 가능 횟수 제한이 삭제 됨으로 인해 사라짐;
		}
		break;
	}
}

void ItemInfoRebuildRandomOptionSlot::hideCheckBox(void)
{
	m_pCheckBox->SetVisibleSingle(FALSE);
	m_pCheckBox->Reset();
}
void ItemInfoRebuildRandomOptionSlot::showCheckBox(void)
{
	m_pCheckBox->SetVisibleSingle(TRUE);
}
void ItemInfoRebuildRandomOptionSlot::resetCheck(void)
{
	m_pCheckBox->Reset();
}
const bool ItemInfoRebuildRandomOptionSlot::isChecked(void) const
{
	return m_pCheckBox->IsChecked();
}


ItemInfoRebuildToolTip::ItemInfoRebuildToolTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_nMaxCheckable(0)
, m_bVisibleCheckBox(false)
, m_bDisplayResist( false )
, m_pResistSpirit( NULL )
, m_pResistFire( NULL )
, m_pResistIce( NULL )
, m_pResistElectric( NULL )
, m_pResistPoison( NULL )
, m_pResistSpiritText( NULL )
, m_pResistFireText( NULL )
, m_pResistIceText( NULL )
, m_pResistElectricText( NULL )
, m_pResistPoisonText( NULL )
, m_fDurabilityPerformance(1.0f)
{
}
ItemInfoRebuildToolTip::~ItemInfoRebuildToolTip(void)
{
}

void ItemInfoRebuildToolTip::CreateSubControl(void)
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, D3DFONT_BOLD | D3DFONT_SHADOW_EX | D3DFONT_KSC5601 );
	
	m_pBasicInformation = new CBasicTextBox(m_pEngineDevice);
	m_pBasicInformation->CreateSub(this, "ITEM_REBUILD_RENEWAL_RANDOM_OPTION_TEXT_BOX");
	m_pBasicInformation->SetFont(pFont9);
	m_pBasicInformation->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
	RegisterControl( m_pBasicInformation );

	m_pAdditionalInformation = new CBasicTextBox(m_pEngineDevice);
	m_pAdditionalInformation->CreateSub(this, "ITEM_REBUILD_RENEWAL_RANDOM_OPTION_TEXT_BOX");
	m_pAdditionalInformation->SetFont(pFont9);
	m_pAdditionalInformation->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP);
	RegisterControl( m_pAdditionalInformation );

	for ( unsigned int _i(RandomOpt::NSIZE); _i--; )
	{
		m_pRandomInformation[_i] = new ItemInfoRebuildRandomOptionSlot(m_pGaeaClient, m_pInterface, m_pEngineDevice);
		m_pRandomInformation[_i]->CreateSub(this, "ITEM_REBUILD_RENEWAL_TOOLTIP");
		m_pRandomInformation[_i]->CreateSubControl();
		RegisterControl(m_pRandomInformation[_i]);
	}

	{
		CUIControl* pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_SPIRIT" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pResistSpirit = pControl;

		pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_FIRE" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pResistFire = pControl;

		pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ICE" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pResistIce = pControl;

		pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ELECTRIC" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pResistElectric = pControl;

		pControl = new CUIControl( m_pEngineDevice );
		pControl->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_POISON" );
		pControl->SetVisibleSingle( FALSE );
		RegisterControl( pControl );
		m_pResistPoison = pControl;
	}

	{
		CBasicTextBox* pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_SPIRIT_TEXT" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pResistSpiritText = pBasicTextBox;

		pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_FIRE_TEXT" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pResistFireText = pBasicTextBox;

		pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ICE_TEXT" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pResistIceText = pBasicTextBox;

		pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_ELECTRIC_TEXT" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pResistElectricText = pBasicTextBox;

		pBasicTextBox = new CBasicTextBox( m_pEngineDevice );
		pBasicTextBox->CreateSub( this, "ITEM_INFOR_TOOLTIP_REGIST_POISON_TEXT" );
		pBasicTextBox->SetFont( pFont9 );
		pBasicTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
		RegisterControl( pBasicTextBox );
		m_pResistPoisonText = pBasicTextBox;
	}
}

void ItemInfoRebuildToolTip::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{	
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);	
}

void ItemInfoRebuildToolTip::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	m_vMousePosition = D3DXVECTOR2(float(x), float(y));

	ItemInfoRebuildToolTip::_updateShowCheckBox(m_bVisibleCheckBox);	
}


void ItemInfoRebuildToolTip::setItemInformation(const SITEMCUSTOM& itemCustom)
{
	const SITEM* const pItemData = GLogicData::GetInstance().GetItem(itemCustom.GetNativeID());
	if ( pItemData == NULL )
		return;

	if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
		m_fDurabilityPerformance = ItemDurability::GetInstance().GetPerformance(itemCustom);
	
	ItemInfoRebuildToolTip::_setItemBasicInformation(itemCustom, pItemData);
	ItemInfoRebuildToolTip::_setItemRandomInformation(itemCustom, pItemData);
	ItemInfoRebuildToolTip::_setItemAdditionalInformation(itemCustom, pItemData);
	

	ItemInfoRebuildToolTip::_resizeInformationBox();	

	m_ItemCustom = itemCustom;
}

void ItemInfoRebuildToolTip::setVisibleCheckBox(const bool bVisible, const DWORD nMaxCheckable)
{
	m_bVisibleCheckBox = bVisible;
	m_nMaxCheckable = nMaxCheckable;
}

void ItemInfoRebuildToolTip::resetCheckBox(void)
{
	for ( unsigned int _i(0); _i < RandomOpt::NSIZE; ++_i )
		m_pRandomInformation[_i]->m_pCheckBox->Reset();
}
void ItemInfoRebuildToolTip::setCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption)
{
	m_ItemCustom = sItemCustom;
	m_stateCheckBox.reset();
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
	{
		if ( fixOption == m_ItemCustom.getOptType(_i) )
		{
			m_pRandomInformation[_i]->m_pCheckBox->SetCheckOn();
			m_stateCheckBox += (1 << _i);			
		}
	}
}


void ItemInfoRebuildToolTip::_setItemBasicInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData)
{
	m_pBasicInformation->ClearText();
	std::string strTemp = "";
	GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
	EMCHARINDEX charIdx = GLCI_NUM_ETC;
	int nLevel = -1;
	if ( pChar )
	{
		charIdx = pChar->GETCHARINDEX();
		nLevel = pChar->GETLEVEL();
	}

	const ITEM::SSUIT* psDisguiseSuit = NULL;
	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( itemCustom.nidDISGUISE );
	if( NULL != pDisguise )
		psDisguiseSuit = &pDisguise->sSuitOp;

	{
        //	공격력
		GLPADATA& sDamage = itemCustom.getdamage(0.0f, charIdx, nLevel);
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(itemCustom.GetNativeID()); 
		
		int nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow ); 
		int nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh );
		int nRndOptDamage = static_cast<int>( sDamage.wLow - pTempITEM.sSuitOp.gdDamage.wLow );
		int nExtraValue = static_cast<int>( itemCustom.GETGRADE_DAMAGE() );
		BYTE uGRADE = itemCustom.GETGRADE(EMGRINDING_DAMAGE);
		
		//코스튬 적용 시 코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptDamage -= static_cast<int>( psDisguiseSuit->gdDamage.wLow ); 

				//기본 로직
				//nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow * m_fDurabilityPerformance);
				//nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh * m_fDurabilityPerformance );
				nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityPerformance );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptDamage += static_cast<int>( psDisguiseSuit->gdDamage.wLow );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				GLDWDATA& damage = itemCustom.GETDAMAGE(charIdx, nLevel);
				nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
				nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//기본 로직
				//nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow * m_fDurabilityPerformance);
				//nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh * m_fDurabilityPerformance );
				nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityPerformance );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				GLDWDATA& damage = itemCustom.GETDAMAGE(charIdx, nLevel);
				nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
				nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
			}
		}

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;
		
		//VAR_PARAM 함수에서 1 이하일때 1로 잡아주는 코드가 있어서 튤팁에서 조건 검사를 한번 더 해줘야함
		if ( nMinDmg || nMaxDmg || nRndOptDamage || nExtraValue || uGRADE )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->gdDamage.wNow && 0 != psDisguiseSuit->gdDamage.wMax )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nMinDmg );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0 )
				{				
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant2 );

				m_pBasicInformation->AddString( nIndex, " ~ ", dwColor );

				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				
			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nMinDmg );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				m_pBasicInformation->AddString( nIndex, " ~ ", dwColor );

				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
			}
		}
	}
	//

	// Info : 방어
	{
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(itemCustom.GetNativeID()); 
		//여기서 해당 아이템 정보가져옴

		//	방어력
		int nBasicDef = static_cast<int>( pTempITEM.sSuitOp.nDefense);
		int nRndOptDef = static_cast<int>( itemCustom.getdefense(charIdx, nLevel) - pTempITEM.sSuitOp.nDefense);
		int nExtraValue = itemCustom.GETGRADE_DEFENSE();
		BYTE uGRADE = itemCustom.GETGRADE(EMGRINDING_DEFENSE);

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;

		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptDef -= psDisguiseSuit->nDefense; 

				//nBasicDef = static_cast<int>( nBasicDef * m_fDurabilityPerformance );
				nRndOptDef = static_cast<int>( nRndOptDef * m_fDurabilityPerformance); 
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptDef += psDisguiseSuit->nDefense;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicDef = static_cast<int>(itemCustom.GETDEFENSE(charIdx, nLevel)* m_fDurabilityPerformance) - nRndOptDef - nExtraValue;	
			}

		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicDef = static_cast<int>( nBasicDef * m_fDurabilityPerformance );
				nRndOptDef = static_cast<int>( nRndOptDef * m_fDurabilityPerformance );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicDef = static_cast<int>(itemCustom.GETDEFENSE(charIdx, nLevel) * m_fDurabilityPerformance) - nRndOptDef - nExtraValue;
			}
		}

		DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
		if( NULL != psDisguiseSuit )
		{
			if( 0 != psDisguiseSuit->nDefense )
				dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
		}

		if ( nBasicDef || uGRADE || nRndOptDef )
		{
			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
				{
					strTemp = sc::string::format( "(+%d)", nExtraValue );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}
				else if ( nExtraValue < 0 )
				{
					strTemp = sc::string::format( "(%d)", nExtraValue );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}
					
			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
			}
		}
	}


	// Info : 명중률, 회피율, 소모 SP
	{
		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwRndOptColor = ITEM_INFOR_COLOR::dwInchant1;
		
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(itemCustom.GetNativeID()); 

		//	명중률
		int nBasicHitRate = static_cast<int>( pTempITEM.sSuitOp.nHitRate );
		int nRndOptHitRate = static_cast<int>( itemCustom.GETHITRATE() - pTempITEM.sSuitOp.nHitRate );
		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptHitRate -= psDisguiseSuit->nHitRate;

				//nBasicHitRate = static_cast<int>( nBasicHitRate * m_fDurabilityPerformance );
				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityPerformance );	

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptHitRate += psDisguiseSuit->nHitRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>( itemCustom.GETHITRATE() * m_fDurabilityPerformance ) - nRndOptHitRate;				
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicHitRate = static_cast<int>( nBasicHitRate * m_fDurabilityPerformance );
				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>(itemCustom.GETHITRATE() * m_fDurabilityPerformance) - nRndOptHitRate;
			}
		}

		if ( nBasicHitRate ||  nRndOptHitRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nHitRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );
			int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicHitRate );
			m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptHitRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptHitRate );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptHitRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptHitRate );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	회피율
		int nBasicAvoidRate = static_cast<int>( pTempITEM.sSuitOp.nAvoidRate );
		int nRndOptAvoidRate = static_cast<int>( itemCustom.GETAVOIDRATE() - pTempITEM.sSuitOp.nAvoidRate );
		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptAvoidRate -= psDisguiseSuit->nAvoidRate;

				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( (nRndOptAvoidRate -  psDisguiseSuit->nAvoidRate) * m_fDurabilityPerformance ); 

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptAvoidRate += psDisguiseSuit->nAvoidRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(itemCustom.GETAVOIDRATE() * m_fDurabilityPerformance) - nRndOptAvoidRate;		
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurabilityPerformance ); 

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(itemCustom.GETAVOIDRATE() * m_fDurabilityPerformance) - nRndOptAvoidRate;	
			}
		}

		if ( nBasicAvoidRate ||  nRndOptAvoidRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nAvoidRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
			int nIndex =  m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicAvoidRate );
			m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptAvoidRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptAvoidRate );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptAvoidRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptAvoidRate );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	소모 SP
		int nBasicReqSP = static_cast<int>( pTempITEM.sSuitOp.wReqSP );
		int nRndOptReqSP = static_cast<int>( itemCustom.GETREQ_SP() - pTempITEM.sSuitOp.wReqSP);
		
		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptReqSP -= static_cast<int>( psDisguiseSuit->wReqSP );

				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityPerformance ); 

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptReqSP += static_cast<int>( psDisguiseSuit->wReqSP );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>( itemCustom.GETREQ_SP()* m_fDurabilityPerformance ) - nRndOptReqSP;
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>(itemCustom.GETREQ_SP() * m_fDurabilityPerformance) - nRndOptReqSP;
			}
		}

		if ( nBasicReqSP || nRndOptReqSP )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->wReqSP )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7)  );
			int nIndex = m_pBasicInformation->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicReqSP );
			m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptReqSP > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptReqSP );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptReqSP < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptReqSP );
				m_pBasicInformation->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}
	}
}


void ItemInfoRebuildToolTip::_setItemAdditionalInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData)
{
	m_pAdditionalInformation->ClearText();

	const ITEM::SSUIT& suitBasic = pItemData->sSuitOp;
	std::string strText = "";
	int nValueArray[EMADD_SIZE];
	SecureZeroMemory(nValueArray, sizeof(nValueArray));

	for ( unsigned int _i(ITEM::SSUIT::ADDON_SIZE); _i--; )
	{
		const EMITEM_ADDON emType = suitBasic.sADDON[_i].emTYPE;

		if ( emType != EMADD_NONE )
		{
			const int nValue(static_cast<int>(suitBasic.sADDON[_i].nVALUE * m_fDurabilityPerformance));

			switch ( emType )
			{
				//	밑에서 한번에 계산
			case EMADD_HP:
			case EMADD_MP:
			case EMADD_SP:
			case EMADD_MA:
				break;
			default:
				nValueArray[emType] += nValue;
			};
		}
	}

	//	전체 옵션을 따로 계산
	nValueArray[EMADD_HP] = static_cast<int>( itemCustom.GETADDHP() * m_fDurabilityPerformance );
	nValueArray[EMADD_MP] = static_cast<int>( itemCustom.GETADDMP() * m_fDurabilityPerformance );
	nValueArray[EMADD_SP] = static_cast<int>( itemCustom.GETADDSP() * m_fDurabilityPerformance );
	nValueArray[EMADD_MA] = static_cast<int>( itemCustom.GETADDMA() * m_fDurabilityPerformance );

	//	코스튬 능력치
	if ( itemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		const SITEM* const pCostume(GLogicData::GetInstance().GetItem( itemCustom.nidDISGUISE ));

		//if ( pCostume && pCostume->sBasicOp.IsDISGUISE() )
		if ( pCostume && ( itemCustom.IsDisguise() != NULL ) )
		{
			const ITEM::SSUIT& suitCostume = pCostume->sSuitOp;
			for ( unsigned int _i(ITEM::SSUIT::ADDON_SIZE); _i--; )
			{
				const EMITEM_ADDON emType(suitCostume.sADDON[_i].emTYPE);

				if ( emType != EMADD_NONE )
					continue;

				nValueArray[emType] += pCostume->sSuitOp.sADDON[_i].nVALUE;
			}
		}
	}

	// 내구도 컬러;
	//const DWORD dwAddAbility(m_fDurabilityPerformance < 1.0f ? ItemDurability::GetInstance().GetTextColor(itemCustom, ITEM_INFOR_COLOR::dwAddAbility) : ITEM_INFOR_COLOR::dwAddAbility);
	const DWORD dwAddAbility(ITEM_INFOR_COLOR::dwAddAbility);

	for ( unsigned int _i(0); _i < EMADD_SIZE; ++_i )
	{
		const int nValue(nValueArray[_i]);
		if( nValue == 0 )
			continue;

		strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADDON_INFO", _i));

		const int nIndex(m_pAdditionalInformation->AddText(strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility));

		strText = sc::string::format(nValue > 0 ? "+%d" : "%d", nValue);

		m_pAdditionalInformation->AddString( nIndex, strText.c_str(), dwAddAbility);
	}

	// Info : 특수 기능
	{
		const EMITEM_VAR emITEM_VAR(suitBasic.sVARIATE.emTYPE);
		const EMITEM_VAR emITEM_VOL(suitBasic.sVOLUME.emTYPE);

		float fValueArray[EMVAR_SIZE];
		SecureZeroMemory(fValueArray, sizeof(fValueArray));

		//	특수기능 ( 변화율 )
		//
		if ( emITEM_VAR != EMVAR_NONE )
		{
			switch ( emITEM_VAR )
			{
			case EMVAR_HP:
			case EMVAR_MP:
			case EMVAR_SP:
			case EMVAR_AP:
				break;
			default:
				fValueArray[emITEM_VAR] = suitBasic.sVARIATE.fVariate * m_fDurabilityPerformance;
				break;
			};
		}

		fValueArray[EMVAR_HP] = itemCustom.GETINCHP() * m_fDurabilityPerformance;
		fValueArray[EMVAR_MP] = itemCustom.GETINCMP() * m_fDurabilityPerformance;
		fValueArray[EMVAR_SP] = itemCustom.GETINCSP() * m_fDurabilityPerformance;
		fValueArray[EMVAR_AP] = itemCustom.GETINCAP() * m_fDurabilityPerformance; 

		//	코스튬 능력치;
		float fCostumVolume = 0.0f;
		if ( itemCustom.nidDISGUISE != NATIVEID_NULL() )
		{
			const SITEM* const pCostume = GLogicData::GetInstance().GetItem(itemCustom.nidDISGUISE);

			SITEMCUSTOM sCustomeItem(itemCustom.nidDISGUISE);
			if ( pCostume && sCustomeItem.IsDisguise())
			{
				EMITEM_VAR emITEM_VAR_COS = pCostume->sSuitOp.sVARIATE.emTYPE;
				EMITEM_VAR emITEM_VOL_COS = pCostume->sSuitOp.sVOLUME.emTYPE;

				if ( emITEM_VAR_COS != EMVAR_NONE )
				{
					float fVariate = pCostume->sSuitOp.sVARIATE.fVariate;		

					switch ( emITEM_VAR_COS )
					{
					case EMVAR_HP:
					case EMVAR_MP:
					case EMVAR_SP:
					case EMVAR_AP:
						fValueArray[emITEM_VAR_COS] += fVariate;
						break;
					default:
						fValueArray[emITEM_VAR_COS] += fVariate;
						break;
					};
				}

				if ( emITEM_VOL_COS != EMVAR_NONE)
				{
					fCostumVolume = pCostume->sSuitOp.sVOLUME.fVolume;
				}
			}
		}


		// 내구도 컬러;
		//const DWORD dwAddAbility(m_fDurabilityPerformance < 1.0f ? ItemDurability::GetInstance().GetTextColor(itemCustom, ITEM_INFOR_COLOR::dwAddAbility) : ITEM_INFOR_COLOR::dwAddAbility);
		const DWORD dwAddAbility( ITEM_INFOR_COLOR::dwAddAbility );

		for ( unsigned int _i = 1; _i  < EMVAR_SIZE; ++_i  )
		{
			if ( fValueArray[_i] != 0.0f ) 
			{
				if ( pItemData->BasicType() != ITEM_PET_FOOD && pItemData->BasicType() != ITEM_VEHICLE_OIL )
				{
					fValueArray[_i] *= COMMENT::ITEMVAR_SCALE[_i];
				}

				CString strTemp = sc::string::format( "%.2f", fValueArray[_i] ).c_str();

				_cutZero(strTemp,2);

				strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", _i) );
				int nIndex = m_pAdditionalInformation->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp);

				if ( itemCustom.nidDISGUISE != NATIVEID_NULL() )
				{

					const SITEM* pItem = GLogicData::GetInstance().GetItem( itemCustom.nidDISGUISE );
					SITEMCUSTOM sCustomeItem(itemCustom.nidDISGUISE);
					if ( pItem && ( sCustomeItem.IsDisguise() != NULL ) )
					{
						const SITEM* pNativeItem = GLogicData::GetInstance().GetItem( itemCustom.GetNativeID() );
						if( pNativeItem )
						{
							// 문자열이 아닌 데이터는 CommetStrTable.xml에 없다;
							// 데이터는 기존 Comment데이터를 참조한다;
							EMITEM_VAR emITEM_VAR_COS = pNativeItem->sSuitOp.sVARIATE.emTYPE;
// 							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 							{
// 								if (atof(CGameTextMan::GetInstance().GetCommentText("ITEMVAR", emITEM_VAR_COS*2+1).GetString()) == 100.0f)
// 									strText += "%";
// 							}
// 							else
 							{
								if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR_COS) )
									strText += "%";
							}
						}
					}
				}
				else
				{
					// 문자열이 아닌 데이터는 CommetStrTable.xml에 없다;
					// 데이터는 기존 Comment데이터를 참조한다;
// 					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 					{
// 						if (atof(CGameTextMan::GetInstance().GetCommentText("ITEMVAR", emITEM_VAR*2+1).GetString()) == 100.0f)
// 							strText += "%";
// 					}
// 					else
					{
						if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR) )
							strText += "%";
					}
				}

				m_pAdditionalInformation->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
		}


		// 경험치 배율
		float fExpMultiple = pItemData->GetExpMultiple();
		
		//내구도 적용
		if( (fExpMultiple > 1.0f) || (m_fDurabilityPerformance < 1.0f) )
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurabilityPerformance ) + 1.0f;
		}

		const SITEM* pItem = GLogicData::GetInstance().GetItem( itemCustom.nidDISGUISE );
		SITEMCUSTOM sCustomeItem(itemCustom.nidDISGUISE);
		if ( pItem && sCustomeItem.IsDisguise() )
		{
			fExpMultiple += pItem->GetExpMultiple() - 1.0f;
			fExpMultiple = max( fExpMultiple, 1.0f );
		}

		if ( fExpMultiple != 1.0f )
		{
			CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();

			_cutZero(strTemp,2);

			strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ));
			int nIndex = m_pAdditionalInformation->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

			strText = sc::string::format( "+%s%%", strTemp);
			m_pAdditionalInformation->AddString( nIndex, strText.c_str(), dwAddAbility );
		}

		//	특수기능 ( 변화량 )
		//
		if ( emITEM_VOL != EMVAR_NONE)
		{
			if ( emITEM_VOL == EMVAR_MOVE_SPEED )
			{
				float fVOLUME = itemCustom.GETMOVESPEED() * m_fDurabilityPerformance;
				CString strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				_cutZero(strTemp,4);
				strText = sc::string::format("%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ));
				int nIndex = m_pAdditionalInformation->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp);
				m_pAdditionalInformation->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
			else
			{
				float fVOLUME = (pItemData->sSuitOp.sVOLUME.fVolume * m_fDurabilityPerformance) + fCostumVolume;
				CString strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				_cutZero(strTemp,4);
				strText = sc::string::format("%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ));
				int nIndex = m_pAdditionalInformation->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp);
				m_pAdditionalInformation->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
		}
	}

	// Info : 저항
	{
		int nELEC   = static_cast<int>(itemCustom.GETRESIST_ELEC());
		int nFIRE   = static_cast<int>(itemCustom.GETRESIST_FIRE());
		int nICE    = static_cast<int>(itemCustom.GETRESIST_ICE());
		int nPOISON = static_cast<int>(itemCustom.GETRESIST_POISON());
		int nSPIRIT = static_cast<int>(itemCustom.GETRESIST_SPIRIT());

		if ( itemCustom.nidDISGUISE != NATIVEID_NULL() )
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem( itemCustom.nidDISGUISE );
			
			if( m_fDurabilityPerformance < 1.0f )
			{
				nELEC   = static_cast<int>( nELEC   * m_fDurabilityPerformance );
				nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityPerformance );
				nICE    = static_cast<int>( nICE    * m_fDurabilityPerformance );
				nPOISON = static_cast<int>( nPOISON * m_fDurabilityPerformance );
				nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityPerformance );
			}

			//코스튬 옵션 추가 - 코스튬 옵션은 내구도 영향을 받지 않음
			if ( pItem && ( itemCustom.IsDisguise() != NULL ) )
			{
				nELEC       += pItem->sSuitOp.sResist.nElectric;
				nFIRE       += pItem->sSuitOp.sResist.nFire;
				nICE        += pItem->sSuitOp.sResist.nIce;
				nPOISON     += pItem->sSuitOp.sResist.nPoison;
				nSPIRIT     += pItem->sSuitOp.sResist.nSpirit;
			}
		}
		else
		{
			if( m_fDurabilityPerformance < 1.0f )
			{
				nELEC   = static_cast<int>( nELEC   * m_fDurabilityPerformance );
				nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityPerformance );
				nICE    = static_cast<int>( nICE    * m_fDurabilityPerformance );
				nPOISON = static_cast<int>( nPOISON * m_fDurabilityPerformance );
				nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityPerformance );
			}
		}

		if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT )//||
			// nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
		{
			// 내구도 컬러
			DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;
			
			m_bDisplayResist = true;

			strText = sc::string::format("%s :", ID2GAMEWORD ("ITEM_BASIC_INFO", 12) );
			m_pAdditionalInformation->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );
			
			//0이라도 출력
			//if ( nSPIRIT )
			{
				strText = sc::string::format("%d", nSPIRIT );
				m_pResistSpiritText->AddText( strText.c_str(), dwAddAbility );
			}
			//if ( nFIRE )
			{
				strText = sc::string::format("%d", nFIRE );
				m_pResistFireText->AddText( strText.c_str(), dwAddAbility );
			}
			//if ( nICE )
			{
				strText = sc::string::format("%d", nICE );
				m_pResistIceText->AddText( strText.c_str(), dwAddAbility );
			}
			//if ( nELEC )
			{
				strText = sc::string::format("%d", nELEC );
				m_pResistElectricText->AddText( strText.c_str(), dwAddAbility );
			}
			//if ( nPOISON )
			{
				strText = sc::string::format("%d", nPOISON );
				m_pResistPoisonText->AddText( strText.c_str(), dwAddAbility );
			}
		}
		else
		{
			m_bDisplayResist = false;

			m_pResistSpiritText->ClearText();
			m_pResistFireText->ClearText();
			m_pResistIceText->ClearText();
			m_pResistElectricText->ClearText();
			m_pResistPoisonText->ClearText();
		}
	}
	//

}
void ItemInfoRebuildToolTip::_setItemRandomInformation(const SITEMCUSTOM& itemCustom, const SITEM* const pItemData)
{
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		m_pRandomInformation[_i]->m_pInformation->ClearText();

	DWORD dwRandomAbilityColor = ITEM_INFOR_COLOR::dwRandomAbility;
	//if( m_fDurabilityPerformance < 1.0f )
	//	dwRandomAbilityColor =  ItemDurability::GetInstance().GetTextColor(itemCustom, dwRandomAbilityColor);

	for ( DWORD _i(0); _i < RandomOpt::NSIZE; ++_i )
	{
		const int nType(itemCustom.getOptType(_i));
		if ( nType == RandomOption::RANDOM_OPTION_NULL )
			continue;

		const DWORD nCountFix(itemCustom.getOptCount(_i));
		float fValue;
		fValue = itemCustom.getOptValue(_i) * COMMENT::ITEM_RANDOM_OPT_SCALE[nType];
		const bool bPerRandOpt(itemCustom.IsPerRandOpt(nType));

		_setItemRandomInformation_makeText( nType, fValue, nCountFix, bPerRandOpt, dwRandomAbilityColor, _i, pItemData );
	}
}

void ItemInfoRebuildToolTip::_setItemRandomInformation_makeText( int nRandOptType, float fVal, const DWORD nCountFix, bool bIsPerRandOpt, DWORD dwAddAbility, const DWORD nIndex, const SITEM* const pItemData )
{
	std::string strDescText = "";

	if( m_fDurabilityPerformance < 1.0f )
		fVal = fVal * m_fDurabilityPerformance;


	if( (RandomOption::RANDOM_OPTION_NULL < nRandOptType) && (nRandOptType < RandomOption::RANDOM_OPTION_NSIZE) )
	{
		if ( fVal != 0.0f )
		{
			strDescText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );
			m_pRandomInformation[nIndex]->m_pInformation->AddText( strDescText.c_str(), ITEM_INFOR_COLOR::dwRandomAbility );

			strDescText = "";

			if ( fVal > 0.0f )
				strDescText += _T('+');
			if ( fVal < 0.0f )
				strDescText += _T('-');

			if ( bIsPerRandOpt )
			{
				CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
				_cutZero(strTemp,2);
				strDescText += sc::string::format("%s", strTemp);

				strDescText += _T('%');
			}
			else
			{
				strDescText += sc::string::format( "%d", static_cast<int>(fVal) );
			}

			const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
			if ( _bShowMinMax == true )
			{
				const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
				const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));
				if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
				{
					RandomOption::Information _information;
					const RandomOption::RANDOM_OPTION _typeRandomOption= RandomOption::RANDOM_OPTION(nRandOptType);
					if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, _typeRandomOption, _information) == true )
					{
						if ( bIsPerRandOpt == true )
						{
							// 왜 RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP 만 다르게 출력 되도록 디자인 한지는 모르겠다;
							if ( _typeRandomOption == RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP )
								strDescText += sc::string::format( "(%.2f~%.2f)", _information.valueFloatLow * 100.0f, _information.valueFloatHigh * 100.0f);
							else
								strDescText += sc::string::format( "(%.2f~%.2f)", _information.valueFloatLow, _information.valueFloatHigh);


// 							const float _fGap(_information.valueFloatHigh - _information.valueFloatLow);
// 							const float _fRate((fVal - _information.valueFloatLow) / _fGap);
// 
// 							if ( _fRate > 0.7f )
// 								strDescText += sc::string::format( " (High %.2f~%.2f)", _information.valueFloatLow, _information.valueFloatHigh);
// 							else if ( _fRate > 0.6f )
// 								strDescText += sc::string::format( " (Mid %.2f~%.2f)", _information.valueFloatLow, _information.valueFloatHigh);
// 							else
// 								strDescText += sc::string::format( " (Low %.2f~%.2f)", _information.valueFloatLow, _information.valueFloatHigh);
						}
						else
						{
// 							const float _fGap(float(_information.valueIntegerHigh - _information.valueIntegerLow));
// 							const float _fRate((fVal - float(_information.valueIntegerLow)) / _fGap);
// 
// 							if ( _fRate > 0.7f )
// 								strDescText += sc::string::format( " (High %d~%d)", _information.valueIntegerLow, _information.valueIntegerHigh);
// 							else if ( _fRate > 0.6f )
// 								strDescText += sc::string::format( " (Mid %d~%d)", _information.valueIntegerLow, _information.valueIntegerHigh);
// 							else
// 								strDescText += sc::string::format( " (Low %d~%d)", _information.valueIntegerLow, _information.valueIntegerHigh);

							strDescText += sc::string::format( "(%d~%d)", _information.valueIntegerLow, _information.valueIntegerHigh);
						}
					}					
				}
			}	

			m_pRandomInformation[nIndex]->m_pInformation->AddString(0, strDescText.c_str(), dwAddAbility );
			m_pRandomInformation[nIndex]->m_nCountFixOption = nCountFix;
		}
	}
}


void ItemInfoRebuildToolTip::_resizeInformationBox(void)
{
	enum
	{
		BASIC, ADDITIONAL, NSIZE
	};

	const int nLine[NSIZE] =
	{
		m_pBasicInformation->GetTotalLine(),
		m_pAdditionalInformation->GetTotalLine()
	};

	const float fInterval(5.0f);

	UIRECT uiPreviousGlobalSize;
	float fPreviousHeight = 0.0f;

	UIRECT uiBasicGlobalSize(m_pBasicInformation->GetGlobalPos());
	UIRECT uiBasicLocalSize(m_pBasicInformation->GetLocalPos());
	uiBasicLocalSize.top = 5.0f;
	const float fBasicHeight(m_pBasicInformation->CalcMaxHEIGHT(0, nLine[BASIC]));	
	if ( nLine[BASIC] != 0 )
	{	
		uiBasicGlobalSize.sizeY = uiBasicLocalSize.sizeY = fBasicHeight;		
		m_pBasicInformation->SetLocalPos(uiBasicLocalSize);
		m_pBasicInformation->SetGlobalPos(uiBasicGlobalSize);

		uiPreviousGlobalSize = uiBasicGlobalSize;
		fPreviousHeight = fBasicHeight;
	}	

	UIRECT uiAdditionalGlobalSize(m_pAdditionalInformation->GetGlobalPos());
	UIRECT uiAdditionalLocalSize(m_pAdditionalInformation->GetLocalPos());
	const float fAdditionalHeight(m_pAdditionalInformation->CalcMaxHEIGHT(0, nLine[ADDITIONAL]));
	if ( nLine[ADDITIONAL] != 0 )
	{	
		uiAdditionalGlobalSize.sizeY = uiAdditionalLocalSize.sizeY = fAdditionalHeight;
		uiAdditionalLocalSize.top = fAdditionalHeight;
		uiAdditionalGlobalSize.top = uiBasicGlobalSize.top + fBasicHeight + fInterval;
		m_pAdditionalInformation->SetLocalPos(uiAdditionalLocalSize);
		m_pAdditionalInformation->SetGlobalPos(uiAdditionalGlobalSize);

		uiPreviousGlobalSize = uiAdditionalGlobalSize;
		fPreviousHeight = fAdditionalHeight;
	}

	uiPreviousGlobalSize.top += 10.0f;
	for ( unsigned int _i(0); _i < RandomOpt::NSIZE; ++_i )
	{
		const int nLine(m_pRandomInformation[_i]->m_pInformation->GetTotalLine());
		if (  nLine == 0 )
		{
			m_pRandomInformation[_i]->SetVisibleSingle(FALSE);
			m_stateCheckBox -= (1 << _i); // 플래그 해제;
			continue;	
		}
		
		UIRECT uiRandomGlobalSize(m_pRandomInformation[_i]->m_pInformation->GetGlobalPos());
		UIRECT uiRandomLocalSize(m_pRandomInformation[_i]->m_pInformation->GetLocalPos());

		UIRECT uiRandomGlobalPosition(m_pRandomInformation[_i]->m_pCheckBox->GetGlobalPos());

		const float fAdditionalHeight(m_pRandomInformation[_i]->m_pInformation->CalcMaxHEIGHT(0, nLine));
		
		uiRandomGlobalSize.sizeY = uiRandomLocalSize.sizeY = fAdditionalHeight;
		uiRandomLocalSize.top = fAdditionalHeight;
		uiRandomGlobalSize.top = uiPreviousGlobalSize.top + fPreviousHeight + 1.0f;
		uiRandomGlobalPosition.top = uiPreviousGlobalSize.top + fPreviousHeight + 1.0f;

		m_pRandomInformation[_i]->m_pInformation->SetLocalPos(uiRandomLocalSize);
		m_pRandomInformation[_i]->m_pInformation->SetGlobalPos(uiRandomGlobalSize);
		m_pRandomInformation[_i]->m_pCheckBox->SetGlobalPos(uiRandomGlobalPosition);

		uiPreviousGlobalSize.top = uiRandomGlobalSize.top;
		fPreviousHeight = fAdditionalHeight;

		m_pRandomInformation[_i]->SetVisibleSingle(TRUE);		
	}

	if( true == m_bDisplayResist )
	{
		const float fResistOffset1 = 5.0f;
		const float fResistOffset2 = 3.0f;

		const UIRECT& AdditionalInfoGlobalSize = m_pAdditionalInformation->GetGlobalPos();
		const float fResistSizeX = m_pResistFire->GetLocalPos().sizeX;
		const float fResistSizeY = m_pResistFire->GetLocalPos().sizeY;

		int nTotalLine = m_pAdditionalInformation->GetTotalLine() - 2;

		D3DXVECTOR2 vPos;
		vPos.x = AdditionalInfoGlobalSize.right - fResistOffset1 * 14 - fResistSizeX * 10;
		vPos.y = AdditionalInfoGlobalSize.top + m_pAdditionalInformation->CalcMaxHEIGHT( 0, nTotalLine ) + 1.0f;

		vPos.x += fResistOffset1;
		vPos.y += fResistOffset2;
		m_pResistSpirit->SetLocalPos( vPos );
		m_pResistSpirit->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;
		vPos.y -= fResistOffset2;
		m_pResistSpiritText->SetLocalPos( vPos );
		m_pResistSpiritText->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;

		vPos.x += fResistOffset1;
		vPos.y += fResistOffset2;
		m_pResistFire->SetLocalPos( vPos );
		m_pResistFire->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;
		vPos.y -= fResistOffset2;
		m_pResistFireText->SetLocalPos( vPos );
		m_pResistFireText->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;

		vPos.x += fResistOffset1;
		vPos.y += fResistOffset2;
		m_pResistIce->SetLocalPos( vPos );
		m_pResistIce->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;
		vPos.y -= fResistOffset2;
		m_pResistIceText->SetLocalPos( vPos );
		m_pResistIceText->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;

		vPos.x += fResistOffset1;
		vPos.y += fResistOffset2;
		m_pResistElectric->SetLocalPos( vPos );
		m_pResistElectric->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;
		vPos.y -= fResistOffset2;
		m_pResistElectricText->SetLocalPos( vPos );
		m_pResistElectricText->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;

		vPos.x += fResistOffset1;
		vPos.y += fResistOffset2;
		m_pResistPoison->SetLocalPos( vPos );
		m_pResistPoison->SetGlobalPos( vPos );
		vPos.x += fResistSizeX;
		vPos.y -= fResistOffset2;
		m_pResistPoisonText->SetLocalPos( vPos );
		m_pResistPoisonText->SetGlobalPos( vPos );
	}

	m_pResistSpirit->SetVisibleSingle( m_bDisplayResist );
	m_pResistFire->SetVisibleSingle( m_bDisplayResist );
	m_pResistIce->SetVisibleSingle( m_bDisplayResist );
	m_pResistElectric->SetVisibleSingle( m_bDisplayResist );
	m_pResistPoison->SetVisibleSingle( m_bDisplayResist );
}

void ItemInfoRebuildToolTip::_cutZero(CString& cstr, int ncount)
{
	// '.' 점이 없다는 것은 소수점 이하 자리가 없으므로 리턴
	if (cstr.Find('.') == -1)
		return;

	CString temp = cstr;

	//ncount 소숫점 몇째 자리 까지 잘라낼찌 정한다
	for (int i = 0; i < ncount + 1; i++ )
	{
		temp = cstr.Right( 1 );

		if ( temp.Find('0') != -1 || temp.Find('.') != -1 )
			cstr = cstr.Left( cstr.GetLength() - 1 );
		else
			break;
	}
}

void ItemInfoRebuildToolTip::_updateShowCheckBox(const bool bShow)
{
	GLCharacter* const pCharacter(m_pGaeaClient->GetCharacter());
	pCharacter->RESET_FIX_OPTION();
	if ( bShow == true )
	{
		const DWORD nChecked(m_stateCheckBox.getNBit());
		const bool bCheckFull(nChecked == m_nMaxCheckable);

		if ( nChecked > m_nMaxCheckable )
		{
			for ( DWORD _i(RandomOpt::NSIZE); _i--; )
			{				
				m_pRandomInformation[_i]->resetCheck();
				m_pRandomInformation[_i]->showCheckBox();
					
				m_stateCheckBox -= (1 << _i); // 플래그 해제;
			}
		}
		else
		{
			for ( DWORD _i(RandomOpt::NSIZE); _i--; )
			{
				const bool& bChecked = m_pRandomInformation[_i]->isChecked();
				if ( bCheckFull == true && bChecked == false )
				{
					m_pRandomInformation[_i]->hideCheckBox();
					m_stateCheckBox -= (1 << _i); // 플래그 해제;
				}
				else
				{
					m_pRandomInformation[_i]->showCheckBox();
				}

				if ( bChecked == true )
				{
					m_stateCheckBox += (1 << _i); // 플래그 설정;
					pCharacter->PUSH_FIX_OPTION(m_ItemCustom.getOptType(_i), m_nMaxCheckable);
				}
				else
					m_stateCheckBox -= (1 << _i);
			}
		}
	}	
	else
	{		
		for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		{
			m_pRandomInformation[_i]->resetCheck();
			m_pRandomInformation[_i]->hideCheckBox();
			m_stateCheckBox.reset();
		}
	}
}
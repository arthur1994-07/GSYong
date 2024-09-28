#include "StdAfx.h"
#include "ProductGoods.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Product/GLProductClient.h"

#include "../Item/ItemImage.h"
#include "../Item/InventoryInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const int nLIMIT_PRODUCT_CNT = 2;
}

CProductGoods::CProductGoods( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface  )
: CBasicLineBoxSmart( pEngineDevice )
, m_pGaeaClient( pGaeaClient )
, m_pInterface( pInterface )
, m_pIsProductText( NULL )
, m_pProductTimeText( NULL )
, m_pProductMessageText( NULL )
, m_pProductCountText( NULL )
, m_nProductCount( 1 )
, m_pCostSumText( NULL )
, m_pMoneyText( NULL )
, m_pProductRecipe( NULL )
{
    ZeroMemory( m_pItemImageArray, sizeof(m_pItemImageArray) );
    ZeroMemory( m_pMaterialText, sizeof(m_pMaterialText) );
    ZeroMemory( m_nMaterialCount, sizeof(m_nMaterialCount) );
}

void CProductGoods::CreateSubControl()
{
    CBasicLineBoxSmart::CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    CString	strItemSlot[LIMIT_COLUMN_CNT] = 
    {
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_0",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_1",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_2",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_3",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_4",
    };
    CString	strItemSlotText[LIMIT_COLUMN_CNT] = 
    {
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_0",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_1",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_2",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_3",
        "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_4",
    };

    { // 상단 박스
        CBasicLineBoxSmart* pLinBoxSmartItemNameBox = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartItemNameBox->CreateSub( this, "PRODUCT_WINDOW_GOODS_ITEMNAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartItemNameBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAYLINE" );
        RegisterControl( pLinBoxSmartItemNameBox );

        { // 제조가능한가? 텍스트
            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_ISPRODUCT_TEXT", UI_FLAG_DEFAULT );
            pTextBox->SetFont ( pFont9 );
            RegisterControl ( pTextBox );
            m_pIsProductText = pTextBox;
            pTextBox = NULL;
        }

        { 
			//아이템 외곽선;
			CBasicLineBoxSmart* pLinBoxSmartMaterial = new CBasicLineBoxSmart( m_pEngineDevice );
			pLinBoxSmartMaterial->CreateSub( this, "PRODUCT_WINDOW_GOODS_RESULT_ITEMSLOT", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			pLinBoxSmartMaterial->CreateSubControl( "PRODUCT_WINDOW_GOODS_ITEMSLOT_BACK" );
			RegisterControl( pLinBoxSmartMaterial );

			// 결과 아이템;
            CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
            pItemImage->CreateSub( this, "PRODUCT_WINDOW_GOODS_RESULT_ITEMSLOT"
                , UI_FLAG_DEFAULT , ID_PRODUCT_GOODS_RESULT_ITEMSLOT );
            pItemImage->CreateSubControl();
            RegisterControl( pItemImage );
            m_pResulteItemImage = pItemImage;
        }

		{
			// 결과 아이템 텍스트라인 박스;
			CBasicLineBoxSmart* pLinBoxSmartMaterialText = new CBasicLineBoxSmart( m_pEngineDevice );
			pLinBoxSmartMaterialText->CreateSub( this, "PRODUCT_WINDOW_GOODS_RESULT_ITEMSLOT_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pLinBoxSmartMaterialText->CreateSubControl( "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_BACK" );
			RegisterControl( pLinBoxSmartMaterialText );

			// 결과 아이템 수량 텍스트;
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_RESULT_ITEMSLOT_TEXT", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );	
			pTextBox->SetTextAlign( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y  );
			RegisterControl ( pTextBox );
			m_pResultItemText = pTextBox;
		}

        { // 제조시간 몇초? 텍스트
            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_PRODUCTTIME_TEXT", UI_FLAG_DEFAULT );
            pTextBox->SetFont ( pFont9 );
            RegisterControl ( pTextBox );
            m_pProductTimeText = pTextBox;
        }

        { // 필요물품 타이틀 텍스트
            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_NEEDMATERIAL_TEXT", UI_FLAG_DEFAULT );
            pTextBox->SetFont ( pFont9 );
            pTextBox->SetTextAlign( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y  );
            RegisterControl ( pTextBox );

            std::string strNeedMaterial = sc::string::format( "- %s -" , ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 6 ) );
            pTextBox->SetText( strNeedMaterial.c_str(), NS_UITEXTCOLOR::GREENYELLOW );
        }

		//hsshin 숙련도 시스템 추가
		{ //필요제작레벨 텍스트
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_REQ_LEVEL", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );
			RegisterControl ( pTextBox );
			m_pReqLevelText = pTextBox;
			pTextBox = NULL;
		}
		{ //필요제작숙련도 텍스트
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_REQ_SKILL", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );
			RegisterControl ( pTextBox );
			m_pReqSkillText = pTextBox;
			pTextBox = NULL;
		}
		{ //획득숙련도 텍스트
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_GAIN_SKILL", UI_FLAG_DEFAULT );
			pTextBox->SetFont ( pFont9 );
			RegisterControl ( pTextBox );
			m_pGainSkillText = pTextBox;
			pTextBox = NULL;
		}
    }

    { // 재료들
        for ( int nCol = 0; nCol < LIMIT_COLUMN_CNT ; nCol++ )
        {
            //아이템 외곽선
            CBasicLineBoxSmart* pLinBoxSmartMaterial = new CBasicLineBoxSmart( m_pEngineDevice );
            pLinBoxSmartMaterial->CreateSub( this, strItemSlot[nCol].GetString(), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
            pLinBoxSmartMaterial->CreateSubControl( "PRODUCT_WINDOW_GOODS_ITEMSLOT_BACK" );
            RegisterControl( pLinBoxSmartMaterial );
            //아이템 슬롯
            CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
            pItemImage->CreateSub( this, strItemSlot[nCol].GetString(), UI_FLAG_DEFAULT, ID_PRODUCT_GOODS_ITEMSLOT_START+nCol );
            pItemImage->CreateSubControl();
            RegisterControl( pItemImage );
            m_pItemImageArray[nCol] = pItemImage;

            //재료아이템 텍스트라인 박스
            CBasicLineBoxSmart* pLinBoxSmartMaterialText = new CBasicLineBoxSmart( m_pEngineDevice );
            pLinBoxSmartMaterialText->CreateSub( this, strItemSlotText[nCol].GetString(), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
            pLinBoxSmartMaterialText->CreateSubControl( "PRODUCT_WINDOW_GOODS_ITEMSLOT_TEXT_BACK" );
            RegisterControl( pLinBoxSmartMaterialText );
            //재료아이템 수량 텍스트
            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub(this, strItemSlotText[nCol].GetString(), UI_FLAG_DEFAULT, ID_PRODUCT_GOODS_ITEMSLOT_TEXT_START+nCol );
            pTextBox->SetFont ( pFont9 );	
            pTextBox->SetTextAlign( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y  );
            RegisterControl ( pTextBox );
            m_pMaterialText[nCol] = pTextBox;
        }
    }

    { // 하단 라인박스, 텍스트박스
        CBasicLineBoxSmart* pLinBoxSmartItemNameBox = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartItemNameBox->CreateSub( this, "PRODUCT_WINDOW_GOODS_MIDDLE", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartItemNameBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAYLINE" );
        RegisterControl( pLinBoxSmartItemNameBox );

        //제조가능한가? 아닌가 메세지
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_MIDDLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( UI_FLAG_CENTER_X | UI_FLAG_CENTER_Y  );
        RegisterControl ( pTextBox );
        m_pProductMessageText = pTextBox;

    }

    { // 생산 수량
        CBasicLineBoxSmart* pLinBoxSmartItemNameBox = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartItemNameBox->CreateSub( this, "PRODUCT_WINDOW_GOODS_MIDDLE_LEFT", UI_FLAG_XSIZE | UI_FLAG_YSIZE , ID_PRODUCT_GOODS_PRODUCT_COUNT_BOX);
        pLinBoxSmartItemNameBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_YELLOWLINE" );
        RegisterControl( pLinBoxSmartItemNameBox );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_MIDDLE_LEFT", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );
        pTextBox->SetTextAlign( UI_FLAG_LEFT | UI_FLAG_CENTER_Y  );
        RegisterControl ( pTextBox );
        std::string strNeedMaterial = sc::string::format( " %s :" , ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 7 ) );
        pTextBox->SetText( strNeedMaterial.c_str(), NS_UITEXTCOLOR::WHITE );

        CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
        pEditBoxMan->CreateSub( this, "PRODUCT_WINDOW_GOODS_EDITBOX_MANAGER", UI_FLAG_DEFAULT, ID_PRODUCT_GOODS_EDITBOX_MANAGER );
        pEditBoxMan->CreateEditBox( ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT, "PRODUCT_WINDOW_GOODS_EDITBOX", "PRODUCT_WINDOW_GOODS_COUNT_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PRODUCT_CNT );
        RegisterControl( pEditBoxMan );
        m_pProductEditBox = pEditBoxMan;
        m_pProductEditBox->GetEditBox(ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT)->DoUSE_NUMBER(true);
    }

    { // 총 제작 비용
        CBasicLineBoxSmart* pLinBoxSmartItemNameBox = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartItemNameBox->CreateSub( this, "PRODUCT_WINDOW_GOODS_MIDDLE_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartItemNameBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAYLINE" );
        RegisterControl( pLinBoxSmartItemNameBox );

        //총 제작비용 텍스트
        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_MIDDLE_RIGHT", UI_FLAG_DEFAULT );
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( UI_FLAG_LEFT | UI_FLAG_CENTER_Y  );
        RegisterControl ( pTextBox );
        m_pCostSumText = pTextBox;
    }

    { // 소지금액
        CBasicLineBoxSmart* pLinBoxSmartItemNameBox = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartItemNameBox->CreateSub( this, "PRODUCT_WINDOW_GOODS_BOTTOM", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartItemNameBox->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAYLINE" );
        RegisterControl( pLinBoxSmartItemNameBox );

        CBasicButton* pButton = new CBasicButton( m_pEngineDevice );
        pButton->CreateSub( this, "PRODUCT_WINDOW_GOODS_MONEY_BUTTON", UI_FLAG_DEFAULT );
        RegisterControl( pButton );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "PRODUCT_WINDOW_GOODS_MONEY_TEXT" , UI_FLAG_DEFAULT);
        pTextBox->SetFont ( pFont9 );	
        pTextBox->SetTextAlign( UI_FLAG_LEFT | UI_FLAG_CENTER_Y );
        RegisterControl ( pTextBox );
        m_pMoneyText = pTextBox;
    }
}

void CProductGoods::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CBasicLineBoxSmart::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CProductGoods::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    if( !pCharacter )
        return;

    CUIGroup::TranslateUIMessage(ControlID, dwMsg);

    switch( ControlID )
    {
    case ID_PRODUCT_GOODS_RESULT_ITEMSLOT:
        {
            if( m_pProductRecipe && CHECK_MOUSE_IN( dwMsg ) )
            {
                SITEM* pItem = GLItemMan::GetInstance().GetItem( m_pProductRecipe->sResultItem.sNID );
                if( pItem == NULL)
                    return;

                SITEMCUSTOM sItemcustom( pItem->BasicId() );
                m_pInterface->SHOW_ITEM_INFO(
                    sItemcustom,
                    FALSE, FALSE, FALSE, TRUE, TRUE,
                    USHRT_MAX, USHRT_MAX );

                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                    {
                        if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                        {
                            m_pInterface->ItemPreviewWindowOpen( sItemcustom );
                            ResetMessageEx();
                        }                        
                    }
                }
                else if( CHECK_RB_UP_LIKE( dwMsg ) )
                {
                    if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                    {
                        if (pItem->BasicType() == ITEM_BOX || pItem->BasicType() == ITEM_RANDOMITEM )
                        {
                            m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemcustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );
                        }
                    }
                }
            }


        }
        break;

    case ID_PRODUCT_GOODS_ITEMSLOT_START:
    case ID_PRODUCT_GOODS_ITEMSLOT_START + 1:
    case ID_PRODUCT_GOODS_ITEMSLOT_START + 2:
    case ID_PRODUCT_GOODS_ITEMSLOT_START + 3:
    case ID_PRODUCT_GOODS_ITEMSLOT_START + 4:
        {
            if( m_pProductRecipe && CHECK_MOUSE_IN( dwMsg ) )
            {
                INT nMaterial = ControlID - ID_PRODUCT_GOODS_ITEMSLOT_START;
                SITEM* pItem = GLItemMan::GetInstance().GetItem( m_pProductRecipe->sMeterialItem[ nMaterial ].sNID );

                int nItemCnt = m_pProductRecipe->sMeterialItem[ nMaterial ].nNum*m_nProductCount;
                /*if( nItemCnt <= 0 )
                    return;*/

                if( pItem )
                {
                    m_pInterface->SHOW_ITEM_INFO(
                        SITEMCUSTOM( pItem->BasicId() ),
                        FALSE, FALSE, FALSE, TRUE, TRUE,
                        USHRT_MAX, USHRT_MAX );

					// 인벤토리 획득;
					GLInventory& Inven = m_pGaeaClient->GetCharacterData().m_cInventory;

					// 인벤토리의 현재 페이지 확인;
					int nViewItemCnt = 0;
					int nAllItemCnt = 0;
					BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *Inven.GetItemList() )
					{
						// 인벤 아이템;
						SINVENITEM* pInvenTurnItem = it.second;
						if ( !pInvenTurnItem )
							continue;

						int nX( pInvenTurnItem->wPosX );
						int nY( pInvenTurnItem->wPosY );

						const SITEM* pInvenItem = GLogicData::GetInstance().GetItem (
							pInvenTurnItem->sItemCustom.GetNativeID() );
						if( pInvenItem &&
							pItem->sBasicOp.sNativeID ==
							pInvenItem->sBasicOp.sNativeID )
						{
							m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );
							IInventory* pInventory =
								m_pInterface->GetInventory();

							// '소모되는 아이템'이 현재 페이지에 있을 경우;
							if ( pInventory && pInventory->IsCurrentInvenPage( nX, nY ) && nItemCnt > nAllItemCnt )
							{
								pInventory->SetBorderItemSlot( nX, nY );

								nViewItemCnt++;
								if ( nItemCnt <= nAllItemCnt )
									break;
							}

							nAllItemCnt += pInvenTurnItem->sItemCustom.TurnNum();
						}//if
					}//BOOST_FOREACH
					
					// 현재 페이지에 없을 경우 모든 페이지 확인;
					if ( !nViewItemCnt )
					{
						BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *Inven.GetItemList() )
						{
							// 인벤 아이템;
							SINVENITEM* pInvenTurnItem = it.second;
							if ( !pInvenTurnItem )
								continue;

							int nX( pInvenTurnItem->wPosX );
							int nY( pInvenTurnItem->wPosY );

							const SITEM* pInvenItem = GLogicData::GetInstance().GetItem (
								pInvenTurnItem->sItemCustom.GetNativeID() );
							if( pInvenItem &&
								pItem->sBasicOp.sNativeID ==
								pInvenItem->sBasicOp.sNativeID )
							{
								m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );
								IInventory* pInventory = m_pInterface->GetInventory();
								if ( pInventory )
								{
									pInventory->ChangeInvenPage( nX, nY );
									pInventory->SetBorderItemSlot( nX, nY );

									nViewItemCnt++;
									if ( nItemCnt <= nViewItemCnt )
										break;
								}
							}//if
						}//BOOST_FOREACH
					}
                }
            }
        }
        break;

    case ID_PRODUCT_GOODS_EDITBOX_MANAGER:
    case ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT:
        {
			IProductProgress* pProductProgress = m_pInterface->GetProductProgress();
			if( pProductProgress != NULL && pProductProgress->IsProdution() )
			{
				m_pProductEditBox->EndEdit();
				return;
			}

            UINT nLastCount = m_nProductCount;

            if( m_pProductEditBox->GetEditLength( ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT ) == 0 )
                m_nProductCount = 1;
            else
                m_nProductCount = _ttoi( m_pProductEditBox->GetEditString() );

            if( m_pProductRecipe )
            {
                BYTE nState;
                const WORD nProductCount = m_pGaeaClient->GetProductClient()->CountProduct( m_pProductRecipe, pCharacter, nState );
                if( m_nProductCount > nProductCount )
                {
                    m_nProductCount = nProductCount;
                    CString strCount;
                    strCount.Format( _T( "%d" ), m_nProductCount );

                    //m_pProductEditBox->BeginEdit();
                    //m_pProductEditBox->Init();
                    m_pProductEditBox->SetEditString( ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT, strCount.GetString() );
					//m_pResultItemText->SetText( strCount.GetString(), NS_UITEXTCOLOR::WHITE );
                    //m_pProductEditBox->EndEdit();
                }
            }

            if( nLastCount != m_nProductCount )
			{
                SetMaterials();

				//CString strCount;
				//strCount.Format( _T( "%d" ), m_nProductCount );
				//m_pResultItemText->SetText( strCount.GetString(), NS_UITEXTCOLOR::WHITE );
			}
        }
        break;

    case ID_PRODUCT_GOODS_PRODUCT_COUNT_BOX:
        {
            if( CHECK_MOUSE_IN( dwMsg ) == FALSE )
            {
                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    m_pProductEditBox->EndEdit();
                }
            }
        }
        break;
    }

    if( m_pProductRecipe )
    {

        //! 기획 변경에 따라 총 제작비용이 제작비용으로 변경되었다.
        //UINT nProductCount = max( m_nProductCount, 1 );

        LONGLONG llProductCost=0;
        //llProductCost += m_pProductRecipe->dwPrice * nProductCount;
        llProductCost += m_pProductRecipe->dwPrice;

        CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llProductCost, 3, "," );
        std::string strCostSum = sc::string::format( " %s : %s" , ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 8 ), strTemp );

        m_pCostSumText->SetText( strCostSum.c_str(), NS_UITEXTCOLOR::ORANGE );
    }

    // 인벤에 골드 표시 실시간으로 체크
    CString strTemp = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( pCharacter->GetInvenMoney(), 3, "," );
    m_pMoneyText->SetText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
}

void CProductGoods::ShowRandomOptionMaterialBorder()
{
	if( !m_pProductRecipe )
		return;

	for ( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
        SITEM* pItem = GLItemMan::GetInstance().GetItem( m_pProductRecipe->sMeterialItem[ i ].sNID );

        int nItemCnt = m_pProductRecipe->sMeterialItem[ i ].nNum/**m_nProductCount*/;
        /*if( nItemCnt <= 0 )
            return;*/

        if( pItem )
        {
			// 인벤토리 획득;
			GLInventory& Inven = m_pGaeaClient->GetCharacterData().m_cInventory;

			// 인벤토리의 현재 페이지 확인;
			int nViewItemCnt = 0;
			int nAllItemCnt = 0;
			BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *Inven.GetItemList() )
			{
				// 인벤 아이템;
				SINVENITEM* pInvenTurnItem = it.second;
				if ( !pInvenTurnItem )
					continue;

				int nX( pInvenTurnItem->wPosX );
				int nY( pInvenTurnItem->wPosY );

				const SITEM* pInvenItem = GLogicData::GetInstance().GetItem (
					pInvenTurnItem->sItemCustom.GetNativeID() );
				if( pInvenItem &&
					pItem->sBasicOp.sNativeID == pInvenItem->sBasicOp.sNativeID )
				{
					IInventory* pInventory =
						m_pInterface->GetInventory();

					// '소모되는 아이템'이 현재 페이지에 있을 경우;
					if ( pInventory &&
						pInventory->IsCurrentInvenPage( nX, nY ) &&
						nItemCnt > nAllItemCnt )
					{
						if ( pInvenTurnItem->sItemCustom.IsSetRandOpt() )
						{
							m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );

							pInventory->SetBorderItemSlot( nX, nY );
						}

						nViewItemCnt++;
						if ( nItemCnt <= nAllItemCnt )
							break;
					}

					nAllItemCnt += pInvenTurnItem->sItemCustom.TurnNum();
				}//if
			}//BOOST_FOREACH
			
			// 현재 페이지에 없을 경우 모든 페이지 확인;
			if ( !nViewItemCnt )
			{
				BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *Inven.GetItemList() )
				{
					// 인벤 아이템;
					SINVENITEM* pInvenTurnItem = it.second;
					if ( !pInvenTurnItem )
						continue;

					int nX( pInvenTurnItem->wPosX );
					int nY( pInvenTurnItem->wPosY );

					const SITEM* pInvenItem = GLogicData::GetInstance().GetItem (
						pInvenTurnItem->sItemCustom.GetNativeID() );
					if( pInvenItem &&
						pItem->sBasicOp.sNativeID == pInvenItem->sBasicOp.sNativeID )
					{
						IInventory* pInventory = m_pInterface->GetInventory();
						if ( pInventory )
						{
							if ( pInvenTurnItem->sItemCustom.IsSetRandOpt() )
							{
								m_pInterface->UiShowGroupFocus( INVENTORY_WINDOW_RENEW );

								pInventory->ChangeInvenPage( nX, nY );
								pInventory->SetBorderItemSlot( nX, nY );
							}

							nViewItemCnt++;
							if ( nItemCnt <= nViewItemCnt )
								break;
						}
					}//if
				}//BOOST_FOREACH
			}
        }
    }
}

void CProductGoods::SetItemInformation( SProductRecipe* pProductRecipe )
{
    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    if( !pCharacter )
        return;
    GLProductClient* pProduct = m_pGaeaClient->GetProductClient();

    if( m_pProductRecipe != pProductRecipe )
    {
        m_pProductEditBox->BeginEdit();
        m_pProductEditBox->Init();
        m_pProductEditBox->SetEditString( ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT, "1" );
        m_pProductEditBox->EndEdit();
        m_pProductRecipe = pProductRecipe;    
    }

    if( m_pProductRecipe == NULL )
    {
        Clear();
    }

    m_pIsProductText->SetVisibleSingle( FALSE );
    m_pReqLevelText->SetVisibleSingle( FALSE );
    m_pReqSkillText->SetVisibleSingle( FALSE );
    m_pGainSkillText->SetVisibleSingle( FALSE );

    // 상단 생산물과 텍스트들
    {
        if( pProduct->CheckProductLearn( m_pProductRecipe ) == false ) //조합법을 미습득시
        {
            m_pIsProductText->SetText( ID2GAMEWORD( "PRODUCT_WINDOW_TEXT", 4 ), NS_UITEXTCOLOR::RED);
            m_pIsProductText->SetVisibleSingle( TRUE );
        }
        else
        {
            //레벨체크가 필요하면서 필요레벨이 부족하다면
            if ( pProduct->CheckProductLevel( m_pProductRecipe, pCharacter->m_wLevel ) == false )
            {
                m_pReqLevelText->SetText( sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 11 ), m_pProductRecipe->wLevel ).c_str(), NS_UITEXTCOLOR::RED );
                m_pReqLevelText->SetVisibleSingle( TRUE );
            }
            //숙련도체크가 필요하면서 필요숙련이 부족하다면
            if ( pProduct->CheckProductSkill( m_pProductRecipe ) == false )
            {
                m_pReqSkillText->SetText( sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 12 ), m_pProductRecipe->wWs ).c_str(), NS_UITEXTCOLOR::RED );
                m_pReqSkillText->SetVisibleSingle( TRUE );
            }
            //경험치를 얻을 수 있는 조합법이면서 경험치 획득을 한적이 없다면
            if (m_pProductRecipe->wWsp != Product::PRODUCT_NOT_USE_REQ
                && m_pGaeaClient->GetProductClient()->IsExpProductRecipe( m_pProductRecipe->dwKey ) == false )
            {
                m_pGainSkillText->SetText( sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 13 ), m_pProductRecipe->wWsp , m_pProductRecipe->wWspPer ).c_str(), NS_UITEXTCOLOR::ORANGE );
                m_pGainSkillText->SetVisibleSingle( TRUE );
            }
        }
        //결과 아이템 표시
        SITEM* pItem = GLItemMan::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
        if( pItem )
        {
            m_pResulteItemImage->ResetItem();
            m_pResulteItemImage->SetItem( pItem->sBasicOp.sNativeID );
        }

        //제작시간
		/*
        std::string strProductTime = ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 5 );
        std::string strProductTimeText = sc::string::format( " %d %s" , m_pProductRecipe->fDelaySec, ID2GAMEWORD("ITEM_BASIC_TIME", 2 ) );
        m_pProductTimeText->SetText( strProductTime.c_str() );
        m_pProductTimeText->AddString( 0, strProductTimeText.c_str(), NS_UITEXTCOLOR::ORANGE );*/

		/*std::string strProductTime = sc::string::format( ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 15 ) , m_pProductRecipe->dwRate );
		m_pProductTimeText->SetText( strProductTime.c_str(), NS_UITEXTCOLOR::ORANGE );*/
    }

    std::string strProductMessage;
    DWORD nColor = 0;   //제작 가능여부를 나타내는 텍스트의 폰트색깔
    BYTE eState;
    if ( pProduct->CheckProduct( m_pProductRecipe, pCharacter, eState , 1 ) )  //제작 가능시
    {
        if( pProductRecipe->dwRate == Product::PRODUCT_NOT_USE_REQ )
        {
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 2 );
        }
        else
        {
            strProductMessage = sc::string::format(ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 1 ), pProductRecipe->dwRate);   //확률제작시
        }   
        nColor = D3DCOLOR_XRGB( 0x0B, 0xFF, 0x00 );
        m_pIsProductText->SetText( ID2GAMEWORD( "PRODUCT_WINDOW_TEXT", 3 ), nColor);
        m_pIsProductText->SetVisibleSingle( TRUE );

		std::string strProductTime = ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 5 );
		std::string strProductTimeText = sc::string::format( " %d %s" , m_pProductRecipe->fDelaySec, ID2GAMEWORD("ITEM_BASIC_TIME", 2 ) );
		m_pProductTimeText->SetText( strProductTime.c_str() );
		m_pProductTimeText->AddString( 0, strProductTimeText.c_str(), NS_UITEXTCOLOR::ORANGE );
    }
    else    //제작 불가시
    {
        switch( eState )
        {
        case Product::EStateNothing:
        case Product::EStatePart:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 0 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
        case Product::EStateNotLearn:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 3 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x00, 0x00 );
            break;
        case Product::EStateNotMoney:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 4 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
        case Product::EStateLevelFail:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 5 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
        case Product::EStateSkillFail:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 6 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
        case Product::EStateLevelAndSkillFail:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 7 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
        case Product::EStateTypeFail:
            strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 8 );
            nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
            break;
		case Product::EStateInven:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 9 );
			nColor = D3DCOLOR_XRGB( 0xFF, 0x85, 0x00 );
			break;
        }
		
		std::string strProductTime = ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 15 );
		std::string strProductTimeText = sc::string::format( " %d%%" , m_pProductRecipe->dwRate );
		m_pProductTimeText->SetText( strProductTime.c_str() );
		m_pProductTimeText->AddString( 0, strProductTimeText.c_str(), NS_UITEXTCOLOR::ORANGE );
		
    }
    m_pProductMessageText->SetText( strProductMessage.c_str(), nColor );

    SetMaterials();
}
 
void CProductGoods::SetMaterials()
{
    for( int i=0; i<LIMIT_COLUMN_CNT ; i++ )
    {
        m_pItemImageArray[ i ]->ResetItem();
        m_pMaterialText[ i ]->ClearText();
    }

    if( !m_pProductRecipe )
        return;

    GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
    if( !pCharacter )
        return;

    //UINT nProductCount = max( m_nProductCount, 1 );

	// 결과 아이템 개수 세팅;
	CString strCount;
	strCount.Format( _T( "%d" ), m_pProductRecipe->sResultItem.nNum );
	m_pResultItemText->SetText( strCount.GetString(), NS_UITEXTCOLOR::WHITE );

	// 재료 아이템 세팅;
    for( int i=0; i<LIMIT_COLUMN_CNT ; i++ )
    {
        const SProductItem& sProductItem = m_pProductRecipe->sMeterialItem[ i ];

        int nItemCnt = sProductItem.nNum;
        if( nItemCnt <= 0 )
            continue;

        SITEM* pItem = GLItemMan::GetInstance().GetItem( sProductItem.sNID );
        if( !pItem )
            continue;

        m_pItemImageArray[ i ]->SetItem( pItem->sBasicOp.sNativeID );

        CString strCntCnt;
        DWORD nMaterial = pCharacter->m_cInventory.CountTurnItem( sProductItem.sNID );
        //strCntCnt.Format( "%d/%d", nMaterial, nItemCnt * nProductCount ); 
        strCntCnt.Format( "%d/%d", nMaterial, nItemCnt );

        DWORD dwTextColor = NS_UITEXTCOLOR::WHITE;
        //if( nMaterial < nItemCnt * nProductCount )
        if( static_cast<int>(nMaterial) < nItemCnt )
            dwTextColor = NS_UITEXTCOLOR::RED;

        m_pMaterialText[ i ]->SetText( strCntCnt.GetString(), dwTextColor );
    }
}

void CProductGoods::DoStart( bool bOngoing )
{
    if( !m_pProductRecipe )
        return;

	GLProductClient* pProduct = m_pGaeaClient->GetProductClient();	//hsshin
    if( pProduct )
		pProduct->ReqProduct( m_pProductRecipe->dwKey, m_nProductCount, bOngoing );
}

void CProductGoods::DoCancel()
{
	GLProductClient* pProduct = m_pGaeaClient->GetProductClient();	//hsshin
	if( pProduct )
		pProduct->ReqProductCancel();
}

void CProductGoods::Clear()
{
    m_pProductRecipe = NULL;

    m_pIsProductText->ClearText();
    m_pResulteItemImage->ResetItem();
	m_pResultItemText->ClearText();
    m_pProductTimeText->ClearText();
    m_pProductMessageText->ClearText();
    m_pReqLevelText->ClearText();
    m_pReqSkillText->ClearText();
    m_pGainSkillText->ClearText();
    m_pCostSumText->ClearText();

    for( int i=0; i<LIMIT_COLUMN_CNT ; i++ )
    {
        m_pItemImageArray[ i ]->ResetItem();
        m_pMaterialText[ i ]->ClearText();
    }
}

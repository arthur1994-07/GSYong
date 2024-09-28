#include "StdAfx.h"
#include "../Item/ItemSlot.h"
#include "../Item/ItemImage.h"
#include "../../InnerInterface.h"
#include "ItemMove.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../Feature/CostumColorInfo.h"
#include "../Tutorial/ExplainInterface.h"
#include "../Item/InventoryInterface.h"
#include "../Item/InventoryWindow.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CItemSlot::nOUTOFRANGE = -1;

CItemSlot::CItemSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMouseOver ( NULL )
    , m_nMaxColumn ( 0 )
    , m_bBLOCK ( false )
	, m_bAutoBorder ( true )
	, m_bExchangeSlot( false )
	, m_pWindowOpenBox( NULL )
{

	SecureZeroMemory ( m_pEmphasisSlot, sizeof( m_pEmphasisSlot ) );
	SecureZeroMemory ( m_pNumberBoxArray, sizeof( m_pNumberBoxArray ) );
	SecureZeroMemory ( m_pItemImageArray, sizeof( m_pItemImageArray ) );
	SecureZeroMemory ( m_pBlockImage, sizeof( m_pBlockImage ) );
	SecureZeroMemory( m_pColorInfo, sizeof( m_pColorInfo ) );
}

CItemSlot::~CItemSlot ()
{
}

void CItemSlot::CreateSubControl ( int nMaxColumn, BOOL bNumberUse )
{
	m_nMaxColumn = nMaxColumn;

	{	//	밑바닥 배경
		CString	strSlotBack[nLIMIT_COLUMN] = 
		{
			"ITEM_IMAGE_BACK0",
			"ITEM_IMAGE_BACK1",
			"ITEM_IMAGE_BACK2",
			"ITEM_IMAGE_BACK3",
			"ITEM_IMAGE_BACK4",
			"ITEM_IMAGE_BACK5",
			"ITEM_IMAGE_BACK6",
			"ITEM_IMAGE_BACK7",
			"ITEM_IMAGE_BACK8",
			"ITEM_IMAGE_BACK9"
		};

		CString	strColorBack[nLIMIT_COLUMN] = 
		{
				"COLOR1_INFO_BOX0",
				"COLOR1_INFO_BOX1",
				"COLOR1_INFO_BOX2",
				"COLOR1_INFO_BOX3",
				"COLOR1_INFO_BOX4",
				"COLOR1_INFO_BOX5",
				"COLOR1_INFO_BOX6",
				"COLOR1_INFO_BOX7",
				"COLOR1_INFO_BOX8",
				"COLOR1_INFO_BOX9"
		};

		for ( int i = 0; i < m_nMaxColumn; i++ )
		{
			m_pItemImageArray[ i ] = CreateItemImage ( strSlotBack[i].GetString (), ITEM_IMAGE0 + i );

			CUIControl* pBlockX = new CUIControl( m_pEngineDevice );
			pBlockX->CreateSub( m_pItemImageArray[ i ], "ITEM_SLOT_BLOCK_X" );
			pBlockX->SetVisibleSingle( FALSE );
			m_pItemImageArray[ i ]->RegisterControl( pBlockX );
			m_pBlockImage[ i ] = pBlockX;

			m_pColorInfo[ i ] = CreateColorInfo( strColorBack[i].GetString(), ITEM_COLOR_INFO0 + i );
			m_pColorInfo[ i ]->SetVisibleSingle( FALSE );
		}
	}

	if ( bNumberUse )
	{
		CreateNumberBox ();
	}

	m_pMouseOver = new CUIControl(m_pEngineDevice);
	m_pMouseOver->CreateSub ( this, "ITEM_MOUSE_OVER" );
	RegisterControl ( m_pMouseOver );

	for ( int i = 0; i < nLIMIT_COLUMN; ++i )
	{
		m_pEmphasisSlot[ i ] = new CUIControl(m_pEngineDevice);
		m_pEmphasisSlot[ i ]->CreateSub ( this, "ITEM_MOUSE_OVER" );
		m_pEmphasisSlot[ i ]->SetDiffuse( NS_UITEXTCOLOR::GREEN );
		m_pEmphasisSlot[ i ]->SetVisibleSingle( FALSE );
		RegisterControl ( m_pEmphasisSlot[ i ] );
	}

	m_pBlock = new CUIControl(m_pEngineDevice);
	m_pBlock->CreateSub ( this, "ITEM_SLOT_BLOCK" );
	RegisterControl ( m_pBlock );

	m_pWindowOpenBox = new CUIControl(m_pEngineDevice);
	m_pWindowOpenBox->CreateSub ( this, "ITEM_MOUSE_OVER" );
	m_pWindowOpenBox->SetDiffuse( NS_UITEXTCOLOR::GREEN );
	m_pWindowOpenBox->SetVisibleSingle( FALSE );
	RegisterControl ( m_pWindowOpenBox );
}

CItemImage*	CItemSlot::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage(m_pGaeaClient, m_pEngineDevice);
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

CCostumColorInfo* CItemSlot::CreateColorInfo( const char* szControl, UIGUID ControlID )
{
	CCostumColorInfo* pItemColorInfo = new CCostumColorInfo(m_pEngineDevice);
	pItemColorInfo->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemColorInfo->CreateSubControl ();
	RegisterControl ( pItemColorInfo );

	return pItemColorInfo;
}

void CItemSlot::ResetItemImage ( int nIndex )
{
	if ( nIndex < 0 || m_nMaxColumn <= nIndex )
	{
//		GASSERT ( 0 && "범위를 넘어섭니다." );
		return ;
	}

	CItemImage* pItem = m_pItemImageArray[nIndex];
	pItem->ResetItem ();
	
	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( m_InvenItem[nIndex].sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
		&& m_InvenItem[nIndex].sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
	{
		m_pColorInfo[ nIndex ]->SetVisibleSingle( false );
	}

	m_InvenItem[nIndex].sItemCustom = SITEMCUSTOM( NATIVEID_NULL() );
}

void CItemSlot::SetItemImage ( int nIndex, SINVENITEM& ref_InvenItem )
{
	if ( nIndex < 0 || m_nMaxColumn <= nIndex )
	{
//		GASSERT ( 0 && "범위를 넘어섭니다." );
		return ;
	}

	SITEMCUSTOM &sItemCustom = ref_InvenItem.sItemCustom;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		GASSERT ( 0 && "등록되지 않은 ID입니다." );
		return ;
	}
    
	CItemImage* pItemImage = m_pItemImageArray[nIndex];
	pItemImage->SetItem ( sItemCustom );

	m_InvenItem[nIndex] = ref_InvenItem;

	// Note : 코스츔 염색 컬러 정보. by luxes.
	if ( sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
		&& sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
	{
		m_pColorInfo[ nIndex ]->SetVisibleSingle( TRUE );

		m_pColorInfo[ nIndex ]->SetSelectColor( sItemCustom.dwMainColor, sItemCustom.dwSubColor );
	}
}

void CItemSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    SetItemIndex ( nOUTOFRANGE );

	if ( m_bAutoBorder )
	{
		m_pMouseOver->SetVisibleSingle ( FALSE );

		for ( int i = 0; i < nLIMIT_COLUMN; ++i )
			m_pEmphasisSlot[ i ]->SetVisibleSingle ( FALSE );
	}
    
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pNumberBoxArray[0] )
	{
		for ( int i = 0; i < m_nMaxColumn; i++ )
		{
			ResetNumber ( i );

			SNATIVEID sNativeID = m_InvenItem[i].sItemCustom.GetNativeID();
			if ( sNativeID != NATIVEID_NULL () )
			{	
				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
				if ( pItemData )
				{
					const WORD wPileNum = pItemData->sDrugOp.wPileNum;
					const WORD wTurnNum = m_InvenItem[i].sItemCustom.wTurnNum;						

					//	NOTE
					//		겹쳐짐 개수 표시
					//
					if ( wPileNum>1 || wTurnNum>1 )
					{
						SetNumber ( i, wTurnNum, wPileNum );
					}


				}

				if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
					m_pGaeaClient->GetTutorial()->IsSubThreeStep() )
				{
					if ( m_pGaeaClient->GetTutorial()->m_sItemID == sNativeID)
					{	// 튜토리얼 중 장착해야 할 스킬 아이콘이 보이면, UI 그린다.
						//const UIRECT& rcSlotLocalPos = m_pNumberBoxArray[i]->GetGlobalPos();
						const UIRECT& rcSlotLocalPos = m_pItemImageArray[ i ]->GetGlobalPos();
						D3DXVECTOR2 vPos;
						vPos.x = rcSlotLocalPos.left;
						vPos.y = rcSlotLocalPos.top;

						UIRECT rcInvenPos;
						if( m_pInterface->GetInventory() )
							rcInvenPos = m_pInterface->GetInventory()->GetUIWindowGlobalPos();
						else
							rcInvenPos = m_pInterface->GetInventoryWindow()->GetUIWindowGlobalPos();

						IExplainInterface* pExplain = m_pInterface->GetExplainInterface();
						pExplain->SetSlotBoxOriginPos( vPos, 0 );
						pExplain->SetSlotBoxOriginPos( rcInvenPos, 0 );
						pExplain->SetSlotBoxVisibleAtInven(TRUE);
					}
				}
			}

			// Note : 코스츔 염색 컬러 정보. by luxes.
			if ( m_InvenItem[i].sItemCustom.GetNativeID() != NATIVEID_NULL () )
			{
				if ( m_InvenItem[ i ].sItemCustom.dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
					&& m_InvenItem[ i ].sItemCustom.dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
				{
					m_pColorInfo[ i ]->SetVisibleSingle( TRUE );

					m_pColorInfo[ i ]->SetSelectColor( m_InvenItem[i].sItemCustom.dwMainColor, m_InvenItem[i].sItemCustom.dwSubColor );
				}
				else
				{
					m_pColorInfo[ i ]->SetVisibleSingle( FALSE );
				}
			}
			else
				m_pColorInfo[ i ]->SetVisibleSingle( FALSE );

		}
	}
}

void CItemSlot::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ITEM_IMAGE0:
	case ITEM_IMAGE1:
	case ITEM_IMAGE2:
	case ITEM_IMAGE3:
	case ITEM_IMAGE4:
	case ITEM_IMAGE5:
	case ITEM_IMAGE6:
	case ITEM_IMAGE7:
	case ITEM_IMAGE8:
	case ITEM_IMAGE9:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_nIndex = ControlID - ITEM_IMAGE0;			

				if ( !m_pInterface->IsFirstItemSlot () ) return ;								

				//	마우스 표시 테두리
				if ( m_pItemImageArray[m_nIndex]->GetItemImageSlotPos () != NATIVEID_NULL () &&
					m_bAutoBorder )
				{
					const UIRECT& rcImagePos = m_pItemImageArray[m_nIndex]->GetGlobalPos ();
					const UIRECT& rcImageLocalPos = m_pItemImageArray[m_nIndex]->GetLocalPos ();
					m_pMouseOver->SetLocalPos ( rcImageLocalPos );
					m_pMouseOver->SetGlobalPos ( rcImagePos );
					m_pMouseOver->SetVisibleSingle ( TRUE );
				}

				//	스냅, 스킬 이미지 붙이기
				IItemMove* pItemMove = m_pInterface->GetItemMove();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemImageArray[m_nIndex]->GetGlobalPos ();
					pItemMove->SetUIWindowGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_MOUSEIN_ITEMSLOT | UIMSG_TOTOPPARENT );
				}

                //bjju.ItemPreviewWindow 미리보기 세팅
                if ( dwMsg & UIMSG_LB_UP )
                {
                    if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                    {
                        SITEMCUSTOM& sItemcustom = m_InvenItem[m_nIndex].sItemCustom;

                        if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemcustom ) )
                        {
                            m_pInterface->ItemPreviewWindowOpen( sItemcustom );

                            // 클릭 메세지 삭제
                            //if ( GetParent() )
                            //{
                            //    DWORD dwMsg = GetMessageEx();
                            //    dwMsg &= ~UIMSG_LB_UP; 
                            //    dwMsg &= ~UIMSG_LB_DOWN; 
                            //    dwMsg &= ~UIMSG_LB_PRESEED;
                            //    GetParent()->SetMessageEx( dwMsg );
                            //}
                            
                            // 위 코드에서 메시지가 정상적으로 삭제되지 않아 수정 2012.7.13 bjkim
                            ResetMessageEx();
                        }                        
                    }
					AddMessageEx ( UIMSG_LBMUP_ITEMSLOT | UIMSG_TOTOPPARENT );
                }
                else if ( CHECK_RB_UP_LIKE(dwMsg) )
                {
                    if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
                    {
                        SITEMCUSTOM& sItemCustom = m_InvenItem[m_nIndex].sItemCustom;

                        const SITEM* pItem = GLItemMan::GetInstance().GetItem(sItemCustom.GetNativeID());
						if (!pItem)
							return;
						
						SSETITEM* pSetItem = NULL;
                        if (pItem->BasicType() == ITEM_BOX || 
							pItem->BasicType() == ITEM_RANDOMITEM ||
							pItem->BasicType() == ITEM_SELECTIVEFORM_BOX)
                        {
							if( !m_bExchangeSlot ) {
								m_pInterface->GetInvenItemTooltipGroup()->ShowItemToolTipMulti( sItemCustom, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );
								ResetMessageEx();
							}
							else
							{
								if( GetParent() ) {
									GetParent()->TranslateUIMessage( GetWndID(), UIMSG_EXCHANGE_LIST );
									ResetMessageEx();
								}
							}
                        }
						else if ( (pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItem->BasicId()))) )
						{
							STARGETID sTargetID(CROW_ITEM, pItem->BasicId().dwID);
							m_pInterface->SetitemGroupComposition(pSetItem, sTargetID);
							m_pInterface->OpenSetitemPreviewWindow();
							ResetMessageEx();
						}
						else
						{
							if( GetParent() ) {
								GetParent()->TranslateUIMessage( GetWndID(), UIMSG_EXCHANGE_LIST );
								ResetMessageEx();
							}
						}
                    }
                }
			}
		}
		break;
	};
}

CBasicTextBox* CItemSlot::CreateNumberBox ( const char* szControl )
{
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, szControl );
    pTextBox->SetFont ( pFont8 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pTextBox );

	return pTextBox;
}

void CItemSlot::SetNumber ( int nIndex, int nNumber, int nMaxNumber )
{
	CString strNumber;
	strNumber.Format ( "%d", nNumber );
	DWORD dwColor = NS_UITEXTCOLOR::GREENYELLOW;
	if ( nNumber == nMaxNumber ) dwColor = NS_UITEXTCOLOR::ORANGE;
	m_pNumberBoxArray[nIndex]->SetOneLineText ( strNumber, dwColor );
	m_pNumberBoxArray[nIndex]->SetVisibleSingle ( TRUE );
}

void CItemSlot::ResetNumber ( int nIndex )
{
	m_pNumberBoxArray[nIndex]->ClearText ();
	m_pNumberBoxArray[nIndex]->SetVisibleSingle ( FALSE );
}

void CItemSlot::CreateNumberBox ()
{
	{	//	밑바닥 배경
		CString	strSlotBack[nLIMIT_COLUMN] = 
		{
			"ITEM_NUMBER0",
			"ITEM_NUMBER1",
			"ITEM_NUMBER2",
			"ITEM_NUMBER3",
			"ITEM_NUMBER4",
			"ITEM_NUMBER5",
			"ITEM_NUMBER6",
			"ITEM_NUMBER7",
			"ITEM_NUMBER8",
			"ITEM_NUMBER9"
		};

		for ( int i = 0; i < m_nMaxColumn; i++ )
		{
			m_pNumberBoxArray[i] = CreateNumberBox ( strSlotBack[i].GetString () );			
		}
	}
}

void	CItemSlot::SetBLOCK ( bool bXImage )
{
	m_bBLOCK = true;

	if( bXImage )
	{
		for( int i=0; i<m_nMaxColumn; ++i )
			m_pBlockImage[ i ]->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pBlock->SetUseRender ( TRUE );
		m_pBlock->SetVisibleSingle ( TRUE );
		m_pBlock->SetDiffuse (D3DCOLOR_ARGB(125,0,0,0));
	}
}

void	CItemSlot::ResetBLOCK ( bool bXImage )
{
	m_bBLOCK = false;

	if( bXImage )
	{
		for( int i=0; i<m_nMaxColumn; ++i )
			m_pBlockImage[ i ]->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pBlock->SetUseRender ( FALSE );
		m_pBlock->SetVisibleSingle ( FALSE );
	}
}

const SINVENITEM* CItemSlot::GetItemImage(int nIndex) const
{
    if( nIndex < 0 || nIndex >= nLIMIT_COLUMN )
        return NULL;
    else
        return &m_InvenItem[nIndex];
}

CItemImage* CItemSlot::GetItemImageStruct( int nColumn )
{
    if ( nColumn < 0 || nColumn >= nLIMIT_COLUMN )
        return NULL;

    return m_pItemImageArray[ nColumn ];
}

void CItemSlot::SetDiffuse( int nIndex, DWORD nColor )
{
	if( nIndex < 0 || nIndex >= nLIMIT_COLUMN )
        return;

	m_pItemImageArray[ nIndex ]->SetDiffuse( nColor );
}

void CItemSlot::SetDiffuseAlpha( int nIndex, BYTE nAlpha )
{
	if( nIndex < 0 || nIndex >= nLIMIT_COLUMN )
        return;

	m_pItemImageArray[ nIndex ]->SetDiffuseAlpha( WORD( nAlpha ) );
}

void CItemSlot::SetBorder( int nX, bool bBorder )
{
	if ( nX < 0 || nX >= nLIMIT_COLUMN )
		return;

	const UIRECT& rcImagePos =
		m_pItemImageArray[nX]->GetGlobalPos ();
	const UIRECT& rcImageLocalPos =
		m_pItemImageArray[nX]->GetLocalPos ();
	
	
	m_pEmphasisSlot[ nX ]->SetLocalPos ( rcImageLocalPos );
	m_pEmphasisSlot[ nX ]->SetGlobalPos ( rcImagePos );
	m_pEmphasisSlot[ nX ]->SetVisibleSingle ( TRUE );

	m_bAutoBorder = bBorder;
}

void CItemSlot::SetWindowOpenBox( int nX, BOOL bShow )
{
	if ( nX < 0 || nX >= nLIMIT_COLUMN )
		return;

	if( m_pWindowOpenBox == NULL ) return;

	const UIRECT& rcImagePos =
		m_pItemImageArray[nX]->GetGlobalPos ();
	const UIRECT& rcImageLocalPos =
		m_pItemImageArray[nX]->GetLocalPos ();

	m_pWindowOpenBox->SetLocalPos ( rcImageLocalPos );
	m_pWindowOpenBox->SetGlobalPos ( rcImagePos );
	m_pWindowOpenBox->SetVisibleSingle ( bShow );
}

void CItemSlot::ResetBorder()
{
	for ( int nX = 0; nX < nLIMIT_COLUMN; ++nX )
	{
		m_pEmphasisSlot[ nX ]->SetVisibleSingle ( FALSE );
	}
	m_bAutoBorder = true;
}

DWORD CItemSlot::GetDiffuseAlpha( int nIndex )
{
	if( nIndex < 0 || nIndex >= nLIMIT_COLUMN )
		return NULL;

	D3DCOLOR dwColor = m_pItemImageArray[ nIndex ]->GetDiffuse();
	return (dwColor>>24)&0xFF;
}

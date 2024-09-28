#include "StdAfx.h"
#include "FlipItemSlot.h"
#include "../../../RanLogic/Inventory/GLInventory.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CFlipItemSlot::CFlipItemSlot ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CItemSlot ( pGaeaClient, pInterface, pEngineDevice )
{
	SecureZeroMemory ( m_pFlipImage, sizeof( m_pFlipImage ) );
}

CFlipItemSlot::~CFlipItemSlot ()
{
}

void CFlipItemSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    SetItemIndex ( nOUTOFRANGE );
	m_pMouseOver->SetVisibleSingle ( FALSE );
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
					//	-1이 의미하는 것은 최대값이 없음을 의미한다.
					//	현재의 겹침가능개수가 최대값이 되면, 색이 변하는데
					//	최대값을 없게 설정함으로서 색이 변하지 않게한다.
					const WORD wPileNum = -1;//pItemData->sDrugOp.wPileNum;
					const WORD wTurnNum = m_InvenItem[i].sItemCustom.wTurnNum;						

					//	NOTE
					//		겹쳐짐 개수 표시
					//
					if ( wTurnNum>1 )
					{
						SetNumber ( i, wTurnNum, wPileNum );
					}
				}
			}
		}
	}
}

CUIControl* CFlipItemSlot::CreateFlipImage ( const char* szFlip, bool bColor, DWORD dwColor )
{
	CUIControl* pFlipImage = new CUIControl(m_pEngineDevice);
	pFlipImage->CreateSub ( this, szFlip );
	pFlipImage->SetVisibleSingle ( FALSE );
	if ( bColor )
		pFlipImage->SetDiffuse(dwColor);

	RegisterControl ( pFlipImage );
	return pFlipImage;
}

void CFlipItemSlot::CreateFlipImage ( int nMaxColumn, bool bColor, DWORD dwColor )
{
	CString strTrade[nLIMIT_COLUMN];
	if ( !bColor )
	{
		CString strTrade_colorNone[nLIMIT_COLUMN] = 
		{
			"TRADE_OVER_0",
			"TRADE_OVER_1",
			"TRADE_OVER_2",
			"TRADE_OVER_3",
			"TRADE_OVER_4",
			"TRADE_OVER_5",
			"TRADE_OVER_6",
			"TRADE_OVER_7",
			"TRADE_OVER_8",
			"TRADE_OVER_9",
		};

		for ( int i = 0; i < nLIMIT_COLUMN; ++i )
		{
			strTrade[i] = strTrade_colorNone[i];
		}
	}
	else
	{
		CString strTrade_diffuse[nLIMIT_COLUMN] = 
		{
			"TRADE_OVER_0_DIFFUSE",
			"TRADE_OVER_1_DIFFUSE",
			"TRADE_OVER_2_DIFFUSE",
			"TRADE_OVER_3_DIFFUSE",
			"TRADE_OVER_4_DIFFUSE",
			"TRADE_OVER_5_DIFFUSE",
			"TRADE_OVER_6_DIFFUSE",
			"TRADE_OVER_7_DIFFUSE",
			"TRADE_OVER_8_DIFFUSE",
			"TRADE_OVER_9_DIFFUSE",
		};

		for ( int i = 0; i < nLIMIT_COLUMN; ++i )
		{
			strTrade[i] = strTrade_diffuse[i];
		}
	}

	for ( int i = 0; i < nMaxColumn; i++ )
	{
		m_pFlipImage[i] = CreateFlipImage ( strTrade[i].GetString(), bColor, dwColor );
	}    
}

void CFlipItemSlot::SetFlipItem ( WORD wIndex, BOOL bFlip )
{
	if ( m_pFlipImage[wIndex] )
	{
		m_pFlipImage[wIndex]->SetVisibleSingle ( bFlip );		
	}	
}

BOOL CFlipItemSlot::IsFlipItem ( WORD wIndex )
{
	if ( m_pFlipImage[wIndex] )
	{
		return m_pFlipImage[wIndex]->IsVisible ();
	}
	else
	{
		return FALSE;
	}
}

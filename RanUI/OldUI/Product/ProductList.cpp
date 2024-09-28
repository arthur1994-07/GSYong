#include "StdAfx.h"
#include "ProductList.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicEx.h"

#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	//현재 사이즈에 최적화된 수치로 윈도우 사이즈가 변경된다면 조절해야만한다.
    const float fTEXT_PART_INTERVAL = 4.5f;       // 줄간격
    const float fSELBOX_PART_INTERVAL = 2.5f;     // 선택박스 간격 조절
}

CProductList::CProductList( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface  )
    : CBasicLineBoxSmart(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTextBox(NULL)
    , m_pScrollBar(NULL)
    , m_pSeleted(NULL)
    , m_nSelectedPos(-1)
    , m_eFindSuit(Product::ETypeTotal)
    , m_eFindClass(GLCL_NUM_CLASS)
    , m_bSetIsPossible(FALSE)
{
}

void CProductList::CreateSubControl()
{
    CBasicLineBoxSmart::CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    { // 선택영역
        CBasicLineBoxSmart* pLinBoxSmartTypeTitle = new CBasicLineBoxSmart( m_pEngineDevice );
        pLinBoxSmartTypeTitle->CreateSub( this, "PRODUCT_WINDOW_LIST_SELECTED", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        pLinBoxSmartTypeTitle->CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_YELLOWLINE" );
        RegisterControl( pLinBoxSmartTypeTitle );
        m_pSeleted = pLinBoxSmartTypeTitle;
        m_rcSeleted = m_pSeleted->GetLocalPos();
    }

    { // 텍스트 박스
        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "PRODUCT_WINDOW_LIST_TEXTBOX", UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_LIST_TEXTLIST );
        pTextBox->SetFont( pFont9 );		
        pTextBox->SetSensitive( true );
        //pTextBox->SetLineInterval( 4.5f );
        pTextBox->SetPartInterval( fTEXT_PART_INTERVAL );
        RegisterControl( pTextBox );
        m_pTextBox = pTextBox;
    }



    { // 스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, ID_PRODUCT_TYPE_LIST_SCROOL );
        pScrollBar->CreateBaseScrollBar( "PRODUCT_WINDOW_LIST_SCROOL" );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pTextBox );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
        RegisterControl ( pScrollBar );
        m_pScrollBar = pScrollBar;
    }

}

void CProductList::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
    


    CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame();
    const int nTotal       = m_pTextBox->GetTotalLine();
    const int nVisibleLine = m_pTextBox->GetVisibleLine();
    m_pScrollBar->GetThumbFrame()->SetState( nTotal, nVisibleLine );


    int nCurPos = 0;
    const int nMovableLine = nTotal - nVisibleLine;
    float fPercent = pThumbFrame->GetPercent();
    nCurPos = (int)floor(fPercent * nMovableLine);
    if ( nCurPos < 0 ) nCurPos = 0;
    m_pTextBox->SetCurLine(nCurPos);

    int nScrollRelativePos = m_nSelectedPos-nCurPos;
    if( 0 <= nScrollRelativePos && nScrollRelativePos <= nVisibleLine)
    {
        m_pSeleted->SetVisibleSingle( TRUE );

		UIRECT rcPart;
		if( !m_pTextBox->GetPartInfo(0, rcPart) )
			return;

        float fHeight = rcPart.sizeY + m_pTextBox->GetPartInterval();
		float fSelectSizeY = fHeight + fSELBOX_PART_INTERVAL;

        D3DXVECTOR2 vLocalPos, vGlobalPos;
        vLocalPos.x = m_rcSeleted.left;
        vLocalPos.y = m_rcSeleted.top + fHeight * (nScrollRelativePos);
        vGlobalPos = vLocalPos;

        vGlobalPos.x += this->GetGlobalPos().left;
        vGlobalPos.y += this->GetGlobalPos().top;

		m_pSeleted->SetControlSize( (int)m_rcSeleted.sizeX,  (int)fSelectSizeY );
        m_pSeleted->SetLocalPos( vLocalPos );
        m_pSeleted->SetGlobalPos( vGlobalPos );
    }
    else
    {
        m_pSeleted->SetVisibleSingle( FALSE );
    }
}

void CProductList::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	IProductProgress* pProductProgress = m_pInterface->GetProductProgress();
	if( pProductProgress != NULL && pProductProgress->IsProdution() )
		return;

    CUIGroup::TranslateUIMessage(ControlID, dwMsg);

    switch( ControlID )
    {
    case ID_PRODUCT_TYPE_LIST_TEXTLIST:
        if( CHECK_MOUSE_IN( dwMsg ) )
        {
            int i = 0;
            if( CHECK_LB_UP_LIKE( dwMsg ) )
            {
                m_pSeleted->SetVisibleSingle( TRUE );

				m_nSelectedPos = m_pTextBox->GetSelectPos();

                if( static_cast< size_t >( m_nSelectedPos ) < m_PtrVectorProductRecipe.size() )
                    m_pInterface->ProductWindowSetSeletedItem( m_PtrVectorProductRecipe[ m_nSelectedPos ] );
            }
        }
        break;
    }
}

void CProductList::SetSeletedTabID( UIGUID ControlID )
{
    m_eFindSuit = ControlID;
    m_pSeleted->SetVisibleSingle( FALSE );
    ResetTextBox();
    m_pInterface->ProductWindowSetSeletedItem( NULL );
}

void CProductList::SetFilter( BYTE eFindClass, BOOL bSetIsPossible )
{
    m_eFindClass = eFindClass;
    m_bSetIsPossible = bSetIsPossible;
    ResetTextBox();
}

SProductRecipe* CProductList::Select( int nIndex )
{
	if( m_pScrollBar )
		m_pScrollBar->GetThumbFrame()->SetPercent( 0.0f );

    if( 0 < nIndex && static_cast<unsigned int>( nIndex ) < m_PtrVectorProductRecipe.size() )
    {
        m_nSelectedPos = nIndex;
        return m_PtrVectorProductRecipe[ nIndex ];
    }

    m_nSelectedPos = -1;
    return NULL;
}

void CProductList::ResetTextBox()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

    GLProductClient* pProduct = m_pGaeaClient->GetProductClient();
    if( pProduct == NULL )
        return;

    pProduct->FindProductList( m_PtrVectorProductRecipe, m_eFindSuit, m_eFindClass ); 

	if( m_bSetIsPossible )
		FilteringTextBox();	

    int nTotalLine = static_cast<int>( m_PtrVectorProductRecipe.size() );
	m_pTextBox->ClearText();
    m_pTextBox->SetLimitLine( nTotalLine );

    for( int i=0; i<nTotalLine; ++i )
    {
        SITEM* pItem = GLItemMan::GetInstance().GetItem( m_PtrVectorProductRecipe[ i ]->sResultItem.sNID );
		if( !pItem )
			continue;

        if( pProduct->CheckProductLearn( m_PtrVectorProductRecipe[i] ) == false )		// 조합서 미습득시
        {
            DWORD nTextColor = NS_UITEXTCOLOR::RED;
			int nIndex = m_pTextBox->AddText( pItem->GetName(), nTextColor );			// 텍스트 추가
            m_pTextBox->SetTextImage(nIndex, "PRODUCT_WINDOW_TYPE_STATUS_NOTTLEARN" );  // 자물쇠 아이콘
        }
        else
        {
            DWORD nTextColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];		// 텍스트 컬러(해당아이템 등급)에 따라
			int nIndex = m_pTextBox->AddOneLineText( pItem->GetName(), false, nTextColor );	// 텍스트 추가
			if ( nIndex != -1 )
			{
				BYTE eState;
				const WORD nProductMaterialCount = pProduct->CountProductMaterial( m_PtrVectorProductRecipe[ i ], pCharacter, eState );

				if ( nProductMaterialCount > 0 )	// 재료 만족
				{
					CString strCount;
					strCount.Format( " (%d)", nProductMaterialCount );
					m_pTextBox->SetTextImage( nIndex, "PRODUCT_WINDOW_TYPE_STATUS_ENOUGH" );				// 재료 만족 이미지
					m_pTextBox->AddStringToLimit( nIndex, strCount, D3DCOLOR_XRGB( 0xC0, 0xFF, 0x00 ) );	// 제작 가능 갯수 텍스트 추가
				}
				else
				{
					switch( eState )
					{
					case Product::EStatePart:		// 재료 부분 만족
						m_pTextBox->SetTextImage(nIndex, "PRODUCT_WINDOW_TYPE_STATUS_PART" );
						break;
					case Product::EStateNothing:	// 재료 하나도 없음
					default:
						m_pTextBox->SetTextImage(nIndex, "PRODUCT_WINDOW_TYPE_STATUS_NOTHING" );
						break;
					}
				}
			}
        }
	}
	m_nSelectedPos = -1;
}

void CProductList::FilteringTextBox( )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;
    GLProductClient* pProduct = m_pGaeaClient->GetProductClient();

	GLProductClient* pProductClient = m_pGaeaClient->GetProductClient();
	if( !pProductClient )
		return;

	PtrVector_ProductRecipe PtrVecFilteringData;

	int nTotalLine = static_cast<int>( m_PtrVectorProductRecipe.size() );

	for( int i=0; i<nTotalLine; ++i )
	{
		BYTE eState;
        if ( pProduct->CheckProductLearn( m_PtrVectorProductRecipe[i] ) == false )
        {
            continue;
        }
        if ( pProduct->CountProductMaterial( m_PtrVectorProductRecipe[ i ], pCharacter, eState ) == 0 )
        {
            continue;
        }
		PtrVecFilteringData.push_back( m_PtrVectorProductRecipe[ i ] );
	}
	m_PtrVectorProductRecipe.clear();
	m_PtrVectorProductRecipe = PtrVecFilteringData;
}
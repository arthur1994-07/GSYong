#include "StdAfx.h"
#include "ProductResetWindow.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Product/GLProductClient.h"

#include "../Util/RnButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../enginelib/GUInterface/UIWindowTitle.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../enginelib/GUInterface/BasicTextBox.h"

#include "../Util/CheckBox.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../Item/InventoryInterface.h"
#include "../Item/InventoryWindow.h"

CProductResetWindow::CProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CUIWindowEx( pInterface, pEngineDevice )
	, m_pInterface(pInterface)
	, m_pGaeaClient(pGaeaClient)
	, m_pTextBoxTitle(NULL)
	, m_pTextBoxMessageTop(NULL)
	, m_pTextBoxMessageBottom(NULL)
	, m_pYes(NULL)
	, m_pNo(NULL)
	, m_wResetAble(0)
	, m_wUserCheckedNum(0)
	, m_wPosX(-1)
	, m_wPosY(-1)
{
	memset( m_pTextBoxProductList, 0, sizeof( m_pTextBoxProductList ) );
	memset( m_nProductEnum, -1, sizeof( m_nProductEnum ) );
	memset( m_pCheckBoxProductList, 0, sizeof( m_pCheckBoxProductList ) );
}

CBasicTextBox* CProductResetWindow::CreateResetWindowText( const std::string& key, CD3DFontPar* pFont, const std::string& text )
{
	CBasicTextBox* pText =	new CBasicTextBox( m_pEngineDevice );

	if( pText )
	{
		pText->CreateSub( this, key.c_str(), UI_FLAG_DEFAULT );
		pText->SetFont( pFont );
		pText->SetText( text.c_str() );
		RegisterControl( pText );
	}

	return pText;
}

CBasicTextBox* CProductResetWindow::CreateResetWindowText( const std::string& key, CD3DFontPar* pFont )
{
	CBasicTextBox* pText =	new CBasicTextBox( m_pEngineDevice );

	if( pText )
	{
		pText->CreateSub( this, key.c_str(), UI_FLAG_DEFAULT );
		pText->SetFont( pFont );
		RegisterControl( pText );
	}

	return pText;
}

void CProductResetWindow::CreateBaseWindow()
{
	CreateBaseWindowLightGray ( "PRODUCT_RESET_WINDOW", ID2GAMEWORD("MODAL_TITLE", 1) );

	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
	pLineBox->CreateSub( this, "PRODUCT_RESET_WND_BG", UI_FLAG_DEFAULT );
	pLineBox->CreateSubControl( "PRODUCT_RESET_WINDOW_BASE" );
	RegisterControl( pLineBox );

	pLineBox = new CBasicLineBoxSmart( m_pEngineDevice );
	pLineBox->CreateSub( this, "PRODUCT_RESET_WND_BG", UI_FLAG_DEFAULT );
	pLineBox->CreateSubControl( "PRODUCT_RESET_WINDOW_BG" );
	RegisterControl( pLineBox );
	ResizeControl ( "PRODUCT_RESET_WINDOW" );
}

void CProductResetWindow::CreateSubControl()
{
	CD3DFontPar* pFont10 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );

	// 텍스트
	{
		// 상단
		m_pTextBoxMessageTop = CreateResetWindowText( "PRODUCT_RESET_WND_TOP_TEXT", pFont10 );

		// 하단
		m_pTextBoxMessageBottom = CreateResetWindowText( "PRODUCT_RESET_WND_BOTTOM_TEXT", pFont10, ID2GAMEINTEXT("PRODUCT_RESET_BOTTOM_TEXT") );
	}

	// 체크박스
	{		
		m_pCheckBoxProductList[0] = CreateCheckBox( "PRODUCT_RESET_WND_CHECKBOX_0", ID_CHECKBOX_PRODUCT_0, "MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF", false);
		m_pCheckBoxProductList[1] = CreateCheckBox( "PRODUCT_RESET_WND_CHECKBOX_1", ID_CHECKBOX_PRODUCT_1, "MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF", false);
		m_pCheckBoxProductList[2] = CreateCheckBox( "PRODUCT_RESET_WND_CHECKBOX_2", ID_CHECKBOX_PRODUCT_2, "MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF", false);
		m_pCheckBoxProductList[3] = CreateCheckBox( "PRODUCT_RESET_WND_CHECKBOX_3", ID_CHECKBOX_PRODUCT_3, "MAP_WINDOW_CHECKBOX_ON","MAP_WINDOW_CHECKBOX_OFF", false);
	
		m_pTextBoxProductList[0] = CreateResetWindowText( "PRODUCT_RESET_WND_CHECKBOX_TEXT_0", pFont10 );
		m_pTextBoxProductList[1] = CreateResetWindowText( "PRODUCT_RESET_WND_CHECKBOX_TEXT_1", pFont10 );
		m_pTextBoxProductList[2] = CreateResetWindowText( "PRODUCT_RESET_WND_CHECKBOX_TEXT_2", pFont10 );
		m_pTextBoxProductList[3] = CreateResetWindowText( "PRODUCT_RESET_WND_CHECKBOX_TEXT_3", pFont10 );
	}

	// 버튼
	{		
		// 스타일(폰트,색상,마우스오버색상,클릭색상
		RnButton::CreateArg argStyle;
		argStyle.pFont = pFont10;
		argStyle.defaultTextureKey    = "RNBUTTON_DEFAULT_TEXTURE";
		argStyle.mouseOverTextureKey  = "RNBUTTON_OVER_TEXTURE";
		argStyle.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

		// 예
		m_pYes = new RnButton( m_pEngineDevice );
		m_pYes->CreateSub( this, "PRODUCT_RESET_WND_BUTTON_POSITIVE", UI_FLAG_DEFAULT, ID_BTN_YES );
		argStyle.text = ID2GAMEWORD("MODAL_BUTTON", 2);
		m_pYes->CreateSubControl( argStyle );	
		RegisterControl( m_pYes );

		// 아니요
		m_pNo = new RnButton( m_pEngineDevice );
		m_pNo->CreateSub( this, "PRODUCT_RESET_WND_BUTTON_NEGATIVE", UI_FLAG_DEFAULT, ID_BTN_NO );
		argStyle.text = ID2GAMEWORD("MODAL_BUTTON", 3);
		m_pNo->CreateSubControl( argStyle );
		RegisterControl( m_pNo );
	}
}

void CProductResetWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	//초기화 가능 개수보다 유저가 체크한 개수가 작아야만 체크 가능
	switch( ControlID )
	{
	case ID_CHECKBOX_PRODUCT_0:
	case ID_CHECKBOX_PRODUCT_1:
	case ID_CHECKBOX_PRODUCT_2:
	case ID_CHECKBOX_PRODUCT_3:
		{
			if( CHECK_MOUSE_IN ( dwMsg ) && dwMsg&UIMSG_LB_DOWN )
				SetCheckOff( ControlID );
		}
		break;
	case ID_BTN_YES:
		{
			if( IsPossibleReset()==NULL )
			{
				break;
			}

			if( CHECK_MOUSE_IN(dwMsg) && CHECK_LB_UP_LIKE(dwMsg) )
			{
				ReqProductTypeDel();
				break;
			}
		}
		break;
	case ID_BTN_NO:
		{
			if( CHECK_MOUSE_IN(dwMsg) && CHECK_LB_UP_LIKE(dwMsg) )
			{
				SetVisibleSingle(FALSE);
				break;
			}
		}
		break;
	}
}

void CProductResetWindow::ReqProductTypeDel()
{
	GLProductClient* pChar = m_pGaeaClient->GetProductClient();
	if( pChar == NULL )
	{
		return;
	}
	else
	{					
		SetVecCheckedProduct();
		pChar->ReqProductTypeDel( m_vecCheckedProduct, m_wPosX, m_wPosY );
		SetVisibleSingle(FALSE);
	}
}

bool CProductResetWindow::Init(WORD wResetAbleNum, WORD wPosX, WORD wPosY)
{
	if( m_pInterface == NULL)
	{
		return false;
	}

	//초기화윈도우가 열려 있다면 리턴
	if( m_pInterface->UiIsVisibleGroup( PRODUCT_RESET_WINDOW ) )
	{
		return false;
	}

	if( SetData( wResetAbleNum, wPosX, wPosY ) == false )
	{
		return false;
	}

	return true;
}

bool CProductResetWindow::SetData(WORD wResetAbleNum, WORD wPosX, WORD wPosY)
{
	//사용용량이 초기화 최대 개수보다 크다면 예외처리
	if( 0>wResetAbleNum || MAX_PRODUCT_RESET<MAX_PRODUCT_RESET )
	{
		return false;
	}
	else
	{
		m_wResetAble = wResetAbleNum;
		m_wPosX = wPosX;
		m_wPosY = wPosY;
	}	

	//학습한 항목이 없으면 사용불가
	if( IsStudyProduct() == false )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_RESET_FAIL" ) );
		return false;
	}

	//유저가 배운 제작항목들 벡터로 셋팅
	if( SetSubControlData() == false )
	{
		return false;
	}

	return true;
}

bool CProductResetWindow::SetSubControlData()
{
	if( m_vecStudyProduct.size() == 0 )
	{
		return false;
	}

	std::vector<int>::iterator it = m_vecStudyProduct.begin();

	int Count = 0;
	for( ; it != m_vecStudyProduct.end(); ++it )
	{
		int product = *it;
		if( Count < MAX_PRODUCT_RESET )
		{
			m_pTextBoxProductList[Count]->SetText( ID2GAMEWORD("PRODUCT_TYPE_STRING", product+1) );
			m_nProductEnum[Count] = product;
			m_pTextBoxProductList[Count]->SetVisibleSingle(TRUE);
			m_pCheckBoxProductList[Count]->SetVisibleSingle(TRUE);
		}
		++Count;
	}

	for( ; Count < MAX_PRODUCT_RESET; ++Count )
	{
		m_pTextBoxProductList[Count]->SetVisibleSingle(FALSE);
		m_nProductEnum[Count] = -1;
		m_pCheckBoxProductList[Count]->SetVisibleSingle(FALSE);
	}

	return true;
}

void CProductResetWindow::SetVecCheckedProduct( )
{
	if( m_vecCheckedProduct.size() != 0)
	{
		m_vecCheckedProduct.clear();
	}

	for( int i=0; i<MAX_PRODUCT_RESET; ++i )
	{
		if( m_pCheckBoxProductList[i]->IsChecked() )
		{
			m_vecCheckedProduct.push_back( m_nProductEnum[i] );
		}
	}
}

const WORD& CProductResetWindow::GetCheckedNum()
{
	SetCheckedNum();
	return m_wUserCheckedNum;
}

void CProductResetWindow::SetCheckedNum()
{
	m_wUserCheckedNum = 0;
	for( int i=0; i<MAX_PRODUCT_RESET; ++i )
	{
		if( m_pCheckBoxProductList[i]->IsChecked() )
		{
			++m_wUserCheckedNum;
		}			
	}
}

void CProductResetWindow::SetCheckOff( UIGUID ControlID )
{
	if( m_wResetAble < GetCheckedNum() )
	{
		switch( ControlID )
		{
		case ID_CHECKBOX_PRODUCT_0:
			{
				m_pCheckBoxProductList[0]->SetCheckOff();
				break;
			}
			
		case ID_CHECKBOX_PRODUCT_1:
			{
				m_pCheckBoxProductList[1]->SetCheckOff();
				break;
			}

		case ID_CHECKBOX_PRODUCT_2:
			{
				m_pCheckBoxProductList[2]->SetCheckOff();
				break;
			}
			
		case ID_CHECKBOX_PRODUCT_3:
			{
				m_pCheckBoxProductList[3]->SetCheckOff();
				break;
			}

		}
	}
}

bool CProductResetWindow::SetWindowPos()
{
	std::string strTemp = sc::string::format( ID2GAMEINTEXT( "PRODUCT_RESET_TOP_TEXT" ), m_wResetAble );
	m_pTextBoxMessageTop->SetText( strTemp.c_str() );

	{
		IInventory* pInventory = m_pInterface->GetInventory();
		if( pInventory == NULL )
		{
			return false;
		}

		UIRECT rcUIRect = pInventory->GetUIWindowGlobalPos();
		UIRECT rcUIRectThis = GetGlobalPos();

		float fX = rcUIRect.left + ( rcUIRect.sizeX/2 - rcUIRectThis.sizeX/2 );
		float fY = rcUIRect.top + ( rcUIRect.sizeY/2 - rcUIRectThis.sizeY/2 );

		this->SetGlobalPos( D3DXVECTOR2(fX,fY) );

		return true;
	}
}

bool CProductResetWindow::SetInventoryLock()
{
	if( m_pInterface == NULL )
	{
		return false;
	}

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory == NULL )
	{
		return false;
	}

	pInventory->LockInven( PRODUCT_RESET_WINDOW );

	return true;
}

bool CProductResetWindow::SetInventoryUnLock()
{
	if( m_pInterface == NULL )
	{
		return false;
	}

	IInventory* pInventory = m_pInterface->GetInventory();
	if( pInventory == NULL )
	{
		return false;
	}

	pInventory->UnLockInven( PRODUCT_RESET_WINDOW);

	return true;
}

void CProductResetWindow::Open(WORD wResetAbleNum, WORD wPosX, WORD wPosY)
{
	if( Init(wResetAbleNum, wPosX, wPosY) )
	{
		OpenResetWindow(wResetAbleNum, wPosX, wPosY);
	}
}

void CProductResetWindow::Close()
{
	Clear();
	CloseResetWindow();
}

void CProductResetWindow::OpenResetWindow(WORD wResetAbleNum, WORD wPosX, WORD wPosY)
{
	if( SetWindowPos() == false )
	{
		return;
	}

	if( SetInventoryLock() == false )
	{
		return;
	}
	
	m_pInterface->UiShowGroupFocus(PRODUCT_RESET_WINDOW, true);
}

void CProductResetWindow::Clear()
{
	for( int Count=0; Count < MAX_PRODUCT_RESET; ++Count )
	{
		m_pTextBoxProductList[Count]->ClearText();
		m_pCheckBoxProductList[Count]->SetCheckOff();
		m_pTextBoxProductList[Count]->SetVisibleSingle(FALSE);
		m_pCheckBoxProductList[Count]->SetVisibleSingle(FALSE);
		m_nProductEnum[Count] = -1;
	}
	m_wUserCheckedNum = 0;
	m_wPosX = -1;
	m_wPosY = -1;

	m_vecStudyProduct.clear();
	m_vecCheckedProduct.clear();
}

void CProductResetWindow::CloseResetWindow()
{
	if( m_pInterface )
	{
		m_pInterface->UiSetGroupLock( INVENTORY_WINDOW_RENEW );
		m_pInterface->UiHideGroup( PRODUCT_RESET_WINDOW );
		SetInventoryUnLock();
	}
}

bool CProductResetWindow::IsPossibleReset()
{
	WORD wChecked = GetCheckedNum();

	if( wChecked==0 || wChecked>m_wResetAble )
	{
		return false;
	}
	return true;
}

bool CProductResetWindow::IsStudyProduct()
{
	if( m_vecStudyProduct.size() != 0)
	{
		m_vecStudyProduct.clear();
	}

	GLProductClient* pProductClient = m_pGaeaClient->GetProductClient();
	if( pProductClient == NULL )
	{
		return false;
	}

	for( int i=Product::ETypeHeadGear; i<Product::ETypeTotal; ++i )
	{
		if( pProductClient->IsStudyProductType( i ) )
		{
			m_vecStudyProduct.push_back( i );
		}
	}

	if( m_vecStudyProduct.size() == 0 )
	{
		return false;
	}

	return true;
}

void CProductResetWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible == FALSE )
	{
		Close();
	}
}

MyProductResetWindow::MyProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CProductResetWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyProductResetWindow::CreateUIWindowAndRegisterOwnership()
{
	Create( PRODUCT_RESET_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CreateBaseWindow();
	CreateSubControl();
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
	m_pInterface->UiRegisterControl( this,  true );
	m_pInterface->UiShowGroupFocus( PRODUCT_RESET_WINDOW );	
}

void MyProductResetWindow::Open( WORD wResetAbleNum, WORD wPosX, WORD wPosY )
{
	CProductResetWindow::Open( wResetAbleNum, wPosX, wPosY );
}



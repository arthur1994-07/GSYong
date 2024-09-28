
#include "stdafx.h"

#include "RnCharacterWindow.h"
#include "Additional/RnCharacterAdditionalWindow.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "RnCharacterInfoTap.h"
#include "RnVehicleInfoTap.h"
#include "RnPetInfoTap.h"

#include "../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../EngineLib/GUInterface/GameTextControl.h"


#include "../../InnerInterface.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_CHARACTER_INFO = "캐릭터정보";
	const char* ID2GAMEWORD_VEHICLE_INFO = "탈것정보";
	const char* ID2GAMEWORD_PET_INFO = "펫정보";
	const char* ID2GAMEWORD_CHARACTER_WINDOW_MAIN_TITLE = "%1%님의 정보";
}


RnCharacterWindow::RnCharacterWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx(pInterface,pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_pCurrentTap(NULL)
, m_pCharacterInfoTap(NULL)
, m_pVehicleInfoTap(NULL)
, m_pPetInfoTap(NULL)
{
	ID2GAMEWORD_CHARACTER_INFO = ID2GAMEINTEXT("CHARACTER_WINDOW_INFO_TAP",0);
	ID2GAMEWORD_VEHICLE_INFO = ID2GAMEINTEXT("CHARACTER_WINDOW_INFO_TAP",1);
	ID2GAMEWORD_PET_INFO = ID2GAMEINTEXT("CHARACTER_WINDOW_INFO_TAP",2);
	ID2GAMEWORD_CHARACTER_WINDOW_MAIN_TITLE = ID2GAMEINTEXT("CHARACTER_WINDOW_MAIN_TITLE");
}


void RnCharacterWindow::CreateSubControl()
{
	CreateBaseWindowLightGray( "CHARACTER_WINDOW_MAIN",  "CHARACTER_NAME" );
	CreateLineBox( "CHARACTER_WINDOW_REGION", "CHARACTER_WINDOW_REGION_TEXTUREINFO" );

	//! 추가 능력창
	m_pCharacterAdditionalAbility = new RnCharacterAdditionalWindow(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pCharacterAdditionalAbility->CreateSub( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pCharacterAdditionalAbility->CreateBaseWindowLightGray( "CHARACTOR_ADDITIONAL_ABILITY_WINDOW_BASE", (char*)ID2GAMEINTEXT("CHARACTER_WINDOW_INFO_TAP", 3) );
	m_pCharacterAdditionalAbility->CreateSubControl();
	m_pCharacterAdditionalAbility->SetVisibleSingle(FALSE);
	RegisterControl( m_pCharacterAdditionalAbility );

	D3DXVECTOR2 vMovableSize;
	vMovableSize.x = m_pCharacterAdditionalAbility->GetLocalPos().right;
	vMovableSize.y = GetLocalPos().sizeY;

	m_rcBasicPos = GetLocalPos();
	m_rcChangePos = m_rcBasicPos;
	m_rcChangePos.sizeX = vMovableSize.x;
	m_rcChangePos.right = m_rcChangePos.left + m_rcChangePos.sizeX;

	//! Tap
	m_pCharacterInfoTap = new RnCharacterInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pCharacterInfoTap->CreateSub(this,"CHARACTER_WINDOW_REGION",UI_FLAG_DEFAULT,CHARACTER_INFO_TAP_WINDOW);
	m_pCharacterInfoTap->CreateSubControl();
	m_pCharacterInfoTap->SetVisibleSingle(FALSE);
	RegisterControl(m_pCharacterInfoTap);


	m_pVehicleInfoTap = new RnVehicleInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pVehicleInfoTap->CreateSub(this,"CHARACTER_WINDOW_REGION",UI_FLAG_DEFAULT,VEHICLE_INFO_TAP_WINDOW);
	m_pVehicleInfoTap->CreateSubControl();
	m_pVehicleInfoTap->SetVisibleSingle(FALSE);
	RegisterControl(m_pVehicleInfoTap);


	m_pPetInfoTap = new RnPetInfoTap(m_pGaeaClient,m_pInterface,m_pEngineDevice);
	m_pPetInfoTap->CreateSub(this,"CHARACTER_WINDOW_REGION",UI_FLAG_DEFAULT,PET_INFO_TAP_WINDOW);
	m_pPetInfoTap->CreateSubControl();
	m_pPetInfoTap->SetVisibleSingle(FALSE);
	RegisterControl(m_pPetInfoTap);

	//! Tap Button
	STAPBUTTON* pCharacterInfoTap = new STAPBUTTON(m_pEngineDevice);
	pCharacterInfoTap->CreateSub( this, "CHARACTER_WINDOW_CHAR_TAP", UI_FLAG_DEFAULT, CHARACTER_INFO_TAP );
	pCharacterInfoTap->CreateSubControl( ID2GAMEWORD_CHARACTER_INFO,
		"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
		"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
		"CHARACTER_WINDOW_TAP_BASE");

	RegisterControl ( pCharacterInfoTap );
	RegisterTapButton( pCharacterInfoTap );

	STAPBUTTON* pVehicleInfoTap = new STAPBUTTON(m_pEngineDevice);
	pVehicleInfoTap->CreateSub ( this, "CHARACTER_WINDOW_VEHICLE_TAP", UI_FLAG_DEFAULT, VEHICLE_INFO_TAP );
	pVehicleInfoTap->CreateSubControl( ID2GAMEWORD_VEHICLE_INFO ,
		"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
		"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
		"CHARACTER_WINDOW_TAP_BASE");

	RegisterControl ( pVehicleInfoTap );
	RegisterTapButton( pVehicleInfoTap );

	//////////////////////////////////////////////////////////////////////////

    //미구현 기능 숨김
	//STAPBUTTON* pPetInfoTap = new STAPBUTTON(m_pEngineDevice);
	//pPetInfoTap->CreateSub ( this, "CHARACTER_WINDOW_PET_TAP", UI_FLAG_DEFAULT, PET_INFO_TAP );
	//pPetInfoTap->CreateSubControl( ID2GAMEWORD_PET_INFO,
	// 	"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_ACTIVE",
	// 	"CHARACTER_WINDOW_TAP_BASE","CHARACTER_WINDOW_TAP_INACTIVE",
	// 	"CHARACTER_WINDOW_TAP_BASE");
	// 
	//RegisterControl ( pPetInfoTap );
	//RegisterTapButton( pPetInfoTap );
	
	
	EventSelectedTap(CHARACTER_INFO_TAP);
}

void RnCharacterWindow::SetFocusControl()
{
	CUIWindowEx::SetFocusControl();
	m_pCharacterAdditionalAbility->SetFocusControl();
}

void RnCharacterWindow::ResetFocusControl()
{
	CUIWindowEx::ResetFocusControl();
	m_pCharacterAdditionalAbility->ResetFocusControl();
}

void RnCharacterWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	const char* charName = m_pGaeaClient->GetCharacter()->m_szName;
	std::string title = sc::string::format(ID2GAMEINTEXT("CHARACTER_WINDOW_MAIN_TITLE"),charName);
	SetTitleName(title.c_str());

	CUIWindowEx::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
}

void RnCharacterWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{	
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );
}

CUIGroup* RnCharacterWindow::GetTapWindow( UIGUID controlID )
{
	switch ( controlID )
	{
	case CHARACTER_INFO_TAP:
		return m_pCharacterInfoTap;
	case VEHICLE_INFO_TAP:
// 		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("CHARACTER_WINDOW_TAP_NOT_IMPLEMENT_MSG") );
// 		return m_pCharacterInfoTap;

		// 아직 미구현
		return m_pVehicleInfoTap;

	case PET_INFO_TAP:
		// 아직 미구현
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("CHARACTER_WINDOW_TAP_NOT_IMPLEMENT_MSG") );
//		return m_pPetInfoTap;
		return m_pCurrentTap;
	}

	GASSERT(false);
	return NULL;
}

void RnCharacterWindow::EventSelectedTap( UIGUID controlID )
{
	if( m_pCurrentTap )
		m_pCurrentTap->SetVisibleSingle(FALSE);

	m_pCurrentTap = GetTapWindow(controlID);
	m_pCurrentTap->SetVisibleSingle(TRUE);
}

void RnCharacterWindow::UpdateTitleData()
{
	m_pCharacterInfoTap->UpdateTitleData();
}

void RnCharacterWindow::SetVisibleSingle( BOOL bVisible )
{
	if(bVisible)
	{
		m_pCurrentTap->SetVisibleSingle(true);
	}
	SetVisibleCharacterAdditionalWindow( RANPARAM::bCharInfoAddition );

	CUIWindowEx::SetVisibleSingle(bVisible);
}

void RnCharacterWindow::SetStorageItemToVehicleTapWindow( const std::vector<DWORD>& vehicleIDVec )
{
	m_pVehicleInfoTap->SetStorageItemToVehicleTapWindow(vehicleIDVec);
}

void RnCharacterWindow::DeleteUIMeshData()
{
    if (m_pCharacterInfoTap)
	    m_pCharacterInfoTap->DeleteUIMeshData();
    if (m_pVehicleInfoTap)
        m_pVehicleInfoTap->DeleteUIMeshData();
    if (m_pPetInfoTap)
	    m_pPetInfoTap->DeleteUIMeshData();
	
}

void RnCharacterWindow::SetArmSwapTabButton()
{
	if (m_pCharacterInfoTap)
		m_pCharacterInfoTap->SetArmSwapTabButton();
}

void RnCharacterWindow::SetVisibleCharacterAdditionalWindow( bool bVisible )
{
	// 사이즈 변경 후 위치 새로고침
	if( bVisible )
	{
		ResizeWindow( D3DXVECTOR2( m_rcChangePos.sizeX, m_rcChangePos.sizeY ) );
		SetGlobalPos( GetGlobalPos() );
		m_pCharacterAdditionalAbility->SetVisibleSingle( true );

		//m_pCharacterInfoTap->SetVisibleSingle(TRUE);
	}
	else if( !bVisible )
	{
		ResizeWindow( D3DXVECTOR2( m_rcBasicPos.sizeX, m_rcBasicPos.sizeY )  );
		SetGlobalPos( GetGlobalPos() );
		m_pCharacterAdditionalAbility->SetVisibleSingle( false );
	}
}

#include "StdAfx.h"
#include "ProductType.h"

#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Product/GLProductClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CProductType::CProductType( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface )
    : CBasicLineBoxSmart( pEngineDevice )
    , m_pInterface( pInterface )
    , m_pCurrentTap( NULL )
    , m_pGaeaClient( pGaeaClient )
    , m_pAllTap( NULL )
    , m_pHeadGearTap( NULL )
    , m_pUpperTap( NULL )
    , m_pLowerTap( NULL )
    , m_pHandTap( NULL )
    , m_pFootTap( NULL )
    , m_pHandHeldTap( NULL )
    , m_pNeck_EarringTap( NULL )
    , m_pWrist_WaistBandTap( NULL )
    , m_pFinger_AccessoriesTap( NULL )
    , m_pRandomBoxTap( NULL )
    , m_pEtcTap( NULL )
    , m_pCostumeTap( NULL )
    , m_iPreviousSelectedTapID( 0 )
{
    
}

void CProductType::CreateSubControl()
{
    CBasicLineBoxSmart::CreateSubControl( "PRODUCT_WINDOW_BASE_SMARTLINEBOX_DARKGRAY" );
    
    {   // 전부 탭
		CProductTypeTapButton* pAllTab = new CProductTypeTapButton(m_pEngineDevice);
		pAllTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_ALL_BASE"
			, UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_ALL );
	
		CProductTypeTapButton::CreateArg arg;
		arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_ALL_INACTIVE";
		arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_ALL_OVER";
		arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_ALL_ACTIVE";
		pAllTab->CreateSubControl(arg);

		RegisterControl(pAllTab);
        RegisterTapButton(pAllTab);
		m_pAllTap = pAllTab;
        m_pAllTap->SetState( CProductTypeTapButton::E_STUDY_NONE );   //전부탭은 None으로 고정
        m_pAllTap->SetShowSkillTooltip( false );        
    }

    {   // 머리 탭
		CProductTypeTapButton* pHeadTab = new CProductTypeTapButton(m_pEngineDevice);
		pHeadTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_HEAD_BASE"
			, UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_HEADGEAR );

		CProductTypeTapButton::CreateArg arg;
		arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_HEAD_INACTIVE";
		arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_HEAD_OVER";
		arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_HEAD_ACTIVE";
		pHeadTab->CreateSubControl(arg);

        RegisterControl(pHeadTab);
        RegisterTapButton(pHeadTab);
		m_pHeadGearTap= pHeadTab;
    }

    {   // 상의 탭
        CProductTypeTapButton* pUpperTab = new CProductTypeTapButton(m_pEngineDevice);
        pUpperTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_UPPER_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_UPPER );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_UPPER_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_UPPER_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_UPPER_ACTIVE";
        pUpperTab->CreateSubControl(arg);

        RegisterControl(pUpperTab);
        RegisterTapButton(pUpperTab);
		m_pUpperTap = pUpperTab;
    }

    {   // 하의 탭
        CProductTypeTapButton* pUnderTab = new CProductTypeTapButton(m_pEngineDevice);
        pUnderTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_UNDER_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_LOWER );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_UNDER_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_UNDER_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_UNDER_ACTIVE";
        pUnderTab->CreateSubControl(arg);

        RegisterControl(pUnderTab);
        RegisterTapButton(pUnderTab);
		m_pLowerTap = pUnderTab;
    }

    {   // 장갑 탭
        CProductTypeTapButton* pGlovesTab = new CProductTypeTapButton(m_pEngineDevice);
        pGlovesTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_GLOVES_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_HAND );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_GLOVES_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_GLOVES_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_GLOVES_ACTIVE";
        pGlovesTab->CreateSubControl(arg);

        RegisterControl(pGlovesTab);
        RegisterTapButton(pGlovesTab);
		m_pHandTap = pGlovesTab;
    }

    {   // 신발 탭
        CProductTypeTapButton* pShoesTab = new CProductTypeTapButton(m_pEngineDevice);
        pShoesTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_SHOES_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_FOOT );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_SHOES_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_SHOES_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_SHOES_ACTIVE";
        pShoesTab->CreateSubControl(arg);

        RegisterControl(pShoesTab);
        RegisterTapButton(pShoesTab);
		m_pFootTap = pShoesTab;
    }

    {   // 무기 탭
        CProductTypeTapButton* pWeaponTab = new CProductTypeTapButton(m_pEngineDevice);
        pWeaponTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_WEAPON_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_HANDHELD );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_WEAPON_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_WEAPON_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_WEAPON_ACTIVE";
        pWeaponTab->CreateSubControl(arg);

        RegisterControl(pWeaponTab);
        RegisterTapButton(pWeaponTab);
		m_pHandHeldTap = pWeaponTab;
    }

    {   // 목걸이 + 귀걸이 탭
        CProductTypeTapButton* pNecklaceTab = new CProductTypeTapButton(m_pEngineDevice);
        pNecklaceTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_NECKLACE_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_NECKEARRING );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_NECKLACE_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_NECKLACE_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_NECKLACE_ACTIVE";
        pNecklaceTab->CreateSubControl(arg);

        RegisterControl(pNecklaceTab);
        RegisterTapButton(pNecklaceTab);
		m_pNeck_EarringTap = pNecklaceTab;
    }

    {   // 팔찌 + 벨트 탭
        CProductTypeTapButton* pBraceletTab = new CProductTypeTapButton(m_pEngineDevice);
        pBraceletTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_BRACELET_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_WRISTWAIST );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_BRACELET_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_BRACELET_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_BRACELET_ACTIVE";
        pBraceletTab->CreateSubControl(arg);

        RegisterControl(pBraceletTab);
        RegisterTapButton(pBraceletTab);
		m_pWrist_WaistBandTap = pBraceletTab;
    }

    {   // 반지 + 악세사리 탭
        CProductTypeTapButton* pRingTab = new CProductTypeTapButton(m_pEngineDevice);
        pRingTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_RING_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_FINGERACCESSORIES );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_RING_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_RING_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_RING_ACTIVE";
        pRingTab->CreateSubControl(arg);

        RegisterControl(pRingTab);
        RegisterTapButton(pRingTab);
		m_pFinger_AccessoriesTap = pRingTab;
    }

    {   // 랜덤박스 탭
        CProductTypeTapButton* pRandomBox = new CProductTypeTapButton(m_pEngineDevice);
        pRandomBox->CreateSub( this, "PRODUCT_WINDOW_TYPE_ETC_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_RANDOMBOX );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_ETC_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_ETC_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_ETC_ACTIVE";
        pRandomBox->CreateSubControl(arg);

        RegisterControl(pRandomBox);
        RegisterTapButton(pRandomBox);
		m_pRandomBoxTap = pRandomBox;
    }

    {   
        // 기타 (탈것 음식 등등) 탭
        CProductTypeTapButton* pEtcTab = new CProductTypeTapButton(m_pEngineDevice);
        pEtcTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_VEHICLE_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_ETC );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_VEHICLE_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_VEHICLE_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_VEHICLE_ACTIVE";
        pEtcTab->CreateSubControl(arg);

        RegisterControl(pEtcTab);
        RegisterTapButton(pEtcTab);
		m_pEtcTap = pEtcTab;
    }

    {   // 코스튬 탭
        CProductTypeTapButton* pCostumeTab = new CProductTypeTapButton(m_pEngineDevice);
        pCostumeTab->CreateSub( this, "PRODUCT_WINDOW_TYPE_COSTUME_BASE"
            , UI_FLAG_DEFAULT, ID_PRODUCT_TYPE_TAB_COSTUME );

        CProductTypeTapButton::CreateArg arg;
        arg.defaultTextureKey = "PRODUCT_WINDOW_TYPE_COSTUME_INACTIVE";
        arg.mouseOverTextureKey = "PRODUCT_WINDOW_TYPE_COSTUME_OVER";
        arg.mouseClickTextureKey = "PRODUCT_WINDOW_TYPE_COSTUME_ACTIVE";
        pCostumeTab->CreateSubControl(arg);

        RegisterControl(pCostumeTab);
        RegisterTapButton(pCostumeTab);
		m_pCostumeTap = pCostumeTab;
    }

	RefreshTapButton();
    TapSelect( ID_PRODUCT_TYPE_TAB_ALL );
	m_iPreviousSelectedTapID = ID_PRODUCT_TYPE_TAB_ALL;
}

void CProductType::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	IProductProgress* pProductProgress = m_pInterface->GetProductProgress();
	if( pProductProgress != NULL && pProductProgress->IsProdution() )
		return;

    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
    CUIGroup::TranslateUIMessage(ControlID, dwMsg);

    if( ControlID < ID_PRODUCT_TYPE_TAB_HEADGEAR || ControlID >= ID_PRODUCT_TYPE_TAB_ALL )
    {
        return;
    }
    //텍스트인덱스와 콘트롤ID값이 일치하지않아서 맞춰주는 작업
    int nTextIndex = ControlID;
    if ( nTextIndex == ID_PRODUCT_TYPE_TAB_ALL )
    {
        nTextIndex = 0;
    }
    if( CHECK_MOUSE_IN( dwMsg ) )
    {
        m_pInterface->SHOW_COMMON_LINEINFO(
            ID2GAMEWORD("PRODUCT_TYPE_STRING", nTextIndex ),
            D3DCOLOR_XRGB( 0x50, 0x82, 0xC0 ),
            TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
    }
    if ( dwMsg & ProductTypeMSG::UI_MSG_PRODUCT_STUDY ) //탭버튼에서 학습 버튼이 클릭되었을 때를 뜻함
    {
        TapSelect( ControlID );
        SetMessageEx( ProductTypeMSG::UI_MSG_PRODUCT_STUDY | ControlID );   //제작학습 확인메시지박스 팝업 요청 메시지
    }
}


//hssshin
void CProductType::EventSelectedTap( UIGUID controlID )
{
	CProductTypeTapButton* pSelectTapButton = NULL;

	switch( controlID )
	{
	case ID_PRODUCT_TYPE_TAB_ALL:
		pSelectTapButton = m_pAllTap;
		break;
	case ID_PRODUCT_TYPE_TAB_HEADGEAR:
		pSelectTapButton = m_pHeadGearTap;
		break;
	case ID_PRODUCT_TYPE_TAB_UPPER:
		pSelectTapButton = m_pUpperTap;
		break;
	case ID_PRODUCT_TYPE_TAB_LOWER:
		pSelectTapButton = m_pLowerTap;
		break;
	case ID_PRODUCT_TYPE_TAB_HAND:
		pSelectTapButton = m_pHandTap;
		break;
	case ID_PRODUCT_TYPE_TAB_FOOT:
		pSelectTapButton = m_pFootTap;
		break;
	case ID_PRODUCT_TYPE_TAB_HANDHELD:
		pSelectTapButton = m_pHandHeldTap;
		break;
	case ID_PRODUCT_TYPE_TAB_NECKEARRING:
		pSelectTapButton = m_pNeck_EarringTap;
		break;
	case ID_PRODUCT_TYPE_TAB_WRISTWAIST:
		pSelectTapButton = m_pWrist_WaistBandTap;
		break;
	case ID_PRODUCT_TYPE_TAB_FINGERACCESSORIES:
		pSelectTapButton = m_pFinger_AccessoriesTap;
		break;
	case ID_PRODUCT_TYPE_TAB_ETC:
		pSelectTapButton = m_pEtcTap;
		break;
	case ID_PRODUCT_TYPE_TAB_RANDOMBOX:
		pSelectTapButton = m_pRandomBoxTap;
		break;
	case ID_PRODUCT_TYPE_TAB_COSTUME:
		pSelectTapButton = m_pCostumeTap;
		break;
	}

	if ( pSelectTapButton == NULL )
	{
		return;
	}
    UIGUID tmpTypeID = controlID - ID_PRODUCT_TYPE_TAB_HEADGEAR;    //ID랑 Enum이랑 매치가 바로 안되서 해주는 작업
	switch( pSelectTapButton->GetState() )
	{
    case CProductTypeTapButton::E_STUDY_UNABLE:
        {
            TapSelect( m_iPreviousSelectedTapID );	//이전 탭으로 롤백
        }
        break;
    default:
        {
            m_pInterface->ProductWindowSetSeletedTabID( tmpTypeID );    //탭을 선택하여 리스트를 갱신하는 부분
            m_iPreviousSelectedTapID = controlID;
        }
		break;
	}
}

void CProductType::RefreshTapButton()
{
    //익힌 제작분야의 숙련도만 얻어오는것보다 차라리 그냥 전부 얻어오는게 빠를것같음
    const GLProductClient* pProduct = m_pGaeaClient->GetProductClient();

    GLProductClient* Product = m_pGaeaClient->GetProductClient();
    if ( Product == NULL )
    {
        return;
    }
    WORD tmpTypeID = GetTapID() - ID_PRODUCT_TYPE_TAB_HEADGEAR;
    if ( Product->IsUnableProductType( tmpTypeID ) )  //사용할수 없는 탭이 선택되면 전부 탭으로 변경
    {
        TapSelect( ID_PRODUCT_TYPE_TAB_ALL );
    }

	m_pHeadGearTap->SetState( GetStudyState( Product::ETypeHeadGear ) );
    m_pHeadGearTap->SetSkill( pProduct->GetProductSkill( Product::ETypeHeadGear ) );

	m_pUpperTap->SetState( GetStudyState( Product::ETypeUpper ) );
    m_pUpperTap->SetSkill( pProduct->GetProductSkill( Product::ETypeUpper ) );

	m_pLowerTap->SetState( GetStudyState( Product::ETypeLower ) );
    m_pLowerTap->SetSkill( pProduct->GetProductSkill( Product::ETypeLower ) );

	m_pHandTap->SetState( GetStudyState( Product::ETypeHand ) );
    m_pHandTap->SetSkill( pProduct->GetProductSkill( Product::ETypeHand ) );

	m_pFootTap->SetState( GetStudyState( Product::ETypeFoot ) );
    m_pFootTap->SetSkill( pProduct->GetProductSkill( Product::ETypeFoot ) );

	m_pHandHeldTap->SetState( GetStudyState( Product::ETypeHandheld ) );
    m_pHandHeldTap->SetSkill( pProduct->GetProductSkill( Product::ETypeHandheld ) );

	m_pNeck_EarringTap->SetState( GetStudyState( Product::ETypeNeck_Earring ) );
    m_pNeck_EarringTap->SetSkill( pProduct->GetProductSkill( Product::ETypeNeck_Earring ) );

	m_pWrist_WaistBandTap->SetState( GetStudyState( Product::ETypeWrist_WaistBand ) );
    m_pWrist_WaistBandTap->SetSkill( pProduct->GetProductSkill( Product::ETypeWrist_WaistBand ) );

	m_pFinger_AccessoriesTap->SetState( GetStudyState( Product::ETypeFinger_Accessories ) );
    m_pFinger_AccessoriesTap->SetSkill( pProduct->GetProductSkill( Product::ETypeFinger_Accessories ) );

	m_pEtcTap->SetState( GetStudyState( Product::ETypeEtc ) );
    m_pEtcTap->SetSkill( pProduct->GetProductSkill( Product::ETypeEtc ) );

	m_pRandomBoxTap->SetState( GetStudyState( Product::ETypeRandomBox ) );
    m_pRandomBoxTap->SetSkill( pProduct->GetProductSkill( Product::ETypeRandomBox ) );

	m_pCostumeTap->SetState( GetStudyState( Product::ETypeCostume ) );
    m_pCostumeTap->SetSkill( pProduct->GetProductSkill( Product::ETypeCostume ) );
}

void CProductType::SetShowSkill( const bool bState )
{
    m_pHeadGearTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHeadGear ) : false );
	m_pUpperTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType(Product::ETypeUpper) : false );
    m_pLowerTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeLower ) : false );
    m_pHandTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHand ) : false );
    m_pFootTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeFoot ) : false );
    m_pHandHeldTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHandheld ) : false );
    m_pNeck_EarringTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeNeck_Earring ) : false );
    m_pWrist_WaistBandTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeWrist_WaistBand ) : false );
    m_pFinger_AccessoriesTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeFinger_Accessories ) : false );
    m_pEtcTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeEtc ) : false );
    m_pRandomBoxTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeRandomBox ) : false );
    m_pCostumeTap->SetShowSkillTooltip( bState ? m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeCostume ) : false );
//     //현재는 반드시 학습한 제작분야의 경우만 보여줌 ( 혹시 공개제작분야의 숙련도가 보여야한다면 isStudy함수 수정요망 )
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHeadGear ) )              //머리제작분야를 익혔는가?
// 	    m_pHeadGearTap->SetShowSkillTooltip( true ); //숙련도 툴팁 출력
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeUpper ) )                 //상의제작분야를 익혔는가?
//         m_pUpperTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeLower ) )                 //하의제작분야를 익혔는가?
//         m_pLowerTap->SetShowSkillTooltip( true );
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHand ) )                  //장갑제작분야를 익혔는가?
//         m_pHandTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeFoot ) )                  //신발제작분야를 익혔는가?
//         m_pFootTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeHandheld ) )              //무기제작분야를 익혔는가?
//         m_pHandHeldTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeNeck_Earring ) )          //목걸이귀걸이제작분야를 익혔는가?
//         m_pNeck_EarringTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeWrist_WaistBand ) )       //팔지벨트제작분야를 익혔는가?
//         m_pWrist_WaistBandTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeFinger_Accessories ) )    //반지제작분야를 익혔는가?
//         m_pFinger_AccessoriesTap->SetShowSkillTooltip( true );
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeEtc ) )                   //기타제작분야를 익혔는가?
//         m_pEtcTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeRandomBox ) )             //박스제작분야를 익혔는가?
//         m_pRandomBoxTap->SetShowSkillTooltip( true ); 
//     if ( m_pGaeaClient->GetProductClient()->IsUsableProductType( Product::ETypeCostume ) )               //코스튬제작분야를 익혔는가?
//         m_pCostumeTap->SetShowSkillTooltip( true ); 
}

const BYTE CProductType::GetStudyState( WORD wProductType )
{
    if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal )
    {
        return CProductTypeTapButton::E_STUDY_NONE;
    }
    if ( m_pGaeaClient->GetProductClient()->IsUnableProductType( wProductType ) )
    {
        return CProductTypeTapButton::E_STUDY_UNABLE;
    }
    if ( m_pGaeaClient->GetProductClient()->IsOpenProductType( wProductType ) )
    {
        return CProductTypeTapButton::E_STUDY_PUBLIC;
    }
    GLProductClient* pProduct = m_pGaeaClient->GetProductClient();
    if ( !pProduct )
    {
        return CProductTypeTapButton::E_STUDY_NONE;
    }
    if ( pProduct->IsStudyProductType( wProductType ) )
    {
        return CProductTypeTapButton::E_STUDY_STUDIED;
    }
    if( pProduct->GetProductTypeStudyPoint() == 0 )
    {
        return CProductTypeTapButton::E_STUDY_DISABLE;
    }
    return CProductTypeTapButton::E_STUDY_NEED;
}
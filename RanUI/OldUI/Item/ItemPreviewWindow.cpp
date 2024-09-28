#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxInputDevice.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

//#include "../../ModalCallerID.h"
#include "./ItemPreviewWindow.h"
#include "./ItemImage.h"
#include "../../InnerInterface.h"

#include "../Util/UIMeshRender.h"
#include "../Util/UIMeshData_SkinChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CItemPreviewWindow::TURN_SPEED              = 1.4f;
const float CItemPreviewWindow::CAMERA_POSITION_Y       = 0.0f;
const float CItemPreviewWindow::CAMERA_POSITION_Z       = -27.0f; 
const float CItemPreviewWindow::CAMERA_POSITION_Z_MIN   = -40.0f;    
const float CItemPreviewWindow::CAMERA_POSITION_Z_MAX   = -20.0f; 

CItemPreviewWindow::CItemPreviewWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
    : CUIWindowEx               (pInterface, pEngineDevice)
    , m_pGaeaClient             (pGaeaClient)
    , m_pMeshRender             (NULL)
    , m_pImage_Arrow_Turn_Left  (NULL)
    , m_pImage_Arrow_Turn_Right (NULL)
    , m_pMeshData               (NULL)
    , m_emCharIndex             (EMCHARINDEX(0))
    , m_emAniSubType            (AN_SUB_NONE)
    , m_fCamDistance            (CAMERA_POSITION_Z)
    , m_fTurnAngle              (0.0f)
    , m_bTurnLeft               (FALSE)
    , m_bTurnRight              (FALSE)
{
    SecureZeroMemory( m_pItemImageArray, sizeof(m_pItemImageArray) );
}

CItemPreviewWindow::~CItemPreviewWindow()
{
}

void CItemPreviewWindow::CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
    const int nAlignLeft = TEXT_ALIGN_LEFT;

    // 배경
    {
        CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub ( this, "ITEMPREVIEW_WINDOW_BKLINE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateSubControl( "ITEMPREVIEW_WINDOW_BKLINE_TEXINFO" );
        RegisterControl ( pLineBox );
    }

    // 메쉬 렌더
    {
        CUIMeshRender* pMeshRender = new CUIMeshRender(m_pEngineDevice);
        pMeshRender->CreateSub ( this, "ITEMPREVIEW_WINDOW_MESHRENDER", UI_FLAG_DEFAULT, ITEMPREVIEW_MESHRENDER );
        RegisterControl ( pMeshRender );
        m_pMeshRender = pMeshRender;
    }

    // 아이템 이미지
    {	
        m_pItemImageArray[EMPREVIEWSLOT_HEAD]   = CreateItemImage ( "ITEMPREVIEW_SLOT_HEAD",   ITEMPREVIEW_SLOT_HEAD );
        m_pItemImageArray[EMPREVIEWSLOT_UPPER]  = CreateItemImage ( "ITEMPREVIEW_SLOT_UPPER",  ITEMPREVIEW_SLOT_UPPER );
        m_pItemImageArray[EMPREVIEWSLOT_LOWER]  = CreateItemImage ( "ITEMPREVIEW_SLOT_LOWER",  ITEMPREVIEW_SLOT_LOWER );
        m_pItemImageArray[EMPREVIEWSLOT_HAND]   = CreateItemImage ( "ITEMPREVIEW_SLOT_HAND",   ITEMPREVIEW_SLOT_HAND );
        m_pItemImageArray[EMPREVIEWSLOT_FOOT]   = CreateItemImage ( "ITEMPREVIEW_SLOT_FOOT",   ITEMPREVIEW_SLOT_FOOT );
        m_pItemImageArray[EMPREVIEWSLOT_WEAPON] = CreateItemImage ( "ITEMPREVIEW_SLOT_WEAPON", ITEMPREVIEW_SLOT_WEAPON );
    }

    // 아이템 슬롯 글자
    {
        CBasicTextBox* pTextBox = NULL;

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_HEAD", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 0), NS_UITEXTCOLOR::LIGHTYELLOW );

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_UPPER", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 1), NS_UITEXTCOLOR::LIGHTYELLOW );

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_LOWER", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 2), NS_UITEXTCOLOR::LIGHTYELLOW );

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_HAND", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 3), NS_UITEXTCOLOR::LIGHTYELLOW );

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_FOOT", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 4), NS_UITEXTCOLOR::LIGHTYELLOW );

        pTextBox = CreateStaticControl ( "ITEMPREVIEW_TEXTBOX_WEAPON", pFont, nAlignLeft );
        pTextBox->SetOneLineText( ID2GAMEWORD("ITEMPREVIEW_TEXTBOX", 5), NS_UITEXTCOLOR::LIGHTYELLOW );
    }
    
    // 모델 회전 화살표
    {
        CSwapImage* pSwapImage = NULL;
        
        pSwapImage = new CSwapImage(m_pEngineDevice);
        pSwapImage->CreateSub   ( this, "ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT", UI_FLAG_DEFAULT, ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT );
        pSwapImage->SetImage    ( "ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT_DEFAULT" );
        RegisterControl ( pSwapImage );
        m_pImage_Arrow_Turn_Left = pSwapImage;

        pSwapImage = new CSwapImage(m_pEngineDevice);
        pSwapImage->CreateSub   ( this, "ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT", UI_FLAG_DEFAULT, ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT );
        pSwapImage->SetImage    ( "ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT_DEFAULT" );
        RegisterControl ( pSwapImage );
        m_pImage_Arrow_Turn_Right = pSwapImage;
    }
}

void CItemPreviewWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( m_pMeshData )
    {
        m_pMeshData->SelectAni( AN_GUARD_N, m_emAniSubType );

        if ( m_bTurnLeft )
        {
            TurnLeft( CItemPreviewWindow::TURN_SPEED*fElapsedTime );
            m_bTurnLeft = FALSE;
        }

        if ( m_bTurnRight )
        {
            TurnRight( CItemPreviewWindow::TURN_SPEED*fElapsedTime );
            m_bTurnRight = FALSE;
        }

        // Rotation
        D3DXMatrixRotationY( &m_matWorld, m_fTurnAngle );

        // Translate ( 모델 높이의 1/2만큼 아래로 내린다 )
        FLOAT fHeight = m_pMeshData->GetHeight();
        m_matWorld._42 = -fHeight/2.0f;
        m_pMeshData->SetWorldMatrix( m_matWorld );

        // View 설정.
        D3DXVECTOR3 vFromPt( 0.f, CAMERA_POSITION_Y, m_fCamDistance );
        D3DXVECTOR3 vLookatPt( 0.f, 0.f, 0.f );
        D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
        D3DXMatrixLookAtLH( &m_matView, &vFromPt, &vLookatPt, &vUpVec );
        m_pMeshData->SetViewMatrix( m_matView, vFromPt );
    }

    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CItemPreviewWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

    switch ( ControlID )
    {
    case ITEMPREVIEW_MESHRENDER:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                int dx,dy,dz;
                DxInputDevice::GetInstance().GetMouseMove ( dx, dy, dz );
                m_fCamDistance += (float) dz/150.0f;

                if ( m_fCamDistance < CAMERA_POSITION_Z_MIN )
                {
                    m_fCamDistance = CAMERA_POSITION_Z_MIN;
                }
                else if ( m_fCamDistance > CAMERA_POSITION_Z_MAX )
                {
                    m_fCamDistance = CAMERA_POSITION_Z_MAX;
                }
            }
        }
        break;

    default:
        break;

    case ITEMPREVIEW_SLOT_HEAD:
    case ITEMPREVIEW_SLOT_UPPER:
    case ITEMPREVIEW_SLOT_LOWER:
    case ITEMPREVIEW_SLOT_HAND:
    case ITEMPREVIEW_SLOT_FOOT:
    case ITEMPREVIEW_SLOT_WEAPON:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                DWORD dwSlot = ControlID - ITEMPREVIEW_SLOT_BEGIN;

                if ( m_sItemCustomArray[dwSlot].GetNativeID() != NATIVEID_NULL () )
                {						
                    m_pInterface->SHOW_ITEM_INFO(
						m_sItemCustomArray[dwSlot],
						FALSE, FALSE, FALSE, FALSE, FALSE,
						0, 0 );
                }
            }
        }
        break;

    case ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&(UIMSG_LB_DOWN|UIMSG_LB_PRESEED|UIMSG_LB_DRAG) )
                {
                    m_pImage_Arrow_Turn_Left->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT_CLICK" );
                    m_bTurnLeft = TRUE;
                }
                else
                {
                    m_pImage_Arrow_Turn_Left->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT_OVER" );
                }
            }
            else
            {
                m_pImage_Arrow_Turn_Left->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_LEFT_DEFAULT" );
            }
        }
        break;

    case ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&(UIMSG_LB_DOWN|UIMSG_LB_PRESEED|UIMSG_LB_DRAG) )
                {
                    m_pImage_Arrow_Turn_Right->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT_CLICK" );
                    m_bTurnRight = TRUE;
                }
                else
                {
                    m_pImage_Arrow_Turn_Right->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT_OVER" );
                }
            }
            else
            {
                m_pImage_Arrow_Turn_Right->SetImage ( "ITEMPREVIEW_WINDOW_ARROW_TURN_RIGHT_DEFAULT" );
            }
        }
        break;
    };

}

void CItemPreviewWindow::SetVisibleSingle ( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle( bVisible );

    if( bVisible )
    {

    }
    else
    {

    }
}

CBasicTextBox* CItemPreviewWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
    CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
    pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
    pStaticText->SetFont ( pFont );
    pStaticText->SetTextAlign ( nAlign );	
    RegisterControl ( pStaticText );

    return pStaticText;
}

CItemImage*	CItemPreviewWindow::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
    CItemImage* pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
    pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
    pItemImage->CreateSubControl ();
    RegisterControl ( pItemImage );

    return pItemImage;
}

void CItemPreviewWindow::TurnLeft ( const float fAngle )
{
    m_fTurnAngle -= fAngle;

    if ( m_fTurnAngle < 0.0f )
    {
        m_fTurnAngle = (D3DX_PI*2.0f) + m_fTurnAngle;
    }
    else if ( m_fTurnAngle > D3DX_PI*2.0f )
    {
        m_fTurnAngle = m_fTurnAngle - (D3DX_PI*2.0f);
    }
}

void CItemPreviewWindow::TurnRight ( const float fAngle )
{
    m_fTurnAngle += fAngle;

    if ( m_fTurnAngle < 0.0f )
    {
        m_fTurnAngle = (D3DX_PI*2.0f) + m_fTurnAngle;
    }
    else if ( m_fTurnAngle > D3DX_PI*2.0f )
    {
        m_fTurnAngle = m_fTurnAngle - (D3DX_PI*2.0f);
    }
}

const DWORD CItemPreviewWindow::ConvertColor ( const WORD wColor )
{
    WORD wR, wG, wB;
    wR = wColor >> 10;
    wG = wColor >> 5;
    wG = wG&0x1f;
    wB = wColor&0x1f;

    wR = (WORD)( wR * 8.225806f );	//<< 3;
    wG = (WORD)( wG * 8.225806f );	//<< 3;
    wB = (WORD)( wB * 8.225806f );	//<< 3;

    DWORD dwColor = 0xff000000 + (wR<<16) + (wG<<8) + wB;
    return dwColor;
}

const EMSLOT CItemPreviewWindow::Item_2_Slot ( const SITEMCUSTOM& sItemCustom )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

    if ( !pItem )
    {
        return SLOT_TSIZE;
    }

    return Item_2_Slot ( pItem );
}

const EMSLOT CItemPreviewWindow::Item_2_Slot ( const SITEM* pItem )
{
    EMSLOT emSlot(SLOT_TSIZE);

    if ( !pItem )
    {
        return emSlot;
    }

    switch ( pItem->sSuitOp.emSuit )
    {
    case SUIT_HEADGEAR:
        emSlot = SLOT_HEADGEAR;
        break;
    case SUIT_UPPER:
        emSlot = SLOT_UPPER;
        break;
    case SUIT_LOWER:
        emSlot = SLOT_LOWER;
        break;
    case SUIT_HAND:
        emSlot = SLOT_HAND;
        break;
    case SUIT_FOOT:
        emSlot = SLOT_FOOT;
        break;
    case SUIT_HANDHELD:
        emSlot = SLOT_LHAND;
        if ( pItem->sSuitOp.emHand&HAND_RIGHT )
        {
            emSlot = SLOT_RHAND;
        }
        break;
    }

    return emSlot;
}

const CItemPreviewWindow::EMPREVIEWSLOT CItemPreviewWindow::Slot_2_PreviewSlot ( const EMSLOT emSlot )
{
    switch ( emSlot )
    {
    case SLOT_HEADGEAR:     return EMPREVIEWSLOT_HEAD;
    case SLOT_UPPER:        return EMPREVIEWSLOT_UPPER;
    case SLOT_LOWER:        return EMPREVIEWSLOT_LOWER;
    case SLOT_HAND:         return EMPREVIEWSLOT_HAND;
    case SLOT_FOOT:         return EMPREVIEWSLOT_FOOT;
    case SLOT_RHAND:
    case SLOT_RHAND_S:
    case SLOT_LHAND:
    case SLOT_LHAND_S:      return EMPREVIEWSLOT_WEAPON;

	// 기획팀에서 셋팅을 코스츔이 되도록 해버렸다. 지원해줘야한다. (SLOT_DECORATION,SLOT_EARRINGS)
	//case SLOT_DECORATION:   return EMPREVIEWSLOT_DECORATION;	// 장식
	//case SLOT_EARRINGS:     return EMPREVIEWSLOT_EARRINGS;		// 귀걸이
    };

    return EMPREVIEWSLOT_SIZE;
}

void CItemPreviewWindow::InitGeometry ()
{
   if ( m_pMeshData )
    {
        m_fTurnAngle = 0.0f;

        // Note : World 설정
        D3DXMatrixIdentity( &m_matWorld );

        // Translate ( 모델 높이의 1/2만큼 아래로 내린다 )
        FLOAT fHeight = m_pMeshData->GetHeight();
        m_matWorld._42 = -fHeight/2.0f;

        m_pMeshData->SetWorldMatrix( m_matWorld );
        
        // Note : View 설정.
        D3DXVECTOR3 vFromPt( 0.f, CAMERA_POSITION_Y, m_fCamDistance );
        D3DXVECTOR3 vLookatPt( 0.f, 0.f, 0.f );
        D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
        D3DXMatrixLookAtLH( &m_matView, &vFromPt, &vLookatPt, &vUpVec );
        m_pMeshData->SetViewMatrix( m_matView, vFromPt );

        // Note : Projection 설정.
        const UIRECT& rcRect = m_pMeshRender->GetLocalPos();
        D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI*0.25f, rcRect.sizeX/rcRect.sizeY, 1.f, 1000.f );
        m_pMeshData->SetProjMatrix( m_matProj );
    }
}

void CItemPreviewWindow::SetItem ( const SITEMCUSTOM& sItemCustom )
{
    if ( !m_pMeshData )
    {
        return;
    }

    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

    if ( !pItem )
    {
        return;
    }

    switch ( pItem->sBasicOp.emItemType )
    {
    case ITEM_BOX:
        {
			int nSize = min(ITEM::SBOX::ITEM_SIZE, (int)pItem->sBox.vecItems.size());
            for ( int i=0; i<nSize; ++i )
            {
                SITEMCUSTOM sBoxCustom(pItem->sBox.vecItems[i].nidITEM);

                if ( sBoxCustom.GetNativeID()==SNATIVEID(false) )	
                {
                    continue;
                }

                const SITEM *pBoxItem = GLogicData::GetInstance().GetItem ( sBoxCustom.GetNativeID() );

                if ( !pBoxItem )
                {
                    continue;
                }

                if ( IsPreviewItem(sBoxCustom, m_emCharIndex) )
                {
                    SetItem ( sBoxCustom, Item_2_Slot(pBoxItem) );
                }
            }
        }
        return;
    };

    SetItem ( sItemCustom, Item_2_Slot(pItem) );
}

void CItemPreviewWindow::SetItem ( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot )
{
    if ( !m_pMeshData )
    {
        return;
    }

    if ( SLOT_TSIZE <= emSlot )
    {
        return;
    }

    SNATIVEID nidITEM = sItemCustom.nidDISGUISE;
    if ( nidITEM==SNATIVEID(false) ) nidITEM = sItemCustom.GetNativeID();

    const SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );

    if ( !pItem )
    {
        return;
    }

    EMCHARINDEX emIndex = m_emCharIndex;

    // 아이템에 저장
    EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot ( emSlot );
    
    if ( emPreviewSlot < EMPREVIEWSLOT_SIZE )
    {
        const SITEM* pItemImage = NULL;
        
        if ( sItemCustom.nidDISGUISE==SNATIVEID(false) )
        {
            pItemImage = pItem;
        }
        else
        {
            pItemImage = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

            if ( !pItemImage )
            {
                pItemImage = pItem;
            }
        }

        m_pItemImageArray  [emPreviewSlot]->SetItem( sItemCustom );
        m_sItemCustomArray [emPreviewSlot] = sItemCustom;
    }
    
    // 슬롯에 따라 피스를 리셋시킨다 (과학부 총, attach bone 때문)
    switch ( emSlot )
    {
    case SLOT_LHAND:
    case SLOT_LHAND_S:
        {
            m_emAniSubType = CHECK_ANISUB ( NULL, pItem, emIndex );
            m_pMeshData->ResetPiece( PIECE_LHAND );
            m_pMeshData->ResetPiece( PIECE_RHAND );
        }
        break;

    case SLOT_RHAND:
    case SLOT_RHAND_S:
        {
            m_emAniSubType = CHECK_ANISUB ( pItem, NULL, emIndex );
            m_pMeshData->ResetPiece( PIECE_LHAND );
            m_pMeshData->ResetPiece( PIECE_RHAND );
        }
        break;
    }

    if ( pItem->GetWearingFile(emIndex) && pItem->GetWearingFile(emIndex)[0] )
    {
        SPIECEINFO sPieceInfo;
        sPieceInfo.szFILENAME   = pItem->GetWearingFile(emIndex);
        sPieceInfo.cGRADE       = sItemCustom.GETGRADE_EFFECT();
        sPieceInfo.dwMAINCOLOR  = sItemCustom.dwMainColor;
        sPieceInfo.dwSUBCOLOR   = sItemCustom.dwSubColor;

        if ( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[0] )
        {
            sPieceInfo.szSELFBODYEFF = pItem->GetSelfBodyEffect();
        }

        SetPiece ( sPieceInfo );
    }

    if ( pItem->GetWearingFileEx(emIndex) && pItem->GetWearingFileEx(emIndex)[0] )
    {
        SPIECEINFO sPieceInfo;
        sPieceInfo.szFILENAME   = pItem->GetWearingFileEx(emIndex);
        sPieceInfo.cGRADE       = sItemCustom.GETGRADE_EFFECT();
        sPieceInfo.dwMAINCOLOR  = sItemCustom.dwMainColor;
        sPieceInfo.dwSUBCOLOR   = sItemCustom.dwSubColor;

        if ( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[0] )
        {
            sPieceInfo.szSELFBODYEFF = pItem->GetSelfBodyEffect();
        }

        SetPiece ( sPieceInfo );
    }
}

void CItemPreviewWindow::ResetItem ( const EMSLOT emSlot )
{
    EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot( emSlot );

    if ( emPreviewSlot < EMPREVIEWSLOT_SIZE )
    {
        m_pItemImageArray [emPreviewSlot]->ResetItem();
        m_sItemCustomArray[emPreviewSlot] = SITEMCUSTOM( NATIVEID_NULL() );
    }

    ResetPiece ( SLOT_2_PIECE(emSlot) );
}

void CItemPreviewWindow::ResetAllItem ()
{
    if ( !m_pMeshData )
    {
        return;
    }

    for ( DWORD i=0; i<SLOT_NSIZE_2; ++i )
    {
        ResetItem ( EMSLOT(i) );
    }
}

const EMPIECECHAR CItemPreviewWindow::SetPiece ( const SPIECEINFO& sInfo )
{
    if ( !m_pMeshData )
    {
        return PIECE_NONE;
    }

    // 피스 세팅
    EMPIECECHAR emPieceChar(PIECE_NONE);
    m_pMeshData->SetPiece ( sInfo.szFILENAME.c_str(), NULL, sInfo.cGRADE, !(sInfo.bNONCHARDATA), FALSE, &emPieceChar );

    if ( emPieceChar == PIECE_NONE )
    {
        return emPieceChar;
    }

    // 오버레이 칼라 세팅
    DWORD dwMainColor = 0;
    DWORD dwSubColor  = 0;
    m_pMeshData->GetDefaultOverlayColor( emPieceChar, dwMainColor, dwSubColor );

    if ( sInfo.dwMAINCOLOR != 0 )
    {
        dwMainColor = sInfo.dwMAINCOLOR;
    }

    if ( sInfo.dwSUBCOLOR != 0 )
    {
        dwSubColor = sInfo.dwSUBCOLOR;
    }

    m_pMeshData->SetMainColor( emPieceChar, dwMainColor );
    m_pMeshData->SetSubColor ( emPieceChar, dwSubColor  );

    // 이펙트 세팅
    if ( !sInfo.szSELFBODYEFF.empty() )
    {
        D3DXVECTOR3 vDir(0.0f, 0.0f, 1.0f);
        m_pMeshData->SetEffect ( sInfo.szSELFBODYEFF.c_str(), emPieceChar, vDir );
    }

    return emPieceChar;
}

void CItemPreviewWindow::ResetPiece ( const EMPIECECHAR emPiece )
{
    if ( !m_pMeshData )
    {
        return;
    }

    if ( emPiece == PIECE_NONE )
    {
        return;
    }

    m_pMeshData->ResetPiece( emPiece );
}

const BOOL CItemPreviewWindow::IsPreviewItem ( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

    if ( !pItem )
    {
        return FALSE;
    }

    switch ( pItem->sBasicOp.emItemType )
    {
    case ITEM_BOX:
        {
			int nSize = min(ITEM::SBOX::ITEM_SIZE, (int)pItem->sBox.vecItems.size());
            for ( int i=0; i<nSize; ++i )
            {
                SITEMCUSTOM sBoxCustom(pItem->sBox.vecItems[i].nidITEM);

                if ( sBoxCustom.GetNativeID()==SNATIVEID(false) )	
                {
                    continue;
                }

                const SITEM *pBoxItem = GLogicData::GetInstance().GetItem ( sBoxCustom.GetNativeID() );

                if ( !pBoxItem )
                {
                    continue;
                }

                if ( IsPreviewItem ( sBoxCustom, emCharIndex, Item_2_Slot(pBoxItem) ) )
                {
                    return TRUE;
                }
            }
        }
        return FALSE;
    };

    return IsPreviewItem ( sItemCustom, emCharIndex, Item_2_Slot(pItem) );
}

const BOOL CItemPreviewWindow::IsPreviewItem ( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot )
{
    EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot ( emSlot );

    if ( emPreviewSlot >= EMPREVIEWSLOT_SIZE )
    {
        return FALSE;
    }

    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

    if ( !pItem )
    {
        return FALSE;
    }

    BOOL bCheck = FALSE;

    if ( !pItem->GetWearingFile(emCharIndex) || !pItem->GetWearingFile(emCharIndex)[0] )
    {
        if ( !pItem->GetWearingFileEx(emCharIndex) || !pItem->GetWearingFileEx(emCharIndex)[0] )
        {
            return FALSE;
        }
    }

    return TRUE;
}

const BOOL CItemPreviewWindow::LoadObjectData ( GLCharacter* pCharacter )
{
    if ( !m_pMeshRender || !pCharacter )
    {
        return FALSE;
    }

    // 착용 아이템 정보 날리기
    ResetAllItem ();

    EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);    
	m_pMeshData = m_pMeshRender->LoadSkinChar ( CUIMeshRender::MESH_ID_CHARACTER, GLCONST_CHAR::szCharSkin[emIndex] );

    if ( !m_pMeshData )
    {
        return FALSE;
    }

    m_emCharIndex = emIndex;

    const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

    // Note : Piece 세팅
    SPIECEINFO sPieceInfo;

    sPieceInfo.RESET();
    sPieceInfo.szFILENAME = sCONST.strHEAD_CPS[pCharacter->m_wFace];
    SetPiece( sPieceInfo );

    sPieceInfo.RESET();
    sPieceInfo.szFILENAME  = sCONST.strHAIR_CPS[pCharacter->m_wHair];
    sPieceInfo.dwMAINCOLOR = ConvertColor(pCharacter->m_wHairColor);
    SetPiece( sPieceInfo );

    for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
    {	
        EMSLOT emSlot = EMSLOT(i);

        // 현재 장착중인 무기가 아니면 넘어간다.
        if( !pCharacter->IsCurUseArm( emSlot ) )
        {
            continue;
        }

        // 복장 아이템
        const SITEMCUSTOM &ItemCustom = pCharacter->m_PutOnItems[i];

        if ( !IsPreviewItem ( ItemCustom, m_emCharIndex, emSlot ) )
        {
            continue;
        }

        SetItem( ItemCustom, emSlot );
    }

    InitGeometry ();

    // 동작 애니메이션
    EMSLOT emRHand = pCharacter->GetCurRHand();
    EMSLOT emLHand = pCharacter->GetCurLHand();
    m_emAniSubType = CHECK_ANISUB ( pCharacter->m_pITEMS[emRHand], pCharacter->m_pITEMS[emLHand], pCharacter->GETCHARINDEX() );

    m_pMeshData->SelectAni     ( AN_GUARD_N, m_emAniSubType );
//    m_pMeshData->SetAttackMode ( TRUE );
    m_pMeshData->FrameMove     ( m_pMeshRender->GetDevice(), 20.0f );

    return TRUE;
}

void CItemPreviewWindow::DeleteUIMeshData()
{
    if( m_pMeshRender )
	    m_pMeshRender->DeleteMeshData( CUIMeshRender::MESH_ID_CHARACTER );

	m_pMeshData = NULL;
}
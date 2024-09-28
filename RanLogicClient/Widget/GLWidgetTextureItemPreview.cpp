#include "pch.h"
#include "GLWidgetTextureItemPreview.h"
#include "../GLGaeaClient.h"

#include "../enginelib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicEx.h"
#include "../RanLogic/Item/GLItem.h"

#include "../RanLogicClient/ItemPreview/GLItemPreviewClient.h"
#include "../RanLogicClient/GfxInterfaceBase.h"

#include "../Widget/GLWidgetScript.h"

#define ItemPreviewTexture_OrigViewPos D3DXVECTOR3( 0.0f, 0.0f, -35.0f )

GLWidgetTextureItemPreview::GLWidgetTextureItemPreview( LPDIRECT3DDEVICEQ pDevice, ItemPreview::GLItemPreviewClient* pItemPreviewClient )
: GLWidgetTextureChar( pDevice )
, m_pItemPreviewClient( pItemPreviewClient )
{
	for( int i=0; i<GLWidgetTextureChar::EMPREVIEWSLOT_SIZE; ++i )
	{
		m_sItemCustomArray[i] = SITEMCUSTOM( NATIVEID_NULL() );
	}

	if( pItemPreviewClient )
		m_fTurnSpeed = pItemPreviewClient->GetTurnSpeed();
}

GLWidgetTextureItemPreview::~GLWidgetTextureItemPreview(void)
{
}

bool GLWidgetTextureItemPreview::OnCreate()
{
	if( GLWidgetTextureChar::OnCreate() )
	{
		m_vViewPos = ItemPreviewTexture_OrigViewPos;
		return true;
	}

	return false;
}

void GLWidgetTextureItemPreview::OnFrameMove( float fElapsedTime )
{
	GLWidgetTextureChar::OnFrameMove( fElapsedTime );

	if( !m_pItemPreviewClient )
		return;

	if( m_pItemPreviewClient->IsTurnLeft() )
		TurnLeft( m_fTurnSpeed*fElapsedTime );

	if( m_pItemPreviewClient->IsTurnRight() )
		TurnRight( m_fTurnSpeed*fElapsedTime );
}

const EMSLOT GLWidgetTextureItemPreview::Item_2_Slot ( const SITEMCUSTOM& sItemCustom )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

	if ( !pItem )
		return SLOT_TSIZE;

	return Item_2_Slot ( pItem );
}

const EMSLOT GLWidgetTextureItemPreview::Item_2_Slot ( const SITEM* pItem )
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
		if( pItem->sSuitOp.emHand & HAND_RIGHT )
			emSlot = SLOT_RHAND;
		break;
	}

	return emSlot;
}

GLWidgetTextureChar::EMPREVIEWSLOT GLWidgetTextureItemPreview::Slot_2_PreviewSlot( const EMSLOT emSlot )
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
	};

	return EMPREVIEWSLOT_SIZE;
}

void GLWidgetTextureItemPreview::TurnLeft( float fAngle )
{
	m_fRotate -= fAngle;

	if ( m_fRotate < 0.0f )
		m_fRotate = (D3DX_PI*2.0f) + m_fRotate;
	else if ( m_fRotate > D3DX_PI*2.0f )
		m_fRotate = m_fRotate - (D3DX_PI*2.0f);
}

void GLWidgetTextureItemPreview::TurnRight( float fAngle )
{
	m_fRotate += fAngle;

	if ( m_fRotate < 0.0f )
		m_fRotate = (D3DX_PI*2.0f) + m_fRotate;
	else if ( m_fRotate > D3DX_PI*2.0f )
		m_fRotate = m_fRotate - (D3DX_PI*2.0f);
}

void GLWidgetTextureItemPreview::SetSlotItem( const GLWidgetTextureChar::EMPREVIEWSLOT emSlot, const SITEMCUSTOM& sItemCustom )
{
	lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	tbItem.set( 1, emSlot );
	tbItem.set( 2, sItemCustom.Mid() );
	tbItem.set( 3, sItemCustom.Sid() );

	GLWidgetCallFuncValue arg;
	arg.SetTable( &tbItem );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_ItemPreview_SetSlot, &arg, 1 );

	GLCharacter* pCharacter = GfxInterfaceBase::GetInstance()->GetGaeaClient()->GetCharacter();
	if( NULL == pCharacter )
		return;

	SITEM* pItem = GLogicData::GetInstance().GetItem( 
		m_sItemCustomArray[ EMPREVIEWSLOT_WEAPON ].GetNativeID() );
	if( NULL == pItem )
	{
		EMSLOT emSlot = false == pCharacter->IsUseArmSub() ? SLOT_RHAND : SLOT_LHAND;
		pItem = GLogicData::GetInstance().GetItem( pCharacter->m_PutOnItems[ emSlot ].GetNativeID() );
	}

	SetAni( AN_GUARD_N, CHECK_ANISUB( pItem, NULL, pCharacter->GETCHARINDEX() ) );
}

void GLWidgetTextureItemPreview::SetSkinChar( GLCHARLOGIC* pChar )
{
	EMCHARINDEX emIndex = CharClassToIndex( pChar->m_emClass );
	DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( 
		GLCONST_CHAR::szCharSkin[ emIndex ], m_pDevice );
	if( NULL == pSkinCharData )
		return;

	SAFE_DELETE( m_pSkinChar );

	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData( pSkinCharData, m_pDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_JIGGLE_BONE );

	m_emCharIndex = emIndex;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// Note : Piece 세팅
	SPIECEINFO sPieceInfo;

	sPieceInfo.RESET();
	sPieceInfo.szFILENAME = sCONST.strHEAD_CPS[ pChar->m_wFace ];
	SetPiece( sPieceInfo );

	sPieceInfo.RESET();
	sPieceInfo.szFILENAME  = sCONST.strHAIR_CPS[ pChar->m_wHair ];
	sPieceInfo.dwMAINCOLOR = ConvertColor( pChar->m_wHairColor );
	SetPiece( sPieceInfo );

	for( int i = 0; i < SLOT_NSIZE_S_2; ++i )
	{	
		EMSLOT emSlot = EMSLOT( i );

		// 현재 장착중인 무기가 아니면 넘어간다.
		if( FALSE == pChar->IsCurUseArm( emSlot ) )
			continue;

		SITEMCUSTOM sItemCustom = pChar->m_PutOnItems[ i ];
		GLWidgetTextureChar::EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot( emSlot );
		if( emPreviewSlot != EMPREVIEWSLOT_SIZE )
		{
			if( m_sItemCustomArray[emPreviewSlot].GetNativeID() != NATIVEID_NULL() )
			{
				sItemCustom = m_sItemCustomArray[emPreviewSlot];
			}
		}

		if( FALSE == IsPreviewItem( sItemCustom, emIndex, emSlot ) )
			continue;

		SetItem( sItemCustom, emSlot );

		GLWidgetTextureChar::EMPREVIEWSLOT emPreSlot = Slot_2_PreviewSlot(emSlot);
		SetSlotItem( emPreSlot, sItemCustom );
	}
}

void GLWidgetTextureItemPreview::SetPreviewItem( const SITEMCUSTOM& sItemCustom )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

	if ( !pItem )
		return;

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

				EMSLOT emSlot = Item_2_Slot( pBoxItem );
				if ( IsPreviewItem( sBoxCustom, m_emCharIndex, emSlot ) )
				{
					GLWidgetTextureChar::EMPREVIEWSLOT emPreSlot = Slot_2_PreviewSlot(emSlot);
					m_sItemCustomArray[emPreSlot] = sBoxCustom;

					SetSlotItem( emPreSlot, sBoxCustom );
				}
			}

			GLWidgetScript::GetInstance().UpdateItemPreviewRT();
		}
		return;
	};

	EMSLOT emSlot = Item_2_Slot( sItemCustom );
	if ( IsPreviewItem( sItemCustom, m_emCharIndex, emSlot ) )
	{
		GLWidgetTextureChar::EMPREVIEWSLOT emPreSlot = Slot_2_PreviewSlot(emSlot);
		m_sItemCustomArray[emPreSlot] = sItemCustom;

		SetSlotItem( emPreSlot, sItemCustom );
	}

	GLWidgetScript::GetInstance().UpdateItemPreviewRT();
}

void GLWidgetTextureItemPreview::ResetPreviewItem()
{
	m_vViewPos = ItemPreviewTexture_OrigViewPos;

	for( int i=0; i<GLWidgetTextureChar::EMPREVIEWSLOT_SIZE; ++i )
		m_sItemCustomArray[i] = SITEMCUSTOM( NATIVEID_NULL() );

	GLWidgetScript::GetInstance().UpdateItemPreviewRT();
}

void GLWidgetTextureItemPreview::MoveViewPosZ( float fValue )
{
	m_vViewPos.z += fValue;
}
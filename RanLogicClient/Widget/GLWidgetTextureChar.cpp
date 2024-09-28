#include "pch.h"
#include "GLWidgetTextureChar.h"

#include "../../enginelib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../enginelib/DxEffect/Char/DxEffCharData.h"
#include "../../enginelib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../enginelib/DxTools/RENDERPARAM.h"
#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Item/GLItem.h"

GLWidgetTextureChar::GLWidgetTextureChar( LPDIRECT3DDEVICEQ pDevice )
:	GLWidgetTexture( pDevice )
,	m_pSkinChar( NULL )
,	m_fRotate( 0.0f )
,	m_emAniMainType( AN_PLACID )
,	m_emAniSubType( AN_SUB_NONE )
,	m_fTime( 0.0f )
{
}

GLWidgetTextureChar::~GLWidgetTextureChar(void)
{
	SAFE_DELETE( m_pSkinChar );
}

bool GLWidgetTextureChar::OnCreate()
{
	if( false == GLWidgetTexture::OnCreate() )
		return false;

	m_vViewPos = D3DXVECTOR3( 0.0f, 10.0f, -25.0f );

	return true;
}

void GLWidgetTextureChar::OnResetDevice()
{
	GLWidgetTexture::OnResetDevice();

	if( NULL != m_pSkinChar )
		m_pSkinChar->RestoreDeviceObjects( m_pDevice );
}

void GLWidgetTextureChar::OnLostDevice()
{
	GLWidgetTexture::OnLostDevice();

	if( NULL != m_pSkinChar )
		m_pSkinChar->InvalidateDeviceObjects();
}

void GLWidgetTextureChar::OnFrameMove( float fElapsedTime )
{
	if( NULL == m_pSkinChar )
		return;

	m_fTime += fElapsedTime;

	D3DXVECTOR3 vLook( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_matView, &m_vViewPos, &vLook, &vUp );
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI / 4.0f, m_fAspect, 0.1f, 2000.0f );

	D3DXVECTOR3 vMax,vMin;
	m_pSkinChar->GetAABBBox( vMax, vMin );
	float fHeight = vMax.y - vMin.y;

	D3DXMATRIX matT, matR;
	D3DXMatrixTranslation( &matT, 0.0f, -fHeight / 2.0f, 0.0f );
	D3DXMatrixRotationY( &matR, m_fRotate );
	m_matWorld = matR * matT;

	SelectAni( m_emAniMainType, m_emAniSubType );

	m_pSkinChar->FrameMove( m_fTime, fElapsedTime, FALSE, m_matWorld );
}

void GLWidgetTextureChar::OnRender()
{
	if( NULL == m_pSkinChar )
		return;

	m_pDevice->SetTransform( D3DTS_VIEW, &m_matView );
	m_pDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
	m_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	NSMaterialSkinManager::SetMob( FALSE );

	// View 와 Projection 등 셋팅.
	{
		NSMaterialSkinManager::FrameMove( m_matView, m_matProj );
	}

	// Render
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		m_pSkinChar->RenderFixed( m_pDevice, FALSE, 1.0f );
	}
	else
	{
		D3DXVECTOR3 vDiffuse(1.f,1.f,1.f);
		D3DXVECTOR3 PointLight(0.f,0.f,0.f);
		m_pSkinChar->SetWorldLight( vDiffuse, PointLight );
		m_pSkinChar->SetPhysX_Cloth( FALSE );
		m_pSkinChar->RenderMaterialOpaque ( m_pDevice, 1.0f );
		m_pSkinChar->RenderMaterialHardAlpha ( m_pDevice, 1.0f );
		m_pSkinChar->RenderMaterialSoftAlpha ( m_pDevice, 1.0f );
		m_pSkinChar->RenderMaterialEffect ( m_pDevice, 1.0f );
	}

	// 되돌린다.
	{
		NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
	}
}

void GLWidgetTextureChar::CleanUp()
{
	SAFE_DELETE( m_pSkinChar );
}

void GLWidgetTextureChar::SelectAni( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
	if( NULL == m_pSkinChar )
		return;

	if( m_pSkinChar->GETCURMTYPE() == MType && m_pSkinChar->GETCURSTYPE() == SType )
		return;

	m_pSkinChar->SELECTANI( MType, SType, dwFlags, dwSelect );
}

void GLWidgetTextureChar::SetSkinChar( GLCHARLOGIC* pChar )
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

		// 복장 아이템
		const SITEMCUSTOM &ItemCustom = pChar->m_PutOnItems[ i ];
		if( FALSE == IsPreviewItem( ItemCustom, emIndex, emSlot ) )
			continue;

		SetItem( ItemCustom, emSlot );
	}
}

void GLWidgetTextureChar::SetPiece( const SPIECEINFO& sInfo )
{
	if( NULL == m_pSkinChar )
		return;

	// 피스 세팅
	EMPIECECHAR emPieceChar = PIECE_NONE;
	m_pSkinChar->SetPiece( sInfo.szFILENAME.c_str(), m_pDevice, 0,
		sInfo.cGRADE, EMSCD_ALL_THREAD, FALSE, &emPieceChar );
	if( PIECE_NONE == emPieceChar )
		return;

	// 오버레이 칼라 세팅
	DWORD dwMainColor = 0;
	DWORD dwSubColor  = 0;

	{
		PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int) emPieceChar );
		if( NULL != pCharPart )
			pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
	}

	if( 0 != sInfo.dwMAINCOLOR )
		dwMainColor = sInfo.dwMAINCOLOR;
	if( 0 != sInfo.dwSUBCOLOR )
		dwSubColor = sInfo.dwSUBCOLOR;

	{
		PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int) emPieceChar );
		if( NULL != pCharPart )
			pCharPart->SetHairColor( dwMainColor );
	}
	{
		PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int) emPieceChar );
		if( NULL != pCharPart )
			pCharPart->SetSubColor( dwSubColor );
	}

	// 이펙트 세팅
	if( false == sInfo.szSELFBODYEFF.empty() )
	{
		D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );

		PDXCHARPART pCharPart = m_pSkinChar->GetPiece( (int) emPieceChar );
		if( NULL != pCharPart && pCharPart->GetFileName()[ 0 ] )
		{
			DxEffcharDataMan::GetInstance().PutEffect ( pCharPart, sInfo.szSELFBODYEFF.c_str(), &vDir );
			return;
		}

		PDXATTBONELINK pAttPart = m_pSkinChar->GetAttachBone( (int) emPieceChar );
		if( NULL != pAttPart && pAttPart->GetFileName()[ 0 ] )
		{
			DxEffcharDataMan::GetInstance().PutEffect ( pAttPart, sInfo.szSELFBODYEFF.c_str(), &vDir );
			return;
		}
	}
}

void GLWidgetTextureChar::SetItem( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot )
{
	if( NULL == m_pSkinChar )
		return;

	if( SLOT_TSIZE <= emSlot )
		return;

	SNATIVEID nidITEM = sItemCustom.nidDISGUISE;
	if( nidITEM == SNATIVEID( false ) )
		nidITEM = sItemCustom.GetNativeID();

	SITEM* pItem = GLogicData::GetInstance().GetItem( nidITEM );
	if( NULL == pItem )
		return;

	EMCHARINDEX emIndex = m_emCharIndex;

	// 아이템에 저장
	EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot( emSlot );
	if( EMPREVIEWSLOT_SIZE != emPreviewSlot )
	{
		SITEM* pItemImage = NULL;
		if( sItemCustom.nidDISGUISE == SNATIVEID( false ) )
		{
			pItemImage = pItem;
		}
		else
		{
			pItemImage = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
			if( NULL == pItemImage )
				pItemImage = pItem;
		}
	}

	// 슬롯에 따라 피스를 리셋시킨다 (과학부 총, attach bone 때문)
	switch( emSlot )
	{
	case SLOT_LHAND:
	case SLOT_LHAND_S:
	case SLOT_RHAND:
	case SLOT_RHAND_S:
		{
			m_pSkinChar->ResetPiece( (int) PIECE_LHAND );
			m_pSkinChar->ResetPiece( (int) PIECE_RHAND );
		}
		break;
	}

	if( pItem->GetWearingFile( emIndex ) && pItem->GetWearingFile( emIndex )[ 0 ] )
	{
		SPIECEINFO sPieceInfo;
		sPieceInfo.szFILENAME   = pItem->GetWearingFile( emIndex );
		sPieceInfo.cGRADE       = sItemCustom.GETGRADE_EFFECT();
		sPieceInfo.dwMAINCOLOR  = sItemCustom.dwMainColor;
		sPieceInfo.dwSUBCOLOR   = sItemCustom.dwSubColor;

		if( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[ 0 ] )
			sPieceInfo.szSELFBODYEFF = pItem->GetSelfBodyEffect();

		SetPiece ( sPieceInfo );
	}

	if( pItem->GetWearingFileEx( emIndex ) && pItem->GetWearingFileEx( emIndex )[ 0 ] )
	{
		SPIECEINFO sPieceInfo;
		sPieceInfo.szFILENAME   = pItem->GetWearingFileEx( emIndex );
		sPieceInfo.cGRADE       = sItemCustom.GETGRADE_EFFECT();
		sPieceInfo.dwMAINCOLOR  = sItemCustom.dwMainColor;
		sPieceInfo.dwSUBCOLOR   = sItemCustom.dwSubColor;

		if( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[ 0 ] )
			sPieceInfo.szSELFBODYEFF = pItem->GetSelfBodyEffect();

		SetPiece ( sPieceInfo );
	}
}

void GLWidgetTextureChar::SetAni( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType )
{
	m_emAniMainType = MType;
	m_emAniSubType = SType;
}

DWORD GLWidgetTextureChar::ConvertColor( const WORD wColor )
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

GLWidgetTextureChar::EMPREVIEWSLOT GLWidgetTextureChar::Slot_2_PreviewSlot( const EMSLOT emSlot )
{
	switch ( emSlot )
	{
	case SLOT_HEADGEAR :
		return EMPREVIEWSLOT_HEAD;

	case SLOT_UPPER :
		return EMPREVIEWSLOT_UPPER;

	case SLOT_LOWER :
		return EMPREVIEWSLOT_LOWER;

	case SLOT_HAND :
		return EMPREVIEWSLOT_HAND;

	case SLOT_FOOT :
		return EMPREVIEWSLOT_FOOT;

	case SLOT_RHAND :
	case SLOT_RHAND_S :
	case SLOT_LHAND :
	case SLOT_LHAND_S :
		return EMPREVIEWSLOT_WEAPON;

	// 기획팀에서 셋팅을 코스츔이 되도록 해버렸다. 지원해줘야한다. (SLOT_DECORATION,SLOT_EARRINGS)
	case SLOT_DECORATION:   
		return EMPREVIEWSLOT_DECORATION;	// 장식

	case SLOT_EARRINGS:     
		return EMPREVIEWSLOT_EARRINGS;		// 귀걸이
	};

	return EMPREVIEWSLOT_SIZE;
}

BOOL GLWidgetTextureChar::IsPreviewItem( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot )
{
	EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot ( emSlot );

	if( emPreviewSlot == EMPREVIEWSLOT_SIZE )
		return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
	if( NULL == pItem )
		return FALSE;

	if( !pItem->GetWearingFile(emCharIndex) || !pItem->GetWearingFile(emCharIndex)[ 0 ] )
	{
		if( !pItem->GetWearingFileEx(emCharIndex) || !pItem->GetWearingFileEx(emCharIndex)[ 0 ] )
			return FALSE;
	}

	return TRUE;
}

const float GLWidgetTextureChar::GetHeight()
{
	if ( !m_pSkinChar )
	{
		return 0.0f;
	}

	D3DXVECTOR3 vMax,vMin;
	m_pSkinChar->GetAABBBox( vMax, vMin );
	return (vMax.y - vMin.y);
}

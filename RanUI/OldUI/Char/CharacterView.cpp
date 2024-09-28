

#include "stdafx.h"

#include "CharacterView.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicEx.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../Util/UIMeshData_SkinChar.h"
#include "../Util/UIMeshData_SkinVehicle.h"

#include "../../../RanLogicClient/Land/GLLandManClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
//	const float CAMERA_POSITION_Y       = 0.0f;
	const float CAMERA_POSITION_Z       = -27.0f; 

	
	VEHICLE_TYPE GetVehicleType(const SNATIVEID& id)
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		const VEHICLE_TYPE vType = pItem->VehicleType();
		GASSERT( vType == VEHICLE_TYPE_BIKE || vType == VEHICLE_TYPE_BOARD || vType == VEHICLE_TYPE_CAR || vType == VEHICLE_TYPE_REARCAR );
		return pItem->VehicleType() ;
	}

	EMANI_MAINTYPE GetBikeMainAniType(const SNATIVEID& id)
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		GASSERT(pItem->VehicleType() == VEHICLE_TYPE_BIKE);
		return EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
	}

	EMANI_MAINTYPE GetMainAniType(const SNATIVEID& id)
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( id );
		GASSERT(pItem->VehicleType() != VEHICLE_TYPE_BOARD);
		switch ( pItem->sVehicle.emVehicleType )
		{
		case VEHICLE_TYPE_BIKE:
			return EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
		case VEHICLE_TYPE_CAR:
			return EMANI_MAINTYPE( AN_CAR_A +  pItem->sVehicle.emPartsType );
		case VEHICLE_TYPE_REARCAR:
			return EMANI_MAINTYPE( AN_REARCAR_A +  pItem->sVehicle.emPartsType );
		}
		// Default
		return AN_GUARD_N;
	}
}

CharacterView::CharacterView(EngineDeviceMan* pEngineDevice) : CUIMeshRender(pEngineDevice)
{
	m_CamPosX = 0;
	m_CamPosY = 0;
	m_CamPosZ = CAMERA_POSITION_Z;

	m_ObjectPosX = 0;
	m_ObjectPosY = 0;
	m_ObjectPosZ = 0;


	m_pMeshCharacterData = NULL;
	m_pMeshVehicleData = NULL;

	m_Scale = 1.0f;
	m_RotationY = 0;
	m_emAniMainType = AN_PLACID;
	m_emAniSubType  = AN_SUB_NONE;

	
	m_initMatrix = false;

	m_ForAniVehicleID = NATIVEID_NULL(); 
	m_VehicleAni = AN_TYPE_SIZE;
	m_BikeAni = AN_SUB_SIZE;
	m_BoardAni = AN_TYPE_SIZE;

	m_pActor = NULL;
	m_pGaeClient = NULL;
}

// void CharacterView::SetCameraZ( float z )
// {
// 	m_fCamDistance = z;
// }

BOOL CharacterView::LoadCharacter( GLCHARLOGIC* pCharacter )
{
	GASSERT(pCharacter);

	m_BackUpCharacterData = MakeBackupCharacterData(pCharacter);
	// 착용 아이템 정보 날리기
	ResetAllItem();

	EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);    
	m_pMeshCharacterData = LoadSkinChar( MESH_ID_CHARACTER, GLCONST_CHAR::szCharSkin[emIndex] );

	if ( !m_pMeshCharacterData )
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

		if ( !IsPreviewItem ( ItemCustom, emIndex, emSlot ) )
		{
			continue;
		}

		SetItem( ItemCustom, emSlot );
	}

	SetCharacterMatrix();

	// 동작 애니메이션
// 	EMSLOT emRHand = pCharacter->GetCurRHand();
// 	EMSLOT emLHand = pCharacter->GetCurLHand();
// 	m_emAniSubType = CHECK_ANISUB ( pCharacter->m_pITEMS[emRHand], pCharacter->m_pITEMS[emLHand]  );

	m_pMeshCharacterData->SelectAni     ( AN_PLACID, AN_SUB_NONE );
//	m_pMeshData->SetAttackMode ( TRUE );
	m_pMeshCharacterData->FrameMove     ( GetDevice(), 20.0f );

	return TRUE;
}

void CharacterView::InitGeometry()
{
	if( m_initMatrix )
		return;

	m_initMatrix = true;

	// Note : World 설정
	D3DXMatrixIdentity( &m_matWorld );

	D3DXMatrixScaling(&m_matWorld,m_Scale,m_Scale,m_Scale);



//	m_pMeshCharacterData->SetWorldMatrix( m_matWorld );

	// Note : View 설정.
	D3DXVECTOR3 vFromPt( m_CamPosX, m_CamPosY, m_CamPosZ );
	D3DXVECTOR3 vLookatPt( 0.f, 0.f, 0.f );
	D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	m_vCameraFromPt = vFromPt;
	D3DXMatrixLookAtLH( &m_matView, &vFromPt, &vLookatPt, &vUpVec );
//	m_pMeshCharacterData->SetViewMatrix( m_matView );

	// Note : Projection 설정.
	const UIRECT& rcRect = GetLocalPos();
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI*0.25f, rcRect.sizeX/rcRect.sizeY, 1.f, 1000.f );
//	m_pMeshCharacterData->SetProjMatrix( m_matProj );

	FLOAT fHeight = m_pMeshCharacterData->GetHeight();
	SetPosition(0, -fHeight/2.0f,0);
}

void CharacterView::ResetAllItem()
{
	if ( !m_pMeshCharacterData )
	{
		return;
	}

	for ( DWORD i=0; i<SLOT_NSIZE_2; ++i )
	{
		ResetItem ( EMSLOT(i) );
	}
}

void CharacterView::ResetItem( EMSLOT emSlot )
{
	ResetPiece( SLOT_2_PIECE(emSlot) );
}


void CharacterView::ResetPiece( EMPIECECHAR emPiece )
{
	if ( !m_pMeshCharacterData )
	{
		return;
	}

	if ( emPiece == PIECE_NONE )
	{
		return;
	}

	m_pMeshCharacterData->ResetPiece( emPiece );
}


EMPIECECHAR CharacterView::SetPiece( const SPIECEINFO& sInfo )
{
	GASSERT( m_pMeshCharacterData );

	// 피스 세팅
	EMPIECECHAR emPieceChar(PIECE_NONE);
	m_pMeshCharacterData->SetPiece( sInfo.szFILENAME.c_str(), NULL, sInfo.cGRADE, !(sInfo.bNONCHARDATA), FALSE, &emPieceChar );

	if ( emPieceChar == PIECE_NONE )
	{
		return emPieceChar;
	}

	// 오버레이 칼라 세팅
	DWORD dwMainColor = 0;
	DWORD dwSubColor  = 0;
	m_pMeshCharacterData->GetDefaultOverlayColor( emPieceChar, dwMainColor, dwSubColor );

	if ( sInfo.dwMAINCOLOR != 0 )
	{
		dwMainColor = sInfo.dwMAINCOLOR;
	}

	if ( sInfo.dwSUBCOLOR != 0 )
	{
		dwSubColor = sInfo.dwSUBCOLOR;
	}

	m_pMeshCharacterData->SetMainColor( emPieceChar, dwMainColor );
	m_pMeshCharacterData->SetSubColor ( emPieceChar, dwSubColor  );

	// 이펙트 세팅
	if ( !sInfo.szSELFBODYEFF.empty() )
	{
		D3DXVECTOR3 vDir(0.0f, 0.0f, 1.0f);
		m_pMeshCharacterData->SetEffect( sInfo.szSELFBODYEFF.c_str(), emPieceChar, vDir );
	}

	return emPieceChar;
}

CharacterView::EMPREVIEWSLOT CharacterView::Slot_2_PreviewSlot( const EMSLOT emSlot )
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
	case SLOT_DECORATION:   return EMPREVIEWSLOT_DECORATION;	// 장식
	case SLOT_EARRINGS:     return EMPREVIEWSLOT_EARRINGS;		// 귀걸이
	};

	return EMPREVIEWSLOT_SIZE;
}

DWORD CharacterView::ConvertColor( const WORD wColor )
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



BOOL CharacterView::IsPreviewItem( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot )
{
	EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot ( emSlot );

	if ( emPreviewSlot == EMPREVIEWSLOT_SIZE )
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


void CharacterView::SetItem( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot )
{
	if ( !m_pMeshCharacterData )
	{
		return;
	}

	if ( SLOT_TSIZE <= emSlot )
	{
		return;
	}

	SNATIVEID nidITEM = sItemCustom.nidDISGUISE;
	if ( nidITEM==SNATIVEID(false) ) nidITEM = sItemCustom.GetNativeID();

	SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );

	if ( !pItem )
	{
		return;
	}

	EMCHARINDEX emIndex = m_emCharIndex;

	// 아이템에 저장
	EMPREVIEWSLOT emPreviewSlot = Slot_2_PreviewSlot ( emSlot );

	if ( emPreviewSlot != EMPREVIEWSLOT_SIZE )
	{
		SITEM* pItemImage = NULL;

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

// 		m_pItemImageArray  [emPreviewSlot]->SetItem( pItemImage->sBasicOp.sICONID, pItemImage->GetInventoryFile(), pItemImage->sBasicOp.sNativeID );
// 		m_sItemCustomArray [emPreviewSlot] = sItemCustom;
	}

	// 슬롯에 따라 피스를 리셋시킨다 (과학부 총, attach bone 때문)
	switch ( emSlot )
	{
	case SLOT_LHAND:
	case SLOT_LHAND_S:
		{
//			m_emAniSubType = CHECK_ANISUB ( NULL, pItem  );
			m_pMeshCharacterData->ResetPiece( PIECE_LHAND );
			m_pMeshCharacterData->ResetPiece( PIECE_RHAND );
		}
		break;

	case SLOT_RHAND:
	case SLOT_RHAND_S:
		{
//			m_emAniSubType = CHECK_ANISUB ( pItem, NULL );
			m_pMeshCharacterData->ResetPiece( PIECE_LHAND );
			m_pMeshCharacterData->ResetPiece( PIECE_RHAND );
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



void CharacterView::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_pMeshCharacterData || m_pMeshVehicleData )
	{
		if(m_pMeshCharacterData && m_pActor && m_pGaeClient)
		{
			BOOL isPeaceZone = m_pGaeClient->GetActiveMap()->IsPeaceZone();
			if ( !isPeaceZone && m_pActor->IsActState(EM_ACT_PEACEMODE) )	
				isPeaceZone = TRUE;

			m_pMeshCharacterData->SetPeaceMode( isPeaceZone );
		}

		if(m_pMeshCharacterData)
			m_pMeshCharacterData->SelectAni( m_emAniMainType, m_emAniSubType );

		D3DXMATRIX scale,rotY;
		D3DXMatrixScaling(&scale,m_Scale,m_Scale,m_Scale);
		D3DXMatrixRotationY(&rotY,m_RotationY);
		D3DXMatrixMultiply(&m_matWorld,&scale,&rotY);


		// Translate ( 모델 높이의 1/2만큼 아래로 내린다 )
 		m_matWorld._41 = m_ObjectPosX;
 		m_matWorld._42 = m_ObjectPosY;
 		m_matWorld._43 = m_ObjectPosZ;

		if(m_pMeshVehicleData)
			m_pMeshVehicleData->SetWorldMatrix( m_matWorld );

		if ( m_pMeshCharacterData && m_pMeshVehicleData )
		{
			// 비히클의 월드행렬이 위의 행렬 변환으로 인해서 변하게됨;
			// 캐릭터는 비히클의 어태치본을 찾아서 설정하기 때문에 여기서;
			// 설정하는 월드 행렬이 충첩이된다;
			D3DXMATRIX matWorld;// * m_matWorld;
			m_pMeshVehicleData->GetDriverMatrix( matWorld );
			m_pMeshCharacterData->SetWorldMatrix( matWorld );
		}
		else if(m_pMeshCharacterData)
			m_pMeshCharacterData->SetWorldMatrix( m_matWorld );

		// View 설정.
		D3DXVECTOR3 vFromPt( m_CamPosX, m_CamPosY, m_CamPosZ );
		D3DXVECTOR3 vLookatPt( 0.f, 0.f, 0.f );
		D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
		m_vCameraFromPt = vFromPt;
		D3DXMatrixLookAtLH( &m_matView, &vFromPt, &vLookatPt, &vUpVec );
	
		if(m_pMeshCharacterData)
			m_pMeshCharacterData->SetViewMatrix( m_matView, vFromPt );

		if(m_pMeshVehicleData)
			m_pMeshVehicleData->SetViewMatrix( m_matView, vFromPt );


	}

	CUIMeshRender::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CharacterView::SetUpdateCharacter( GLCHARLOGIC* pCharacter,bool isForce )
{
	if( isForce )
	{
		LoadCharacter(pCharacter);
		return;
	}

	m_UpdateBackUpCharacterData = MakeBackupCharacterData(pCharacter);
	if( m_BackUpCharacterData != m_UpdateBackUpCharacterData || !m_pMeshCharacterData )
		LoadCharacter(pCharacter);
}

CharacterView::BackupCharacterData CharacterView::MakeBackupCharacterData(GLCHARLOGIC* pCharacter)
{
	GASSERT(pCharacter);

	BackupCharacterData backupData;
	EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);    
	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// Note : Piece 세팅
	SPIECEINFO sPieceInfo;
	sPieceInfo.RESET();
	sPieceInfo.szFILENAME = sCONST.strHEAD_CPS[pCharacter->m_wFace];
	
	backupData.Head = sPieceInfo;

	sPieceInfo.RESET();
	sPieceInfo.szFILENAME  = sCONST.strHAIR_CPS[pCharacter->m_wHair];
	sPieceInfo.dwMAINCOLOR = ConvertColor(pCharacter->m_wHairColor);
	
	backupData.Hair = sPieceInfo; 


	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 복장 아이템
		backupData.Item[i] = pCharacter->m_PutOnItems[i];
	}

	backupData.isUseArmSub = pCharacter->IsUseArmSub() ? true : false;

	return backupData;
}

//////////////////////////////////////////////////////////////////////////


BOOL CharacterView::LoadVehicle(const VehicleData& data)
{
	// 메쉬 캐릭터가 반드시 있어야함
	GASSERT(m_pMeshCharacterData);

	//
	DeleteMeshData(MESH_ID_VEHICLE);
	m_pMeshVehicleData = NULL;

	ResetItem(SLOT_VEHICLE);
	
//	EMCHARINDEX emIndex = CharClassToIndex(pCharacter->m_emClass);   
	m_BackUpVehicleData = data;

	if( GetVehicleType(data.VehicleID) == VEHICLE_TYPE_BOARD )
	{
		//////////////////////////////////////////////////////////////////////////

		float camY = 0;
		SetCamPosY(camY);
		SetCamPosZ(-35);

		float x=0,y=0,z=0;
		FLOAT fHeight = m_pMeshCharacterData->GetHeight();

		// + 2는 보드 크기로 추정한 하드코딩값
		// 남자는 20 여자는 17.91
		// 16 이 이상적인 위치로 보임
		SetPosition(x,-fHeight + (fHeight-16),z);

		float rot = -70;
		SetRotationY(rot);


		//////////////////////////////////////////////////////////////////////////
		SetItem( data.VehicleID , SLOT_VEHICLE );
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
 		float camY = 10;
 		SetCamPosY(camY);

		SetCamPosZ(-35);

		FLOAT fHeight = m_pMeshCharacterData->GetHeight();
		float x=5;
		float y=-fHeight/2 + 3;
		float z=0;
		SetPosition(x,y,z);

		float rot = 70;
		SetRotationY(rot);
		//////////////////////////////////////////////////////////////////////////

		const SITEM* pItem = GLogicData::GetInstance().GetItem ( data.VehicleID );
        if ( pItem == NULL )
        {
            return FALSE;
        }
		std::string vehicleName = pItem->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType);
        GASSERT( !vehicleName.empty() );
        if ( vehicleName.empty() )
        {
            return FALSE;
        }
		m_pMeshVehicleData = LoadSkinVehicle(MESH_ID_VEHICLE,vehicleName);
		for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
		{
			if ( data.Parts[i] == NATIVEID_NULL() )
				continue;

			const SITEM* pITEM = GLogicData::GetInstance().GetItem ( data.Parts[i] );
			if ( !pITEM )
				continue;

			m_pMeshVehicleData->SetPart( pITEM->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType), m_pd3dDevice);
		}

		for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
		{
			DWORD dwMainColor = 0, dwSubColor = 0;
			m_pMeshVehicleData->GetDefaultOverlayColor(i,dwMainColor,dwSubColor);
			if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				m_pMeshVehicleData->SetMainColor(i,dwMainColor);
				m_pMeshVehicleData->SetSubColor(i,dwSubColor);
			}
		}

		for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
		{
			if( data.ColorArray[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				data.ColorArray[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				m_pMeshVehicleData->SetMainColor(i, data.ColorArray[i].dwMainColor );
				m_pMeshVehicleData->SetSubColor(i, data.ColorArray[i].dwSubColor );
			}									
		}
	}



// 

//	SetAnimationStop();
	SetAnimationWalk();

	//////////////////////////////////////////////////////////////////////////

	SetVehicleMatrix();

	//////////////////////////////////////////////////////////////////////////
	

	return TRUE;
}

void CharacterView::SetCharacterMatrix()
{
	InitGeometry();

	m_pMeshCharacterData->SetWorldMatrix( m_matWorld );
	m_pMeshCharacterData->SetViewMatrix( m_matView, m_vCameraFromPt );
	m_pMeshCharacterData->SetProjMatrix( m_matProj );
}

void CharacterView::SetVehicleMatrix()
{
	InitGeometry();

	if( m_pMeshVehicleData )
	{
		m_pMeshVehicleData->SetWorldMatrix( m_matWorld );
		m_pMeshVehicleData->SetViewMatrix( m_matView, m_vCameraFromPt );
		m_pMeshVehicleData->SetProjMatrix( m_matProj );
	}
}


void CharacterView::SetUpdateVehicle(const VehicleData& data,bool isForce)
{
	if( isForce )
	{
		LoadVehicle(data);
		return;
	}

	m_UpdateBackUpVehicleData = data;
	if( m_BackUpVehicleData != m_UpdateBackUpVehicleData || !IsVisibleVehicle() )
		LoadVehicle(data);
}

void CharacterView::SetCamPosX( float value )
{
	m_CamPosX = value;
}

void CharacterView::SetCamPosY( float value )
{
	m_CamPosY = value;
}

void CharacterView::SetCamPosZ( float value )
{
	m_CamPosZ = value;
}

void CharacterView::SetPosition( float x,float y,float z )
{
	m_ObjectPosX = x;
	m_ObjectPosY = y;
	m_ObjectPosZ = z;
}

void CharacterView::SetRotationY( float rot )
{
	m_RotationY = rot;
}

void CharacterView::SetAnimationStop()
{
	SetAnimationHelper(AN_VEHICLE_STAY,AN_SUB_DRIVER_STOP,AN_PLACID);
}

void CharacterView::SetAnimationWalk()
{
	SetAnimationHelper(AN_VEHICLE_WALK,AN_SUB_DRIVER_WALK,AN_WALK);
}

void CharacterView::SetAnimationHelper( EMANI_MAINTYPE vehicleAni,EMANI_SUBTYPE bikeAni,EMANI_MAINTYPE boardAni)
{
	if( m_ForAniVehicleID == m_UpdateBackUpVehicleData.VehicleID && 
		m_VehicleAni == vehicleAni && m_BikeAni == bikeAni && m_BikeAni == boardAni )
		return;

	m_ForAniVehicleID = m_UpdateBackUpVehicleData.VehicleID;
	m_VehicleAni = vehicleAni;
	m_BikeAni = bikeAni;
	m_BoardAni = boardAni;

	if( m_pMeshVehicleData )
	{
		m_pMeshVehicleData->SelectAni( vehicleAni, AN_SUB_NONE );
		m_pMeshVehicleData->FrameMove( GetDevice(), 20.0f );
	}

	if( m_pMeshCharacterData )
	{
		if( GetVehicleType(m_BackUpVehicleData.VehicleID) != VEHICLE_TYPE_BOARD )
		{
			m_pMeshCharacterData->FrameMove( GetDevice(), 20.0f );

			m_emAniMainType = GetMainAniType(m_BackUpVehicleData.VehicleID);
			m_emAniSubType = bikeAni;
		}
		else // ( GetVehicleType(m_BackUpVehicleData.VehicleID) == VEHICLE_TYPE_BOARD )
		{
			m_pMeshCharacterData->FrameMove( GetDevice(), 20.0f );

			m_emAniMainType = boardAni;
			m_emAniSubType = AN_SUB_HOVERBOARD;
		}

	}
}

void CharacterView::Clear()
{
	//
	DeleteMeshData(MESH_ID_CHARACTER);
	m_pMeshCharacterData = NULL;

	DeleteMeshData(MESH_ID_VEHICLE);
	m_pMeshVehicleData = NULL;

	ResetItem(SLOT_VEHICLE);
}

void CharacterView::DeleteUIMeshData()
{
	Clear();
}

bool CharacterView::IsVisibleVehicle()
{
	const SNATIVEID& vehicleID = m_BackUpVehicleData.VehicleID;
	if( vehicleID == NATIVEID_NULL() )
		return false;

	if( GetVehicleType(vehicleID) == VEHICLE_TYPE_BOARD )
	{		
		if( m_pMeshCharacterData->IsSettingPiece(SLOT_2_PIECE(SLOT_VEHICLE)) )
			return true;
	}
	else
	{
		if( m_pMeshVehicleData )
			return true;
	}

	return false;
}

void CharacterView::SetPeaceModeData( ClientActor* pActor,GLGaeaClient* pGaeClient )
{
	m_pActor = pActor;
	m_pGaeClient = pGaeClient;
}

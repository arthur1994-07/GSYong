#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/Glogic.h"

#include "../Item/GLItemMan.h"
#include "../Item/GlItemLMT.h"
#include "../GlogicData.h"
#include "../GLogicDataMan.h"
#include "./GLVEHICLE.h"


#include "../../SigmaCore/DebugInclude.h"

SVEHICLEITEMINFO& SVEHICLEITEMINFO::operator= ( const SVEHICLEITEMINFO& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_nFull				= rvalue.m_nFull;
	m_dwBoosterID		= rvalue.m_dwBoosterID;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	return *this;
}

// constructor & operator
GLVEHICLE::GLVEHICLE() :
    m_emTYPE(VEHICLE_TYPE_NONE),
    m_DbNum(GAEAID_NULL),
    m_OwnerDbNum(0),
    m_nFull(0),
    m_sVehicleID(NATIVEID_NULL()),
    m_fSpeedRate( 0.0f ),
    m_fSpeedVol( 0.0f ),
    m_sSkinID ( NATIVEID_NULL() ),
    m_bActiveValue ( false ),
    m_pSkinChar( NULL ),
    m_dwBoosterID( 0 ),
    m_bUseBoost( FALSE ),
    m_bUseBoostDelay( FALSE ),
    m_fBoostDelay( 0.0f ),
    m_fBoostTime( 0.0f ),
    m_bOnceBoost( FALSE )
{
    for( INT i = 0 ; i < MAX_PASSENGER_COUNT; i++ )
    {
        m_dwPassengerID[ i ] = GAEAID_NULL;
    }
    m_VehicleData.ClearAll();
    m_CacheTickCount = GetTickCount();
}

GLVEHICLE::~GLVEHICLE()
{
}

GLVEHICLE::GLVEHICLE(const GLVEHICLE& value)
{
    operator=( value );
}

GLVEHICLE& GLVEHICLE::operator= ( const GLVEHICLE& rvalue )
{
	m_emTYPE = rvalue.m_emTYPE;
	m_DbNum = rvalue.m_DbNum;					// 탈것의 GUID
	m_OwnerDbNum = rvalue.m_OwnerDbNum;					// 주인의 charID
	m_sVehicleID = rvalue.m_sVehicleID;				// 소환을 요청한 탈것 아이템의 Mid/Sid
	m_nFull = rvalue.m_nFull;
	m_fSpeedVol = rvalue.m_fSpeedVol;					// 탈것의 속도 ( 파츠 부분을 합친값 )
	m_fSpeedRate = rvalue.m_fSpeedRate;					// 탈것의 속도 ( 파츠 부분을 합친값 )
	m_sSkinID = rvalue.m_sSkinID;
	m_dwBoosterID = rvalue.m_dwBoosterID;
	m_bUseBoost = rvalue.m_bUseBoost;
	m_bUseBoostDelay = rvalue.m_bUseBoostDelay;
	m_fBoostDelay = rvalue.m_fBoostDelay;
	m_fBoostTime = rvalue.m_fBoostTime;
	m_bOnceBoost = rvalue.m_bOnceBoost;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	for ( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		m_sColor[ i ].dwMainColor = rvalue.m_sColor[ i ].dwMainColor;
		m_sColor[ i ].dwSubColor = rvalue.m_sColor[ i ].dwSubColor;
	}

	for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		m_dwPassengerID[ i ] = rvalue.m_dwPassengerID[ i ];
	}
	
	ITEM_UPDATE();

    m_CacheTickCount = rvalue.m_CacheTickCount;

	return *this;
}

BOOL GLVEHICLE::GETPUTONITEMS_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) ACCE_TYPE_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return true;
}

BOOL GLVEHICLE::GETITEMSCOLOR_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SVEHICLE_COLOR::VERSION;
	ByteStream << (DWORD)sizeof(SVEHICLE_COLOR);

	DWORD dwSize = (DWORD) SVEHICLE_COLOR::COLOR_NUM;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_sColor[0], (DWORD)sizeof(SVEHICLE_COLOR)*dwSize );

	return true;
}

BOOL SETITEMSCOLOR_BYBUF ( SVEHICLE_COLOR *pVehiclePartsColor, se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )	return TRUE;

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SVEHICLE_COLOR sVehicleColor;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i >= SVEHICLE_COLOR::COLOR_NUM )	break;

		//	Note : 향후 버전에서 이전 버전을 대입 연산으로 처리 할 부분.
		//
		if ( dwVersion==SVEHICLE_COLOR::VERSION )
		{
			GASSERT(dwSize==sizeof(SVEHICLE_COLOR));
			ByteStream.ReadBuffer ( (LPBYTE)&sVehicleColor, sizeof(SVEHICLE_COLOR) );

			pVehiclePartsColor[i] = sVehicleColor;
		}
		else
		{
		}
	}

	return TRUE;
}

BOOL GLVEHICLE::GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItem ) const
{
	for( int i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		if ( m_PutOnItems[i].GetNativeID() != NATIVEID_NULL() )
		{
			SINVENITEM_SAVE sItem( m_PutOnItems[i], i, 0 );
			vecItem.push_back( sItem );
		}
	}
	return TRUE;
}

void GLVEHICLE::SETPUTONITEM( VEHICLE_ACCESSORYTYPE emSlot, const SITEMCUSTOM& sItem )
{
	if ( emSlot >= 0 && emSlot < ACCE_TYPE_SIZE )
	{
		m_PutOnItems[emSlot] = sItem;
	}
}

void GLVEHICLE::ASSIGN ( const GLVEHICLE& sVehicleData )
{
	m_emTYPE = sVehicleData.m_emTYPE;
	m_DbNum = sVehicleData.m_DbNum;				// 탈것의 GUID
	m_OwnerDbNum = sVehicleData.m_OwnerDbNum;			// 주인의 charID
	m_sVehicleID = sVehicleData.m_sVehicleID;		// 소환을 요청한 탈것 아이템의 Mid/Sid
	m_nFull = sVehicleData.m_nFull;
	m_fSpeedVol = sVehicleData.m_fSpeedVol;					// 탈것의 속도 ( 파츠 부분을 합친값 )
	m_fSpeedRate = sVehicleData.m_fSpeedRate;				// 탈것의 속도 ( 파츠 부분을 합친값 )
	m_sSkinID = sVehicleData.m_sSkinID;
	m_pSkinChar = sVehicleData.m_pSkinChar;
	m_dwBoosterID = sVehicleData.m_dwBoosterID;
	m_bUseBoost = sVehicleData.m_bUseBoost;
	m_bUseBoostDelay = sVehicleData.m_bUseBoostDelay;
	m_fBoostDelay = sVehicleData.m_fBoostDelay;
	m_fBoostTime = sVehicleData.m_fBoostTime;
	m_bOnceBoost = sVehicleData.m_bOnceBoost;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = sVehicleData.m_PutOnItems[i];
	}

	for ( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		m_sColor[ i ].dwMainColor = sVehicleData.m_sColor[ i ].dwMainColor;
		m_sColor[ i ].dwSubColor = sVehicleData.m_sColor[ i ].dwSubColor;
	}

	for ( WORD i = 0; i < MAX_PASSENGER_COUNT; ++i )
	{
		m_dwPassengerID[ i ] = sVehicleData.m_dwPassengerID [ i ];
	}

	ITEM_UPDATE();
}

void GLVEHICLE::RESET ()
{
	m_emTYPE = VEHICLE_TYPE_NONE;
	m_DbNum = GAEAID_NULL;
	m_OwnerDbNum = 0;
	m_nFull = 0;
	m_sVehicleID = NATIVEID_NULL();
	m_fSpeedRate = 0.0f;
	m_fSpeedVol = 0.0f;
	m_sSkinID = NATIVEID_NULL();
	m_bActiveValue = false;
	m_pSkinChar = NULL;
	m_VehicleData.ClearAll();
	m_dwBoosterID = 0;
	m_bUseBoost = FALSE;
	m_bUseBoostDelay = FALSE;
	m_fBoostDelay = 0.0f;
	m_fBoostTime = 0.0f;
	m_bOnceBoost = FALSE;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
	}

	for ( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		m_sColor[ i ].dwMainColor = D3DCOLOR_ARGB( 0, 0, 0, 0 );
		m_sColor[ i ].dwSubColor = D3DCOLOR_ARGB( 0, 0, 0, 0 );
	}

	for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		m_dwPassengerID[ i ] = GAEAID_NULL;
	}
}

bool GLVEHICLE::IncreaseFull ( const WORD volume, const BOOL bRatio)
{
	int nMaxFull = GLCONST_VEHICLE::pGLVEHICLE[m_emTYPE]->m_nFull;

	// 포만감이 가득찼으면
	if ( m_nFull >= nMaxFull ) return false;
	if ( bRatio )
	{
		m_nFull += ( nMaxFull*volume )/100;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}
	else
	{
		m_nFull += volume;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}

	return true;
}

INT GLVEHICLE::GetPassengerNum() const
{
	INT nNumber = 0;
	for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
	{
		if (m_dwPassengerID[i] == GAEAID_NULL)
			continue;		
		nNumber++;
	}
	return nNumber;
}

bool GLVEHICLE::GetAccessoryType( EMSUIT emSuit, VEHICLE_ACCESSORYTYPE& Type )
{
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE )
	{
		return false;
	}
	else
	{
		Type = static_cast< VEHICLE_ACCESSORYTYPE >( i );
		return true;
	}
}

SITEMCUSTOM GLVEHICLE::GetSlotitembySuittype ( EMSUIT emSuit )
{
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE )
        return SITEMCUSTOM();
    else
	    return m_PutOnItems[i];
}

bool GLVEHICLE::SetSlotItem( EMSUIT emType, SITEMCUSTOM sCustomItem)
{ 
	WORD i = ( WORD ) emType - ( WORD ) SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE )
		return false;
	m_PutOnItems[i] = sCustomItem;
	ITEM_UPDATE();
	return true;
}

bool GLVEHICLE::ReSetSlotItem( EMSUIT emType )
{
	WORD i = ( WORD ) emType - ( WORD ) SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE )
        return false;
	m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
	ITEM_UPDATE();
	return true;
}

SNATIVEID GLVEHICLE::PutOnItemId(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_PutOnItems[Type].GetNativeID();
}

void GLVEHICLE::PutOnItemIdReset(VEHICLE_ACCESSORYTYPE Type)
{
    m_PutOnItems[Type] = NATIVEID_NULL();
}

const SITEMCUSTOM& GLVEHICLE::PutOnItemRef(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_PutOnItems[Type];
}

SITEMCUSTOM GLVEHICLE::PutOnItem(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_PutOnItems[Type];
}

void GLVEHICLE::PutOnItemSet(VEHICLE_ACCESSORYTYPE Type, SNATIVEID ItemId)
{
    m_PutOnItems[Type] = SITEMCUSTOM( ItemId );
}

void GLVEHICLE::PutOnItemReset(VEHICLE_ACCESSORYTYPE Type)
{
    m_PutOnItems[Type] = SITEMCUSTOM();
}

float GLVEHICLE::PutOnItemMoveSpeedTotal() const
{
    float TotalSpeed = 0.0f;
    for (int i=0; i<ACCE_TYPE_SIZE; ++i)
        TotalSpeed += m_PutOnItems[i].GETMOVESPEED();
    return TotalSpeed;
}

DWORD GLVEHICLE::ColorMain(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_sColor[Type].dwMainColor;
}

DWORD GLVEHICLE::ColorSub(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_sColor[Type].dwSubColor;
}

SVEHICLE_COLOR GLVEHICLE::Color(VEHICLE_ACCESSORYTYPE Type) const
{
    return m_sColor[Type];
}

BOOL GLVEHICLE::CheckSlotItem(SNATIVEID sNativeID, EMSUIT emSUIT) const
{
	if (sNativeID == NATIVEID_NULL())
        return FALSE;

	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
	if (!pItemData)
        return FALSE;

	// 복장류 아이템이어야 한다.
	if ( pItemData->BasicType() != ITEM_SUIT )
        return FALSE;

	// 팻타입과 아이템의 팻타입이 일치해야 한다.
	if ( pItemData->sVehicle.emVehicleType != m_emTYPE )
        return FALSE;

	// 악세서리 슬롯이 일치해야 한다.
	if ( pItemData->sSuitOp.emSuit != emSUIT )
        return FALSE;

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem ( m_sVehicleID );
	if (!pVehicleItem)
        return FALSE;

	return TRUE;
}

/*
EMPIECECHAR GLVEHICLE::GetPieceFromSlot ( VEHICLE_ACCESSORYTYPE emType )
{

	if ( emType == ACCETYPESIZE ) return PIECE_NONE;
	if ( emType == ACCETYPEA ) return PIECE_HEAD;
	else					   return PIECE_LOBODY;
    
	return PIECE_NONE;
}
*/


// access
BOOL GLVEHICLE::LoadFile(const std::string& FileName, VEHICLE_TYPE emType)
{
	if (FileName.empty())
        return FALSE;
	
	std::string strPath(GLOGIC::GetPath());
	strPath.append(FileName);

	gltexfile cFILE;
	cFILE.reg_sep("\t ,{}[]()|");

	if (GLOGIC::UseLogicZipFile()) // by 경대
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
        sc::writeLogError(
            sc::string::format(
                "GLVEHICLE::LoadFile %1% %2%",
                GLogicData::GetInstance().GetGlogicZipFile(), strPath));		
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION, true );

	cFILE.getflag( "emTYPE",		1, 1, m_emTYPE, true );
	cFILE.getflag( "nFull",		1, 1, m_nFull, true );

	GLCONST_VEHICLE::pGLVEHICLE[emType] = this;

	// 포만도 감소치
	cFILE.getflag( "nFullDecrement", 1, 1, GLCONST_VEHICLE::nFullDecrVehicle[emType], true );
	
	return true;
}



void GLVEHICLE::ITEM_UPDATE()
{
    m_fSpeedRate = 0.0f;
	m_fSpeedVol = 0.0f;


	SITEM* pITEM;
//	SITEM* pITEM = GLogicData::GetInstance().GetItem ( m_sVehicleID );
//	if ( pITEM )
//	{
//		if ( pITEM->sSuitOp.sVARIATE.emTYPE == EMVAR_MOVE_SPEED )	m_fSpeedRate += pITEM->sSuitOp.sVARIATE.fVariate;
//		if ( pITEM->sSuitOp.sVOLUME.emTYPE == EMVAR_MOVE_SPEED )	m_fSpeedVol += pITEM->sSuitOp.sVOLUME.fVolume;		
//	}

	for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		if ( m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
            continue;
		pITEM = GLogicData::GetInstance().GetItem ( m_PutOnItems[i].GetNativeID() );
		if (!pITEM)
            continue;
		
		float fValue = 0.0f;

		/// 가산옵션이 동기화가 안되는것 같다, 수정 후 적용해야한다
		/*fValue = m_PutOnItems[i].sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_MOVE_SPEED);
		if (0.f < fValue)
		{
			m_fSpeedRate += fValue;
		}*/
		if ( pITEM->sSuitOp.GetHaveAddonValue_float(EMADDEX_INCR_MOVE_SPEED, fValue) == true )
		{
			m_fSpeedRate += fValue;
		}
		
		/*fValue = m_PutOnItems[i].sAddonInfo.GetValue_OptType<float>(EMADDEX_INC_MOVE_SPEED);
		if (0.f < fValue)
		{
			m_fSpeedVol += fValue;
		}*/
		fValue = 0.f;
		if ( pITEM->sSuitOp.GetHaveAddonValue_float(EMADDEX_INC_MOVE_SPEED, fValue) == true )
		{
			m_fSpeedVol += fValue;
		}      				
	}

	m_sSkinID = m_sVehicleID;
	
	// cps 미리 계산
	if ( m_PutOnItems[ACCE_TYPE_SKIN].GetNativeID() != NATIVEID_NULL() )
	{
		m_sSkinID = m_PutOnItems[ACCE_TYPE_SKIN].GetNativeID();
	}
}

void GLVEHICLE::BoostReset()
{
	m_bUseBoost = FALSE;
	m_fBoostTime = 0.0f;
	m_bUseBoostDelay = TRUE;
	m_fBoostDelay = 20.0f;
}

bool GLVEHICLE::IsValidSlot( EMSUIT emSuit )
{
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_VEHICLE_SKIN;
	if ( i >= ACCE_TYPE_SIZE )
	{
		return false;
	}
	else
	{
		return true;
	}
}

namespace COMMENT
{
	extern std::string VEHICLE_TYPE[VEHICLE_TYPE_SIZE] = { "호버", "오토바이","자동차","인력거" };
};
